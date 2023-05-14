#include <OneWire.h>

class TemperatureSensor
{
    private:
        int sensorPin;
        OneWire ds;
        unsigned long timepoint = 0;

    public:
        void setup(int pin)
        {
            sensorPin = pin;
            ds = OneWire(pin);
        }

        void loop(unsigned int timeout = 100U)
        {
            if (millis() - timepoint > timeout)
            {
                timepoint = millis();
                float temperature = getTemp();
                Serial.print("Temperature= ");
                Serial.println(temperature, DEC);
            }
        }

    private:
        float getTemp()
        {
            byte data[12];
            byte addr[8];

            if (!ds.search(addr))
            {
                ds.reset_search();
                return -1000;
            }

            if (OneWire::crc8(addr, 7) != addr[7])
            {
                Serial.println("CRC is not valid!");
                return -1000;
            }

            if (addr[0] != 0x10 && addr[0] != 0x28)
            {
                Serial.print("Device is not recognized");
                return -1000;
            }

            ds.reset();
            ds.select(addr);
            ds.write(0x44, 1); // start conversion, with parasite power on at the end
            byte present = ds.reset();
            ds.select(addr);
            ds.write(0xBE); // Read Scratchpad

            for (int i = 0; i < 9; i++)
            {
                data[i] = ds.read();
            }

            ds.reset_search();

            byte MSB = data[1];
            byte LSB = data[0];
            float tempRead = ((MSB << 8) | LSB);
            float temperatureSum = tempRead / 16;

            return temperatureSum;
        }
};
