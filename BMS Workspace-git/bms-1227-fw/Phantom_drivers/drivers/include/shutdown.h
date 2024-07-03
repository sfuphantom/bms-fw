#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#define PIN_LEVEL_ON 1
#define BMS_FLT_PIN 5

void shutdownHV(void);
extern BMSState_t BMSState;

#endif
