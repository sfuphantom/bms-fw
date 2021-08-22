/*
 * task_sensorRead.c
 *
 *  Created on: Aug. 21, 2021
 *      Author: mkamalel
 */


#include "task_sensorRead.h"
#include "phantom_pl455.h"
#include "soc.h"
#include "bms_data.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"


void vSensorReadTask(void *pvParameters)
{

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 2000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        if(!getBMSinitFlag())
        {
           BMS_init();
        }

        //BMS_Balance_SIM();

        //thermistorRead();

        BMS_Read_All(true);

        //UARTprintf("sensor read task \n\r");
    }while(1);

}
