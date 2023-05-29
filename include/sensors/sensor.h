#ifndef sensor_h
#define sensor_h

#include <Arduino.h>
#include <device.h>
#include <map>

#include <status.h>

class Sensor : public Device
{
    private:
        std::map<String, float> values;
        int pin;
        SensorStatus status = SensorStatus::InProgress;

    public:
        Sensor(int pin)
        {
            this->pin = pin;
            setValidPins(pin > -1);
        }

        void setValue(String type, float value)
        {
            values[type] = value;
        }

        float getValue(String type)
        {
            return values[type] ? values[type] : NAN;
        }

        void setStatus(SensorStatus status)
        {
            this->status = status;
        }

        String getStatus()
        {
            return sensorStatusMap[this->status];
        }

        int getPin()
        {
            return this->pin;
        }

        virtual void readSensorValue() = 0;
        //virtual void calibrateSensor() = 0; //TODO: implement calibration
        virtual void publish() = 0;
};

#endif // sensor_h