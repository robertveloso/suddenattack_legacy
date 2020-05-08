#pragma once

#include <algorithm>
using std::min;
using std::max;

#include "max.h"
#include "bmmlib.h"
#include "resource.h"
#include "iparamb2.h"


// config filename and version
#define CFG_FILENAME	"MaxDTXLoader.cfg"
#define CFG_VERSION		1


// plug-in class id
#define DTXLOADER_CLASSID Class_ID( 0x7a8157d7, 0x1e9823bf )


// global functions
TCHAR* GetString( int id, bool secondary=false );
extern ClassDesc* GetDTXLoaderDesc();


// the actual bitmap i/o class definition
class DTXLoader : public BitmapIO
{
public:
	Interface* ip;

	// constructors and destructors
	DTXLoader();
	~DTXLoader();

	// supported extensions
	int ExtCount();
	const TCHAR* Ext( int n );

	// descriptions and messages
	const TCHAR* LongDesc() { return GetString( IDS_LONGDESC ); }
	const TCHAR* ShortDesc() { return GetString( IDS_SHORTDESC ); }
	const TCHAR* AuthorName() { return GetString( IDS_AUTHOR ); }
	const TCHAR* CopyrightMessage() { return GetString( IDS_COPYRIGHT ); }

	// version number of the exporter
	unsigned int Version() { return 100; }

	// display the about box
	void ShowAbout( HWND hWnd ) {}

	// what functionality exists in this format
	int Capability() { return BMMIO_READER | BMMIO_EXTENSION; }

	// configuration info
	DWORD EvaluateConfigure() { return 0; }
	BOOL LoadConfigure( void* ptr ) { return TRUE; }
	BOOL SaveConfigure( void* ptr ) { return TRUE; }

	// basic information about the image (size, aspect, etc.)
	BMMRES GetImageInfo( BitmapInfo* bi );

	// load the image
	BitmapStorage* Load( BitmapInfo* bi, Bitmap* map, BMMRES* status );
};
