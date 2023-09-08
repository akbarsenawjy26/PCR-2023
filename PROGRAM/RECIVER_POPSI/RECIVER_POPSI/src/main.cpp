#include <Arduino.h>

char myData[50];

void setup()
{
  Serial.begin(57600);
  Serial2.begin(9600);
}

void loop()
{
  if (Serial2.available() > 0)
  {
    byte m = Serial2.readBytesUntil('\n', myData, 50);
    myData[m] = '\0';
    Serial.println(myData);
  }
  delay(1000);
}