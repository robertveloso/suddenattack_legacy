#include "MaxWorldImport.h"


// class descriptor stuff to let MAX know a little something about this plug-in
class MAXWorldImportClassDesc : public ClassDesc2
{
public:
	int IsPublic()					{ return TRUE; }
	void* Create( BOOL loading )	{ return new MAXWorldImport; }
	const TCHAR* ClassName()		{ return GetString(IDS_CLASSNAME); }
	SClass_ID SuperClassID()		{ return SCENE_IMPORT_CLASS_ID; }
	Class_ID ClassID()				{ return MAXWORLDIMPORT_CLASSID; }
	const TCHAR* Category()			{ return _T(""); }
	HINSTANCE HInstance()			{ return hInstance; }

	// fixed MAXScript name
	const TCHAR* InternalName()		{ return _T("LithTechWorldImport"); }
};

static MAXWorldImportClassDesc MAXWorldImportCD;
ClassDesc* GetMAXWorldImportDesc( void ) { return &MAXWorldImportCD; }


MAXWorldImport::MAXWorldImport()
{
	// dialog defaults
	scale = 1.0f;
	importTexInfo = false;
	importTexTga = true;
	importObjects = false;
	basePath[0] = 0;
	basePathLen = 0;

	ip = NULL;
	ii = NULL;

	multiMat = NULL;
	numFailedTextureLoads = 0;
}

MAXWorldImport::~MAXWorldImport()
{
}

// number of extensions supported by this importer
int MAXWorldImport::ExtCount( void )
{
	return 2;
}

const TCHAR* MAXWorldImport::Ext( int n )
{
	switch( n )
	{
	case 0: return _T("LTA");
	case 1: return _T("LTC");
	}

	return _T("");
}

// description of the importer.  short one is in the extensions drop-down in the import dialog
const TCHAR* MAXWorldImport::LongDesc( void )
{
	return GetString( IDS_LONGDESC );
}

const TCHAR* MAXWorldImport::ShortDesc( void )
{
	return GetString( IDS_SHORTDESC );
}

const TCHAR* MAXWorldImport::AuthorName( void )
{
	return GetString( IDS_AUTHOR );
}

const TCHAR* MAXWorldImport::CopyrightMessage( void )
{
	return GetString( IDS_COPYRIGHT );
}

const TCHAR* MAXWorldImport::OtherMessage1( void )
{
	return _T("");
}

const TCHAR* MAXWorldImport::OtherMessage2( void )
{
	return _T("");
}

// version number (100 == 1.0, 221 == 2.21, etc.)
unsigned int MAXWorldImport::Version( void )
{
	return 100;
}

// about dialog box process
static BOOL CALLBACK AboutDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	MAXWorldImport* import = (MAXWorldImport*)GetWindowLong( hWnd, GWL_USERDATA );

	if( !import && (msg != WM_INITDIALOG) ) return FALSE;

	switch( msg )
	{
	case WM_INITDIALOG:
		import = (MAXWorldImport*)lParam;
		SetWindowLong( hWnd, GWL_USERDATA, lParam );
		CenterWindow( hWnd, GetParent( hWnd ) );
		break;

	case WM_DESTROY:
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_OK:
			EndDialog( hWnd, 1 );
			break;
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

void MAXWorldImport::ShowAbout( HWND hWnd )
{
	DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc, (LPARAM)this );
}

// get full path of config file
TSTR MAXWorldImport::GetConfigFilename( void )
{
	TSTR filename;

	filename += ip->GetDir( APP_PLUGCFG_DIR );
	filename += "\\";
	filename += CFG_FILENAME;

	return filename;
}

// save configuration info to file
void MAXWorldImport::SaveImporterConfig( void )
{
	// open the configuration file for writing
	TSTR filename = GetConfigFilename();
	FILE* cfgStream;

	cfgStream = fopen( filename, "wb" );
	if( !cfgStream )
		return;

	// write version info
	_putw( CFG_VERSION, cfgStream );

	// write configuration
	_putw( importTexInfo ? 1 : 0, cfgStream );
	_putw( importObjects ? 1 : 0, cfgStream );
	_putw( *((int*)&(scale)), cfgStream );
	for( int i = 0; i < _MAX_PATH; i++ )
	{
		fputc( basePath[i], cfgStream );
	}
	_putw( importTexTga ? 1 : 0, cfgStream );

	// close the configuration file
	fclose( cfgStream );
}

// load configuration info from a file (or create it with defaults if it doesn't already exist)
bool MAXWorldImport::LoadImporterConfig( void )
{
	TSTR filename = GetConfigFilename();

	// if the config file doesn't exist, write out the defaults
	if( !DoesFileExist( filename ) )
		SaveImporterConfig();

	FILE* cfgStream = fopen( filename, "rb" );
	if( !cfgStream )
		return false;

	// read the file version
	int version = _getw( cfgStream );

	if( version > CFG_VERSION )
	{
		// unknown version
		fclose( cfgStream );
		return false;
	}

	if( version >= 1 )
	{
		importTexInfo = _getw( cfgStream ) > 0;
		importObjects = _getw( cfgStream ) > 0;
		int tmpScale = _getw( cfgStream );
		scale = *((float*)&tmpScale);
		for( int i = 0; i < _MAX_PATH; i++ )
		{
			basePath[i] = fgetc( cfgStream );
		}
	}

	if( version >= 2 )
	{
		importTexTga = _getw( cfgStream ) > 0;
	}

	fclose( cfgStream );
	return true;
}