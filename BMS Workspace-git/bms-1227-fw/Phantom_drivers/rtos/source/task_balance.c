/*
 * task_balance.c
 *
 *  Created on: Aug. 21, 2021
 *      Author: mkamalel
 */


#include "task_balance.h"
#include "phantom_pl455.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"

void vBalanceTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    do {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        BMS_Balance();

    } while(1);

}
