/*
 * task_statemachine.c for BMS
 *
 *  Created on: June 6, 2020
 *      Author: Andrei D
 */

#include "task_statemachine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
#include "hwConfig.h"
#include "hal_stdtypes.h"
#include "os_portmacro.h"
#include "bms_data.h"
#include "Phantom_sci.h"
#include "gio.h"

extern BMSState_t BMSState;
bms_data BMSDataPtr;

// #include "charger_main.h" TODO: Uncomment when charger code is integrated
// #include "soc.h"

/* USER CODE END */

/* Include Files */



/* USER CODE BEGIN (4) */
/*********************************************************************************
 *                          FreeRTOS Tasks
 *********************************************************************************/
void vStateMachineTask(void *pvParameters){
    uint32 lrval;
    char stbuf[64];
    int nchars;

    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 4000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();
    do{
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        //UARTprintf("battery level = %f\n\r", getBattLevel());

/*********************** STATE MACHINE EVALUATION ***********************************/
        
        if ((BMSState == BMS_RUNNING) || (BMSState == BMS_CHARGING)) { 
            if (STATE_PRINT) {
                if (BMSState == BMS_CHARGING) {
                    UARTSend(PC_UART, "********BMS CHARGING********");
                }
                else { // BMS_RUNNING
                    UARTSend(PC_UART, "********BMS RUNNING********");
                }
                UARTSend(PC_UART, "\n\r");
            }

            // Check all fault flags
            if (BMSDataPtr.Flags.FUSE_FAULT) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "FUSE_FAULT detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr.Flags.THREE_SECOND_FLAG) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "THREE_SECOND_FLAG detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr.Flags.TOTAL_CELL_ERROR_FLAG)  {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "TOTAL_CELL_ERROR_FLAG detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr.Flags.BAD_SLAVE_CONNECTION_FLAG) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "BAD_SLAVE_CONNECTION_FLAG detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
        }
    if (BMSState == BMS_FAULT) {
        if (STATE_PRINT) {
            UARTSend(PC_UART, "********BMS FAULT********");
            UARTSend(PC_UART, "\n\r");
        }

        // TODO: Pull down GPIO that is connected to Shutdown Circuit
        // TODO: Send message over CAN so all controllers know we are shutting down
    }


    }while(1);
}
