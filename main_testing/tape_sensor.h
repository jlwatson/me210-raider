/*
 * Tests cross-configuration IR LED/photoresistor pairs for tape detection.
 * jlwatson
 * Created 02/20/16
 */
 
/*
 * note: 0 - white floor, 1 - black tape marking. state is stored in 
 * a one-byte character where FRONT = 0th bit -> BACK = 3rd bit
 */

#ifndef __TAPE_SENSOR_H__
#define __TAPE_SENSOR_H__

#include <Arduino.h>
#include "pin_mapping.h"

bool tapeTestLoop(char userInput);
unsigned char getState();
void printState(unsigned char state);

#endif //__TAPE_SENSOR_H__
