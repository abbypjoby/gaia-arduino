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
int previousState = 0;


void setup() {

  pinMode(openGateSignalPin, OUTPUT);
  pinMode(closeGateSignalPin, OUTPUT);

  USE_SERIAL.begin(115200);
  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  wifiMulti.addAP("ZTE-ua2FJr", "cth4rchz");

}



void loop() {  
  int openSignal = callAPIandSetOpenSignal();

  switch(openSignal){
    
    case 0 : closeGate();
             break;

    case 1 : openGate();
             break;
             
    default : relaxGate();
              break;
  }
  
  delay(1000);
}




int callAPIandSetOpenSignal() {
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
   delay(2000);
}
