/*  @file       hv_driver.c
 *  @brief      Battery Voltage Reading ADC Driver
 *  @date       08-May-2024
 *  @version    2.0
 *
 *  ADS7044 is the name of the Analog to Digital converter on the High Voltage board
 */

    /* Includes */
#include "sys_common.h"
#include <hv_driver.h>
#include <math.h>
#include <gio.h>
#include "sci.h"
#include "string.h"
#include "stdio.h"
#include "mibspi.h"

    /* Definitions */
#define TransferGroup0 0            // not currently in use
#define TransferGroup1 1            // mibspi transfer group 1 used by mibspi1 to get voltage data, and mibspi3 to simulate sending voltage data
#define MAX_DATA_BITS  (12u)        // voltage data being sent is in 12 bits

// Definitions for the linear interpolation function for mapping HV battery voltage
// TODO: Tune and test these values such that they are correct with 96S li-ion battery
#define SLOPE          (0.01173261f)
#define Y_INTERCEPT    (144.285f)
#define START_INDEX    (1u) // what index to start at when extracting 12 data bits from SPI transfer from ADS7044

// battery is 96 cells in series, so add the voltages of 96 cells together to get the battery voltage
#define MAX_VOLTAGE_HV  403.2 // 96 (cells in series) * 4.2 (max voltage for a lithium ion cell) = 403.2 V
#define MIN_VOLTAGE_HV  297.6 // 96 (cells in series) * 3.1 (our minimum cell voltage according to Michael) = 297.6 V

    /* Global Variables */
static uint16 TX_BMS_Master[1]   = {0};     // we only need RX_BMS_Master since we don't send anything to the ADS7044; the BMS master does not need to transmit data
static uint16 RX_BMS_Master[1]   = {0};

static uint16 TX_ADS7044_Slave[1] = {0};    // we only need TX_ADS7044_Slave since we don't send receive anything using the ADS7044; it doesn't even have a MOSI pin
static uint16 RX_ADS7044_Slave[1] = {0};

float Battery_Voltage_HV = 0.0; // global variable for HV main battery voltage. updates every time sensor read task runs
static uint8 binaryNum[12];     // binaryNum is used in the conversion from ADC number to voltage value

static bool underVoltageFlag = 0;   // flag for when measured voltage greater than 4.2*96 = 403.2 V
static bool overVoltageFlag = 0;    // flag for when measured voltage less than 3.1*96 = 297.6 V


/** @fn float getBatteryVoltageHV()
*   @brief Uses mibspi to retrieve battery voltage data
*
*   mibspi1 module on the BMS master is used to get the number output by the HV ADC (ADS7044)
*   getADCdata function converts that number to the actual voltage value
*
*/
float getBatteryVoltageHV(){

    mibspiSetData(mibspiREG1, TransferGroup1, TX_BMS_Master);       // not important what is sent to ADS7044 since it cannot recieve data, only send
    mibspiEnableGroupNotification(mibspiREG1, TransferGroup1, 0);   // enable transfer group finished interrupt for mibspi module 1, transfer group 1
    mibspiTransfer(mibspiREG1, TransferGroup1);

    Battery_Voltage_HV =  getADCdata(RX_BMS_Master[0]);     // convert ADC data to voltage value
    return Battery_Voltage_HV;
}

/** @fn void mibspiGroupNotification(mibspiBASE_t *mibspi, uint32 group)
*   @brief ISR callback function called when transfer group finished interrupt triggered
*   @param[in] mibspi   - mibspi module base pointer (e.g. mibspiREG1, mibspiREG3)
*   @param[in] group    - Transfer group (0 to 7)
*
*   this function is only called when mibspiEnableGroupNotification(mibspiBASE_t *mibspi, uint32 group, uint32 level) is called with the same group and mibspi module first
*   then when that transfer group and module combination finish a mibspiTransfer(mibspiBASE_t *mibspi, uint32 group) call,
*   the transfer group finished interupt is triggered and this ISR is called
*
*/
// TODO: put this in its own file when it's being used by multiple things (e.g. slaves and voltage)
void mibspiGroupNotification(mibspiBASE_t *mibspi, uint32 group)
{
    UARTprintf("mibspiGroupNotification hit\n");
    // BMS master receive from ADS7044 case
    if (mibspi == mibspiREG1 && group == TransferGroup1)
    {
        mibspiDisableGroupNotification(mibspiREG1, TransferGroup1); // disable transfer group finished interrupt
        mibspiGetData(mibspi, group, RX_BMS_Master);                // take data from mibspi1 rx buffer and put it in RX_BMS_Master
    }

    // transmit to ADS7044 case, not relevant since it cannot recieve data
    if (mibspi == mibspiREG3 && group == TransferGroup1)
    {
        mibspiDisableGroupNotification(mibspiREG3, TransferGroup1);
        mibspiGetData(mibspi, group, RX_ADS7044_Slave);
    }


    // transfer group 0 is currently not in use for mibspi1 or mibspi3
    if (mibspi == mibspiREG3 && group == TransferGroup0)
    {
        mibspiDisableGroupNotification(mibspiREG3, TransferGroup0);
    }

    if (mibspi == mibspiREG1 && group == TransferGroup0)
     {
         mibspiDisableGroupNotification(mibspiREG1, TransferGroup0);
     }
}

/** @fn void simulateVoltageHVADC(uint16 testValue)
*   @brief Uses mibspi to simulate sending battery voltage data
*
*   mibspi3 module is used to send a simulated HV ADC value using transfer group 0 on mibspi3
*   mibspi1 module receives the data using getBatteryVoltageHV()
*   wires must be connected between mibspi1 and mibspi3 on the launchpad for this test to work
*
*/
void simulateVoltageHVADC(uint16 testValue)
{
    TX_ADS7044_Slave[0] = testValue; // voltage data to be sent/tested

    mibspiSetData(mibspiREG3, TransferGroup1, TX_ADS7044_Slave);
    mibspiEnableGroupNotification(mibspiREG3, TransferGroup1, 0);
    mibspiTransfer(mibspiREG3, TransferGroup1);

}

bool overVoltageFaultCheckHV()
{
    overVoltageFlag = false;

    if(Battery_Voltage_HV > MAX_VOLTAGE_HV)
    {
        overVoltageFlag =  true;
    }

    return overVoltageFlag;
}

bool underVoltageFaultCheckHV()
{
    underVoltageFlag = false;

    if(Battery_Voltage_HV < MIN_VOLTAGE_HV)
    {
        underVoltageFlag =  true;
    }

    return underVoltageFlag;
}

/*
 * The functions below are for the purpose of transforming the raw ADC data sent by the ADS7044
 * 14 bits are sent to the BMS master, the first 2 are zeros and the last 12 are the real data
 * those 12 bits are in twos complement binary
 * a linear scaling function is applied to transform the ADC data into battery voltage
 */
static uint16 bitExtracted(uint16 number, uint8 k, uint8 p)
{
    return (((1 << k) - 1) & (number >> (p - 1)));
}

static void decToBinary(int n)
{
    int  idx = 0;
    // Size of an integer is assumed to be 12 bits
    for (idx = (MAX_DATA_BITS -1) ; idx >= 0; idx--)
    {
        int k = n >> idx;
        if (k & 1)
        {
            binaryNum[idx] = 1;
        }
        else
        {
            binaryNum[idx] = 0;
        }
    }
}

static float Interpolate_input (int adc_input) //change to uint_8
{
    float output_voltage;
    output_voltage = ((float)adc_input * SLOPE) + Y_INTERCEPT;

    return output_voltage;

}

static int binTwosComplementToSignedDecimal(uint8 binary[],uint8 significantBits)
{
    int power = pow(2,significantBits-1);
    int sum = 0;
    int i;

    for (i = significantBits-1; i >= 0; i--)
    {
        if ( i == (MAX_DATA_BITS -1) && binary[i]!= 0)
        {
            sum = power * -1;
        }
        else
        {
            sum += (binary[i])*power;
        }
        power /= 2;
    }

    return sum;
}



float getADCdata(uint16 input)
{
    int conversion_dec;
    float otp;

    uint16 input_12bits = bitExtracted(input, MAX_DATA_BITS, START_INDEX ); // extracting 12 bits of data (the first 2 bits are 0)
    decToBinary(input_12bits);
    conversion_dec = binTwosComplementToSignedDecimal(binaryNum , MAX_DATA_BITS);
    otp = Interpolate_input(conversion_dec);
//    UARTprintf("otp value: \n\r");
//    unsigned int adc_input = (int) (otp);
//    int i,j;
//    for (i = 2; i >= 0; i--)
//    {
//        uint8 bit = (int) (adc_input % 10);
//        for (j = 3; j >= 0; j--)
//        {
//            uint8 t = (int) ((bit >> j) & 1);
//            if (t == 0)
//            {
//                UARTprintf("0");
//            }
//            sciSend(scilinREG, 1, &t);
//        }
//        if (i == 2)
//        {
//            UARTprintf(" ");
//            adc_input = (int) (adc_input/10);
//        }
//        if (i == 1)
//        {
//            UARTprintf(" ");
//            adc_input = (int) (adc_input/10);
//        }
//    }
//    UARTprintf("\n\r");
    return otp;
}

void printBinaryInt(int integer){

}
