/*
 * soc.c
 *
 *  Created on: Jul 20, 2020
 *      Author: Mahmoud Ahmed
 */

#include <stdio.h>
#include <stdlib.h>
#include "soc.h"
#include "phantom_pl455.h"
#include "thermistor.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"


static const float MAX_CAPACITY = 6840;   // 1.9Ah = 6840 Coloumbs
static const float  MAX_LEVEL = 100;

static float battLevel = 0;
static float battCapacity = 0;
static float fakeCurrentArray[] = {10, 8.7, 9.2, 16.5, 12.6, 19.5, 5, 3.5, -5, -2, -6, 12.2};

static float getInstantaneousCurrent();

void socInit(void)
{
    //float totalVoltage = getTotalBattVolt();
    //float avgTemp = getAvgTemp();
    //battLevel = (totalVoltage, avgTemp);
    battCapacity = MAX_CAPACITY;
}

void socUpdate(void)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    UARTprintf("current time = %d\n\r", xLastWakeTime);
    float current = 0.0;
    current = getInstantaneousCurrent();

    battCapacity = battCapacity - current*SOC_TIMER_PERIOD;
    battLevel = battCapacity/MAX_CAPACITY * 100;
    if(battLevel >= 0 && battLevel <= 100)
    {
        //UARTprintf("battery capacity = %f | battery level = %f | current = %f \n\r ", battCapacity, battLevel, current);
    }
    else
    {
       // UARTprintf("battery empty");
    }
}

float getBattLevel(void)
{
    return battLevel;
}

static float getInstantaneousCurrent()
{
    return fakeCurrentArray[rand() % 12];
}
