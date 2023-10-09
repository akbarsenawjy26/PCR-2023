#include <espnow.h>
#include <WiFi.h>

typedef struct struct_message
{
  uint16_t VLdistance;
} struct_message;

struct_message dataSensor;

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  memcpy(&dataSensor, incomingData, sizeof(dataSensor));
  Serial.println(String(dataSensor.VLdistance));
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  // put your main code here, to run repeatedly:
}