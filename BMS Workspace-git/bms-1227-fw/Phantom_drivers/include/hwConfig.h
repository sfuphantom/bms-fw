/*
 * launchpadhw.h
 *
 *  Created on: Jul 13, 2020
 *      Author: Mahmoud Ahmed
 */

#ifndef PHANTOM_DRIVERS_INCLUDE_HWCONFIG_H_
#define PHANTOM_DRIVERS_INCLUDE_HWCONFIG_H_

//#define LAUNCHPAD_CONFIG        true
#define BMS_MASTER_CONFIG       true

// BMS Hardware Pins
#ifdef LAUNCHPAD_CONFIG

#define BMS_UART                sciREG
#define WAKEUP_PORT             gioPORTA
#define WAKEUP_PIN              0
#define FAULT_PORT              gioPORTA
#define FAULT_PIN               1

// Debug UART
#define PC_UART                 scilinREG

#endif

#ifdef BMS_MASTER_CONFIG

#define BMS_UART                scilinREG
#define WAKEUP_PORT             hetPORT1
#define WAKEUP_PIN              9
#define FAULT_PORT              hetPORT1
#define FAULT_PIN               25

// Debug UART
#define PC_UART                 sciREG


#endif

#endif /* PHANTOM_DRIVERS_INCLUDE_HWCONFIG_H_ */
