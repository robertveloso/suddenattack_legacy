#ifndef WORLDIMPORT_H
#define WORLDIMPORT_H

#pragma once

#pragma warning(disable : 4786 4018 4244)

#include "ltamgr.h"
#include <string>
#include <map>

class WIBrush;
class WIProp;
class WINode;
class WIPoly;
class WIPrefabRef;


// node types
enum WINodeType
{
	eContainer,		// node is a hierarchy placeholder
	eBrush,			// node is a brush
	eObject,		// node is an object
	ePrefabRef		// node is a prefab reference
};

	
class WorldImport
{
public:
	WorldImport();
	~WorldImport();

	bool LoadFile( const char* fileName );

	WIProp* GetProperty( int propNum ) const { assert( propNum < properties.size() ); return properties[propNum]; }
	WIBrush* GetBrush( int brushNum ) const { assert( brushNum < brushes.size() ); return brushes[brushNum]; }
	WIPrefabRef* GetPrefab( int prefabNum ) const { assert( prefabNum < prefabs.size() ); return prefabs[prefabNum]; }
	WINode* GetWorldRoot() { assert( rootNode ); return rootNode; }

	// add a texture name if it's not already there and return its index
	int AddTexture( const std::string& texName );

	// get the number of texture names in the world
	int NumTextures() const { return nextTexIndex; }

	// retrieve the name of a specific texture
	const char* TextureName( int texIndex ) const;

private:
	void Free();
	bool ParseFile( CLTANode& root );
	bool ParseProperties( CLTANode* propList );
	bool ParseBrushes( CLTANode* brushList );
	bool ParseNodes( CLTANode* nodeTree );
	WINode* ParseNodeHierarchy( CLTANode* ltaNode );

	std::vector<WIProp*> properties;					// list of all properties in the world
	std::vector<WIBrush*> brushes;						// list of all brushes in the world
	std::vector<WIPrefabRef*> prefabs;					// list of all prefabs in the world
	WINode* rootNode;									// root of the node hierarchy

	// texture info
	typedef std::map<std::string,int> TexList;
	int nextTexIndex;									// the index of the next texture to be added
	TexList texList;									// list of texture names and their associated indices
	std::vector<TexList::iterator> texListByIndex;		// fast way to retrieve a texture name given an index
};


class WINode
{
public:
	WINode( WINodeType type, int propIndex, int brushIndex, int prefabIndex, std::string& name, std::vector<WINode*>& children );
	~WINode();

	WINodeType Type() const { return type; }
	int PropIndex() const { return propIndex; }
	int BrushIndex() const { assert(brushIndex>=0); return brushIndex; }
	int PrefabIndex() const { assert(prefabIndex>=0); return prefabIndex; }
	const char* Name() const { return name.c_str(); }

	WINode* GetChild( int childNum ) const { assert( childNum < children.size() ); return children[childNum]; }
	int NumChildren() const { return children.size(); }

private:
	WINodeType type;
	int propIndex;
	int brushIndex;
	int prefabIndex;
	std::string name;

	std::vector<WINode*> children;
};


class WIProp
{
public:
	WIProp( const char* name, const LTVector3f& pos = LTVector3f(0,0,0), const LTVector3f& rot = LTVector3f(0,0,0) );
	~WIProp() {};

	const LTVector3f& Pos() const { return pos; }
	const LTVector3f& Rot() const { return rot; }
	const std::string& Name() const { return name; }

private:
	LTVector3f pos;
	LTVector3f rot;
	std::string name;
};


class WIPrefabRef
{
public:
	WIPrefabRef( const std::string& file, const LTVector3f& pos, const LTVector3f& rot );
	~WIPrefabRef() {};

	const LTVector3f& Pos() const { return pos; }
	const LTVector3f& Rot() const { return rot; }
	const std::string& File() const { return file; }

private:
	LTVector3f pos;
	LTVector3f rot;
	std::string file;
};


class WIBrush
{
public:
	WIBrush();
	~WIBrush();

	bool ParseBrush( CLTANode* brushNode, WorldImport* world );

	LTVector3f center;	// center of the brush (average of verts)
	LTVector3f* verts;	// array of verts in world space
	WIPoly* polies;		// array of polygons that make up this brush
	int numVerts;
	int numPolies;

private:
	void Free();
};


class WIPoly
{
public:
	WIPoly();
	~WIPoly();

	bool ParsePoly( CLTANode* polyNode, WorldImport* world );

	int* vertIndices;
	int numIndices;
	LTVector3f normal;
	LTVector3f O, P, Q;
	float dist;
	int textureIndex;

private:
	void Free();
};


// take a list node and extract a vector, returns false on failure (without altering vector)
bool ParseVector( CLTANode* node, LTVector3f& vector );

// take a list node and extract a vector after the name element, returns false on failure (without altering vector)
bool ParseNamedVector( CLTANode* node, LTVector3f& vector );


#endif // WORLDIMPORT_H