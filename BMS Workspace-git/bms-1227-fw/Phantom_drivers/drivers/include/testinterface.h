/*
 * testinterface.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Mahmoud Ahmed
 */

#ifndef INCLUDE_TESTINTERFACE_H_
#define INCLUDE_TESTINTERFACE_H_

#include <phantom_sci.h>
#include "sci.h"

#define BUFFER_SIZE     100

typedef struct {
    uint32_t head;      // index of buffer head
    uint32_t tail;      // index of buffer head
    unsigned char *buffer;    // byte buffer to store the data
    uint32_t length;    // size of buffer
    uint32_t count;     // Current number of elements in buffer
    uint32_t datasize;  // size of each data in buffer
} testBuffer;

typedef enum {
    voltage,
    temperature,

    MAX_SIGNAL
} signalIndex;

typedef struct {
    char str[BUFFER_SIZE];
    void (*cmd)(void);
} auxcmd;

typedef struct {
    char str[BUFFER_SIZE];
    signalIndex sensor;
} getcmd;

typedef enum {
    allvoltages,
    alltemperatures,

    MAX_AUX
} auxIndex;


void echoChar(void);
void processChar(unsigned char character);
void push_tb(unsigned char character);
void pop_tb(void);
void displayPrompt(void);
void executeAUXCommand(unsigned char command[]);
void executeGETCommand(unsigned char command[], uint16_t argument);
void getSingleVoltageReading(uint8_t cell);
void getSingleTemperature(uint8_t cell);
void argumentParse(unsigned char charArray[]);
void arrayCleanup(void);


#endif /* INCLUDE_TESTINTERFACE_H_ */
