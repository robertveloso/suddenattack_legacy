#pragma once

#include <algorithm>
using std::min;
using std::max;

#include "max.h"
#include "resource.h"
#include "iparamm2.h"
#include "WorldImport.h"


// config filename and version
#define CFG_FILENAME	"MAXWorldImport.cfg"
#define CFG_VERSION		2


// plug-in class id
#define MAXWORLDIMPORT_CLASSID Class_ID( 0x713a5494, 0x357f2809 )


// size of the dummy object created for imported objects (adjusted by scale option)
#define IMPORTED_OBJECT_SIZE	20.0f


// global functions
TCHAR* GetString( int id );
extern ClassDesc* GetMAXWorldImportDesc();


// forward declarations
class MultiMtl;
class StdMat;
class BitmapTex;


// the actual importer class definition
class MAXWorldImport : public SceneImport
{
public:
	// dialog options
	float scale;				// amount to scale imported scene by
	bool importTexInfo;			// true if we should attempt to import texture information
	bool importTexTga;			// true if imported textures are created using .tga extension, otherwise .dtx
	bool importObjects;			// true if objects should be imported as dummy objects
	char basePath[_MAX_PATH];	// base path of the texture directory
	int basePathLen;

	Interface* ip;

	// constructors and destructors
	MAXWorldImport();
	~MAXWorldImport();

	// supported extensions
	int ExtCount();
	const TCHAR* Ext( int n );

	// descriptions and messages
	const TCHAR* LongDesc();
	const TCHAR* ShortDesc();
	const TCHAR* AuthorName();
	const TCHAR* CopyrightMessage();
	const TCHAR* OtherMessage1();
	const TCHAR* OtherMessage2();

	// version number of the importer
	unsigned int Version();

	// display the about box
	void ShowAbout( HWND hWnd );

	// do the actual import
	int DoImport( const TCHAR* name, ImpInterface* ii, Interface* ip, BOOL suppressPrompts=FALSE );

	// called when the user clicks on OK in the configuration dialog
	void DialogAccepted( HWND hWnd );

private:
	WorldImport world;				// the generic world loader
	ImpInterface* ii;				// max import interface

	MultiMtl* multiMat;				// the multi-material containing all the used textures
	std::vector<StdMat*> matList;	// array containing all the used textures
	typedef std::pair<float,float> floatPair;
	std::vector<floatPair> texDims;	// dimensions of the textures
	int numFailedTextureLoads;		// number of texture files that were not able to be loaded

	bool LoadTextures();
	void LoadTexture( const TSTR name, StdMat* mat, BitmapTex* tex, floatPair& dims );
	void GenerateTextureName( TSTR& name, TSTR& simpleName );
	bool BuildHierarchy();
	INode* AddNode( WINode* node );
	INode* AddObject( WINode* node );
	INode* AddBrush( WINode* node );

	// returns true if the child should be created given the current import options
	bool IsValidChild( WINode* node );

	// configuration file info
	bool LoadImporterConfig();
	void SaveImporterConfig();
	TSTR GetConfigFilename();
};
