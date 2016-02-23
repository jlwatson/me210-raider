/*
 * Initial line-following algorithm for the raider-bot, using front, left, and right sensors.
 * Currently 'simulates' commands to robot for use with manual box array of sensors.
 * jlwatson
 * Created 02/22/16
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
unsigned char readSensors(){
  
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

unsigned char getFront(unsigned char state){
  return state & 1;
}

unsigned char getLeft(unsigned char state){
  return (state & (1 << 1)) >> 1;
}

unsigned char getRight(unsigned char state){
  return (state & (1 << 2)) >> 2;
}

void loop() {
  
  static unsigned char lastSensorReading = 0;
  unsigned char currentReading = readSensors();

  if(currentReading != lastSensorReading){
    lastSensorReading = currentReading;

    if(getFront(currentReading)) Serial.println("Go forwards");
    else if(getLeft(currentReading)) Serial.println("Turn left");
    else if(getRight(currentReading)) Serial.println("Turn right");
  }
}
