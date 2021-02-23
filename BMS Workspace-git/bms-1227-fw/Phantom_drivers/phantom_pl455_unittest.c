/*
 * phantom_pl455_unittest.c
 *
 * Unit tests for BMS slaves.
 * Created on: Feb 21, 2021
 * Author: Amneet Mann
 */

#include <stdio.h>
#include "phantom_pl455.h"

extern BMS_ARRAYS BMS_Voltages;
extern BMS_FLAGS BMS;

/* Testing BMS_Balance function */
void testBMSBalance(void)
{
	// Set-up
	double voltageArray[7] = {3, 4, 3.55, 4.3, 3.2, 4.1, 3.62};
	int count = 0;
	BMS.cellVoltageLow = 3.6;
	for (int i = 0; i<10; i++)
	{
		BMS_Voltages.BMS_Slave_1[i] = voltageArray[count%7];
		count++;
		BMS_Voltages.BMS_Slave_2[i] = voltageArray[count%7];
		count++;
		BMS_Voltages.BMS_Slave_3[i] = voltageArray[count%7];
		count++;
		BMS_Voltages.BMS_Slave_4[i] = voltageArray[count%7];
		count++;
	}

	// Execute and evaluate result through mock WriteReg() print statements
	// Unit test passed if registers are written with the following values: 588, 403, 612, 153
	BMS_Balance();

	return;
}

int main()
{
	// testBMSBalance();

	return 0;
}