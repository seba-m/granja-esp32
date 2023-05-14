#include <Arduino.h>

class TurbiditySensor
{
    private:
        int pin;
        unsigned long timepoint = 0;

    public:
        void setup(int sensorPin)
        {
            pin = sensorPin;
        }

        void loop(unsigned int timeout = 100U)
        {
            if (millis() - timepoint > timeout)
            {
                timepoint = millis();
                int sensorValue = analogRead(pin);
                Serial.print("Turbidity value= ");
                Serial.println(sensorValue, DEC);
            }
        }
};
