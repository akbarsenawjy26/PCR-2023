#include <Arduino.h>

long int detik_skrg = 0;
long int detik_sblm = 0;
long int jarak_waktu = 100;

int Value1 = 0;
const int panjang_data = 2+2;
int i = 0;
byte nilai[panjang_data];
int jumlah_data = 0;
int jumlah_data_serial=0;

bool proses_mengirim = false;
byte address_slave_1 = 0x45;
byte address_slave_2 = 0x46;
byte address;
String from;

void request_data(){
  byte kode = 0x42;
  byte read = 0x43; // byte yang akan dikirimkan dari master ke slave untuk memberitahu slave bahwa master meminta data dari slave
  byte write = 0x44; // byte yang akan dikirimkan dari master ke slave untuk memberitahu slave bahwa master akan mengirim data ke slave

  Serial2.write(kode);
  Serial2.write(address); // akan menginstruksikan slave 1
  Serial2.write(read); // master akan menerima data dari slave

  // Serial.print(kode);
  // Serial.print(address_slave_1);
  // Serial.print(read);
  // Serial.println();
}

void checksum_serial(){
  jumlah_data = 0;
  jumlah_data_serial = 0;
  
  for(int j = 0;j<panjang_data-2;j++){
    jumlah_data += nilai[j];
  }

  jumlah_data_serial = nilai[panjang_data-2] << 8 | nilai[panjang_data-1];

  if(jumlah_data == jumlah_data_serial){
    Value1 = (nilai[0] << 8) | nilai[1];

    Serial.print("Data dari slave " + from + " : " + (String)Value1);
    Serial.println();
  } else{
    Serial.print("GAGAL!");
    request_data();
  }
  proses_mengirim = false;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);

}

void loop() {
  if (Serial.available() > 0) {
    char data = Serial.read();
    switch (data)
    {
    case 'r':
      // request data kepada slave sertiap mengirim char s ke serial monitor
      if (!proses_mengirim) {
        request_data();
        proses_mengirim = true;
      }
      break;
    case '1':
      address = address_slave_1;
      from = "1";
      break;
    case '2':
      address = address_slave_2;
      from = "2";
      break;
    default:
      break;
    }
  }  
  

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

  detik_skrg = millis();
  if(detik_skrg-detik_sblm >= jarak_waktu){
    detik_sblm = detik_skrg;
    // request_data();
  }
}
