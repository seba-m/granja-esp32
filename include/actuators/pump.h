#ifndef pump_h
#define pump_h

#include <Arduino.h>
#include <settings.h>
#include <communication/mqtt_manager.h>

#include <actuators/actuator.h>

class PumpController : public MqttObserver, public Actuator
{
    public:
        PumpController();
        void setup();
        void turnOn(int seconds = 10) override;
        void turnOff() override;
        void update() override;
};

#endif // pump_h
