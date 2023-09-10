#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "CTBot.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

int pushUpSkor = 0, pushUpThresholdVL = 15;
bool flag = false;
String ssid = "Singkong Krispy", pass = "kibar899", token = "6392401256:AAGpFAWo2R2WDamcMOkoxVyocxbMQ6I3wiQ";

VL53L0X sensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
CTBot myBot;
TBMessage msg;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);

  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");

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
  uint16_t VLdistance = sensor.readRangeContinuousMillimeters() / 10;

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

  if (msg.text.equalsIgnoreCase("/getdata"))
  {
    myBot.sendMessage(msg.sender.id, "Total skor push-up: " + String(pushUpSkor));
    delay(100);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("send data!");
    display.display();
    delay(2000);
    display.clearDisplay();
  }
  delay(10);
}