/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>
#include <ArduinoJson.h>


#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

int openGateSignalPin = 2;
int closeGateSignalPin = 4;

int closeButtonPin = 21;
int openButtonPin = 18;
int relaxButtonPin = 19;

int closeLedPin = 25;
int openLedPin = 26;
int relaxLedPin = 27;

int previousState = 0;
volatile int buttonClick = 2; //2 is default status, no action required, no button clicked, 0->relax, 1->open, -1->closed, 2->server control


void setup() {

  pinMode(openGateSignalPin, OUTPUT);
  pinMode(closeGateSignalPin, OUTPUT);
  
  pinMode(closeLedPin, OUTPUT);
  pinMode(openLedPin, OUTPUT);
  pinMode(relaxLedPin, OUTPUT);
  
  pinMode(closeButtonPin, INPUT);
  pinMode(openButtonPin, INPUT);
  pinMode(relaxButtonPin, INPUT);

  USE_SERIAL.begin(115200);
  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  //Interrupts for close button press
  attachInterrupt(digitalPinToInterrupt(closeButtonPin), onCloseGateButtonClick, CHANGE);
  
  //Interrupts for open button press
  attachInterrupt(digitalPinToInterrupt(openButtonPin), onOpenGateButtonClick, CHANGE);
   
  //Interrupts for open button press
  attachInterrupt(digitalPinToInterrupt(relaxButtonPin), onRelaxGateButtonClick, CHANGE);

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  wifiMulti.addAP("ZTE-ua2FJr", "cth4rchz");
}



void loop() {  
  int buttonSignal = updateGateServerWithButtonClick();
  int serverSignal = callAPIandGetOpenSignal();
  int activeControlSignal = serverSignal;

  if (isManualControlActive()){
    activeControlSignal = buttonSignal;
  }
    
  switch(activeControlSignal){
    case 0 : closeGate();
             break;
  
    case 1 : openGate();
             break;
             
    default : relaxGate();
              break;
  }
  
  delay(1000);
}


boolean isManualControlActive(){
  if(buttonClick == 2){
    //2 means no button is pressed => use controls from server
    return false;
  }
   return true;
}


int callAPIandGetOpenSignal() {
// wait for WiFi connection
  int openSignal = 2;
  if ((wifiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin("https://pallathatta.herokuapp.com/node/1"); //HTTP
    int httpCode = http.GET();

    if (httpCode > 0) {
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String jsonPayload = http.getString();
        USE_SERIAL.println(jsonPayload);

        StaticJsonDocument<1000> doc;
        DeserializationError error = deserializeJson(doc, jsonPayload);

        if (error) {
          USE_SERIAL.println("deserializeJson() failed: ");
          USE_SERIAL.println(error.c_str());
          http.end();
          return openSignal;
        }

        openSignal = doc["is_on"];
        USE_SERIAL.println(openSignal);
      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
   }
   return openSignal;
}



void openGate() {
    if(previousState == 1){
      stopGateBeforeReversing();
      previousState = 0;
    }
    USE_SERIAL.println("Opening Gate");
    turnOnLed(openLedPin);
    digitalWrite(openGateSignalPin, LOW);
    digitalWrite(closeGateSignalPin, HIGH);
}


void closeGate() {
    if(previousState == 0){
      stopGateBeforeReversing();
      previousState = 1;
    }
    turnOnLed(closeLedPin);
    USE_SERIAL.println("Closing Gate");
    digitalWrite(openGateSignalPin, HIGH);
    digitalWrite(closeGateSignalPin, LOW);
}


void relaxGate() {
    USE_SERIAL.println("Relaxing Gate");
    turnOnLed(relaxLedPin);
    digitalWrite(openGateSignalPin, HIGH);
    digitalWrite(closeGateSignalPin, HIGH);
}


void turnOnLed(int ledPin){
  digitalWrite(closeLedPin, LOW);
  digitalWrite(openLedPin, LOW);
  digitalWrite(relaxLedPin, LOW);

  digitalWrite(ledPin, HIGH);
}


void stopGateBeforeReversing() {
   USE_SERIAL.println("Stopping Gate");
   digitalWrite(openGateSignalPin, LOW);
   digitalWrite(closeGateSignalPin, LOW);
   delay(4000);
}


void onCloseGateButtonClick() {
  USE_SERIAL.println("Close button Clicked");
  buttonClick = 0; 
}


void onOpenGateButtonClick() {
  USE_SERIAL.println("Open button Clicked");
  buttonClick = 1;
}


void onRelaxGateButtonClick() {
  USE_SERIAL.println("Stop button Clicked");
  buttonClick = -1;
}


int updateGateServerWithButtonClick() {
  int buttonSignal = -1;
  String url = "https://pallathatta.herokuapp.com/node/update?node_id=1&is_on=";
  
  if (buttonClick == 0){
    buttonSignal = 0;
  }

  if (buttonClick == 1){
    buttonSignal = 1;
  }

  if (buttonClick == -1){
    buttonSignal = -1;
  }

  if (buttonClick == 2){
    //Auto server Mode enabled
    return 2;
  }

  url = url + buttonSignal;
  
  USE_SERIAL.printf("Updating gate state on server to %d");
  if ((wifiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(url);
    USE_SERIAL.printf("Starting API call to %s", url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        buttonClick = 2; 
      }
    }
  }

 return buttonSignal;
}
