#include <sensors/sensor_turbidity.h>

TurbiditySensor::TurbiditySensor(MqttManager &manager) : mqttManager(manager) {}

void TurbiditySensor::loop(unsigned int timeout)
{
    if (turbiditySensorPin < 0)
        return;

    if (millis() - timepoint > timeout)
    {
        timepoint = millis();
        int sensorValue = analogRead(turbiditySensorPin);

        if (log_enabled)
        {
            Serial.print("Turbidity value = ");
            Serial.println(sensorValue, DEC);
        }

        mqttManager.publish(mqtt_topic_turbidity, String(sensorValue));
    }
}

void TurbiditySensor::update()
{
    // No se necesita implementar nada aquí para este ejemplo
}
