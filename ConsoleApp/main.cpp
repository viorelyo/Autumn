#include <Windows.h>
#include <iostream>
#include <string>
#include "ImportFunctions.h"
#include "MyFuncs.h"
#include "ObjectCreatorDll.h"
#include "Comm.h"
#include "IFramework.h"
#include "IProcessMonitor.h"

void test_dll()
{
    HMODULE hDll = LoadLibrary("DLL_Exercise.dll");
    pSumFunction sumFunction;

    if (NULL != hDll)
    {
        sumFunction = (pSumFunction)GetProcAddress(hDll, "SumFunction");
        if (NULL != sumFunction)
        {
            std::cout << sumFunction(5, 6) << std::endl;
        }
        else
        {
            std::cout << "Could not load function." << std::endl;
        }

        FreeLibrary(hDll);
        hDll = NULL;
    }
    else
    {
        std::cout << "Could not load DLL." << std::endl;
    }
}

void test_lib()
{
    std::cout << SumFunction(6, 6) << std::endl;
}

void test_creatorDll()
{
    HMODULE hDll = LoadLibrary("ObjectCreatorDll.dll");
    pCreateObject CreateObject = NULL;
    pDestroyObject DestroyObject = NULL;

    if (NULL != hDll)
    {
        CreateObject = (pCreateObject)GetProcAddress(hDll, CreateObjectMethodName);
        if (NULL == CreateObject)
        {
            std::cout << "Could not load function " << CreateObjectMethodName << std::endl;
            return;
        }

        DestroyObject = (pDestroyObject)GetProcAddress(hDll, DestroyObjectMethodName);
        if (NULL == DestroyObject)
        {
            std::cout << "Could not load function " << DestroyObjectMethodName << std::endl;
            return;
        }
    }
    else
    {
        std::cout << "Could not load DLL." << std::endl;
        return;
    }

    // Get Multiplication
    IMultiplier *multiplier = nullptr;
    CreateObject(IMultiplier::GetName(), (void**)&multiplier);

    std::cout << multiplier->Multiply(2, 4) << std::endl;

    // Get Sum
    IAdder *adder = nullptr;
    CreateObject(IAdder::GetName(), (void**)&adder);

    std::cout << adder->Add(2, 4) << std::endl;

    DestroyObject(IMultiplier::GetName(), (void*)multiplier);
    DestroyObject(IAdder::GetName(), (void*)adder);
    multiplier = nullptr;
    adder = nullptr;

    if (NULL != hDll)
    {
        FreeLibrary(hDll);
        hDll = NULL;
    }
}

void sendMessages(IClient *client)
{
    int status = 0;
    Buffer req = Buffer();
    Buffer res = Buffer();
    std::string userInput;

    std::cout << "Operation: <op1 +/* op2>" << std::endl;
    std::cout << "> ";
    while (getline(std::cin, userInput))
    {
        req.content = std::vector<unsigned char>(userInput.begin(), userInput.end());
        req.content.push_back('\0');
        status = client->Send(req, res);
        if (0 != status)
        {
            break;
        }
        std::cout << "> ";
    }
}

void test_CommClient()
{
    HMODULE hDll = LoadLibrary("Autumn.dll");
    pCreateObject CreateObject = NULL;
    pDestroyObject DestroyObject = NULL;
    int status = 0;

    if (NULL != hDll)
    {
        CreateObject = (pCreateObject)GetProcAddress(hDll, CreateObjectMethodName);
        if (NULL == CreateObject)
        {
            std::cout << "Could not load function " << CreateObjectMethodName << std::endl;
            return;
        }

        DestroyObject = (pDestroyObject)GetProcAddress(hDll, DestroyObjectMethodName);
        if (NULL == DestroyObject)
        {
            std::cout << "Could not load function " << DestroyObjectMethodName << std::endl;
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
    framework->Init("framework.xml");

    // Instantiate client
    IClient *client = nullptr;
    if (0 != framework->GetPlugin("Client", (void**)&client))
    {
        return;
    }
    status = client->Connect(pipeName);
    if (0 != status)
    {
        std::cout << "Exiting..." << std::endl;
        return;
    }
    // wait for user input, send it to server and then get response
    sendMessages(client);
    client->Disconnect();

    // Unload client
    framework->FreePlugin("Client");
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

void test_Framework()
{
    HMODULE hDll = LoadLibrary("Autumn.dll");
    pCreateObject CreateObject = NULL;
    pDestroyObject DestroyObject = NULL;
    int status = 0;

    if (NULL != hDll)
    {
        CreateObject = (pCreateObject)GetProcAddress(hDll, CreateObjectMethodName);
        if (NULL == CreateObject)
        {
            std::cout << "Could not load function " << CreateObjectMethodName << std::endl;
            return;
        }

        DestroyObject = (pDestroyObject)GetProcAddress(hDll, DestroyObjectMethodName);
        if (NULL == DestroyObject)
        {
            std::cout << "Could not load function " << DestroyObjectMethodName << std::endl;
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
    framework->Init("framework.xml");

    IAdder *adder = nullptr;
    if (0 != framework->GetPlugin("Adder", (void**)&adder))
    {
        return;
    }
    std::cout << adder->Add(2, 4) << std::endl;

    IMultiplier *multiplier = nullptr;
    if (0 != framework->GetPlugin("Multiplier", (void**)&multiplier))
    {
        return;
    }
    std::cout << multiplier->Multiply(2, 4) << std::endl;

    framework->FreePlugin("Multiplier");
    framework->FreePlugin("Adder");
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

void test_ProcMon()
{
    HMODULE hDll = LoadLibrary("Autumn.dll");
    pCreateObject CreateObject = NULL;
    pDestroyObject DestroyObject = NULL;
    int status = 0;

    if (NULL != hDll)
    {
        CreateObject = (pCreateObject)GetProcAddress(hDll, CreateObjectMethodName);
        if (NULL == CreateObject)
        {
            std::cout << "Could not load function " << CreateObjectMethodName << std::endl;
            return;
        }

        DestroyObject = (pDestroyObject)GetProcAddress(hDll, DestroyObjectMethodName);
        if (NULL == DestroyObject)
        {
            std::cout << "Could not load function " << DestroyObjectMethodName << std::endl;
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
    framework->Init("framework.xml");

    // Create Process Monitor
    IProcessMonitor *procmon = nullptr;
    if (0 != framework->GetPlugin("ProcessMonitor", (void**)&procmon))
    {
        return;
    }

    // Create Observer
    IProcessMonitor::INotification *observer = nullptr;
    if (0 != framework->GetPlugin("Observer", (void**)&observer))
    {
        return;
    }
    
    procmon->Register(observer);
    ////USER INPUT
    std::string userInput;
    std::set<std::string> processesToBeMonitored;
    bool procMonStarted = false;

    std::cout << "Add processes:" << std::endl;
    std::cout << "> ";
    while (std::getline(std::cin, userInput))
    {
        if ("start" == userInput)
        {
            if (0 == processesToBeMonitored.size())
            {
                std::cout << "No processes added." << std::endl;
                std::cout << "> ";
            }
            else
            {
                std::cout << "ProcMon Started." << std::endl;
                procmon->SetMonitoredProcesses(processesToBeMonitored);
                procmon->Start();
                procMonStarted = true;
            }
        }
        else if ("stop" == userInput)
        {
            procmon->Stop();
            std::cout << "ProcMon Stopped." << std::endl;
            break;
        }
        else
        {
            if (procMonStarted)
            {
                std::cout << "ProcMon already started..." << std::endl;
            }
            else
            {
                processesToBeMonitored.insert(userInput);
                std::cout << "> ";
            }
        }
    }
    ////END USER INPUT


    // Unload framework
    framework->FreePlugin("ProcessMonitor");
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
    //test_dll();
    //test_lib();
    //test_creatorDll();
    //test_CommClient();
    //test_Framework();
    test_ProcMon();
   
    return 0;
}