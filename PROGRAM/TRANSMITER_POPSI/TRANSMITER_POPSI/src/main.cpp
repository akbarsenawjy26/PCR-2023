<<<<<<< HEAD
#include "pushupcounter_main.h"

unsigned long waktu_skrg = 0, waktu_sblm = 0, interval = 500;


=======
#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

int pushUpSkor = 0, pushUpThresholdVL = 15;
bool flag = false;

VL53L0X sensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

>>>>>>> parent of 07f6eaf (Update main.cpp)
void setup()
{
  inisiasi_variabel();
  Serial.begin(57600);
  Serial2.begin(9600);
<<<<<<< HEAD
  inisiasi_utama();
=======
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
    Serial2.println("Failed to initialize VL53L0X sensor!");
    while (1)
      ;
  }

  sensor.startContinuous();
>>>>>>> parent of 07f6eaf (Update main.cpp)
}

void loop()
{
<<<<<<< HEAD
  fungsi_utama();
  waktu_skrg = millis();
  if(waktu_skrg-waktu_sblm >= interval){
    tampilan_lcd();
    waktu_sblm = waktu_skrg;
  }
=======
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
  Serial2.println(pushUpSkor);
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
>>>>>>> parent of 07f6eaf (Update main.cpp)
}