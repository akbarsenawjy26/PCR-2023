
#include <Arduino.h>
#include "UbidotsEsp32Mqtt.h"
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//-------------------------------UBIDOTS---------------------------------------------------------------
const char *UBIDOTS_TOKEN = "BBUS-R7nOjqo6WS530rYHaAGmGsrjSr8duh";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "Kurniasyah";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "Izzulizzam2002";      // Put here your Wi-Fi password

//int adressDevice = 57; // Nilai integer
//const char *topicPrefix = "01"; // String topik awal
//char fullTopic[50]; // Array char untuk menyimpan topik MQTT lengkap


const char *DEVICE_LABEL = "01";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "PushUpGraph"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 10; // Update rate in milliseconds

unsigned long timer;
//uint8_t analogPin = 32; // Pin used to read data from GPIO34 ADC_CH6.

Ubidots ubidots(UBIDOTS_TOKEN);



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

//--------------------------------UBIDOTS-------------------------------------------------------

//---------------------------------COUNTER------------------------------------------------------

int pushUpSkor = 0, pushUpThresholdVL = 15;
bool flag = false;

VL53L0X sensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int i = 0;
int x = 0;
double rata;
bool start = false;

//---------------------------------COUNTER------------------------------------------------------


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
  //-------------------------------------------------------------------------------------
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.clearDisplay();

  if (!sensor.init())
  {
    Serial.println("Failed to initialize VL53L0X sensor!");
    while (1)
      ;
  }

  sensor.startContinuous();
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

    uint16_t VLdistance = sensor.readRangeContinuousMillimeters()/10;
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("VL-mode");
    display.setCursor(0, 30);
    display.println("Count:");
    display.setCursor(80, 30);
    display.println(pushUpSkor);
    display.display();
    ubidots.add(VARIABLE_LABEL, VLdistance); // Insert your variable Labels and the value to be sent
    ubidots.publish(DEVICE_LABEL);
    timer = millis();

    if (VLdistance <= pushUpThresholdVL && flag == false && VLdistance != 0)
    {
      pushUpSkor += 1;
      Serial.print("Skor Push Up = ");
      Serial.println(pushUpSkor);
      flag = true;
    }
    if (VLdistance > pushUpThresholdVL)
    {
      flag = false;
    }
    //snprintf(fullTopic, sizeof(fullTopic), "%s/%d", topicPrefix, adressDevice);

  }
  delay(100);
  ubidots.loop();
}