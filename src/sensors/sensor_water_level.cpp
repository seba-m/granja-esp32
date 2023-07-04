#include <sensors/sensor_water_level.h>

WaterLevelSensor::WaterLevelSensor(MqttManager &manager) : mqttManager(manager), Sensor() {
}

void WaterLevelSensor::setup(int pin, String deviceName)
{
    setPin(pin);
    setDeviceName(deviceName);
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
        readSensorValue();
        publish();
        this->setStatus(SensorStatus::OkLoop);
        timepoint = millis();
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
    doc["sensor"] = this->getDeviceName();
    doc[this->getDeviceName()] = level;
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

void WaterLevelSensor::update(StaticJsonDocument<200> value)
{
    if (!value.containsKey("command"))
    {
        if (log_enabled)
            Serial.println("No command in message");
        return;
    }

    const char *command = value["command"];

    if (command == "enable")
    {
        this->enable();
    }
    else if (command == "disable")
    {
        this->disable();
    }
    else if (command == "set_pin")
    {
        int pin = value["pin"];
        this->setPin(pin);
        // TODO: check if pin wont be used by other sensor
    }
    else if (command == "set_name")
    {
        const char *topic = value["new_name"];
        this->setDeviceName(topic);
    }
    // TODO: add other commands
}