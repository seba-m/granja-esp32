#ifndef sensor_turbidity_h
#define sensor_turbidity_h

#include <Arduino.h>
#include <settings.h>
#include <communication/mqtt_manager.h>

class TurbiditySensor : public MqttObserver
{
    private:
        unsigned long timepoint = 0;
        MqttManager &mqttManager;

    public:
        TurbiditySensor(MqttManager &manager);
        void loop(unsigned int timeout = 100U);
        void update() override;
};

#endif // sensor_turbidity_h
