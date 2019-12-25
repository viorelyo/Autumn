#include "ExportedFunctions.h"
#include "ProcessMonitor.h"
#include <string>


void CreateObject(const char * name, void ** ptr)
{
    if (0 == strcmp(name, "ProcessMonitor"))
    {
        IProcessMonitor *obj = new ProcessMonitor();
        *ptr = obj;
    }
}

void DestroyObject(const char * name, void * ptr)
{
    if (0 == strcmp(name, "ProcessMonitor"))
    {
        IProcessMonitor *obj = static_cast<ProcessMonitor*>(ptr);
        delete obj;
    }
}