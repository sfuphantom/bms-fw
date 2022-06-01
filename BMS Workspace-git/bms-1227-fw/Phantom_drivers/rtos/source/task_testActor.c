/*
 * task_testActor.c
 *
 *  Created on: April 3, 2022
 *      Author: dlutz
 *      Based heavily off of Raf's "queue prototype" architecture mockup.
 */

#include "task_testActor.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "queue_common.h"

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

// Set up storage for a queue (type defined in queue_common.h)
QueueArr_t queue; // Assigns storage
static QueueArr_t* q_ptr = &queue; // Creates static pointer access. TODO check if we need this as static!

void actorConsumeAndActSomething(testActorMsg_t* messageToActUpon) {
    // Take something out of the queue and process it.
    // Equivalent to queue prototype's "actor_logic) function

    // Read from internal data, or set tx_data that will be sent later on....
}

void vTestActorTask(void *queueParams){

    // Set up task params:
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    // Setup storage for message we will receive into:
    static testActorMsg_t actor_data = {
            .data = {0},
            .tx_data = 0,
            .agent1_state = 0,
        };
    static testActorMsg_t* internalMessagePtr = &actor_data;
    /* Initialize intermediate variables */
    static BaseType_t rx_success = pdFALSE;

    // Enter infinite loop of processing objects
    do {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        TickType_t xLastWakeTime = xTaskGetTickCount();

        // Receive into rx queue:
        rx_success = xQueueReceive(
            q_ptr->rx,
            &internalMessagePtr->data,
            ( TickType_t ) pdMS_TO_TICKS(500)  // block for 500ms
        );

        // Error checking for receive:
        if (rx_success == pdFALSE){
            // TODO: Add error handling on a per-task basis.
            // For now, just continue:
            continue;
        }

        // Hit the business logic function with the data we recieved:
        actorConsumeAndActSomething(internalMessagePtr);

        // Transmit into the tx queue (not necessary for all tasks!)
        xQueueSend(
            q_ptr->tx,
            &internalMessagePtr->tx_data,
            ( TickType_t ) 1
        );


    } while(1); // Repeat forever.

}

