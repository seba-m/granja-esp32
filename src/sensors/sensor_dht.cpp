#include <sensors/sensor_dht.h>
#include <status.h>

DHTSensor::DHTSensor(MqttManager &manager) : mqttManager(manager), Sensor()
{
}

DHTSensor::~DHTSensor()
{
    delete dht;
}

void DHTSensor::setup(int pin, int sensorType, String name)
{
    setPin(pin);
    setSensorType(sensorType);
    setDeviceName(name);
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
    doc["sensor"] = this->getDeviceName();
    doc[this->getDeviceName() + "_humidity"] = humidity;
    doc[this->getDeviceName() + "_temperature"] = temperature;
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
            this->setStatus(SensorStatus::InvalidPins);
            return;
        }

        if (dht != nullptr)
            delete dht;

        dht = new DHT(this->getPin(), this->getSensorType());
        dht->begin();
    }
    else if (strcmp(command, "set_name") == 0)
    {
        const char *topic = value["new_name"];
        this->setDeviceName(topic);
    }
    else if (strcmp(command, "get_status") == 0)
    {
        StaticJsonDocument<200> doc;
        doc["type"] = "sensor";
        doc["sensor"] = "dht";
        doc["status"] = this->getStatus();
        doc["pin"] = this->getPin();
        doc["name"] = this->getDeviceName();
        doc["type"] = this->getSensorType();
        doc["enabled"] = this->isEnabled();
        doc["humidity"] = getValue("humidity");
        doc["temperature"] = getValue("temperature");
        mqttManager.publish(mqtt_topic_dht, doc);
    }
    else if (log_enabled)
    {
        Serial.println("Invalid command " + String(command));
    }
    // TODO: add other commands
}