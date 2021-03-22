/**
 * bms_data.h
 * Author: Amneet Mann
 * Date: February 8, 2021
 */

#ifndef BMS_DATA_H
#define BMS_DATA_H

#include "sys_common.h"

typedef enum State_t
{
    BMS_RUNNING,
    BMS_CHARGING,
    BMS_FAULT
}State_t;

typedef struct flags
{
	uint8_t BALANCE_EN; // If 1, then cell balancing is enabled (only done during charging), if 0, then cell balancing is disabled
						// TODO: Set this = 1 if state machine evaluates to CHARGING state
	uint8_t FUSE_FAULT; // If 1, then fuse has blown, if 0, then fuses are intact
	uint8_t 3SECOND_FLAG; // If 1, then a cell has been in fault for longer than 3 seconds, else 0
						  // TODO: If 1, put BMS in FAULT state
	uint8_t TOTAL_CELL_ERROR_FLAG; // If 1, then more than four cells have been in fault during a check, else 0
								   // TODO: If 1, put BMS in FAULT state
	uint8_t BAD_SLAVE_CONNECTION_FLAG; // If 1, then BMS master is unable to read/write to/from at least one BMS slave
									   // TODO: If 1, put BMS in FAULT state
} flags;

typedef struct data
{
	float SOC; // State of charge estimation, expressed in % (i.e, 55.5% = 55.5)
	float remainingRunTime; // Remaining run time estimation, expressing in minutes

	double minimumCellVoltage; // Minimum voltage of all cells from most recent query
	State_t state; // The current state of the BMS
} data;

typedef struct voltage
{
	double BMS_Slave_1[CELLS_PER_BOARD]; // Voltages of all cells monitored by BMS slave 1 
    double BMS_Slave_2[CELLS_PER_BOARD]; // Voltages of all cells monitored by BMS slave 2
    double BMS_Slave_3[CELLS_PER_BOARD]; // Voltages of all cells monitored by BMS slave 3 
    double BMS_Slave_4[CELLS_PER_BOARD]; // Voltages of all cells monitored by BMS slave 4 
} voltage;

typedef struct bms_data
{
    flags Flags;
    data Data;
    voltage SlaveVoltage;
} bms_data;

void initBMSData(bms_data* BMSDataPtr);

#endif // BMS_DATA_H