/** @file sys_main.c
*   @brief Application main file
*   @date 22-June-2023
* */
#include <task_imd.h>

#define SERIAL_SEND

//NOTE: pseudocode for now to get the thoughts down
void vMonitorIMD() {
    while(1) {
        IMDData_t imd_data = getIMDData();
        //TODO: double check all fault conditions. These are just an example
        if (imd_data.IMDState == Device_Error) {
            //switch the BMS state to faulting
            //send shutdown signal
        }

        if (imd_data.IsolationState == Isolation_Failure) {
            //switch the BMS state to faulting
            //send shutdown signal
        }

        //Once shutdown sends a signal back saying the fault has been handled and shutdown circuit is restarted,
        //switch the BMS state back to normal or whatever it was previously.
    }
}
