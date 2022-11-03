/*
 * task_slave_pipeline.c
 *
 *  Created on: Oct 10, 2022
 *      Author: rafgu
 */

#include "task_slave_pipeline.h"

#include "Phantom_sci.h"


static void initAgent();
static void initActor();

static void vBMSSlaveActorTask(void*);
static void vBMSSlaveAgentTask(void*);


static uint8_t PostData(bmsSlaveMsg_t data, uint32_t timeout);
static uint8_t receiveQ(bmsSlaveMsg_t* data_ptr);

static SlavePipeline_t me = { NULL };

static uint8_t created()
{
    uint8_t x = (me.consumer != NULL);
    uint8_t y = (me.producer != NULL);
    uint8_t z = (me.queue    != NULL);

    return ( x || y || z );
}

#define PHANTOM_NUM_MESSAGES_IN_SHARED_BMS_QUEUE 1

uint8_t initSlavePipeline()
{

    UARTprintf("Initializing slave pipeline!\r\n");

    me.queue = xQueueCreate(PHANTOM_NUM_MESSAGES_IN_SHARED_BMS_QUEUE, sizeof(bmsSlaveMsg_t));

    initActor();

    initAgent();

    return created();
}

void initAgent()
{

    if (me.producer == NULL)
    {
        BaseType_t task = xTaskCreate(
            vBMSSlaveAgentTask,
            (const char*)"BMSSlaveAgentTask",
            240,
            NULL,
            1,
            &me.producer
        );

        // not really sure what xTaskCreate does to this variable if the task is
        // not created; let's make sure we set it back to the default value
        if (task == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
        {
            me.producer = NULL;
        }
    }
}

void initActor()
{
    if (me.producer == NULL)
    {
        BaseType_t task = xTaskCreate(
            vBMSSlaveActorTask,
            (const char*)"BMSSlaveActorTask",
            240,
            NULL,
            1,
            &me.consumer
        );

        if (task == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
        {
            me.consumer = NULL;
        }
    }
}

uint8_t receiveQ(bmsSlaveMsg_t* data_ptr)
{
    BaseType_t ret = xQueueReceive(
        me.queue,
        data_ptr,
        ( TickType_t ) pdMS_TO_TICKS(500)  // block for 500ms
    );

    return ret == pdTRUE;
}

uint8_t PostData(bmsSlaveMsg_t data, uint32_t timeout)
{
    BaseType_t ret = xQueueSend(
        me.queue,
        &data,
        pdMS_TO_TICKS(timeout)
    );

    return ret == pdTRUE;
}


void vBMSSlaveActorTask(void* args)
{
    // Setup storage for message we will receive into:
    bmsSlaveMsg_t data = {0};

    /* Initialize intermediate variables */
    uint8_t rx_success = 1;

    // Enter infinite loop of processing objects
    for (;;)
    {

        rx_success = receiveQ(&data);
        // Error checking for receive:
        if (!rx_success){
            // This means we are not able to read from the BMS slaves! This is either because it wasn't initialized
            // properly OR (if we tested the code before), the slaves have a hardware failure. Most definitely
            // a shutdown car type of deal...

            // TODO: Add error handling for no messages from BMS slave agent (probably shutdown at this point, or if messages are lost for a long enough period)
            continue;
        }

        UARTprintf("Received some data!\r\n");

        // Post data or event to some external task using their API queue
        // SomeAPIPost()
    }
}

static void agentReadSlaves(/* TBD */)
{
    //TODO: Implement slave communications
}

void vBMSSlaveAgentTask(void* args)
{

    // Set up task params:
    bmsSlaveMsg_t data = {0};

    // Enter infinite loop of creating objects for the queue
    for (;;)
    {
        agentReadSlaves(/* TBD */);

        // send data to actor
        if (PostData(data, portMAX_DELAY))
        {
            // This means that we were able to read successfully from the slaves
            // but the actor is not processing the data...someone is STARVING the CPU!

            // TODO: do something!
            // notify state machine or watcher task? Maybe so we can shutdown the car?
        }

        vTaskDelay(pdMS_TO_TICKS( 1000 ));
    }
}



