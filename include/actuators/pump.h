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
        void setup(int ena = -1, int in1 = -1, int in2 = -1, String name = "pump");
        void turnOn(int seconds = 10) override;
        void turnOff() override;
        void update(StaticJsonDocument<200> doc) override;

    private:
        bool on = false;
        int speedPercentage = 100; // Default speed: 100%
        int timeInSeconds = 10;    // Default time: 10 seconds

        void setSpeedPercentage(int percentage);
        int convertPercentageToSpeed(int percentage);
};

#endif // pump_h
