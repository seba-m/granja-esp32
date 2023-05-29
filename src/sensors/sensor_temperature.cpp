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
    sensors.setOneWire(&ourWire);
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
        timepoint = millis();
        readSensorValue();
        publish();
        this->setStatus(SensorStatus::OkLoop);
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

    if (isnan(temp))
    {
        if (log_enabled)
            Serial.println("Failed to read from Temperature sensor!");
        this->setStatus(SensorStatus::FailedRead);
        return;
    }

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

void TemperatureSensor::update()
{
    //TODO: Implement update
}