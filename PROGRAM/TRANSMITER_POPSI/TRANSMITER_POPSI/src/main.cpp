/****************************************
 * Include Libraries
 ****************************************/
#include "UbidotsESPMQTT.h"

/****************************************
 * Define Constants
 ****************************************/
#define TOKEN "BBUS-R7nOjqo6WS530rYHaAGmGsrjSr8duh" // Your Ubidots TOKEN
#define WIFINAME "TP-Link_AFBC" //Your SSID
#define WIFIPASS "Penelitian2023" // Your Wifi Pass

Ubidots client(TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup() {
  // put your setup code here, to run once:
  //client.ubidotsSetBroker("industrial.api.ubidots.com"); // Sets the broker properly for the industrial account
  client.setDebug(true); // Pass a true or false bool value to activate debug messages
  Serial.begin(115200);
  client.wifiConnection(WIFINAME, WIFIPASS);
  client.begin(callback);
  }

void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
      client.reconnect();
      }
  
  float value1 = analogRead(A0);
  client.add("temperature", value1);
  client.ubidotsPublish("version1");
  client.loop();
  delay(500);
}