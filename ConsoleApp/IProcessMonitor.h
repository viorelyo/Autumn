#pragma once
#include <string>
#include <set>

class IProcessMonitor
{
public:
    class INotification
    {
    public:
        virtual void OnProcessStarted(const std::string procName) const = 0;
        virtual void OnProcessEnded(const std::string procName) const = 0;

        virtual ~INotification() {};
    };

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void SetMonitoredProcesses(const std::set<std::string>& monitoredProcesses) = 0;
    virtual void Register(const INotification* observer) = 0;
    virtual void UnRegister(const INotification* observer) = 0;

    virtual ~IProcessMonitor() {};
};