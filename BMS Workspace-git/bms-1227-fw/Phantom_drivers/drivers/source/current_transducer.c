/*
 * Current_transducer.c
 *
 *  Created on: Nov 2, 2021
 *      Author: Ryan Heo
 */

#include <current_transducer.h>
#include "adc.h"
#include "stdlib.h"

double getHVsensorVoltage()
{
    adcData_t adc_data;   // ADC data structure
    adcData_t *adc_data_ptr = &adc_data;  // ADC data pointer
     // declare variables

    adcInit();   // Initialize the ADC module

    // Move the data from the ADC module 1, group 1 to somewhere it can be accessed by adcGetData then stored in a variable (value)
    adcStartConversion(adcREG1, 1U);

    while(!adcIsConversionComplete(adcREG1, 1U)); //Wait for ADC conversion
    adcGetData(adcREG1, 1U, adc_data_ptr); //Store conversion into ADC pointer
    adcOutputValue = (unsigned int)adc_data_ptr->value;

    sensor_voltage = (double) adcOutputValue/adc_resolution*max_voltage;

    return sensor_voltage;
}

double getHVsensorCurrent()
{
    // if(voltage < Uref){     // negative current
    // if(voltage > Uref){     // positive current
    output_current  = (sensor_voltage - Uref)*I_PN/1.25;

    return output_current;
}

// Fault check method for current above +200A
_Bool overHVCurrentFaultCheck()
{
    overCurrentFlag = false;

    if(output_current >=  MAX_POS_CURRENT)
    {
        overCurrentFlag =  true;
    }

    return overCurrentFlag;
}

// Fault check method for below above -200A
_Bool underHVCurrentFaultCheck()
{
    underCurrentFlag = false;

    if(output_current <= MIN_NEGATIVE_CURRENT)
    {
        underCurrentFlag =  true;
    }

    return underCurrentFlag;
}
