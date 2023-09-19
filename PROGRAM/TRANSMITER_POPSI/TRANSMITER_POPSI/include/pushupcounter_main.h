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

void inisiasi_utama();
void tampilan_lcd();
void fungsi_utama();