#ifndef sensor_temperature_h
#define sensor_temperature_h

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include <settings.h>
#include <communication/mqtt_manager.h>

class TemperatureSensor : public MqttObserver
{
private:
    OneWire ds;
    unsigned long timepoint = 0;
    MqttManager &mqttManager;
    OneWire ourWire;
    DallasTemperature sensors;

public:
    TemperatureSensor(MqttManager &manager);
    ~TemperatureSensor();

    void setup();
    void loop(unsigned int timeout = 500U);
    void update() override;
};

#endif // sensor_temperature_h
