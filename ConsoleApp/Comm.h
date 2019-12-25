#pragma once
#include <vector>

const char* pipeName = "\\\\.\\pipe\\mathOperationsPipe";

struct Buffer
{
    std::vector<unsigned char> content;
};

struct IClient
{
    virtual int Connect(const char * remote) = 0;
    virtual int Send(const Buffer & request, Buffer & reply) = 0;
    virtual int Disconnect() = 0;
    virtual ~IClient() {}

    static const char* GetName() { return "Client"; }
};