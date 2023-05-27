#include <sensors/sensor_water_level.h>

WaterLevelSensor::WaterLevelSensor(MqttManager &manager) : mqttManager(manager) {}

void WaterLevelSensor::setup()
{
    if (waterLevelSensorPin < 0)
        return;

    pinMode(waterLevelSensorPin, INPUT);
}

void WaterLevelSensor::loop(unsigned int timeout)
{
    if (waterLevelSensorPin < 0)
        return;

    if (millis() - timepoint > timeout)
    {
        timepoint = millis();
        int liquidLevel = digitalRead(waterLevelSensorPin);

        if (log_enabled)
        {
            Serial.print("Liquid level= ");
            Serial.println(liquidLevel, DEC);
        }

        mqttManager.publish(mqtt_topic_water_level, String(liquidLevel));
    }
}

void WaterLevelSensor::update()
{
    // No se necesita implementar nada aquí para este ejemplo
}
