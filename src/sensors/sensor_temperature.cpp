#include <OneWire.h>
#include <DallasTemperature.h>
#include <settings.h>
#include <communication/mqtt_manager.cpp>

class TemperatureSensor
{
    private:
        OneWire ds;
        unsigned long timepoint = 0;
        MqttManager &mqttManager;
        OneWire ourWire;
        DallasTemperature sensors;

    public:
        TemperatureSensor(MqttManager &manager) : mqttManager(manager)
        {
        }

        void setup()
        {
            if (temperatureSensorPin < 0)
                return;

            ds = OneWire(temperatureSensorPin);
            sensors.setOneWire(&ourWire);
            sensors.begin();
        }

        void loop(unsigned int timeout = 500U)
        {
            if (temperatureSensorPin < 0)
                return;

            if (millis() - timepoint > timeout)
            {
                timepoint = millis();
                sensors.requestTemperatures();
                float temp = sensors.getTempCByIndex(0);

                if (temp == -127.00)
                {
                    if (log_enabled)
                        Serial.println("Error: Could not read temperature data");
                    return;
                }

                if (log_enabled)
                {
                    Serial.print("Temperature = ");
                    Serial.print(temp);
                    Serial.println(" °C");
                }

                mqttManager.publish("sensor/temperature", String(temp));
            }
        }
};
