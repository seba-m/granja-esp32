#ifndef actuator_h
#define actuator_h

#include <Arduino.h>
#include <device.h>
#include <map>

class Actuator : public Device
{
    private:
        bool state = false;
        std::map<int, String> pins = {};
        float powerLevel = 0;
        int onTime = 0;

    public:
        void setState(bool state)
        {
            this->state = state;
        }

        bool getState()
        {
            return this->state;
        }

        void setPins(std::map<int, String> pins)
        {
            this->pins = pins;
            setValidPins(
                pins.size() > 0 &&
                std::all_of(
                    pins.begin(), pins.end(), [](std::pair<int, String> pin)
                    { return pin.first > -1; }));
        }

        std::map<int, String> getPins()
        {
            return this->pins;
        }

        virtual void turnOn(int seconds) = 0;

        virtual void turnOff() = 0;

        void setPowerLevel(float powerLevel)
        {
            this->powerLevel = powerLevel;
        }

        void setOnTime(int seconds)
        {
            this->onTime = seconds;
        }

        int getOnTime()
        {
            return this->onTime;
        }
};

#endif // actuator_h