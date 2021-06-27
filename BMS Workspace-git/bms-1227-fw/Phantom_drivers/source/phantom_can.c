#include "phantom_can.h"
#include "can.h"
#include "stdlib.h"
#include "sys_main.h"
#include "Phantom_sci.h" //for UARTprintf
#include "hwConfig.h" //for PC_UART
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct Queue* queueData;

uint8 rx_data1[8] = { 0 };
uint8 *rx_ptr1 = &rx_data1[0];

void sendChargerMsg1(double maxVoltage, double maxCurrent)
{
    uint16 outgoing_data[4] = { 0 }; //make it a struct

    maxVoltage *= 10; //need voltage to be an int so instead of 3.3 it's as 33
    maxCurrent *= 10;

    outgoing_data[0] = maxVoltage;
    outgoing_data[1] = maxCurrent;
//outgoing_data[2] is defined in sys_main.c
//I dont think we need to define outgoing_data[3] because it's empty anyways, we just need the 4th placeholder because that's what the charger wants to see.

    uint16 *outgoing_data_ptr = &outgoing_data[0];

    int j = 0;
    for (j = 0; j < 4; j++)
    {
        // can't find a workaround to the uint16, the DBC needs it
        canTransmit(canREG1, canMESSAGE_BOX9, outgoing_data_ptr);
        outgoing_data_ptr += 16; /* next chunk ...*/
    }
}

void canMessageNotification(canBASE_t *node, uint32 messageBox) //Receiving Interrupt
{

//this is specifically to receive uint16 (4 blocks)
//now they are saved as 4 separate integers (converted from hex)
     queueData = createQueue(10);
//RECEIVING DATA AND ENQUEING IT
     if(node==canREG1)
     {
        while(!canIsRxMessageArrived(canREG1, canMESSAGE_BOX11)); //0xB
        canGetData(canREG1, canMESSAGE_BOX11, rx_ptr1); /* copy to RAM */
        UARTprintf("test print", PC_UART);
        enqueue(queueData, rx_data1[0]);
        UARTprintf("test print", PC_UART);
        enqueue(queueData, rx_data1[1]);
        UARTprintf("test print", PC_UART);
        enqueue(queueData, rx_data1[2]);
        UARTprintf("test print", PC_UART);
        enqueue(queueData, rx_data1[3]);
        rx_ptr1=&rx_data1[0]; //when get next message under this address, rx_ptr1 is "reset".
     }

}

//    /*********************************************************************************
//     *                          Queue Code
//     *
//     * Note: This should be moved to the main code, does not belong in CAN
//     *       Maybe its own file?
//     *********************************************************************************/
//
// A structure to represent a queue
struct Queue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};
// function to create a queue
// of given capacity.
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*)malloc(
        sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;

    // This is important, see the enqueue
    queue->rear = capacity - 1;
    queue->array = (int*)malloc(
        queue->capacity * sizeof(int));
    return queue;
}


// Queue is full when size becomes
// equal to the capacity
int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, int item) //ADDS TO QUEUE
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)
                  % queue->capacity;
    //UARTprintf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue.
// It changes front and size
int dequeue(struct Queue* queue) //REMOVES FROM QUEUESZ
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)
                   % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}
