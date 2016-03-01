/*
 * Motor test
 * ----------------- 
 * mingyuw / jlwatson
 * Drives motors around using WASD keys (X to stop), Z to quit test
*/

#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <Arduino.h>
#include "pin_mapping.h"

#define GO_FORWARD   0
#define GO_BACK      1
#define GO_LEFT      2
#define GO_RIGHT     3
#define STOP         4

#define FORWARD1     HIGH //high is forward for motor 1
#define FORWARD2     HIGH  //low is forward for motor 2
#define MOTOR1_SPEED 200
#define MOTOR2_SPEED 150

bool motorTestLoop(char userInput);

#endif //__MOTOR_H__
