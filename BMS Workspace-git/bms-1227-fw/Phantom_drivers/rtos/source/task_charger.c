/*
 * task_charger.c
 *
 *  Created on: Aug. 21, 2021
 *      Author: mkamalel
 */

#include "task_charger.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"

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

