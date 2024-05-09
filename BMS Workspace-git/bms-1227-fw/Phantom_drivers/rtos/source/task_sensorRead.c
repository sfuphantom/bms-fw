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
#include "hv_driver.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"


void vSensorReadTask(void *pvParameters)
{

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = pdMS_TO_TICKS(20); // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    do{

        if(!getBMSinitFlag())
        {
           BMS_init();
        }

        // Read HV Battery Voltage from ADS7044 ADC using MIBSPI module 1
        UARTprintf("Reading HV \n\r");
        simulateVoltageHVADC(2021); // test function that simulates behavior of ADS7044 using mibspi module 3. requires hardware connections between mibspi modules 1 and 3
        getBatteryVoltageHV();

        //BMS_Balance_SIM();

        //thermistorRead();

        //BMS_Read_All(true);

        //UARTprintf("sensor read task \n\r");


        vTaskDelay(xFrequency);
        xLastWakeTime = xTaskGetTickCount();
    } while(1);

}
