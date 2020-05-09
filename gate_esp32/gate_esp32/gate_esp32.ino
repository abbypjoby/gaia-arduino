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
int openButtonPin = 19;
int relaxButtonPin = 18;

int previousState = 0;
volatile int buttonClick = 2; //2 is default status, no action required, no button clicked, 0->relax, 1->open, -1->closed, 2->server control


void setup() {

  pinMode(openGateSignalPin, OUTPUT);
  pinMode(closeGateSignalPin, OUTPUT);
  pinMode(closeButtonPin, INPUT_PULLUP);
  pinMode(openButtonPin, INPUT_PULLUP);
  pinMode(relaxButtonPin, INPUT_PULLUP);

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
  if ((wifiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin("https://pallathatta.herokuapp.com/node/1"); //HTTP

    USE_SERIAL.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String jsonPayload = http.getString();
        USE_SERIAL.println(jsonPayload);

        StaticJsonDocument<1000> doc;

        // Deserialize the JSON document
        DeserializationError error = deserializeJson(doc, jsonPayload);

        // Test if parsing succeeds.
        if (error) {
          USE_SERIAL.println("deserializeJson() failed: ");
          USE_SERIAL.println(error.c_str());
          return -1;
        }

        int openSignal = doc["is_on"];
        USE_SERIAL.println(openSignal);

        return openSignal;

      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      digitalWrite(openGateSignalPin, HIGH);
      digitalWrite(closeGateSignalPin, HIGH);
      return -1;
    }

    http.end();
   }
}



void openGate() {
    if(previousState == 1){
      stopGateBeforeReversing();
      previousState = 0;
    }
    USE_SERIAL.println("Opening Gate");
    digitalWrite(openGateSignalPin, LOW);
    digitalWrite(closeGateSignalPin, HIGH);
}


void closeGate() {
    if(previousState == 0){
      stopGateBeforeReversing();
      previousState = 1;
    }
    USE_SERIAL.println("Closing Gate");
    digitalWrite(openGateSignalPin, HIGH);
    digitalWrite(closeGateSignalPin, LOW);
}


void relaxGate() {
    USE_SERIAL.println("Relaxing Gate");
    digitalWrite(openGateSignalPin, HIGH);
    digitalWrite(closeGateSignalPin, HIGH);
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
