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

void PumpController::loop()
{
    if (!isEnabled() || !isValidPins())
        return;

    if (on)
    {
        turnOn(timeInSeconds);
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

    if (command == "enable")
    {
        this->enable();
    }
    else if (command == "disable")
    {
        this->disable();
    }
    else if (command == "turn_on")
    {
        int time = value["time"];
        this->turnOn(time);
    }
    else if (command == "turn_off")
    {
        this->turnOff();
    }
    else if (command == "set_pin")
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
    else if (command == "set_name")
    {
        const char *topic = value["new_name"];
        this->setDeviceName(topic);
    } else if (command == "set_speed") {
        int percentage = value["percentage"];
        this->setSpeedPercentage(percentage);
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
