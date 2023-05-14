#include <Arduino.h>
#include <Wire.h>
#include <sensors/sensor_tds.cpp>
#include <sensors/sensor_water_level.cpp>
#include <sensors/sensor_turbidity.cpp>
#include <sensors/sensor_temperature.cpp>

#define waterLevelSensorPin 5
#define turbiditySensorPin 6
#define TdsSensorPin 7
#define temperatureSensorPin 8

MeasureTDS measureTDS;
WaterLevelSensor waterLevelSensor;
TurbiditySensor turbiditySensor;
TemperatureSensor temperatureSensor;

void setup()
{
    measureTDS.setup(TdsSensorPin);
    waterLevelSensor.setup(waterLevelSensorPin);
    turbiditySensor.setup(turbiditySensorPin);
    temperatureSensor.setup(temperatureSensorPin);
}

void loop()
{
    measureTDS.loop();
    waterLevelSensor.loop();
    turbiditySensor.loop();
    temperatureSensor.loop();
}