#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

int pushUpSkor = 0, pushUpThresholddown = 15, pushUpThresholdup = 15;
bool flag = false;

VL53L0X sensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long waktu_skrg = 0, waktu_sblm = 0, interval = 500;

void setup()
{
  Serial.begin(57600);
  Serial2.begin(9600);
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    while(1)
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

  waktu_skrg = millis();
  if(waktu_skrg-waktu_sblm >= interval){

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

    waktu_sblm = waktu_skrg;
  }

  if (VLdistance <= pushUpThresholddown && flag == false && VLdistance != 0)
  {
    pushUpSkor += 1;
    Serial.println(pushUpSkor);
    Serial2.println(pushUpSkor);
    flag = true;
  }
  if (VLdistance > pushUpThresholdup)
  {
    flag = false;
  }

}