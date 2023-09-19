#include "pushupcounter_main.h"

void inisiasi_utama(){    
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

void tampilan_lcd(){
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
}

void fungsi_utama(){
    uint16_t VLdistance = sensor.readRangeContinuousMillimeters() / 10;
    if (VLdistance <= pushUpThresholddown && flag == false && VLdistance != 0){
        pushUpSkor += 1;
        Serial.println(pushUpSkor);
        flag = true;
    }
    if (VLdistance > pushUpThresholdup){
        flag = false;
    }
}