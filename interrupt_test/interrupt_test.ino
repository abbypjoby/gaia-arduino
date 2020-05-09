const byte ledPin = 2;
const byte interruptPin = 23;
volatile byte state = LOW;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello World");
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
}

void loop() {
  digitalWrite(ledPin, state);
  delay(5000);
}

void blink() {
  Serial.println("Blink! Interrupt called");
  state = !state;
}
