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

#include "task_bmsSlaveAgent.h"
#include "task_bmsSlaveActor.h"
#include "agentactor.h"
#include "queue_common.h"


// Register agent and actor tasks:
bool startSensorAgentsActors() {
    // This type of function could potentially be merged into phantom_freertos.c - but if we are to add more agents/actors in the future I think
    // having a separate file is not a bad idea.

    struct QueueArr_t sharedQueue;
    sharedQueue.tx = xQueueCreate(PHANTOM_NUM_MESSAGES_IN_SHARED_BMS_QUEUE, sizeof(bmsSlaveMsg_t));
    sharedQueue.rx = xQueueCreate(PHANTOM_NUM_MESSAGES_IN_SHARED_BMS_QUEUE, sizeof(bmsSlaveMsg_t));

    bmsSlaveAgentInit(sharedQueue);
    bmsSlaveActorInit(sharedQueue);


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

