/*
 * Current_transducer.h
 *
 *  Created on: Nov 2, 2021
 *      Author: Ryan Heo
 */

#include <stdbool.h>

#ifndef PHANTOM_DRIVERS_CURRENT_TRANSDUCER_CURRENT_TRANSDUCER_H_
#define PHANTOM_DRIVERS_CURRENT_TRANSDUCER_CURRENT_TRANSDUCER_H_

#define MAX_POS_CURRENT 200
#define MIN_NEGATIVE_CURRENT -200
#define I_PN 200    // primary nominal current
#define Uref 2.5    // reference voltage halfway between zero and max_voltage, current transducer outputs this at 0A

#define true 1
#define false 0

// global variables
static int overCurrentFlag; // fault indicator for above +200A current level
static int underCurrentFlag; // fault indicator for below -200A current level
static unsigned int max_voltage = 5; // ADC high reference voltage
static unsigned int adc_resolution = 4096; // 12 bits (2^12) resolution for ADCs on TMS570LS1227 / 1224
static double sensor_voltage; // converted adc output that represents the voltage read from current transducer output pin
static double output_current; // in Amps
static unsigned int adcOutputValue; // number output by adcGetData from 0 to 4096, will be converted into voltage of current transducer output pin

// methods
double getHVsensorVoltage();
double getHVsensorCurrent();
_Bool overHVCurrentFaultCheck(); // Fault check method for current above +200A
_Bool underHVCurrentFaultCheck(); // Fault check method for current below -200A


#endif /* PHANTOM_DRIVERS_CURRENT_TRANSDUCER_CURRENT_TRANSDUCER_H_ */
