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
            ds = OneWire(temperatureSensorPin);
            sensors.setOneWire(&ourWire);
            sensors.begin();
            sensors.setResolution(12); // Ajusta la resolución del sensor a 12 bits (máxima precisión)
        }

        void loop(unsigned int timeout = 500U)
        {
            if (millis() - timepoint > timeout)
            {
                timepoint = millis();
                sensors.requestTemperatures();
                float temp = sensors.getTempCByIndex(0);

                if (temp == -127.00)
                {
                    Serial.println("Error: Lectura inválida del sensor");
                    return;
                }

                Serial.print("Temperatura = ");
                Serial.print(temp);
                Serial.println(" °C");

                mqttManager.publish("sensor/temperature", String(temp));
            }
        }
};
