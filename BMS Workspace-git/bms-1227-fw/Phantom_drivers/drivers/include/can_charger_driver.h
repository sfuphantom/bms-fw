/*
 * can_driver.h
 *
 *  Created on: Jun 14, 2024
 *      Author: Mohnish Devarapalli
 */

#ifndef PHANTOM_DRIVERS_CAN_DRIVER_H_
#define PHANTOM_DRIVERS_CAN_DRIVER_H_


#define CAN_Protocol_num 1000;
#define CAN_baud_rate 250000;
#define CAN_cycle 1000;// in ms ei. 1 second cycle


int min_voltage = 2976; // 297.6 V
int max_voltage = 4032; // 403.2 V
int single_battery_max_protection_voltage = 4200; // 4.2 V
int single_battery_min_voltage = 3100; // 3.1 V
int single_battery_min_protection_voltage  = 3000; // 3V
int min_current = 0; // 0A
int max_current = 26; //2.6A
int max_discharge_current = 35;// 35A
int AH = 26; //amp hours 2.6 AH
int actual_charging_current = 0; // TODO find actual charging current, not in data sheet
int max_temp = 60-100;// 60 C
int min_temp = 0-100; // 0 C
int battery_numbers = 96; // 96 batteries
int id1 = 0x1806E6F4;


typedef struct can_message1{
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
    uint8_t byte4;
    uint8_t byte5;
    uint8_t byte6;
    uint8_t byte7;
    uint8_t byte8;
    uint32_t id;
}can_message1;



void charger_init(){

    can_message1 message_10;
    can_message1 message_11;
    can_message1 message_12;
    can_message1 message_13;
    can_message1 message_14;

    message_10.id = id1;
    message_11.id = id1;
    message_12.id = id1;
    message_13.id = id1;
    message_14.id = id1;

    message_10.byte1 = max_voltage & 0xFF00; //  Max Allowable Charging Terminal Voltage High Byte
    message_10.byte2 = max_voltage & 0x00FF; //Max Allowable Charging Terminal Voltage Low Byte
    message_10.byte3 = max_current & 0xFF00; //Max Allowable Charging Current High Byte
    message_10.byte4 = max_current & 0x00FF; //Max Allowable Charging Current Low Byte
    message_10.byte5 = 0; //Control
    message_10.byte6 = max_discharge_current;// Max allowable discharging current

    //TODO message 11 byte3 and byte4
    message_11.byte1 = AH & 0xFF00;//Battery Nominal AH High Byte
    message_11.byte2 = AH & 0x00FF;//Battery Nominal AH Low Byte
    message_11.byte3 = (AH - 1) & 0xFF00; // this is a guess as to the actual AH - Battery Actual AH High Byte -- AH-1 = AH - 0.1
    message_11.byte4 = (AH - 1) & 0x00FF; // this is a guess as to the actaul AH - Battery Actual AH Low Byte  -- AH-1 = AH - 0.1
    message_11.byte5 = single_battery_max_protection_voltage & 0xFF00;//Single Battery Max Protection Voltage High Byte
    message_11.byte6 = single_battery_max_protection_voltage & 0x00FF;//Single Battery Max Protection Voltage Low Byte
    message_11.byte7 = battery_numbers ;//

    message_12.byte1 = single_battery_max_protection_voltage & 0xFF00;//Single Battery Max Voltage High Byte
    message_12.byte2 = single_battery_max_protection_voltage & 0x00FF;//Single Battery Max Voltage Low Byte
    message_12.byte3 = single_battery_min_voltage  & 0xFF00;//Single Battery Min Voltage High Byte
    message_12.byte4 = single_battery_min_voltage  & 0x00FF;//Single Battery Min Voltage Low Byte
    message_12.byte5 = single_battery_min_protection_voltage  & 0xFF00;//Single Battery Min Protection Voltage High Byte
    message_12.byte6 = single_battery_min_protection_voltage  & 0x00FF;//Single Battery Min Protection Voltage Low Byte
    message_12.byte7 = 0x00;//Battery State

    //TODO byte3, byte4, byte5 for message 13
    message_13.byte1 = message_10.byte1; //Battery Pack Total Voltage High Byte
    message_13.byte2 = message_10.byte2;//Battery Pack Total Voltage Low Byte
    message_13.byte3 = actual_charging_current & 0xFF00;//Actual Charging Current High Byte
    message_13.byte4 = actual_charging_current & 0x00FF; // Actual Charging Current Low Byte
    message_13.byte5 =0 ;// TODO investigate SOC driver
    message_13.byte6 = max_temp;//Battery Max Temperature
    message_13.byte7 = min_temp;//Battery Min Temperature

    message_14.byte1 = battery_numbers  & 0xFF00; //Battery Numbers High Byte
    message_14.byte2 = battery_numbers  & 0x00FF;// Battery Nnumebrs Low Byte
};

#endif /* PHANTOM_DRIVERS_CAN_DRIVER_H_ */
