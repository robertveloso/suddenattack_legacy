#include "MaxModelExport.h"
#include "Exporter.h"
#include "ltautil.h"


// class descriptor stuff to let MAX know a little something about this plug-in
class MAXModelExportClassDesc : public ClassDesc2
{
public:
	int IsPublic()					{ return TRUE; }
	void* Create( BOOL loading )	{ return new MAXModelExport; }
	const TCHAR* ClassName()		{ return GetString(IDS_CLASSNAME); }
	SClass_ID SuperClassID()		{ return SCENE_EXPORT_CLASS_ID; }
	Class_ID ClassID()				{ return MAXMODELEXPORT_CLASSID; }
	const TCHAR* Category()			{ return _T(""); }
	HINSTANCE HInstance()			{ return hInstance; }

	// fixed MAXScript name
	const TCHAR* InternalName()		{ return _T("LTAModelExport"); }
};

static MAXModelExportClassDesc MAXModelExportCD;
ClassDesc* GetMAXModelExportDesc( void ) { return &MAXModelExportCD; }


MAXModelExport::MAXModelExport()
{
	// dialog defaults
	options.animationName = "baseAnim";
	options.appendAnimation = false;
	options.scale = 1.0f;
	options.baseTexPath = "";
	options.useTexInfo = false;
	options.displayPatchWarning = true;
}

MAXModelExport::~MAXModelExport()
{
}

// number of extensions supported by this exporter
int MAXModelExport::ExtCount( void )
{
	return 2;
}

const TCHAR* MAXModelExport::Ext( int n )
{
	switch( n )
	{
	case 0: return _T("LTA");
	case 1: return _T("LTC");
	}

	return _T("");
}

// description of the exporter.  short one is in the extensions drop-down in the save dialog
const TCHAR* MAXModelExport::LongDesc( void )
{
	return GetString( IDS_LONGDESC );
}

const TCHAR* MAXModelExport::ShortDesc( void )
{
	return GetString( IDS_SHORTDESC );
}

const TCHAR* MAXModelExport::AuthorName( void )
{
	return GetString( IDS_AUTHOR );
}

const TCHAR* MAXModelExport::CopyrightMessage( void )
{
	return GetString( IDS_COPYRIGHT );
}

const TCHAR* MAXModelExport::OtherMessage1( void )
{
	return _T("");
}

const TCHAR* MAXModelExport::OtherMessage2( void )
{
	return _T("");
}

// version number (100 == 1.0, 221 == 2.21, etc.)
unsigned int MAXModelExport::Version( void )
{
	return 100;
}

// about dialog box process
static BOOL CALLBACK AboutDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	MAXModelExport* export = (MAXModelExport*)GetWindowLong( hWnd, GWL_USERDATA );

	if( !export && (msg != WM_INITDIALOG) ) return FALSE;

	switch( msg )
	{
	case WM_INITDIALOG:
		export = (MAXModelExport*)lParam;
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

void MAXModelExport::ShowAbout( HWND hWnd )
{
	DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc, (LPARAM)this );
}

// get full path of config file
TSTR MAXModelExport::GetConfigFilename( void )
{
	TSTR filename;

	filename += ip->GetDir( APP_PLUGCFG_DIR );
	filename += "\\";
	filename += CFG_FILENAME;

	return filename;
}

// save configuration info to file
void MAXModelExport::SaveExporterConfig( void )
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
	_putw( options.appendAnimation ? 1 : 0, cfgStream );
	_putw( *((int*)&(options.scale)), cfgStream );
	_putw( options.animationName.Length(), cfgStream );
	for( int i = 0; i < options.animationName.Length(); i++ )
	{
		fputc( options.animationName[i], cfgStream );
	}
	_putw( options.useTexInfo ? 1 : 0, cfgStream );
	_putw( options.baseTexPath.Length(), cfgStream );
	for( i = 0; i < options.baseTexPath.Length(); i++ )
	{
		fputc( options.baseTexPath[i], cfgStream );
	}
	_putw( options.displayPatchWarning ? 1 : 0, cfgStream );

	// close the configuration file
	fclose( cfgStream );
}

// load configuration info from a file (or create it with defaults if it doesn't already exist)
bool MAXModelExport::LoadExporterConfig( void )
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

	if( version >= 1 )
	{
		options.appendAnimation = _getw( cfgStream ) > 0;
		int tmpScale = _getw( cfgStream );
		options.scale = *((float*)&tmpScale);
		int nameLength = _getw( cfgStream );
		options.animationName.Resize( nameLength );
		for( int i = 0; i < nameLength; i++ )
		{
			options.animationName[i] = fgetc( cfgStream );
		}
	}

	if( version >= 2 )
	{
		options.useTexInfo = _getw( cfgStream ) > 0;
		int pathLength = _getw( cfgStream );
		options.baseTexPath.Resize( pathLength );
		for( int i = 0; i < pathLength; i++ )
		{
			options.baseTexPath[i] = fgetc( cfgStream );
		}
	}

	if( version >= 3 )
	{
		options.displayPatchWarning = _getw( cfgStream ) > 0;
	}

	fclose( cfgStream );
	return true;
}

// determine supported exporter options
BOOL MAXModelExport::SupportsOptions( int ext, DWORD options )
{
	switch( ext )
	{
	case 0:		// .lta
	case 1:		// .ltc
		if( options == SCENE_EXPORT_SELECTED )
			return TRUE;
		else
			return FALSE;
		break;
	}

	return FALSE;
}

// configuration dialog process
static BOOL CALLBACK ConfigDialogProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	MAXModelExport* export = (MAXModelExport*)GetWindowLong( hWnd, GWL_USERDATA );
	if( !export && (msg != WM_INITDIALOG) )
		return FALSE;

	switch( msg )
	{
	case WM_INITDIALOG:
		export = (MAXModelExport*)lParam;
		// setup dialog controls to match settings
		SetCheckBox( hWnd, IDC_APPEND, export->options.appendAnimation );
		SetWindowText( GetDlgItem( hWnd, IDC_ANIM_NAME ), export->options.animationName );
		char tmpStr[256];
		sprintf( tmpStr, "%.3f", export->options.scale );
		SetWindowText( GetDlgItem( hWnd, IDC_SCALE ), tmpStr );
		SetCheckBox( hWnd, IDC_USETEXINFO, export->options.useTexInfo );
		SetWindowText( GetDlgItem( hWnd, IDC_BASETEXPATH ), export->options.baseTexPath );
		EnableWindow( GetDlgItem( hWnd, IDC_BASETEXPATH ), export->options.useTexInfo );
		EnableWindow( GetDlgItem( hWnd, IDC_BROWSE_BASETEXPATH ), export->options.useTexInfo );
		EnableWindow( GetDlgItem( hWnd, IDC_STATIC_BASETEXPATH ), export->options.useTexInfo );
		// put a pointer to the exporter class instance in the user data
		SetWindowLong( hWnd, GWL_USERDATA, lParam );
		CenterWindow( hWnd, GetParent( hWnd ) );
		break;

	case WM_DESTROY:
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_CANCEL:
			EndDialog( hWnd, 1 );
			break;

		case IDC_OK:
			if( export->DialogAccepted( hWnd ) )
				EndDialog( hWnd, 0 );
			break;

		case IDC_USETEXINFO:
		{
			bool on = GetCheckBox( hWnd, IDC_USETEXINFO ) > 0;
			EnableWindow( GetDlgItem( hWnd, IDC_BASETEXPATH ), on );
			EnableWindow( GetDlgItem( hWnd, IDC_BROWSE_BASETEXPATH ), on );
			EnableWindow( GetDlgItem( hWnd, IDC_STATIC_BASETEXPATH ), on );
			break;
		}

		case IDC_BROWSE_BASETEXPATH:
		{
			TCHAR dir[_MAX_PATH];
			GetWindowText( GetDlgItem( hWnd, IDC_BASETEXPATH ), dir, _MAX_PATH );
			export->ip->ChooseDirectory( hWnd, GetString(IDS_CHOOSE_DIRECTORY), dir );
			if( strlen( dir ) > 0 )
				SetWindowText( GetDlgItem( hWnd, IDC_BASETEXPATH ), dir );
			break;
		}
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

// progress bar dummy function
DWORD WINAPI ProgressBarUpdate( LPVOID arg )
{
	return 0;
}

// fill in settings based on export dialog
bool MAXModelExport::DialogAccepted( HWND hWnd )
{
	char tmpStr[256];
	GetWindowText( GetDlgItem( hWnd, IDC_SCALE ), tmpStr, 255 );
	float tmpScale = (float)atof( tmpStr );
	if( tmpScale > 0.0f )
	{
		options.scale = tmpScale;
	}
	else
	{
		TSTR title = GetString( IDS_EXPORTERROR_TITLE );
		TSTR badScale = GetString( IDS_EXPORTERROR_BADSCALE );
		MessageBox( hWnd, badScale, title, MB_OK | MB_ICONERROR );
		return false;
	}

	options.appendAnimation = GetCheckBox( hWnd, IDC_APPEND ) > 0;

	GetWindowText( GetDlgItem( hWnd, IDC_ANIM_NAME ), tmpStr, 255 );
	options.animationName = tmpStr;

	options.useTexInfo = GetCheckBox( hWnd, IDC_USETEXINFO ) > 0;
	char dir[_MAX_PATH];
	GetWindowText( GetDlgItem( hWnd, IDC_BASETEXPATH ), dir, _MAX_PATH );
	int dirLen = strlen( dir );

	// make sure path ends in a backslash
	if( dirLen && dir[dirLen-1] != '\\' )
	{
		dir[dirLen] = '\\';
		dir[dirLen+1] = 0;
	}

	// make path lowercase
	for( int i = 0; i < dirLen; i++ )
	{
		if( isupper( dir[i] ) )
			dir[i] = tolower( dir[i] );
	}

	options.baseTexPath = dir;

	return true;
}

// export to a file
int MAXModelExport::DoExport( const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options )
{
	this->ip = ip;

	// try to load the configuration info
	LoadExporterConfig();

	if( !suppressPrompts )
	{
		// warn the user about requiring patched character studio 3
		if( this->options.displayPatchWarning )
		{
			DWORD dontShowAgain;
#ifndef USES_MAX3_SDK
			MaxMsgBox( ip->GetMAXHWnd(), GetString(IDS_MESSAGE_PATCHWARNING4), GetString(IDS_MESSAGE_TITLE,true), MB_OK | MB_ICONINFORMATION, MAX_MB_DONTSHOWAGAIN, &dontShowAgain );
#else
			MaxMsgBox( ip->GetMAXHWnd(), GetString(IDS_MESSAGE_PATCHWARNING3), GetString(IDS_MESSAGE_TITLE,true), MB_OK | MB_ICONINFORMATION, MAX_MB_DONTSHOWAGAIN, &dontShowAgain );
#endif
			if( dontShowAgain & MAX_MB_DONTSHOWAGAIN )
				this->options.displayPatchWarning = false;
		}

		// show the configuration dialog
		int res = DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_EXPORT), ip->GetMAXHWnd(), ConfigDialogProc, (LPARAM)this );

		// they canceled
		if( res != 0 ) return 1;
	}

	// can't append to a compressed file
	if( this->options.appendAnimation && CLTAUtil::IsFileCompressed( name ) )
	{
		MessageBox( ip->GetMAXHWnd(), GetString(IDS_MESSAGE_APPENDCOMPRESSED), GetString(IDS_MESSAGE_TITLE,true), MB_OK | MB_ICONERROR );
		return 1;
	}

	// only exporting selected nodes?
	bool exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	// start the progress bar
	ip->ProgressStart( GetString(IDS_PROGRESSBAR), TRUE, ProgressBarUpdate, NULL );
	ip->ProgressUpdate( 0 );

	// export the model
	ModelExporter exporter( &(this->options), ip, this, exportSelected );
	if( exporter.Export( name ) )
	{
		// export was successful
		ip->ProgressUpdate( 99, FALSE, GetString(IDS_PROGRESS_SUCCESS) );
		MessageBox( ip->GetMAXHWnd(), GetString(IDS_MESSAGE_SUCCESS), GetString(IDS_MESSAGE_TITLE,true), MB_OK );
	}
	else
	{
		// export didn't complete correctly
		ip->ProgressUpdate( 99, FALSE, GetString(IDS_PROGRESS_ERROR) );
		MessageBox( ip->GetMAXHWnd(), GetString(IDS_MESSAGE_ERROR), GetString(IDS_MESSAGE_TITLE,true), MB_OK|MB_ICONERROR );
	}

	// close the progress bar
	ip->ProgressEnd();

	// save the config info
	SaveExporterConfig();

	return 1;
}
