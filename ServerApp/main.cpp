#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include "ImportedFunctions.h"
#include "Handler.h"
#include "IFramework.h"

const char* pipeName = "\\\\.\\pipe\\mathOperationsPipe";


void test_CommServer()
{
    HMODULE hDll = LoadLibrary("Autumn.dll");
    pCreateObject CreateObject = NULL;
    pDestroyObject DestroyObject = NULL;
    int status = 0;

    if (NULL != hDll)
    {
        CreateObject = (pCreateObject)GetProcAddress(hDll, "CreateObject");
        if (NULL == CreateObject)
        {
            std::cout << "Could not load function CreateObject" << std::endl;
            return;
        }

        DestroyObject = (pDestroyObject)GetProcAddress(hDll, "DestroyObject");
        if (NULL == DestroyObject)
        {
            std::cout << "Could not load function DestroyObject" << std::endl;
            return;
        }
    }
    else
    {
        std::cout << "Could not load DLL." << std::endl;
        return;
    }

    // Instantiate framework
    IFramework *framework = nullptr;
    CreateObject("Framework", (void**)&framework);
    framework->Init("C:\\GIT\\vgurdis\\Endpoint_C++\\DLL_Lib_Exercise\\x64\\Debug\\framework.xml");

    // Instantiate server
    IServer *server = nullptr;
    if (0 != framework->GetPlugin("Server", (void**)&server))
    {
        return;
    }
    OperationHandler serverHandler;
    server->SetHandler(&serverHandler);
    server->Init(pipeName);

    std::string input;
    while (getline(std::cin, input))
    {
        if ("0" == input)
        {
            DestroyObject(IServer::GetName(), (void*)server);
            break;
        }
    }

    // Unload client
    framework->FreePlugin("Server");
    if (-1 == framework->Uninit())
    {
        std::cout << "Couldn't uninit framework." << std::endl;
        return;
    }
    DestroyObject("Framework", (void*)framework);

    if (NULL != hDll)
    {
        FreeLibrary(hDll);
        hDll = NULL;
    }
}


int main()
{
    test_CommServer();

    //_CrtDumpMemoryLeaks();
    return 0;
}