#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <PubSubClient.h>
#include <Adafruit_SH1106.h>
#include <Preferences.h>
#include <esp_now.h>

#define OLED_SDA 21
#define OLED_SCL 22

//sensor
uint16_t VLdistance;

//ESPNOW
uint8_t broadcastAddress[] = {0x08, 0xB6, 0x1F, 0x75, 0x60, 0x94};

typedef struct struct_message
{
  uint16_t VLdistance;
} struct_message;

struct_message dataSensor;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

unsigned long waktu_skrg = 0, waktu_sblm = 0, interval = 10;

//Moving Average
const int numReadings  = 10;
uint16_t readings [numReadings];
int readIndex  = 0;
long total  = 0;  
int aisVal  = 0;

int pushUpSkor_vl = 0;
bool flag = false;

int pushUpThresholddown = 100;
int pushUpThresholdup = 200;
int pushUpThresholdup_up = 600;

unsigned long waktu_skrg_vl = 0, waktu_sblm_vl = 0, interval_vl = 10;
//setjarak
unsigned long waktu_skrg_setjarak = 0, waktu_sblm_setjarak = 0, interval_setjarak = 100;
unsigned long waktu_skrg_countdown_setjarak = 0, waktu_sblm_countdown_setjarak = 0, interval_countdown_setjarak = 1000;
unsigned long rata_rata_jarak,total_ratarata;
unsigned int jumlah_ratarata=0;
int setjarak_countdown=6;

long smooth(uint16_t data_VL) { 
  long average;
  total = total - readings[readIndex];
  readings[readIndex] = data_VL;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  average = total / numReadings;

  return average;
}

//END ESPNOW

Preferences push_up;
Preferences coba;
int jarak_set = 0;
int pushUpSkor = 0, pushUpThresholdVL = 400;
bool flag_vl = false;

VL53L0X sensor;
Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

int Value1 = 9999, Value2 = 999;

const int panjang_data = 1+1+1;
int i = 0;
byte nilai[panjang_data];

int slave_index = 2; // PENTING!!! isi sinteger sesuai slave ke berapa yang akan di upload
const int banyak_slave = 3;
byte address_slave[banyak_slave] = {0x45,0x46,0x47};
Preferences slave_index_preferences;

byte kode = 0x42;

const int panjang_data_kirim = 4;
byte nilai_kirim[panjang_data_kirim];
byte checksum[2];
int jumlah_data = 0;

int jumlah;

//----------------IKMAL-----------------------
const char* ssid = "Galaxy A51 CF0A"; // Nama jaringan WiFi
const char* password = "uuxz9656"; // Kata sandi WiFi
const char* mqttServer = "broker.mqtt-dashboard.com"; // Alamat broker MQTT
int mqttPort = 1883; // Port broker MQTT

WiFiClient espClient;
PubSubClient client(espClient);

int adressDevice = 57; // Nilai integer
const char* topicGraph = "/esp32/data/sensor/graph"; 
const char* topicScore = "/esp32/data/sensor/score"; 
unsigned long lastMsg = 0;

//------------------------------PROGRAM MQTT----------------------------------------------------------
void setup_wifi() {

  delay(10);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Connecting to ");
  display.println(ssid);
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
  display.println("");
  display.println("WiFi connected");
  display.println("IP address: ");
  display.println(WiFi.localIP());
  display.display();
  delay(1000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("/esp32/mqtt/in");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//------------------------------PROGRAM MQTT----------------------------------------------------------
//-----------------------------Button-------------------
int tombol = 0;
bool tombol_up_ditekan = 0;
bool tombol_down_ditekan = 0;
bool tombol_set_ditekan = 0;
bool lastbuttonstate_up = 0;
bool lastbuttonstate_down = 0;
bool lastbuttonstate_set = 0;
int mode_tampilan = 0;

const int up_pin = 15;
const int down_pin = 2;
const int set_pin = 32;


//-----------------------------Button Finish------------

const unsigned char epd_bitmap_logo__2_ [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0xe0, 0x00, 
	0x00, 0x00, 0x23, 0xc0, 0x00, 0xf1, 0x00, 0x00, 0x00, 0x67, 0x80, 0x00, 0x79, 0x80, 0x00, 0x00, 
	0x67, 0x80, 0x00, 0x79, 0x80, 0x00, 0x00, 0xc7, 0x80, 0x00, 0x78, 0xc0, 0x00, 0x00, 0xcf, 0x80, 
	0x00, 0x7c, 0xc0, 0x00, 0x01, 0xcf, 0x80, 0x00, 0x7c, 0xe0, 0x00, 0x01, 0xcf, 0x80, 0x00, 0x7c, 
	0xe0, 0x00, 0x09, 0xcf, 0x81, 0x00, 0x7c, 0xe4, 0x00, 0x09, 0xc7, 0x80, 0x00, 0x78, 0xe4, 0x00, 
	0x18, 0xe7, 0x81, 0x60, 0x79, 0xc6, 0x00, 0x1c, 0xe7, 0xc1, 0xe0, 0xf9, 0xce, 0x00, 0x1e, 0x73, 
	0xc1, 0x80, 0xf3, 0x9e, 0x00, 0x1f, 0x19, 0xe1, 0x81, 0xe6, 0x1e, 0x00, 0x1f, 0x8c, 0xe0, 0x61, 
	0xcc, 0x7e, 0x00, 0x4f, 0xc0, 0xf1, 0xc3, 0xc0, 0xfc, 0x80, 0x47, 0xf0, 0x70, 0xc3, 0x83, 0xf8, 
	0x80, 0x63, 0xf8, 0x70, 0xc3, 0x87, 0xf1, 0x80, 0x70, 0xfc, 0x72, 0xd3, 0x8f, 0xc3, 0x80, 0x3c, 
	0x1e, 0x32, 0xd3, 0x1e, 0x0f, 0x00, 0x1f, 0x86, 0x32, 0xd3, 0x18, 0x7e, 0x00, 0x80, 0x03, 0x32, 
	0xd3, 0x30, 0x00, 0x40, 0xe0, 0x01, 0x72, 0xd3, 0xa0, 0x01, 0xc0, 0x7f, 0xe1, 0x72, 0xd3, 0xa1, 
	0xff, 0x80, 0x7f, 0xf9, 0x62, 0xd1, 0xa7, 0xff, 0x80, 0x3f, 0x3d, 0xc2, 0xd0, 0xef, 0x3f, 0x00, 
	0x00, 0x07, 0xc2, 0xd0, 0xf8, 0x00, 0x00, 0x41, 0xe3, 0x82, 0xd0, 0x71, 0xe0, 0x80, 0x3f, 0xf1, 
	0x82, 0xd0, 0x63, 0xff, 0x00, 0x1e, 0x01, 0x86, 0xd8, 0x60, 0x1e, 0x00, 0x00, 0x38, 0x82, 0xd0, 
	0x43, 0x00, 0x00, 0x01, 0xfe, 0xc0, 0xc0, 0xdf, 0xe0, 0x00, 0x1f, 0xf3, 0xfc, 0x0f, 0xf3, 0xfe, 
	0x00, 0x0f, 0x80, 0xff, 0x3f, 0xc0, 0x7c, 0x00, 0x00, 0x1c, 0x3f, 0xff, 0x8e, 0x00, 0x00, 0x00, 
	0x70, 0xff, 0xff, 0xc3, 0x80, 0x00, 0x07, 0xe3, 0x83, 0xf0, 0x71, 0xf8, 0x00, 0x00, 0x07, 0x01, 
	0xe0, 0x38, 0x00, 0x00, 0x00, 0x1e, 0x00, 0xc0, 0x1e, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x0f, 
	0xc0, 0x00, 0x00, 0x70, 0x00, 0x00, 0x03, 0x80, 0x00
};

void start(){
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(20,20);
  display.println("START");
  display.display();
}

void logo(){	
	display.drawBitmap(40, 3, epd_bitmap_logo__2_, 50, 47, WHITE); 
	display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(50,52);
  display.println("UNESA");
  display.display(); //tampilkan data
}

/*void menu(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(48,0);
  display.println("Menu");
  display.setCursor(18,18);
  display.println("Pribadi");
  display.setCursor(18,30);
  display.println("Bersama");
  display.setCursor(18,42);
  display.println("Set Jarak");
  display.display();
}*/

void pribadi(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(48,0);
  display.println("Menu");
  display.setCursor(0,18);
  display.println("-> Pribadi");
  display.setCursor(18,30);
  display.println("Bersama");
  display.setCursor(18,42);
  display.println("Set Jarak");
  display.display();
}

void bersama(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(48,0);
  display.println("Menu");
  display.setCursor(18,18);
  display.println("Pribadi");
  display.setCursor(0,30);
  display.println("-> Bersama");
  display.setCursor(18,42);
  display.println("Set Jarak");
  display.display();
}

void setjarak(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(48,0);
  display.println("Menu");
  display.setCursor(18,18);
  display.println("Pribadi");
  display.setCursor(18,30);
  display.println("Bersama");
  display.setCursor(0,42);
  display.println("-> Set Jarak");
  display.display();
}

/*void bersama2(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.println("Bersama:");
  display.setCursor(18,18);
  display.println("Wired");
  display.setCursor(18,30);
  display.println("Wireless");
  display.display();
}*/

void wired(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.println("Bersama:");
  display.setTextSize(1);
  display.setCursor(0,18);
  display.println("-> Wired");
  display.setCursor(18,30);
  display.println("Wireless");
  display.display();
}

void wireless(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.println("Bersama:");
  display.setCursor(18,18);
  display.println("Wired");
  display.setCursor(0,30);
  display.println("-> Wireless");
  display.display();
}

void TotalPU(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(24,0);
  display.println("Total Push Up:");
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(50,25);
  display.println(pushUpSkor);
  display.display();
}

void press(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,28);
  display.println("Press Button to Start");
  display.display();
}

void countdown(){
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(56,22);
  display.println("3");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(56,22);
  display.println("2");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(56,22);
  display.println("1");
  display.display();
  delay(1000);
  display.clearDisplay();
}

void checksum_serial() {
  jumlah_data = 0;

  if(nilai[0] == kode){
    if (nilai[1] == address_slave[slave_index-1]) {
      switch (nilai[2])
      {
      case 0x43:
        // Master meminta data
        // Proses mengirim data yang diminta
        // Value1 = random(1000,10000);
        // Value2 = random(10,100);
        
        nilai_kirim[0] = (Value1 >> 8) & 0xFF;
        nilai_kirim[1] = Value1 & 0xFF;
        nilai_kirim[2] = (Value2 >> 8) & 0xFF;
        nilai_kirim[3] = Value2 & 0xFF;

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
      Serial.println("Berbeda Address. Address ini : " + (String)address_slave[slave_index-1] + ". Sedangkan address yang dikirimkan" + (String)nilai[1]);
    }
  }else{
    Serial.println("GAGAL!");
  }

  // empty the receiver buffer
  // while (Serial2.available())
  // {
  //   Serial2.read();
  // }
}

void getSkor() {
  VLdistance = sensor.readRangeContinuousMillimeters();
  if(VLdistance>1000){
    VLdistance = 1000;
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("VL-mode");
  display.setCursor(0, 20);
  display.println("Count:");
  display.setCursor(80, 20);
  display.println(pushUpSkor);
  display.setCursor(0, 40);
  display.println(VLdistance);
  display.display();

  if (VLdistance <= pushUpThresholddown && flag == false && VLdistance != 0){
      flag = true;
  }
  if (VLdistance > pushUpThresholdVL && VLdistance <=pushUpThresholdup_up && flag == true){
      pushUpSkor += 1;
      flag = false;
  }

  Value1 = VLdistance;
  Value2 = pushUpSkor;
}

void setSlaveIndexPreferences(int index_nilai) {
  // menyimpan variabel slave index dalam integer ke dalam preferences
  // index nilai adalah nilai dari slave index
  slave_index_preferences.putInt("index",index_nilai);
}

void mqtt(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 1) {
    lastMsg = now;
    VLdistance = sensor.readRangeContinuousMillimeters();
    if(VLdistance>1000){
      VLdistance = 1000;
    }
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("VL-mode");
    display.setCursor(0, 30);
    display.println("Count : ");  
    display.setCursor(80, 30);
    display.println(pushUpSkor);
    display.display();

    char fullTopic[50]; // Array char untuk menyimpan topik MQTT lengkap
    snprintf(fullTopic, sizeof(fullTopic), "%s/%d", topicGraph, adressDevice);
    //int randomData = random(0, 101);
    client.publish(fullTopic, String(VLdistance).c_str());

    if (VLdistance <= pushUpThresholddown && flag == false && VLdistance != 0){
      flag = true;
    }
    if (VLdistance > pushUpThresholdVL && VLdistance <=pushUpThresholdup_up && flag == true){
        pushUpSkor += 1;
        char fullTopicScore[50];
        snprintf(fullTopicScore, sizeof(fullTopicScore), "%s/%d", topicScore, adressDevice);
        client.publish(fullTopicScore, String(pushUpSkor).c_str());
        flag = false;
    }

    client.loop();

    delay(100);
  }
}

void espnow(){
  VLdistance = sensor.readRangeContinuousMillimeters();
  if(VLdistance>1000){
    VLdistance = 1000;
  }
  dataSensor.VLdistance = smooth(VLdistance);

  waktu_skrg_vl = millis();
  if(waktu_skrg_vl-waktu_sblm_vl >= interval_vl){
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&dataSensor, sizeof(dataSensor));

    if (result == ESP_OK)
    {
      Serial.println("Sent with success");
    }
    else
    {
      Serial.println("Error sending the data");
    }

    waktu_sblm_vl = waktu_skrg_vl;
  }

  
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

  if (VLdistance <= pushUpThresholddown && flag == false && VLdistance != 0){
      flag = true;
  }
  if (VLdistance > pushUpThresholdVL && VLdistance <=pushUpThresholdup_up && flag == true){
      pushUpSkor += 1;
      flag = false;
  }
}

void setup()
{
  slave_index_preferences.begin("index",false);
  // mendapatkan nilai slave index dari preferences, jika tidak ada value didalamnya maka akan mendapatkan nilai 0
  slave_index = slave_index_preferences.getInt("index",0);
  // jika tidak ada nilai di dalam preferences maka set default slave indexnya 1
  if (slave_index == 0)
  {
    slave_index = 1;
  }

  push_up.begin("jaraksetup",false);
  coba.begin("lagi", false);
  
  Serial.begin(115200);
  Serial2.begin(9600);

  WiFi.mode(WIFI_STA);
  
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);

  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  if (!sensor.init())
  {
    display.println("Failed to initialize VL53L0X sensor!");
    display.display();
    while (1)
      ;
  }

  pinMode(up_pin, INPUT);
  pinMode(down_pin, INPUT);
  pinMode(set_pin, INPUT);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  

  sensor.startContinuous();
  //mode_tampilan = 10;
  
}

void loop()
{
  display.clearDisplay();
   switch(mode_tampilan){
    case 0:
      logo();
      delay(1000);
      mode_tampilan = 1;
      display.clearDisplay();
      break;

    case 1:
      tombol_down_ditekan = digitalRead(down_pin);
      if(tombol_down_ditekan != lastbuttonstate_down){
        if(tombol_down_ditekan == HIGH){
          tombol++;
          if(tombol>2){
            tombol = 0;
          }
        }
        lastbuttonstate_down = tombol_down_ditekan;
      }
      tombol_up_ditekan = digitalRead(up_pin);
      if(tombol_up_ditekan != lastbuttonstate_up){
        if(tombol_up_ditekan == HIGH){
          tombol--;
          if(tombol<0){
            tombol = 2;
          }
        }
        lastbuttonstate_up = tombol_up_ditekan;
      }
      if(tombol == 0){
        pribadi();
      }else if(tombol == 1){
        bersama();
      }else if(tombol == 2){
        setjarak();
      }
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if((tombol == 0) && (tombol_set_ditekan == HIGH)){
          display.clearDisplay();
          setup_wifi();
          client.setServer(mqttServer, 1883);
          client.setCallback(callback);
          mode_tampilan = 2;
        }else if((tombol == 1) && (tombol_set_ditekan == HIGH)){
          mode_tampilan = 3;
        }else if((tombol == 2) && (tombol_set_ditekan == HIGH)){
          mode_tampilan = 4;
        } 
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;

    case 2://menu mqtt
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if(tombol_set_ditekan == HIGH){
          countdown();
          mode_tampilan = 5; //menu mqtt
        }else if(tombol_set_ditekan == LOW){
          press();
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;

    case 3://menu bersama
      tombol_down_ditekan = digitalRead(down_pin);
      if(tombol_down_ditekan != lastbuttonstate_down){
        if(tombol_down_ditekan == HIGH){
          tombol++;
          if(tombol>1){
            tombol = 0;
          }
        }
        lastbuttonstate_down = tombol_down_ditekan;
      }
      tombol_up_ditekan = digitalRead(up_pin);
      if(tombol_up_ditekan != lastbuttonstate_up){
        if(tombol_up_ditekan == HIGH){
          tombol--;
          if(tombol<0){
            tombol = 1;
          }
        }
        lastbuttonstate_up = tombol_up_ditekan;
      }
      if(tombol == 0){
        wired();
      }else if(tombol == 1){
        wireless();
      }
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if((tombol == 0) && (tombol_set_ditekan == HIGH)){
          mode_tampilan = 12; //menu rs485
        }else if((tombol == 1) && (tombol_set_ditekan == HIGH)){
          mode_tampilan = 13; //menu espnow
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;

    case 4://menu setjarak
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if(tombol_set_ditekan == HIGH){
          countdown();
          mode_tampilan = 10;//menu setjarak
        }else{
          press();
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;

    case 5://MQTT
      jarak_set = push_up.getUInt("jaraksetup",0);
      pushUpThresholdVL = jarak_set-(0.3*jarak_set);
      mqtt();
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if(tombol_set_ditekan == HIGH){
          WiFi.disconnect();
          mode_tampilan = 9;
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;

    case 6://RS485
      jarak_set = push_up.getUInt("jaraksetup",0);
      pushUpThresholdVL = jarak_set-(0.3*jarak_set);
      if (i==0) 
      {
        getSkor();
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
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if(tombol_set_ditekan == HIGH){
          mode_tampilan = 9;
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;

    case 7://ESPNOW
      jarak_set = push_up.getUInt("jaraksetup",0);
      pushUpThresholdVL = jarak_set-(0.3*jarak_set);
      espnow();
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if(tombol_set_ditekan == HIGH){
          mode_tampilan = 9;
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;

    case 8:
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(40,0);
      display.println("SETJARAK:");

      //Mencari rata-rata nilai ketinggian dada
      VLdistance = sensor.readRangeContinuousMillimeters();
      waktu_skrg_setjarak = millis();
      if(waktu_skrg_setjarak-waktu_sblm_setjarak >= interval_setjarak){
        jumlah_ratarata++;
        total_ratarata = total_ratarata + VLdistance;
        rata_rata_jarak = total_ratarata/jumlah_ratarata;
        waktu_sblm_setjarak = waktu_skrg_setjarak;
      }
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(50,25);
      display.println(rata_rata_jarak);
      display.display();

      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if(tombol_set_ditekan == HIGH){
          push_up.putUInt("jaraksetup", rata_rata_jarak);
          jumlah_ratarata = 0;
          total_ratarata = 0;
          mode_tampilan = 11;
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;

    case 9:
      TotalPU();
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if(tombol_set_ditekan == HIGH){
          mode_tampilan = 1;
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;
    
    case 10: //SETJARAK
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(10,0);
      display.println("SET JARAK");
      waktu_skrg_countdown_setjarak = millis();
      if(waktu_skrg_countdown_setjarak-waktu_sblm_countdown_setjarak >= interval_countdown_setjarak){
        setjarak_countdown--;
        waktu_sblm_countdown_setjarak = waktu_skrg_countdown_setjarak;
      }
      //Mencari rata-rata nilai ketinggian dada
      VLdistance = sensor.readRangeContinuousMillimeters();
      waktu_skrg_setjarak = millis();
      if(waktu_skrg_setjarak-waktu_sblm_setjarak >= interval_setjarak){
        jumlah_ratarata++;
        total_ratarata = total_ratarata + VLdistance;
        rata_rata_jarak = total_ratarata/jumlah_ratarata;
        waktu_sblm_setjarak = waktu_skrg_setjarak;
      }
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(50,25);
      display.println(setjarak_countdown);
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(45,50);
      display.println(rata_rata_jarak);
      display.display();
      if(setjarak_countdown <= 0){
        push_up.putUInt("jaraksetup", rata_rata_jarak);
        jumlah_ratarata = 0;
        total_ratarata = 0;
        setjarak_countdown = 6;
        mode_tampilan = 11;
      }
      break;
      
    case 11:
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(24,0);
      display.println("Rata-rata Tinggi");
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(40,25);
      display.println(rata_rata_jarak);
      display.display();
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if(tombol_set_ditekan == HIGH){
          mode_tampilan = 1;
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;
    case 12://menu rs485
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if(tombol_set_ditekan == HIGH){
          countdown();
          mode_tampilan = 6; //rs485
        }else if(tombol_set_ditekan == LOW){
          press();
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;
    case 13://menu espnow
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan != lastbuttonstate_set){
        if(tombol_set_ditekan == HIGH){
          countdown();
          mode_tampilan = 7; //espnow
        }else if(tombol_set_ditekan == LOW){
          press();
        }
        lastbuttonstate_set = tombol_set_ditekan;
      }
      break;
  }

}