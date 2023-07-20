#include <sensors/sensor_water_level.h>

WaterLevelSensor::WaterLevelSensor(MqttManager &manager) : mqttManager(manager), Sensor()
{
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

    pinMode(this->getPin(), INPUT);
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
    JsonObject obj = doc.createNestedObject("sensor");
    obj["name"] = this->getDeviceName();
    obj["water_level"] = level;
    obj["status"] = this->getStatus();

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

    int liquidLevel = digitalRead(this->getPin());

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

    if (strcmp(command, "enable") == 0)
    {
        this->enable();
    }
    else if (strcmp(command, "disable") == 0)
    {
        this->disable();
    }
    else if (strcmp(command, "set_pin") == 0)
    {
        int pin = value["pin"];
        // TODO: check if pin wont be used by other sensor
        this->setPin(pin);

        if (!isValidPins())
        {
            if (log_enabled)
                Serial.println("Invalid pins");
            return;
        }

        pinMode(this->getPin(), INPUT);
    }
    else if (strcmp(command, "set_name") == 0)
    {
        const char *topic = value["new_name"];
        this->setDeviceName(topic);
    }
    else if (strcmp(command, "get_status") == 0)
    {
        StaticJsonDocument<200> doc;
        JsonObject obj = doc.createNestedObject("sensor");

        obj["name"] = this->getDeviceName();
        obj["water_level"] = getValue("water_level");
        obj["status"] = this->getStatus();
        obj["pin"] = this->getPin();
        obj["type"] = this->getSensorType();
        obj["enabled"] = this->isEnabled();

        mqttManager.publish(mqtt_topic_water_level, doc);
    }
    else if (log_enabled)
    {
        Serial.println("Invalid command " + String(command));
    }
    // TODO: add other commands
}