#include <sensors/sensor_turbidity.h>

TurbiditySensor::TurbiditySensor(MqttManager &manager) : mqttManager(manager), Sensor(turbiditySensorPin) {
    setDeviceName("turbidity");
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
        timepoint = millis();
        readSensorValue();
        publish();
        this->setStatus(SensorStatus::OkLoop);
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

    if (isnan(turbidity))
    {
        if (log_enabled)
        {
            Serial.println("Failed to read from turbidity sensor!");
        }
        this->setStatus(SensorStatus::FailedRead);
        return;
    }

    if (log_enabled)
    {
        Serial.print("Turbidity: ");
        Serial.println(turbidity);
    }

    StaticJsonDocument<200> doc;
    doc["type"] = "sensor";
    doc["sensor"] = "turbidity";
    doc["turbidity"] = turbidity;
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

    int sensorValue = analogRead(turbiditySensorPin);

    if (log_enabled)
    {
        Serial.print("Turbidity value = ");
        Serial.println(sensorValue, DEC);
    }

    setValue("turbidity", sensorValue);
    this->setStatus(SensorStatus::OkRead);
}

void TurbiditySensor::update()
{
    //TODO: implement update
}