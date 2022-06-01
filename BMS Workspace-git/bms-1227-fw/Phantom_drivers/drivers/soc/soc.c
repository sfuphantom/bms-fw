/*
 * soc.c
 *
 *  Created on: Jul 20, 2020
 *      Author: Mahmoud Ahmed
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "soc.h"
#include "bms_data.h"
#include "phantom_pl455.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"


static const float MAX_CAPACITY = 7560;   // 2.1Ah = 7560 Coloumbs
static const float  MAX_LEVEL = 100;
static const float LOW_DISCHARGE_CURRENT_THRESH = 2; // A TODO: Verify this by testing battery and seeing what the idle current is
static const float BLOWN_FUSE_RESISTANCE_THRESH = 1000000000; // Ohms

float battLevel = 0; // TODO: Make static after unit-testing
float battCapacity = 0; // Max chemical capacity for cell TODO: Make static after unit-testing
static TickType_t xLastWakeTime = 0;
float lastCurrentValue = 0; // TODO: Make static after unit-testing

SOCState_t SOCState = INIT; // TODO: Make static after unit-testing 
bms_data* BMSDataPtr; // TODO: Make this extern and initialize BMSDataPtr when BMS is initialized

/**
 * Main access function for SOC
 * Maintains SOC state machine
 */
void socProcess(void)
{
    float current = getInstantaneousCurrent();

    if (SOCState == INIT)
    {
        if (1) // TODO: Change this to "if BMS slaves have been initialized"
        {
            socInit();

            if (current < 0) // TODO: Make sure current is recorded as negative when it's charging
            {
                SOCState = CHARGE;
            }
            else if (current < LOW_DISCHARGE_CURRENT_THRESH)
            {
                SOCState = LOW_DISCHARGE;
            }
            else
            {
                SOCState = DISCHARGE;
            }
        }
        return;
    }
    else
    {
        if (current < 0) // TODO: Make sure current is recorded as negative when it's charging
        {
            SOCState = CHARGE;
            updateBattCapacity();
        }
        else if (current < LOW_DISCHARGE_CURRENT_THRESH)
        {
            SOCState = LOW_DISCHARGE;
            updateBattCapacity();
            calculateRemainingRunTime(getBattLevel(), current);
        }
        else
        {
            SOCState = DISCHARGE;
            updateSOC(xTaskGetTickCount(), current);
            calculateRemainingRunTime(getBattLevel(), current);
            calculateImpedance(getOCV(getSOC(), getInstantaneousTemp()), getInstantaneousVoltage(), current);
        }
        return;
    }
}

/**
 * Initialize SOC driver
 */
void socInit(void)
{
    BMSDataPtr->Data.SOC = 55.00;
    battLevel = BMSDataPtr->Data.SOC*battCapacity/100;
    battCapacity = MAX_CAPACITY; // Is the order of this correct?
    xLastWakeTime = xTaskGetTickCount();
    lastCurrentValue = getInstantaneousCurrent();
}

/** 
 * Updates maximum chemical capacity for cell using SOC measurements.
 * Only performs update if cell is in a low discharge period 
 * (low discharge period is characterized by dV = 4uV/s or less),
 * or if cell is charging (current and temperature more likely to 
 * be constant)
 */
/* TODO: Test the accuracy of this wrt how much time needs to pass between SOC readings
            From unit-test: function is accurate if battery capacity is updated after 83 min?*/
void updateBattCapacity()
{
    TickType_t xLastWakeTime_store = xLastWakeTime;
    TickType_t xWakeTime = xTaskGetTickCount();

    // Get first data set from last time updateSOC() was called
    float current1 = lastCurrentValue;
    float SOC1 = getSOC();

    // Update SOC and get second data set
    updateSOC(xWakeTime, getInstantaneousCurrent());
    float current2 = lastCurrentValue;
    float SOC2 = getSOC();

    // Update battery capacity
    float chargePassed = (current1 + current2)/2*(xWakeTime-xLastWakeTime_store)/1000;
    battCapacity = chargePassed/(fabs(SOC1 - SOC2)/100);
}

/**
 * Updates state of charge estimation based on current time.
 *
 * @param xWakeTime     current time
 * @param current       instantaneous current of battery
 */
void updateSOC(TickType_t xWakeTime, float current)
{
    //UARTprintf("current time = %d\n\r", xLastWakeTime);
    float chargePassed = current*portTICK_PERIOD_MS*(xWakeTime - xLastWakeTime)/1000; // in Coulombs
    //printf("Current = %f, xLastWakeTime = %f, chargePassed = %f\n", current, xLastWakeTime, chargePassed);
    
    if(SOCState == CHARGE)
    {
        battLevel = battLevel + chargePassed;
    }
    else
    {
        battLevel = battLevel - chargePassed;
    }

    BMSDataPtr->Data.SOC = battLevel/battCapacity*100;

    if(battLevel >= 0 && battLevel <= 100)
    {
        //UARTprintf("battery capacity = %f | battery level = %f | current = %f \n\r ", battCapacity, battLevel, current);
    }
    else
    {
       // UARTprintf("battery empty");
    }

    xLastWakeTime = xWakeTime;
    lastCurrentValue = current;
}

/**
 * Returns state of charge
 */
float getSOC(void)
{
    return BMSDataPtr->Data.SOC;
}

/**
 * Obtains SOC of battery from battery voltage. This function only to 
 * be used during a low discharge state (ex: for initialization)
 *
 * @param voltage       instantaneous voltage of battery
 * @param temp          instantaneous temperature of battery
 * @param estimatedSOC  SOC estimate 
 */
float getSOCFromVoltage(float voltage, float temp, float estimatedSOC)
{
    // Current approach: binary search from getOCV() (since it's
    // a non-linear function)
    float eps = 0.05; 
    float testOCV = getOCV(estimatedSOC, temp);
    float test_val = testOCV - voltage;
    if (fabs(test_val) < eps)
    {
        return estimatedSOC;
    }
    else if (test_val > eps)
    {
        return getSOCFromVoltage(voltage, temp, estimatedSOC/2);
    }
    else // testOCV < OCV
    {
        return getSOCFromVoltage(voltage, temp, (100 + estimatedSOC)/2);
    }
}

/**
 * Obtains open circuit voltage of battery using polynomial model.
 *
 * @param SOC       state of charge (ex: 55.5% SOC represented as 55.5)
 * @param temp      instantaneous temperature of battery
 */
float getOCV(float SOC, float temp)
{
    float OCV;
    float p0;
    float p1;
    float p2;
    float alpha1;
    float alpha2;

    switch(SOCState)
    {
        case CHARGE:
            if (temp <= 5)
            {
                p0 = p0_Charge5;
                p1 = p1_Charge5;
                p2 = p2_Charge5;
                alpha1 = alpha1_Charge5;
                alpha2 = alpha2_Charge5;
            }
            else if (temp <= 15)
            {
                p0 = p0_Charge5+(p0_Charge15-p0_Charge5)/(15-5)*(temp-5);
                p1 = p1_Charge5+(p1_Charge15-p1_Charge5)/(15-5)*(temp-5);
                p2 = p2_Charge5+(p2_Charge15-p2_Charge5)/(15-5)*(temp-5);
                alpha1 = alpha1_Charge5+(alpha1_Charge15-alpha1_Charge5)/(15-5)*(temp-5);
                alpha2 = alpha2_Charge5+(alpha2_Charge15-alpha2_Charge5)/(15-5)*(temp-5);

            }
            else if (temp <= 25)
            {
                p0 = p0_Charge15+(p0_Charge25-p0_Charge15)/(25-15)*(temp-15);
                p1 = p1_Charge15+(p1_Charge25-p1_Charge15)/(25-15)*(temp-15);
                p2 = p2_Charge15+(p2_Charge25-p2_Charge15)/(25-15)*(temp-15);
                alpha1 = alpha1_Charge15+(alpha1_Charge25-alpha1_Charge15)/(25-15)*(temp-15);
                alpha2 = alpha2_Charge15+(alpha2_Charge25-alpha2_Charge15)/(25-15)*(temp-15);
            }
            else if (temp < 45)
            {
                p0 = p0_Charge25+(p0_Charge45-p0_Charge25)/(45-25)*(temp-25);
                p1 = p1_Charge25+(p1_Charge45-p1_Charge25)/(45-25)*(temp-25);
                p2 = p2_Charge25+(p2_Charge45-p2_Charge25)/(45-25)*(temp-25);
                alpha1 = alpha1_Charge25+(alpha1_Charge45-alpha1_Charge25)/(45-25)*(temp-25);
                alpha2 = alpha2_Charge25+(alpha2_Charge45-alpha2_Charge25)/(45-25)*(temp-25);
            }
            else // temp >= 45
            {
                p0 = p0_Charge45;
                p1 = p1_Charge45;
                p2 = p2_Charge45;
                alpha1 = alpha1_Charge45;
                alpha2 = alpha2_Charge45;
            }
            break;

        case DISCHARGE:
            if (temp <= 5)
            {
                p0 = p0_Discharge5;
                p1 = p1_Discharge5;
                p2 = p2_Discharge5;
                alpha1 = alpha1_Discharge5;
                alpha2 = alpha2_Discharge5;
            }
            else if (temp <= 15)
            {
                p0 = p0_Discharge5+(p0_Discharge15-p0_Discharge5)/(15-5)*(temp-5);
                p1 = p1_Discharge5+(p1_Discharge15-p1_Discharge5)/(15-5)*(temp-5);
                p2 = p2_Discharge5+(p2_Discharge15-p2_Discharge5)/(15-5)*(temp-5);
                alpha1 = alpha1_Discharge5+(alpha1_Discharge15-alpha1_Discharge5)/(15-5)*(temp-5);
                alpha2 = alpha2_Discharge5+(alpha2_Discharge15-alpha2_Discharge5)/(15-5)*(temp-5);

            }
            else if (temp <= 25)
            {
                p0 = p0_Discharge15+(p0_Discharge25-p0_Discharge15)/(25-15)*(temp-15);
                p1 = p1_Discharge15+(p1_Discharge25-p1_Discharge15)/(25-15)*(temp-15);
                p2 = p2_Discharge15+(p2_Discharge25-p2_Discharge15)/(25-15)*(temp-15);
                alpha1 = alpha1_Discharge15+(alpha1_Discharge25-alpha1_Discharge15)/(25-15)*(temp-15);
                alpha2 = alpha2_Discharge15+(alpha2_Discharge25-alpha2_Discharge15)/(25-15)*(temp-15);
            }
            else if (temp < 45)
            {
                p0 = p0_Discharge25+(p0_Discharge45-p0_Discharge25)/(45-25)*(temp-25);
                p1 = p1_Discharge25+(p1_Discharge45-p1_Discharge25)/(45-25)*(temp-25);
                p2 = p2_Discharge25+(p2_Discharge45-p2_Discharge25)/(45-25)*(temp-25);
                alpha1 = alpha1_Discharge25+(alpha1_Discharge45-alpha1_Discharge25)/(45-25)*(temp-25);
                alpha2 = alpha2_Discharge25+(alpha2_Discharge45-alpha2_Discharge25)/(45-25)*(temp-25);
            }
            else // temp >= 45
            {
                p0 = p0_Discharge45;
                p1 = p1_Discharge45;
                p2 = p2_Discharge45;
                alpha1 = alpha1_Discharge45;
                alpha2 = alpha2_Discharge45;
            }
            break;
    }
    
    OCV = p0*exp(alpha1*SOC)+p1*exp(alpha2*SOC)+p2*pow(SOC,2);
    return OCV;
}

/**
 * Returns charge remaining in battery, represented in Coulombs 
 */
float getBattLevel(void)
{
    return battLevel;
}

/**
 * Returns maximum chemical capacity for cell, represented in Coulombs
 */
float getBattCapacity(void)
{
    return battCapacity;
}

/**
 * Returns instantaneous temperature of battery
 */
float getInstantaneousTemp(void)
{
    // TODO
}

/**
 * Returns instantaneous voltage of battery
 */
float getInstantaneousVoltage(void)
{
    // TODO
    return 3.9;
}

/**
 * Returns instantaneous current of battery 
 */
float getInstantaneousCurrent(void)
{
    // TODO
    //return fakeCurrentArray[rand() % 12];
    return 1;
}

/**
 * Calculates impedance of battery
 *
 * @param OCV       open circuit voltage of battery
 * @param voltage   instananeous voltage of battery
 * @param current   instananeous current of battery
 */
void calculateImpedance(float OCV, float voltage, float current)
{
    float impedance = (OCV-voltage)/current;
    if (impedance > BLOWN_FUSE_RESISTANCE_THRESH)
    {
        BMSDataPtr->Flags.FUSE_FAULT = 1;
    }

    return;
}

/**
 * Calculates the remaining run time of battery in minutes
 * This function is only valid during discharge
 *
 * @param battLevel     charge left in battery
 * @param current       instantaneous current 
 */
void calculateRemainingRunTime(float battLevel, float current)
{
    float battLevel_Ah = battLevel/3600;
    float runTime_hours = battLevel_Ah/current;
    
    BMSDataPtr->Data.remainingRunTime = runTime_hours*60;

    return;
}
