#pragma once
#include "IFramework.h"
#include "Plugin.h"
#include <map>
#include <string>
#include <Windows.h>


class Framework : public IFramework
{
private:
    std::map<std::string, Plugin> plugins;
    std::map<std::string, HMODULE> dllHandles;
    std::map<std::string, std::string> dlls;

    int loadPluginFromDll(std::string &dllName, std::string &pluginName);
public:
    int Init(const char * configFile) override;
    int Uninit() override;

    int GetPlugin(const char * pluginName, void ** instance) override;
    int FreePlugin(const char * pluginName) override;
};