/*
 * task_bmsSlaveAgent.c
 *
 *  Created on: May 14, 2022
 *      Author: Devon
  *  Based heavily off of Raf's "queue prototype" architecture mockup.
 */

#include "task_bmsSlaveAgent.h"
#include "queue_common.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"

/***********************************************************
 * @function                - vTestAgentTask
 *
 * @brief                   - This task will perform the Agent role for our testing of agent/actor model.
 *
 * @param[in]               - queueParams
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/

static QueueArr_t q;
static QueueArr_t* q_ptr = &q;

void bmsSlaveAgentInit(QueueArr_t other){

    /* Deep copy elements */
    q_ptr->tx = other.tx;
}

void agentReadSlaves(bmsSlaveAgentMsg_t* messagePtr) {

    // TODO: Read from Ryan's rewritten pl455 driver and add messages as needed
}

void vBMSSlaveAgentTask(void *queueParams){

    // Set up task params:
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    static bmsSlaveAgentMsg_t data = {0};
    static bmsSlaveAgentMsg_t* data_ptr = &data;

    // Enter infinite loop of creating objects for the queue
    do {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        agentReadSlaves(data_ptr);

        // send data to actor
        xQueueSend(
            q_ptr->tx,
            (void *) data_ptr,
            ( TickType_t ) 10
        );

    } while(1);

}
