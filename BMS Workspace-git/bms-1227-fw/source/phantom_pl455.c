/*
 * phantom_pl455.c
 *
 *  Created on: Apr 18, 2020
 *      Author: Mahmoud Ahmed
 */
#include "sys_main.h"
#include "pl455.h"
#include "phantom_pl455.h"
#include "gio.h"
#include "sci.h"
#include "can.h"
#include "reg_het.h"
#include "Phantom_sci.h"
#include "testinterface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatypes.h"
#include "sys_vim.h"
#include "sys_core.h"

static uint16 HB_LED = 0;
static const uint8 TOTALCELLS = 10;
static const uint8 TOTALAUX = 8;

BYTE  SingleSlaveReading[BMSByteArraySize];
BYTE  MultipleSlaveReading[BMSByteArraySize*(TOTALBOARDS)];

volatile BMS_FLAGS BMS = { {0}, {0}, {0}, {0}, 0, false, false };
BMS_ARRAYS BMS_Voltages = {{0},{0},{0},{0}};


void BMS_init(){
        int nSent, nRead, nTopFound = 0;
        int nDev_ID, nGrp_ID;
        uint32_t  wTemp = 0;
        unsigned char command;

        _enable_IRQ();
        gioInit();
        sciInit();
        while ((scilinREG->FLR & 0x4) == 4);

        _enable_interrupt_();
        canInit();
        canEnableErrorNotification(canREG1);

        sciEnableNotification(scilinREG, SCI_RX_INT);

        char buf[50];

        snprintf(buf, 29, "log: WakeBit:%d FaultBit:%d\n\r", gioGetBit(hetPORT1, 9), gioGetBit(hetPORT1, 25));
        UARTSend(sciREG, buf);
        WakePL455();
        snprintf(buf, 29, "log: WakeBit:%d FaultBit:%d\n\r", gioGetBit(hetPORT1, 9), gioGetBit(hetPORT1, 25));
        UARTSend(sciREG, buf);



        CommClear();
        CommReset();

        for(nDev_ID = 0; nDev_ID < TOTALBOARDS>>1; nDev_ID++) {
                nSent = WriteReg(nDev_ID, 12, 0x40, 1, FRMWRT_ALL_NR);  // send out broadcast pwrdown command
                delayms(5); //~5ms
                WakePL455();
                delayms(5); //~5ms
            }


        delayms(100);

            // Mask Customer Checksum Fault bit
            nSent = WriteReg(0, 107, 0x8000, 2, FRMWRT_ALL_NR); // clear all fault summary flags

            // Clear all faults
            nSent = WriteReg(0, 82, 0xFFC0, 2, FRMWRT_ALL_NR);      // clear all fault summary flags
            nSent = WriteReg(0, 81, 0x38, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register
    //for(;;){
            // Auto-address all boards (section 1.2.2)
            nSent = WriteReg(0, 14, 0x19, 1, FRMWRT_ALL_NR); // set auto-address mode on all boards
            nSent = WriteReg(0, 12, 0x08, 1, FRMWRT_ALL_NR); // enter auto address mode on all boards, the next write to this ID will be its address

            // Set addresses for all boards in daisy-chain (section 1.2.3)
            for (nDev_ID = 0; nDev_ID < TOTALBOARDS; nDev_ID++)
            {
                nSent = WriteReg(nDev_ID, 10, nDev_ID, 1, FRMWRT_ALL_NR); // send address to each board
            }

            // Enable all communication interfaces on all boards in the stack (section 1.2.1)
            nSent = WriteReg(0, 16, 0x10F8, 2, FRMWRT_ALL_NR);  // set communications baud rate and enable all interfaces on all boards in stack

        delayms(1);

        /* Set communications interfaces appropriately for their position in the stack, and
         * for baud rate used in the application (set by BAUDRATE define in pl455.h).
         * (section 1.2.4)
         */

        switch(BAUDRATE)
            {
            case 125000:
                nSent = WriteReg(0, 16, 0x00F8, 2, FRMWRT_ALL_NR);  // set communications baud rate and enable all interfaces
                delayms(1);
                sciSetBaudrate(scilinREG, BAUDRATE);
                break;
            case 250000:
                delayms(1);
                break;
            case 500000:
                nSent = WriteReg(0, 16, 0x20F8, 2, FRMWRT_ALL_NR);  // set communications baud rate and enable all interfaces
                delayms(1);
                sciSetBaudrate(scilinREG, BAUDRATE);
                break;
            case 1000000:
                nSent = WriteReg(0, 16, 0x30F8, 2, FRMWRT_ALL_NR);  // set communications baud rate and enable all interfaces
                delayms(1);
                sciSetBaudrate(scilinREG, BAUDRATE);
                break;
            }
    //}
       for (nDev_ID = TOTALBOARDS - 1; nDev_ID >= 0; --nDev_ID)
       {

                //nRead = ReadReg(nDev_ID, 10, &wTemp, 1, 0); // 0ms timeout
                //delayms(100);

            nRead = ReadReg(nDev_ID, 10, &wTemp, 1, 0); // 0ms timeout


            if(nRead == 0) // if nothing is read then this board doesn't exist
                nTopFound = 0;
            else // a response was received
            {
                if(nTopFound == 0)
                { // if the last board was not present but this one is, this is the top board



                    if(nDev_ID == 0) // this is the only board
                    {
                        switch(BAUDRATE)
                            {
                            case 125000:
                                nSent = WriteReg(nDev_ID, 16, 0x0080, 2, FRMWRT_SGL_NR);    // enable only single-end comm port on board
                                break;
                            case 250000:
                                nSent = WriteReg(nDev_ID, 16, 0x1080, 2, FRMWRT_SGL_NR);    // enable only single-end comm port on board
                                break;
                            case 500000:
                                nSent = WriteReg(nDev_ID, 16, 0x2080, 2, FRMWRT_SGL_NR);    // enable only single-end comm port on board
                                break;
                            case 1000000:
                                nSent = WriteReg(nDev_ID, 16, 0x3080, 2, FRMWRT_SGL_NR);    // enable only single-end comm port on board
                                break;
                            }
                    }
                    else // this is the top board of a stack (section 1.2.5)
                    {
                        switch(BAUDRATE)
                        {
                        case 125000:
                            nSent = WriteReg(nDev_ID, 16, 0x0028, 2, FRMWRT_SGL_NR);    // enable only comm-low and fault-low for the top board
                            break;
                        case 250000:
                            nSent = WriteReg(nDev_ID, 16, 0x1028, 2, FRMWRT_SGL_NR);    // enable only comm-low and fault-low for the top board
                            break;
                        case 500000:
                            nSent = WriteReg(nDev_ID, 16, 0x2028, 2, FRMWRT_SGL_NR);    // enable only comm-low and fault-low for the top board
                            break;
                        case 1000000:
                            nSent = WriteReg(nDev_ID, 16, 0x3028, 2, FRMWRT_SGL_NR);    // enable only comm-low and fault-low for the top board
                            break;
                        }
                        nTopFound = 1;
                    }
                }
                else // this is a middle or bottom board
                {
                    if(nDev_ID == 0) // this is a bottom board of a stack (section 1.2.6)
                    {
                        switch(BAUDRATE)
                        {
                        case 125000:
                            nSent = WriteReg(nDev_ID, 16, 0x00D0, 2, FRMWRT_SGL_NR);    // enable comm-high, fault-high and single-end comm port on bottom board
                            break;
                        case 250000:
                            nSent = WriteReg(nDev_ID, 16, 0x10D0, 2, FRMWRT_SGL_NR);    // enable comm-high, fault-high and single-end comm port on bottom board
                            break;
                        case 500000:
                            nSent = WriteReg(nDev_ID, 16, 0x20D0, 2, FRMWRT_SGL_NR);    // enable comm-high, fault-high and single-end comm port on bottom board
                            break;
                        case 1000000:
                            nSent = WriteReg(nDev_ID, 16, 0x30D0, 2, FRMWRT_SGL_NR);    // enable comm-high, fault-high and single-end comm port on bottom board
                            break;
                        }
                    }
                    else // this is a middle board
                    {
                        switch(BAUDRATE)
                        {
                        case 125000:
                            nSent = WriteReg(nDev_ID, 16, 0x0078, 2, FRMWRT_SGL_NR);    // enable comm-high, fault-high, comm-low and fault-low on all middle boards
                            break;
                        case 250000:
                            nSent = WriteReg(nDev_ID, 16, 0x1078, 2, FRMWRT_SGL_NR);    // enable comm-high, fault-high, comm-low and fault-low on all middle boards
                            break;
                        case 500000:
                            nSent = WriteReg(nDev_ID, 16, 0x2078, 2, FRMWRT_SGL_NR);    // enable comm-high, fault-high, comm-low and fault-low on all middle boards
                            break;
                        case 1000000:
                            nSent = WriteReg(nDev_ID, 16, 0x3078, 2, FRMWRT_SGL_NR);    // enable comm-high, fault-high, comm-low and fault-low on all middle boards
                            break;
                        }
                    }
                }
            }
        }

        // Clear all faults (section 1.2.7)
        nSent = WriteReg(0, 82, 0xFFC0, 2, FRMWRT_ALL_NR); // clear all fault summary flags
        nSent = WriteReg(0, 81, 0x38, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register

        delayms(10);

        //while(1);
        //TESTING POINT #1-----------------------------------------------------------------------------------------------------------------------

        // Configure AFE (section 2.2.1)-------------------------------------------------------------------------------------------------

        nDev_ID = 0;
        nSent = WriteReg(nDev_ID, 60, 0x00, 1, FRMWRT_SGL_NR); // set 0 mux delay
        nSent = WriteReg(nDev_ID, 61, 0x00, 1, FRMWRT_SGL_NR); // set 0 initial delay

        // Configure voltage and internal sample period (section 2.2.2)
        nDev_ID = 0;
        nSent = WriteReg(nDev_ID, 62, 0xCC, 1, FRMWRT_SGL_NR); // set 99.92us ADC sampling period

        // Configure the oversampling rate (section 2.2.3)
        nDev_ID = 0;
        nSent = WriteReg(nDev_ID, 7, 0x00, 1, FRMWRT_SGL_NR); // set no oversampling period

        // Clear and check faults (section 2.2.4)
        nDev_ID = 0;
        nSent = WriteReg(nDev_ID, 81, 0x38, 1, FRMWRT_SGL_NR); // clear fault flags in the system status register
        nSent = WriteReg(nDev_ID, 82, 0xFFC0, 2, FRMWRT_SGL_NR); // clear all fault summary flags
        nRead = ReadReg(nDev_ID, 81, &wTemp, 1, 0); // 0ms timeout
        nRead = ReadReg(nDev_ID, 82, &wTemp, 2, 0); // 0ms timeout

        // Select number of cells and channels to sample (section 2.2.5.1)
        nDev_ID = 0;

        nSent = WriteReg(nDev_ID, 13, 0x0A, 1, FRMWRT_ALL_NR); // set number of cells to 16
        nSent = WriteReg(nDev_ID, 3, 0x03FFFFC0, 4, FRMWRT_ALL_NR); // select all cell, all AUX channe1s, internal digital die and internal analog die temperatures

        // Set cell over-voltage and cell under-voltage thresholds on a single board (section 2.2.6.1)

        nDev_ID = 0;
        nSent = WriteReg(nDev_ID, 144, 0xD70A, 2, FRMWRT_SGL_NR); // set OV threshold = 4.2000V
        nSent = WriteReg(nDev_ID, 142, 0xA3D6, 2, FRMWRT_SGL_NR); // set UV threshold = 3.2000V

        nSent = WriteReg(nDev_ID, 120, 0x3F, 1, FRMWRT_ALL_NR); // set GPIO direction for GPIO4 and GPIO[2:0] as outputs, GPIO3 and GPIO5 as inputs
        nSent = WriteReg(nDev_ID, 19, 0x08, 1, FRMWRT_ALL_NR);

        UARTprintf("\n\rBATTERY MANAGEMENT SYSTEM INITIALIZED\n\n\r");
        sciReceive(sciREG, 1, (unsigned char *)&command);
        displayPrompt();
}

void Thermistor_Read(void)
{
    UARTSend(sciREG, "Thermistor read \n\r");
}

void BMS_Slaves_Heartbeat(void){
    int nSent = 0;
    if(HB_LED == 0)
    {
        nSent = WriteReg(0, 121, 0x00, 1, FRMWRT_ALL_NR); // set GPIO4 and GPIO1, clear GPIO2 and GPIO0
        HB_LED = 1;
    }
    else
    {
        nSent = WriteReg(0, 121, 0x02, 1, FRMWRT_ALL_NR);
        HB_LED = 0;
    }
}

void BMS_Read_All(){
        int nSent = 0;

        char buf[100];

        nSent = WriteReg(0, 2, TOTALBOARDS-1, 1, FRMWRT_ALL_R); // send sync sample command



        sciReceive(scilinREG, BMSByteArraySize*TOTALBOARDS, MultipleSlaveReading); //1 header, 32x2 cells, 2x16 AUX, 4 dig die, 4 ana die, 2 CRC

        delayms(5); // for the tms to record all the data first


        BMS.cellVoltageLow = 5;
        uint8 j;
        sint8 i;
        uint8 cellCount = TOTALCELLS*TOTALBOARDS;
        uint8 x = 9;
        uint8 voltageLoopCounter = cellCount*2+1;
        uint8 auxLoopCounter = voltageLoopCounter + TOTALAUX*2;
        for (i = TOTALBOARDS-1; i > -1; i--){
            for (j = 0; j < voltageLoopCounter; j++) {
                if (j == 0) {
                     snprintf(buf, 30, "Header -> Decimal: %d, Hex: %X\n\n", MultipleSlaveReading[j+BMSByteArraySize*i], MultipleSlaveReading[j+BMSByteArraySize*i]);
                     UARTSend(sciREG, buf);
                     UARTSend(sciREG, "\n\r");
                     continue;
                }


                uint32 tempVal = MultipleSlaveReading[j+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[j+1+BMSByteArraySize*i];
                double div = tempVal/65535.0; //FFFF
                double fin = div * 5.0;

                if(i == 0){
                        BMS_Voltages.BMS_Slave_1[x] = fin;
                        if(fin < BMS.cellVoltageLow)
                        {
                            BMS.cellVoltageLow = fin;
                        }
                }
                if(i == 1){
                        BMS_Voltages.BMS_Slave_2[x] = fin;
                }
                if(i == 2){
                        BMS_Voltages.BMS_Slave_3[x] = fin;
                }
                if(i == 3){
                        BMS_Voltages.BMS_Slave_4[x] = fin;
                }

               // if(fin < BMS.cellVoltageLow)
               // {
                 //   BMS.cellVoltageLow = fin;
                //}



                snprintf(buf, 40, "Cell %d: Hex: %X %X Voltage: %fV \n\r", cellCount, MultipleSlaveReading[j+BMSByteArraySize*i], MultipleSlaveReading[j+1+BMSByteArraySize*i], fin);
                UARTSend(sciREG, buf);
                UARTSend(sciREG, "\n\r");

                if(fin > 4.2){
                    snprintf(buf, 20, "Cell %d Overvoltage\n\r", cellCount);
                    UARTSend(sciREG, buf);
                    UARTSend(sciREG, "\n\r");

                    BMS.CELL_OVERVOLTAGE_FLAG[cellCount] = true;
                    BMS.TOTAL_CELL_ERROR_COUNTER++;
                }
                else if(fin < 3.2){
                    snprintf(buf, 21, "Cell %d Undervoltage\n\r", cellCount);
                    UARTSend(sciREG, buf);
                    UARTSend(sciREG, "\n\r");

                    BMS.CELL_UNDERVOLTAGE_FLAG[cellCount] = true;
                    BMS.TOTAL_CELL_ERROR_COUNTER++;
                }

                if(BMS.CELL_OVERVOLTAGE_FLAG[cellCount] == true || BMS.CELL_UNDERVOLTAGE_FLAG[cellCount] == true){
                    BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount]++;
                }
                else{
                    BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount] = 0;
                }

                if(BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount] > 300){
                    BMS.CELL_3SECOND_FLAG = true;
                }

                cellCount--;
                j++;
                x--;
             }
            x = 9;
        }

             if(BMS.TOTAL_CELL_ERROR_COUNTER > 4){
                 BMS.TOTAL_CELL_ERROR_FLAG = true;
             }

             snprintf(buf, 26, "NUMBER OF CELL ERRORS: %d\n\r", BMS.TOTAL_CELL_ERROR_COUNTER);
             UARTSend(sciREG, buf);
             UARTSend(sciREG, "\n\r");

             BMS.TOTAL_CELL_ERROR_COUNTER = 0;

             uint8 auxCount = TOTALAUX*TOTALBOARDS-1;
         for (i = TOTALBOARDS-1; i > -1; i--){
             for (j = voltageLoopCounter; j < auxLoopCounter; j++) {
                 int tempVal = MultipleSlaveReading[j+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[j+1+BMSByteArraySize*i];
                 double div = tempVal/65535.0; //FFFF
                 double fin = div * 5.0;

                 double resistance = 10000*(fin/(4.56-fin));

                 snprintf(buf, 46, "AUX %d: Hex: %X %X Voltage: %fV Resistance: %f Ohms\n\n\r", auxCount, MultipleSlaveReading[j+BMSByteArraySize*i], MultipleSlaveReading[j+1+BMSByteArraySize*i], fin, resistance);
                 UARTSend(sciREG, buf);
                 UARTSend(sciREG, "\n\r");
                 j++;
                 auxCount--;
             }

             double digDieTemp = ((((MultipleSlaveReading[auxLoopCounter+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[auxLoopCounter+1+BMSByteArraySize*i])/65535.0)*5) - 2.287) * 131.944;
             snprintf(buf, 50, "Digital Die: Hex: %X %X Temp: %f degrees C\n\r", MultipleSlaveReading[auxLoopCounter+BMSByteArraySize*i], MultipleSlaveReading[auxLoopCounter+1+BMSByteArraySize*i], digDieTemp);
             UARTSend(sciREG, buf);
             UARTSend(sciREG, "\n\r");

             double anaDieTemp = ((((MultipleSlaveReading[auxLoopCounter+2+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[auxLoopCounter+3+BMSByteArraySize*i])/65535.0)*5) - 1.8078) * 147.514;
             snprintf(buf, 49, "Analog Die: Hex: %X %X Temp: %f degrees C\n\n\r", MultipleSlaveReading[auxLoopCounter+2+BMSByteArraySize*i], MultipleSlaveReading[auxLoopCounter+3+BMSByteArraySize*i], anaDieTemp);
             UARTSend(sciREG, buf);
             UARTSend(sciREG, "\n\r");

             }
}

void BMS_Read_All_NP(){
        int nSent = 0;

        nSent = WriteReg(0, 2, TOTALBOARDS-1, 1, FRMWRT_ALL_R); // send sync sample command

        sciReceive(scilinREG, BMSByteArraySize*TOTALBOARDS, MultipleSlaveReading); //1 header, 32x2 cells, 2x16 AUX, 4 dig die, 4 ana die, 2 CRC

        delayms(5); // for the tms to record all the data first


        BMS.cellVoltageLow = 5;
        uint8 j;
        sint8 i;
        uint8 cellCount = TOTALCELLS;
        uint8 voltageLoopCounter = cellCount*2+1;
        uint8 auxLoopCounter = voltageLoopCounter + TOTALAUX*2;
        for (i = TOTALBOARDS-1; i > -1; i--){
            for (j = 0; j < voltageLoopCounter; j++) {
                if (j == 0) {
                     continue;
                }


                uint32 tempVal = MultipleSlaveReading[j+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[j+1+BMSByteArraySize*i];
                double div = tempVal/65535.0; //FFFF
                double fin = div * 5.0;

                if(i == 0){
                        BMS_Voltages.BMS_Slave_1[cellCount-1] = fin;
                }
                if(i == 1){
                        BMS_Voltages.BMS_Slave_2[cellCount-1] = fin;
                }
                if(i == 2){
                        BMS_Voltages.BMS_Slave_3[cellCount-1] = fin;
                }
                if(i == 3){
                        BMS_Voltages.BMS_Slave_4[cellCount-1] = fin;
                }

                if(fin < BMS.cellVoltageLow)
                {
                    BMS.cellVoltageLow = fin;
                }


                if(fin > 4.2){

                    //BMS.CELL_OVERVOLTAGE_FLAG[cellCount] = true;
                    //BMS.TOTAL_CELL_ERROR_COUNTER++;
                }
                else if(fin < 3.2){

                    //BMS.CELL_UNDERVOLTAGE_FLAG[cellCount] = true;
                    //BMS.TOTAL_CELL_ERROR_COUNTER++;
                }

                if(BMS.CELL_OVERVOLTAGE_FLAG[cellCount] == true || BMS.CELL_UNDERVOLTAGE_FLAG[cellCount] == true){
                    //BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount]++;
                }
                else{
                    //BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount] = 0;
                }

                if(BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount] > 300){
                    BMS.CELL_3SECOND_FLAG = true;
                }

                cellCount--;
                j++;
             }
            cellCount = TOTALCELLS;
        }

        BMS.cellVoltageLow = 3.8;
             if(BMS.TOTAL_CELL_ERROR_COUNTER > 4){
                 BMS.TOTAL_CELL_ERROR_FLAG = true;
             }

             BMS.TOTAL_CELL_ERROR_COUNTER = 0;

             uint8 auxCount = TOTALAUX*TOTALBOARDS-1;
         for (i = TOTALBOARDS-1; i > -1; i--){
             for (j = voltageLoopCounter; j < auxLoopCounter; j++) {
                 int tempVal = MultipleSlaveReading[j+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[j+1+BMSByteArraySize*i];
                 double div = tempVal/65535.0; //FFFF
                 double fin = div * 5.0;

                 double resistance = 10000*(fin/(4.56-fin));

                 j++;
                 auxCount--;
             }

             double anaDieTemp = ((((MultipleSlaveReading[auxLoopCounter+2+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[auxLoopCounter+3+BMSByteArraySize*i])/65535.0)*5) - 1.8078) * 147.514;
             }
}

void BMS_Read_Single(uint8_t device){
    int nSent = 0;

    char buf[100];

    nSent = WriteReg(device, 2, 0x02, 1, FRMWRT_SGL_R); // send sync sample command

    sciReceive(scilinREG, 42, SingleSlaveReading); //1 header, 16x2 cells, 2x8 AUX, 2 dig die, 2 ana die, 2 CRC

    delayms(10); // for the tms to record all the data first

    snprintf(buf, 30, "Device number %d \n\r", device);
    UARTSend(sciREG, buf);

    uint8 j;
    uint8 cellCount = 10;
    for (j = 0; j < 21; j++) {
        if (j == 0) {
             snprintf(buf, 30, "Header -> Decimal: %d, Hex: %X\n\n", SingleSlaveReading[j], SingleSlaveReading[j]);
             UARTSend(sciREG, buf);
             UARTSend(sciREG, "\n\r");
             continue;
        }


        uint32 tempVal = SingleSlaveReading[j]*16*16 + SingleSlaveReading[j+1];
        double div = tempVal/65535.0; //FFFF
        double fin = div * 5.0;


        snprintf(buf, 40, "Cell %d: Hex: %X %X Voltage: %fV \n\r", cellCount, SingleSlaveReading[j], SingleSlaveReading[j+1], fin);
        UARTSend(sciREG, buf);
        UARTSend(sciREG, "\n\r");

        if(fin > 4.2){
            snprintf(buf, 20, "Cell %d Overvoltage\n\r", cellCount);
            UARTSend(sciREG, buf);
            UARTSend(sciREG, "\n\r");

            BMS.CELL_OVERVOLTAGE_FLAG[cellCount] = true;
            BMS.TOTAL_CELL_ERROR_COUNTER++;
        }
        else if(fin < 3.2){
            snprintf(buf, 21, "Cell %d Undervoltage\n\r", cellCount);
            UARTSend(sciREG, buf);
            UARTSend(sciREG, "\n\r");

            BMS.CELL_UNDERVOLTAGE_FLAG[cellCount] = true;
            BMS.TOTAL_CELL_ERROR_COUNTER++;
        }

        if(BMS.CELL_OVERVOLTAGE_FLAG[cellCount] == true || BMS.CELL_UNDERVOLTAGE_FLAG[cellCount] == true){
            BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount]++;
        }
        else{
            BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount] = 0;
        }

        if(BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount] > 300){

            BMS.CELL_3SECOND_FLAG = true;
        }

        cellCount--;
        j++;
     }

     if(BMS.TOTAL_CELL_ERROR_COUNTER > 4){
         BMS.TOTAL_CELL_ERROR_FLAG = true;
     }

     snprintf(buf, 26, "NUMBER OF CELL ERRORS: %d\n\r", BMS.TOTAL_CELL_ERROR_COUNTER);
     UARTSend(sciREG, buf);
     UARTSend(sciREG, "\n\r");

     BMS.TOTAL_CELL_ERROR_COUNTER = 0;

     uint8 auxCount = 7;
     for (j = 21; j < 37; j++) {
         int tempVal = SingleSlaveReading[j]*16*16 + SingleSlaveReading[j+1];
         double div = tempVal/65535.0; //FFFF
         double fin = div * 5.0;

         double resistance = 10000*(fin/(4.56-fin));

         snprintf(buf, 46, "AUX %d: Hex: %X %X Voltage: %fV Resistance: %f Ohms\n\n\r", auxCount, SingleSlaveReading[j], SingleSlaveReading[j+1], fin, resistance);
         UARTSend(sciREG, buf);
         UARTSend(sciREG, "\n\r");
         j++;
         auxCount--;
     }

     double digDieTemp = ((((SingleSlaveReading[37]*16*16 + SingleSlaveReading[38])/65535.0)*5) - 2.287) * 131.944;
     snprintf(buf, 50, "Digital Die: Hex: %X %X Temp: %f degrees C\n\r", SingleSlaveReading[37], SingleSlaveReading[38], digDieTemp);
     UARTSend(sciREG, buf);
     UARTSend(sciREG, "\n\r");

     double anaDieTemp = ((((SingleSlaveReading[39]*16*16 + SingleSlaveReading[40])/65535.0)*5) - 1.8078) * 147.514;
     snprintf(buf, 49, "Analog Die: Hex: %X %X Temp: %f degrees C\n\n\r", SingleSlaveReading[39], SingleSlaveReading[40], anaDieTemp);
     UARTSend(sciREG, buf);
     UARTSend(sciREG, "\n\r");
}

void BMS_Read_Single_NP(uint8_t device){
    int nSent = 0;

    nSent = WriteReg(device, 2, 0x02, 1, FRMWRT_SGL_R); // send sync sample command

    sciReceive(scilinREG, 55, SingleSlaveReading); //1 header, 16x2 cells, 2x8 AUX, 2 dig die, 2 ana die, 2 CRC

    delayms(10); // for the tms to record all the data first

    //snprintf(buf, 30, "Device number %d", device);
    //UARTSend(sciREG, buf);

    uint8 j;
    uint8 cellCount = 16;
    for (j = 0; j < 33; j++) {
        if (j == 0) {
             continue;
        }


        uint32 tempVal = SingleSlaveReading[j]*16*16 + SingleSlaveReading[j+1];
        double div = tempVal/65535.0; //FFFF
        double fin = div * 5.0;


        if(fin > 4.2){
            BMS.CELL_OVERVOLTAGE_FLAG[cellCount] = true;
            BMS.TOTAL_CELL_ERROR_COUNTER++;
        }
        else if(fin < 3.2){
            BMS.CELL_UNDERVOLTAGE_FLAG[cellCount] = true;
            BMS.TOTAL_CELL_ERROR_COUNTER++;
        }

        if(BMS.CELL_OVERVOLTAGE_FLAG[cellCount] == true || BMS.CELL_UNDERVOLTAGE_FLAG[cellCount] == true){
            BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount]++;
        }
        else{
            BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount] = 0;
        }

        if(BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount] > 300){

            BMS.CELL_3SECOND_FLAG = true;
        }

        cellCount--;
        j++;
     }

     if(BMS.TOTAL_CELL_ERROR_COUNTER > 4){
         BMS.TOTAL_CELL_ERROR_FLAG = true;
     }

     BMS.TOTAL_CELL_ERROR_COUNTER = 0;

     uint8 auxCount = 7;
     for (j = 33; j < 49; j++) {
         int tempVal = SingleSlaveReading[j]*16*16 + SingleSlaveReading[j+1];
         double div = tempVal/65535.0; //FFFF
         double fin = div * 5.0;

         double resistance = 10000*(fin/(4.56-fin));

         j++;
         auxCount--;
     }

     double digDieTemp = ((((SingleSlaveReading[49]*16*16 + SingleSlaveReading[50])/65535.0)*5) - 2.287) * 131.944;

     double anaDieTemp = ((((SingleSlaveReading[51]*16*16 + SingleSlaveReading[52])/65535.0)*5) - 1.8078) * 147.514;
}

void BMS_Balance()
{
    int nSent = 0;
    uint8_t i;
    double voltageDiff;
    char buf[50];
    uint16 CellBalanceMask1 = 0x0000;
    uint16 CellBalanceMask2 = 0x0000;
    uint16 CellBalanceMask3 = 0x0000;
    uint16 CellBalanceMask4 = 0x0000;

    for(i = 0; i < 10; i++)
    {
        voltageDiff = BMS_Voltages.BMS_Slave_1[i] - BMS.cellVoltageLow;;
        if(voltageDiff >= 0.1)
        {
            CellBalanceMask1 = CellBalanceMask1 | (uint32)((uint32)1U << i);
        }

        voltageDiff = BMS_Voltages.BMS_Slave_2[i] - BMS.cellVoltageLow;;
        if(voltageDiff >= 0.01)
        {
            CellBalanceMask2 = CellBalanceMask2 | (uint32)((uint32)1U << i);
        }

        voltageDiff = BMS_Voltages.BMS_Slave_3[i] - BMS.cellVoltageLow;;
        if(voltageDiff >= 0.1)
        {
            CellBalanceMask3 = CellBalanceMask3 | (uint32)((uint32)1U << i);
        }

        voltageDiff = BMS_Voltages.BMS_Slave_4[i] - BMS.cellVoltageLow;;
        if(voltageDiff >= 0.1)
        {
            CellBalanceMask4 = CellBalanceMask4 | (uint32)((uint32)1U << i);
        }
    }

        nSent = WriteReg(1, 20, CellBalanceMask1, 2, FRMWRT_SGL_NR );
        nSent = WriteReg(0, 20, CellBalanceMask2, 2, FRMWRT_SGL_NR );
        nSent = WriteReg(2, 20, CellBalanceMask3, 2, FRMWRT_SGL_NR );
        nSent = WriteReg(3, 20, CellBalanceMask4, 2, FRMWRT_SGL_NR );

        snprintf(buf, 50, "Cell bitmask: %d\n\r", CellBalanceMask1);
        UARTSend(sciREG, buf);

}

void getCurrentReadings(void)
{
            char buf[100];


            BMS.cellVoltageLow = 5;
            uint8 j;
            sint8 i;
            uint8 cellCount = TOTALCELLS*TOTALBOARDS;
            uint8 x = 9;
            uint8 voltageLoopCounter = cellCount*2+1;
            uint8 auxLoopCounter = voltageLoopCounter + TOTALAUX*2;
            for (i = TOTALBOARDS-1; i > -1; i--){
                for (j = 0; j < voltageLoopCounter; j++) {
                    if (j == 0) {
                         snprintf(buf, 30, "Header -> Decimal: %d, Hex: %X\n\n", MultipleSlaveReading[j+BMSByteArraySize*i], MultipleSlaveReading[j+BMSByteArraySize*i]);
                         UARTSend(sciREG, buf);
                         UARTSend(sciREG, "\n\r");
                         continue;
                    }


                    uint32 tempVal = MultipleSlaveReading[j+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[j+1+BMSByteArraySize*i];
                    double div = tempVal/65535.0; //FFFF
                    double fin = div * 5.0;

                    if(i == 0){
                            BMS_Voltages.BMS_Slave_1[x] = fin;
                            if(fin < BMS.cellVoltageLow)
                            {
                                BMS.cellVoltageLow = fin;
                            }
                    }
                    if(i == 1){
                            BMS_Voltages.BMS_Slave_2[x] = fin;
                    }
                    if(i == 2){
                            BMS_Voltages.BMS_Slave_3[x] = fin;
                    }
                    if(i == 3){
                            BMS_Voltages.BMS_Slave_4[x] = fin;
                    }

                   // if(fin < BMS.cellVoltageLow)
                   // {
                     //   BMS.cellVoltageLow = fin;
                    //}



                    snprintf(buf, 40, "Cell %d: Hex: %X %X Voltage: %fV \n\r", cellCount, MultipleSlaveReading[j+BMSByteArraySize*i], MultipleSlaveReading[j+1+BMSByteArraySize*i], fin);
                    UARTSend(sciREG, buf);
                    UARTSend(sciREG, "\n\r");

                    if(fin > 4.2){
                        snprintf(buf, 20, "Cell %d Overvoltage\n\r", cellCount);
                        UARTSend(sciREG, buf);
                        UARTSend(sciREG, "\n\r");

                        BMS.CELL_OVERVOLTAGE_FLAG[cellCount] = true;
                        BMS.TOTAL_CELL_ERROR_COUNTER++;
                    }
                    else if(fin < 3.2){
                        snprintf(buf, 21, "Cell %d Undervoltage\n\r", cellCount);
                        UARTSend(sciREG, buf);
                        UARTSend(sciREG, "\n\r");

                        BMS.CELL_UNDERVOLTAGE_FLAG[cellCount] = true;
                        BMS.TOTAL_CELL_ERROR_COUNTER++;
                    }

                    if(BMS.CELL_OVERVOLTAGE_FLAG[cellCount] == true || BMS.CELL_UNDERVOLTAGE_FLAG[cellCount] == true){
                        BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount]++;
                    }
                    else{
                        BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount] = 0;
                    }

                    if(BMS.CELL_VOLTAGE_ERROR_COUNTER[cellCount] > 300){
                        BMS.CELL_3SECOND_FLAG = true;
                    }

                    cellCount--;
                    j++;
                    x--;
                 }
                x = 9;
            }

                 if(BMS.TOTAL_CELL_ERROR_COUNTER > 4){
                     BMS.TOTAL_CELL_ERROR_FLAG = true;
                 }

                 snprintf(buf, 26, "NUMBER OF CELL ERRORS: %d\n\r", BMS.TOTAL_CELL_ERROR_COUNTER);
                 UARTSend(sciREG, buf);
                 UARTSend(sciREG, "\n\r");

                 BMS.TOTAL_CELL_ERROR_COUNTER = 0;

                 uint8 auxCount = TOTALAUX*TOTALBOARDS-1;
             for (i = TOTALBOARDS-1; i > -1; i--){
                 for (j = voltageLoopCounter; j < auxLoopCounter; j++) {
                     int tempVal = MultipleSlaveReading[j+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[j+1+BMSByteArraySize*i];
                     double div = tempVal/65535.0; //FFFF
                     double fin = div * 5.0;

                     double resistance = 10000*(fin/(4.56-fin));

                     snprintf(buf, 46, "AUX %d: Hex: %X %X Voltage: %fV Resistance: %f Ohms\n\n\r", auxCount, MultipleSlaveReading[j+BMSByteArraySize*i], MultipleSlaveReading[j+1+BMSByteArraySize*i], fin, resistance);
                     UARTSend(sciREG, buf);
                     UARTSend(sciREG, "\n\r");
                     j++;
                     auxCount--;
                 }

                 double digDieTemp = ((((MultipleSlaveReading[auxLoopCounter+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[auxLoopCounter+1+BMSByteArraySize*i])/65535.0)*5) - 2.287) * 131.944;
                 snprintf(buf, 50, "Digital Die: Hex: %X %X Temp: %f degrees C\n\r", MultipleSlaveReading[auxLoopCounter+BMSByteArraySize*i], MultipleSlaveReading[auxLoopCounter+1+BMSByteArraySize*i], digDieTemp);
                 UARTSend(sciREG, buf);
                 UARTSend(sciREG, "\n\r");

                 double anaDieTemp = ((((MultipleSlaveReading[auxLoopCounter+2+BMSByteArraySize*i]*16*16 + MultipleSlaveReading[auxLoopCounter+3+BMSByteArraySize*i])/65535.0)*5) - 1.8078) * 147.514;
                 snprintf(buf, 49, "Analog Die: Hex: %X %X Temp: %f degrees C\n\n\r", MultipleSlaveReading[auxLoopCounter+2+BMSByteArraySize*i], MultipleSlaveReading[auxLoopCounter+3+BMSByteArraySize*i], anaDieTemp);
                 UARTSend(sciREG, buf);
                 UARTSend(sciREG, "\n\r");

                 }

}

void getBMSSlaveArray(BYTE BMSArray[BMSByteArraySize*(TOTALBOARDS)])
{
    memcpy(&BMSArray[0], &MultipleSlaveReading[0], BMSByteArraySize*(TOTALBOARDS)*sizeof(BYTE));
}
