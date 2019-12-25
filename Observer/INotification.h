#pragma once
#include <string>

class INotification
{
public:
    virtual void OnProcessStarted(const std::string procName) const = 0;
    virtual void OnProcessEnded(const std::string procName) const = 0;

    virtual ~INotification() {};
};