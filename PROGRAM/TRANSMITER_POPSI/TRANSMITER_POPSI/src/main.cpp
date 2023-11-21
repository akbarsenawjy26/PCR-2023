#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define OLED_SDA 21
#define OLED_SCL 22

int pushUpSkor = 0, pushUpThresholdVL = 100;
bool flag = false;

VL53L0X sensor;
Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

int i = 0;
int x = 0;
double rata;
bool start = false;

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);

  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
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
  uint16_t VLdistance = sensor.readRangeContinuousMillimeters();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("VL-mode");
  display.setCursor(0, 30);
  display.println("Count : ");  
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

  delay(100);
}