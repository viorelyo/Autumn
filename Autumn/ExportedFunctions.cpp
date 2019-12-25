#include "ExportedFunctions.h"
#include "Framework.h"
#include <string>


void CreateObject(const char * name, void ** ptr)
{
    if (0 == strcmp(name, "Framework"))
    {
        IFramework *obj = new Framework();
        *ptr = obj;
    }
}

void DestroyObject(const char * name, void * ptr)
{
    if (0 == strcmp(name, "Framework"))
    {
        IFramework *obj = static_cast<Framework*>(ptr);
        delete obj;
    }
}
