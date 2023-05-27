#ifndef observer_h
#define observer_h

class MqttObserver
{
    public:
        virtual void update() = 0;
};

#endif