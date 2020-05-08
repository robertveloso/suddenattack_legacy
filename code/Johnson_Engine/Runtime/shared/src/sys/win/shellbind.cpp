#include "bdefs.h"

#include "shellbind.h"
#include "bindmgr.h"

class CShellModuleType {
    //no members, this is just a name.
};

class ShellModule : public CShellModuleType
{
public:
    CBindModuleType *m_hModule;
};

typedef int (*GetShellVersionFn)();
typedef void (*GetShellFunctionsFn)(CreateShellFn *pCreate, DeleteShellFn *pDelete);


int sb_LoadModule(const char *pModuleName, CShellModuleType *&phModule) {
    char functionName[100];
    CBindModuleType *hModule;
    ShellModule *pModule;
    int status;

    
    status = bm_BindModule(pModuleName, hModule);
    if (status == BIND_CANTFINDMODULE)
        return SB_CANTFINDMODULE;

//    bm_SetInstanceHandle(hModule);

//    // Get the create and delete function pointers.
//    sprintf(functionName, "Get%sFunctions", pShellName);
//    getFunctionsFn = (GetShellFunctionsFn)bm_GetFunctionPointer(hModule, functionName);
//    if (!getFunctionsFn)
//    {
//        bm_UnbindModule(hModule);
//        return SB_NOTSHELLMODULE;
//    }

//    // Check the version.
//    sprintf(functionName, "Get%sVersion", pShellName);
//    getVersionFn = (GetShellVersionFn)bm_GetFunctionPointer(hModule, functionName);
//    if (!getVersionFn)
//    {
//        bm_UnbindModule(hModule);
//        return SB_NOTSHELLMODULE;
//    }
//
//    *actualVersion = getVersionFn();
//    if (*actualVersion != neededVersion)
//    {
//        bm_UnbindModule(hModule);
//        return SB_VERSIONMISMATCH;
//    }
//
//    getFunctionsFn(&createFn, &deleteFn);
    
    LT_MEM_TRACK_ALLOC(pModule = (ShellModule*)LTMemFree(sizeof(ShellModule)),LT_MEM_TYPE_MISC);
    pModule->m_hModule = hModule;

    phModule = pModule;

    return SB_NOERROR;
}


void sb_UnloadModule(CShellModuleType *hModule)
{
    ShellModule *pModule;

    pModule = (ShellModule*)hModule;
    bm_UnbindModule(pModule->m_hModule);
    LTMemFree(pModule);
}


CBindModuleType *sb_GetBindModule(CShellModuleType *hModule)
{
    return ((ShellModule*)hModule)->m_hModule;
}



