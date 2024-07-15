/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
///////////////////* USER CODE END */
#include "can.h"

/* Include Files */

#include "sys_common.h"
#include "system.h"

/* USER CODE BEGIN (1) */
///////////////////* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
#define D_SIZE 9
uint8_t tx_data[D_SIZE] = {'H', 'E', 'R', 'C', 'U', 'L', 'E', 'S', '\0'};
uint32_t checkPackets(uint8_t *src_packet, uint8_t *dst_packet, uint32_t psize);

///////////////////* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    //initialize can 1
    canInit();

    //transmit on can1
    canTransmit(canREG1, CANMESSAGE_BOX1, tx_data);

    //run forever
    while(1);
/////////////////////////* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
uint32_t checkPackets(uint8_t * src_packet, uint8_t* dst_packet, uint32_t psize){
    uint32_t err = 0;
    uint32_t cnt = psize;
    while(cnt--){
        if((*src_packet++) != (*dst_packet++)){
            err++;
        }
    }
    return (err);

}

// can interupt notification (Not used but must be provided)
void canMessageNotification(canBASE_t *node, uint32_t notification){
    return;

}

// can error notification (Not used by must be provided)
void canErrorNotification(canBASE_t *node, uint32_t notification)
{
    return;
}

// ESM group2 notification (not used but must be provided)
void esmGroup1Notification(unsigned channel){
    return;
}

// ESM group2 notification (not used but must be provided)
void esmGroup2Notification(unsigned channel){
    return;
}
///////////////////* USER CODE END */
