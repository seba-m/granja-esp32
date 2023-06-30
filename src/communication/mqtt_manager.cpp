#include <communication/mqtt_manager.h>

MqttManager::MqttManager() : client(wifiClient)
{
    espId = getEspId();
}

void MqttManager::setup()
{
    connectWiFi();
    connectMQTT();
}

void MqttManager::loop()
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

void MqttManager::publish(const char *topic, StaticJsonDocument<200> &doc)
{
    if (!isValidMqtt())
    {
        return;
    }

    char buffer[200];
    serializeJson(doc, buffer);
    client.publish(topic, buffer);
}

bool MqttManager::isValidWifi()
{
    return wifi_enabled && wifi_ssid && wifi_password;
}

bool MqttManager::isValidMqtt()
{
    return isValidWifi() && mqtt_enabled && mqtt_server && mqtt_port;
}

const char *MqttManager::getEspId()
{
    static char id[70];
    if (!String(mqtt_user).isEmpty())
    {
        snprintf(id, sizeof(id), "%s_%012X", mqtt_user, ESP.getEfuseMac());
    }
    else
    {
        snprintf(id, sizeof(id), "esp_%012X", ESP.getEfuseMac());
    }
    return id;
}

void MqttManager::connectWiFi()
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

void MqttManager::connectMQTT()
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
    client.setCallback([this](char *topic, byte *payload, unsigned int length)
                        { this->notify(topic, payload, length); });


    while (!client.connected())
    {
        if (mqtt_user && !*mqtt_user && mqtt_password && !*mqtt_password)
        {
            if (client.connect(espId.c_str(), mqtt_user, mqtt_password))
            {
                break;
            }
        }
        else
        {
            if (client.connect(espId.c_str()))
            {
                break;
            }
        }

        if (log_enabled)
        {
            Serial.println("Failed to connect to MQTT broker. Retrying in 1 second...");
        }
        delay(1000);
    }

    if (client.connected())
    {
        String message = "device connected: " + espId;
        client.subscribe("actions");

        if (log_enabled)
        {
            Serial.println(message);
            Serial.println("MQTT connected");
        }
    }
}

void MqttManager::reconnect()
{
    if (!isValidMqtt())
    {
        return;
    }

    if (log_enabled)
    {
        Serial.println("Reconnecting to MQTT broker: " + String(mqtt_server) + " ...");
    }

    connectWiFi();

    while (!client.connected())
    {
        if (mqtt_user && !*mqtt_user && mqtt_password && !*mqtt_password)
        {
            if (client.connect(espId.c_str(), mqtt_user, mqtt_password))
            {
                break;
            }
        }
        else
        {
            if (client.connect(espId.c_str()))
            {
                break;
            }
        }

        if (log_enabled)
        {
            Serial.println("Failed to reconnect to MQTT broker. Retrying in 1 second...");
        }
        delay(1000);
    }

    if (client.connected())
    {
        String message = "device connected: " + espId;
        client.subscribe("actions");

        if (log_enabled)
        {
            Serial.println(message);
            Serial.println("MQTT connected");
        }
    }
}
