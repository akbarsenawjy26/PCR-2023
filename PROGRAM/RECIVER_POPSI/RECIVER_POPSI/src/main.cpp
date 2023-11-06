#include <Arduino.h>

const int panjang_data = 1+1+2+2; // byte kode+addres_salve+data+checksum
byte nilai[panjang_data];
int i = 0;

void checksum_serial(){
  byte kode = 0x42;
  int banyak_slave = 2;
  byte address_slave[banyak_slave] = {0x45,0x46};
  int from;
  bool address_cocok = false;
  
  int Value1 = 0;
  int jumlah_data = 0;
  int jumlah_data_serial = 0;

  if (nilai[0] == kode) {
    for (size_t i = 0; i < banyak_slave; i++)
    {
      if (nilai[1] == address_slave[i])
      {
        from = i+1;
        address_cocok = true;
      } 
    }
  } else {
    Serial.println("GAGAL!, Kode salah");
  }
  


  if (address_cocok) {
    for(int j = 2;j<panjang_data-2;j++){
      jumlah_data += nilai[j];
    }

    jumlah_data_serial = nilai[panjang_data-2] << 8 | nilai[panjang_data-1];

    if(jumlah_data == jumlah_data_serial){
      Value1 = (nilai[2] << 8) | nilai[3];

      Serial.println("Data dari slave " + (String)from + " : " + (String)Value1);
    } else{
      Serial.println("GAGAL!, checksum eror");
    }
  } else {
    Serial.println("Address tidak ada yang cocok");
  }
}

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
      checksum_serial();
    }
    for(int k = 0; k<panjang_data;k++){
      // Serial.print(nilai[k]);
    }
    // Serial.println();
  }
}