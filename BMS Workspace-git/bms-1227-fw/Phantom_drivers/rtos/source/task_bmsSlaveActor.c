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

void actOnBMSSlaveData(bmsSlaveActorMsg_t* messageToActUpon) {
    // Take something out of the queue and process it.

    // TODO Fill this in with what we assume the PL455 rewrite will give us!
}

void vBMSSlaveActorTask(void *queueParams){

    // Set up task params:
    TickType_t xLastWakeTime;          // will hold the timestamp at which the task was last unblocked
    const TickType_t xFrequency = 1000; // task frequency in ms

    // Initialize the xLastWakeTime variable with the current time;
    xLastWakeTime = xTaskGetTickCount();

    // Setup storage for message we will receive into:
    static bmsSlaveActorMsg_t actor_data = {
            .data = {0},
            .tx_data = 0,
            .bms_state = 0,
        };
    static bmsSlaveActorMsg_t* internalMessagePtr = &actor_data;
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
