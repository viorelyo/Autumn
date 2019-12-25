#pragma once
#include "INotification.h"

class Observer : public INotification
{
public:
    void OnProcessStarted(const std::string procName) const override;
    void OnProcessEnded(const std::string procName) const override;
};