/*
 * pin_mapping
 * ===================
 * Contains all the pin mappings and initialization code
 * (in a nice centralized location)
 * 
 * The Raiders - ME210
 * 02/27/2016
 */

#ifndef __PIN_MAPPING_H__
#define __PIN_MAPPING_H__

#include <Arduino.h>
#include <Servo.h>

#define IR_INPUT     2
#define MOTOR_EN2    3  // PWM
#define TAPE_FRONT   4
#define TAPE_RIGHT   5  // PWM
#define TAPE_LEFT    6  // PWM
#define MOTOR_DIR2   7
#define MOTOR_DIR1   8
#define MOTOR_EN1    9  // PWM
#define TAPE_BACK    10 // PWM
#define SERVO_LEFT   11 // PWM
#define SERVO_MIDDLE 12
#define SERVO_RIGHT  13

#define INIT_MOTOR1_DIR LOW
#define INIT_MOTOR2_DIR LOW

static Servo ServoLeft;
static Servo ServoRight;
static Servo ServoMiddle;

void initializePins();

#endif // __PIN_MAPPING_H__
