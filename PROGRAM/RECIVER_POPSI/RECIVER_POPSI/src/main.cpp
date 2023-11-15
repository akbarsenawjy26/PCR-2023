#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

void setup()   {                
  Serial.begin(115200);
  /* initialize OLED with I2C address 0x3C */
  display.begin(SH1106_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();

}
void loop() { 
  /* set text size, color, cursor position, 
  set buffer with  Hello world and show off*/
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello, world!");
  display.display();
  delay(2000);
  display.clearDisplay();
}