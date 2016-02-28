#include "motor.h"

// motor initial direction -- note: MUST be synced with ./pin_mapping
static unsigned char currentDir1 = INIT_MOTOR1_DIR;   
static unsigned char currentDir2 = INIT_MOTOR2_DIR;   

static int motorTestState = STOP;

bool motorTestLoop(char userChar) {
  
#if DEBUG
  Serial.print("Motor test input: "); Serial.println(userChar);
#endif
  
  switch(userChar){
    case 'W':
      analogWrite(MOTOR_EN1, MOTOR1_SPEED);
      analogWrite(MOTOR_EN2, MOTOR2_SPEED);
      digitalWrite(MOTOR_DIR1, FORWARD1);
      digitalWrite(MOTOR_DIR2, FORWARD2);
#if DEBUG
      Serial.println("Motor test: forward");
#endif
      break;
      
    case 'A':
      analogWrite(MOTOR_EN1, MOTOR1_SPEED);
      analogWrite(MOTOR_EN2, MOTOR2_SPEED);
      digitalWrite(MOTOR_DIR1, !FORWARD1);
      digitalWrite(MOTOR_DIR2, FORWARD2);
#if DEBUG
      Serial.println("Motor test: left");
#endif
      break;

    case 'S':
      analogWrite(MOTOR_EN1, MOTOR1_SPEED);
      analogWrite(MOTOR_EN2, MOTOR2_SPEED);
      digitalWrite(MOTOR_DIR1, !FORWARD1);
      digitalWrite(MOTOR_DIR2, !FORWARD2);
#if DEBUG
      Serial.println("Motor test: back");
#endif
      break;

    case 'D':
      analogWrite(MOTOR_EN1, MOTOR1_SPEED);
      analogWrite(MOTOR_EN2, MOTOR2_SPEED);
      digitalWrite(MOTOR_DIR1, FORWARD1);
      digitalWrite(MOTOR_DIR2, !FORWARD2);
#if DEBUG
      Serial.println("Motor test: right");
#endif
      break;

    case 'X':
      analogWrite(MOTOR_EN1, LOW);
      analogWrite(MOTOR_EN2, LOW);
#if DEBUG
      Serial.println("Motor test: stop");
#endif
      break;

    case 'Z':
      analogWrite(MOTOR_EN1, LOW);
      analogWrite(MOTOR_EN2, LOW);
#if DEBUG
      Serial.println("Motor test: finished");
#endif
      return true;
  }
  return false;
}







