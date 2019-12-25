#pragma once
#include "IServer.h"
#include <Windows.h>
#include <vector>
#include <thread>
#include <atomic>


class CommServer : public IServer
{
private:
    IHandler *serverHandler;
    std::vector<std::thread> clientThreads;
    std::thread clientsProcessor;
    HANDLE stopEvent;

    void InstanceThread(HANDLE hPipe, OVERLAPPED oConnect);
    int WaitForClients(const char *pipeName);
public:
    ~CommServer();
    int Init(const char *pipeName) override;
    virtual int SetHandler(IHandler *handler) override;
};