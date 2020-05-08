#ifndef MAXMODELEXPORT_H
#define MAXMODELEXPORT_H
#pragma once

#include <algorithm>
using std::min;
using std::max;

#include "max.h"
#include "resource.h"
#include "iparamm2.h"


// config filename and version
#define CFG_FILENAME	"MAXModelExport.cfg"
#define CFG_VERSION		3


// plug-in class id
#define MAXMODELEXPORT_CLASSID Class_ID( 0x431ff5ac, 0x2ad7d3c2 )


// global functions
TCHAR* GetString( int id, bool secondary=false );
extern ClassDesc* GetMAXModelExportDesc();


// user configurable options in the exporter
struct ExporterOptions
{
	bool appendAnimation;		// true if we are appending an animation to a model file
	TSTR animationName;			// name of the animation being exported
	float scale;				// scale factor for the exported model
	bool useTexInfo;			// true if max texture map paths should be stored
	TSTR baseTexPath;			// portion of path to strip off of texture paths
	bool displayPatchWarning;	// true if we should display the warning about requiring the CS patch
};


// the actual exporter class definition
class MAXModelExport : public SceneExport
{
public:
	ExporterOptions options;	// user configurable exporter options
	Interface* ip;

	// constructors and destructors
	MAXModelExport();
	~MAXModelExport();

	// supported extensions
	int ExtCount();
	const TCHAR* Ext( int n );

	// what options do we support for a given extension
	BOOL SupportsOptions( int ext, DWORD options );

	// descriptions and messages
	const TCHAR* LongDesc();
	const TCHAR* ShortDesc();
	const TCHAR* AuthorName();
	const TCHAR* CopyrightMessage();
	const TCHAR* OtherMessage1();
	const TCHAR* OtherMessage2();

	// version number of the exporter
	unsigned int Version();

	// display the about box
	void ShowAbout( HWND hWnd );

	// do the actual export
	int DoExport( const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts=FALSE, DWORD options=0 );

	// called when the user clicks on OK in the configuration dialog
	bool DialogAccepted( HWND hWnd );

private:
	// configuration file info
	bool LoadExporterConfig();
	void SaveExporterConfig();
	TSTR GetConfigFilename();
};


#endif // MAXMODELEXPORT_H
