int gateOpenedSensorPin = 6; // schematic pin 12
int gateClosedSensorPin = 7; // schematic pin 13

int motorOpen = 11; // schematic pin 17
int motorClose = 12; // schematic pin 18

void setup() {
  // put your setup code here, to run once:
  pinMode(motorOpen, OUTPUT);
  pinMode(motorClose, OUTPUT);
  pinMode(gateOpenedSensorPin, INPUT);
  pinMode(gateClosedSensorPin, INPUT);

  digitalWrite(motorOpen, HIGH);
  digitalWrite(motorClose, HIGH);
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  int openGateButton = digitalRead(gateOpenedSensorPin);
  int closeGateButton = digitalRead(gateClosedSensorPin);

  if (openGateButton && !closeGateButton){
    digitalWrite(motorOpen, HIGH);
    digitalWrite(motorClose, LOW);
  } else if (closeGateButton && !openGateButton){
    digitalWrite(motorOpen, LOW);
    digitalWrite(motorClose, HIGH);
  } else {
      digitalWrite(motorOpen, HIGH);
      digitalWrite(motorClose, HIGH);
  }

    delay(10);
}
