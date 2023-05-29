#ifndef device_h
#define device_h

#include <Arduino.h>

class Device
{
    private:
        bool enabled = true;
        bool validPins = true;
        String topicName = "";

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

        void setTopicName(String topicName)
        {
            this->topicName = topicName;
        }

        bool isEnabled()
        {
            return this->enabled;
        }

        String getTopicName()
        {
            return this->topicName;
        }
};

#endif // device_h