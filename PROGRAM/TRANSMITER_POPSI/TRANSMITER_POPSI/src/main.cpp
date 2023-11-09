#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//-----------------------------DEFINE MQTT-------------------------------------------------------------

const char* ssid = "Kurniasyah"; // Nama jaringan WiFi
const char* password = "Izzulizzam2002"; // Kata sandi WiFi
//const char* ssid = "TP-Link_AFBC"; // Nama jaringan WiFi
//const char* password = "Penelitian2023"; // Kata sandi WiFi
const char* mqttServer = "broker.mqtt-dashboard.com"; // Alamat broker MQTT
int mqttPort = 1883; // Port broker MQTT

WiFiClient espClient;
PubSubClient client(espClient);

int adressDevice = 57; // Nilai integer
const char* topicPrefix = "/esp32/data/sensor"; // String topik awal
unsigned long lastMsg = 0;

//-----------------------------DEFINE MQTT-------------------------------------------------------------

int pushUpSkor = 0, pushUpThresholdVL = 15;
bool flag = false;

VL53L0X sensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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

//-------------------------------------RUN----------------------------------------


//-------------------------------------RUN----------------------------------------


void loop()
{
 if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 1) {
    lastMsg = now;

    uint16_t VLdistance = sensor.readRangeContinuousMillimeters()/10;
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

    char fullTopic[50]; // Array char untuk menyimpan topik MQTT lengkap
    snprintf(fullTopic, sizeof(fullTopic), "%s/%d", topicPrefix, adressDevice);
    //int randomData = random(0, 101);
    client.publish(fullTopic, String(VLdistance).c_str());
    Serial.println("Data terkirim ke topik: " + String(VLdistance));

    if (VLdistance <= pushUpThresholdVL && flag == false && VLdistance != 0)
    {
      pushUpSkor += 1;
      Serial.print("Skor Push Up = ");
      Serial.println(pushUpSkor);
      flag = true;
    }
    if (VLdistance > pushUpThresholdVL)
    {
      flag = false;
    }
  client.loop();
  delay(10);   // Kirim data setiap 1 detik*/
  }
}