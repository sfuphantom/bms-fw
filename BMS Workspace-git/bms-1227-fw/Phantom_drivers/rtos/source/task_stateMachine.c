/*
 * task_stateMachine.c
 *
 *  Created on: Aug. 21, 2021
 *      Author: mkamalel
 */

#include "task_stateMachine.h"
#include "Phantom_sci.h"
#include "bms_data.h"
#include "hwConfig.h"
#include "sys_main.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"

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

        if (BMSState == BMS_CHARGING || BMS_RUNNING) {
            if (STATE_PRINT) {
                if (BMS_CHARGING) {
                    UARTSend(PC_UART, "********BMS CHARGING********");
                }
                else { // BMS_RUNNING
                    UARTSend(PC_UART, "********BMS RUNNING********");
                }
                UARTSend(PC_UART, "\n\r");
            }

            // Check all fault flags
            if (BMSDataPtr->Flags.FUSE_FAULT) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "FUSE_FAULT detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr->Flags.THREE_SECOND_FLAG) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "THREE_SECOND_FLAG detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr->Flags.TOTAL_CELL_ERROR_FLAG)  {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "TOTAL_CELL_ERROR_FLAG detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr->Flags.BAD_SLAVE_CONNECTION_FLAG) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "BAD_SLAVE_CONNECTION_FLAG detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr->Flags.HV_LV_ISOLATION_FAILURE) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "IMD_ISOLATION_FAILURE detected. BMS entering FAULT state");
                    UARTSend(PC_UART, "\n\r");
                }
                BMSState = BMS_FAULT;
            }
            if (BMSDataPtr->Flags.IMD_OPERATION_FAILURE) {
                if (STATE_PRINT) {
                    UARTSend(PC_UART, "IMD_ISOLATION_FAILURE detected. BMS entering FAULT state");
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

