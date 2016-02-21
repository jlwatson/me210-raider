/*
 * Tests cross-configuration IR LED/photoresistor pairs for tape detection.
 * jlwatson
 * Created 02/20/16
 */

#define FRONT 3
#define RIGHT 5
#define LEFT 6
#define BACK 9

void setup() {
  Serial.begin(9600);
  
  pinMode(FRONT, INPUT_PULLUP);
  digitalWrite(FRONT, HIGH);
  
  pinMode(BACK, INPUT_PULLUP);
  digitalWrite(BACK, HIGH);
  
  pinMode(LEFT, INPUT_PULLUP);
  digitalWrite(LEFT, HIGH);
  
  pinMode(RIGHT, INPUT_PULLUP);
  digitalWrite(RIGHT, HIGH);
}

/*
 * note: 0 - white floor, 1 - black tape marking. state is stored in 
 * a one-byte character where PIN_VAL_BIT = PIN_NUM - 3. (e.g. value of
 * FRONT sensor is pin 3 - 3 = bit 0.
 */
unsigned char getState(){
  
  unsigned char result = digitalRead(FRONT);
  result |= digitalRead(RIGHT) << 1;
  result |= digitalRead(LEFT) << 2;
  return result | digitalRead(BACK) << 3;
}

/*
 * note: only use for debugging; serial will slow down execution
 */
void printState(unsigned char state){

  Serial.println("-------- state --------");
  Serial.println(state);
  Serial.print("Front: "); Serial.println(state & 1);
  Serial.print("Right: "); Serial.println((state & (1 << 1)) >> 1);
  Serial.print("Left: "); Serial.println((state & (1 << 2)) >> 2);
  Serial.print("Back: "); Serial.println((state & (1 << 3)) >> 3);
}

void loop() {
  
  static unsigned char lastState = 0;
  unsigned char currentState = getState();
  
  if(currentState != lastState){
    printState(currentState);
    lastState = currentState;
  }
}
