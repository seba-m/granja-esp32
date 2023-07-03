#ifndef sensor_tds_h
#define sensor_tds_h

#include <Arduino.h>
#include <settings.h>
#include <communication/mqtt_manager.h>
#include <sensors/sensor.h>

#define VREF 5.0
#define SCOUNT 30

class MeasureTDS : public Sensor
{
    private:
        int analogBuffer[SCOUNT];
        int analogBufferTemp[SCOUNT];
        int analogBufferIndex = 0, copyIndex = 0;
        float averageVoltage = 0, tdsValue = 0, temperature = 25;

        unsigned long analogSampleTimepoint = 0;
        unsigned long printTimepoint = 0;

        MqttManager &mqttManager;

    public:
        MeasureTDS(MqttManager &manager);
        void setup();
        void loop();
        void update(StaticJsonDocument<200> doc) override;

    private:
        void readAnalogValue(unsigned int timeout = 40U);
        void printTDSValue(unsigned int timeout = 800U);
        int getMedianNum(int bArray[], int iFilterLen);
        void readSensorValue() override;
        void publish() override;
};

#endif // sensor_tds_h
