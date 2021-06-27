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

//Charger//
void sendChargerMsg1(double maxVoltage, double maxCurrent);
int* receiveChargerMsg2();
//int* testFunction();
void canReceive();

//VCU//
//void sendVCUState(int TS_OFF, int TS_ON, int VCU_RUNNING, int MINOR_FAULT, int SEVERE_FAULT);
//void sendVCUCurrent(int CURRENT);
//void sendVCUHVbus(int HIGH_VOLTAGE);
//void sendVCUShutdownGPIO(int SHUTDOWN_IMD, int SHUTDOWN_BMS, int SHUTDOWN_BSPD, int SHUTDOWN_VCU);
//
//int* receiveVCUBMSState();
//int* receiveVCUDashState();
//
//int TS_OFF;
//int TS_ON;
//int VCU_RUNNING;
//int MINOR_FAULT;
//int SEVERE_FAULT;
//int CURRENT;
//int HIGH_VOLTAGE;
//int SHUTDOWN_IMD;
//int SHUTDOWN_BMS;
//int SHUTDOWN_BSPD;
//int SHUTDOWN_VCU;
//
//int BMS_ALIVE;
//int BMS_FAULT;
//int BMS_SOC;
//int BMS_TIME;
//
//int DASH_ALIVE;
//int DASH_FAULT;

#endif /* INCLUDE_PHANTOM_CAN_H_ */
