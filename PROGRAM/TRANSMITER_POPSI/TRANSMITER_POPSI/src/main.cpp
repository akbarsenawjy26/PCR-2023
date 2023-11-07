#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PubSubClient.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

const char* ssid = "TP-Link_AFBC";
const char* password = "Penelitian2023";
//const char* ssid = "Lab Mikroprosessor";
//const char* password = "mikrojos08";
const char* mqtt_server = "broker.mqtt-dashboard.com";

//-------------------------------MQTT IOT--------------------------------------------------
WiFiClient espClient;
PubSubClient client(espClient);

int AdressDevice = 01; // Nilai integer

const char* topicPrefix_Graph = "/esp32-mqtt/PushUpGrafik/msg/graph";
const char* topicPrefix_Score = "/esp32-mqtt/PushUpGrafik/msg/score";

unsigned long lastMsg = 0;

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
////-------------------------------MQTT IOT--------------------------------------------------

////-------------------------------MAIN------------------------------------------------------

int pushUpSkor = 0, pushUpThresholdVL = 15; 
bool flag = false;

VL53L0X sensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void run()
{
  uint16_t VLdistance = sensor.readRangeContinuousMillimeters() / 10;

  char fullTopic_G[50];
  char fullTopic_S[50];  
  snprintf(fullTopic_G, sizeof(fullTopic_G), "%s/%d", topicPrefix_Graph, AdressDevice);
  snprintf(fullTopic_S, sizeof(fullTopic_S), "%s/%d", topicPrefix_Score, AdressDevice);

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
  char PushUpGrafik[8];
  dtostrf(VLdistance, 3, 5, PushUpGrafik);
  Serial.print("Skor: ");
  Serial.println(PushUpGrafik);
  client.publish(fullTopic_G, PushUpGrafik);  

  if (VLdistance <= pushUpThresholdVL && flag == false && VLdistance != 0)
  {
    pushUpSkor += 1;
    Serial.print("Skor Push Up = ");
    Serial.println(pushUpSkor);
    flag = true;
    char ScorePushUp[8];
    dtostrf(pushUpSkor, 5, 0, ScorePushUp);
    Serial.print("Skor: ");
    Serial.println(ScorePushUp);
    client.publish(fullTopic_S, ScorePushUp);
  }
  if (VLdistance > pushUpThresholdVL)
  {
    flag = false;
  }

  delay(100);
}

////-------------------------------MAIN------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  //MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //WiFi.mode(WIFI_STA);

  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
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
  if (now - lastMsg > 1000) {
    lastMsg = now;
  
  char fullTopic[50]; // Array char untuk menyimpan topik MQTT lengkap
  snprintf(fullTopic, sizeof(fullTopic), "%s/%d", topicPrefix_Graph, AdressDevice);

  int randomData = random(0, 101);
  Serial.println(randomData);

  client.publish(fullTopic, String(randomData).c_str());
  //run();
  Serial.println("Data terkirim ke topik: " + String(fullTopic));
  client.loop();
  //delay(1000);
}
}