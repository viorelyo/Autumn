#pragma once

class IFramework
{
public:
    virtual int Init(const char * configFile) = 0;
    virtual int Uninit() = 0;

    virtual int GetPlugin(const char * pluginName, void ** instance) = 0;
    virtual int FreePlugin(const char * pluginName) = 0;
};