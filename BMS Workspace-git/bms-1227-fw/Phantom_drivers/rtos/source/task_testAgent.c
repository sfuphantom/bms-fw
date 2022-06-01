/*
 * task_testAgent.c
 *
 *  Created on: April 3, 2022
 *      Author: dlutz
 *  Based heavily off of Raf's "queue prototype" architecture mockup.
 */

#include "task_testAgent.h"
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

void testAgentInit(QueueArr_t other){

    /* Deep copy elements */
    q_ptr->tx = other.tx;
}

void agentProduceSomething(testAgentMsg_t* messagePtr) {
    // Do something here to put data into a queue somewhere
}

void vTestAgentTask(void *queueParams){

    // Set up task params:
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    static testAgentMsg_t data = {0};
    static testAgentMsg_t* data_ptr = &data;

    // Enter infinite loop of creating objects for the queue
    do {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        agentProduceSomething(data_ptr);

        // send data to actor
        xQueueSend(
            q_ptr->tx,
            (void *) data_ptr,
            ( TickType_t ) 10
        );

    } while(1);

}

