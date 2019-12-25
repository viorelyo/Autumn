#pragma once


const char* CreateObjectMethodName = "CreateObject";
const char* DestroyObjectMethodName = "DestroyObject";

typedef void(*pCreateObject)(const char* name, void** obj);
typedef void(*pDestroyObject)(const char* name, void* obj);


class IAdder
{
public:
    virtual int Add(const int a, const int b) = 0;
    static const char* GetName() { return "Adder"; }
};

class IMultiplier
{
public:
    virtual int Multiply(const int a, const int b) = 0;
    static const char* GetName() { return "Multiplier"; }
};