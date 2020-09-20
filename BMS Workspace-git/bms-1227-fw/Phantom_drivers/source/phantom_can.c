/*
 * phantom_vcu.c
 *
 *  Created on: Feb 25, 2020
 *      Author: XingLu Wang
 */
#include "phantom_can.h"
#include "can.h"
#include "stdlib.h"
#include "sys_main.h"

#define D_COUNT 8
uint32 cnt = 0, error = 0, tx_done = 0;
uint8 rx_data1[8] = { 0 };
uint8 *rx_ptr1 = &rx_data1[0];
uint8 vcuint = 0;
int timeout;

volatile uint32_t transmitCounter = 0;
volatile uint32_t receiveVCUCounter = 0;

void CANSend()
{
    uint8 tx_data[D_COUNT] = { 'B', 'M', 'S', 'R', 'E', 'A', 'D', 'Y' };
    uint8 *tx_ptr = &tx_data[0];

    for (cnt = 0; cnt < D_COUNT; cnt++)
    {
//        canTransmit(canREG1, canMESSAGE_BOX2, tx_ptr); /* transmitting 8 different chunks 1 by 1 */
//        while (timeout != 1U)
//        {
//            timeout = GetTimeout(); /* ... wait until transmit request is through */
//        }

        timeout = 0U;
        tx_done = 0;
        tx_ptr += 8; /* next chunk ...*/
    }
}

void sendVoltageArray()
{
    float voltArr[5] = { 1.1, 1.2, 1.3, 1.4, 1.5 };
    int i, clk;
    for (i = 0; i < 5; i++)
    {
        sendFloat(voltArr[i]);
        for (clk = 0; clk < 1000000; clk++);
    }

}

void sendFloat(float val)
{
    // val is MAX 10 for now
    int num = val * 6553.6;

    uint16 tx_data2[D_COUNT] = { 'S', 'B', 'I', 'N' }; //wtf? and change uint16 everywhere pls
    tx_data2[3] = num;

    uint16 *tx_ptr2 = &tx_data2[0];

    int j = 0;
    for (j = 0; j < D_COUNT; j++)
    {
        canTransmit(canREG1, canMESSAGE_BOX2, tx_ptr2);
        tx_ptr2 += 16; /* next chunk ...*/
    }
}

void canMessageNotification(canBASE_t *node, uint32 messageBox)
{

    //The following is executed when transmission is successful
    if ((node == canREG1) && (messageBox == canMESSAGE_BOX2))
    {
        tx_done = 1;
        transmitCounter++;
    }

    //The following is executed when reception is successful
    if ((node == canREG1) && (messageBox == canMESSAGE_BOX7))
    {
        while (!canIsRxMessageArrived(canREG1, canMESSAGE_BOX7))
            ;

        canGetData(canREG1, canMESSAGE_BOX7, rx_ptr1); /* copy to RAM */
        rx_ptr1 += 8;
        receiveVCUCounter++;

    }
}

