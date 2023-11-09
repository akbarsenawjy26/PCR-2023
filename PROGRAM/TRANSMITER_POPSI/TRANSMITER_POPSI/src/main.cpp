/****************************************
 * Include Libraries
 ****************************************/
//#include "UbidotsESPMQTT.h"

/****************************************
 * Define Constants
 ****************************************/
 /*
#define TOKEN "BBUS-R7nOjqo6WS530rYHaAGmGsrjSr8duh" // Your Ubidots TOKEN
#define WIFINAME "TP-Link_AFBC" //Your SSID
#define WIFIPASS "Penelitian2023" // Your Wifi Pass
*/
/******************************************
 *
 * This example works for both Industrial and STEM users.
 *
 * Developed by Jose Garcia, https://github.com/jotathebest/
 *
 * ****************************************/

/****************************************
 * Include Libraries
 ****************************************/
#include "UbidotsEsp32Mqtt.h"

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBUS-tt5el5LbI7OuFCwS92MYTJyzQD9zuq";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "Kurniasyah";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "Izzulizzam2002";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "ESP32DevKit";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "Device-1"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 500; // Update rate in milliseconds

unsigned long timer;
uint8_t analogPin = 32; // Pin used to read data from GPIO34 ADC_CH6.

Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if ((millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    float value = analogRead(analogPin);
    ubidots.add(VARIABLE_LABEL, value); // Insert your variable Labels and the value to be sent
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();
}