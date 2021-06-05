/*
 * task_charger.c for BMS
 *
 *  Created on: June 6, 2020
 *      Author: Andrei D
 */

#include "task_charger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
#include "hwConfig.h"
#include "os_portmacro.h"

/***********************************************************
 * @function                - vChargerTask
 *
 * @brief                   - This task runs functionto charge cells during BMS_CHARGING
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vChargerTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1500; // task frequency in ms - frequency taken to be the same as frequency in sys_main.c

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        // function_to_charge(); TODO: Uncomment when charger code is integrated

    }while(1);

}