#include <Arduino.h>
#include <settings.h>
#include <DHT.h>

#include <communication/mqtt_manager.cpp>

class DHTSensor
{
    private:
        unsigned long timepoint = 0;
        DHT *dht;
        MqttManager &mqttManager;

    public:
        DHTSensor(MqttManager &manager) : mqttManager(manager) {}
        ~DHTSensor()
        {
            delete dht;
        }

        void setup()
        {
            if (dhtSensorPin < 0)
                return;

            dht = new DHT(dhtSensorPin, dhtSensorType);
            dht->begin();
        }

        void loop(unsigned int timeout = 500U)
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

                mqttManager.publish("sensor/temperature", String(temperature));
                mqttManager.publish("sensor/humidity", String(humidity));
            }
        }
};
