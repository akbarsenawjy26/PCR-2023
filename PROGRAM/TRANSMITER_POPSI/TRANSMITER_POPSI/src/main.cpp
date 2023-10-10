#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

uint8_t broadcastAddress[] = {0xC8, 0xF0, 0x9E, 0xEA, 0xC2, 0x80}; // C8:F0:9E:EA:C2:80
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

int pushUpSkor = 0, pushUpThresholdVL = 15;
bool flag = false;

VL53L0X sensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
  uint16_t VLdistance = sensor.readRangeContinuousMillimeters() / 10;
  dataSensor.VLdistance = sensor.readRangeContinuousMillimeters() / 10;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&dataSensor, sizeof(dataSensor));

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
  delay(300);

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

  delay(100);
}