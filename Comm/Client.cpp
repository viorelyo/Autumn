#include "stdafx.h"
#include "Client.h"
#include <iostream>


int CommClient::Connect(const char * remote)
{
    DWORD dwMode = NULL;
    BOOL fSuccess = FALSE;

    while (true)
    {
        hPipe = CreateFileA(
            remote,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);
        // Break if the pipe handle is valid. 
        if (INVALID_HANDLE_VALUE != hPipe)
        {
            break;
        }
        // Exit if an error other than ERROR_PIPE_BUSY occurs. 
        if (ERROR_PIPE_BUSY != GetLastError())
        {
            std::cout << "Could not open pipe. Error: " << GetLastError() << std::endl;
            return -1;
        }
        // All pipe instances are busy, so wait for 20 seconds. 
        if (!WaitNamedPipeA(remote, 20000))
        {
            std::cout << "Could not open pipe: 20 second wait timed out." << std::endl;
            return -1;
        }
    }

    return 0;
}

int CommClient::Send(const Buffer & request, Buffer & reply)
{
    DWORD cbToWrite, cbWritten, cbRead;
    BOOL fSuccess = FALSE;
    unsigned char *buffer = nullptr;

    cbToWrite = (request.content.size() * sizeof(unsigned char)); 
    std::cout << "Sending message: " << &(request.content[0]) << std::endl;

    fSuccess = WriteFile(
        hPipe,
        &(request.content[0]),   
        cbToWrite,
        &cbWritten,
        NULL);
    if (!fSuccess)
    {
        std::cout << "WriteFile to pipe failed. Error: " << GetLastError() << std::endl;
        return -1;
    }

    std::cout << "Message sent to server. Receiving response as follows : " << std::endl;
    buffer = new unsigned char[BUFSIZ];
    do
    {
        fSuccess = ReadFile(
            hPipe,
            buffer,
            BUFSIZ * sizeof(unsigned char),
            &cbRead,
            NULL);
        if (!fSuccess && (ERROR_MORE_DATA != GetLastError()))
        {
            break;
        }
        
        reply.content = std::vector<unsigned char>(buffer, buffer + cbRead);
        std::cout << buffer << std::endl;
    } while (!fSuccess);

    delete[] buffer; 

    if (!fSuccess)
    {
        std::cout << "ReadFile from pipe failed. Error: " << GetLastError() << std::endl;
        return -1;
    }

    return 0;
}

int CommClient::Disconnect()
{
    CloseHandle(hPipe);
    return 0;
}
