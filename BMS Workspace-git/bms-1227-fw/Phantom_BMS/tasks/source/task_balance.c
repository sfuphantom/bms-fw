/*
 * task_balance.c for BMS
 *
 *  Created on: June 6, 2020
 *      Author: Andrei D
 */

#include "task_balance.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
#include "hwConfig.h"
#include "os_portmacro.h"
#include "bms_data.h"
#include "Phantom_sci.h"
#include "gio.h"

/***********************************************************
 * @function                - vBalanceTask
 *
 * @brief                   - This task will passively balance the cells during BMS_CHARGING state
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vBalanceTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        BMS_Balance();

    }while(1);

}