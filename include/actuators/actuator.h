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

            bool isValid = false;

            bool size = pins.size() > 0;

            bool invalidPin = false;

            for (std::pair<int, String> pin : pins)
            {
                if (pin.first < 0 || pin.first > 40)
                {
                    invalidPin = true;
                    break;
                }
            }

            isValid = size && !invalidPin;

            setValidPins(isValid);
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