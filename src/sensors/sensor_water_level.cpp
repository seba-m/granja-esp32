#include <Arduino.h>

class WaterLevelSensor
{
    private:
        int pin;
        unsigned long timepoint = 0;

    public:
        void setup(int sensorPin)
        {
            pin = sensorPin;
            pinMode(pin, INPUT);
        }

        void loop(unsigned int timeout = 500U)
        {
            if (millis() - timepoint > timeout)
            {
                timepoint = millis();
                int liquidLevel = digitalRead(pin);
                Serial.print("Liquid_level= ");
                Serial.println(liquidLevel, DEC);
            }
        }
};
