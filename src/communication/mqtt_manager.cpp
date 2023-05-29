#ifndef mqtt_manager_cpp
#define mqtt_manager_cpp

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

void MqttManager::publish(const char *topic, StaticJsonDocument <200> &doc)
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
    return isValidWifi() && mqtt_enabled && mqtt_server && mqtt_port && mqtt_user && mqtt_password;
}

const char *MqttManager::getEspId()
{
    static char id[32];
    snprintf(id, sizeof(id), "%s_%012X", mqtt_user, ESP.getEfuseMac());
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
    { 
        this->notify(topic, payload, length); 
    });

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

void MqttManager::reconnect()
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

void MqttManager::handleSensorAction(Device *device, StaticJsonDocument<200> value)
{
    Sensor *sensor = (Sensor *)device;
    
    String command = value["command"];
    
    if (command == "enable")
    {
        sensor->enable();
    } else if (command == "disable")
    {
        sensor->disable();
    } else if (command == "setPin")
    {
        int pin = value["pin"];
        sensor->setPin(pin);
    } 
    //TODO: add other commands
    /* else if (command == "setInterval")
     {
         int interval = value["interval"];
         sensor->setInterval(interval);
     } else if (command == "setTopic")
     {
         const char *topic = value["topic"];
         sensor->setTopic(topic);
     }*/
}

void MqttManager::handleActuatorAction(Device *device, StaticJsonDocument<200> value)
{
    Actuator *actuator = (Actuator *)device;
}

#endif