#include <OneWire.h>
#include <DallasTemperature.h>

#include <settings.h>
#include <communication/mqtt_manager.cpp>

class TemperatureSensor
{
    private:
        OneWire ds;
        DallasTemperature sensor;
        unsigned long timepoint = 0;
        MqttManager &mqttManager;

    public:
        TemperatureSensor(MqttManager &manager) : mqttManager(manager)
        {
        }

        void setup()
        {
            ds = OneWire(temperatureSensorPin);
            sensor = DallasTemperature(&ds);
            sensor.begin();
        }

        void loop(unsigned int timeout = 100U)
        {
            if (millis() - timepoint > timeout)
            {
                timepoint = millis();
                sensor.requestTemperatures();
                Serial.print("Temperature= ");
                float temperature = sensor.getTempCByIndex(0);
                Serial.println(temperature, DEC);

                mqttManager.publish("sensor/temperature", String(temperature));
            }
        }
};
