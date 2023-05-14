#include <Arduino.h>

#include <settings.h>
#include <communication/mqtt_manager.cpp>

#define VREF 5.0          // analog reference voltage(Volt) of the ADC
#define SCOUNT 30         // sum of sample point

class MeasureTDS
{
    private:
        int analogBuffer[SCOUNT]; // store the analog value in the array, read from ADC
        int analogBufferTemp[SCOUNT];
        int analogBufferIndex = 0, copyIndex = 0;
        float averageVoltage = 0, tdsValue = 0, temperature = 25;

        unsigned long analogSampleTimepoint = 0;
        unsigned long printTimepoint = 0;

        MqttManager &mqttManager;

    public:
        MeasureTDS(MqttManager &manager) : mqttManager(manager)
        {
        }

        void setup()
        {
            pinMode(TdsSensorPin, INPUT);
        }

        void loop()
        {
            readAnalogValue();
            printTDSValue();
        }

    private:
        void readAnalogValue(unsigned int timeout = 40U)
        {
            if (millis() - analogSampleTimepoint > timeout)
            {
                analogSampleTimepoint = millis();
                analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);
                analogBufferIndex++;
                if (analogBufferIndex == SCOUNT)
                    analogBufferIndex = 0;
            }
        }

        void printTDSValue(unsigned int timeout = 800U)
        {
            if (millis() - printTimepoint > timeout)
            {
                printTimepoint = millis();
                for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
                    analogBufferTemp[copyIndex] = analogBuffer[copyIndex];

                averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0;
                float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
                float compensationVolatge = averageVoltage / compensationCoefficient;
                tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5;

                Serial.print("TDS Value:");
                Serial.print(tdsValue, DEC);
                Serial.println("ppm");

                mqttManager.publish("sensor/tds", String(tdsValue));
            }
        }

        int getMedianNum(int bArray[], int iFilterLen)
        {
            int bTab[iFilterLen];
            for (byte i = 0; i < iFilterLen; i++)
                bTab[i] = bArray[i];
            int i, j, bTemp;
            for (j = 0; j < iFilterLen - 1; j++)
            {
                for (i = 0; i < iFilterLen - j - 1; i++)
                {
                    if (bTab[i] > bTab[i + 1])
                    {
                        bTemp = bTab[i];
                        bTab[i] = bTab[i + 1];
                        bTab[i + 1] = bTemp;
                    }
                }
            }
            if ((iFilterLen & 1) > 0)
                bTemp = bTab[(iFilterLen - 1) / 2];
            else
                bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
            return bTemp;
        }

};