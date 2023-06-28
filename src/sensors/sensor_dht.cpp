#include <sensors/sensor_dht.h>
#include <status.h>

DHTSensor::DHTSensor(MqttManager &manager) : mqttManager(manager), Sensor(dhtSensorPin, dhtSensorType)
{
    setDeviceName("dht");
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

    dht = new DHT(this->getPin(), this->getSensorType());
    dht->begin();
    this->setStatus(SensorStatus::OkSetup);
}

void DHTSensor::loop(unsigned int timeout)
{
    if (millis() - timepoint > timeout)
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

        readSensorValue();
        publish();
        this->setStatus(SensorStatus::OkLoop);
        timepoint = millis();
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

void DHTSensor::update(StaticJsonDocument<200> value)
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
    } else if (command == "get_status") {
        StaticJsonDocument<200> doc;
        doc["type"] = "sensor";
        doc["sensor"] = "dht";
        doc["status"] = this->getStatus();
        doc["pin"] = this->getPin();
        doc["name"] = this->getDeviceName();
        doc["type"] = this->getSensorType();
        mqttManager.publish(mqtt_topic_dht, doc);
    }
    //TODO: add other commands
}