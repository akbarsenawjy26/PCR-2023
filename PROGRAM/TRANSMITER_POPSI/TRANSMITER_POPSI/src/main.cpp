#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <PubSubClient.h>
#include <Adafruit_SH1106.h>
#include <Preferences.h>

#define OLED_SDA 21
#define OLED_SCL 22

Preferences push_up;
int jarak_set = 0;
int pushUpSkor = 0, pushUpThresholdVL = 100;
bool flag = false;

VL53L0X sensor;
Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

int Value1 = 9999, Value2 = 999;

const int panjang_data = 1+1+1;
int i = 0;
byte nilai[panjang_data];

int slave_index = 1; // PENTING!!! isi sinteger sesuai slave ke berapa yang akan di upload
const int banyak_slave = 3;
byte address_slave[banyak_slave] = {0x45,0x46,0x47};

byte kode = 0x42;

const int panjang_data_kirim = 4;
byte nilai_kirim[panjang_data_kirim];
byte checksum[2];
int jumlah_data = 0;

int jumlah=21;

//----------------IKMAL-----------------------
const char* ssid = "TP-Link_AFBC"; // Nama jaringan WiFi
const char* password = "Penelitian2023"; // Kata sandi WiFi
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
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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
int mode_tampilan = 0;

const int up_pin = 2;
const int down_pin = 15;
const int set_pin = 32;

byte lastbuttonstate_up = 0;
byte lastbuttonstate_down = 0;
byte lastbuttonstate_set = 0;

//-----------------------------Button Finish------------

void start(){
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(20,20);
  display.println("START");
  display.display();
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
  display.println(jumlah);
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
  uint16_t VLdistance = sensor.readRangeContinuousMillimeters();
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

  if (VLdistance <= pushUpThresholdVL && flag == false && VLdistance != 0)
  {
    pushUpSkor += 1;
    // Serial.print("Jarak = ");
    // Serial.println(VLdistance);
    flag = true;
  }
  if (VLdistance > pushUpThresholdVL)
  {
    flag = false;
  }

  Value1 = VLdistance;
  Value2 = pushUpSkor;
}

void mqtt(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 1) {
    lastMsg = now;

    uint16_t VLdistance = sensor.readRangeContinuousMillimeters();
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
    if (VLdistance <= pushUpThresholdVL && flag == false && VLdistance != 0)
    {
      pushUpSkor += 1;
      char fullTopicScore[50];
      snprintf(fullTopicScore, sizeof(fullTopicScore), "%s/%d", topicScore, adressDevice);
      client.publish(fullTopicScore, String(pushUpSkor).c_str());
      flag = true;
    }
    if (VLdistance > pushUpThresholdVL)
    {
      flag = false;
    }
    client.loop();
    delay(100);
  }
}

void setup()
{
  push_up.begin("data",false);
  
  Serial.begin(115200);
  Serial2.begin(9600);
  //WiFi.mode(WIFI_STA);

  setup_wifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);

  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);

  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  if (!sensor.init())
  {
    Serial.println("Failed to initialize VL53L0X sensor!");
    while (1)
      ;
  }

  pinMode(up_pin, INPUT);
  pinMode(down_pin, INPUT);
  pinMode(set_pin, INPUT);
  

  sensor.startContinuous();
}

void loop()
{
  display.clearDisplay();
   switch(mode_tampilan){
    case 0:
      start();
      mode_tampilan = 1;
      display.clearDisplay();
      break;

    case 1:
      tombol_down_ditekan = digitalRead(down_pin);
      if(tombol_down_ditekan == HIGH){
        tombol++;
        if(tombol>2){
          tombol = 0;
        }
      }
      if(tombol == 0){
        pribadi();
      }else if(tombol == 1){
        bersama();
      }else if(tombol == 2){
        setjarak();
      }
      tombol_set_ditekan = digitalRead(set_pin);
      if((tombol == 0) && (tombol_set_ditekan == HIGH)){
        mode_tampilan = 2;
      }else if((tombol == 1) && (tombol_set_ditekan == HIGH)){
        mode_tampilan = 3;
      }else if((tombol == 2) && (tombol_set_ditekan == HIGH)){
        mode_tampilan = 4;
      }
      break;

    case 2:
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan == HIGH){
        countdown();
        mode_tampilan == 5; //menu mqtt
      }else{
        press();
      }
      break;

    case 3:
      tombol_down_ditekan = digitalRead(down_pin);
      if(tombol_down_ditekan == HIGH){
        tombol++;
        if(tombol>1){
          tombol = 0;
        }
      }
      if(tombol == 0){
        wired();
      }else if(tombol == 1){
        wireless();
      }
      tombol_set_ditekan = digitalRead(set_pin);
      if((tombol == 0) && (tombol_set_ditekan == HIGH)){
        mode_tampilan = 6; //menu rs485
      }else if((tombol == 1) && (tombol_set_ditekan == HIGH)){
        mode_tampilan = 7; //menu espnow
      }
      break;

    case 4:
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan == HIGH){
        countdown();
        mode_tampilan = 8;//menu setjarak
      }else{
        press();
      }
      break;

    case 5:
      jarak_set = push_up.getUInt("jaraksetup",0);
      pushUpThresholdVL = jarak_set-(0.3*jarak_set);
      mqtt();
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan == HIGH){
        mode_tampilan = 9;
      }
      break;

    case 6:
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
      if(tombol_set_ditekan == HIGH){
        mode_tampilan = 9;
      }
      break;

    case 7:
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(40,0);
      display.println("ESPNOW:");
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan == HIGH){
        mode_tampilan = 9;
      }
      break;

    case 8:
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(40,0);
      display.println("SETJARAK:");
      push_up.putUInt("jaraksetup", 150);
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan == HIGH){
        mode_tampilan = 9;
      }
      break;

    case 9:
      TotalPU();
      tombol_set_ditekan = digitalRead(set_pin);
      if(tombol_set_ditekan == HIGH){
        mode_tampilan = 1;
      }
      break;

  }

}