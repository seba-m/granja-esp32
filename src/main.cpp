#include <Arduino.h>
#include <Wire.h>

#include <sensors/sensor_tds.cpp>
#include <sensors/sensor_water_level.cpp>
#include <sensors/sensor_turbidity.cpp>
#include <sensors/sensor_temperature.cpp>

#include <communication/mqtt_manager.cpp>

MqttManager mqttManager;

MeasureTDS measureTDS = MeasureTDS(mqttManager);
WaterLevelSensor waterLevelSensor = WaterLevelSensor(mqttManager);
TurbiditySensor turbiditySensor = TurbiditySensor(mqttManager);
TemperatureSensor temperatureSensor = TemperatureSensor(mqttManager);

void setup()
{
    Serial.begin(9600);
    // mqtt configuration
    mqttManager.setup();

    // sensors configuration
    //measureTDS.setup();
    //waterLevelSensor.setup();
    //temperatureSensor.setup();
}

void loop()
{
    // mqtt loop
    mqttManager.loop();

    // sensors loop
    //measureTDS.loop();
    //waterLevelSensor.loop();
    //turbiditySensor.loop();
    //temperatureSensor.loop();
}