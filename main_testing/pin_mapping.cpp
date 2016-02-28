#include "pin_mapping.h"

void initializePins(){

  //motor ENABLE & DIRECTION pins setup
  pinMode(MOTOR_EN1, OUTPUT);
  pinMode(MOTOR_DIR1, OUTPUT);
  pinMode(MOTOR_EN2, OUTPUT);
  pinMode(MOTOR_DIR2, OUTPUT);
  
  // HIGH is forward for motor 1 (MOTOR_DIR1)
  // LOW is forward for motor 2 (MOTOR_DIR2)

  // note: MUST be synced with ./IR_sensor and ./motor
  digitalWrite(MOTOR_DIR1, INIT_MOTOR1_DIR); // set motors to turn cause why not
  digitalWrite(MOTOR_DIR2, INIT_MOTOR2_DIR);
  
  digitalWrite(MOTOR_EN1, LOW);
  digitalWrite(MOTOR_EN2, LOW);

  //IR sensor
  pinMode(IR_INPUT, INPUT);
  
  pinMode(TAPE_FRONT, INPUT_PULLUP);
  pinMode(TAPE_BACK, INPUT_PULLUP);
  pinMode(TAPE_LEFT, INPUT_PULLUP);
  pinMode(TAPE_RIGHT, INPUT_PULLUP);
  //  digitalWrite(FRONT, HIGH);
  //  digitalWrite(BACK, HIGH);
  //  digitalWrite(LEFT, HIGH);
  //  digitalWrite(RIGHT, HIGH);

  ServoLeft.attach(SERVO_LEFT);
  ServoRight.attach(SERVO_MIDDLE);
  ServoMiddle.attach(SERVO_RIGHT);
}

