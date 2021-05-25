#include<avr/wdt.h>

int openGateSignalPin = 15; // schematic pin 23
int closeGateSignalPin = 14; // schematic pin 24

int motorOpen = 11; // schematic pin 17
int motorClose = 12; // schematic pin 18

int gateOpenedSensorPin = 7; // schematic pin 12
int gateClosedSensorPin = 6; // schematic pin 13
int overloadDetectionPin = 0; // schematic pin 2

int dataOutputPin = 18; // Schematic pin 27 used to send data back to Home Controller

int MOTOR_STOP_DELAY = 4000;

enum STATE {
  OPEN,
  CLOSE,
  RELAX
};
STATE state = RELAX;
STATE previousState = RELAX;

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

  wdt_disable();  /* Disable the watchdog and wait for more than 2 seconds */
  delay(3000);  /* Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration */
  wdt_enable(WDTO_8S);  /* Enable the watchdog with a timeout of 8 seconds */
  
  delay(100);
}


void loop() {
  performIO();
  switch(state) {
    case OPEN: 
      if(previousState != OPEN){
        openGate();
        previousState = OPEN;
      }
      break;

    case CLOSE: 
      if(previousState != CLOSE){
        closeGate();
        previousState = CLOSE;
      }
      break;

    case RELAX: 
      if(previousState != RELAX){
        relaxGate();
        previousState = RELAX;
      }
      break;
  }
  delay(50);
  wdt_reset();
}


void performIO() {
  int openGateButton = digitalRead(openGateSignalPin);
  int closeGateButton = digitalRead(closeGateSignalPin);

  // there is a pullup on the sensor. Turns low when opened or closed
  int isGateOpen = !digitalRead(gateOpenedSensorPin);
  int isGateClosed = !digitalRead(gateClosedSensorPin);

  //detect if motor is stuck
  int isMotorStuck = digitalRead(overloadDetectionPin);
  
  if(openGateButton && !isGateOpen && !closeGateButton){
      state = OPEN;
  } else if(closeGateButton && !isGateClosed && !openGateButton){
      state = CLOSE;
  } else {
      state = RELAX;
  }
}



void relaxGate(){
  digitalWrite(motorOpen, LOW);
  digitalWrite(motorClose, LOW);
  digitalWrite(LED_BUILTIN, LOW);
}



void closeGate(){
  relaxGate();
  delay(MOTOR_STOP_DELAY);
  digitalWrite(motorOpen, HIGH);
  digitalWrite(motorClose, LOW);
  digitalWrite(LED_BUILTIN, LOW);
}


void openGate(){
  relaxGate();
  delay(MOTOR_STOP_DELAY);
  digitalWrite(motorOpen, LOW);
  digitalWrite(motorClose, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
}



//void handleMotorStuck(){
//  relaxGate();
//  digitalWrite(dataOutputPin, HIGH);
//  delay(10000);
//}
//
//
//void handleMotorReady(){
//  digitalWrite(dataOutputPin, LOW);
//}
