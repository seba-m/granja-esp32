#ifndef MqttManager_cpp
#define MqttManager_cpp

#include <WiFi.h>
#include <PubSubClient.h>

#include <settings.h>

class MqttManager
{
    private:
        WiFiClient wifiClient;
        PubSubClient client;
        const char *espId;

    public: 
        MqttManager()
        {
            client.setClient(wifiClient);
            espId = (String(mqtt_user) + "_" + String(ESP.getEfuseMac())).c_str();
        }

        void setup()
        {
            connectWiFi();
            connectMQTT();
        }

        void loop()
        {
            if (!client.connected())
            {
                reconnect();
            }
            client.loop();
        }

        void publish(const char *topic, String message)
        {
            client.publish(topic, message.c_str());
        }

    private:
        void connectWiFi()
        {
            WiFi.begin(wifi_ssid, wifi_password);
            WiFi.mode(WIFI_STA);
            while (WiFi.status() != WL_CONNECTED)
            {
                delay(100);
            }
        }

        void connectMQTT()
        {
            client.setServer(mqtt_server, mqtt_port);
            client.setCallback(callback);

            while (!client.connected())
            {

                if (client.connect("espId", mqtt_user, mqtt_password))
                {
                    String message = "device connected " + String(mqtt_user) + "_" + String(ESP.getEfuseMac());
                    client.subscribe("device/test");
                    client.publish("device/test", message.c_str());
                    Serial.println(message);
                    Serial.println("MQTT connected");
                }
                else
                {
                    Serial.print("failed with state ");
                    Serial.print(client.state());
                    Serial.println(espId);
                    delay(5000);
                }
            }
        }

        void reconnect()
        {
            while (!client.connected())
            {
                if (client.connect("espId", mqtt_user, mqtt_password))
                {
                    String message = "device connected " + String(mqtt_user) + "_" + String(ESP.getEfuseMac());
                    client.subscribe("device/test");
                    client.publish("device/test", message.c_str());
                    Serial.println(message);
                    Serial.println("MQTT connected");
                }
                else
                {
                    Serial.print("failed with state ");
                    Serial.print(client.state());
                    Serial.println(espId);
                    delay(1000);
                }
            }
        }

        static void callback(char *topic, byte *payload, unsigned int length)
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
};

#endif