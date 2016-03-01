#include "servo.h"

bool servoTestLoop(char userInput) { 
  ServoLeft.write(135);
  ServoRight.write(0);
  ServoMiddle.write(135);
  
  if(userInput != '*')
  {
    ServoLeft.write(30);   // Rotate servo counter clockwise
    ServoRight.write(105); 
    ServoMiddle.write(65);
    
    delay(2000);// Wait 2 seconds
    ServoLeft.write(135);     // Rotate servo clockwise
    ServoRight.write(0);
    ServoMiddle.write(135);
    return true;
  }  
  return false;
}

