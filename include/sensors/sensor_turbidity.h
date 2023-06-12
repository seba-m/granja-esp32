#ifndef sensor_turbidity_h
#define sensor_turbidity_h

#include <Arduino.h>
#include <settings.h>
#include <communication/mqtt_manager.h>
#include <sensors/sensor.h>

class TurbiditySensor : public Sensor
{
    private:
        unsigned long timepoint = 0;
        MqttManager &mqttManager;

    public:
        TurbiditySensor(MqttManager &manager);
        void loop(unsigned int timeout = 100U);
        void update(StaticJsonDocument<200> doc) override;
        void readSensorValue() override;

    private:
        void publish() override;
};

#endif // sensor_turbidity_h
