#pragma once
#include <vector>


struct Buffer
{
    std::vector<unsigned char> content;
};

struct IServer
{
    struct IHandler
    {
        virtual int OnMessage(const Buffer & request, Buffer & response) = 0;
    };

    virtual int Init(const char * pipeName) = 0;
    virtual int SetHandler(IHandler * handler) = 0;
    virtual ~IServer() {}

    static const char* GetName() { return "Server"; }
};