#ifndef mqtt_manager_h
#define mqtt_manager_h

#include <WiFi.h>
#include <PubSubClient.h>

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
        void publish(const char *topic, String message);
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

            if (log_enabled)
            {
                Serial.print("Message arrived in topic: ");
                Serial.println(topic);

                Serial.print("Message:");
                char message[length];

                for (int i = 0; i < length; i++)
                {
                    message[i] = (char)payload[i];
                }

                Serial.println(message);

                Serial.println("-----------------------");
            }
            /*for (auto observer : observers)
            {
                observer->update();
            }*/
        }

    private:
        static bool isValidWifi();
        static bool isValidMqtt();
        static void callback(char *topic, byte *payload, unsigned int length);
        const char *getEspId();
        void connectWiFi();
        void connectMQTT();
        void reconnect();
};

#endif // mqtt_manager_h