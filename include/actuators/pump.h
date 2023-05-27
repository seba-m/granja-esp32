#ifndef pump_h
#define pump_h

#include <Arduino.h>
#include <settings.h>
#include <communication/mqtt_manager.h>

class PumpController : public MqttObserver
{
    private:
        bool isOn = false;

    public:
        void setup();
        void turnOn(int seconds = 10);
        void turnOff();
        void update() override;
};

#endif // pump_h
