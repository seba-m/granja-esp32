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
        espId = getEspId();
    }

    void setup()
    {
        connectWiFi();
        connectMQTT();
    }

    void loop()
    {
        if (!isValidMqtt())
        {
            return;
        }

        if (!client.connected())
        {
            reconnect();
        }
        client.loop();
    }

    void publish(const char *topic, String message)
    {
        if (!isValidMqtt())
        {
            return;
        }

        client.publish(topic, message.c_str());
    }

private:
    static bool isValidWifi()
    {
        return wifi_enabled && wifi_ssid && wifi_password;
    }

    static bool isValidMqtt()
    {
        return isValidWifi() && mqtt_enabled && mqtt_server && mqtt_port && mqtt_user && mqtt_password;
    }

    static void callback(char *topic, byte *payload, unsigned int length)
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
    }

    const char *getEspId()
    {
        static char id[32];
        snprintf(id, sizeof(id), "%s_%012X", mqtt_user, ESP.getEfuseMac());
        return id;
    }

    void connectWiFi()
    {
        if (!isValidWifi() || WiFi.status() == WL_CONNECTED)
        {
            return;
        }

        if (log_enabled)
        {
            Serial.println("Connecting to WiFi network: " + String(wifi_ssid) + " ...");
        }

        WiFi.begin(wifi_ssid, wifi_password);
        WiFi.mode(WIFI_STA);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(100);
        }

        if (log_enabled)
        {
            Serial.println("WiFi connected");
        }
    }

    void connectMQTT()
    {
        if (!isValidMqtt())
        {
            return;
        }

        if (log_enabled)
        {
            Serial.println("Connecting to MQTT broker: " + String(mqtt_server) + " ...");
        }
        client.setServer(mqtt_server, mqtt_port);
        client.setCallback(callback);

        while (!client.connected())
        {

            if (client.connect("espId", mqtt_user, mqtt_password))
            {
                String message = "device connected " + String(mqtt_user) + "_" + String(ESP.getEfuseMac());
                client.subscribe("device/test");
                client.publish("device/test", message.c_str());

                if (log_enabled)
                {
                    Serial.println(message);
                    Serial.println("MQTT connected");
                }
            }
            else
            {
                if (log_enabled)
                {
                    Serial.println("failed with state '" + String(client.state()) + "' espId: " + espId);
                }
                delay(1000);
            }
        }

        if (log_enabled)
        {
            Serial.println("MQTT connected");
        }
    }

    void reconnect()
    {
        if (!isValidMqtt())
        {
            return;
        }

        if (log_enabled)
        {
            Serial.println("Connecting to MQTT broker again: " + String(mqtt_server) + " ...");
        }
        connectWiFi();

        while (!client.connected())
        {
            if (client.connect("espId", mqtt_user, mqtt_password))
            {
                String message = "device connected " + String(mqtt_user) + "_" + String(ESP.getEfuseMac());
                client.subscribe("device/test");
                client.publish("device/test", message.c_str());

                if (log_enabled)
                {
                    Serial.println(message);
                    Serial.println("MQTT connected");
                }
            }
            else
            {
                if (log_enabled)
                {
                    Serial.println("failed with state " + String(client.state()) + " espId: " + espId);
                }
                delay(1000);
            }
        }

        if (log_enabled)
        {
            Serial.println("MQTT connected");
        }
    }
};

#endif