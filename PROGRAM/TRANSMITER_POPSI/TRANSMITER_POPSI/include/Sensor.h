#include <VL53L0X.h>
VL53L0X sensor;

int dataSensor, avgDataSensor, divider = 30;
uint16_t VLdistance;

void sensorSetup()
{
    sensor.init();
    sensor.setTimeout(500);
    if (!sensor.init())
    {
        Serial.println("Failed to initialize VL53L0X sensor!");
        while (1)
            ;
    }

    sensor.startContinuous();
}