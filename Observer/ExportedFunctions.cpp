#include "ExportedFunctions.h"
#include "Observer.h"
#include <string>


void CreateObject(const char * name, void ** ptr)
{
    if (0 == strcmp(name, "Observer"))
    {
        INotification *obj = new Observer();
        *ptr = obj;
    }
}

void DestroyObject(const char * name, void * ptr)
{
    if (0 == strcmp(name, "Observer"))
    {
        INotification *obj = static_cast<Observer*>(ptr);
        delete obj;
    }
}