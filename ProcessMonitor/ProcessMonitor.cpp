#include "ProcessMonitor.h"



int ProcessMonitor::startProcessListening(std::string & processes)
{
    HRESULT hres;
    
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        std::cout << "Failed to initialize COM library. Error code = 0x"
            << std::hex << hres << std::endl;
        return 1;                  // Program has failed.
    }

    // Set general COM security levels 
    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM negotiates service
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
    );
    if (FAILED(hres))
    {
        std::cout << "Failed to initialize security. Error code = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return 1;                      // Program has failed.
    }

    // Obtain the initial locator to WMI 
    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID *)&pLoc);
    if (FAILED(hres))
    {
        std::cout << "Failed to create IWbemLocator object. "
            << "Err code = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return 1;                 // Program has failed.
    }

    // Connect to WMI through the IWbemLocator::ConnectServer method
    IWbemServices *pSvc = NULL;

    // Connect to the local root\cimv2 namespace
    // and obtain pointer pSvc to make IWbemServices calls.
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        NULL,
        NULL,
        0,
        NULL,
        0,
        0,
        &pSvc
    );
    if (FAILED(hres))
    {
        std::cout << "Could not connect. Error code = 0x"
            << std::hex << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        return 1;                // Program has failed.
    }

    std::cout << "Connected to ROOT\\CIMV2 WMI namespace" << std::endl;

    // Set security levels on the proxy 
    hres = CoSetProxyBlanket(
        pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
        NULL,                        // Server principal name 
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities 
    );
    if (FAILED(hres))
    {
        std::cout << "Could not set proxy blanket. Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;               // Program has failed.
    }

    IUnsecuredApartment* pUnsecApp = NULL;

    hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL,
        CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment,
        (void**)&pUnsecApp);

    EventSink* pSink = new EventSink(*this);
    pSink->AddRef();

    IUnknown* pStubUnk = NULL;
    pUnsecApp->CreateObjectStub(pSink, &pStubUnk);

    IWbemObjectSink* pStubSink = NULL;
    pStubUnk->QueryInterface(IID_IWbemObjectSink,
        (void **)&pStubSink);

    // Create Query for WMI to monitor InstanceCreationEvent
    std::string wmiQueryCreation = "SELECT * FROM __InstanceCreationEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process' AND" + processes;
    // The ExecNotificationQueryAsync method will call the EventQuery::Indicate method when an event occurs
    hres = pSvc->ExecNotificationQueryAsync(
        _bstr_t("WQL"),
        _bstr_t(wmiQueryCreation.c_str()),
        WBEM_FLAG_SEND_STATUS,
        NULL,
        pStubSink);
    if (FAILED(hres))
    {
        printf("ExecNotificationQueryAsync failed "
            "with = 0x%X\n", hres);
        pSvc->Release();
        pLoc->Release();
        pUnsecApp->Release();
        pStubUnk->Release();
        pSink->Release();
        pStubSink->Release();
        CoUninitialize();
        return 1;
    }

    // Create Query for WMI to monitor InstanceDeletionEvent
    std::string wmiQueryDeletion = "SELECT * FROM __InstanceDeletionEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process' AND" + processes;
    // The ExecNotificationQueryAsync method will call the EventQuery::Indicate method when an event occurs
    hres = pSvc->ExecNotificationQueryAsync(
        _bstr_t("WQL"),
        _bstr_t(wmiQueryDeletion.c_str()),
        WBEM_FLAG_SEND_STATUS,
        NULL,
        pStubSink);
    if (FAILED(hres))
    {
        printf("ExecNotificationQueryAsync failed "
            "with = 0x%X\n", hres);
        pSvc->Release();
        pLoc->Release();
        pUnsecApp->Release();
        pStubUnk->Release();
        pSink->Release();
        pStubSink->Release();
        CoUninitialize();
        return 1;
    }

    wmiStruct->pSvc = pSvc;
    wmiStruct->pLoc = pLoc;
    wmiStruct->pUnsecApp = pUnsecApp;
    wmiStruct->pStubUnk = pStubUnk;
    wmiStruct->pStubSink = pStubSink;
    wmiStruct->pSink = pSink;
}

std::string ProcessMonitor::createStringOfProcessesToMonitorForWMI()
{
    std::string finalString = " (";
    std::string helperString = "TargetInstance.Name LIKE ";


    for (std::set<std::string>::iterator it = monitoredProcesses.begin(); it != monitoredProcesses.end(); ++it)
    {
        finalString += helperString;
        finalString += "'";
        finalString += *it;
        finalString += "'";

        std::set<std::string>::iterator tmpIt = it;
        if (++tmpIt != monitoredProcesses.end())
        {
            finalString += " OR ";
        }
    }
    finalString += ")";
    return finalString;
}

void ProcessMonitor::Start()
{
    if (0 == monitoredProcesses.size())
    {
        std::cout << "There are no processes to be monitored." << std::endl;
        return;
    }
    wmiStruct = new WMIStruct();
    startProcessListening(createStringOfProcessesToMonitorForWMI());
}

void ProcessMonitor::Stop()
{
    wmiStruct->hres = wmiStruct->pSvc->CancelAsyncCall(wmiStruct->pStubSink);

    // Cleanup
    wmiStruct->pSvc->Release();
    wmiStruct->pLoc->Release();
    wmiStruct->pUnsecApp->Release();
    wmiStruct->pStubUnk->Release();
    wmiStruct->pSink->Release();
    wmiStruct->pStubSink->Release();
    CoUninitialize();

    delete wmiStruct;
}

void ProcessMonitor::SetMonitoredProcesses(const std::set<std::string>& monitoredProcesses)
{
    this->monitoredProcesses = monitoredProcesses;
}

void ProcessMonitor::Register(const INotification * observer)
{
    observers.insert(observer);
}

void ProcessMonitor::UnRegister(const INotification * observer)
{
    std::set<const INotification*>::iterator it = observers.find(observer);
    observers.erase(it);
}


ULONG ProcessMonitor::EventSink::AddRef()
{
    return InterlockedIncrement(&m_lRef);
}

ULONG ProcessMonitor::EventSink::Release()
{
    LONG lRef = InterlockedDecrement(&m_lRef);
    if (lRef == 0)
        delete this;
    return lRef;
}

HRESULT ProcessMonitor::EventSink::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
    {
        *ppv = (IWbemObjectSink *) this;
        AddRef();
        return WBEM_S_NO_ERROR;
    }
    else return E_NOINTERFACE;
}


HRESULT ProcessMonitor::EventSink::Indicate(long lObjectCount,
    IWbemClassObject **apObjArray)
{
    HRESULT hr = S_OK;
    _variant_t vtProp;

    // Notify each observer
    for (const auto &observer : parent.observers)
    {
        for (int i = 0; i < lObjectCount; i++)
        {
            if (apObjArray[i]->InheritsFrom(L"__InstanceCreationEvent") != WBEM_S_FALSE)
            {
                hr = apObjArray[i]->Get(_bstr_t(L"TargetInstance"), 0, &vtProp, 0, 0);
                if (!FAILED(hr))
                {
                    IUnknown* str = vtProp;
                    hr = str->QueryInterface(IID_IWbemClassObject, reinterpret_cast<void**>(&apObjArray[i]));
                    if (SUCCEEDED(hr))
                    {
                        _variant_t cn;
                        hr = apObjArray[i]->Get(L"Name", 0, &cn, NULL, NULL);
                        if (SUCCEEDED(hr))
                        {
                            //std::wcout << "START ";
                            if ((cn.vt == VT_NULL) || (cn.vt == VT_EMPTY))
                                std::wcout << "Name : " << ((cn.vt == VT_NULL) ? "NULL" : "EMPTY") << std::endl;
                            else
                            {
                                //std::wcout << "Name : " << cn.bstrVal << std::endl;
                                std::string procName = _bstr_t(cn.bstrVal);
                                observer->OnProcessStarted(procName);
                            }
                        }
                        VariantClear(&cn);
                    }
                }
                VariantClear(&vtProp);
            }
            else if (apObjArray[i]->InheritsFrom(L"__InstanceDeletionEvent") != WBEM_S_FALSE)
            {
                hr = apObjArray[i]->Get(_bstr_t(L"TargetInstance"), 0, &vtProp, 0, 0);
                if (!FAILED(hr))
                {
                    IUnknown* str = vtProp;
                    hr = str->QueryInterface(IID_IWbemClassObject, reinterpret_cast<void**>(&apObjArray[i]));
                    if (SUCCEEDED(hr))
                    {
                        _variant_t cn;
                        hr = apObjArray[i]->Get(L"Name", 0, &cn, NULL, NULL);
                        if (SUCCEEDED(hr))
                        {
                            //std::wcout << "STOP ";
                            if ((cn.vt == VT_NULL) || (cn.vt == VT_EMPTY))
                                std::wcout << "Name : " << ((cn.vt == VT_NULL) ? "NULL" : "EMPTY") << std::endl;
                            else
                            {
                                //std::wcout << "Name : " << cn.bstrVal << std::endl;
                                std::string procName = _bstr_t(cn.bstrVal);
                                observer->OnProcessEnded(procName);
                            }
                        }
                        VariantClear(&cn);
                    }
                }
                VariantClear(&vtProp);
            }
        }
    }

    return WBEM_S_NO_ERROR;
}

HRESULT ProcessMonitor::EventSink::SetStatus(
    /* [in] */ LONG lFlags,
    /* [in] */ HRESULT hResult,
    /* [in] */ BSTR strParam,
    /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
)
{
    if (lFlags == WBEM_STATUS_COMPLETE)
    {
        printf("Call complete. hResult = 0x%X\n", hResult);
    }
    else if (lFlags == WBEM_STATUS_PROGRESS)
    {
        printf("Call in progress.\n");
    }

    return WBEM_S_NO_ERROR;
}    // end of EventSink.cpp