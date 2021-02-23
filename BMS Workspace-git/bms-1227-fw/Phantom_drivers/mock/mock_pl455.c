#include <stdio.h>
#include "hal_stdtypes.h"

#include "pl455.h"

void WakePL455()
{
	return;
}

void CommClear(void)
{
	return;
}

void CommReset(void)
{
	return;
}

int  WriteReg(BYTE bID, uint16 wAddr, uint64 dwData, BYTE bLen, BYTE bWriteType)
{
	printf("In register %d, we have written %d.\n", wAddr, dwData);
	return 0;
}

int  ReadReg(BYTE bID, uint16 wAddr, void * pData, BYTE bLen, uint32 dwTimeOut)
{
	return 0;
}

void delayms(uint16 ms)
{
	return;
}