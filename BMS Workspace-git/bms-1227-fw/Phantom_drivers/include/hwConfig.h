/*
 * launchpadhw.h
 *
 *  Created on: Jul 13, 2020
 *      Author: Mahmoud Ahmed
 */

#ifndef PHANTOM_DRIVERS_INCLUDE_HWCONFIG_H_
#define PHANTOM_DRIVERS_INCLUDE_HWCONFIG_H_

// Define config depending on MCU Board
//#define LAUNCHPAD_CONFIG        true
#define BMS_MASTER_CONFIG       true

// BMS Slave Hardware Pins
#ifdef LAUNCHPAD_CONFIG

#define BMS_UART                sciREG
#define WAKEUP_PORT             gioPORTA
#define WAKEUP_PIN              0
#define FAULT_PORT              gioPORTA
#define FAULT_PIN               1

// Debug UART
#define PC_UART                 scilinREG

// Thermistor Board Pins
#define THERMISTOR_SPI                  mibspiREG3
#define THERMISTOR_INIT_TRANSFER_GROUP  TransferGroup0
#define THERMISTOR_COMM_TRANSFER_GROUP  TransferGroup1
#define THERMISTOR_MUX_PORT             gioPORTA
#define THERMISTOR_MUX_PIN_A            5
#define THERMISTOR_MUX_PIN_B            6
#define THERMISTOR_MUX_PIN_C            7

#endif

#ifdef BMS_MASTER_CONFIG

#define BMS_UART                scilinREG
#define WAKEUP_PORT             hetPORT1
#define WAKEUP_PIN              9
#define FAULT_PORT              hetPORT1
#define FAULT_PIN               25

// Debug UART
#define PC_UART                 sciREG

// Thermistor Board Pins
#define THERMISTOR_SPI                  mibspiREG3
#define THERMISTOR_INIT_TRANSFER_GROUP  TransferGroup0
#define THERMISTOR_COMM_TRANSFER_GROUP  TransferGroup1
#define THERMISTOR_MUX_PORT             hetPORT1
#define THERMISTOR_MUX_PIN_A            4
#define THERMISTOR_MUX_PIN_B            5
#define THERMISTOR_MUX_PIN_C            7

#endif

#endif /* PHANTOM_DRIVERS_INCLUDE_HWCONFIG_H_ */
