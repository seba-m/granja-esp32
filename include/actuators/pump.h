#ifndef pump_h
#define pump_h

#include <Arduino.h>
#include <settings.h>
#include <communication/mqtt_manager.h>
#include <actuators/actuator.h>

class PumpController : public Actuator
{
    public:
        PumpController();
        void setup();
        void turnOn(int seconds = 10) override;
        void turnOff() override;
        void update(StaticJsonDocument<200> doc) override;

    private:
        bool on;
        int speedPercentage;
        int timeInSeconds;

        void setSpeedPercentage(int percentage);
        int convertPercentageToSpeed(int percentage);
};

#endif // pump_h
