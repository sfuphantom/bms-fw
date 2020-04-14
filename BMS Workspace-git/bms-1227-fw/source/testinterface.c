/*
 * testinterface.c
 *
 *  Created on: Apr 9, 2020
 *      Author: Mahmoud Ahmed
 */

#include "testinterface.h"
#include "sci.h"
#include "Phantom_sci.h"
#include "string.h"
#include "stdbool.h"
#include "sys_main.h"
#include "pl455.h"
#include <stdio.h>
#include "stdlib.h"


static unsigned char chr;

static unsigned char charbuf[BUFFER_SIZE];

static unsigned char command[BUFFER_SIZE];

static BYTE BMSArray[BMSByteArraySize*(TOTALBOARDS)];

uint16_t argint;

//------TEST COMMANDS----------
void getAllReadings(void);
void getSingleVoltageReading(uint8_t cell);

static volatile testBuffer testbuf = {
    0,
    0,
    charbuf,
    BUFFER_SIZE,
    0,
    sizeof(unsigned char)
};

testcmd testCommands[] =
{
 { .str = "allvoltages", .cmd = getAllReadings},
// { .str = "getvoltage", .cmd = getSingleVoltageReading},
};

void echoChar(void)
{
    /* Await further character */

        sciReceive(sciREG, 1,(unsigned char *)&chr);
        processChar(chr);

}

void processChar(unsigned char character)
{
    if(character == '\r')
    {
        argumentParse(charbuf);
        executeCommand(command);
        displayPrompt();
    }
    else if(character == '\b')
    {
        UARTprintf("\b");
        UARTprintf(" ");
        UARTprintf("\b");

        pop_tb();
    }
    else{
        push_tb(character);
        //sciSend(sciREG, 1,(unsigned char *)&character);
        UARTprintf("\r> ");
        UARTprintf((char *)&testbuf.buffer[1]);
    }
}

void push_tb(unsigned char character)
{

    //memcpy(testbuf.buffer + (testbuf.tail*testbuf.datasize), &character, testbuf.datasize);
    charbuf[testbuf.count] = character;
    //charbuf[testbuf.count + 1] = "\0";
    testbuf.tail = (testbuf.tail + 1) % testbuf.length;

    testbuf.count++;
}

void pop_tb(void)
{
    charbuf[testbuf.count] = '\x00';
    testbuf.count--;
}

void displayPrompt(void)
{
    UARTprintf("\n\r> ");
}

void executeCommand(unsigned char command[])
{
    if(strcmp(&command[0], testCommands[0].str) == 0)
    {
        testCommands[0].cmd();
    }
    if(strcmp(&command[0], "getvoltage") == 0)
    {
        getSingleVoltageReading(argint);
    }
    if(strcmp(&command[0], "just chillin mr tms570") == 0)
    {
        UARTprintf("\n\rdope");
    }
    if(strcmp(&command[0], "hotdog") == 0)
    {
        UARTprintf("\n\rMMMMmmmmm");
    }

    memset(command, 0, BUFFER_SIZE);
    memset(charbuf, 0, BUFFER_SIZE);
    testbuf.count = 1;
}

//--------TEST COMMANDS----------
void getAllReadings(void)
{
    getCurrentReadings();
}

void getSingleVoltageReading(uint8_t cell)
{
    char buf[50];

    getBMSSlaveArray(BMSArray);


    uint32 tempVal = BMSArray[cell + cell-1]*16*16 + BMSArray[cell + cell];
    double div = tempVal/65535.0; //FFFF
    double fin = div * 5.0;

    snprintf(buf, 40, "\n\rCell %d Voltage: %fV \n\r", cell, fin);
    UARTSend(sciREG, buf);
}

void argumentParse(unsigned char charArray[])
{
    unsigned char arg[10];
    int i;
    for(i = 0; charArray[i+1] != ' ' && i < 20; i++)
    {
        command[i] = charArray[i+1];
    }

    command[i] = '\0';
    i++;

    uint8_t j = 0;
    for(; charArray[i] != '\0'; i++)
    {
        arg[j] = charArray[i];
        j++;
    }
    argint = atoi(arg);
}
