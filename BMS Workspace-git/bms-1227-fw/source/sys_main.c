/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */

/*
 *
 *  3V3         -> bq76PL455A-Q1 EVM J3 pin 3 (VIO)
 *  SCI1_RX     -> bq76PL455A-Q1 EVM J3 pin 5 (TX) **remove R8 from TMS570LS04x Launchpad
 *  SCI1_TX     -> bq76PL455A-Q1 EVM J3 pin 4 (RX)
 *  GND         -> bq76PL455A-Q1 EVM J3 pin 1 (DGND)
 *  GIOA0       -> bq76PL455A-Q1 EVM J3 pin 6 (nWAKE)
 *  GIOA1       -> bq76PL455A-Q1 EVM J3 pin 2 (nFAULT)
 *
 */


#include "gio.h"
#include "sci.h"
#include "rti.h"
#include "system.h"
#include "sys_vim.h"
#include "pl455.h"
#include "Phantom_sci.h"
#include "phantom_can.h"
#include "phantom_pl455.h"
#include "can.h"
#include "esm.h"
#include "sys_core.h"
#include "reg_het.h"
#include "testinterface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
#include "sys_main.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"


/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */

int UART_RX_RDY = 0;
int RTI_TIMEOUT = 0;

/*********************************************************************************
 *                          TASK HEADER DECLARATIONS
 *********************************************************************************/
static void vStateMachineTask(void *);  // This task will evaluate the state machine and decide whether or not to change states
static void vSensorReadTask(void *);    // This task will read all the sensors in the vehicle (except for the APPS which requires more critical response)

/*********************************************************************************
 *                          SOFTWARE TIMER INITIALIZATION
 *********************************************************************************/
#define NUMBER_OF_TIMERS   2

/* array to hold handles to the created timers*/
TimerHandle_t xTimers[NUMBER_OF_TIMERS];

void Timer_10ms(TimerHandle_t xTimers);
void Timer_2s(TimerHandle_t xTimers);

/*********************************************************************************
 *                          STATE ENUMERATION
 *********************************************************************************/

State state = RUNNING;
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
       BMS_init();

/*********************************************************************************
*                          freeRTOS SOFTWARE TIMER SETUP
*********************************************************************************/

       xTimers[0] = xTimerCreate
                  ( /* Just a text name, not used by the RTOS
                   kernel. */
                   "RTDS_Timer",
                   /* The timer period in ticks, must be
                   greater than 0. */
                   pdMS_TO_TICKS(10),
                   /* The timers will auto-reload themselves
                   when they expire. */
                   pdFALSE,
                   /* The ID is used to store a count of the
                   number of times the timer has expired, which
                   is initialised to 0. */
                   ( void * ) 0,
                   /* Callback function for when the timer expires*/
                   Timer_10ms
                 );

      xTimers[1] = xTimerCreate
                  ( /* Just a text name, not used by the RTOS
                   kernel. */
                   "RTDS_Timer",
                   /* The timer period in ticks, must be
                   greater than 0. */
                   pdMS_TO_TICKS(2000),
                   /* The timers will auto-reload themselves
                   when they expire. */
                   pdFALSE,
                   /* The ID is used to store a count of the
                   number of times the timer has expired, which
                   is initialised to 0. */
                   ( void * ) 0,
                   /* Callback function for when the timer expires*/
                   Timer_2s
                 );
      if( xTimers[0] == NULL )
      {
               /* The timer was not created. */
          UARTSend(sciREG, "The timer was not created.\r\n");
      }
      else
      {
           /* Start the timer.  No block time is specified, and
           even if one was it would be ignored because the RTOS
           scheduler has not yet been started. */
           if( xTimerStart( xTimers[0], 0 ) != pdPASS )
           {
               /* The timer could not be set into the Active
               state. */
               UARTSend(sciREG, "The timer could not be set into the active state.\r\n");
           }
      }

      if( xTimers[1] == NULL )
      {
           /* The timer was not created. */
          UARTSend(sciREG, "The timer was not created.\r\n");
      }
      else
      {
           /* Start the timer.  No block time is specified, and
           even if one was it would be ignored because the RTOS
           scheduler has not yet been started. */
           if( xTimerStart( xTimers[1], 0 ) != pdPASS )
           {
               /* The timer could not be set into the Active
               state. */
               UARTSend(sciREG, "The timer could not be set into the active state.\r\n");
           }
      }

/*********************************************************************************
*                          freeRTOS TASK & QUEUE CREATION
*********************************************************************************/
      if (xTaskCreate(vStateMachineTask, (const char*)"StateMachineTask",  240, NULL,  (STATE_MACHINE_TASK_PRIORITY), NULL) != pdTRUE)
      {
          // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
          // probably need a better error handler
          sciSend(sciREG,23,(unsigned char*)"StateMachineTask Creation Failed.\r\n");
          while(1);
      }
      if (xTaskCreate(vSensorReadTask, (const char*)"SensorReadTask",  240, NULL,  (SENSOR_READ_TASK_PRIORITY), NULL) != pdTRUE)
      {
          // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
          // probably need a better error handler
          sciSend(sciREG,23,(unsigned char*)"SensorReadTask Creation Failed.\r\n");
          while(1);
      }

      vTaskStartScheduler();

      // infinite loop to prevent code from ending. The scheduler will now pre-emptively switch between tasks.
      while(1);
}


/* USER CODE BEGIN (4) */
static void vStateMachineTask(void *pvParameters){
    uint32 lrval;
    char stbuf[64];
    int nchars;

    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 4000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();
    do{
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    UARTprintf("STATE MACHINE TASK\n\r");
    }while(1);
}


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
static void vSensorReadTask(void *pvParameters){

    // any initialization
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 2000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    int nchars;
    char stbuf[64];
    do{
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    BMS_Read_All_NP();
    }while(1);

}

void Timer_10ms(TimerHandle_t xTimers)
{

}

/* Timer callback when it expires for the ready to drive sound */
void Timer_2s(TimerHandle_t xTimers)
{

}

/* USER CODE BEGIN (4) */
/* USER CODE END */



/* USER CODE BEGIN (4) */
/* USER CODE END */
