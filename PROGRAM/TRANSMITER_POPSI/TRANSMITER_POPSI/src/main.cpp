#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "TP-Link_AFBC"; // Nama jaringan WiFi
const char* password = "Penelitian2023"; // Kata sandi WiFi
const char* mqttServer = "broker.mqtt-dashboard.com"; // Alamat broker MQTT
int mqttPort = 1883; // Port broker MQTT

WiFiClient espClient;
PubSubClient client(espClient);

int sensorValue = 56; // Nilai integer
const char* topicPrefix = "/esp32/data/sensor"; // String topik awal

void callback(char* topic, byte* payload, unsigned int length) {
  // Callback jika Anda ingin menangani pesan yang diterima dari broker MQTT
  // Anda dapat menambahkan logika khusus di sini.
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  // Gabungkan nilai integer dan string
  char fullTopic[50]; // Array char untuk menyimpan topik MQTT lengkap
  snprintf(fullTopic, sizeof(fullTopic), "%s/%d", topicPrefix, sensorValue);

  int randomData = random(0, 101);

  client.publish(fullTopic, String(randomData).c_str());

  //client.publish(fullTopic, randomData);
  Serial.println("Data terkirim ke topik: " + String(fullTopic));
  client.loop();
  delay(1000);
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
