#pragma once

extern "C"
{
    __declspec(dllexport) void CreateObject(const char* name, void **ptr);
    __declspec(dllexport) void DestroyObject(const char* name, void *ptr);
}