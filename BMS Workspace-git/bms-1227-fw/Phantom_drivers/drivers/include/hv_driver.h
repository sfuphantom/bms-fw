/*
 * hv_driver.h
 *
 *  Created on: Oct. 20, 2021
 *      Author: Ryan H
 */

#ifndef HV_DRIVER_H_
#define HV_DRIVER_H_

/*============================================================================*/
/* Function Prototypes           */
/*============================================================================*/

// working UARTprintf implementation
void UARTprintf(const char *_format, ...);


void adcVoltageTest(uint16_t ADCValue);

// the SPI 'slave' in this case is the BMS Master which is receiving data from the HV board which is considered the master
void adcSlaveDataSetup();

// the SPI 'master' in this case is the HV board where the HV voltmeter is physically located
// the HV board gets a signal from that voltmeter through and ADC then sends that ADC value to the slave, the BMS board
void masterDataTransfer();

// the input uint16 is not actually an integer yet. it is the raw binary data read from SPI
// that raw data is prepended with two 0s and then 12 bits in two's complement binary representing the actual sent value
// this function returns that actual sent value as a float by stripping away all the leading zeros and making it not 2's complement
float getADCdata(uint16 input);

// the following four static functions are used by getADCdata to help transform the recieved SPI value into a usable float
static uint16 bitExtracted(uint16 number, uint8 k, uint8 p);
static void decToBinary(int n);;
static int binTwosComplementToSignedDecimal(uint8 binary[],uint8 significantBits);
static float Interpolate_input (int adc_input);


void unitTesting();

void testMIBSPI(uint16 testData);
void testSPI(uint16 testData);


#endif /* HV_DRIVER_H_ */
