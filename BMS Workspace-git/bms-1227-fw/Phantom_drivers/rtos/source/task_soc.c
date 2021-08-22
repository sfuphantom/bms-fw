/*
 * task_soc.c
 *
 *  Created on: Aug. 21, 2021
 *      Author: mkamalel
 */

#include "task_soc.h"
#include "soc.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"

/***********************************************************
 * @function                - vSOCTask
 *
 * @brief                   - This task will update the state of charge and remaining run time estimations, and check for blown fuses)
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vSOCTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        socProcess();

    }while(1);

}

