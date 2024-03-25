/*
 * task_imd.c
 *
 *  Created on: Jan 14, 2024
 *      Author: kkramer
 */
#include "task_imd.h"
#include "IMD.h"
#include "bms_data.h"

//#define SERIAL_SEND

//NOTE: pseudocode for now to get the thoughts down
void vMonitorIMD() {
    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 2000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        updateIMDData();
        serialSendData();   //Where to put this?
        IMDData_t imd_data = getIMDData();

        //TODO: check if any combo of IMDState and IsolationState combos make up a fault.
        // For the IMDState, there are multiple fault conditions so check for normal conditions rather
        // than each fault.
        if (!(imd_data.IMDState == Normal_Condition || imd_data.IMDState == Speed_Start_Measurement_Good)) {
            BMSDataPtr->Flags.HV_LV_ISOLATION_FAILURE = 1;
        }

        if (imd_data.IsolationState == Isolation_Failure) {
            BMSDataPtr->Flags.IMD_OPERATION_FAILURE = 1;
        }

        //Once shutdown sends a signal back saying the fault has been handled and shutdown circuit is restarted,
        //switch the BMS state back to normal or whatever it was previously.
    }
}
