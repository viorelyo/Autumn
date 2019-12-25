#include "stdafx.h"
#include "ExportedFunctions.h"
#include "Client.h"
#include "Server.h"
#include <string>


void CreateObject(const char * name, void ** ptr)
{
    if (0 == strcmp(name, "Client"))
    {
        IClient *obj = new CommClient();
        *ptr = obj;
    }
    else if (0 == strcmp(name, "Server"))
    {
        IServer *obj = new CommServer();
        *ptr = obj;
    }
}

void DestroyObject(const char * name, void * ptr)
{
    if (0 == strcmp(name, "Client"))
    {
        IClient *obj = static_cast<CommClient*>(ptr);
        delete obj;
    }
    else if (0 == strcmp(name, "Server"))
    {
        IServer *obj = static_cast<CommServer*>(ptr);
        delete obj;
    }
}
