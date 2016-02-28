/*
 * IR Sensor Test
 * ----------------- 
 * mingyuw
 * Tests IR sensor functionality. Can rotate until IR sensor detects 5kHz signal, then stop.
*/

#ifndef __IR_SENSOR_H__
#define __IR_SENSOR_H__

#include <Timers.h>   // For motor control
#include "pin_mapping.h"

//3 states
#define   ROT_SEARCHING     0
#define   ROT_ORI           1
#define   TO_BEACON         2

bool irTestLoop();
unsigned char TestForFLAPBeacon(void);
unsigned char OriTimerExp(void);
unsigned char TestForLoseFLAP(void);
unsigned char HitFLAPLine(void);
void RespToFLAPBeacon(void);
void RespToOriTimerExp(void);
void RespToLoseFLAP(void);

#endif // __IR_SENSOR_H__
