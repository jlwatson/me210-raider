#include "token_delivery.h"

bool tokenTestLoop(char userInput) { 
  ServoLeft.write(180);
  ServoRight.write(0);
  ServoMiddle.write(150);
  
  if(userInput != '*')
  {
    ServoLeft.write(90);   // Rotate servo counter clockwise
    ServoRight.write(90); 
    ServoMiddle.write(65);
    
    delay(2000);// Wait 2 seconds
    ServoLeft.write(180);     // Rotate servo clockwise
    ServoRight.write(0);
    ServoMiddle.write(135);
    return true;
  }  
  return false;
}

