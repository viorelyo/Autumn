#include "stdafx.h"
#include "ExportFunctions.h"
#include "Adder.h"
#include "Multiplier.h"
#include <string>


void CreateObject(const char * name, void ** ptr)
{
    if (0 == strcmp(name, "Adder"))
    {
        IAdder *obj = new Adder();
        *ptr = obj;
    }
    else if (0 == strcmp(name, "Multiplier"))
    {
        IMultiplier *obj = new Multiplier();
        *ptr = obj;
    }
}

void DestroyObject(const char * name, void * ptr)
{
    if (0 == strcmp(name, "Adder"))
    {
        IAdder *obj = static_cast<Adder*>(ptr);
        delete obj;
    }
    else if (0 == strcmp(name, "Multiplier"))
    {
        IMultiplier *obj = static_cast<Multiplier*>(ptr);
        delete obj;
    }
}
