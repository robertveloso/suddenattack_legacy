#include "bdefs.h"

#include "bindmgr.h"
#include "winbind.h"
#include "ltmodule.h"



typedef void (*SetInstanceHandleFn)(void *handle);


// --------------------------------------------------------- //
// Main interface functions.
// --------------------------------------------------------- //

int bm_BindModule(const char *pModuleName, bool bTempFile, CBindModuleType *&pModule)
{
	// Check if we already have this module loaded.  If we do
	// then we don't need to do the setmasterdatabase.
	HMODULE hModule = GetModuleHandle( pModuleName );
	bool bModuleAlreadyLoaded = ( hModule != NULL );

	HINSTANCE hInstance = LoadLibrary(pModuleName);
    if (hInstance == NULL) 
	{		
		return BIND_CANTFINDMODULE;
    }

	// If this is the first time the module is getting loaded,
	// then tell it about our master database.
	if( !bModuleAlreadyLoaded )
	{
		//merge our interface database with the database in the DLL we just loaded.
		TSetMasterFn pSetMasterFn = (TSetMasterFn)GetProcAddress(hInstance, "SetMasterDatabase");
		//check if the function existed.
		if (pSetMasterFn != NULL) {
			//merge our database with theirs
			pSetMasterFn(GetMasterDatabase());
		}
	}

	WinBind *pBind;
	LT_MEM_TRACK_ALLOC(( pBind = new WinBind ),LT_MEM_TYPE_MISC);
	pBind->m_hInstance = hInstance;
	pBind->m_Type = BINDTYPE_DLL;
	pBind->m_sTempFileName = bTempFile ? pModuleName : "";

	pModule = (CBindModuleType *)pBind;
	return BIND_NOERROR;
}


void bm_UnbindModule(CBindModuleType *hModule)
{
	WinBind *pBind = (WinBind*)hModule;

	ASSERT(pBind);

	if(pBind->m_Type == BINDTYPE_DLL)
	{
		FreeLibrary(pBind->m_hInstance);
	}

	// Delete the temporary file.
	if( pBind->m_sTempFileName.length( ))
	{
		DeleteFile( pBind->m_sTempFileName.c_str( ));
		pBind->m_sTempFileName = "";
	}

	delete pBind;
	pBind = NULL;
}


LTRESULT bm_SetInstanceHandle(CBindModuleType *hModule)
{
	SetInstanceHandleFn fn;
	WinBind *pBind;


	pBind = (WinBind*)hModule;
	if(!pBind)
		RETURN_ERROR(1, bm_SetInstanceHandle, LT_INVALIDPARAMS);

	fn = (SetInstanceHandleFn)GetProcAddress(pBind->m_hInstance, "SetInstanceHandle");
	if(fn)
	{
		fn((void*)pBind->m_hInstance);
	}

	return LT_OK;	
}


LTRESULT bm_GetInstanceHandle(CBindModuleType *hModule, void **pHandle)
{
	WinBind *pBind;

	pBind = (WinBind*)hModule;
	if(!pBind)
		RETURN_ERROR(1, bm_GetInstanceHandle, LT_INVALIDPARAMS);

	*pHandle = (void*)pBind->m_hInstance;
	return LT_OK;
}


void* bm_GetFunctionPointer(CBindModuleType *hModule, const char *pFunctionName)
{
	WinBind *pBind = (WinBind*)hModule;

	ASSERT(pBind);

	return (void*)GetProcAddress(pBind->m_hInstance, pFunctionName);
}







