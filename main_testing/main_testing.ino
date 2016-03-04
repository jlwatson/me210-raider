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

#define DEBUG false
#define BYPASS false

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

// PINS AND SERVOS
#define IR_INPUT     2
#define TAPE_BACK    6  // PWM
#define TAPE_FRONT   4
#define TAPE_RIGHT   9  // PWM
#define TAPE_LEFT    10  // PWM
#define MOTOR_DIR2   7
#define MOTOR_DIR1   8
#define MOTOR_EN1    5  // PWM
#define MOTOR_EN2    3  // PWM
#define SERVO_LEFT   11 // PWM
#define SERVO_MIDDLE 12
#define SERVO_RIGHT  13
#define FRONT_BUMPER A3
#define BACK_BUMPER  A4

#define INIT_MOTOR1_DIR LOW
#define INIT_MOTOR2_DIR HIGH

static Servo ServoLeft;
static Servo ServoRight;
static Servo ServoMiddle;

// IR TESTING
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

//BUMPER TESTING
bool bumperTestLoop();

//MOTOR TESTING
#define GO_FORWARD   0
#define GO_BACK      1
#define GO_LEFT      2
#define GO_RIGHT     3
#define STOP         4

#define FORWARD1     LOW   //high is forward for motor 1
#define FORWARD2     HIGH  //low is forward for motor 2
#define MOTOR1_SPEED 228
#define MOTOR2_SPEED 227

#define MOTOR1_BACK 236
#define MOTOR2_BACK 220

bool motorTestLoop(char userInput);

//SERVO TESTING
bool servoTestLoop(char userInput);

//TAPE TESTING
bool tapeTestLoop(char userInput);
unsigned char getState();
void printState(unsigned char state);

void setup() {
  Serial.begin(9600);
  Serial.println("========= Integrated Tests (ME210, The Raiders) =========");
  Serial.println("NOTE: TURN ON THE POWER SUPPLY");
  Serial.println("NOTE: CONNECT ARDUINO & SERVO GNDS");
  if(!BYPASS){
    
    pinMode(MOTOR_EN1, OUTPUT);
    pinMode(MOTOR_DIR1, OUTPUT);
    pinMode(MOTOR_EN2, OUTPUT);
    pinMode(MOTOR_DIR2, OUTPUT);
    
    // HIGH is forward for motor 1 (MOTOR_DIR1)
    // LOW is forward for motor 2 (MOTOR_DIR2)
  
    // note: MUST be synced with ./IR_sensor and ./motor
    digitalWrite(MOTOR_DIR1, INIT_MOTOR1_DIR); // set motors to turn cause why not
    digitalWrite(MOTOR_DIR2, INIT_MOTOR2_DIR);

    analogWrite(MOTOR_EN1, 0);
    analogWrite(MOTOR_EN2, 0);
  
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
    ServoMiddle.attach(SERVO_MIDDLE);
    ServoRight.attach(SERVO_RIGHT);
    ServoLeft.write(128);
    ServoRight.write(0);
    ServoMiddle.write(100);
  }

  Serial.println(TEST_MENU);
}

void doBypass(){

  analogWrite(MOTOR_EN1, 0);
  analogWrite(MOTOR_EN2, 0);
  Serial.print("IR: "); Serial.println(digitalRead(IR_INPUT));
  
}

void loop() {

  if(BYPASS){
    doBypass();
    
  }else{

    // Receive any user input
    String inputText = "";
    delay(100); 
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
    }else{
    
      switch(testState){
        case TEST_IR:
          if(irTestLoop()){
            Serial.println("IR test completed");
            testState = TEST_NONE;
            Serial.println(TEST_MENU);
          }
          break;
        case TEST_BUMPER:
          if(bumperTestLoop()){
            Serial.println("Bumper test completed");
            testState = TEST_NONE;
            Serial.println(TEST_MENU);
          }
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
  }
}

void parseCommand(char inputChar){
  
  switch(inputChar){
    case 'I':
      testState = TEST_IR;
      Serial.println("Rotating to find IR beacon.....");

      //motor initial speed setup
      analogWrite(MOTOR_EN1, MOTOR1_SPEED*.8);
      analogWrite(MOTOR_EN2, MOTOR2_SPEED*.8);
      digitalWrite(MOTOR_DIR1, HIGH);
      break;

    case 'B':
      testState = TEST_BUMPER;
      Serial.println("The bot will continue forwards until a bumper is triggered.");
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

/**************
 * IR TESTING *
 **************/

// motor initial direction -- note: MUST be synced with ./pin_mapping
static unsigned char irCurrentDir1 = INIT_MOTOR1_DIR;   //high is forward for motor 1
static unsigned char irCurrentDir2 = INIT_MOTOR2_DIR;   //low is forward for motor 2

static int irState = ROT_SEARCHING;

// returns true when test completes
bool irTestLoop() {
  switch(irState){
    case (ROT_SEARCHING):
      if(TestForFLAPBeacon()) {
        RespToFLAPBeacon();
        irState = ROT_ORI;
      }
      break;
      
    case (ROT_ORI):
      if(OriTimerExp()){
        RespToOriTimerExp();
        irState = ROT_SEARCHING;
        Serial.println("IR sensor detected beacon!");
        return true;
      }
      if(TestForLoseFLAP()){
        RespToLoseFLAP();
        irState = ROT_SEARCHING;
      }
      break;

    case (TO_BEACON):
      if(HitFLAPLine()){
          digitalWrite(MOTOR_EN2, LOW);
          digitalWrite(MOTOR_EN1, LOW);
      }
      break;    
  }
  return false;
}

unsigned char TestForFLAPBeacon(void){
  unsigned char EventOccurred;
  static unsigned char LastIRLevel = HIGH;
  unsigned char ThisIRLevel = digitalRead(IR_INPUT);
  Serial.println(ThisIRLevel);
  EventOccurred = ((ThisIRLevel == LOW) && (LastIRLevel == HIGH));
  LastIRLevel = ThisIRLevel;
  return EventOccurred;
}

unsigned char OriTimerExp(void){
  return (unsigned char)(TMRArd_IsTimerExpired(0));
}

unsigned char TestForLoseFLAP(void){
  unsigned char EventOccurred;
  static unsigned char LastIRLevelLose = LOW;
  unsigned char ThisIRLevelLose = digitalRead(IR_INPUT);
  EventOccurred = ((ThisIRLevelLose == HIGH) && (LastIRLevelLose == LOW));
  LastIRLevelLose = ThisIRLevelLose;
  return EventOccurred;
}

unsigned char HitFLAPLine(void){
  return false;
}
void RespToFLAPBeacon(void){
  TMRArd_InitTimer(0, 0);    //continue rotate for 200 milliseconds
}

void RespToOriTimerExp(void){
  digitalWrite(MOTOR_EN2, LOW);
  digitalWrite(MOTOR_EN1, LOW);  
#if DEBUG
  Serial.print("stop both motor");
#endif DEBUG
}

void RespToLoseFLAP(void){
  TMRArd_StopTimer(0);
}

/******************
 * BUMPER TESTING *
 ******************/
bool bumperTestLoop(){
  analogWrite(MOTOR_EN1, MOTOR1_SPEED);
  analogWrite(MOTOR_EN2, MOTOR2_SPEED);

  if(!digitalRead(FRONT_BUMPER) || !digitalRead(BACK_BUMPER)){
    analogWrite(MOTOR_EN1, 0);
    analogWrite(MOTOR_EN2, 0);
    return true;
  }
  return false;
}

/*****************
 * MOTOR TESTING *
 *****************/
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
      digitalWrite(MOTOR_DIR1, FORWARD1);
      digitalWrite(MOTOR_DIR2, !FORWARD2);
#if DEBUG
      Serial.println("Motor test: left");
#endif
      break;

    case 'S':
      analogWrite(MOTOR_EN1, MOTOR1_BACK);
      analogWrite(MOTOR_EN2, MOTOR2_BACK);
      digitalWrite(MOTOR_DIR1, !FORWARD1);
      digitalWrite(MOTOR_DIR2, !FORWARD2);
#if DEBUG
      Serial.println("Motor test: back");
#endif
      break;

    case 'D':
      analogWrite(MOTOR_EN1, MOTOR1_SPEED);
      analogWrite(MOTOR_EN2, MOTOR2_SPEED);
      digitalWrite(MOTOR_DIR1, !FORWARD1);
      digitalWrite(MOTOR_DIR2, FORWARD2);
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

/***************************
 * PINS/SERVO INIT TESTING *
 ***************************/
void initializePins(){

  
}

/*****************
 * SERVO TESTING *
 *****************/
bool servoTestLoop(char userInput) { 
  ServoLeft.write(128);
  ServoRight.write(0);
  ServoMiddle.write(100);
  
  if(userInput != '*')
  {
    
    ServoLeft.write(40);   // Rotate servo counter clockwise
    ServoRight.write(100); 
    ServoMiddle.write(65);
    
    delay(2000);// Wait 2 seconds
    ServoLeft.write(128);
    ServoRight.write(0);
    ServoMiddle.write(100);
    return true;
  }  
  return false;
}

/*****************
 * TAPE TESTING  *
 *****************/
unsigned char getState(){
  unsigned char result = digitalRead(TAPE_FRONT);
  result |= digitalRead(TAPE_LEFT) << 1;
  result |= digitalRead(TAPE_RIGHT) << 2;
  return result | digitalRead(TAPE_BACK) << 3;
}

/*
 * note: only use for debugging/testing; serial will slow down execution
 */
void printState(unsigned char state){

  Serial.println("-------- state --------");
  Serial.println("On tape = 1; Off tape = 0");
  Serial.print("Front: "); Serial.println(state & 1);
  Serial.print("Left: "); Serial.println((state & (1 << 1)) >> 1);
  Serial.print("Right: "); Serial.println((state & (1 << 2)) >> 2);
  Serial.print("Back: "); Serial.println((state & (1 << 3)) >> 3);
}

bool tapeTestLoop(char userChar) {

  if(userChar == 'X') return true;
  
  static unsigned char lastState = 0;
  unsigned char currentState = getState();

  if(currentState != lastState){
    printState(currentState);
    lastState = currentState;
  }
  return false;
}


