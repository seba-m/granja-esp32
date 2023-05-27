#include <actuators/pump.h>

void PumpController::setup()
{
    pinMode(pumpEnaPin, OUTPUT); // ENA
    pinMode(pumpIn1Pin, OUTPUT); // IN1
    pinMode(pumpIn2Pin, OUTPUT); // IN2
}

void PumpController::turnOn(int seconds)
{
    if (isOn)
    {
        return;
    }

    digitalWrite(pumpEnaPin, HIGH); // ENA
    digitalWrite(pumpIn1Pin, HIGH); // IN1
    digitalWrite(pumpIn2Pin, LOW);  // IN2
    isOn = true;

    delay(seconds * 1000);

    turnOff();
}

void PumpController::turnOff()
{
    if (!isOn)
    {
        return;
    }

    digitalWrite(pumpEnaPin, LOW); // ENA
    digitalWrite(pumpIn1Pin, LOW); // IN1
    digitalWrite(pumpIn2Pin, LOW); // IN2
    isOn = false;
}

void PumpController::update()
{
    // No se necesita implementar nada aquí para este ejemplo
}
