#include <Arduino.h>
//Uji Coba Checksum
int Value1 = 9999;
// int Value2 = 100;
// int Value3 = 89;

long int detik_skrg = 0;
long int detik_sblm = 0;
long int jarak_waktu = 100;

void checksum_serial(){
  int panjang_data = 2;
  byte nilai[panjang_data];
  byte kode = 0x42;
  byte address = 0x45;
  byte checksum[2];
  int jumlah_data = 0;
  
  nilai[0] = (Value1 >> 8) & 0xFF;
  nilai[1] = Value1 & 0xFF;
  // nilai[2] = (Value2 >> 8) & 0xFF;
  // nilai[3] = Value2 & 0xFF;
  // nilai[4] = (Value3 >> 8) & 0xFF;
  // nilai[5] = Value3 & 0xFF;


  for(int i=0;i < panjang_data; i++){
    jumlah_data += nilai[i];
  }

  checksum[0] = (jumlah_data >> 8) & 0xFF;
  checksum[1] = jumlah_data & 0xFF;

  Serial2.write(kode);
  Serial2.write(address);
  Serial2.write(nilai,panjang_data);
  Serial2.write(checksum,2);

  Serial.print(kode);
  Serial.print(address);
  for(int i = 0; i < panjang_data; i++) {
    Serial.print(nilai[i]);
  }
  for(int i = 0; i < 2; i++) {
    Serial.print(checksum[i]);
  }
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);

}

void loop() {
  detik_skrg = millis();
  if(detik_skrg-detik_sblm >= jarak_waktu){
    detik_sblm = detik_skrg;
    checksum_serial();
  }
}
