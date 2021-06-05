/*
 * task_watchdog.c from BMS
 *
 *  Created on: Mar 31, 2020
 *      Author: gabriel
 */

#include "task_watchdog.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "os_timer.h"
#include "os_portmacro.h"

//#include "board_hardware.h"   // contains hardware defines for specific board used (i.e. VCU or launchpad)
#include "hwConfig.h"

#include "Phantom_sci.h"
#include "gio.h"
//#include "priorities.h"

/***********************************************************
 * @function                - vWatchdogTask
 *
 * @brief                   - This task will monitor all threads and pet the watchdog if everything is fine. Else it will let the watchdog reset the MCU
 *
 * @param[in]               - pvParameters
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/

#include "reg_het.h" //THIS IS GROSS BTW FIND A WAY TO STORE HARDWARE SHIT
// ANDREI'S GARBAGE - ORGANIZE THESE BETTER (START) /////////////////////////////////////////////////////////

#define WATCHDOG_TASK_PRIORITY         0 //PUT TASKS SOMEWHERE ELSE
#define TASK_PRINT                     1
void ANDREItaskCreate(void);

void vWatchdogTask(void *);             // This task will perform watchdog petting
void xBlinkTask1(void *pvParameters);
void xBlinkTask2(void *pvParameters);

/***************  WATCHDOG GPIO  *********************/
#define WATCHDOG_PORT         hetPORT1
#define WATCHDOG_PIN          2
// ANDREI'S GARBAGE - ORGANIZE THESE BETTER (END) //////////////////////////////////////////////////////////


void vWatchdogTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms
    // watchdog timeout is 1.6 seconds

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    while(true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        //UARTSend(PC_UART, "Pet\r\n");

        if (TASK_PRINT) {UARTSend(PC_UART, "------------->WATCHDOG TASK\r\n");}
        //UARTSend(PC_UART, xTaskGetTickCount());

        gioToggleBit(WATCHDOG_PORT, WATCHDOG_PIN);
    }

}