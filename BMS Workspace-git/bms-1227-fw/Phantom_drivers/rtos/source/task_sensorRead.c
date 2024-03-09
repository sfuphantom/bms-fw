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
#include "Current_transducer.h"
#include "Phantom_sci.h"

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

        UARTprintf("Reading main battery current...\n\r");
        // global variables in Current_transducer.h store these ADC voltage and accumulator current such that HVcurrent_Range_Check can read them
        getHVsensorVoltage(); // this function reads the ADC voltage from the current transducer output pin
        getHVsensorCurrent(); // this function transforms the

        // set OVER_CURRENT_FLAG to 1 when current is OUTSIDE the maximum +/-200A
        // this will put BMS to FAULT state in vStateMachineTask, which will trigger shutdown
        // returns 0 on normal operating current
        BMSDataPtr->Flags.OVER_CURRENT_FLAG = HVcurrent_Range_Check();

        //UARTprintf("sensor read task \n\r");
    } while(1);

}
