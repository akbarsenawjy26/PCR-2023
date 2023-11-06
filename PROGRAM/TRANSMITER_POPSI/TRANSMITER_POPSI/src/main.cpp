#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial2 (8, 7); // RX, TX

long int detik_skrg = 0;
long int detik_sblm = 0;
long int jarak_waktu = 100;

const int panjang_data = 1+1+2+2; // byte kode+addres_salve+data+checksum
byte nilai[panjang_data];
int i = 0;

int slave_index = 1; // isi integer berikut sesuai dengan slave ke berapa yang akan di uploadkan program !!!! PENTING
int tunggu_slave_index;
int banyak_slave = 2;

bool tunggu = false;

void send_data() {
  byte kode = 0x42;
  byte address_slave[2] = {0x45,0x46};

  int Value = 9999;
  int panjang_data_kirim = 2;
  byte nilai_kirim[panjang_data_kirim];
  byte checksum[2];
  int jumlah_data = 0;

  nilai_kirim[0] = (Value >> 8) & 0xFF;
  nilai_kirim[1] = Value & 0xFF;

  for(int i=0;i < panjang_data_kirim; i++){
  jumlah_data += nilai_kirim[i];
  }

  checksum[0] = (jumlah_data >> 8) & 0xFF;
  checksum[1] = jumlah_data & 0xFF;

  // Proses mengirim data
  Serial2.write(kode);
  Serial2.write(address_slave[slave_index-1]);
  Serial2.write(nilai_kirim,panjang_data_kirim);
  Serial2.write(checksum,2);
}

void checksum_serial(){
  byte kode = 0x42;
  byte address_slave[banyak_slave] = {0x45,0x46};

  if (nilai[0] == kode) {
    for (int i = 0; i < banyak_slave; i++)
    {
      if (nilai[1] == address_slave[tunggu_slave_index - 1])
      {
        tunggu = false;
      } 
    }
  } else {
    Serial.println("GAGAL!, Kode salah");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);

  tunggu_slave_index = slave_index - 1;
  if (tunggu_slave_index == 0)
  {
    tunggu_slave_index = banyak_slave;
  }
  
}

void loop() {
  while (Serial2.available() > 0) {
    nilai[i] = Serial2.read();
    i++;
    if(i > panjang_data-1){
      i = 0;
      tunggu = true;
      checksum_serial();
    }
    for(int k = 0; k<panjang_data;k++){
      // Serial.print(nilai[k]);
    }
    // Serial.println();
  }

  detik_skrg = millis();
  if(detik_skrg-detik_sblm >= jarak_waktu){
    detik_sblm = detik_skrg;
    
    if (!tunggu)
    {
      send_data();
    }
    
  }
}
