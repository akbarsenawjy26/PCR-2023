#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial2 (8, 7); // RX, TX

int Value = 8888;

const int panjang_data = 1+1+1;
int i = 0;
byte nilai[panjang_data];

void checksum_serial(){
  int slave_index = 1; // PENTING!!! isi sinteger sesuai slave ke berapa yang akan di upload
  int banyak_slave = 2;

  byte kode = 0x42;
  byte address_slave[banyak_slave] = {0x45,0x46};
  int panjang_data_kirim = 2;
  byte nilai_kirim[panjang_data];
  byte checksum[2];
  int jumlah_data = 0;

  if(nilai[0] == kode){
    if (nilai[1] == address_slave[slave_index-1]) {
      switch (nilai[2])
      {
      case 0x43:
        // Master meminta data
        // Proses mengirim data yang diminta
        nilai_kirim[0] = (Value >> 8) & 0xFF;
        nilai_kirim[1] = Value & 0xFF;

        for(int i=0;i < panjang_data_kirim; i++){
          jumlah_data += nilai_kirim[i];
        }

        checksum[0] = (jumlah_data >> 8) & 0xFF;
        checksum[1] = jumlah_data & 0xFF;

        Serial2.write(nilai_kirim,panjang_data_kirim);
        Serial2.write(checksum,2);
        break;

      case 0x44:
        // Master mengirim data
        break;

      default:
        break;
      }
    } else {
      Serial.print("Berbeda Address. Address ini : " + (String)address_slave[slave_index-1] + ". Sedangkan address yang dikirimkan" + (String)nilai[1]);
    }
  }else{
    Serial.print("GAGAL!");
  }
  Serial.println();
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
      Serial.print(nilai[k]);
    }
    Serial.println();
    
    
  }
}