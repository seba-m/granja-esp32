#include <sensors/sensor_turbidity.h>

TurbiditySensor::TurbiditySensor(MqttManager &manager) : mqttManager(manager), Sensor() {
}

void TurbiditySensor::setup(int pin, String name)
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
    this->setStatus(SensorStatus::OkSetup);
}

void TurbiditySensor::loop(unsigned int timeout)
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

void TurbiditySensor::publish()
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

    float turbidity = getValue("turbidity");

    if (log_enabled)
    {
        Serial.print("Turbidity: ");
        Serial.println(turbidity);
    }

    StaticJsonDocument<200> doc;
    doc["type"] = "sensor";
    doc["sensor"] = this->getDeviceName();
    doc[this->getDeviceName()] = turbidity;
    doc["status"] = this->getStatus();

    mqttManager.publish(mqtt_topic_turbidity, doc);
}

void TurbiditySensor::readSensorValue()
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

    int sensorValue = analogRead(this->getPin());

    if (log_enabled)
    {
        Serial.print("Turbidity value = ");
        Serial.println(sensorValue, DEC);
    }

    setValue("turbidity", sensorValue);
    this->setStatus(SensorStatus::OkRead);
}

void TurbiditySensor::update(StaticJsonDocument<200> value)
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

        if (!isValidPins())
        {
            if (log_enabled)
                Serial.println("Invalid pins");
            return;
        }
    }
    else if (command == "set_name")
    {
        const char *topic = value["new_name"];
        this->setDeviceName(topic);
    }
    // TODO: add other commands
}