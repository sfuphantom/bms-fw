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


static unsigned char command;

static unsigned char charbuf[BUFFER_SIZE];

//------TEST COMMANDS----------
void getAllReadings(void);

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
};

void echoChar(void)
{
    /* Await further character */

        sciReceive(sciREG, 1,(unsigned char *)&command);
        processChar(command);

}

void processChar(unsigned char character)
{
    if(character == '\r')
    {
        executeCommand(charbuf);
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
    if(strcmp(&command[1], testCommands[0].str) == 0)
    {
        testCommands[0].cmd();
    }
    if(strcmp(&command[1], "what's up") == 0)
    {
        UARTprintf("\n\rnothin much how about you");
    }
    if(strcmp(&command[1], "just chillin mr tms570") == 0)
    {
        UARTprintf("\n\rdope");
    }
    if(strcmp(&command[1], "hotdog") == 0)
    {
        UARTprintf("\n\rMMMMmmmmm");
    }

    memset(command, 0, BUFFER_SIZE);
    testbuf.count = 1;
}

//--------TEST COMMANDS----------
void getAllReadings(void)
{
    getCurrentReadings();
}
