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
WaterLevelSensor waterLevelSensor2(mqttManager);
WaterLevelSensor waterLevelSensor3(mqttManager);
TurbiditySensor turbiditySensor(mqttManager);
TemperatureSensor temperatureSensor(mqttManager);
DHTSensor dhtSensor(mqttManager);

void setup()
{
    Serial.begin(115200);

    if (log_enabled) {
        Serial.println("-----------------------");
        Serial.println("Starting...");
    }

    // mqtt configuration
    mqttManager.setup();

    mqttManager.attach(&dhtSensor);
    mqttManager.attach(&measureTDS);
    mqttManager.attach(&waterLevelSensor);
    mqttManager.attach(&turbiditySensor);
    mqttManager.attach(&temperatureSensor);

    mqttManager.attach(&waterLevelSensor2);
    mqttManager.attach(&waterLevelSensor3);

    // sensors configuration
    measureTDS.setup(tdsSensorPin);
    temperatureSensor.setup(temperatureSensorPin);
    dhtSensor.setup(dhtSensorPin);
    turbiditySensor.setup(turbiditySensorPin);
    waterLevelSensor.setup(waterLevelSensorPin);

    waterLevelSensor2.setup(waterLevelSensorPin2, "water_level2");
    waterLevelSensor3.setup(waterLevelSensorPin3, "water_level3");

    if (log_enabled)
    {
        Serial.println("Started");
        Serial.println("-----------------------");
    }
}

void loop()
{
    // mqtt loop
    mqttManager.loop();

    // sensors loop
    measureTDS.loop();
    waterLevelSensor.loop();
    temperatureSensor.loop();
    dhtSensor.loop();
    turbiditySensor.loop();

    waterLevelSensor2.loop();
    waterLevelSensor3.loop();
}