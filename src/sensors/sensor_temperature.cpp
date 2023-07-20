#include <sensors/sensor_temperature.h>

TemperatureSensor::TemperatureSensor(MqttManager &manager) : mqttManager(manager), Sensor()
{
}

void TemperatureSensor::setup(int pin, String name)
{
    setDeviceName(name);
    setPin(pin);
    if (!isValidPins() || !isEnabled())
    {
        if (isEnabled() && log_enabled)
        {
            Serial.println("Invalid pins");
            this->setStatus(SensorStatus::InvalidPins);
        }
        return;
    }

    ds = OneWire(this->getPin());
    sensors.setOneWire(&ds);
    sensors.begin();
    this->setStatus(SensorStatus::OkSetup);
}

void TemperatureSensor::loop(unsigned int timeout)
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

void TemperatureSensor::publish()
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

    float temp = getValue("temperature");

    StaticJsonDocument<200> doc;
    JsonObject obj = doc.createNestedObject("sensor");
    obj["name"] = this->getDeviceName();
    obj["temperature"] = temp;
    obj["status"] = this->getStatus();

    mqttManager.publish(mqtt_topic_water_temperature, doc);
}

void TemperatureSensor::readSensorValue()
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

    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);

    if (temp == -127.00)
    {
        if (log_enabled)
            Serial.println("Error: Could not read temperature data");
        setValue("temperature", NAN);
        this->setStatus(SensorStatus::FailedRead);
        return;
    }

    if (log_enabled)
    {
        Serial.print("Temperature = ");
        Serial.print(temp);
        Serial.println(" °C");
    }

    setValue("temperature", temp);
    this->setStatus(SensorStatus::OkRead);
}

void TemperatureSensor::update(StaticJsonDocument<200> value)
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
        this->setPin(pin);
        // TODO: check if pin wont be used by other sensor

        if (!isValidPins())
        {
            if (log_enabled)
                Serial.println("Invalid pins");
            return;
        }

        ds.~OneWire();
        ds = OneWire(this->getPin());
        sensors.setOneWire(&ds);
        sensors.begin();
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
        obj["temperature"] = getValue("temperature");
        obj["status"] = this->getStatus();
        obj["pin"] = this->getPin();
        obj["type"] = this->getSensorType();
        obj["enabled"] = this->isEnabled();

        mqttManager.publish(mqtt_topic_water_temperature, doc);
    }
    else if (log_enabled)
    {
        Serial.println("Invalid command " + String(command));
    }
    // TODO: add other commands
}