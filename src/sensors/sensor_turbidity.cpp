#include <Arduino.h>

#include <settings.h>
#include <communication/mqtt_manager.cpp>

class TurbiditySensor
{
    private:
        unsigned long timepoint = 0;
        MqttManager &mqttManager;

    public:
        TurbiditySensor(MqttManager &manager) : mqttManager(manager)
        {
        }
        void loop(unsigned int timeout = 100U)
        {
            if (millis() - timepoint > timeout)
            {
                timepoint = millis();
                int sensorValue = analogRead(turbiditySensorPin);
                Serial.print("Turbidity value= ");
                Serial.println(sensorValue, DEC);

                mqttManager.publish("sensor/turbidity", String(sensorValue));
            }
        }
};
