#ifndef STATUS_H
#define STATUS_H

#include <map>

enum class GeneralStatus : int
{
    Failed,
    Success,
    InProgress,
    Unknown
};

enum class SensorStatus : int
{
    FailedRead,
    FailSetup,
    InvalidPins,

    OkSetup,
    OkRead,
    OkLoop,

    InProgress,
    Unknown
};

enum class ActuatorStatus : int
{
    FailedWrite,
    FailSetup,
    InvalidPins,

    OkSetup,
    OkWrite,
    OkLoop,

    InProgress,
    Unknown
};

//create a function to convert from enum to string, using maps
static std::map<GeneralStatus, String> generalStatusMap = {
    {GeneralStatus::Failed, "Failed"},
    {GeneralStatus::Success, "Success"},
    {GeneralStatus::InProgress, "InProgress"},
    {GeneralStatus::Unknown, "Unknown"}
};

static std::map<SensorStatus, String> sensorStatusMap = {
    {SensorStatus::FailedRead, "FailedRead"},
    {SensorStatus::FailSetup, "FailSetup"},
    {SensorStatus::InvalidPins, "InvalidPins"},
    {SensorStatus::OkSetup, "OkSetup"},
    {SensorStatus::OkRead, "OkRead"},
    {SensorStatus::OkLoop, "OkLoop"},
    {SensorStatus::InProgress, "InProgress"},
    {SensorStatus::Unknown, "Unknown"}
};

static std::map<ActuatorStatus, String> actuatorStatusMap = {
    {ActuatorStatus::FailedWrite, "FailedWrite"},
    {ActuatorStatus::FailSetup, "FailSetup"},
    {ActuatorStatus::InvalidPins, "InvalidPins"},
    {ActuatorStatus::OkSetup, "OkSetup"},
    {ActuatorStatus::OkWrite, "OkWrite"},
    {ActuatorStatus::OkLoop, "OkLoop"},
    {ActuatorStatus::InProgress, "InProgress"},
    {ActuatorStatus::Unknown, "Unknown"}
};


#endif // STATUS_H
