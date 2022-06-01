/*
 * agentactor.c
 *
 *  Created on: Apr. 7, 2022
 *      Author: dlutz
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"
#include "Phantom_sci.h"
#include "sys_main.h"
#include "bms_data.h"

#include "task_testActor.h"
#include "task_testAgent.h"
#include "task_bmsSlaveAgent.h"
#include "task_bmsSlaveActor.h"
#include "agentactor.h"

// Register agent and actor tasks:
//bool startTestAgentActor() {
//    // If you really wanted to do this properly, put the task creation in phantom_freertos.c
//
//    if (xTaskCreate(vTestActorTask, (const char*)"TestActorTask",  240, NULL,  (STATE_MACHINE_TASK_PRIORITY), NULL) != pdTRUE) {
//          // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
//          // probably need a better error handler
//          sciSend(sciREG,23,(unsigned char*)"TestActorTask Creation Failed.\r\n");
//          return false;
//    }
//
//    if (xTaskCreate(vTestAgentTask, (const char*)"TestAgentTask",  240, NULL,  (STATE_MACHINE_TASK_PRIORITY), NULL) != pdTRUE) {
//              // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
//              // probably need a better error handler
//              sciSend(sciREG,23,(unsigned char*)"TestAgentTask Creation Failed.\r\n");
//              return false;
//    }
//
//    return true;
//}

// Register agent and actor tasks:
bool startSensorAgentsActors() {
    // If you really wanted to do this properly, put the task creation in phantom_freertos.c

    if (xTaskCreate(vBMSSlaveAgentTask, (const char*)"BMSSlaveAgentTask",  240, NULL,  (STATE_MACHINE_TASK_PRIORITY), NULL) != pdTRUE) {
          // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
          // probably need a better error handler
          sciSend(sciREG,23,(unsigned char*)"BMSSlaveAgentTask Creation Failed.\r\n");
          return false;
    }

    if (xTaskCreate(vBMSSlaveActorTask, (const char*)"BMSSlaveActorTask",  240, NULL,  (STATE_MACHINE_TASK_PRIORITY), NULL) != pdTRUE) {
              // if xTaskCreate returns something != pdTRUE, then the task failed, wait in this infinite loop..
              // probably need a better error handler
              sciSend(sciREG,23,(unsigned char*)"BMSSlaveActorTask Creation Failed.\r\n");
              return false;
    }

    return true;
}

