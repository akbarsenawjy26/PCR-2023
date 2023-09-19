#include "pushupcounter_main.h"

unsigned long waktu_skrg = 0, waktu_sblm = 0, interval = 500;

void setup()
{
  Serial.begin(57600);
  Serial2.begin(9600);
  inisiasi_utama();
}

void loop()
{
  fungsi_utama();
  waktu_skrg = millis();
  if(waktu_skrg-waktu_sblm >= interval){
    tampilan_lcd();
    waktu_sblm = waktu_skrg;
  }
}