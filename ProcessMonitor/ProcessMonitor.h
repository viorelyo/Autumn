#pragma once
#include "IProcessMonitor.h"
#define _WIN32_DCOM
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <Windows.h>

#pragma comment(lib, "wbemuuid.lib")




class ProcessMonitor : public IProcessMonitor
{
public:
    class EventSink : public IWbemObjectSink
    {
    private:
        LONG m_lRef;
        bool bDone;

        ProcessMonitor& parent;
    public:
        EventSink(ProcessMonitor & x) : parent(x) { m_lRef = 0; }
        ~EventSink() { bDone = true; }

        virtual ULONG STDMETHODCALLTYPE AddRef();
        virtual ULONG STDMETHODCALLTYPE Release();
        virtual HRESULT
            STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

        virtual HRESULT STDMETHODCALLTYPE Indicate(
            LONG lObjectCount,
            IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
        );

        virtual HRESULT STDMETHODCALLTYPE SetStatus(
            /* [in] */ LONG lFlags,
            /* [in] */ HRESULT hResult,
            /* [in] */ BSTR strParam,
            /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
        );
    };

    struct WMIStruct
    {
        HRESULT hres;
        IWbemServices *pSvc;
        IWbemLocator *pLoc;
        IUnsecuredApartment* pUnsecApp;
        IUnknown* pStubUnk;
        EventSink* pSink;
        IWbemObjectSink* pStubSink;
    };
public:
    virtual void Start() override;
    virtual void Stop() override;
    void SetMonitoredProcesses(const std::set<std::string>& monitoredProcesses) override;
    void Register(const INotification* observer) override;
    void UnRegister(const INotification* observer) override;

private:
    std::set<std::string> monitoredProcesses;
    std::set<const INotification*> observers;
    WMIStruct* wmiStruct;

    int startProcessListening(std::string & processes);
    std::string createStringOfProcessesToMonitorForWMI();
};


