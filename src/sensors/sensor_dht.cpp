#include <sensors/sensor_dht.h>
#include <status.h>

DHTSensor::DHTSensor(MqttManager &manager) : mqttManager(manager), Sensor(dhtSensorPin) {
    setTopicName("dht");
}

DHTSensor::~DHTSensor()
{
    delete dht;
}

void DHTSensor::setup()
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

    dht = new DHT(dhtSensorPin, dhtSensorType);
    dht->begin();
    this->setStatus(SensorStatus::OkSetup);
}

void DHTSensor::loop(unsigned int timeout)
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

void DHTSensor::publish()
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

    float humidity = getValue("humidity");
    float temperature = getValue("temperature");

    if (isnan(humidity) || isnan(temperature))
    {
        if (log_enabled)
            Serial.println("Failed to read from DHT sensor!");
        this->setStatus(SensorStatus::FailedRead);
        return;
    }

    StaticJsonDocument<200> doc;
    doc["type"] = "sensor";
    doc["sensor"] = "dht";
    doc["humidity"] = humidity;
    doc["temperature"] = temperature;
    doc["status"] = this->getStatus();

    mqttManager.publish(mqtt_topic_dht, doc);
}

void DHTSensor::readSensorValue()
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

    float humidity = dht->readHumidity();
    float temperature = dht->readTemperature();

    if (isnan(humidity) || isnan(temperature))
    {
        if (log_enabled)
            Serial.println("Failed to read from DHT sensor!");
        setValue("humidity", NAN);
        setValue("temperature", NAN);
        this->setStatus(SensorStatus::FailedRead);
        return;
    }

    if (log_enabled)
    {
        Serial.print("Humidity = ");
        Serial.print(humidity);
        Serial.println(" %");
        Serial.print("Temperature = ");
        Serial.print(temperature);
        Serial.println(" °C");
    }

    setValue("humidity", humidity);
    setValue("temperature", temperature);
    this->setStatus(SensorStatus::OkRead);
}

void DHTSensor::update()
{
    //TODO: implement update
}