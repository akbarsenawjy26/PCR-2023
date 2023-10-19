#include <Arduino.h>

int Value1 = 0;
int Value2 = 0;
int Value3 = 0;


int jumlah_data = 0;
int jumlah_data_serial=0;

const int panjang_data = 1+6+2;
int i = 0;
byte nilai[panjang_data];

unsigned long detik_skrg = 0;
unsigned long detik_sblm = 0;
const long jarak_waktu = 200;


void checksum_serial(){
  byte kode = 0x42;
  jumlah_data = 0;
  jumlah_data_serial = 0;
  
  if(nilai[0] == kode){
    for(int j = 1;j<panjang_data-2;j++){
      jumlah_data += nilai[j];
    }
    jumlah_data_serial = nilai[panjang_data-2] << 8 | nilai[panjang_data-1];

    if(jumlah_data == jumlah_data_serial){
      Value1 = (nilai[1] << 8) | nilai[2];
      Value2 = (nilai[3] << 8) | nilai[4];
      Value3 = (nilai[5] << 8) | nilai[6];
    }else{
      Serial.print("GAGAL!");
    }
    
  }else{
    Serial.print("GAGAL!");
  }
}

int incomingByte = 0; // for incoming serial data

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);

}

void loop() { 
  if (Serial2.available() > 0) {
    nilai[i] = Serial2.read();
    i++;
    if(i > panjang_data-1){
      i = 0;
    }
  }
  for(int k = 0; k<panjang_data;k++){
    Serial.print(nilai[k]);
  }
  Serial.println();
  
  checksum_serial();

  detik_skrg = millis();
  if(detik_skrg-detik_sblm >= jarak_waktu){
    detik_sblm = detik_skrg;
    Serial.print(Value1);Serial.print("\t");
    Serial.print(Value2);Serial.print("\t");
    Serial.print(Value3);Serial.println();
  }

}
