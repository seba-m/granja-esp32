#ifndef mqtt_manager_h
#define mqtt_manager_h

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <settings.h>

#include <observer/subject.h>

class MqttManager : public Subject
{
    private:
        WiFiClient wifiClient;
        PubSubClient client;
        const char *espId;
        std::vector<MqttObserver*> observers;

    public:
        MqttManager();

        void setup();
        void loop();
        void publish(const char *topic, StaticJsonDocument<200> & message);

        void attach(MqttObserver *observer) override
        {
            observers.push_back(observer);
        }

        void detach(MqttObserver *observer) override
        {
            // Buscar y eliminar el observador de la lista
            auto it = std::find(observers.begin(), observers.end(), observer);
            if (it != observers.end())
            {
                observers.erase(it);
            }
        }

        void notify(char *topic, byte *payload, unsigned int length) override
        {
            if (!isValidMqtt())
            {
                return;
            }

            String message = "";

            for (int i = 0; i < length; i++)
            {
                message.concat(payload[i]);
            }

            if (log_enabled)
            {
                Serial.print("Message arrived in topic: ");
                Serial.println(topic);

                Serial.print("Message:");
                Serial.println(message);

                Serial.println("-----------------------");
            }

            StaticJsonDocument<200> doc;
            deserializeJson(doc, message);

            String command = doc["command"];
            String sensorId = doc["sensor"];
            String value = doc["value"];

            /*if (command == "set")
            {
                MqttObserver *sensor = getSensor(sensorId);
                sensor->setEnabled(value == "true");
                sensor.loop();
            }
            /*else if (command == "get")
            {
                if (sensor == "water_level")
                {
                    publish(mqtt_topic_water_level, String(waterLevelSensor->isEnabled()));
                }
                else if (sensor == "water_pump")
                {
                    publish(mqtt_topic_water_pump, String(waterPump->isEnabled()));
                }
                else if (sensor == "light")
                {
                    publish(mqtt_topic_light, String(light->isEnabled()));
                }
            }*/
            /*for (auto observer : observers)
            {
                observer->update();
            }*/
        }

        /*MqttObserver *getSensor(String sensorId) {
            auto it = std::find_if(observers.begin(), observers.end(), [sensorId](MqttObserver *observer) {
                return observer->getTopicName() == sensorId;
            });

            return *it;
        }*/

    private:
        static bool isValidWifi();
        static bool isValidMqtt();
        const char *getEspId();
        void connectWiFi();
        void connectMQTT();
        void reconnect();
};

#endif // mqtt_manager_h