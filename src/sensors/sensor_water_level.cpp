#include <sensors/sensor_water_level.h>

WaterLevelSensor::WaterLevelSensor(MqttManager &manager) : mqttManager(manager), Sensor(waterLevelSensorPin) {
    setDeviceName("water_level");
}

void WaterLevelSensor::setup()
{
    if (!isValidPins() || !isEnabled())
    {
        if (isEnabled() && log_enabled)
        {
            Serial.println("Invalid pins");
            this->setStatus(SensorStatus::InvalidPins);
        }
        return;
    }

    pinMode(waterLevelSensorPin, INPUT);
    this->setStatus(SensorStatus::OkSetup);
}

void WaterLevelSensor::loop(unsigned int timeout)
{
    if (!isValidPins() || !isEnabled())
    {
        if (isEnabled() && log_enabled)
        {
            Serial.println("Invalid pins");
            this->setStatus(SensorStatus::InvalidPins);
        }
        return;
    }

    if (millis() - timepoint > timeout)
    {
        timepoint = millis();
        readSensorValue();
        publish();
        this->setStatus(SensorStatus::OkLoop);
    }
}

void WaterLevelSensor::publish()
{
    if (!isValidPins() || !isEnabled())
    {
        if (isEnabled() && log_enabled)
        {
            Serial.println("Invalid pins");
            this->setStatus(SensorStatus::InvalidPins);
        }
        return;
    }

    float level = getValue("water_level");

    StaticJsonDocument<200> doc;
    doc["type"] = "sensor";
    doc["sensor"] = "water_level";
    doc["water_level"] = level;
    doc["status"] = this->getStatus();

    mqttManager.publish(mqtt_topic_water_level, doc);
}

void WaterLevelSensor::readSensorValue()
{
    if (!isValidPins() || !isEnabled())
    {
        if (isEnabled() && log_enabled)
        {
            Serial.println("Invalid pins");
            this->setStatus(SensorStatus::InvalidPins);
        }
        return;
    }

    int liquidLevel = digitalRead(waterLevelSensorPin);

    if (log_enabled)
    {
        Serial.print("Water level: ");
        Serial.println(liquidLevel);
    }

    setValue("water_level", liquidLevel);
    this->setStatus(SensorStatus::OkRead);
}

void WaterLevelSensor::update()
{
    // TODO: implement update
}