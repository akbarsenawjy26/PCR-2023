#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define OLED_SDA 21
#define OLED_SCL 22
#define BLUE            0x001F

// 'logo (2)', 50x47px
const unsigned char epd_bitmap_logo__2_ [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0xe0, 0x00, 
	0x00, 0x00, 0x23, 0xc0, 0x00, 0xf1, 0x00, 0x00, 0x00, 0x67, 0x80, 0x00, 0x79, 0x80, 0x00, 0x00, 
	0x67, 0x80, 0x00, 0x79, 0x80, 0x00, 0x00, 0xc7, 0x80, 0x00, 0x78, 0xc0, 0x00, 0x00, 0xcf, 0x80, 
	0x00, 0x7c, 0xc0, 0x00, 0x01, 0xcf, 0x80, 0x00, 0x7c, 0xe0, 0x00, 0x01, 0xcf, 0x80, 0x00, 0x7c, 
	0xe0, 0x00, 0x09, 0xcf, 0x81, 0x00, 0x7c, 0xe4, 0x00, 0x09, 0xc7, 0x80, 0x00, 0x78, 0xe4, 0x00, 
	0x18, 0xe7, 0x81, 0x60, 0x79, 0xc6, 0x00, 0x1c, 0xe7, 0xc1, 0xe0, 0xf9, 0xce, 0x00, 0x1e, 0x73, 
	0xc1, 0x80, 0xf3, 0x9e, 0x00, 0x1f, 0x19, 0xe1, 0x81, 0xe6, 0x1e, 0x00, 0x1f, 0x8c, 0xe0, 0x61, 
	0xcc, 0x7e, 0x00, 0x4f, 0xc0, 0xf1, 0xc3, 0xc0, 0xfc, 0x80, 0x47, 0xf0, 0x70, 0xc3, 0x83, 0xf8, 
	0x80, 0x63, 0xf8, 0x70, 0xc3, 0x87, 0xf1, 0x80, 0x70, 0xfc, 0x72, 0xd3, 0x8f, 0xc3, 0x80, 0x3c, 
	0x1e, 0x32, 0xd3, 0x1e, 0x0f, 0x00, 0x1f, 0x86, 0x32, 0xd3, 0x18, 0x7e, 0x00, 0x80, 0x03, 0x32, 
	0xd3, 0x30, 0x00, 0x40, 0xe0, 0x01, 0x72, 0xd3, 0xa0, 0x01, 0xc0, 0x7f, 0xe1, 0x72, 0xd3, 0xa1, 
	0xff, 0x80, 0x7f, 0xf9, 0x62, 0xd1, 0xa7, 0xff, 0x80, 0x3f, 0x3d, 0xc2, 0xd0, 0xef, 0x3f, 0x00, 
	0x00, 0x07, 0xc2, 0xd0, 0xf8, 0x00, 0x00, 0x41, 0xe3, 0x82, 0xd0, 0x71, 0xe0, 0x80, 0x3f, 0xf1, 
	0x82, 0xd0, 0x63, 0xff, 0x00, 0x1e, 0x01, 0x86, 0xd8, 0x60, 0x1e, 0x00, 0x00, 0x38, 0x82, 0xd0, 
	0x43, 0x00, 0x00, 0x01, 0xfe, 0xc0, 0xc0, 0xdf, 0xe0, 0x00, 0x1f, 0xf3, 0xfc, 0x0f, 0xf3, 0xfe, 
	0x00, 0x0f, 0x80, 0xff, 0x3f, 0xc0, 0x7c, 0x00, 0x00, 0x1c, 0x3f, 0xff, 0x8e, 0x00, 0x00, 0x00, 
	0x70, 0xff, 0xff, 0xc3, 0x80, 0x00, 0x07, 0xe3, 0x83, 0xf0, 0x71, 0xf8, 0x00, 0x00, 0x07, 0x01, 
	0xe0, 0x38, 0x00, 0x00, 0x00, 0x1e, 0x00, 0xc0, 0x1e, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x0f, 
	0xc0, 0x00, 0x00, 0x70, 0x00, 0x00, 0x03, 0x80, 0x00
};


const int epd_bitmap_allArray_LEN = 1;
const unsigned char* epd_bitmap_allArray[1] = {
	epd_bitmap_logo__2_
};


int jumlah=21;

Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

void start(){
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(20,20);
  display.println("START");
  display.display();
}

/*void menu(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(48,0);
  display.println("Menu");
  display.setCursor(18,18);
  display.println("Pribadi");
  display.setCursor(18,30);
  display.println("Bersama");
  display.setCursor(18,42);
  display.println("Set Jarak");
  display.display();
}*/

void pribadi(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(48,0);
  display.println("Menu");
  display.setCursor(0,18);
  display.println("-> Pribadi");
  display.setCursor(18,30);
  display.println("Bersama");
  display.setCursor(18,42);
  display.println("Set Jarak");
  display.display();
}

void bersama(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(48,0);
  display.println("Menu");
  display.setCursor(18,18);
  display.println("Pribadi");
  display.setCursor(0,30);
  display.println("-> Bersama");
  display.setCursor(18,42);
  display.println("Set Jarak");
  display.display();
}

void setjarak(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(48,0);
  display.println("Menu");
  display.setCursor(18,18);
  display.println("Pribadi");
  display.setCursor(18,30);
  display.println("Bersama");
  display.setCursor(0,42);
  display.println("-> Set Jarak");
  display.display();
}

/*void bersama2(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.println("Bersama:");
  display.setCursor(18,18);
  display.println("Wired");
  display.setCursor(18,30);
  display.println("Wireless");
  display.display();
}*/

void wired(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.println("Bersama:");
  display.setCursor(0,18);
  display.println("-> Wired");
  display.setCursor(18,30);
  display.println("Wireless");
  display.display();
}

void wireless(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.println("Bersama:");
  display.setCursor(18,18);
  display.println("Wired");
  display.setCursor(0,30);
  display.println("-> Wireless");
  display.display();
}

void TotalPU(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(24,0);
  display.println("Total Push Up:");
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(50,25);
  display.println(jumlah);
  display.display();
}

void press(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,28);
  display.println("Press Button to Start");
  display.display();
}

void countdown(){
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(56,22);
  display.println("3");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(56,22);
  display.println("2");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(56,22);
  display.println("1");
  display.display();
  delay(1000);
  display.clearDisplay();
}

void logo(){	
	display.drawBitmap(40, 3, epd_bitmap_logo__2_, 50, 47, WHITE); 
	display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(50,52);
    display.println("UNESA");
    display.display(); //tampilkan data
}

void setup()   {                
  Serial.begin(115200);
  /* initialize OLED with I2C address 0x3C */
  display.begin(SH1106_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();

}
void loop() { 
  //start();
  //pribadi();
  //bersama();
  //setjarak();
  //wired();
  //wireless();
  //press();
  //countdown();
  //TotalPU();
  logo();
}