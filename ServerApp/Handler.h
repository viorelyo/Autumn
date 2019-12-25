#pragma once
#include "Comm.h"


class OperationHandler : public IServer::IHandler
{
private:
    int doOperation(std::string &request);
    void parseRequest(std::string &request, int *operand1, int *operand2, std::string &command);
    int doAddition(int operand1, int operand2);
    int doMultiplication(int operand1, int operand2);
public:
    int OnMessage(const Buffer &request, Buffer &response) override;
};