/*
 * main_testing
 * ===================
 * Unifies all sub-system testing code into one package, controlled
 * with serial commands.
 * - Token delivery: front dump and side arms
 * - Motor movement
 * - Bumpers
 * - IR beacon sensing (only 5kHz needed)
 * - Tape sensing
 * 
 * The Raiders - ME210
 * 02/27/2016
 */

#include <Timers.h>   // For motor control
#include <Servo.h> // For servo-controlled token arms

#include "pin_mapping.h"
#include "IR_sensor.h"
#include "motor.h"
#include "tape_sensor.h"
#include "servo.h"
#include "bumper.h"

#define DEBUG false

// Testing states
#define TEST_NONE   0
#define TEST_IR     1
#define TEST_BUMPER 2
#define TEST_MOTOR  3
#define TEST_TAPE   4
#define TEST_TOKEN  5

#define TEST_MENU "Choices: Test [I]R | Test [B]umper | Test [M]otor | Test [T]ape | Test Token [A]rms"

void parseCommand(String inputText);

// global testing state
static int testState = TEST_NONE;

void setup() {
  Serial.begin(9600);
  Serial.println("========= Integrated Tests (ME210, The Raiders) =========");
  Serial.println("NOTE: TURN ON THE POWER SUPPLY");
  Serial.println("NOTE: CONNECT ARDUINO & SERVO GNDS");
  initializePins(); // see ./pin_mapping

  Serial.println(TEST_MENU);
}

void loop() {

  // Receive any user input
  String inputText = ""; 
  while(Serial.available()){
    char c = Serial.read();
    inputText.concat(c);
  }

  char inputChar = '*';
  if(inputText.length() > 0){
    inputChar = inputText.charAt(0);
    inputChar = isLowerCase(inputChar) ? 'A' + (inputChar - 'a') : inputChar;
  }
  if(testState == TEST_NONE){
    parseCommand(inputChar);
  }
  
  switch(testState){
    case TEST_IR:
      if(irTestLoop()){
        Serial.println("IR test completed");
        testState = TEST_NONE;
        Serial.println(TEST_MENU);
      }
      break;
    case TEST_BUMPER:
      testState = TEST_NONE;
      Serial.println(TEST_MENU);
      break;
    case TEST_MOTOR:
      if(motorTestLoop(inputChar)){
        Serial.println("Motor test completed");
        testState = TEST_NONE;
        Serial.println(TEST_MENU);
      }
      break;
    case TEST_TAPE:
      if(tapeTestLoop(inputChar)){
        Serial.println("Tape test completed");
        testState = TEST_NONE;
        Serial.println(TEST_MENU);
      }
      break;
    case TEST_TOKEN:
      if(servoTestLoop(inputChar)){
        Serial.println("Token test completed");
        testState = TEST_NONE;
        Serial.println(TEST_MENU);
      }
      break;
  }
}

void parseCommand(char inputChar){
  
  switch(inputChar){
    case 'I':
      testState = TEST_IR;
      Serial.println("Rotating to find IR beacon.....");

      //motor initial speed setup
      analogWrite(MOTOR_EN1, 200);
      analogWrite(MOTOR_EN2, 150);
      break;

    case 'B':
      testState = TEST_BUMPER;
      Serial.println("Not implemented yet :|");
      break;

    case 'M':
      testState = TEST_MOTOR;
      Serial.println("Control motors with WASD, X to stop, Z to finish test");
      break;

    case 'T':
      testState = TEST_TAPE;
      Serial.println("Press 'X' to stop test");
      break;

    case 'A':
      testState = TEST_TOKEN;
      Serial.println("Press any key + <enter> to move arms");
      break;
  }
    
#if DEBUG
  Serial.print("Input received: ");
  Serial.println(inputChar);
#endif
  
}

