/*
 * phantom_freertos.c
 *
 *  Created on: Apr 18, 2020
 *      Author: Mahmoud Ahmed
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"
#include "Phantom_sci.h"
#include "sys_main.h"

void xphRtosInit(void)
{
    xphTimerInit();
    xphTaskInit();
}

void xphTimerInit(void)
{
    /*********************************************************************************
    *                          freeRTOS SOFTWARE TIMER SETUP
    *********************************************************************************/

           xTimers[0] = xTimerCreate
                      ( /* Just a text name, not used by the RTOS
                       kernel. */
                       "RTDS_Timer",
                       /* The timer period in ticks, must be
                       greater than 0. */
                       pdMS_TO_TICKS(1),
                       /* The timers will auto-reload themselves
                       when they expire. */
                       pdTRUE,
                       /* The ID is used to store a count of the
                       number of times the timer has expired, which
                       is initialised to 0. */
                       ( void * ) 0,
                       /* Callback function for when the timer expires*/
                       socTimer
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
                       pdTRUE,
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
}

void xphTaskInit(void)
{
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
}


