/*
 * checkoff
 * ===================
 * Code necessary to complete final checkoff requirements
 * 
 * The Raiders - ME210
 * 03/03/2016
 */

#include <Timers.h>   // For motor control
#include <Servo.h> // For servo-controlled token arms

#define DEBUG false
#define BYPASS false

#define TURN_TIME    0
#define T_TURN_DELAY 1000
#define BUCKET_DELAY 1000
#define BUCKET_FORWARD_TIME 300
#define LOAD_BACKUP_TIME 100
#define LOAD_TIME    3000

// Motor constants
#define FORWARD1     LOW   //high is forward for motor 1
#define FORWARD2     HIGH  //low is forward for motor 2

#define GO_FORWARD   0
#define GO_BACK      1
#define GO_LEFT      2
#define GO_RIGHT     3
#define STOP         4

#define MOTOR1_FORWARD 170
#define MOTOR2_FORWARD 170
#define MOTOR1_BACK    160
#define MOTOR2_BACK    160

#define BACKALL1       190
#define BACKALL2       173

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
#define FRONT_BUMPER A4
#define BACK_BUMPER  A5

// States
#define IR_SEEK_STATE 0
#define FORWARD_TO_TAPE_STATE 1
#define TURN_ONTO_LINE_STATE 2
#define LINE_FOLLOW_TO_CORNER_STATE 3
#define LEFT_TURN_STATE 4
#define LINE_FOLLOW_TO_T_STATE 5
#define RIGHT_TURN_STATE 6
#define LINE_FOLLOW_TO_BUCKET_STATE 7
#define BACK_STATE 8
#define LOADING_STATE 9
#define STOP_STATE 10

// IR SEEK states
#define   ROT_SEARCHING     0
#define   ROT_ORI           1
#define   TO_BEACON         2

// global state
static int global_state;

// Servos
static Servo ServoLeft;
static Servo ServoRight;
static Servo ServoMiddle;

// Bypass code for quick testing
void doBypass(){

  // turn motors off if necessary
  analogWrite(MOTOR_EN1, 0);
  analogWrite(MOTOR_EN2, 0);

 // Serial.println(digitalRead(IR_INPUT));
 Serial.print("Front: "); Serial.println(digitalRead(FRONT_BUMPER));
 Serial.print("Back: ");  Serial.println(digitalRead(BACK_BUMPER));
}

unsigned char getState();

bool detectTapeState(unsigned char current_state, unsigned char desired);
void stopAll();
void turnLeft(unsigned int t_delay);
void turnRight(unsigned int t_delay);
void goForward();
void goForward(int, int);
void goBack();

/******************
 * LINE_FOLLOWING *
 ******************/
#define FOLLOW_FORWARD 0
#define FOLLOW_LEFT    1
#define FOLLOW_RIGHT   2

unsigned char currentFollowState = FOLLOW_FORWARD;
void lineFollow(unsigned char tape_state);
void lineFollowBack(unsigned char tape_state);

bool bumperTriggered();
void deployTokens();
unsigned char getState();
bool tapeLeft(unsigned char tape_state);
bool tapeRight(unsigned char tape_state);
bool tapeFront(unsigned char tape_state);
bool tapeBack(unsigned char tape_state);

//// ****************************** ////

void setup() {

  Serial.begin(9600);

  if(!BYPASS){

    global_state = IR_SEEK_STATE;
    
    pinMode(MOTOR_EN1, OUTPUT);
    pinMode(MOTOR_DIR1, OUTPUT);
    pinMode(MOTOR_EN2, OUTPUT);
    pinMode(MOTOR_DIR2, OUTPUT);
    
    digitalWrite(MOTOR_DIR1, !FORWARD1); // set motors to turn initially for IR sensing
    digitalWrite(MOTOR_DIR2, FORWARD2);

    analogWrite(MOTOR_EN1, MOTOR1_FORWARD);
    analogWrite(MOTOR_EN2, MOTOR2_FORWARD);
  
    //IR sensor
    pinMode(IR_INPUT, INPUT);
    
    pinMode(TAPE_FRONT, INPUT_PULLUP);
    pinMode(TAPE_BACK, INPUT_PULLUP);
    pinMode(TAPE_LEFT, INPUT_PULLUP);
    pinMode(TAPE_RIGHT, INPUT_PULLUP);
  
    ServoLeft.attach(SERVO_LEFT);
    ServoMiddle.attach(SERVO_MIDDLE);
    ServoRight.attach(SERVO_RIGHT);

    // put servos into position (disabled to avoid tokens flying out)
    ServoLeft.write(128);
    ServoRight.write(0);
    ServoMiddle.write(120);
  }
}

void loop() {

  if(BYPASS) doBypass();
  else{

    int tape_state = getState();
    switch(global_state){
      case IR_SEEK_STATE:
          if(detectedIRBeacon()){
              stopAll();
              delay(1000);
              turnRight(0);
              goForward();
              
              global_state = FORWARD_TO_TAPE_STATE;
          }   
          break;
      case FORWARD_TO_TAPE_STATE:
          if(tapeFront(tape_state)){ // forward sensor only; add if necessary
              stopAll();
              delay(1000);
              turnRight(0);
              delay(5);
              goBack();
              delay(50);
              turnRight(0);
              global_state = TURN_ONTO_LINE_STATE;
          }        
          break;
      case TURN_ONTO_LINE_STATE:
          if(detectTapeState(tape_state, 0B1001)){
              stopAll();
              delay(1000);
              goForward();
              global_state = LINE_FOLLOW_TO_BUCKET_STATE;
              currentFollowState = FOLLOW_FORWARD;
          }       
          break;
      case LINE_FOLLOW_TO_BUCKET_STATE:
          if(bumperTriggered()){
              delay(300);
              stopAll();
              deployTokens();
              global_state = BACK_STATE;
              goBack(BACKALL1, BACKALL2);
              delay(500);
              currentFollowState = FOLLOW_FORWARD;
          }
          break;
      case BACK_STATE:
          //if(tapeLeft(tape_state) && tapeRight(tape_state)){
          if(tapeFront(tape_state) && !tapeRight(tape_state) && !tapeLeft(tape_state)){
              delay(LOAD_BACKUP_TIME);
              stopAll();
              global_state = LOADING_STATE;
          };
          break;
      case LOADING_STATE:
          delay(LOAD_TIME);
          goForward(MOTOR1_FORWARD-15, MOTOR2_FORWARD);
          delay(LOAD_BACKUP_TIME);
          global_state = LINE_FOLLOW_TO_BUCKET_STATE;
          break;
      case STOP_STATE:
          break;
    }
  }
} 

bool detectTapeState(unsigned char current_state, unsigned char desired){
    return current_state == desired;
}

void stopAll(){
    analogWrite(MOTOR_EN1, 0);
    analogWrite(MOTOR_EN2, 0);
}

void turnLeft(unsigned int t_delay){

    digitalWrite(MOTOR_DIR1, FORWARD1);
    digitalWrite(MOTOR_DIR2, !FORWARD2);

    delay(t_delay);

    analogWrite(MOTOR_EN1, 0);
    analogWrite(MOTOR_EN2, 0);

    delay(100);

    analogWrite(MOTOR_EN1, 255);
    analogWrite(MOTOR_EN2, -255);

    delay(300);

    analogWrite(MOTOR_EN1, MOTOR1_FORWARD);
    analogWrite(MOTOR_EN2, -MOTOR2_FORWARD); // turn left
}

void turnRight(unsigned int t_delay){

    digitalWrite(MOTOR_DIR1, !FORWARD1);
    digitalWrite(MOTOR_DIR2, FORWARD2);

    delay(t_delay);

    analogWrite(MOTOR_EN1, 0);
    analogWrite(MOTOR_EN2, 0);

    delay(100);

    analogWrite(MOTOR_EN1, -255);
    analogWrite(MOTOR_EN2, 255);

    delay(250);

    analogWrite(MOTOR_EN1, -MOTOR1_FORWARD);
    analogWrite(MOTOR_EN2, MOTOR2_FORWARD); // turn right 
}

void goForward(int m1, int m2){
    Serial.println("GOING FORWARD");
  
    digitalWrite(MOTOR_DIR1, FORWARD1);
    digitalWrite(MOTOR_DIR2, FORWARD2);

    analogWrite(MOTOR_EN1, 0);
    analogWrite(MOTOR_EN2, 0);

    delay(100);

    analogWrite(MOTOR_EN1, 255);
    analogWrite(MOTOR_EN2, 255);

    delay(200);

    analogWrite(MOTOR_EN1, m1);
    analogWrite(MOTOR_EN2, m2); // go forwards 
}

void goForward(){
    goForward(MOTOR1_FORWARD, MOTOR2_FORWARD);
}

void goBack(int m1, int m2){
    digitalWrite(MOTOR_DIR1, !FORWARD1);
    digitalWrite(MOTOR_DIR2, !FORWARD2);

    delay(100);

    analogWrite(MOTOR_EN1, 0);
    analogWrite(MOTOR_EN2, 0);

    delay(100);

    analogWrite(MOTOR_EN1, 255);
    analogWrite(MOTOR_EN2, 255);

    delay(100);

    analogWrite(MOTOR_EN1, m1);
    analogWrite(MOTOR_EN2, m2);
}

void goBack(){
    goBack(MOTOR1_BACK, MOTOR2_BACK);
}

/******************
 * LINE_FOLLOWING *
 ******************/
void lineFollow(unsigned char tape_state){

    if(currentFollowState == FOLLOW_FORWARD){
        if(tapeFront(tape_state)) return;
        else if(tapeLeft(tape_state)){
            currentFollowState = FOLLOW_LEFT;
            turnLeft(0);
        }else if(tapeRight(tape_state)){
            currentFollowState = FOLLOW_RIGHT;
            turnRight(0);
        } 
    }else if(currentFollowState == FOLLOW_LEFT){
        if(tapeFront(tape_state)){
            currentFollowState = FOLLOW_FORWARD;
            goForward();
        }
    }else if(currentFollowState == FOLLOW_RIGHT){
        if(tapeFront(tape_state)){
            currentFollowState = FOLLOW_FORWARD;
            goForward();
        }
    } 
}

void lineFollowBack(unsigned char tape_state){
    if(currentFollowState == FOLLOW_FORWARD){
        if(tapeBack(tape_state)) return;
        else if(tapeLeft(tape_state)){
            currentFollowState = FOLLOW_LEFT;
            turnRight(0);
        }else if(tapeRight(tape_state)){
            currentFollowState = FOLLOW_RIGHT;
            turnLeft(0);
        } 
    }else if(currentFollowState == FOLLOW_LEFT){
        if(tapeBack(tape_state)){
            currentFollowState = FOLLOW_FORWARD;
            goBack();
        }
    }else if(currentFollowState == FOLLOW_RIGHT){
        if(tapeBack(tape_state)){
            currentFollowState = FOLLOW_FORWARD;
            goBack();
        }
    } 
}
/*****************
 * IR_SEEK STATE *
 *****************/
 
bool detectedIRBeacon(){
  static int prevBeacon = 0;
  int reading = digitalRead(IR_INPUT);
  if(prevBeacon == 0 && reading == 1) delay(100);
  bool returnVal = (reading == 0 && prevBeacon == 1);
  prevBeacon = reading;
  return returnVal;
}

/******************
 * BUMPER TRIGGER *
 ******************/
bool bumperTriggered(){
    static int prevFront = 0;
    static int prevBack = 0;
    bool triggered = (!digitalRead(FRONT_BUMPER) && prevFront == 1) || (!digitalRead(BACK_BUMPER) && prevBack == 1);
    prevFront = digitalRead(FRONT_BUMPER);
    prevBack = digitalRead(BACK_BUMPER);
    return triggered;
}

/*****************
 * SERVO / TOKENS*
 *****************/
void deployTokens() { 
    
  ServoLeft.write(40);   // Rotate servo counter clockwise
  ServoRight.write(100); 
  ServoMiddle.write(65);
  
  delay(2000);// Wait 2 seconds

  ServoLeft.write(128);
  ServoRight.write(0);
  ServoMiddle.write(100);
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

bool tapeLeft(unsigned char tape_state){
    return (tape_state & 0B0010) >> 1;
}

bool tapeRight(unsigned char tape_state){
    return (tape_state & 0B0100) >> 2;
}

bool tapeFront(unsigned char tape_state){
    return tape_state & 0B0001;
}

bool tapeBack(unsigned char tape_state){
    return (tape_state & 0B1000) >> 3;
}
