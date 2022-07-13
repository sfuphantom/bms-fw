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
    q_ptr->rx = other.rx;
}

void agentReadSlaves(bmsSlaveMsg_t* messagePtr) {

    // Put data in a message to be added to queue for processing by our corresponding Actor.

    // This is a TODO for after the PL455 rewrite merges, but this should take the place of task_sensorRead.c/h and maaaybe task_balance.c/h?
    // We also need to handle getting temperature data here.

    // Our goal is to comply here with FSAE 2022 Rules:
    // - EV.8.3.1, EV.8.3.3, EV.8.4.1, EV.8.5.1, EV.8.5.5, EV.8.5.6

    // I think those are all the rules around the "AMS" (BMS) that relate to READING/making available the data from the BMS slaves.
    // Someone should double-check for sanity's sake however, and of course this is a joint effort with the corresponding actor.


}

void vBMSSlaveAgentTask(void *queueParams){

    // Set up task params:
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    static bmsSlaveMsg_t data = {0};
    static bmsSlaveMsg_t* data_ptr = &data;

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
