#include "stdafx.h"
#include "Server.h"
#include <iostream>


CommServer::~CommServer()
{
    SetEvent(stopEvent);

    for (auto &t : clientThreads)
    {
        t.join();
    }

    if (clientsProcessor.joinable())
    {
        clientsProcessor.join();
    }
    CloseHandle(stopEvent);

    std::cout << "Server is exiting..." << std::endl;
}

int CommServer::Init(const char *pipeName)
{
    stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    clientsProcessor = std::thread([this, pipeName]() { WaitForClients(pipeName); });

    return 0;
}

int CommServer::WaitForClients(const char *pipeName)
{
    HANDLE hPipe = INVALID_HANDLE_VALUE;
    BOOL fConnected = FALSE;
    std::atomic_bool serverRunning = true;

    std::cout << "Pipe Server : Main thread awaiting client connection on " << pipeName << std::endl;
    while (serverRunning)
    {
        hPipe = CreateNamedPipeA(
            pipeName,
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            BUFSIZ,
            BUFSIZ,
            0,      // client time-out
            NULL);
        if (INVALID_HANDLE_VALUE == hPipe)
        {
            std::cout << "CreateNamedPipe failed. Error: " << GetLastError() << std::endl;
            return -1;
        }

        OVERLAPPED oConnect = { 0, 0, 0, 0 };
        HANDLE handleEvents[2];

        handleEvents[0] = oConnect.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        handleEvents[1] = this->stopEvent;

        // Wait for the client to connect
        // If the function returns zero, GetLastError returns ERROR_PIPE_CONNECTED.
        fConnected = ConnectNamedPipe(hPipe, &oConnect);
        if (0 == fConnected)
        {
            switch (WaitForMultipleObjects(2, handleEvents, FALSE, INFINITE))
            {
            case WAIT_OBJECT_0:
                // Handle the event from OVERLAPPED struct
                std::cout << "Client connected, creating a processing thread..." << std::endl;
                clientThreads.push_back(std::thread([this, hPipe, oConnect]() { InstanceThread(hPipe, oConnect); }));
                //ResetEvent(handleEvents[0]);
                break;

            case WAIT_OBJECT_0 + 1:
                // Waiting for stopEvent
                //CloseHandle(hPipe);
                CloseHandle(handleEvents[0]);
                serverRunning = false;
                break;
            }
        }
        else
        {
            CloseHandle(hPipe);
        }
    }

    return 0;
}


int CommServer::SetHandler(IHandler *handler)
{
    serverHandler = handler;
    return 0;
}

void CommServer::InstanceThread(HANDLE hPipe, OVERLAPPED oConnect)
{
    std::atomic_bool clientRunning = true;
    BOOL fSuccess = FALSE;
    DWORD cbBytesRead = 0, 
        cbWritten = 0,
        cbReplyBytes = 0;
    unsigned char *pchRequest = new unsigned char[BUFSIZ];
    unsigned char *pchReply = nullptr;
    Buffer request = Buffer();
    Buffer response = Buffer();

    HANDLE handleEvents[2] = { oConnect.hEvent, stopEvent };

    if (NULL == hPipe)
    {
        std::cout << "ERROR - Pipe Server Failure:" << std::endl;
        std::cout << "   InstanceThread got an unexpected NULL value in lpvParam." << std::endl;
        std::cout << "   InstanceThread exiting." << std::endl;

        delete[] pchRequest;
        return;
    }

    std::cout << "InstanceThread created, receiving and processing messages..." << std::endl;
    while (clientRunning)
    {
        // Read client requests from the pipe
        ReadFile(
            hPipe,
            pchRequest,          // buffer to receive data 
            BUFSIZ * sizeof(unsigned char),
            &cbBytesRead, 
            &oConnect);

        switch (WaitForMultipleObjects(2, handleEvents, FALSE, INFINITE))
        {
        case WAIT_OBJECT_0 + 1:
            std::cout << "Server closed" << std::endl;
            // Waiting for stopEvent      
            goto Cleanup;
            break;
        }

        // Process incoming message
        GetOverlappedResult(hPipe, &oConnect, &cbBytesRead, FALSE);     // get the actual value of cbBytesRead
        request.content.assign(pchRequest, pchRequest + cbBytesRead);
        if (-1 == this->serverHandler->OnMessage(request, response))
        {
            std::cout << "InstanceThread: client disconnected." << std::endl;
            clientRunning = false;
        }
        pchReply = &(response.content[0]);
        cbReplyBytes = response.content.size();

        // Write the reply to the pipe.
        WriteFile(
            hPipe,       
            pchReply,    
            cbReplyBytes,
            &cbWritten, 
            &oConnect);
    }

Cleanup:
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    delete[] pchRequest;

    std::cout << "InstanceThread exiting." << std::endl;
}