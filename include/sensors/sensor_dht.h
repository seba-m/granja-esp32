#ifndef sensor_dht_h
#define sensor_dht_h

#include <Arduino.h>
#include <settings.h>
#include <DHT.h>
#include <communication/mqtt_manager.h>

class DHTSensor : public MqttObserver
{
    private:
        unsigned long timepoint = 0;
        DHT *dht;
        MqttManager &mqttManager;

    public:
        DHTSensor(MqttManager &manager);
        ~DHTSensor();

        void setup();
        void loop(unsigned int timeout = 500U);
        void update() override;
};

#endif // sensor_dht_h