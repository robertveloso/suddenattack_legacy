#pragma once

#pragma warning( disable : 4786 )

#include <algorithm>
using std::min;
using std::max;

#include "max.h"
#include "resource.h"
#include "iparamm2.h"
#include "stdmat.h"
#include "LTAFile.h"
#include <list>
#include <map>


// config filename and version
#define CFG_FILENAME	"MAXWorldExport.cfg"
#define CFG_VERSION		7


// number of verts we use a static index array for, any over this switch to dynamic
#define STATIC_VERT_INDEX_SIZE		256


// plug-in class id
#define MAXWORLDEXPORT_CLASSID Class_ID( 0x2f693841, 0x46a13bb5 )


// global functions
TCHAR* GetString( int id, bool secondary=false );
extern ClassDesc* GetMAXWorldExportDesc();


// forward declarations
struct AnimNode;
struct MultiTextureInfo;


// the actual exporter class definition
class MAXWorldExport : public SceneExport
{
public:
	// dialog options
	bool objectSpace;				// true if object pivot points are placed at the DEdit origin
	bool exportAnimations;			// true if animated objects are exported as keyframed worldmodels
	bool asPrefab;					// true if keyframers should be setup for use in a prefab
	bool forceKeys;					// true if animated objects have keyframes forced at even intervals
	bool includeDefinedKeys;		// true if defined keys should be used in addition to forced keys
	bool externalKeyFile;			// true if keys should be placed in an external file (no key objects will be created)
	bool triangulateBrushes;		// true if brushes should be triangulated (basically joined terrain)
	bool exportUVs;					// true if we preserve texture coordinates in the lta
	char projectPath[_MAX_PATH];	// game project path
	int projectPathLen;				// length of the game project path
	char keyFilePath[_MAX_PATH];	// project path relative directory to place external key files into
	int keyFilePathLen;				// length of the key file relative path
	int forceKeysFrames;			// if exporting animated objects, keyframes are forced at this interval

	Interface* ip;

	// constructors and destructors
	MAXWorldExport();
	~MAXWorldExport();

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
	void DialogAccepted( HWND hWnd );

private:
	bool exportSelected;				// true if only selected brushes should be exported
	bool canceled;
	int curNode;						// used for updating the progress bar
	int numNodes;
	FILE* file;
	int numImproperFaces;				// number of faces with improper material assignments

	Interval animRange;					// the current animation range
	std::list<AnimNode*> animNodes;		// list of nodes with keyframes assigned (if exporting keyframed worldmodels)

	// configuration file info
	bool LoadExporterConfig();
	void SaveExporterConfig();
	TSTR GetConfigFilename();

	// initial assessment of the scene prior to actual save
	void PreProcess( INode* node, int& nodeCount, AnimNode* parentAnimNode );

	// process and export a specific node
	bool NodeEnum( INode* node, Box3* bounds, LTAFile& file, bool checkForAnim=true );
	bool ExportNode( INode* node, Box3* bounds, LTAFile& file );
	void ExportNodeAsBrush( Mesh* mesh, Mtl* material, Matrix3& xform, LTAFile& file );
	void ExportNodeAsTriBrush( Mesh* mesh, Mtl* material, Matrix3& xform, LTAFile& file );
	TriObject* GetTriObjectFromNode( INode* node, bool& deleteIt );
	void GenerateOPQ( const Point3& v0, const Point3& v1, const Point3& v2, const Point3& tv0, const Point3& tv1, const Point3& tv2, int width, int height, Point3& o, Point3& p, Point3& q );
	bool GenerateTextureName( TSTR& name );
	TSTR LowercaseFileExtension( const TCHAR* name );
	void ExternalKeyFileName( const TCHAR* name, TSTR& projectRelative, TSTR& fullPath, TSTR& fullPathDir );
	void GetMultiTextureInfo( Mtl* material, int matID, MultiTextureInfo& texInfo );
	bool GetTextureInfo( BitmapTex* tex, int texNum, MultiTextureInfo& texInfo );

	// export the animated brushes
	bool ExportAnimNodes( LTAFile& file, TSTR& keyFilePath );
	bool ExportAnimNode( AnimNode* animNode, LTAFile& file, TSTR& keyFilePath );
	bool ExportKeyFramerGroups( LTAFile& file );
	// export a key for this node
	bool ExportNodeKey( INode* node, TimeValue keyTime, TimeValue timeInterval, const char* keyNote, const Matrix3& offset, const Point3& kfPos, const char* keyName, LTAFile& file );

	// convert a max matrix to a position and euler rotation in lithtech space
	void ConvertMatrix( const Matrix3& source, Point3& position, Point3& eulerRotation );

	// convert a point from max to lithtech space
	Point3 ConvertPoint( const Point3& source ) { return Point3( source.x, source.z, source.y ); }

	// given a max node, find the matching animnode if one exists, otherwise NULL
	AnimNode* FindAnimNode( const INode* node );

	// take a max node and fill in the keyframes that it uses, returns NULL if the node is not animated
	AnimNode* BuildAnimNode( INode* node, const AnimNode* parentAnimNode );

	// find the keys set for this node, returns false if no keys were found for the node (non-recursive check)
	bool BuildNodeKeyTimes( AnimNode* animNode );

	// given a key note string, break it into a command, sound, and sound radius if any exist
	bool ParseKeyNote( const char* keyNote, TSTR& command, TSTR& sound, float& soundRadius );
};


struct AnimNode
{
	INode* objNode;						// the max node at the start of the animated hierarchy
	std::list<TimeValue> keys;			// actual key times on this node
	std::map<TimeValue,TSTR> noteKeys;	// times with associated key strings
};


struct MultiTextureInfo
{
	int numTextures;
	TSTR texName[2];
	int mapWidth[2];
	int mapHeight[2];
	Matrix3 uvXForm[2];
	int mapChannel[2];
};
