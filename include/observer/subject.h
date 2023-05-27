#ifndef subject_h
#define subject_h

#include <vector>
#include <observer/observer.h>

class Subject
{
    public:
        virtual void attach(MqttObserver *observer) = 0;
        virtual void detach(MqttObserver *observer) = 0;
        virtual void notify(char *topic, byte *payload, unsigned int length) = 0;
};

#endif