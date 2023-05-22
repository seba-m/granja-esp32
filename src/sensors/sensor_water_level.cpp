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
            if (waterLevelSensorPin < 0)
                return;

            pinMode(waterLevelSensorPin, INPUT);
        }

        void loop(unsigned int timeout = 500U)
        {
            if (waterLevelSensorPin < 0)
                return;

            if (millis() - timepoint > timeout)
            {
                timepoint = millis();
                int liquidLevel = digitalRead(waterLevelSensorPin);

                if (log_enabled) {
                    Serial.print("Liquid level= ");
                    Serial.println(liquidLevel, DEC);
                }
                
                mqttManager.publish("sensor/level", String(liquidLevel));
            }
        }
};
