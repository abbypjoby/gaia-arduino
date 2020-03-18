int openGateSignalPin = 14; // schematic pin 23
int closeGateSignalPin = 15; // schematic pin 24

int motorOpen = 11; // schematic pin 17
int motorClose = 12; // schematic pin 18

int gateOpenedSensorPin = 7; // schematic pin 12
int gateClosedSensorPin = 6; // schematic pin 13

int previousState = 0;


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
//  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(motorOpen, OUTPUT);
  pinMode(motorClose, OUTPUT);
  pinMode(openGateSignalPin, INPUT_PULLUP);
  pinMode(closeGateSignalPin, INPUT_PULLUP);
  pinMode(gateOpenedSensorPin, INPUT);
  pinMode(gateClosedSensorPin, INPUT);

  digitalWrite(motorOpen, HIGH);
  digitalWrite(motorClose, HIGH);
  delay(10);
}

// the loop function runs over and over again forever
void loop() {
  
  int openGateButton = digitalRead(openGateSignalPin);
  int closeGateButton = digitalRead(closeGateSignalPin);

  // there is a pullup on the sensor. Turns low when opened or closed
  int isGateOpen = !digitalRead(gateOpenedSensorPin);
  int isGateClosed = !digitalRead(gateClosedSensorPin);

  
  if(openGateButton && !isGateOpen && !closeGateButton){
     
      openGate();
  
  } else if(closeGateButton && !isGateClosed && !openGateButton){
  
      closeGate();
  
  } else {

      relaxGate();
//      Serial.println("Both buttons high - relaxed - some connection is cut");
  
  }

  delay(5);

}


void closeGate(){
//    if (previousState == 0) {
//      stopGate();
//      delay(2000);
//      previousState = 1;
//    }
    digitalWrite(motorOpen, LOW);
    digitalWrite(motorClose, HIGH);
//    Serial.println("Close button high - closing");
    digitalWrite(LED_BUILTIN, LOW);
}


void openGate(){
//    if (previousState == 1) {
//      stopGate();
//      delay(2000);
//      previousState = 0;
//    }
    digitalWrite(motorOpen, HIGH);
    digitalWrite(motorClose, LOW);
    digitalWrite(LED_BUILTIN, LOW);
//    Serial.println("Open button high - opening");
}


void relaxGate(){
  digitalWrite(motorOpen, HIGH);
  digitalWrite(motorClose, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
}
