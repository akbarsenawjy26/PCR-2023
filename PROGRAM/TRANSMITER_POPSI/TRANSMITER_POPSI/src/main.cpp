#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial2 (8, 7); // RX, TX

long int detik_skrg = 0;
long int detik_sblm = 0;
long int jarak_waktu = 10;

const int banyak_data_setiap_slave = 2;
const int banyak_checksum = 2;
int Value[banyak_data_setiap_slave] = {0,0};
const int panjang_data = (banyak_data_setiap_slave + 2) + banyak_checksum;
int i = 0;
byte nilai[panjang_data];
int jumlah_data = 0;
int jumlah_data_serial=0;

const int banyak_slave = 3;
byte address_slave[banyak_slave] = {0x45,0x46,0x47};
int slave_index = 1;
String from;

int final_value[banyak_slave][banyak_data_setiap_slave]; 

void upload_data() {
  // save the data
  for (int i = 0; i < banyak_data_setiap_slave; i++)
  {
    final_value[slave_index - 1][i] = Value[i];
  }

  for (int i = 0; i < banyak_slave; i++)
  {    
    for (int j = 0; j < banyak_data_setiap_slave; j++)
    {
      if (i > 0 || j > 0)
      {
        Serial.print(",");
      }
      Serial.print(final_value[i][j]);
    }
  }
  Serial.println();

  // waits for the transmission of outgoing serial data to complete
  Serial.flush();

  slave_index++;
  if (slave_index > banyak_slave)
  {
    slave_index = 1;
  }
}

void request_data(){
  byte kode = 0x42;
  byte read = 0x43; // byte yang akan dikirimkan dari master ke slave untuk memberitahu slave bahwa master meminta data dari slave
  byte write = 0x44; // byte yang akan dikirimkan dari master ke slave untuk memberitahu slave bahwa master akan mengirim data ke slave

  Serial2.write(kode);
  Serial2.write(address_slave[slave_index-1]); // akan menginstruksikan slave 1
  Serial2.write(read); // master akan menerima data dari slave

  // waits for the transmission of outgoing serial data to complete
  Serial2.flush();
}

void checksum_serial(){
  jumlah_data = 0;
  jumlah_data_serial = 0;
  
  for(int j = 0;j<panjang_data-2;j++){
    jumlah_data += nilai[j];
  }

  jumlah_data_serial = nilai[panjang_data-2] << 8 | nilai[panjang_data-1];

  if(jumlah_data == jumlah_data_serial){
    Value[0] = (nilai[0] << 8) | nilai[1];
    Value[1] = (nilai[2] << 8) | nilai[3];

    // Serial.println("Data dari slave " + from + " : " + (String)Value1);

    upload_data();
  } else{
    Serial.println("GAGAL! checksum");
    
    // meminta data lagi jika data yang diterima gagal
    request_data();
  }

  // empty the receiver buffer
  while (Serial2.available())
  {
    Serial2.read();
  }
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
      // request data kepada slave sertiap mengirim char r ke serial monitor
      request_data();
      break;

    case '1':
      slave_index = 1;
      from = data;
      break;

    case '2':
      slave_index = 2;
      from = data;
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
    // for(int k = 0; k<panjang_data;k++){
    //   Serial.print(nilai[k]);
    // }
    // Serial.println();
  }

  // cek dan memastikan agar tidak request data ketika sedang menerima data 
  if (i == 0)
  {
    request_data();
  }

  detik_skrg = millis();
  if(detik_skrg-detik_sblm >= jarak_waktu){
    detik_sblm = detik_skrg;
    
    // cek dan memastikan agar tidak request data ketika sedang menerima data 
    // if (i == 0)
    // {
    //   request_data();
    // }
  }
}