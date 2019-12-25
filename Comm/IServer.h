#pragma once
#include "Buffer.h"

class IServer
{
public:
    struct IHandler
    {
        virtual int OnMessage(const Buffer &request, Buffer &response) = 0;
    };
    virtual int Init(const char *pipeName) = 0;
    virtual int SetHandler(IHandler *handler) = 0;

    virtual ~IServer() = default;
};