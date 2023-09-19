#include <Arduino.h>
#include <WiFi.h>
#include "Display.h"
#include "Sensor.h"
#include "Actuator-LED.h"

int pushUpSkor = 0, setUp, setDown, task;
bool flag = false;

// Definisi Fungsi
void pushUpCounter();
void setParameter();

void setup()
{
  Serial.begin(115200);
  pinMode(LEDpin, OUTPUT);
  WiFi.mode(WIFI_STA);
  displaySetup();
  sensorSetup();
}

void loop()
{
  switch (task)
  {
  case 0: // Untuk mencari data parameter

    break;

  case 1: // Push-up Counter
    pushUpCounter();
    break;
  }
}

void pushUpCounter()
{
  uint16_t VLdistance = sensor.readRangeContinuousMillimeters() / 10;

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Push-up Counter");
  display.setCursor(0, 30);
  display.println("Count:");
  display.setCursor(80, 30);
  display.println(pushUpSkor);
  display.display();

  if (VLdistance <= setDown && flag == false && VLdistance != 0)
  {
    digitalWrite(LEDpin, HIGH);
    pushUpSkor += 1;
    Serial.print("Skor Push Up = ");
    Serial.println(pushUpSkor);
    flag = true;
  }

  if (VLdistance > setDown && VLdistance > setUp)
  {
    digitalWrite(LEDpin, HIGH);
    flag = false;
  }

  delay(100);
}