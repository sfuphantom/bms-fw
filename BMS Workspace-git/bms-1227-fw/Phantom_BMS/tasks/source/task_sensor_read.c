/*
 * task_sensor_read.c for BMS
 *
 *  Created on: June 6, 2020
 *      Author: Andrei D
 */

#include "task_sensor_read.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "gio.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"
#include "hwConfig.h"  // contains hardware defines for specific board used (i.e. BMS or launchpad)
#include "Phantom_sci.h"
#include "FreeRTOS.h"
#include "bms_data.h"


// change to better data type
int lv_current = 0;

extern uint8_t RTDS;// = 0;
long RTDS_RAW;      // = 0;

/***********************************************************
 * @function                - vSensorReadTask
 *
 * @brief                   - This task will read all the sensors in the vehicle (except for the APPS which requires more critical response)
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/
void vSensorReadTask(void *pvParameters){

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
