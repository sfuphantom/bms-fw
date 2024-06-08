/*
 * task_sensorRead.c
 *
 *  Created on: Aug. 21, 2021
 *      Author: mkamalel
 */


#include <current_transducer.h>
#include <phantom_sci.h>
#include "task_sensorRead.h"
#include "phantom_pl455.h"
#include "soc.h"
#include "bms_data.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "hv_driver.h"

// private function only for use in vSensorReadTask
// reads high voltage current using the functions in current_transducer.h/c
// checks for under/over voltage current and assigns appropriate flags to global BMSDataPtr
static void currentCheckHV()
{
    UARTprintf("Reading main battery current...\n\r");
    // global variables in Current_transducer.h store these ADC voltage and accumulator current such that HVcurrent_Range_Check can read them
    getHVsensorVoltage(); // this function reads the ADC voltage from the current transducer output pin
    getHVsensorCurrent(); // this function transforms the read voltage to the main battery current in amps

    // set OVER_CURRENT_FLAG to 1 when current is ABOVE the maximum +200A
    // this will put BMS to FAULT state in vStateMachineTask, which will trigger shutdown
    // returns 0 on normal operating current
    BMSDataPtr->Flags.OVER_CURRENT_FLAG = overCurrentFaultCheckHV();

    // set UNDER_CURRENT_FLAG to 1 when current is BELOW the minimum -200A
    // this will put BMS to FAULT state in vStateMachineTask, which will trigger shutdown
    // returns 0 on normal operating current
    BMSDataPtr->Flags.UNDER_CURRENT_FLAG = underCurrentFaultCheckHV();
}

static void voltageCheckHV()
{
    UARTprintf("Reading main battery voltage...\n\r");
    // global variables in Current_transducer.h store these ADC voltage and accumulator current such that HVcurrent_Range_Check can read them
    simulateVoltageHVADC(2021); // this function uses mibspi3 to simulate the behavior of the ADS7044 ADC that reads HV voltage
    getBatteryVoltageHV();         // this reads voltage from the ADS7044, it drives the mibspi1 CS[0] pin low which initiates the transfer

    // set OVER_CURRENT_FLAG to 1 when current is ABOVE the maximum +200A
    // this will put BMS to FAULT state in vStateMachineTask, which will trigger shutdown
    // returns 0 on normal operating current
    BMSDataPtr->Flags.OVER_VOLTAGE_FLAG = overVoltageFaultCheckHV();

    // set UNDER_CURRENT_FLAG to 1 when current is BELOW the minimum -200A
    // this will put BMS to FAULT state in vStateMachineTask, which will trigger shutdown
    // returns 0 on normal operating current
    BMSDataPtr->Flags.UNDER_VOLTAGE_FLAG = underVoltageFaultCheckHV();
}


void vSensorReadTask(void *pvParameters)
{
    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 2000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    static int shutdownDelay;


    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();
        shutdownDelay ++;
        if(shutdownDelay%2){
            gioSetBit(gioPORTA, 5, 1);
        }
        else{
            gioSetBit(gioPORTA, 5, 0);
        }

        if(!getBMSinitFlag())
        {
           BMS_init();
        }

        //BMS_Balance_SIM();

        //thermistorRead(); //TODO: implement this

        BMS_Read_All(true);

        currentCheckHV(); //check for over  max or under min main battery current
        voltageCheckHV(); //check for over max or under min main battery voltage

        //UARTprintf("sensor read task \n\r");
    } while(1);

}
