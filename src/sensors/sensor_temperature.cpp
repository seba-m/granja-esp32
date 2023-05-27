#include <sensors/sensor_temperature.h>

TemperatureSensor::TemperatureSensor(MqttManager &manager) : mqttManager(manager) {}

TemperatureSensor::~TemperatureSensor() {}

void TemperatureSensor::setup()
{
    if (temperatureSensorPin < 0)
        return;

    ds = OneWire(temperatureSensorPin);
    sensors.setOneWire(&ourWire);
    sensors.begin();
}

void TemperatureSensor::loop(unsigned int timeout)
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

        mqttManager.publish(mqtt_topic_water_temperature, String(temp));
    }
}

void TemperatureSensor::update()
{
    // No se necesita implementar nada aquí para este ejemplo
}
