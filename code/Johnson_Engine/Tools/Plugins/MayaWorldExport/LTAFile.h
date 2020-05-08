// lta file creation class

#pragma once

#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MString.h>
#include "ltamgr.h"


struct LTANodeList;
struct PropertyList;


class LTAFile
{
public:
	LTAFile( bool detailBrushes, bool externalKeys );
	~LTAFile();

	// output the tree to a stream
	bool Output( const MString& fileName, const MString& keyFileName );

	// going up and down the node hierarchy
	void AddObject( const char* name, bool asBrush );	// adds a new object as a child, and sets the root to that object
	void AscendTree();									// root becomes the parent of the current root

	// add a property to the tree if needed and set the curPropID
	void AddProperty( const char* name );

	// add polygon info to the current polyhedron
	void AddBrushPolygon( int numVerts, int* indices, const MVector& n, const float d, const MVector& o, const MVector& p, const MVector& q, const MString& textureName );

	// add vertex info to the current polyhedron
	void AddBrushVertices( int numVerts, const MPoint* verts );

	// adds a new worldmodel as a child, and sets the root to that object
	void AddWorldModel( const char* name, const MVector& pos );

	// adds a new keyframer as a child, and sets the root to that object
	void AddKeyFramer( const char* name, const char* worldModelName, const char* keyBaseName, const char* keyFile, const MVector& pos );

	// add a new key as a child, and sets the root to that object
	void AddKey( const char* name, float time, const MVector& kfPos, const MVector& pos, const MVector& eulerRotation, const char* sound, float soundRadius, const char* command );

	// add a new group object as a child, and sets the root to that object
	void AddKeyFramerGroup( const char* name, std::vector<MString>& keyFramerNames );

private:
	// index of the next polyhedron to be added
	int curPolyhedron;

	// next node id to be saved out
	int curNodeID;

	// next property id to use (only used for brush export)
	int nextPropID;

	// property id of current brush (might not be nextPropID-1) (only used for brush export)
	int curPropID;

	// brush index (only used for brush export)
	int curBrushID;

	// true if exported brushes should be marked as detail brushes
	bool detailBrushes;

	// places to hook into the node tree
	CLTANode* root;
	CLTANode* header;
	CLTANode* polyhedronList;
	CLTANode* nodeHierarchy;
	CLTANode* globalPropList;
	CLTANode* navigatorPosList;
	CLTANode* nodeRoot;
	CLTANode* polyRoot;
	CLTANode* propRoot;
	CLTANode* curPolyhedronRoot;	// where properties are added into the current polyhedron
	CLTANode* curEditPoly;			// where polygons are added into the current polyhedron
	CLTANode* curPointList;			// where points are added into the current polyhedron

	// external key file node tree
	CLTANode* keyRoot;
	CLTANode* keyList;

	// true if keys should be added to an external file
	bool externalKeys;

	// number of keys that have been added so far
	int numKeys;

	// linked list for the nodehierarchy tree, points to current level nodes are added to
	LTANodeList* nodeList;

	// linked list for the properties in the tree, points to first item in list
	PropertyList* propList;

	ILTAAllocator* nodeAlloc;		// allocator used for generating new nodes

	// add a new key as a child, and sets the root to that object
	void AddKeyWorldFile( const char* name, float time, const MVector& pos, const MVector& eulerRotation, const char* sound, float soundRadius, const char* command );

	// add a key to the external key file
	void AddKeyExternalFile( const char* name, float time, const MVector& pos, const MVector& eulerRotation, const char* sound, float soundRadius, const char* command );

	// shortcuts to steamline things A = Append  N = Node
	inline CLTANode* A( CLTANode* a, CLTANode* b ) { a->AppendElement( b, nodeAlloc ); return a; }
	inline CLTANode* N() { return new CLTANode(); }
	inline CLTANode* N( const char* str ) { CLTANode* newNode = new CLTANode; newNode->SetValue( str, false, nodeAlloc ); return newNode; }
	inline CLTANode* N( const char* str, bool b ) { CLTANode* newNode = new CLTANode; newNode->SetValue( str, b, nodeAlloc ); return newNode; }

	MString VectorString( const MVector& vec ) const;
};


struct LTANodeList
{
	CLTANode* containerNode;		// node to append children to
	LTANodeList* next;				// next node down the tree
	LTANodeList* prev;				// next node up the tree (towards the root)
};


struct PropertyList
{
	MString name;					// name of the property
	int id;							// id of the property
	bool isBrush;					// true if this is a brush property
	PropertyList* next;				// next property in the list
};
