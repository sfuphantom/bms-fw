#ifndef CHARGER_CAN_H
#define CHARGER_CAN_H

#include "can.h"
#include "phantom_pl455.h"
#include "reg_can.h"

// CAN id's
#define BMS_TO_CHARGER_CAN_ID 0x1806E5F4
#define CHARGER_TO_BMS_CAN_ID 0x18FF50E5

// Cell Values
#define NUMBER_OF_CELLS 96
#define NUMBER_OF_STRINGS 5
#define NOMINAL_CELL_VOLTAGE 3.6
#define CELL_CAHRGE_CURRENT  2.6

// Data size in bytes
#define DATA_SIZE 8

#define START_CHARGING 0
#define STOP_CHARGING  1



void initialize_can();


void send_charger_limits(uint8 control);

// returns a pointer to a uint8 array
bool get_charger_info(uint8* rx_data, uint8 size);




#endif // CHARGER_CAN_H
