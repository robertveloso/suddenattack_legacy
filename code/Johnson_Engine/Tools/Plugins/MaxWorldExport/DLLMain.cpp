// this file contains the exported dll functions

#include "MaxWorldExport.h"

HINSTANCE hInstance;
bool controlsInit = false;

// main dll entry point
BOOL WINAPI DllMain( HINSTANCE hInstDLL, ULONG reason, LPVOID reserved )
{
	hInstance = hInstDLL;

	if( !controlsInit )
	{
		controlsInit = true;
		InitCustomControls( hInstance );
		InitCommonControls();
	}

	return TRUE;
}

// description of the plug-in file that shows up in summary info dialog
__declspec( dllexport ) const TCHAR* LibDescription( void )
{
	return GetString( IDS_LIBDESC );
}

// number of actual plug-in classes implemented in this dll
__declspec( dllexport ) int LibNumberClasses( void )
{
	return 1;
}

// returns pointer to plug-in class descriptor based on index
__declspec( dllexport ) ClassDesc* LibClassDesc( int i )
{
	switch( i )
	{
	case 0: return GetMAXWorldExportDesc();
	default: return NULL;
	}
}

// version of max sdk this dll was compiled against
__declspec( dllexport ) ULONG LibVersion( void )
{
	return VERSION_3DSMAX;
}

// this dll supports deferred plug-in loading (if you add plug-ins, make sure they are deferrable)
__declspec( dllexport ) ULONG CanAutoDefer( void )
{
	return 1;
}

// returns a string from the resources by id
TCHAR* GetString( int id, bool secondary )
{
	static TCHAR buf[256];
	static TCHAR secBuf[256];

	if( hInstance )
	{
		if( !secondary )
			return LoadString( hInstance, id, buf, sizeof(buf) ) ? buf : NULL;
		else
			return LoadString( hInstance, id, secBuf, sizeof(secBuf) ) ? secBuf : NULL;
	}

	return NULL;
}
