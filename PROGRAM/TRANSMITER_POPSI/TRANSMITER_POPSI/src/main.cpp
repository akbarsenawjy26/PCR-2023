#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial2 (8, 7); // RX, TX

long int detik_skrg = 0;
long int detik_sblm = 0;
long int jarak_waktu = 10;

int Value1 = 0;
const int panjang_data = 2+2;
int i = 0;
byte nilai[panjang_data];
int jumlah_data = 0;
int jumlah_data_serial=0;

const int banyak_slave = 2;
byte address_slave[banyak_slave] = {0x45,0x46};
int slave_index = 1;
String from;

int final_value[banyak_slave]; 

void upload_data() {
  // save the data
  final_value[slave_index - 1] = Value1;

  for (int i = 0; i < banyak_slave; i++)
  {
    if ((i % 2 == 1))
    {
      Serial.print(",");
    }
    
    Serial.print(final_value[i]);
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
    Value1 = (nilai[0] << 8) | nilai[1];

    // Serial.println("Data dari slave " + from + " : " + (String)Value1);

    upload_data();
  } else{
    Serial.println("GAGAL!");
    
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
    
    // request_data()
  }
}