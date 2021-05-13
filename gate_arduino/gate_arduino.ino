int openGateSignalPin = 15; // schematic pin 23
int closeGateSignalPin = 14; // schematic pin 24

int motorOpen = 11; // schematic pin 17
int motorClose = 12; // schematic pin 18

int gateOpenedSensorPin = 7; // schematic pin 12
int gateClosedSensorPin = 6; // schematic pin 13
int overloadDetectionPin = 0; // schematic pin 2

int dataOutputPin = 18; // Schematic pin 27 used to send data back to Home Controller

int MOTOR_STOP_DELAY = 4000;
int OPEN_STATE = 1;
int CLOSE_STATE = 0;
int RELAX_STATE = 2;
int previousState = RELAX_STATE;


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(motorOpen, OUTPUT);
  pinMode(motorClose, OUTPUT);
  pinMode(dataOutputPin, OUTPUT);
  
  pinMode(openGateSignalPin, INPUT_PULLUP);
  pinMode(closeGateSignalPin, INPUT_PULLUP);
  pinMode(gateOpenedSensorPin, INPUT);
  pinMode(gateClosedSensorPin, INPUT);
  pinMode(overloadDetectionPin, INPUT);

  digitalWrite(motorOpen, LOW);
  digitalWrite(motorClose, LOW);
  digitalWrite(dataOutputPin, LOW);
  
  delay(10);
}

// the loop function runs over and over again forever
void loop() {
  
  int openGateButton = digitalRead(openGateSignalPin);
  int closeGateButton = digitalRead(closeGateSignalPin);

  // there is a pullup on the sensor. Turns low when opened or closed
  int isGateOpen = !digitalRead(gateOpenedSensorPin);
  int isGateClosed = !digitalRead(gateClosedSensorPin);

  //detect if motor is stuck
  int isMotorStuck = digitalRead(overloadDetectionPin);


  if (isMotorStuck){
    handleMotorStuck();
    return;
  }

  //Signal HC that motor is not stuck, and its ready
  handleMotorReady();
  
  
  if(openGateButton && !isGateOpen && !closeGateButton){
     
      openGate();
      previousState = OPEN_STATE;
  
  } else if(closeGateButton && !isGateClosed && !openGateButton){
  
      closeGate();
      previousState = CLOSE_STATE;
  
  } else {
    
      relaxGate();
      previousState = RELAX_STATE;  
  }

  delay(5);

}



void relaxGate(){
  digitalWrite(motorOpen, LOW);
  digitalWrite(motorClose, LOW);
  digitalWrite(LED_BUILTIN, LOW);
}



void closeGate(){
  if(previousState != CLOSE_STATE){
    relaxGate();
    delay(MOTOR_STOP_DELAY);
  } 
  digitalWrite(motorOpen, HIGH);
  digitalWrite(motorClose, LOW);
  digitalWrite(LED_BUILTIN, LOW);
}


void openGate(){
  if(previousState != OPEN_STATE){
    relaxGate();
    delay(MOTOR_STOP_DELAY);
  } 
  digitalWrite(motorOpen, LOW);
  digitalWrite(motorClose, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
}



void handleMotorStuck(){
  relaxGate();
  digitalWrite(dataOutputPin, HIGH);
  delay(10000);
}


void handleMotorReady(){
  digitalWrite(dataOutputPin, LOW);
}
