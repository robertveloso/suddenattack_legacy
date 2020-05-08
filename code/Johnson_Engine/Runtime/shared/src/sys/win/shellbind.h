

#ifndef __SHELLBIND_H__
#define __SHELLBIND_H__

class CShellModuleType;
class CBindModuleType;

#define SB_NOERROR			-1
#define SB_CANTFINDMODULE	0
#define SB_NOTSHELLMODULE	1
#define SB_VERSIONMISMATCH	2


typedef void* (*CreateShellFn)(void *pInterface);
typedef void  (*DeleteShellFn)(void *pShell);


// Returns a SB_ status. version is set if it returns SB_VERSIONMISMATCH.
int			sb_LoadModule(const char *pModuleName, const char *pShellName, 
				CShellModuleType *&phModule);

void		sb_UnloadModule(CShellModuleType *hModule);

CBindModuleType *sb_GetBindModule(CShellModuleType *hModule);


#endif  // __SHELLBIND_H__

