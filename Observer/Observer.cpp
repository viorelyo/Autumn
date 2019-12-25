#include "Observer.h"
#include <iostream>

void Observer::OnProcessStarted(const std::string procName) const
{
    std::cout << "START: " << procName << std::endl;
}

void Observer::OnProcessEnded(const std::string procName) const
{
    std::cout << "STOP: " << procName << std::endl;
}
