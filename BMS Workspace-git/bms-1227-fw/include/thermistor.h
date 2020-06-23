/*
 * thermistor.h
 *
 *  Created on: Jan 9, 2020
 *      Author: JASKANWAL SINGH
 */

#ifndef THERMISTOR_H_
#define THERMISTOR_H_


/*
#define MAX_TEMP_CHARGING 45
#define MAX_TEMP_RUNNING  60
*/


#include "sys_common.h"
#include "mibspi.h"
#include "sys_vim.h"
#include "sys_core.h"
#include "sci.h"
#include "temperature_yash.h"
#include "gio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "reg_het.h"
#include "het.h"
#include "Phantom_sci.h"
#include "pl455.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_timer.h"


#define TOTAL_MUXES 96
#define REFERENCE_VOLTAGE 3.0

/*TERMISTOR INTERFACE FUNCTIONS************************************************************************************************************************************/

/*Setting up mibSPI communication with the ADC*/

void setupThermistor();        //prepare the thermistor to start reading

/*Validating usage status (0/1)*/

uint8_t     validate_usage_status_thermistor(uint8_t status );      //Inquires whether the car is charging or running while the thermistor is measuring?

/*Reading Thermistor values*/  /*following three functions might require transitioning back to Manual mode*/

uint16_t    read_all_muxes_all_channels_thermistor();                                                       //reads and returns the thermistor values from all muxes on all channels
uint16_t    read_all_muxes_specific_channel_thermistor(uint8_t channel_identity);                           //reads and returns the thermistor values from all muxes on a specific channel
uint16_t    read_specific_mux_all_channels_thermistor(uint8_t mux_identity);                                //reads and returns the thermistor values from a specific mux on all the channels
uint16_t    read_specific_mux_specific_channel_thermistor(uint8_t mux_identity, uint8_t channel_identity);  //reads and returns the thermistor values from a specific mux on a specific channel
void thermistorRead();
void thermistorReadPrint();
void processThermistorState();
bool pollThermistorState(void);

/*Print ADC readings*/
void        parseThermistorRXBuffer();
void        printThermistorReadings(uint8 input);                   //prints using SCI

/*Converting Thermistor readings*/
void        convert_reading_thermistor ();        //converts thermistor reading into temperature

/*Structure for storing temperature and resistance values from the thermistor*/
typedef enum{
    THERMISTOR_GOOD, // Thermistor working properly
    THERMISTOR_LOST_COMS, // Communication dropped with thermistor board
    THERMISTOR_STARTUP, // Initialization state
    THERMISTOR_TEMPERATURE_FAULT, // Less than 3 cells over 60 degrees
    THERMISTOR_CRITICAL_FAULT // Shutdown tier fault
}thermistorState;

//typedef struct
//{
//    uint8 temperature;
//    bool temperature_flag;       // 0 or 1
//}thermistor_temperature_and_flag;

typedef struct
{
    uint8 cellTemp[TOTAL_MUXES];
    thermistorState State;
    uint8 totalFaults;
}thermistorStruct;

//thermistor_temperature_and_flag thermistor_temperature_and_flag_struct[TOTAL_MUXES];     //can be a double pointer

uint8 updateTemperature(uint8 mux);

/*Update mux*/

void update_mux(uint8 current_mux);
void update_input(uint8 current_input);

/*Reading faults*/

uint16_t    read_fault_thermistor();    //verifies if the operating temperatures are within acceptable limits

/*Fault Details*/

void        print_fault_details_thermistor();     //Identifies and prints details of usage status (0/1), mux and channel corresponding to the occurred fault
uint16_t    send_fault_signal_thermistor();       //flags a fault occurrence to the VCU and/or Dashboard or .....over SCI or something
uint16_t    send_fault_details_thermistor();      //sends fault details to the VCU and/or Dashboard or...... over SCI or something

/*mibspi Interrupts*/
//void mibspiGroupNotification(mibspiBASE_t *mibspi, uint32 group);
void getCellTemperatureArray(uint8* tempStruct);

#endif /* THERMISTOR_H_ */
