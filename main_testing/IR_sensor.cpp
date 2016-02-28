#include "IR_sensor.h"

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
        irState = TO_BEACON;
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
  TMRArd_InitTimer(0, 200);    //continue rotate for 200 milliseconds
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

