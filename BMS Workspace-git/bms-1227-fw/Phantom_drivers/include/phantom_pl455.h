/*
 * phantom_pl455.h
 *
 *  Created on: Apr 18, 2020
 *      Author: Mahmoud Ahmed
 */

#ifndef INCLUDE_PHANTOM_PL455_H_
#define INCLUDE_PHANTOM_PL455_H_

#include "datatypes.h"
#include "stdbool.h"
#include "pl455.h"
#include "hal_stdtypes.h"
#include "stdint.h"


#define BMSByteArraySize  43
#define CELLS_PER_BOARD   10

typedef enum{
    BMS_INIT,
    BMS_OPERATIONAL,
    BMS_FAULT,
    BMS_CRITICAL_FAULT
}BMS_STATE;

typedef struct BMS_FLAGS{
    bool CELL_OVERVOLTAGE_FLAG[16];
    bool CELL_UNDERVOLTAGE_FLAG[16];
    bool CELL_HIGH_TEMPRATURE_FLAG[16];
    uint16_t CELL_VOLTAGE_ERROR_COUNTER[16];
    uint8_t TOTAL_CELL_ERROR_COUNTER;
    bool TOTAL_CELL_ERROR_FLAG;
    bool CELL_3SECOND_FLAG;

    double cellVoltageLow;
} BMS_FLAGS;

typedef struct BMS_ARRAYS{
    double BMS_Slave_1[CELLS_PER_BOARD];
    double BMS_Slave_2[CELLS_PER_BOARD];
    double BMS_Slave_3[CELLS_PER_BOARD];
    double BMS_Slave_4[CELLS_PER_BOARD];
} BMS_ARRAYS;

BMS_STATE bms_process();
void bms_operational();
void bms_init(void);
void BMS_Read_Single(uint8_t device);
void BMS_Read_Single_NP(uint8_t device);
void BMS_Read_All(void);
void BMS_Read_All_NP();
void BMS_Read_All_NP_SIM();
void BMS_processState(void);
void BMS_Slaves_Heartbeat(void);
void BMS_Balance();
void BMS_Balance_SIM();
void getCurrentReadings(void);
void getBMSSlaveArray(BYTE BMSArray[BMSByteArraySize*(TOTALBOARDS)]);

#endif /* INCLUDE_PHANTOM_PL455_H_ */
