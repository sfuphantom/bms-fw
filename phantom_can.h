/*
 * phantom_can.h
 *
 *  Created on: Feb 25, 2020
 *      Author: XingLu Wang
 */

#ifndef INCLUDE_PHANTOM_CAN_H_
#define INCLUDE_PHANTOM_CAN_H_

#include "stdint.h"

void CANSend();
void sendVoltageArray();
void sendFloat(float val);
void testMessage1();


struct Queue* queue;
struct Queue* createQueue(unsigned capacity);
int isFull(struct Queue* queue);
int isEmpty(struct Queue* queue);
void enqueue(struct Queue* queue, int item);
int dequeue(struct Queue* queue);
int front(struct Queue* queue);
int rear(struct Queue* queue);

uint16_t outgoing_data[4];
int maxVoltage;
int maxCurrent;
uint16_t outgoing_data_ptr;
uint16_t incoming_data[4];
int outputVoltage;
int outputCurrent;
int outputState;
uint16_t incoming_data_ptr;

void sendChargerMsg1(double maxVoltage, double maxCurrent, int incoming_data[2]);

int* receiveChargerMsg2();
//int* testFunction();
void canReceive();


#endif /* INCLUDE_PHANTOM_CAN_H_ */
