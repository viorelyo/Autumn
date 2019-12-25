#pragma once

typedef void(*pCreateObject)(const char* name, void** obj);
typedef void(*pDestroyObject)(const char* name, void* obj);