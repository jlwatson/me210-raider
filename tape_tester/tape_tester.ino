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
 * a one-byte character where FRONT = 0th bit -> BACK = 3rd bit
 */
unsigned char getState(){
  
  unsigned char result = digitalRead(FRONT);
  result |= digitalRead(LEFT) << 1;
  result |= digitalRead(RIGHT) << 2;
  return result | digitalRead(BACK) << 3;
}

/*
 * note: only use for debugging; serial will slow down execution
 */
void printState(unsigned char state){

  Serial.println("-------- state --------");
  Serial.print("Front: "); Serial.println(state & 1);
  Serial.print("Left: "); Serial.println((state & (1 << 1)) >> 1);
  Serial.print("Right: "); Serial.println((state & (1 << 2)) >> 2);
  Serial.print("Back: "); Serial.println((state & (1 << 3)) >> 3);
}

void loop() {
  
  static unsigned char lastState = 0;
  unsigned char currentState = getState();

  if(currentState != lastState){
    printState(currentState);
    switch(currentState){
      //st: BRLF
      case B0000: Serial.println("Lost");                     break;
      case B0001: Serial.println("Tape in front");            break;
      case B0010: Serial.println("Tape to left");             break;
      case B0100: Serial.println("Tape to right");            break;
      case B1000: Serial.println("Tape in back");             break;
      case B0011: Serial.println("Inverse left turn corner"); break;
      case B0110: Serial.println("Crossing line");            break;
      case B1001: Serial.println("On line");                  break;
      case B0101: Serial.println("Inverse right turn corner");break;
      case B1010: Serial.println("Left turn corner");         break;
      case B1100: Serial.println("Right turn corner");        break;
      case B1011: Serial.println("Left T junction");          break;
      case B1101: Serial.println("Right T junction");         break;
      case B1110: Serial.println("Forward T junction");       break;
      case B0111: Serial.println("Backwards T junction");     break;
      case B1111: Serial.println("Cross junction");           break;
    }
    lastState = currentState;
  }
}
