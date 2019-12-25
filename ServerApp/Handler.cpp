#include "Handler.h"
#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>
#include "ImportedFunctions.h"


int OperationHandler::OnMessage(const Buffer & request, Buffer & response)
{
    int returnStatus = 0;
    int result = 0;
    std::string reqString(request.content.begin(), request.content.end() - 1);
    std::string resultStr;

    if ("exit" == reqString)
    {
        returnStatus = -1;
        resultStr = "Exiting...";
    }
    else 
    {
        try
        {
            result = doOperation(reqString);
            resultStr = std::to_string(result);
        }
        catch (std::exception ex)
        {
            std::cout << ex.what() << std::endl;
            resultStr = "Invalid.";
        }
    }

    response.content = std::vector<unsigned char>(resultStr.begin(), resultStr.end());
    response.content.push_back('\0');

    return returnStatus;
}

int OperationHandler::doOperation(std::string &request)
{
    int operand1 = 0,
        operand2 = 0,
        result = 0;
    std::string command;

    try
    {
        parseRequest(request, &operand1, &operand2, command);
    }
    catch (std::exception ex)
    {
        std::cout << ex.what() << std::endl;
        throw std::exception("Couldn't do operation.");
    }

    // Do operation
    if ("+" == command)
    {
        try
        {
            result = doAddition(operand1, operand2);
        }
        catch (std::exception ex)
        {
            std::cout << ex.what() << std::endl;
            throw std::exception("Couldn't do operation.");
        }
    }
    else if ("*" == command)
    {
        try
        {
            result = doMultiplication(operand1, operand2);
        }
        catch (std::exception ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }
    else
    {
        throw std::exception("Couldn't do operation. Operation unknown.");
    }
    

    return result;
}

void OperationHandler::parseRequest(std::string &request, int *operand1, int *operand2, std::string &command)
{
    std::string operand1Str;
    std::string operand2Str;
    std::istringstream iss(request);

    // Split request by space
    iss >> operand1Str;
    if (0 == iss.rdbuf()->in_avail())       // if there aren't any chars left in the stringstream
    {
        throw std::exception("Parsing error. Couldn't read command from request");
    }
    iss >> command;
    if (0 == iss.rdbuf()->in_avail())       // if there aren't any chars left in the stringstream
    {
        throw std::exception("Parsing error. Couldn't read operand2 from request");
    }
    iss >> operand2Str;

    // Convert string operators to ints
    try
    {
        *operand1 = std::stoi(operand1Str);
    }
    catch (std::invalid_argument)
    {
        throw std::exception("Invalid number error. Couldn't convert operand1 to int");
    }

    try
    {
        *operand2 = std::stoi(operand2Str);
    }
    catch (std::invalid_argument)
    {
        throw std::exception("Invalid number error. Couldn't convert operand2 to int");
    }
}

int OperationHandler::doAddition(int operand1, int operand2)
{
    int result = 0;
    HMODULE hDll = LoadLibrary("ObjectCreatorDll.dll");
    pCreateObject CreateObject = NULL;
    pDestroyObject DestroyObject = NULL;

    if (NULL != hDll)
    {
        CreateObject = (pCreateObject)GetProcAddress(hDll, "CreateObject");
        if (NULL == CreateObject)
        {
            throw std::exception("Could not load function ");
        }

        DestroyObject = (pDestroyObject)GetProcAddress(hDll, "DestroyObject");
        if (NULL == DestroyObject)
        {
            throw std::exception("Could not load function ");
        }
    }
    else
    {
        throw std::exception("Could not load DLL.");
    }

    // Get Sum
    IAdder *adder = nullptr;
    CreateObject(IAdder::GetName(), (void**)&adder);

    result = adder->Add(operand1, operand2);

    DestroyObject(IAdder::GetName(), (void*)adder);
    adder = nullptr;

    if (NULL != hDll)
    {
        FreeLibrary(hDll);
        hDll = NULL;
    }

    return result;
}

int OperationHandler::doMultiplication(int operand1, int operand2)
{
    int result = 0;
    HMODULE hDll = LoadLibrary("ObjectCreatorDll.dll");
    pCreateObject CreateObject = NULL;
    pDestroyObject DestroyObject = NULL;

    if (NULL != hDll)
    {
        CreateObject = (pCreateObject)GetProcAddress(hDll, "CreateObject");
        if (NULL == CreateObject)
        {
            throw std::exception("Could not load function ");
        }

        DestroyObject = (pDestroyObject)GetProcAddress(hDll, "DestroyObject");
        if (NULL == DestroyObject)
        {
            throw std::exception("Could not load function ");
        }
    }
    else
    {
        throw std::exception("Could not load DLL.");
    }

    // Get Multiplication
    IMultiplier *multiplier = nullptr;
    CreateObject(IMultiplier::GetName(), (void**)&multiplier);

    result = multiplier->Multiply(operand1, operand2);

    DestroyObject(IMultiplier::GetName(), (void*)multiplier);
    multiplier = nullptr;

    if (NULL != hDll)
    {
        FreeLibrary(hDll);
        hDll = NULL;
    }

    return result;
}