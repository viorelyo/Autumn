#pragma once
#include "IClient.h"
#include <Windows.h>


class CommClient : public IClient
{
private:
    HANDLE hPipe;
public:
    int Connect(const char *remote) override;
    int Send(const Buffer &request, Buffer &reply) override;
    int Disconnect() override;
};