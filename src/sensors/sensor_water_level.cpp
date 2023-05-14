#include <Arduino.h>

#include <settings.h>
#include <communication/mqtt_manager.cpp>

class WaterLevelSensor
{
    private:
        unsigned long timepoint = 0;
        
        MqttManager &mqttManager;

    public:
        WaterLevelSensor(MqttManager &manager) : mqttManager(manager)
        {
        }

        void setup()
        {
            pinMode(waterLevelSensorPin, INPUT);
        }

        void loop(unsigned int timeout = 500U)
        {
            if (millis() - timepoint > timeout)
            {
                timepoint = millis();
                int liquidLevel = digitalRead(waterLevelSensorPin);
                Serial.print("Liquid_level= ");
                Serial.println(liquidLevel, DEC);

                mqttManager.publish("sensor/level", String(liquidLevel));
            }
        }
};
