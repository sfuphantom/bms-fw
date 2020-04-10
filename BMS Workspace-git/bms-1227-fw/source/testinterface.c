/*
 * testinterface.c
 *
 *  Created on: Apr 9, 2020
 *      Author: Mahmoud Ahmed
 */

#include "testinterface.h"
#include "sci.h"
#include "Phantom_sci.h"


static unsigned char command;

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
        UARTSend(sciREG, "\n\r");
    }
    else if(character == '\b')
    {
        UARTSend(sciREG, "\b");
        UARTSend(sciREG, " ");
        UARTSend(sciREG, "\b");
    }
    else{
        sciSend(sciREG, 1,(unsigned char *)&character);
    }
}


