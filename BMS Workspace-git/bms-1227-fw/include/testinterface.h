/*
 * testinterface.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Mahmoud Ahmed
 */

#ifndef INCLUDE_TESTINTERFACE_H_
#define INCLUDE_TESTINTERFACE_H_

#include "sci.h"
#include "Phantom_sci.h"

#define BUFFER_SIZE     100

typedef struct {
    uint32_t head;      // index of buffer head
    uint32_t tail;      // index of buffer head
    unsigned char *buffer;    // byte buffer to store the data
    uint32_t length;    // size of buffer
    uint32_t count;     // Current number of elements in buffer
    uint32_t datasize;  // size of each data in buffer
} testBuffer;

typedef struct {
    char str[BUFFER_SIZE];
    void (*cmd)(void);
} testcmd;

void echoChar(void);
void processChar(unsigned char character);
void push_tb(unsigned char character);
void pop_tb(void);
void displayPrompt(void);
void executeCommand(unsigned char command[]);
void getSingleVoltageReading(uint8_t cell);
void argumentParse(unsigned char charArray[]);


#endif /* INCLUDE_TESTINTERFACE_H_ */
