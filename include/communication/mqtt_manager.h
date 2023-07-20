#ifndef mqtt_manager_h
#define mqtt_manager_h

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <settings.h>
#include <observer/subject.h>
#include <device.h>
#include <actuators/actuator.h>
#include <sensors/sensor.h>

#include <actuators/pump.h>

class MqttManager : public Subject
{
    private:
        WiFiClient wifiClient;
        PubSubClient client;
        String espId;
        std::vector<Device *> observers;

    public:
        MqttManager();

        void setup();
        void loop();
        void publish(const char *topic, StaticJsonDocument<200> &message);
        void report(const char *topic, StaticJsonDocument<200> &message);
        void attach(Device *device) override;
        void detach(Device *device) override;
        void notify(char *topic, byte *payload, unsigned int length) override;
    private:
        static bool isValidWifi();
        static bool isValidMqtt();
        const char *getEspId();
        void connectWiFi();
        void connectMQTT();
        void reconnect();
        Device *getDevice(const String &deviceName);
};

#endif