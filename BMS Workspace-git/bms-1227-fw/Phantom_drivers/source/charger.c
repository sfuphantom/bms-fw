///*
// * charger_main.c
// *
// * The purpose of this file is to...
// *      a) Set the values at which we want to charge (maxVoltage, maxCurrent)
// *      b) Define when we send the message
// *      c) Read and store the incoming values over CAN via pointers (output current, output voltage, any faults)
// *
// * Created on: Jan 31, 2021
// * Author: Elena Soboleva
// */
//
//#include <stdio.h>
////#include "phantom_can.c"
//#include "phantom_can.h"
//#include "pl455.h" //for delay function
//
//
//void function_to_charge(){
//    double maxVoltage = 3.3;    //in Volts
//    double maxCurrent = 10.5;   //in Amps
//    double minCurrent = 0.1;    //This is out cut off current. So when the current output falls below or hits that value, we stop charging.
//
//    const char* outputVoltage;
//    double outputCurrent;
//
//    //uint16_t chargerData[] = receiveChargerMsg2(); //reading the outputs
//
//    int* ptr = receiveChargerMsg2();
//
//    UARTprintf("%d %d", ptr[0], ptr[1]);
//
//    //outputVoltage = chargerData[0];
//    //outputCurrent = chargerData[1];
//
//    int GPIO_pin_17 = 1;    //temp until know how to read actual value from VCU
//
//    if(GPIO_pin_17 == 1){   //Replace GPIO_pin_17 with the actual GPIO pin name
//        while(outputCurrent > minCurrent){
//
//            sendChargerMsg1(maxVoltage, maxCurrent, incoming_data[2]);
//            delayms(1000);            //This is how often we send this message. Every 1 second.
//
//            printf(outputVoltage, outputCurrent, incoming_data[2]);
//        }
//    }
//    else
//    {
//        incoming_data[2] = 256;
//        sendChargerMsg1(0, 0, incoming_data[2]);
//    }
//}
