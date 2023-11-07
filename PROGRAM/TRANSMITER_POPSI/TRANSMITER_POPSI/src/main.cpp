#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "TP-Link_AFBC"; // Nama jaringan WiFi
const char* password = "Penelitian2023"; // Kata sandi WiFi
const char* mqttServer = "broker.mqtt-dashboard.com"; // Alamat broker MQTT
int mqttPort = 1883; // Port broker MQTT

WiFiClient espClient;
PubSubClient client(espClient);

int adressDevice = 57; // Nilai integer
const char* topicPrefix = "/esp32/data/sensor"; // String topik awal
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

void setup() {
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
  
  // Gabungkan nilai integer dan string
  char fullTopic[50]; // Array char untuk menyimpan topik MQTT lengkap
  snprintf(fullTopic, sizeof(fullTopic), "%s/%d", topicPrefix, adressDevice);
  int randomData = random(0, 101);
  client.publish(fullTopic, String(randomData).c_str());

  //client.publish(fullTopic, randomData);
  Serial.println("Data terkirim ke topik: " + String(randomData));
  client.loop();
  delay(1000);
  }
  /* 
  // Kirim data dengan topik lengkap
  if (client.publish(fullTopic, "horee")) {
    Serial.println("Data terkirim ke topik: " + String(fullTopic));
  } else {
    Serial.println("Gagal mengirim data");
  }

  client.loop();
  delay(1000); // Kirim data setiap 1 detik*/
}
