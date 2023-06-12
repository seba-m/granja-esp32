#ifndef device_h
#define device_h

#include <Arduino.h>
#include <observer/observer.h>

class Device : public MqttObserver
{
    private:
        bool enabled = false;
        bool validPins = true;
        String deviceName = "";

    public:
        void setValidPins(bool validPins)
        {
            this->validPins = validPins;
        }

        bool isValidPins()
        {
            return this->validPins;
        }

        void enable()
        {
            this->enabled = true;
        }

        void disable()
        {
            this->enabled = false;
        }

        void setDeviceName(String deviceName)
        {
            this->deviceName = deviceName;
        }

        bool isEnabled()
        {
            return this->enabled;
        }

        String getDeviceName()
        {
            return this->deviceName;
        }
};

#endif // device_h