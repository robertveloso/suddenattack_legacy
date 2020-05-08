#include "WorldImport.h"


//-------------------
//--- WorldImport ---
//-------------------

WorldImport::WorldImport() :
	rootNode(NULL), nextTexIndex(0)
{
}


WorldImport::~WorldImport()
{
	// clear out world data
	Free();
}


void WorldImport::Free( void )
{
	unsigned i;

	delete rootNode;

	// clean up the property list
	for( i = 0; i < properties.size(); i++ )
	{
		delete properties[i];
	}
	properties.clear();

	// clean up the brush list
	for( i = 0; i < brushes.size(); i++ )
	{
		delete brushes[i];
	}
	brushes.clear();

	// clean up the prefab list
	for( i = 0; i < prefabs.size(); i++ )
	{
		delete prefabs[i];
	}
	prefabs.clear();

	// clean up the texture list
	nextTexIndex = 0;
	texList.clear();
	texListByIndex.clear();

	rootNode = NULL;
}


// add a texture name if it's not already there and return its index
int WorldImport::AddTexture( const std::string& texName )
{
	std::pair<TexList::iterator,bool> insertSuccess;
	insertSuccess = texList.insert( std::make_pair( texName, nextTexIndex ) );

	// the texture name was added, so increase the index and update the mapping
	if( insertSuccess.second )
	{
		nextTexIndex++;
		texListByIndex.push_back( insertSuccess.first );
	}

	// get the index out of the map
	return (*(insertSuccess.first)).second;
}


// retrieve the name of a specific texture
const char* WorldImport::TextureName( int texIndex ) const
{
	assert( texIndex < nextTexIndex );
	return (*(texListByIndex[texIndex])).first.c_str();
}


bool WorldImport::LoadFile( const char* fileName )
{
	CLTALoadOnlyAlloc nodeAllocator( 1024 * 1024 );
	CLTANode root;
	bool status = true;

	// clear out any existing world data
	Free();

	if( strlen( fileName ) <= 0 )
		status = false;

	// create a node list from the lta file
	if( status && !CLTANodeReader::LoadEntireFile( fileName, CLTAUtil::IsFileCompressed( fileName ), &root, &nodeAllocator ) )
		status = false;

	// parse the node list into world data
	if( status && !ParseFile( root ) )
		status = false;

	// load failed, so free anything already created
	if( !status )
		Free();

	root.Free( &nodeAllocator );

	return status;
}


bool WorldImport::ParseFile( CLTANode& root )
{
	CLTANode* world = CLTAUtil::ShallowFindList( &root, "world" );

	if( !world )
		return true;

	// grab the three lists we care about from the lta
	CLTANode* brushList = CLTAUtil::ShallowFindList( world, "polyhedronlist" );
	CLTANode* nodeTree = CLTAUtil::ShallowFindList( world, "nodehierarchy" );
	CLTANode* propList = CLTAUtil::ShallowFindList( world, "globalproplist" );

	if( propList && propList->GetNumElements() > 1 )
		propList = propList->GetElement( 1 );
	else
		propList = NULL;

	if( brushList && brushList->GetNumElements() > 1 )
		brushList = brushList->GetElement( 1 );
	else
		brushList = NULL;

	// make sure we have the lists we want before proceeding
	if( !(brushList && nodeTree && propList) || !brushList->IsList() || !propList->IsList() )
		return true;

	if( !ParseProperties( propList ) )
		return false;

	if( !ParseBrushes( brushList ) )
		return false;

	if( !ParseNodes( nodeTree ) )
		return false;

	return true;
}


bool WorldImport::ParseProperties( CLTANode* propList )
{
	assert( propList && propList->IsList() );

	// grab the property node pointers
	std::vector<CLTANode*> props;
	int numProps = CLTAUtil::ShallowFindAll( props, propList, "proplist" );

	if( numProps <= 0 )
		return true;

	properties.reserve( numProps );

	// grab each property and add it to the array
	for( int i = 0; i < numProps; i++ )
	{
		LTVector3f pos( 0.0f, 0.0f, 0.0f );
		LTVector3f rot( 0.0f, 0.0f, 0.0f );
		char name[256] = "";

		if( props[i]->GetNumElements() > 1 )
		{
			// get the actual list of properties
			CLTANode* curProp = props[i]->GetElement( 1 );

			int numItems = curProp->GetNumElements();
			bool foundName = false;
			bool foundPos = false;
			bool foundRot = false;

			// look for the three props we actually care about
			for( int curItem = 0; !(foundName && foundPos && foundRot) && (curItem < numItems); curItem++ )
			{
				// get the current property
				CLTANode* item = curProp->GetElement( curItem );
				if( !item->IsList() )
					continue;

				// make sure there are enough items in the property
				if( item->GetNumElements() < 4 )
					continue;

				// we're looking for the 2nd element, which should be a string
				CLTANode* itemType = item->GetElement( 1 );
				if( !itemType->IsAtom() )
					continue;

				// skip if the property data can't possibly be what we need
				CLTANode* propData = item->GetElement( 3 );
				if( !propData->IsList() || (propData->GetNumElements() < 2) )
					continue;
				propData = propData->GetElement( 1 );
				
				// see if the property name matches what we're looking for
				const char* type = itemType->GetValue();

				if( 0 == strcmp( type, "Name" ) )
				{
					// found the name property
					if( propData->IsAtom() )
					{
						strcpy( name, propData->GetValue() );
						foundName = true;
					}
				}
				else if( 0 == strcmp( type, "Pos" ) )
				{
					// found the position property
					if( !propData->IsList() || (propData->GetNumElements() < 2) )
						continue;
					propData = propData->GetElement( 1 );

					foundPos = ParseVector( propData, pos );
				}
				else if( 0 == strcmp( type, "Rotation" ) )
				{
					// found the rotation property
					if( !propData->IsList() || (propData->GetNumElements() < 2) )
						continue;
					propData = propData->GetElement( 1 );

					foundRot = ParseVector( propData, rot );
				}
			}
		}

		WIProp* newProp = new WIProp( name, pos, rot );
		properties.push_back( newProp );
	}

	return true;
}


bool WorldImport::ParseBrushes( CLTANode* brushList )
{
	assert( brushList && brushList->IsList() );

	// grab the brush node pointers
	std::vector<CLTANode*> brushNodes;
	int numBrushes = CLTAUtil::ShallowFindAll( brushNodes, brushList, "polyhedron" );

	if( numBrushes <= 0 )
		return true;

	brushes.reserve( numBrushes );

	// grab each brush and add it to the array
	for( int i = 0; i < numBrushes; i++ )
	{
		// create and parse the new brush
		WIBrush* brush = new WIBrush();
		if( !brush->ParseBrush( brushNodes[i], this ) )
		{
			delete brush;
			return false;
		}

		// add the new brush to the list
		brushes.push_back( brush );
	}

	return true;
}


bool WorldImport::ParseNodes( CLTANode* nodeTree )
{
	assert( nodeTree && nodeTree->IsList() );

	// get the first node in the tree
	CLTANode* worldNode = CLTAUtil::ShallowFindList( nodeTree, "worldnode" );
	if( !worldNode || !worldNode->IsList() )
		return false;

	// descend the tree, parsing as we go
	rootNode = ParseNodeHierarchy( worldNode );
	if( !rootNode )
		return false;

	return true;
}


WINode* WorldImport::ParseNodeHierarchy( CLTANode* ltaNode )
{
	// grab the nodes we care about
	CLTANode* typeNode = CLTAUtil::ShallowFindList( ltaNode, "type" );
	CLTANode* nameNode = CLTAUtil::ShallowFindList( ltaNode, "label" );
	CLTANode* propertiesNode = CLTAUtil::ShallowFindList( ltaNode, "properties" );
	CLTANode* childListNode = CLTAUtil::ShallowFindList( ltaNode, "childlist" );
	CLTANode* brushIndexNode = CLTAUtil::ShallowFindList( ltaNode, "brushindex" );

	// get the type of the node
	WINodeType type = eContainer;

	if( !typeNode || typeNode->GetNumElements() < 2 )
		return NULL;

	typeNode = typeNode->GetElement( 1 );
	if( !typeNode->IsAtom() )
		return NULL;

	std::string nodeType = GetString( typeNode );

	if( 0 == nodeType.compare( "null" ) )
		type = eContainer;
	else if( 0 == nodeType.compare( "brush" ) )
		type = eBrush;
	else if( 0 == nodeType.compare( "object" ) )
		type = eObject;
	else if( 0 == nodeType.compare( "prefabref" ) )
		type = ePrefabRef;

	// get the name of the node
	std::string nodeName( "" );

	if( nameNode && nameNode->GetNumElements() >= 2 )
	{
		nameNode = nameNode->GetElement( 1 );
		if( !nameNode->IsAtom() )
			return NULL;

		nodeName = GetString( nameNode );
	}

	// get the property index for this node
	CLTANode* propIndexNode = CLTAUtil::ShallowFindList( propertiesNode, "propid" );

	if( !propIndexNode || propIndexNode->GetNumElements() < 2 )
		return NULL;

	propIndexNode = propIndexNode->GetElement( 1 );
	if( !propIndexNode->IsAtom() )
		return NULL;

	int propIndex = GetInt32( propIndexNode );

	// get the brush index for this node if it's a brush
	int brushIndex = -1;

	if( brushIndexNode && brushIndexNode->GetNumElements() >= 2 )
	{
		brushIndexNode = brushIndexNode->GetElement( 1 );
		if( !brushIndexNode->IsAtom() )
			return NULL;

		brushIndex = GetInt32( brushIndexNode );
	}

	// fill in the node name for objects and brushes from the name property
	if( type == eObject || type == eBrush )
	{
		if( propIndex >= (int)properties.size() )
			return NULL;

		nodeName = properties[propIndex]->Name();
	}

	// handle prefab specific information
	int prefabIndex = -1;

	if( type == ePrefabRef )
	{
		CLTANode* prefabFileNode = CLTAUtil::ShallowFindList( ltaNode, "prefabfile" );
		CLTANode* prefabNameNode = CLTAUtil::ShallowFindList( ltaNode, "prefabname" );
		CLTANode* prefabPosNode = CLTAUtil::ShallowFindList( ltaNode, "position" );
		CLTANode* prefabRotNode = CLTAUtil::ShallowFindList( ltaNode, "orientation" );

		if( !prefabFileNode || !prefabNameNode || !prefabPosNode || !prefabRotNode )
			return NULL;
		if( (prefabFileNode->GetNumElements() < 2) || (prefabNameNode->GetNumElements() < 2) ||
				(prefabPosNode->GetNumElements() < 4) || (prefabRotNode->GetNumElements() < 4) )
			return NULL;

		// get the prefab name
		prefabNameNode = prefabNameNode->GetElement( 1 );
		if( !prefabNameNode->IsAtom() )
			return NULL;

		nodeName = GetString( prefabNameNode );

		// get the prefab filename
		std::string prefabFileName( "" );

		prefabFileNode = prefabFileNode->GetElement( 1 );
		if( !prefabFileNode->IsAtom() )
			return NULL;

		prefabFileName = GetString( prefabFileNode );

		// get the prefab position
		LTVector3f prefabPos;
		if( !ParseNamedVector( prefabPosNode, prefabPos ) )
			return NULL;

		// get the prefab rotation
		LTVector3f prefabRot;
		if( !ParseNamedVector( prefabRotNode, prefabRot ) )
			return NULL;

		// create the prefab ref and add it to the list
		WIPrefabRef* newPrefab = new WIPrefabRef( prefabFileName, prefabPos, prefabRot );
		prefabs.push_back( newPrefab );
		prefabIndex = prefabs.size() - 1;
	}

	// get the children if they exist
	std::vector<WINode*> children;
	unsigned numChildNodes = 0;

	if( childListNode && childListNode->GetNumElements() >= 2 )
	{
		childListNode = childListNode->GetElement( 1 );
		if( !childListNode->IsList() )
			return false;

		std::vector<CLTANode*> childNodes;
		numChildNodes = CLTAUtil::ShallowFindAll( childNodes, childListNode, "worldnode" );

		children.reserve( numChildNodes );

		for( unsigned i = 0; i < numChildNodes; i++ )
		{
			WINode* newChildNode = ParseNodeHierarchy( childNodes[i] );
			if( !newChildNode )
				break;

			children.push_back( newChildNode );
		}

		// we failed to parse one of the children, delete them all and fail
		if( i != numChildNodes )
		{
			for( unsigned j = 0; j < children.size(); j++ )
			{
				delete children[j];
			}

			return NULL;
		}
	}

	// create the new node
	WINode* newNode = new WINode( type, propIndex, brushIndex, prefabIndex, nodeName, children );

	return newNode;
}



//--------------
//--- WIProp ---
//--------------

WIProp::WIProp( const char* name, const LTVector3f& pos, const LTVector3f& rot ) :
	pos( pos ), rot( rot ), name( name )
{
}



//-------------------
//--- WIPrefabRef ---
//-------------------

WIPrefabRef::WIPrefabRef( const std::string& file, const LTVector3f& pos, const LTVector3f& rot ) :
	pos( pos ), rot( rot ), file( file )
{
}



//---------------
//--- WIBrush ---
//---------------

WIBrush::WIBrush() :
	verts(NULL), polies(NULL), numVerts(0), numPolies(0), center(0,0,0)
{
}


WIBrush::~WIBrush()
{
	Free();
}


// free the memory associated with this brush
void WIBrush::Free( void )
{
	delete [] verts;
	delete [] polies;

	verts = NULL;
	polies = NULL;
	numVerts = 0;
	numPolies = 0;
}


// given a brush lta node, parse it, returning false on failure
bool WIBrush::ParseBrush( CLTANode* brushNode, WorldImport* world )
{
	int i;

	// free any brush info already in this brush
	Free();

	if( !brushNode || !brushNode->IsList() )
		return false;

	if( brushNode->GetNumElements() < 2 )
		return false;

	brushNode = brushNode->GetElement( 1 );
	if( !brushNode->IsList() )
		return false;

	CLTANode* pointListNode = CLTAUtil::ShallowFindList( brushNode, "pointlist" );
	CLTANode* polyListNode = CLTAUtil::ShallowFindList( brushNode, "polylist" );

	if( !pointListNode || !polyListNode )
		return false;

	// parse the point list
	numVerts = pointListNode->GetNumElements() - 1;
	if( numVerts <= 0 )
		return true;

	verts = new LTVector3f[numVerts];

	for( i = 0; i < numVerts; i++ )
	{
		if( !ParseVector( pointListNode->GetElement( i+1 ), verts[i] ) )
			return false;
	}

	// get the bounds of the brush in world space
	LTVector3f min = verts[0];
	LTVector3f max = verts[0];

	for( i = 1; i < numVerts; i++ )
	{
		LTVector3f& curVert = verts[i];
		if( curVert.x < min.x ) min.x = curVert.x;
		else if( curVert.x > max.x ) max.x = curVert.x;
		if( curVert.y < min.y ) min.y = curVert.y;
		else if( curVert.y > max.y ) max.y = curVert.y;
		if( curVert.z < min.z ) min.z = curVert.z;
		else if( curVert.z > max.z ) max.z = curVert.z;
	}

	// calculate the center of the brush
	center = (min + max) / 2.0;

	// parse the poly list
	if( polyListNode->GetNumElements() < 2 )
		return false;

	polyListNode = polyListNode->GetElement( 1 );
	if( !polyListNode->IsList() )
		return false;

	// get the polygon nodes
	std::vector<CLTANode*> polyNodes;
	numPolies = CLTAUtil::ShallowFindAll( polyNodes, polyListNode, "editpoly" );

	if( numPolies <= 0 )
		return true;

	polies = new WIPoly[numPolies];

	for( i = 0; i < numPolies; i++ )
	{
		if( !polies[i].ParsePoly( polyNodes[i], world ) )
			return false;
	}

	return true;
}



//--------------
//--- WIPoly ---
//--------------

WIPoly::WIPoly()
{
	vertIndices = NULL;
	numIndices = 0;
}


WIPoly::~WIPoly()
{
	Free();
}


// free the memory associated with this poly
void WIPoly::Free( void )
{
	delete [] vertIndices;
	vertIndices = NULL;
	numIndices = 0;
}


bool WIPoly::ParsePoly( CLTANode* polyNode, WorldImport* world )
{
	// remove any info already stored in this poly
	Free();

	if( !polyNode || !polyNode->IsList() )
		return false;

	CLTANode* indexListNode = CLTAUtil::ShallowFindList( polyNode, "f" );
	CLTANode* normalListNode = CLTAUtil::ShallowFindList( polyNode, "n" );
	CLTANode* distListNode = CLTAUtil::ShallowFindList( polyNode, "dist" );
	CLTANode* textureListNode = CLTAUtil::ShallowFindList( polyNode, "textureinfo" );

	if( !indexListNode || !normalListNode || !distListNode || !textureListNode )
		return false;

	// parse the texture information
	if( textureListNode->GetNumElements() < 4 )
		return false;

	// OPQ info
	if( !ParseVector( textureListNode->GetElement( 1 ), O ) )
		return false;
	if( !ParseVector( textureListNode->GetElement( 2 ), P ) )
		return false;
	if( !ParseVector( textureListNode->GetElement( 3 ), Q ) )
		return false;

	// texture name
	CLTANode* textureNameNode = CLTAUtil::ShallowFindList( textureListNode, "name" );
	if( !textureNameNode || (textureNameNode->GetNumElements() < 2) )
		return false;

	textureNameNode = textureNameNode->GetElement( 1 );
	if( !textureNameNode->IsAtom() )
		return false;

	std::string textureName = textureNameNode->GetValue();

	// poly normal
	if( normalListNode->GetNumElements() < 4 )
		return false;

	for( int i = 0; i < 3; i++ )
	{
		CLTANode* curNode = normalListNode->GetElement( i + 1 );
		if( !curNode->IsAtom() )
			return false;
		normal[i] = GetFloat( curNode );
	}

	// poly plane equation d
	if( distListNode->GetNumElements() < 2 )
		return false;

	CLTANode* distNode = distListNode->GetElement( 1 );
	if( !distNode->IsAtom() )
		return false;
	dist = GetFloat( distNode );

	// vertex indices
	numIndices = indexListNode->GetNumElements() - 1;
	if( numIndices <= 0 )
		return true;

	vertIndices = new int[numIndices];

	for( i = 0; i < numIndices; i++ )
	{
		CLTANode* curNode = indexListNode->GetElement( i + 1 );
		if( !curNode->IsAtom() )
			return false;
		vertIndices[i] = GetInt32( curNode );
	}

	// add the texture name to the world list and set the index
	textureIndex = world->AddTexture( textureName );

	return true;
}



//--------------
//--- WINode ---
//--------------

WINode::WINode( WINodeType type, int propIndex, int brushIndex, int prefabIndex, std::string& name, std::vector<WINode*>& children )
	: type(type), propIndex(propIndex), brushIndex(brushIndex), prefabIndex(prefabIndex), name(name), children(children)
{
}


WINode::~WINode()
{
	// delete the children
	for( unsigned i = 0; i < children.size(); i++ )
	{
		delete children[i];
	}
	children.clear();
}



//---------------
//--- Globals ---
//---------------

// take a list node and extract a vector, returns false on failure (without altering vector)
bool ParseVector( CLTANode* node, LTVector3f& vector )
{
	if( !node->IsList() || (node->GetNumElements() < 3) )
		return false;

	LTVector3f tmp;

	for( int i = 0; i < 3; i++ )
	{
		CLTANode* curNode = node->GetElement( i );
		if( !curNode->IsAtom() )
			return false;

		tmp[i] = GetFloat( curNode );
	}

	vector = tmp;
	return true;
}


// take a list node and extract a vector after the name element, returns false on failure (without altering vector)
bool ParseNamedVector( CLTANode* node, LTVector3f& vector )
{
	if( !node->IsList() || (node->GetNumElements() < 4) )
		return false;

	LTVector3f tmp;

	for( int i = 0; i < 3; i++ )
	{
		CLTANode* curNode = node->GetElement( i+1 );
		if( !curNode->IsAtom() )
			return false;

		tmp[i] = GetFloat( curNode );
	}

	vector = tmp;
	return true;
}
