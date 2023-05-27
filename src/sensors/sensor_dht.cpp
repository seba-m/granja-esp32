#include <sensors/sensor_dht.h>

DHTSensor::DHTSensor(MqttManager &manager) : mqttManager(manager) {}

DHTSensor::~DHTSensor()
{
    delete dht;
}

void DHTSensor::setup()
{
    if (dhtSensorPin < 0)
        return;

    dht = new DHT(dhtSensorPin, dhtSensorType);
    dht->begin();
}

void DHTSensor::loop(unsigned int timeout)
{
    if (dhtSensorPin < 0)
        return;

    if (millis() - timepoint > timeout)
    {
        timepoint = millis();
        float temperature = dht->readTemperature();
        float humidity = dht->readHumidity();

        if (isnan(temperature) || isnan(humidity))
        {
            if (log_enabled)
                Serial.println("Failed to read from DHT sensor!");
            return;
        }

        if (log_enabled)
        {
            Serial.print("Temperature = ");
            Serial.print(temperature);
            Serial.print(" °C, Humidity = ");
            Serial.println(humidity);
        }

        mqttManager.publish(mqtt_topic_dht_temperature, String(temperature));
        mqttManager.publish(mqtt_topic_dht_humidity, String(humidity));
    }
}

void DHTSensor::update()
{
    // No se necesita implementar nada aquí para este ejemplo
}