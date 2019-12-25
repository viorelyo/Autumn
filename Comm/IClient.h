#pragma once
#include "Buffer.h"


class IClient
{
public:
    virtual int Connect(const char *remote) = 0;
    virtual int Send(const Buffer &request, Buffer &reply) = 0;
    virtual int Disconnect() = 0;

    virtual ~IClient() = default;
};