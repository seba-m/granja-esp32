#include <sensors/sensor_temperature.h>

TemperatureSensor::TemperatureSensor(MqttManager &manager) : mqttManager(manager), Sensor(temperatureSensorPin) {
    setDeviceName("temperature");
}

void TemperatureSensor::setup()
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

    ds = OneWire(temperatureSensorPin);
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
    doc["type"] = "sensor";
    doc["sensor"] = "temperature";
    doc["temperature"] = temp;
    doc["status"] = this->getStatus();

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
    
    if (command == "enable")
    {
        this->enable();
    } else if (command == "disable")
    {
        this->disable();
    } else if (command == "set_pin")
    {
        int pin = value["pin"];
        this->setPin(pin);
        //TODO: check if pin wont be used by other sensor
    } else if (command == "set_name")
    {
        const char *topic = value["new_name"];
        this->setDeviceName(topic);
    } else {
        if (log_enabled)
            Serial.println("Invalid command" + String(command));
    }
    //TODO: add other commands
}