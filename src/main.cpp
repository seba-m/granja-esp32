#include <Arduino.h>
#include <Wire.h>
#include <settings.h>

#include <sensors/sensor_tds.h>
#include <sensors/sensor_water_level.h>
#include <sensors/sensor_turbidity.h>
#include <sensors/sensor_temperature.h>
#include <sensors/sensor_dht.h>

#include <actuators/pump.h>

#include <communication/mqtt_manager.h>

PumpController controller;

MqttManager mqttManager;

MeasureTDS measureTDS(mqttManager);
WaterLevelSensor waterLevelSensor(mqttManager);
TurbiditySensor turbiditySensor(mqttManager);
TemperatureSensor temperatureSensor(mqttManager);
DHTSensor dhtSensor(mqttManager);

void setup()
{
    Serial.begin(115200);

    Serial.println("-----------------------");
    Serial.println("Starting...");

    // mqtt configuration
    mqttManager.setup();

    mqttManager.attach(&dhtSensor);
    mqttManager.attach(&measureTDS);
    mqttManager.attach(&waterLevelSensor);
    mqttManager.attach(&turbiditySensor);
    mqttManager.attach(&temperatureSensor);

    // sensors configuration
    measureTDS.setup();
    waterLevelSensor.setup();
    temperatureSensor.setup();
    dhtSensor.setup();

    Serial.println("Started");
    Serial.println("-----------------------");
}

void loop()
{
    // mqtt loop
    mqttManager.loop();

    // sensors loop
    measureTDS.loop();
    waterLevelSensor.loop();
    turbiditySensor.loop();
    temperatureSensor.loop();
    dhtSensor.loop();
}