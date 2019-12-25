// Framework.cpp : Defines the exported functions for the DLL application.
//

#include "Framework.h"
#include <iostream>
#include "Utils.h"
#include "tinyxml2.h"


int Framework::loadPluginFromDll(std::string &dllName, std::string &pluginName)
{
    HMODULE hDll = NULL;
    pCreateObject CreateObject = NULL;

    // check if Dll is already loaded
    if (dllHandles.find(dllName) == dllHandles.end())
    {
        hDll = LoadLibrary(std::wstring(dllName.begin(), dllName.end()).c_str());
        dllHandles.insert({ dllName, hDll });
    }
    else
    {
        hDll = dllHandles.at(dllName);
    }

    if (NULL != hDll)
    {
        CreateObject = (pCreateObject)GetProcAddress(hDll, "CreateObject");
        if (NULL == CreateObject)
        {
            std::cout << "Could not load function CreateFunction" << std::endl;
            return -1;
        }
    }
    else
    {
        std::cout << "Could not load DLL." << std::endl;
        return -1;
    }

    Plugin plugin;
    plugin.pluginObj = nullptr;
    plugin.refCount = 0;
    CreateObject(pluginName.c_str(), (void**)&plugin.pluginObj);

    plugins.insert({ pluginName, plugin });

    return 0;
}

int Framework::Init(const char * configFile)
{
    tinyxml2::XMLError status;
    tinyxml2::XMLDocument doc;

    status = doc.LoadFile(configFile);
    if (tinyxml2::XML_SUCCESS != status)
    {
        std::cout << "Couldn't load XML file." << std::endl;
        return -1;
    }

    tinyxml2::XMLElement *root = doc.FirstChildElement("settings");
    if (nullptr == root)
    {
        std::cout << "Couldn't get first Element from XML file." << std::endl;
        return -1;
    }

    tinyxml2::XMLElement *element = root->FirstChildElement("plugin");
    while (nullptr != element)
    {
        // Extract attributes of plugins
        std::string name = element->Attribute("name");
        std::string dll = element->Attribute("dll");
        std::string startup = element->Attribute("startup");

        // Insert dll in dlls map
        dlls.insert({ name, dll });

        // Load plugins
        if ("1" == startup)
        {
            loadPluginFromDll(dll, name);
        }

        // Go to next element
        element = element->NextSiblingElement("plugin");
    }

    return 0;
}

int Framework::Uninit()
{
    std::map<std::string, Plugin>::iterator it;
    for (it = plugins.begin(); it != plugins.end();)
    {
        if (0 != it->second.refCount)
        {
            std::cout << "Plugin: " << it->first << " still has references." << std::endl;
            return -1;
        }
        else
        {
            plugins.erase(it++);
        }
    }

    return 0;
}

int Framework::GetPlugin(const char * pluginName, void ** instance)
{
    // If there is no plugin in plugins, load it
    if (plugins.find(pluginName) == plugins.end())
    {
        if (dlls.find(pluginName) == dlls.end())
        {
            std::cout << "Unknown plugin." << std::endl;
            return -1;
        }
        loadPluginFromDll(dlls.at(pluginName), std::string(pluginName));
    }

    Plugin *plugin = &plugins.at(pluginName);

    plugin->refCount++;
    *instance = plugin->pluginObj;

    return 0;
}

int Framework::FreePlugin(const char * pluginName)
{
    if (plugins.find(pluginName) == plugins.end())
    {
        std::cout << "Unknown plugin." << std::endl;
        return -1;
    }

    Plugin *plugin = &plugins.at(pluginName);
    plugin->refCount--;
    if (0 != plugin->refCount)
    {
        return 0;
    }

    // if refCount of Plugin == 0, unload plugin
    std::string dllName = dlls.at(pluginName);
    HMODULE *hDll = &dllHandles.at(dllName);
    pDestroyObject DestroyObject = NULL;

    if (NULL != hDll)
    {
        DestroyObject = (pDestroyObject)GetProcAddress(*hDll, "DestroyObject");
        if (NULL == DestroyObject)
        {
            std::cout << "Could not load function DestroyObject" << std::endl;
            return -1;
        }
    }
    else
    {
        std::cout << "Could not load DLL." << std::endl;
        return -1;
    }

    DestroyObject(pluginName, (void*)plugin->pluginObj);

    std::map<std::string, Plugin>::iterator pluginsIt = plugins.find(pluginName);
    std::map<std::string, std::string>::iterator dllsIt = dlls.find(pluginName);
    plugins.erase(pluginsIt);
    dlls.erase(dllsIt);

    // Check if current DLL is used by other plugins
    bool isDllUsed = false;
    for (dllsIt = dlls.begin(); dllsIt != dlls.end(); ++dllsIt)
    {
        if (dllsIt->second == dllName)
        {
            isDllUsed = true;
            break;
        }
    }
    
    // Unload unused DLL
    if (!isDllUsed)
    {
        if (NULL != *hDll)
        {
            FreeLibrary(*hDll);
            *hDll = NULL;
        }
        std::map<std::string, HMODULE>::iterator dllHandlesIt = dllHandles.find(dllName);
        dllHandles.erase(dllHandlesIt);
    }

    return 0;
}
