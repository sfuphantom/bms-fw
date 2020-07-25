/*
 * thermistor.c
 *
 *  Created on: Jan 9, 2020
 *      Author: JASKANWAL SINGH
 */

#include "thermistor.h"



/************************************************************************************************************************************************/
//Setting up mibSPI communication with the ADC
/************************************************************************************************************************************************/

#define TransferGroup0           0x0
#define TransferGroup1           0x1
#define Channels                 12

static volatile int isTxComplete;
volatile int adcConfigured;
static volatile int ReceivedData;
static volatile int Processed;
static volatile bool messageRequest = false;

static uint16 TG0_dummydata[11];

static uint16 rxData_Buffer[12];

static uint16 adc_configuration[11] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3C00, 0x3000, 0x9300};

static uint16 adc_mode[12]={0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000};

static volatile int currentIndex;
static thermistorStruct thStruct = {{0}, THERMISTOR_STARTUP, 0};



void setupThermistor()        //prepare the thermistor to start reading
{
        /*
         * Configuring ADS7952.
         */

        mibspiSetData(mibspiREG3, TransferGroup0, adc_configuration);
        mibspiEnableGroupNotification(mibspiREG3, TransferGroup0, 0);
        mibspiTransfer(mibspiREG3, TransferGroup0);
        adcConfigured = 0;
        while(!adcConfigured);

        thStruct.State = THERMISTOR_GOOD;

}

void thermistorRead()
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    //UARTprintf("current time = %d\n\r", xLastWakeTime);
    uint8 tempFaults = 0;

    mibspiSetData(mibspiREG3, TransferGroup1, adc_mode);
    mibspiEnableGroupNotification(mibspiREG3, TransferGroup1, 0);
    ReceivedData = 0;
    mibspiTransfer(mibspiREG3, TransferGroup1);
    while(!ReceivedData);

    if(adcConfigured && ReceivedData){

        uint8 input = 0;
        for(; input < 8; input++)
        {
//               update_input(input);
           update_input(input);

           mibspiSetData(mibspiREG3, TransferGroup1, adc_mode);
           mibspiEnableGroupNotification(mibspiREG3, TransferGroup1, 0);
           ReceivedData = 0;
           mibspiTransfer(mibspiREG3, TransferGroup1);
           while(!ReceivedData);

           parseThermistorRXBuffer();
           tempFaults += updateTemperature(input);

        }
        xLastWakeTime = xTaskGetTickCount() - xLastWakeTime ;
        //UARTprintf("current time = %d\n\r", xLastWakeTime);
    }

    thStruct.totalFaults = tempFaults;

    processThermistorState();
}

void thermistorReadPrint()
{

    uint8 tempFaults = 0;

    mibspiSetData(mibspiREG3, TransferGroup1, adc_mode);
    mibspiEnableGroupNotification(mibspiREG3, TransferGroup1, 0);
    ReceivedData = 0;
    mibspiTransfer(mibspiREG3, TransferGroup1);
    while(!ReceivedData);

    if(adcConfigured && ReceivedData){

        uint8 input = 0;
        for(; input < 8; input++)
        {
//               update_input(input);
           update_input(input);

           parseThermistorRXBuffer();
           tempFaults += updateTemperature(input);
           printThermistorReadings(input);

           mibspiSetData(mibspiREG3, TransferGroup1, adc_mode);
           mibspiEnableGroupNotification(mibspiREG3, TransferGroup1, 0);
           ReceivedData = 0;
           mibspiTransfer(mibspiREG3, TransferGroup1);
           while(!ReceivedData);

        }
    }

    thStruct.totalFaults = tempFaults;

    processThermistorState();
}

/************************************************************************************************************************************************/
/*Validating usage status */
/************************************************************************************************************************************************/
// TODO: is this needed?
//uint8 measuring_charge_thermistor =   0;
//uint8 measuring_run_thermistor =   0;
//
//uint8_t validate_usage_status_thermistor(uint8_t status )      //Inquires whether the car is charging or running while the thermistor is measuring?
//{
//   (status == 0)   ?   (measuring_charge_thermistor = 1)   :   (measuring_run_thermistor = 1);
//   return status;
//}


/************************************************************************************************************************************************/
/*Reading, Extracting and Printing Thermistor values*/
/************************************************************************************************************************************************/


void    parseThermistorRXBuffer()
{
    uint8 channel=0;
    for (;  channel<12;   channel++)
    {
        rxData_Buffer[channel]    &=  0x0FFF;  //Masking to remove channel address from the data received from the ADC and
                                               //keeping only the Hex format of thermistor readings
    }
}

void    printThermistorReadings(uint8 input)
{
    sciInit();

    uint16 value, location = input*12, channel = 0;
    float voltage;

    for(;   channel<12;   channel++, location++)
    {
        value   =   (uint16)rxData_Buffer[channel];
        voltage =   (((float)value)/4095)*REFERENCE_VOLTAGE;

        UARTprintf("INPUT : CHANNEL : VOLTAGE : TEMP\t||\t%d : %d\t: %f : %d\n\r", input, channel+1, (float)voltage, thStruct.cellTemp[location]);
        //sciSend(scilinREG, 90,(uint8 *)buff );

    }
    //char* buff[25];
    UARTprintf("\n\r");
    //sciSend(scilinREG, 25,(uint8 *)buff );

}

/************************************************************************************************************************************************/
/*Updating the Temperature logging sructure*/
/************************************************************************************************************************************************/

uint8 updateTemperature(uint8 input)      //logging and updating flag functions can be merged
{
    uint8 location = input*12;
    uint8 channel = 0;
    uint8 tempFaults = 0;
    for (; channel < 12; channel++, location++)
    {
        //logging Temperature
        thStruct.cellTemp[location] = DoCalculation( (((float)rxData_Buffer[channel])/4095)*REFERENCE_VOLTAGE);
        //Updating Flag
        if((thStruct.cellTemp[location] >= 60) || (thStruct.cellTemp[location] <= 5))
        {
            tempFaults++;
        }
        else
        {

        }

    }

    return tempFaults;
}

void processThermistorState()
{
    // TODO: Check faults to determine thermistor state
    if(thStruct.totalFaults > 3)
    {
        thStruct.State = THERMISTOR_CRITICAL_FAULT;
        return;
    }
    else
    if(thStruct.totalFaults != 0)
    {
        thStruct.State = THERMISTOR_TEMPERATURE_FAULT;
        return;
    }
    else
    {
        thStruct.State = THERMISTOR_GOOD;
        return;
    }
}

bool pollThermistorState(void)
{
    return thStruct.State;
}

void update_input(uint8 current_input)      //can have a structure array and quickly approach the element the GIO config depending on the current mux, instead of scanning
{
    if (current_input == 0)
    {
            gioSetBit(hetPORT1, 4, 0);
            gioSetBit(hetPORT1, 5, 0);
            gioSetBit(hetPORT1, 7, 0);
    }
    else if (current_input == 1)
    {
            gioSetBit(hetPORT1, 4, 1);
            gioSetBit(hetPORT1, 5, 0);
            gioSetBit(hetPORT1, 7, 0);
    }
    else if (current_input == 2)
        {
            gioSetBit(hetPORT1, 4, 0);
            gioSetBit(hetPORT1, 5, 1);
            gioSetBit(hetPORT1, 7, 0);
        }
    else if (current_input == 3)
        {
            gioSetBit(hetPORT1, 4, 1);
            gioSetBit(hetPORT1, 5, 1);
            gioSetBit(hetPORT1, 7, 0);
        }
    else if (current_input == 4)
        {
            gioSetBit(hetPORT1, 4, 0);
            gioSetBit(hetPORT1, 5, 0);
            gioSetBit(hetPORT1, 7, 1);
        }
    else if (current_input == 5)
        {
            gioSetBit(hetPORT1, 4, 1);
            gioSetBit(hetPORT1, 5, 0);
            gioSetBit(hetPORT1, 7, 1);
        }
    else if (current_input == 6)
        {
            gioSetBit(hetPORT1, 4, 0);
            gioSetBit(hetPORT1, 5, 1);
            gioSetBit(hetPORT1, 7, 1);
        }
    else if (current_input == 7)
        {
            gioSetBit(hetPORT1, 4, 1);
            gioSetBit(hetPORT1, 5, 1);
            gioSetBit(hetPORT1, 7, 1);
        }
    }

void getCellTemperatureArray(uint8* tempArray)
{
    memcpy(&tempArray, &thStruct.cellTemp[0], TOTAL_MUXES);
}

void mibspiGroupNotification(mibspiBASE_t *mibspi, uint32 group)
{
    mibspiDisableGroupNotification(mibspiREG3, TransferGroup0);
    mibspiDisableGroupNotification(mibspiREG3, TransferGroup1);

    if (mibspi == mibspiREG3 && group == TransferGroup0) {
                mibspiGetData(mibspi, group, TG0_dummydata);
                mibspiDisableGroupNotification(mibspiREG3, TransferGroup0);
                adcConfigured = 1;

    }

    if (mibspi == mibspiREG3 && group == TransferGroup1 && adcConfigured == 1) {


            mibspiGetData(mibspi, group, rxData_Buffer);

              ReceivedData = 1;

        }
}
