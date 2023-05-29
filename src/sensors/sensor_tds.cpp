#include <sensors/sensor_tds.h>

MeasureTDS::MeasureTDS(MqttManager &manager) : mqttManager(manager), Sensor(TdsSensorPin) {
    setDeviceName("tds");
}

void MeasureTDS::setup()
{
    if (!isValidPins() || !isEnabled())
    {
        if (isEnabled() && log_enabled)
        {
            Serial.println("Invalid pins");
            this->setStatus(SensorStatus::InvalidPins);
        }
        return;
    }

    pinMode(TdsSensorPin, INPUT);
    this->setStatus(SensorStatus::OkSetup);
}

void MeasureTDS::loop()
{
    if (!isValidPins() || !isEnabled())
    {
        if (isEnabled() && log_enabled)
        {
            Serial.println("Invalid pins");
            this->setStatus(SensorStatus::InvalidPins);
        }
        return;
    }

    readSensorValue();
}

void MeasureTDS::readSensorValue()
{
    if (!isValidPins() || !isEnabled())
    {
        if (isEnabled() && log_enabled)
        {
            Serial.println("Invalid pins");
            this->setStatus(SensorStatus::InvalidPins);
        }
        return;
    }

    readAnalogValue();
    printTDSValue();
    this->setStatus(SensorStatus::OkLoop);
}

void MeasureTDS::publish()
{
    if (!isValidPins() || !isEnabled())
    {
        if (isEnabled() && log_enabled)
        {
            Serial.println("Invalid pins");
            this->setStatus(SensorStatus::InvalidPins);
        }
        return;
    }

    float tdsValue = getValue("tds");

    if (isnan(tdsValue))
    {
        if (log_enabled)
            Serial.println("Failed to read from TDS sensor!");
        this->setStatus(SensorStatus::FailedRead);
        return;
    }

    StaticJsonDocument<200> doc;
    doc["type"] = "sensor";
    doc["sensor"] = "tds";
    doc["tds"] = tdsValue;
    doc["status"] = this->getStatus();

    mqttManager.publish(mqtt_topic_tds, doc);
}

void MeasureTDS::readAnalogValue(unsigned int timeout)
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

void MeasureTDS::printTDSValue(unsigned int timeout)
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

        if (log_enabled)
        {
            Serial.print("TDS Value = ");
            Serial.print(tdsValue, DEC);
            Serial.println("ppm");
        }

        setValue("tds", tdsValue);
        this->setStatus(SensorStatus::OkRead);
        publish();
    }
}

int MeasureTDS::getMedianNum(int bArray[], int iFilterLen)
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

void MeasureTDS::update()
{
    // TODO: implement update
}