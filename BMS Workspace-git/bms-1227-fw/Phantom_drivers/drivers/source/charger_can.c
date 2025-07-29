#include "charger_can.h"

void initialize_can(){
    // Initialize can
    canInit();

    // Update the id's of the CAN messgae boxes to match Charger CAN specification
    canUpdateID(canREG1, canMESSAGE_BOX1, BMS_TO_CHARGER_CAN_ID); // CAN TX
    canUpdateID(canREG1, canMESSAGE_BOX2, CHARGER_TO_BMS_CAN_ID); // CAN RX

}

void send_charger_limits(uint8 control){

    // Multiply by 10 for the 0.1 resoltution needed e.g. 320.1 should be 3201
    uint16 charger_voltage = (NUMBER_OF_CELLS * NOMINAL_CELL_VOLTAGE) * 10;  
    uint16 charger_current = (NUMBER_OF_STRINGS * CELL_CAHRGE_CURRENT) * 10;
    
    // Byte 5 (index 4) is the control, if it's 0, then start charging, if it's 1, stop charging
    uint8 tx_data[DATA_SIZE] = {
        (charger_voltage >> 8) & 0xFF,
        charger_voltage & 0xFF,
        (charger_current >> 8) & 0xFF,
        charger_current & 0xFF,
        control,
        0,
        0,
        0 };
    
    canTransmit(canREG1,  canMESSAGE_BOX1, tx_data);
}


bool get_charger_info(uint8* rx_data, uint8 size){
    // Makes sure there is enough size in the array --> return 0 if failure else
    if (size < 8){
        return false; 
    }

    canGetData(canREG1, canMESSAGE_BOX2, rx_data);
    return true;
}
