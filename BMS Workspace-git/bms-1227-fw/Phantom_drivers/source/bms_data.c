/**
 * bms_data.c
 * Author: Amneet Mann
 * Date: February 8, 2021
 */

#include "bms_data.h"

void initBMSData(bms_data* BMSDataPtr)
{
	/***********************************************************
     *              FLAG INITIALIZATION
     ***********************************************************/
	BMSDataPtr->Flags.BALANCE_EN = 0;
	BMSDataPtr->Flags.FUSE_FAULT = 0;
	BMSDataPtr->Flags.3SECOND_FLAG = 0;
	BMSDataPtr->Flags.BAD_SLAVE_CONNECTION_FLAG = 0;

	/***********************************************************
     *              DATA INITIALIZATION
     ***********************************************************/
	BMSDataPtr->Data.SOC = 0;
	BMSDataPtr->Data.remainingRunTime = 0;
	BMSDataPtr->Data.minimumCellVoltage = 5;
	BMSDataPtr->Data.state = BMS_RUNNING;

	/***********************************************************
     *              SLAVE VOLTAGE INITIALIZATION
     ***********************************************************/
	BMSDataPtr->SlaveVoltage.BMS_Slave_1 = {0};
	BMSDataPtr->SlaveVoltage.BMS_Slave_2 = {0};
	BMSDataPtr->SlaveVoltage.BMS_Slave_3 = {0};
	BMSDataPtr->SlaveVoltage.BMS_Slave_4 = {0};
}
