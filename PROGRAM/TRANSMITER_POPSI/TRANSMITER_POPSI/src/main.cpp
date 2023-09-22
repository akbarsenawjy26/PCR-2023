#include <Arduino.h>
#include <WiFi.h>
#include "Display.h"
#include "Sensor.h"
#include "Actuator-LED.h"
#include "GPIO-Button.h"

int pushUpSkor = 0, setUp, setDown, task, preCount;
bool flag = false;

// Definisi Fungsi
void pushUpCounter();
void setParameter();
void dataParameter();
void setup()
{
  Serial.begin(115200);
  pinMode(LEDpin, OUTPUT);
  pinMode(btn, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  displaySetup();
  sensorSetup();
}

void loop()
{
  switch (task)
  {
  case 0: // Untuk mencari data parameter
    setParameter();
    break;

  case 1: // Push-up Counter
    dataParameter();
    break;

  case 2:
    pushUpCounter();
    break;
  }
}

void pushUpCounter()
{
  VLdistance = sensor.readRangeContinuousMillimeters() / 10;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(27, 0);
  display.print("Push-Up Score");
  display.setTextSize(5);

  if (pushUpSkor < 10)
  {
    display.setCursor(49, 20);
  }

  if (pushUpSkor >= 10)
  {
    display.setCursor(39, 20);
  }

  display.print(pushUpSkor);
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

void setParameter()
{
  for (int i = 3; i > 0; i--)
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(29, 0);
    display.print("Ready!");
    display.setCursor(55, 25);
    display.print(preCount);
    display.display();
    for (int o = 0; o < 10; o++)
    {
      dataSensor += VLdistance;
      Serial.println(dataSensor);
      delay(100);
    }
    preCount -= 1;
  }

  if (preCount <= 1)
  {
    task = 1;
    delay(10);
  }
}

void dataParameter()
{
  setDown = 3;
  setUp = (dataSensor / 30) - setDown;
  Serial.print("AVG: ");
  Serial.println(dataSensor / 30);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 45);
  display.print("Jarak Rata-Rata: ");
  display.setCursor(100, 45);
  display.print(dataSensor / 30);
  display.display();

  delay(100);
  Serial.end();

  if (digitalRead(btn) == LOW)
  {
    task = 2;
    delay(10);
  }
}