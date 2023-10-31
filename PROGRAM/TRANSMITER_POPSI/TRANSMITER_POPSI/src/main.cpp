#include <WiFi.h>
#include <esp_now.h>

uint16_t VLdistance = 100;
int i = 0;

uint8_t broadcastAddress[] = {0x08, 0xB6, 0x1F, 0x71, 0xBB, 0x6C}; // C8:F0:9E:EA:C2:80
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

unsigned long waktu_skrg = 0, waktu_sblm = 0, interval = 100;

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
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

}

void loop()
{
  waktu_skrg = millis();
  if(waktu_skrg-waktu_sblm >= interval){

    dataSensor.VLdistance = VLdistance*i;
    i++;
    if(i>30){
      i=0;
    }
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&dataSensor, sizeof(dataSensor));

    if (result == ESP_OK)
    {
      Serial.println("Sent with success");
    }
    else
    {
      Serial.println("Error sending the data");
    }

    waktu_sblm = waktu_skrg;
  }

}