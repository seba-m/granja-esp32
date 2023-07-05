#include <actuators/pump.h>

PumpController::PumpController(): Actuator()
{
}

void PumpController::setup(int ena, int in1, int in2, String name)
{
    setPins({{ena, "ENA"}, {in1, "IN1"}, {in2, "IN2"}});
    setDeviceName(name);
    if (!isEnabled() || !isValidPins())
        return;

    for (auto const pin : getPins())
    {
        pinMode(pin.first, OUTPUT);
    }
}

void PumpController::turnOn(int seconds)
{
    if (on || !isEnabled() || !isValidPins())
        return;

    for (auto const pin : getPins())
    {
        if (pin.second == "IN2")
            digitalWrite(pin.first, LOW);
        else
            digitalWrite(pin.first, HIGH);
    }

    on = true;
    timeInSeconds = seconds;
    delay(timeInSeconds * 1000);
    turnOff();
}

void PumpController::turnOff()
{
    if (!on || !isEnabled() || !isValidPins())
        return;

    for (auto const pin : getPins())
    {
        digitalWrite(pin.first, LOW);
    }
    on = false;
}

void PumpController::update(StaticJsonDocument<200> value)
{
    if (!value.containsKey("command"))
    {
        if (log_enabled)
            Serial.println("No command in message");
        return;
    }

    const char *command = value["command"];

    if (strcmp(command, "enable") == 0)
    {
        this->enable();
    }
    else if (strcmp(command, "disable") == 0)
    {
        this->disable();
    }
    else if (strcmp(command, "turn_on") == 0)
    {
        int time = value["time"];
        this->turnOn(time);
    }
    else if (strcmp(command, "turn_off") == 0)
    {
        this->turnOff();
    }
    else if (strcmp(command, "set_pin") == 0)
    {
        JsonObject pins = value["pins"];

        std::map<int, String> pinValues;
        for (JsonPair keyValue : pins)
        {
            int pin = String(keyValue.key().c_str()).toInt();
            String pinValue = keyValue.value().as<String>();
            pinValues[pin] = pinValue;
        }

        this->setPins(pinValues);
    }
    else if (strcmp(command, "set_name") == 0)
    {
        const char *topic = value["new_name"];
        this->setDeviceName(topic);
    }
    else if (strcmp(command, "set_speed") == 0)
    {
        int percentage = value["percentage"];
        this->setSpeedPercentage(percentage);
    }
    else if (log_enabled)
    {
        Serial.println("Invalid command " + String(command));
    }
}

void PumpController::setSpeedPercentage(int percentage)
{
    speedPercentage = constrain(percentage, 0, 100);
    int speedValue = convertPercentageToSpeed(speedPercentage);
    analogWrite(pumpEnaPin, speedValue);
}

int PumpController::convertPercentageToSpeed(int percentage)
{
    return map(percentage, 0, 100, 0, 255);
}
