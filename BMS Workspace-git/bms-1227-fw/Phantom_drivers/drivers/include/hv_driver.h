/*  @file       hv_driver.h
 *  @brief      Battery Voltage Reading ADC Driver
 *  @date       08-May-2024
 *  @version    2.0
 */

#ifndef HV_DRIVER_H_
#define HV_DRIVER_H_

#include "hal_stdtypes.h" // necessary for uint16 definition

    /* Function Prototypes */
static uint16 bitExtracted(uint16 number, uint8 k, uint8 p);
static void decToBinary(int n);;
static int binTwosComplementToSignedDecimal(uint8 binary[],uint8 significantBits);
static float Interpolate_input (int adc_input);
void UARTprintf(const char *_format, ...);
void simulateVoltageHVADC(uint16 testValue);
float getADCdata(uint16 input);
float getBatteryVoltageHV();


#endif /* HV_DRIVER_H_ */
