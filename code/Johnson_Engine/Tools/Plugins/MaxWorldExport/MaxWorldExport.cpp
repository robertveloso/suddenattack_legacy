#include "MaxWorldExport.h"


// class descriptor stuff to let MAX know a little something about this plug-in
class MAXWorldExportClassDesc : public ClassDesc2
{
public:
	int IsPublic()					{ return TRUE; }
	void* Create( BOOL loading )	{ return new MAXWorldExport; }
	const TCHAR* ClassName()		{ return GetString(IDS_CLASSNAME); }
	SClass_ID SuperClassID()		{ return SCENE_EXPORT_CLASS_ID; }
	Class_ID ClassID()				{ return MAXWORLDEXPORT_CLASSID; }
	const TCHAR* Category()			{ return _T(""); }
	HINSTANCE HInstance()			{ return hInstance; }

	// fixed MAXScript name
	const TCHAR* InternalName()		{ return _T("LTATerrainExport"); }
};

static MAXWorldExportClassDesc MAXWorldExportCD;
ClassDesc* GetMAXWorldExportDesc( void ) { return &MAXWorldExportCD; }


MAXWorldExport::MAXWorldExport()
{
	// dialog defaults
	objectSpace = false;
	exportAnimations = false;
	asPrefab = false;
	forceKeys = true;
	forceKeysFrames = 5;
	includeDefinedKeys = false;
	triangulateBrushes = false;
	exportUVs = true;
	projectPath[0] = 0;
	projectPathLen = 0;
	keyFilePath[0] = 0;
	keyFilePathLen = 0;

	canceled = false;
	numImproperFaces = 0;
}

MAXWorldExport::~MAXWorldExport()
{
	// delete the animnodes
	for( std::list<AnimNode*>::iterator it = animNodes.begin(); it != animNodes.end(); it++ )
	{
		AnimNode* curAnimNode = *it;
		assert( curAnimNode );
		delete curAnimNode;
	}

}

// number of extensions supported by this exporter
int MAXWorldExport::ExtCount( void )
{
	return 2;
}

const TCHAR* MAXWorldExport::Ext( int n )
{
	switch( n )
	{
	case 0: return _T("LTA");
	case 1: return _T("LTC");
	}

	return _T("");
}

// description of the exporter.  short one is in the extensions drop-down in the save dialog
const TCHAR* MAXWorldExport::LongDesc( void )
{
	return GetString( IDS_LONGDESC );
}

const TCHAR* MAXWorldExport::ShortDesc( void )
{
	return GetString( IDS_SHORTDESC );
}

const TCHAR* MAXWorldExport::AuthorName( void )
{
	return GetString( IDS_AUTHOR );
}

const TCHAR* MAXWorldExport::CopyrightMessage( void )
{
	return GetString( IDS_COPYRIGHT );
}

const TCHAR* MAXWorldExport::OtherMessage1( void )
{
	return _T("");
}

const TCHAR* MAXWorldExport::OtherMessage2( void )
{
	return _T("");
}

// version number (100 == 1.0, 221 == 2.21, etc.)
unsigned int MAXWorldExport::Version( void )
{
	return 100;
}

// about dialog box process
static BOOL CALLBACK AboutDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	MAXWorldExport* export = (MAXWorldExport*)GetWindowLong( hWnd, GWL_USERDATA );

	if( !export && (msg != WM_INITDIALOG) ) return FALSE;

	switch( msg )
	{
	case WM_INITDIALOG:
		export = (MAXWorldExport*)lParam;
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

void MAXWorldExport::ShowAbout( HWND hWnd )
{
	DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc, (LPARAM)this );
}

// get full path of config file
TSTR MAXWorldExport::GetConfigFilename( void )
{
	TSTR filename;

	filename += ip->GetDir( APP_PLUGCFG_DIR );
	filename += "\\";
	filename += CFG_FILENAME;

	return filename;
}

// save configuration info to file
void MAXWorldExport::SaveExporterConfig( void )
{
	// open the configuration file for writing
	TSTR filename = GetConfigFilename();
	FILE* cfgStream;
	int i;

	cfgStream = fopen( filename, "wb" );
	if( !cfgStream )
		return;

	// write version info
	_putw( CFG_VERSION, cfgStream );

	// write configuration
	_putw( exportUVs ? 1 : 0, cfgStream );
	_putw( objectSpace ? 1 : 0, cfgStream );
	_putw( 1, cfgStream );						// used to be asBrushes
	for( i = 0; i < _MAX_PATH; i++ )
	{
		fputc( projectPath[i], cfgStream );
	}
	_putw( exportAnimations ? 1 : 0, cfgStream );
	_putw( triangulateBrushes ? 1 : 0, cfgStream );
	_putw( forceKeys ? 1 : 0, cfgStream );
	_putw( forceKeysFrames, cfgStream );
	_putw( includeDefinedKeys ? 1 : 0, cfgStream );
	_putw( externalKeyFile ? 1 : 0, cfgStream );
	for( i = 0; i < _MAX_PATH; i++ )
	{
		fputc( keyFilePath[i], cfgStream );
	}
	_putw( asPrefab ? 1 : 0, cfgStream );

	// close the configuration file
	fclose( cfgStream );
}

// load configuration info from a file (or create it with defaults if it doesn't already exist)
bool MAXWorldExport::LoadExporterConfig( void )
{
	TSTR filename = GetConfigFilename();

	// if the config file doesn't exist, write out the defaults
	if( !DoesFileExist( filename ) )
		SaveExporterConfig();

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

	if( version == 1 )
	{
		exportUVs = _getw( cfgStream ) > 0;
		objectSpace = _getw( cfgStream ) > 0;
		_getw( cfgStream );						// used to be asBrushes
		fscanf( cfgStream, "%s", projectPath );
	}
	else if( version >= 2 )
	{
		int i;

		// version 2 settings
		exportUVs = _getw( cfgStream ) > 0;
		objectSpace = _getw( cfgStream ) > 0;
		_getw( cfgStream );						// used to be asBrushes
		for( i = 0; i < _MAX_PATH; i++ )
		{
			projectPath[i] = fgetc( cfgStream );
		}

		if( version >= 3 )
		{
			exportAnimations = _getw( cfgStream ) > 0;
		}

		if( version >= 4 )
		{
			triangulateBrushes = _getw( cfgStream ) > 0;
		}

		if( version >= 5 )
		{
			forceKeys = _getw( cfgStream ) > 0;
			forceKeysFrames = _getw( cfgStream );
			if( forceKeysFrames <= 0 )
			{
				assert( 0 );
				forceKeysFrames = 5;
			}
			includeDefinedKeys = _getw( cfgStream ) > 0;
		}

		if( version >= 6 )
		{
			externalKeyFile = _getw( cfgStream ) > 0;
			for( i = 0; i < _MAX_PATH; i++ )
			{
				keyFilePath[i] = fgetc( cfgStream );
			}
		}

		if( version >= 7 )
		{
			asPrefab = _getw( cfgStream ) > 0;
		}
	}

	fclose( cfgStream );
	return true;
}