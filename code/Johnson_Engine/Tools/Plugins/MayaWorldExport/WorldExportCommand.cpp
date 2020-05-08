#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MFnTransform.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MEulerRotation.h>
#include <maya/MQuaternion.h>
#include <maya/MPlug.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MAnimControl.h>
#include <maya/MAnimUtil.h>
#include "LTEulerAngles.h"
#include "WorldExportCommand.h"

#pragma warning(disable : 4018 4244)


WorldExportCommand::WorldExportCommand()
{
	exportAll = false;
	exportUVs = false;
	projectPath = "";
	scale = 1.0;
	detailBrushes = false;
	triangulateBrushes = false;
	exportAnimations = false;
	forceKeys = false;
	forceKeysFrames = 5;
	includeKeys = false;
	externalKeys = false;
	keyPath = "";

	numImproperFaces = 0;
}


WorldExportCommand::~WorldExportCommand()
{
	// delete the animnodes
	for( std::list<AnimNode*>::iterator it = animNodes.begin(); it != animNodes.end(); it++ )
	{
		AnimNode* curAnimNode = *it;
		assert( curAnimNode );
		delete curAnimNode;
	}
}


void* WorldExportCommand::creator( void )
{
	return new WorldExportCommand();
}


bool WorldExportCommand::isUndoable( void ) const
{
	return false;
}


MSyntax WorldExportCommand::newSyntax( void )
{
	MSyntax syntax;

	// add the optional flags
	syntax.addFlag( "all", "allObjects" );
	syntax.addFlag( "pp", "projectPath", MSyntax::kString );
	syntax.addFlag( "ti", "useTexInfo" );
	syntax.addFlag( "sc", "scale", MSyntax::kDouble );
	syntax.addFlag( "db", "detailBrushes" );
	syntax.addFlag( "tb", "triangulateBrushes" );
	syntax.addFlag( "ea", "exportAnimations" );
	syntax.addFlag( "fk", "forceKeys", MSyntax::kUnsigned );
	syntax.addFlag( "ik", "includeKeys" );
	syntax.addFlag( "ek", "externalKeyFile", MSyntax::kString );

	// add the filename argument
	syntax.addArg( MSyntax::kString );

	return syntax;
}


MStatus WorldExportCommand::parseArgs( const MArgList& args )
{
	MStatus status;

	// initialize the parser
	MArgDatabase argData( syntax(), args );

	// the game project path
	if( argData.isFlagSet( "pp" ) )
	{
		MString tmp;
		status = argData.getFlagArgument( "pp", 0, tmp );
		if( !status )
		{
			return MStatus::kFailure;
		}

		ConvertPath( tmp );
		projectPath = tmp;
	}

	// get the scale value
	if( argData.isFlagSet( "sc" ) )
	{
		double tmp;
		status = argData.getFlagArgument( "sc", 0, tmp );
		if( !status )
		{
			return MStatus::kFailure;
		}

		scale = tmp;
	}

	// should texture info be saved
	if( argData.isFlagSet( "ti" ) )
	{
		exportUVs = true;
	}

	// should brushes be exported as detail brushes
	if( argData.isFlagSet( "db" ) )
	{
		detailBrushes = true;
	}

	// should brushes be triangulated
	if( argData.isFlagSet( "tb" ) )
	{
		triangulateBrushes = true;
	}

	// should animations be exported as worldmodels
	if( argData.isFlagSet( "ea" ) )
	{
		exportAnimations = true;
	}

	// should keys be forced at even intervals
	if( argData.isFlagSet( "fk" ) )
	{
		unsigned tmp;
		status = argData.getFlagArgument( "fk", 0, tmp );
		if( !status )
		{
			return MStatus::kFailure;
		}

		if( tmp < 1 )
			tmp = 1;

		forceKeys = true;
		forceKeysFrames = tmp;
	}

	// should user defined keys also be included
	if( argData.isFlagSet( "ik" ) )
	{
		includeKeys = true;
	}

	// should keys be saved to an external path
	if( argData.isFlagSet( "ek" ) )
	{
		MString tmp;
		status = argData.getFlagArgument( "ek", 0, tmp );
		if( !status )
			return MStatus::kFailure;

		externalKeys = true;

		ConvertPath( tmp );
		keyPath = tmp;
	}

	// should the entire scene be exported, or just the current selection
	if( argData.isFlagSet( "all" ) )
	{
		exportAll = true;
	}

	// get the full path of the file to be exported to
	MString tmp;
	status = argData.getCommandArgument( 0, tmp );
	if( !status )
	{
		MGlobal::displayError( "Expected filename argument was not found." );
		return MStatus::kFailure;
	}
	fileName = tmp;

	return MStatus::kSuccess;
}


MStatus WorldExportCommand::doIt( const MArgList& args )
{
	MStatus status;

	// default to failure
	setResult( false );

	status = parseArgs( args );

	if( status && ExportScene() )
		setResult( true );

	return MStatus::kSuccess;
}


bool WorldExportCommand::ExportScene( void )
{
	// create the lta node tree
	ltaFile = new LTAFile( detailBrushes, externalKeys );

	// get the selection list if we care about it
	if( !exportAll )
		MGlobal::getActiveSelectionList( selectionList );

	// get the current animation range
	beginTime = MAnimControl::minTime();
	endTime = MAnimControl::maxTime();

	// build up the scaling and space conversion matrix
	MTransformationMatrix convertTMat;
	double convertScale[3] = { 1.0 * scale, 1.0 * scale, -1.0 * scale };
	convertTMat.setScale( convertScale, MSpace::kWorld );
	convertMat = convertTMat.asMatrix();

	// set the current time to the beginning of the animation range
	MGlobal::viewFrame( beginTime );

	// get the root of the scene hierarchy
	MStatus status;
	bool success = true;
	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status );

	if( !dagIterator.isDone() )
	{
		MDagPath rootPath;
		status = dagIterator.getPath( rootPath );

		if( status )
		{
			int numChildren = rootPath.childCount();

			for( int i = 0; i < numChildren; i++ )
			{
				MDagPath childPath = rootPath;
				MObject childObj = rootPath.child( i );

				// for some odd reason, not all nodes in the dag are dag nodes ?!
				if( !childObj.hasFn( MFn::kDagNode ) )
					continue;

				childPath.push( childObj );

				// find animated nodes
				if( exportAnimations && !FindAnimations( childPath, NULL ) )
				{
					success = false;
					break;
				}

				// export brushes
				if( !NodeEnum( childPath, NULL, true ) )
				{
					success = false;
					break;
				}
			}
		}
		else
			success = false;
	}

	// get the external key file paths (project relative and full)
	MString keyFileRelative, keyFileFull, keyFileFullDir;
	ExternalKeyFileName( fileName.asChar(), keyFileRelative, keyFileFull, keyFileFullDir );

	// make sure the directory for the external key file exists (create it if not)
	if( success && externalKeys )
	{
		// attempt to create the directory
		if( !CreateDirectory( keyFileFullDir.asChar(), NULL ) )
		{
			if( GetLastError() != ERROR_ALREADY_EXISTS )
			{
				MGlobal::displayError( "Couldn't create directory for external key file." );
				success = false;
			}
		}
	}

	// export animated brushes
	if( success && !ExportAnimNodes( keyFileRelative ) )
		success = false;

	// save the file
	if( success )
		success = ltaFile->Output( fileName, keyFileFull );

	delete ltaFile;

	return success;
}


// recursively build a list of animated nodes in the scene
bool WorldExportCommand::FindAnimations( const MDagPath& path, AnimNode* parentAnimNode )
{
	AnimNode* curAnimNode = BuildAnimNode( path, parentAnimNode );

	if( curAnimNode )
		animNodes.push_back( curAnimNode );

	// recurse through the children
	int numChildren = path.childCount();
	for( int i = 0; i < numChildren; i++ )
	{
		MDagPath childPath = path;
		MObject childObj = path.child( i );

		// for some reason, not all nodes in the dag are dag nodes ?!
		if( !childObj.hasFn( MFn::kDagNode ) )
			continue;

		childPath.push( childObj );

		if( !FindAnimations( childPath, curAnimNode ) )
			return false;
	}

	return true;
}


// export a specific node in the scene
bool WorldExportCommand::NodeEnum( const MDagPath& path, MBoundingBox* bounds, bool checkForAnim )
{
	bool nodeAdded = false;

	// don't add the node (or its children) here if it is animated
	if( checkForAnim && FindAnimNode( path ) )
		return true;

	nodeAdded = ExportNode( path, bounds );

	// recurse through children
	int numChildren = path.childCount();
	for( int i = 0; i < numChildren; i++ )
	{
		MDagPath childPath = path;
		MObject childObj = path.child( i );

		// for some odd reason, not all nodes in the dag are dag nodes ?!
		if( !childObj.hasFn( MFn::kDagNode ) )
			continue;

		childPath.push( childObj );

		if( !NodeEnum( childPath, bounds, true ) )
			return false;
	}

	// back up to the correct level in the node tree
	if( nodeAdded )
		ltaFile->AscendTree();

	return true;
}


// returns true if the dag path is a parent of an exportable mesh (anywhere in the hierarchy at or below path)
bool WorldExportCommand::PathParentsValidMesh( const MDagPath& path )
{
	MFnDagNode dagNode( path );

	// check if the current path is a valid mesh
	if( path.hasFn( MFn::kTransform ) && path.hasFn( MFn::kMesh ) && !dagNode.isFromReferencedFile() )
	{
		if( exportAll || selectionList.hasItem( path ) )
			return true;
	}

	// recurse through children
	int numChildren = path.childCount();

	for( int i = 0; i < numChildren; i++ )
	{
		MDagPath childPath = path;
		MObject childObj = path.child( i );

		// for some odd reason, not all nodes in the dag are dag nodes ?!
		if( !childObj.hasFn( MFn::kDagNode ) )
			continue;

		childPath.push( childObj );

		if( PathParentsValidMesh( childPath ) )
			return true;
	}

	return false;
}


bool WorldExportCommand::ExportNode( const MDagPath& path, MBoundingBox* bounds )
{
	// skip intermediate objects
	MFnDagNode dagNode( path );
	if( dagNode.isIntermediateObject() )
		return false;

	// got a transform, but no mesh, assume its a group node
	if( path.hasFn( MFn::kTransform ) && !path.hasFn( MFn::kMesh ) )
	{
		// don't add anything if there are no valid meshes in the hierarchy below
		if( !PathParentsValidMesh( path ) )
			return false;

		// there is a valid mesh somewhere below, add a container
		ltaFile->AddObject( dagNode.name().asChar(), false );
		return true;
	}

	// got a mesh transform node, get the shape and export it
	if( path.hasFn( MFn::kTransform ) && path.hasFn( MFn::kMesh ) )
	{
		// check to see if it is a referenced prefab
		if( dagNode.isFromReferencedFile() )
			return false;

		// check to see if it is selected
		if( !exportAll && !selectionList.hasItem( path ) )
			return false;

		// grab the mesh
		MStatus status;
		MFnMesh mesh( path, &status );
		if( !status )
			return false;

		// get instance number required for determining shader connections
		unsigned instanceNum = 0;
		if( path.isInstanced() )
			instanceNum = path.instanceNumber();

		// get shading groups connected to this mesh
		MObjectArray shadingGroups;
		MObjectArray shaders;
		MIntArray shaderIndices;
		if( !mesh.getConnectedShaders( instanceNum, shadingGroups, shaderIndices ) )
			return false;

		// get the surface shaders for this mesh
		for( int curGroup = 0; curGroup < shadingGroups.length(); curGroup++ )
		{
			MFnDependencyNode curNode( shadingGroups[curGroup] );
			MPlug shaderPlug = curNode.findPlug( "surfaceShader" );

			shaders.append( MObject::kNullObj );
			
			if( !shaderPlug.isNull() )
			{
				MPlugArray connectedPlugs;
				shaderPlug.connectedTo( connectedPlugs, true, false );
				if( connectedPlugs.length() == 1 )
					shaders[curGroup] = connectedPlugs[0].node();
			}
		}

		// arrays of texture info for each of the shaders
		MFloatArray texWidth;
		MFloatArray texHeight;
		MStringArray texName;

		// get the texture info for this mesh
		for( int curShader = 0; curShader < shaders.length(); curShader++ )
		{
			texWidth.append( 0 );
			texHeight.append( 0 );
			texName.append( "Default" );

			// no need for texture info
			if( !exportUVs )
				continue;

			// find the file texture node
			MObject textureNode = MObject::kNullObj;
			MPlug colorPlug = MFnDependencyNode( shaders[curShader] ).findPlug( "color", &status );
			if( status )
			{
				MItDependencyGraph colorGraph( colorPlug, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel, &status );
				if( status )
				{
					colorGraph.disablePruningOnFilter();
					if( !colorGraph.isDone() )
					{
						textureNode = colorGraph.thisNode();
					}
				}
			}

			// found a file texture, grab the info
			if( !textureNode.isNull() )
			{
				bool gotValidTex = false;
				float curWidth, curHeight;
				MString curName;
				MPlug curPlug;
				MFnDependencyNode textureDepNode( textureNode );

				// get the texture name
				curPlug = textureDepNode.findPlug( "fileTextureName", &status );
				if( status && curPlug.getValue( curName ) )
				{
					// get the texture width
					curPlug = textureDepNode.findPlug( "outSizeX", &status );
					if( status && curPlug.getValue( curWidth ) )
					{
						// get the texture height
						curPlug = textureDepNode.findPlug( "outSizeY", &status );
						if( status && curPlug.getValue( curHeight ) )
							gotValidTex = true;
					}
				}

				// set the texture info
				if( gotValidTex && GenerateTextureName( curName ) )
				{
					texName[curShader] = curName;

					// maya 3 had a bug that caused texture width and height to be swapped
#ifdef USES_MAYA3_SDK
					texWidth[curShader] = curHeight;
					texHeight[curShader] = curWidth;
#else
					texWidth[curShader] = curWidth;
					texHeight[curShader] = curHeight;
#endif
				}
			}
		}

		return ExportNodeAsBrush( path, bounds, texWidth, texHeight, texName, shaderIndices );
	}

	// don't know what this path is
	return false;
}


bool WorldExportCommand::ExportNodeAsBrush( const MDagPath& path, MBoundingBox* bounds, const MFloatArray& texWidth, const MFloatArray& texHeight, const MStringArray& texName, const MIntArray& shaderIndices )
{
	assert( path.hasFn( MFn::kTransform ) && path.hasFn( MFn::kMesh ) );

	MStatus status;
	MFnDagNode node( path, &status );
	if( !status )
		return false;

	// set up the mesh
	MFnMesh mesh( path, &status );
	if( !status )
		return false;

	// set up the polygon iterator
	MItMeshPolygon polyIter( path, MObject::kNullObj, &status );
	if( !status )
		return false;

	// add the object to the hierarchy
	ltaFile->AddObject( node.name().asChar(), true );

	int numVerts = mesh.numVertices();

	// list of vertex positions to add to the lta file
	MPoint* vertList = new MPoint[numVerts];

	// fill in the vert list from the mesh
	for( int i = 0; i < numVerts; i++ )
	{
		mesh.getPoint( i, vertList[i], MSpace::kWorld );

		// update the bounding box (in maya space pre-conversion and scale)
		if( bounds )
			bounds->expand( vertList[i] );

		// convert to lithtech space and scale
		vertList[i].z *= -1.0;
		vertList[i] = vertList[i] * scale;
	}

	// add the vertex list to the lta file
	ltaFile->AddBrushVertices( numVerts, vertList );

	// the static vertex index list
	int fixedIndexList[STATIC_VERT_INDEX_SIZE];

	MVector v0, v1, v2, n, o, p, q;
	float2 uv0, uv1, uv2;
	float d;

	// add the faces in the mesh
	for( ; !polyIter.isDone(); polyIter.next() )
	{
		int numPolyVerts = polyIter.polygonVertexCount();

		// texture info
		int polyShader = shaderIndices[polyIter.index()];

		// generate plane equation info
		v0 = vertList[polyIter.vertexIndex(0)];
		v1 = vertList[polyIter.vertexIndex(1)];
		v2 = vertList[polyIter.vertexIndex(2)];
		n = (v2-v0) ^ (v1-v0);
		n.normalize();
		d = n * v0;

		// default OPQs
		o = MVector( 0, 0, 0 );
		p = MVector( 1, 0, 0 );
		q = MVector( 0, 0, 1 );

		// generate OPQ info
		if( polyIter.hasUVs() && (polyShader != -1) && (texWidth[polyShader] > 0) && (texHeight[polyShader] > 0) )
		{
			polyIter.getUV( 0, uv0 );
			polyIter.getUV( 1, uv1 );
			polyIter.getUV( 2, uv2 );
			GenerateOPQ( v0, v1, v2, MVector(uv0[0],uv0[1]), MVector(uv1[0],uv1[1]), MVector(uv2[0],uv2[1]), texWidth[polyShader], texHeight[polyShader], o, p, q );
		}

		MString curTexName( "Default" );
		if( polyShader != -1 )
			curTexName = texName[polyShader];

		// either export triangles, or a full polygon
		if( triangulateBrushes )
		{
			assert( STATIC_VERT_INDEX_SIZE >= 3 );
			int* indices = fixedIndexList;

			// loop over each triangle in the polygon
			for( int curVert = 2; curVert < numPolyVerts; curVert++ )
			{
				indices[0] = polyIter.vertexIndex( 0 );
				indices[1] = polyIter.vertexIndex( curVert );
				indices[2] = polyIter.vertexIndex( curVert - 1 );

				// add the triangle to the lta file
				ltaFile->AddBrushPolygon( 3, indices, n, d, o, p, q, curTexName );
			}
		}
		else
		{
			// dynamically create the index list if there are too many verts
			int* indices;
			if( numPolyVerts > STATIC_VERT_INDEX_SIZE )
				indices = new int[numPolyVerts];
			else
				indices = fixedIndexList;

			int curIndex, indexDelta;
			curIndex = numPolyVerts - 1;
			indexDelta = -1;

			// add indices to the index list
			for( int j = 0; j < numPolyVerts; j++ )
			{
				indices[curIndex] = polyIter.vertexIndex( j );
				curIndex += indexDelta;
			}

			// add the polygon to the lta file
			ltaFile->AddBrushPolygon( numPolyVerts, indices, n, d, o, p, q, curTexName );

			// delete dynamically allocated index list
			if( numPolyVerts > STATIC_VERT_INDEX_SIZE )
				delete [] indices;
		}
	}

	delete [] vertList;

	return true;
}


// given a dag node, find the matching animnode if one exists, otherwise NULL
AnimNode* WorldExportCommand::FindAnimNode( const MDagPath& path )
{
	for( std::list<AnimNode*>::iterator it = animNodes.begin(); it != animNodes.end(); it++ )
	{
		AnimNode* curAnimNode = *it;
		assert( curAnimNode );

		if( curAnimNode->path == path )
			return curAnimNode;
	}

	return NULL;
}


// export the animated brushes
bool WorldExportCommand::ExportAnimNodes( const MString& keyFileRelative )
{
	int numNodes = animNodes.size();
	if( !numNodes )
		return true;

	// sort and remove duplicates for the global keytime list
	allKeys.sort();
	allKeys.unique();

	// the current frame for each animnode
	std::vector<int> curFrames( numNodes, 0 );

	for( std::list<MTime>::iterator timeIt = allKeys.begin(); timeIt != allKeys.end(); timeIt++ )
	{
		// set the current time in maya
		MGlobal::viewFrame( *timeIt );

		// loop over all the nodes and see if they need to store their transform for this frame
		int curNode = 0;
		for( std::list<AnimNode*>::iterator nodeIt = animNodes.begin(); nodeIt != animNodes.end(); nodeIt++ )
		{
			// build the transform for the current node at the current time, increment the node's frame if it had a key at this time
			if( BuildAnimNodeTransform( *nodeIt, *timeIt, curFrames[curNode] ) )
				curFrames[curNode]++;

			curNode++;
		}
	}

	MGlobal::viewFrame( beginTime );

	for( std::list<AnimNode*>::iterator nodeIt = animNodes.begin(); nodeIt != animNodes.end(); nodeIt++ )
	{
		if( !ExportAnimNode( *nodeIt, keyFileRelative ) )
			return false;
	}

	// export group objects for any keyframers that were created
	if( !ExportKeyFramerGroups() )
		return false;

	return true;
}


// export group objects for the keyframers
bool WorldExportCommand::ExportKeyFramerGroups( void )
{
	int curGroup = 0;

	std::list<AnimNode*>::iterator it = animNodes.begin();

	// loop through all animnodes adding them to a group
	while( it != animNodes.end() )
	{
		// make space for the keyframer names
		std::vector<MString> keyFramerNames;
		keyFramerNames.reserve( 50 );

		// only 50 nodes to a group, get their names
		for( int numNodesInGroup = 0; (it != animNodes.end()) && (numNodesInGroup < 50); numNodesInGroup++, it++ )
		{
			AnimNode* curAnimNode = *it;
			MFnTransform xform( curAnimNode->path );
			MString baseName( xform.name() );
			MString keyFramerName = baseName + "KF";
			keyFramerNames.push_back( keyFramerName );
		}

		// name the group
		MString groupName( "KeyFramers" );
		if( curGroup || it != animNodes.end() )
			groupName += curGroup;

		// add the group
		ltaFile->AddKeyFramerGroup( groupName.asChar(), keyFramerNames );
		ltaFile->AscendTree();

		// move on to the next group
		curGroup++;
	}

	return true;
}


// build the transform for the current time, returns false if this node doesn't have a matching key time
bool WorldExportCommand::BuildAnimNodeTransform( AnimNode* animNode, MTime time, int curFrame )
{
	// see if the key list for this node has a key at time
	if( animNode->keys.end() == std::find( animNode->keys.begin(), animNode->keys.end(), time ) )
		return false;

	// get the transform at this time
	animNode->xform[curFrame] = animNode->path.inclusiveMatrix();

	return true;
}


// export a specific animnode
bool WorldExportCommand::ExportAnimNode( AnimNode* animNode, const MString& keyFileRelative )
{
	assert( animNode );

	MFnTransform xform( animNode->path );

	MTransformationMatrix nodeXForm( animNode->path.inclusiveMatrix() );
	MVector nodePos( xform.translation( MSpace::kWorld ) );
	nodePos *= convertMat;

	// add the worldmodel object
	MString baseName( xform.name() );
	MString worldModelName = baseName + "WM";
	ltaFile->AddWorldModel( worldModelName.asChar(), nodePos );

	// bounding box for the animnode geometry
	MBoundingBox bounds;

	// add the node and its children
	if( !NodeEnum( animNode->path, &bounds, false ) )
		return false;

	// pop the worldmodel object
	ltaFile->AscendTree();

	// build the offset transform for the keys (worldnodes rotate about their frame 0 AABB center)
	MTransformationMatrix boundsOffset;
	boundsOffset.setTranslation( bounds.center(), MSpace::kWorld );
	MMatrix offset = boundsOffset.asMatrix() * nodeXForm.asMatrixInverse();

	// add the keyframer object
	MString keyFramerName = baseName + "KF";
	MString keyBaseName = baseName + "Key";

	ltaFile->AddKeyFramer( keyFramerName.asChar(), worldModelName.asChar(), keyBaseName.asChar(), keyFileRelative.asChar(), nodePos );

	int curKeyNum = 0;
	MString curKeyName;
	char buf[256];
	MTime prevKeyTime, curTimeInterval;

	// loop through the key times and add key nodes for them
	for( std::list<MTime>::iterator it = animNode->keys.begin(); it != animNode->keys.end(); it++ )
	{
		// first key time is the difference between two keys (last and first)
		if( !curKeyNum )
		{
			MTime frameInterval;
			frameInterval = 0.0;
			frameInterval += 1.0;
			curTimeInterval = frameInterval;
		}
		else
		{
			curTimeInterval = *it - prevKeyTime;
		}
		
		prevKeyTime = *it;

		// get the name for this key
		if( curKeyNum == 0 )
		{
			curKeyName = keyBaseName + "0";
		}
		else
		{
			sprintf( buf, "%02d", curKeyNum );
			curKeyName = keyBaseName + buf;
		}

		MMatrix keyXForm = offset * animNode->xform[curKeyNum];

		curKeyNum++;

		ExportNodeKey( animNode, curTimeInterval, keyXForm, nodePos, curKeyName.asChar() );
	}

	// pop the keyframer object
	ltaFile->AscendTree();

	return true;
}


// export a key for this node
bool WorldExportCommand::ExportNodeKey( AnimNode* animNode, MTime timeInterval, const MMatrix& xform, const MVector& kfPos, const char* keyName )
{
	MVector pos, rot;

	MTransformationMatrix trans( xform );

	MQuaternion quat = trans.rotation();
	LTRotation ltQuat( quat[0], quat[1], quat[2], quat[3] );
	EulerAngles eul = Eul_FromQuat( ltQuat, EulOrdYXZr );

	rot.y = -eul.x;
	rot.x = -eul.y;
	rot.z = eul.z;

	pos = trans.translation( MSpace::kWorld ) * convertMat;

	float timeInSeconds = timeInterval.as( MTime::kSeconds );

	// add the key object
	ltaFile->AddKey( keyName, timeInSeconds, kfPos, pos, rot, "", 0.0f, "" );

	// pop the key object if we added it to the main world file
	if( !externalKeys )
		ltaFile->AscendTree();

	return true;
}


// take a dag node and fill in the keyframes that it uses, returns NULL if the node is not animated
AnimNode* WorldExportCommand::BuildAnimNode( const MDagPath& path, const AnimNode* parentAnimNode )
{
	AnimNode* curAnimNode = new AnimNode;
	curAnimNode->path = path;

	if( !exportAll && !selectionList.hasItem( path ) )
	{
		// the node isn't selected
		delete curAnimNode;
		return NULL;
	}

	if( !BuildNodeKeyTimes( curAnimNode ) )
	{
		// the node isn't animated
		delete curAnimNode;
		return NULL;
	}

	// copy the parents keys if we want them
	if( parentAnimNode && (includeKeys || !forceKeys) )
	{
		for( std::list<MTime>::const_iterator it = parentAnimNode->keys.begin(); it != parentAnimNode->keys.end(); it++ )
		{
			curAnimNode->keys.push_back( *it );
		}
	}

	// clear out keyframed keys, we're only exporting forced interval keys
	if( forceKeys && !includeKeys )
		curAnimNode->keys.clear();

	// set forced keys
	if( forceKeys )
	{
		MTime frameInterval;
		frameInterval = 0.0;
		frameInterval += forceKeysFrames;

		for( MTime curTime = beginTime; curTime < endTime; curTime += frameInterval )
		{
			curAnimNode->keys.push_back( curTime );
		}
	}

	// set begin and end keys
	curAnimNode->keys.push_front( beginTime );
	curAnimNode->keys.push_back( endTime );

	// make keys unique and sorted
	curAnimNode->keys.sort();
	curAnimNode->keys.unique();

	// setup the position and orientation arrays
	int numKeys = curAnimNode->keys.size();
	curAnimNode->xform.resize( numKeys );

	// add the keys to the global list of keys
	for( std::list<MTime>::const_iterator it = curAnimNode->keys.begin(); it != curAnimNode->keys.end(); it++ )
	{
		allKeys.push_back( *it );
	}

	return curAnimNode;
}


// find the keys set for this node, returns false if no keys were found for the node (non-recursive check)
bool WorldExportCommand::BuildNodeKeyTimes( AnimNode* animNode )
{
	MStatus status;
	assert( animNode );

	// check to see if this node is animated
	if( !MAnimUtil::isAnimated( animNode->path ) )
		return false;

	int keysAdded = 0;
	animNode->keys.clear();

	// grab the animated attributes
	MPlugArray plugs;
	MAnimUtil::findAnimatedPlugs( animNode->path, plugs, true );

	// loop over the plugs and get their animcurves
	for( int curPlugNum = 0; curPlugNum < plugs.length(); curPlugNum++ )
	{
		MObjectArray animCurves;

		// get the animcurves for this plug
		MAnimUtil::findAnimation( plugs[curPlugNum], animCurves );

		// loop over the animcurves and get their keys
		for( int curAnimNum = 0; curAnimNum < animCurves.length(); curAnimNum++ )
		{
			MFnAnimCurve curAnim( animCurves[curAnimNum], &status );
			if( !status )
			{
				assert( 0 );	// why isn't this a valid animcurve?
				continue;
			}

			// make sure it's a time based animcurve
			if( !curAnim.isTimeInput() )
				continue;

			int numKeys = curAnim.numKeys();
			for( int curKey = 0; curKey < numKeys; curKey++ )
			{
				MTime curTime = curAnim.time( curKey );
				if( curTime >= beginTime && curTime <= endTime )
				{
					animNode->keys.push_back( curTime );
					keysAdded++;
				}
			}
		}
	}

	return keysAdded > 0;
}


double OPQArea( const MVector& p0, const MVector& p1, const MVector& p2 )
{
	MVector e0 = p1 - p0;
	MVector e1 = p2 - p0;

	return (e0.x * e1.y - e1.x * e0.y);
}


MVector OPQBaryCoords( const MVector& p0, const MVector& p1, const MVector& p2, const MVector& p )
{
	double n = OPQArea( p0, p1, p2 );

	if( fabs(n) < 0.00000001 )
		return MVector( 1.0, 0.0, 0.0 );

	double u = OPQArea( p1, p2, p ) / n;
	double v = OPQArea( p2, p0, p ) / n;
	double w = 1.0 - u - v;

	return MVector( u, v, w );
}


void WorldExportCommand::GenerateOPQ( const MVector& v0, const MVector& v1, const MVector& v2, const MVector& tv0, const MVector& tv1, const MVector& tv2, int width, int height, MVector& o, MVector& p, MVector& q )
{
	assert( width > 0 && height > 0 );

	// vertex positions in texture space
	MVector uv0( tv0.x, -tv0.y );
	MVector uv1( tv1.x, -tv1.y );
	MVector uv2( tv2.x, -tv2.y );

	// determine barycentric coordinates of OPQ in texture space
	MVector bcO = OPQBaryCoords( uv0, uv1, uv2, MVector( 0.0, 0.0 ) );
	MVector bcP = OPQBaryCoords( uv0, uv1, uv2, MVector( 1.0, 0.0 ) );
	MVector bcQ = OPQBaryCoords( uv0, uv1, uv2, MVector( 0.0, 1.0 ) );

	// calculate OPQ in world space
	MVector O = bcO.x*v0 + bcO.y*v1 + bcO.z*v2;
	MVector P = bcP.x*v0 + bcP.y*v1 + bcP.z*v2;
	MVector Q = bcQ.x*v0 + bcQ.y*v1 + bcQ.z*v2;
	P = P - O;
	Q = Q - O;

	double tp = P.length();
	tp *= 1.0 / (double)width;
	tp = 1.0 / tp;
	double tq = Q.length();
	tq *= 1.0 / (double)height;
	tq = 1.0 / tq;

	P.normalize();
	Q.normalize();

	//fix up p and q to be what DEdit really wants
	MVector R = Q ^ P;
	MVector PNew = R ^ Q;
	MVector QNew = P ^ R;

	// fix up scale factors for new p and q
	PNew.normalize();
	QNew.normalize();
	double pscale = 1.0 / ( P * PNew );
	double qscale = 1.0 / ( Q * QNew );

	R = QNew ^ PNew;

	PNew *= tp * pscale;
	QNew *= tq * qscale;

	// orthogonalize P and Q
	R.normalize();
	P = PNew + R;
	Q = QNew - (( PNew * QNew ) * R);

	o = O;
	p = P;
	q = Q;
}


// strip the base path off of the texture name, if a bogus name results, return false
bool WorldExportCommand::GenerateTextureName( MString& name )
{
	char newdir[_MAX_PATH];
	char fulldir[_MAX_PATH];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char path[_MAX_PATH];

	strcpy( path, name.asChar() );
	int pathLen = strlen( path );
	for( int i = 0; i < pathLen; i++ )
	{
		if( isupper( path[i] ) )
			path[i] = tolower( path[i] );
		else if( path[i] == '/' )
			path[i] = '\\';
	}

	// fulldir starts out being just the drive
	_splitpath( path, fulldir, dir, fname, ext );
	strcat( fulldir, dir );

	int fulldirLen = strlen( fulldir );

	// oops, the base path is longer than the texture directory path
	int basePathLen = strlen( projectPath.asChar() );
	if( fulldirLen < basePathLen )
		return false;

	// copy off the last part of the directory
	strcpy( newdir, fulldir + basePathLen );

	// test to make sure the roots of each of the paths are identical
	fulldir[basePathLen] = 0;
	if( strcmp( fulldir, projectPath.asChar() ) )
		return false;

	// create the new texture name
	sprintf( path, "%s%s.dtx", newdir, fname );
	name = path;

	return true;
}


// create the external key file name based on the filename and key path offset
void WorldExportCommand::ExternalKeyFileName( const char* name, MString& projectRelative, MString& fullPath, MString& fullPathDir )
{
	char projRel[_MAX_PATH];
	char full[_MAX_PATH];
	char fullDir[_MAX_PATH];
	char fname[_MAX_FNAME];

	_splitpath( name, NULL, NULL, fname, NULL );

	sprintf( full, "%s%s%sKeys.ltc", projectPath.asChar(), keyPath.asChar(), fname );
	sprintf( projRel, "%s%sKeys.ltc", keyPath.asChar(), fname );
	sprintf( fullDir, "%s%s", projectPath.asChar(), keyPath.asChar() );

	projectRelative = projRel;
	fullPath = full;
	fullPathDir = fullDir;
}


// convert a path into a friendly format
void WorldExportCommand::ConvertPath( MString& path )
{
	char tmpPath[_MAX_PATH];

	strcpy( tmpPath, path.asChar() );
	int tmpPathLen = strlen( tmpPath );
	for( int i = 0; i < tmpPathLen; i++ )
	{
		if( isupper( tmpPath[i] ) )
			tmpPath[i] = tolower( tmpPath[i] );
		else if( tmpPath[i] == '/' )
			tmpPath[i] = '\\';
	}
	if( tmpPathLen && tmpPath[tmpPathLen-1] != '\\' )
	{
		tmpPath[tmpPathLen] = '\\';
		tmpPath[tmpPathLen+1] = 0;
	}

	path = tmpPath;
}
