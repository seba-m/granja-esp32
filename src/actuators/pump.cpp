#include <actuators/pump.h>

PumpController::PumpController() 
    : Actuator({
        {pumpEnaPin, "ENA"}, 
        {pumpIn1Pin, "IN1"}, 
        {pumpIn2Pin, "IN2"}
    })
{
    setDeviceName("pump");
}

void PumpController::setup()
{
    if (!isEnabled() || !isValidPins()) return;

    for (auto const pin : getPins())
    {
        pinMode(pin.first, OUTPUT);
    }
}

void PumpController::turnOn(int seconds = 10)
{
    if (getState() || !isEnabled() || !isValidPins()) return;

    for (auto const pin : getPins())
    {
        if (pin.second == "IN2")
            digitalWrite(pin.first, LOW);
        else
            digitalWrite(pin.first, HIGH);
    }    

    setState(true);
    delay(seconds * 1000);
    turnOff();
}

void PumpController::turnOff()
{
    if (!getState() || !isEnabled() || !isValidPins())
        return;

    for (auto const pin : getPins())
    {
        digitalWrite(pin.first, LOW);
    }
    setState(false);
}

void PumpController::update()
{
    // TODO: implement update
}