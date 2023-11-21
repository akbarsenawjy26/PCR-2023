#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <PubSubClient.h>
#include <Adafruit_SH1106.h>

#define OLED_SDA 21
#define OLED_SCL 22

//-----------------------------DEFINE MQTT-------------------------------------------------------------

//const char* ssid = "Kurniasyah"; // Nama jaringan WiFi
//const char* password = "Izzulizzam2002"; // Kata sandi WiFi
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

//-----------------------------DEFINE MQTT-------------------------------------------------------------

int pushUpSkor = 0, pushUpThresholdVL = 100;
bool flag = false;

VL53L0X sensor;
Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

int i = 0;
int x = 0;
double rata;
bool start = false;

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

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  //WiFi.mode(WIFI_STA);
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

  sensor.startContinuous();
}

void loop()
{
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
    Serial.println("Data terkirim ke topik: " + String(VLdistance));


    if (VLdistance <= pushUpThresholdVL && flag == false && VLdistance != 0)
    {
      pushUpSkor += 1;
      Serial.print("Skor Push Up = ");
      Serial.println(pushUpSkor);
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