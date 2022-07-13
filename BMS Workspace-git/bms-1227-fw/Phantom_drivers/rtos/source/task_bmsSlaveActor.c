/*
 * task_bmsSlaveActor.c
 *
 *  Created on: May 14, 2022
 *      Author: Devon
 *      Based heavily off of Raf's "queue prototype" architecture mockup.
 */

#include "task_bmsSlaveActor.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "queue_common.h"

/***********************************************************
 * @function                - vBMSSlaveActorTask
 *
 * @brief                   - This task will read BMS slave data from a queue and act upon it.
 *
 * @param[in]               - queueParams
 *
 * @return                  - None
 * @Note                    - None
 ***********************************************************/

// Set up storage for a queue (type defined in queue_common.h)
QueueArr_t queue; // Assigns storage
static QueueArr_t* q_ptr = &queue; // Creates static pointer access. TODO check if we need this as static!

void bmsSlaveActorInit(QueueArr_t other){

    /* Deep copy elements */
    q_ptr->tx = other.tx;
    q_ptr->rx = other.rx;
}


void actOnBMSSlaveData(bmsSlaveMsg_t* messageToActUpon) {
    // Take something out of the queue and process it.

    // This is a TODO for after PL455 rewrite code merges, but should take the place of the tasks in task_soc.c/h, task_stateMachine.c/h, as
    // well as something for temperature.
    // Our goal is to comply here with FSAE 2022 Rules:
    // - EV.8.3.1, EV.8.3.4, EV.8.3.5, EV.8.4.2, EV.8.5.2

    // I think those are all the rules around the "AMS" (BMS) that relate to ACTING on the data read from the BMS slaves.
    // Someone should double-check for sanity's sake however.
}

// Actor loop is to receive data from the corresponding Agent, and then ACT on it.
void vBMSSlaveActorTask(void *queueParams){

    // Set up task params:
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    // Setup storage for message we will receive into:
    static bmsSlaveMsg_t actor_data = {
            .data = {0},
            .tx_data = 0,
            .agent_state = 0,
            .actor_state = 0,
        };
    static bmsSlaveMsg_t* internalMessagePtr = &actor_data;
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
            // TODO: Add error handling for no messages from BMS slave agent (probably shutdown at this point, or if messages are lost for a long enough period)
            // For now, just continue:
            continue;
        }

        // Hit the business logic function with the data we recieved:
        actOnBMSSlaveData(internalMessagePtr);

        // Transmit into the tx queue (not necessary for all tasks!)
        xQueueSend(
            q_ptr->tx,
            &internalMessagePtr->tx_data,
            ( TickType_t ) 1
        );


    } while(1); // Repeat forever.

}
