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
static unsigned char getcmp[] = {'g', 'e', 't', '\0'};

//------TEST COMMANDS----------
void getAllReadings(void);
void getAllTemperatures(void);
void getSingleVoltageReading(uint8_t cell);

static volatile testBuffer testbuf = {
    0,
    0,
    charbuf,
    BUFFER_SIZE,
    0,
    sizeof(unsigned char)
};

auxcmd testAUXCommands[] =
{
 { .str = "allvoltages", .cmd = getAllReadings},
 { .str = "alltemperatures", .cmd = getAllTemperatures},
};

getcmd testGETCommands[] =
{
 { .str = "voltage", .sensor = voltage},
 { .str = "temperature", .sensor = temperature},
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


//--------AUX COMMANDS----------
void getAllReadings(void)
{
    getCurrentReadings();
}

void getAllTemperatures(void)
{

}

//--------GET COMMANDS----------
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

void getSingleTemperature(uint8_t cell)
{
    UARTprintf("Read cell temperature\n\r");
}

//---------ARGUMENT PARSING-----------
void argumentParse(unsigned char charArray[])
{
    unsigned char arg[10];
    unsigned char argnum[10];


    int i;
    for(i = 0; charArray[i+1] != ' ' && i < 20; i++)
    {
        command[i] = charArray[i+1];
    }

    i++;

    if(strcmp(&command[0], getcmp) == 0)
    {
        uint8_t j = 0;
        for(; charArray[i+1] != ' '; i++)
        {
            arg[j] = charArray[i+1];
            j++;
        }

        arg[j] = '\0';

        j = 0;

        for(; charArray[i+1] != '\0'; i++)
        {
            argnum[j] = charArray[i+1];
            j++;
        }

        uint16_t argint = atoi(argnum);
        executeGETCommand(arg, argint);
    }
    else
    {
        command[i] = '\0';
        executeAUXCommand(command);
    }
}

void executeAUXCommand(unsigned char command[])
{
    uint8_t i;

    for(i=0; i<MAX_AUX; i++)
    {
        if(strcmp(&command[0], testAUXCommands[i].str) == 0)
        {
            testAUXCommands[0].cmd();
        }
    }

    if(strcmp(&command[0], "hotdog") == 0)
    {
        UARTprintf("\n\rMMMMmmmmm");
    }

    arrayCleanup();
}

void executeGETCommand(unsigned char command[], uint16_t argument)
{
    uint8_t i;
    signalIndex signalRequest;

    for(i=0; i<MAX_AUX; i++)
    {
        if(strcmp(&command[0], testGETCommands[i].str) == 0)
        {
            signalRequest = testGETCommands[i].sensor;
        }
    }

    switch(signalRequest)
    {
        case voltage:
            getSingleVoltageReading(argument);
            break;
        case temperature:
            getSingleTemperature(argument);
            break;
        default:
            break;
    }

    arrayCleanup();
}

void arrayCleanup(void)
{
    memset(command, 0, BUFFER_SIZE);
    memset(charbuf, 0, BUFFER_SIZE);
    testbuf.count = 1;
}

