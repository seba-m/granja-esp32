#ifndef sensor_water_level_h
#define sensor_water_level_h

#include <Arduino.h>
#include <settings.h>
#include <communication/mqtt_manager.h>
#include <sensors/sensor.h>

class WaterLevelSensor : public Sensor
{
    private:
        unsigned long timepoint = 0;
        MqttManager &mqttManager;

    public:
        WaterLevelSensor(MqttManager &manager);
        void setup(int pin = -1, String deviceName = "water_level");
        void loop(unsigned int timeout = 500U);
        void update(StaticJsonDocument<200> doc) override;
        void readSensorValue() override;

    private:
        void publish() override;
};

#endif // sensor_water_level_h