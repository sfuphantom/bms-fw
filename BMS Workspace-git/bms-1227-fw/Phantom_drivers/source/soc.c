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

static const uint16 MAX_CAPACITY = 6840;   // 1.9Ah = 6840 Coloumbs
static const float  MAX_LEVEL = 100;

static float battLevel = 0;
static uint16 battCapacity = 0;
static float fakeCurrentArray[] = {10, 8.7, 9.2, 16.5, 12.6, 19.5, 5, 3.5, -5, -2, -6, 12.2};

static float getInstantaneousCurrent();

void socInit(void)
{
    //float totalVoltage = getTotalBattVolt();
    //float avgTemp = getAvgTemp();
    //battLevel = (totalVoltage, avgTemp);

}

void socUpdate(void)
{
    float current = 0;
    current = getInstantaneousCurrent();

    battCapacity = battCapacity - current*SOC_TIMER_PERIOD;

    battLevel = battCapacity/MAX_CAPACITY * 100;
}

float getBattLevel(void)
{
    return battLevel;
}

static float getInstantaneousCurrent()
{
    return fakeCurrentArray[rand() % 12];
}
