#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include <maya/MAnimControl.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatMatrix.h>
#include <maya/MDagPathArray.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnTransform.h>
#include <maya/MItGeometry.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnMatrixData.h>
#include "ltautil.h"
#include "ModelExportCommand.h"

#pragma warning(disable : 4244 4018)

//---------------
//--- ExpNode ---
//---------------

ExpNode::~ExpNode()
{
	// clean up the tree
	for( int i = 0; i < children.size(); i++ )
	{
		delete children[i];
	}

	// clean up vertex animation data
	for( list<VertexFrame*>::iterator vaIt = vaData.begin(); vaIt != vaData.end(); vaIt++ )
	{
		delete *vaIt;
	}
	vaData.clear();

	// clean up skeletal animation data
	for( list<SkelFrame*>::iterator skelIt = skelData.begin(); skelIt != skelData.end(); skelIt++ )
	{
		delete *skelIt;
	}
	skelData.clear();
}



//--------------------------
//--- ModelExportCommand ---
//--------------------------

ModelExportCommand::ModelExportCommand()
{
	useTexInfo = false;
	baseTexPath = "";
	scale = 1.0;
	usePlaybackRange = false;
	appendAnim = false;
	ignoreBindPose = false;
	exportNormals = false;
	exportWorldNode = false;
	selectedOnly = true;

	minimumWeight = 0.0001f;
	root = NULL;
}


ModelExportCommand::~ModelExportCommand()
{
}


void* ModelExportCommand::creator( void )
{
	return new ModelExportCommand();
}


bool ModelExportCommand::isUndoable( void ) const
{
	return false;
}


MSyntax ModelExportCommand::newSyntax( void )
{
	MSyntax syntax;

	// add the optional flags
	syntax.addFlag( "all", "allObjects" );
	syntax.addFlag( "ti", "useTexInfo", MSyntax::kString );
	syntax.addFlag( "sc", "scale", MSyntax::kDouble );
	syntax.addFlag( "pr", "usePlaybackRange" );
	syntax.addFlag( "ib", "ignoreBindPose" );
	syntax.addFlag( "en", "exportNormals" );
	syntax.addFlag( "wn", "exportWorldNode" );
	syntax.addFlag( "ap", "append" );

	// add the animation name argument
	syntax.addArg( MSyntax::kString );

	// add the filename argument
	syntax.addArg( MSyntax::kString );

	return syntax;
}


MStatus ModelExportCommand::parseArgs( const MArgList& args )
{
	MStatus status;

	// initialize the parser
	MArgDatabase argData( syntax(), args );

	// should texture info be saved, and if so, what is the base texture path
	if( argData.isFlagSet( "ti" ) )
	{
		MString tmp;
		status = argData.getFlagArgument( "ti", 0, tmp );
		if( !status )
		{
			return MStatus::kFailure;
		}

		useTexInfo = true;

		char tmpPath[_MAX_PATH];

		strcpy( tmpPath, tmp.asChar() );
		int tmpPathLen = strlen( tmpPath );
		for( int i = 0; i < tmpPathLen; i++ )
		{
			if( isupper( tmpPath[i] ) )
				tmpPath[i] = tolower( tmpPath[i] );
			else if( tmpPath[i] == '/' )
				tmpPath[i] = '\\';
		}

		baseTexPath = tmpPath;
		baseTexPath.toLowerCase();
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

	// should animation be restricted to the animation range
	if( argData.isFlagSet( "pr" ) )
	{
		usePlaybackRange = true;
	}

	// should shapes be retrieved from the bindpose or the first frame
	if( argData.isFlagSet( "ib" ) )
	{
		ignoreBindPose = true;
	}

	// should normals be exported
	if( argData.isFlagSet( "en" ) )
	{
		exportNormals = true;
	}

	// should the world node be added to the model node hierarchy
	if( argData.isFlagSet( "wn" ) )
	{
		exportWorldNode = true;
	}

	if( argData.isFlagSet( "all" ) )
	{
		selectedOnly = false;
	}

	// are we appending animation data
	if( argData.isFlagSet( "ap" ) )
	{
		appendAnim = true;
	}

	// get the animation name
	MString tmp;
	status = argData.getCommandArgument( 0, tmp );
	if( !status )
	{
		MGlobal::displayError( "Expected animation name argument was not found." );
		return MStatus::kFailure;
	}
	animName = tmp;

	// get the full path of the file to be exported to
	status = argData.getCommandArgument( 1, tmp );
	if( !status )
	{
		MGlobal::displayError( "Expected filename argument was not found." );
		return MStatus::kFailure;
	}
	fileName = tmp;

	return MStatus::kSuccess;
}


MStatus ModelExportCommand::doIt( const MArgList& args )
{
	MStatus status;

	// default to failure
	setResult( false );

	status = parseArgs( args );

	// grab the initial frame number so we can return once done
	MTime initialTime = MAnimControl::currentTime();

	// export the model
	if( status && ExportModel() )
		setResult( true );

	// return to the frame we were at when the export process began
	MGlobal::viewFrame( initialTime );

	// close the file
	if( ltaWriter.IsValid() )
		ltaWriter.Close();

	// reset everything to get ready for the next export
	CleanUp();

	return MStatus::kSuccess;
}


bool ModelExportCommand::ExportModel( void )
{
	// get the selection list
	MGlobal::getActiveSelectionList( selectionList );

	// get the root of the scene hierarchy
	MStatus status;
	bool success = true;
	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status );
	if( dagIterator.isDone() )
		return true;

	MDagPath rootPath;
	status = dagIterator.getPath( rootPath );

	if( !status )
		return false;

	// open the file
	if( !ltaWriter.Open( fileName.asChar(), CLTAUtil::IsFileCompressed( fileName.asChar() ), appendAnim ) )
	{
		MGlobal::displayError( "Couldn't open \"" + fileName + "\" for writing." );
		return false;
	}

	// build the key frame list
	if( !BuildKeyTimes() )
		return false;

	// jump to the first frame of the animation
	float startTimeMS = keyTimes.front();
	MTime startTime( startTimeMS, MTime::kMilliseconds );
	MGlobal::viewFrame( startTime );

	// grab skin deformers
	if( !EnumSkinNodes() )
		return false;

	// build the tree
	if( !(root = EnumNode( rootPath, NULL )) )
		return false;

	// find all link nodes related to forced nodes and sockets
	if( !TagLinkNodes() )
		return false;

	// export the vertex animated pieces
	for( int i = 0; i < vertexPieces.size(); i++ )
	{
		if( !ExportVertexPiece( vertexPieces[i] ) )
			return false;
	}

	// export the skeletal pieces
	for( i = 0; i < skeletalPieces.size(); i++ )
	{
		if( !ExportSkeletalPiece( skeletalPieces[i] ) )
			return false;
	}

	// export the link tree if we aren't appending an animation
	if( !appendAnim )
	{
		if( !ExportTree( root ) )
			return false;
	}

	// export the vertex animations
	if( !ExportVertexAnimations() )
		return false;

	// export the node animations
	if( keyTimes.size() > 0 )
	{
		if( !ExportNodeAnimations() )
			return false;
	}

	// export the skeletal bindings and sockets
	if( !appendAnim )
	{
		if( !ExportOnLoadCmds() )
			return false;

		// export the texture binding info
		if( !ExportToolsInfo() )
			return false;
	}

	return true;
}


// reset all the local storage
bool ModelExportCommand::CleanUp( void )
{
	// delete the skin nodes list
	vector<SkinNode*>::iterator skinIt;
	for( skinIt = skinNodes.begin(); skinIt != skinNodes.end(); skinIt++ )
	{
		delete *skinIt;
	}
	skinNodes.clear();

	// delete the ExpNode tree
	delete root;
	root = NULL;
	
	// clear out the various node lists
	vertexPieces.clear();
	skeletalPieces.clear();
	forcedNodes.clear();
	socketNodes.clear();

	// clear out the selection list
	selectionList.clear();

	// delete the texture path strings
	for( int i = 0; i < texturePaths.size(); i++ )
	{
		if( texturePaths[i] )
			delete [] texturePaths[i];
	}

	return true;
}


// fill in skinNodes with path / skin cluster associations
bool ModelExportCommand::EnumSkinNodes( void )
{
	MStatus status;

	// iterator for finding skin cluster dependency nodes
	MItDependencyNodes skinIt( MFn::kSkinClusterFilter );

	for( ; !skinIt.isDone(); skinIt.next() )
	{
		// grab the object
		MObject skinObj = skinIt.item();
		if( !skinObj.hasFn( MFn::kSkinClusterFilter ) )
		{
			MGlobal::displayError( "Error retrieving skin cluster." );
			return false;
		}

		// bind it to the skin cluster function set
		MFnSkinCluster skinCluster( skinObj );

		// common error message string
		MString errorMsg( " " + skinCluster.name() + "." );

		// grab the input and output shapes
		MObjectArray inputGeometry;
		MObjectArray outputGeometry;
		MStatus inputStatus;
		MStatus outputStatus;

		// if the user wants to ignore the bind pose, just use the outputs as inputs as well
		if( ignoreBindPose )
			inputStatus = skinCluster.getOutputGeometry( inputGeometry );
		else
			inputStatus = skinCluster.getInputGeometry( inputGeometry );
		outputStatus = skinCluster.getOutputGeometry( outputGeometry );

		CheckErrorMsg( inputStatus && outputStatus, "Couldn't find geometry associated with skin cluster:" + errorMsg );

		// check the number of input and output geometries
		int numInputs = inputGeometry.length();
		int numOutputs = outputGeometry.length();
		CheckErrorMsg( numInputs == numOutputs, "Skin cluster has unmatched input and output geometry:" + errorMsg );

		// loop through each output object and add the skin node
		for( int i = 0; i < numOutputs; i++ )
		{
			const MObject& inputObj = inputGeometry[i];
			const MObject& outputObj = outputGeometry[i];

			// check to see if the inputs and outputs support the function sets we need
			bool inputIsMesh = inputObj.hasFn( MFn::kMesh );
			bool outputIsMesh = outputObj.hasFn( MFn::kMesh );
			bool inputIsDag = inputObj.hasFn( MFn::kDagNode );
			bool outputIsDag = outputObj.hasFn( MFn::kDagNode );

			CheckErrorMsg( inputIsMesh && outputIsMesh, "Skin cluster not operating on polygonal objects:" + errorMsg );
			CheckErrorMsg( inputIsDag && outputIsDag, "Skin cluster has unexpected connections:" + errorMsg );

			// get the path to the inputs and outputs
			MFnDagNode inputDag( inputObj );
			MFnDagNode outputDag( outputObj );
			MDagPath inputPath;
			MDagPath outputPath;
			inputStatus = inputDag.getPath( inputPath );
			outputStatus = outputDag.getPath( outputPath );

			CheckErrorMsg( inputStatus && outputStatus, "Couldn't find dag path information for skin cluster:" + errorMsg );

			// make sure the objects aren't instanced or otherwise odd
			MDagPathArray inputPathArray;
			MDagPathArray outputPathArray;
			inputStatus = inputDag.getAllPaths( inputPathArray );
			outputStatus = outputDag.getAllPaths( outputPathArray );

			CheckErrorMsg( inputStatus && outputStatus, "Couldn't find dag path information for skin cluster:" + errorMsg );

			int numInputPaths = inputPathArray.length();
			int numOutputPaths = outputPathArray.length();

			CheckErrorMsg( numInputPaths == numOutputPaths == 1, "Skin cluster appears to be assigned to instanced geometry:" + errorMsg );

			// check the meshes for validity (should actually check for identical topologies, but vert count will do)
			MFnMesh inputMesh( inputPath );
			MFnMesh outputMesh( outputPath );

			int numInputVerts = inputMesh.numVertices();
			int numOutputVerts = outputMesh.numVertices();

			CheckErrorMsg( numInputVerts == numOutputVerts, "Mesh modifications must occur before skin clusters.  Found mesh modifications after skin cluster:" + errorMsg );

			// made it through validation, add the skin cluster to the list
			SkinNode* skinNode = new SkinNode();
			outputPath.pop();
			skinNode->path = outputPath;
			skinNode->origPath = inputPath;
			skinNode->skin = new MFnSkinCluster( skinObj );
			skinNodes.push_back( skinNode );
		}
	}

	return true;
}


// get the skin node for this dag node, or NULL if there is none
SkinNode* ModelExportCommand::GetSkinNode( const MDagPath& path )
{
	vector<SkinNode*>::iterator skinIt;
	for( skinIt = skinNodes.begin(); skinIt != skinNodes.end(); skinIt++ )
	{
		if( (*skinIt)->path == path )
			return (*skinIt);
	}

	return NULL;
}


// locate a node within a subtree, returns NULL if nothing is found
ExpNode* ModelExportCommand::GetNode( const MDagPath& path, ExpNode* start )
{
	ExpNode* curNode;

	if( start )
		curNode = start;
	else
		curNode = root;

	// tree is empty
	if( !curNode )
		return NULL;

	// found it
	if( curNode->path == path )
		return curNode;

	// recurse into children
	ExpNode* foundNode;
	for( int i = 0; i < curNode->children.size(); i++ )
	{
		if( foundNode = GetNode( path, curNode->children[i] ) )
			return foundNode;
	}

	// nothing found
	return NULL;
}


// build up the tree and piece lists recursively
ExpNode* ModelExportCommand::EnumNode( const MDagPath& path, ExpNode* parent )
{
	ExpNode* expNode = new ExpNode();
	if( !expNode )
		return NULL;

	expNode->path = path;
	expNode->parent = parent;
	expNode->pieceType = DeterminePieceType( path );
	expNode->xform = path.inclusiveMatrix();

	// get the bind pose information if this is a joint
	if( !ignoreBindPose && path.hasFn( MFn::kJoint ) )
	{
		MFnDagNode node( path );

		MObject bindPoseAttribute = node.attribute( "bindPose" );
		MPlug bindPosePlug( node.object(), bindPoseAttribute );
		MObject bindPoseObject;
		MStatus status = bindPosePlug.getValue( bindPoseObject );
		if( !status )
		{
			MGlobal::displayError( "Error retrieving bind pose information for joint." );
			return NULL;
		}

		MFnMatrixData bindPoseData( bindPoseObject );
		MMatrix bindPose = bindPoseData.matrix( &status );
		if( !status )
		{
			MGlobal::displayError( "Error retrieving bind pose information for joint." );
			return NULL;
		}

		expNode->xform = bindPose;
	}

	// the root is automatically a valid link
	if( !parent )
		expNode->isLinkNode = true;

	// add the node to the appropriate flat list of pieces
	if( expNode->pieceType == Vertex )
	{
		vertexPieces.push_back( expNode );
	}
	else if( expNode->pieceType == Skeletal )
	{
		skeletalPieces.push_back( expNode );
	}
	else if( expNode->pieceType == Socket )
	{
		socketNodes.push_back( expNode );
	}
	else if( expNode->pieceType == ForcedNode )
	{
		forcedNodes.push_back( expNode );
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

		// only recurse into children that accept the transform function set
		if( childPath.hasFn( MFn::kTransform ) )
		{
			ExpNode* childNode = EnumNode( childPath, expNode );
			if( !childNode )
				return NULL;

			expNode->children.push_back( childNode );
		}
	}

	return expNode;
}


// tag any forced nodes or socket nodes and their parents
bool ModelExportCommand::TagLinkNodes( void )
{
	// tag forced nodes and their parents
	for( int i = 0; i < forcedNodes.size(); i++ )
	{
		ExpNode* curExpNode = forcedNodes[i];

		while( curExpNode )
		{
			curExpNode->isLinkNode = true;
			curExpNode = curExpNode->parent;
		}
	}

	// tag socket nodes and their parents
	for( i = 0; i < socketNodes.size(); i++ )
	{
		ExpNode* curExpNode = socketNodes[i];
		curExpNode = curExpNode->parent;	// the socket node itself isn't a link

		while( curExpNode )
		{
			curExpNode->isLinkNode = true;
			curExpNode = curExpNode->parent;
		}
	}

	return true;
}


// recursively write the scene link hierarchy to the file
bool ModelExportCommand::ExportTree( ExpNode* expNode )
{
	CheckError( ltaWriter.IsValid() );
	CheckError( expNode );

	// check to see if this is the root node (which might not be written out)
	bool isRootNode = !expNode->parent;

	// count the number of children which are link nodes
	int numValidChildren = 0;

	for( int curChild = 0; curChild < expNode->children.size(); curChild++ )
	{
		if( expNode->children[curChild]->isLinkNode )
			numValidChildren++;
	}

	// if we are skipping the root node, ensure that there is only one link node at the top
	if( isRootNode && !exportWorldNode )
		CheckErrorMsg( numValidChildren == 1, "More than one top level node found.  Either fix this, or enable the exportation of the world node." );

	// if this is the root node, write out the header for the hierarchy
	if( isRootNode )
	{
		ltaWriter.BeginNode();	// push hierarchy node
		ltaWriter.Write( "hierarchy \"model\"" );
		ltaWriter.BeginNode();	// push children node
		ltaWriter.Write( "children" );
		ltaWriter.BeginNode();	// push children list node
	}

	// export this link and its children
	if( expNode->isLinkNode )
	{
		// don't write out this node if it's the world node and exportWorldNode is false
		if( !isRootNode || exportWorldNode )
		{
			MStatus status;

			MFnDagNode dagNode( expNode->path, &status );
			CheckErrorMsg( status, "Invalid dag node." );

			ltaWriter.BeginNode();	// push transform node
			ltaWriter.Write( "transform" );
			ltaWriter.Write( dagNode.name().asChar(), true );

			LTMatrix mat;
			ConvertMatrix( expNode->xform, mat );
			if( !WriteMatrix( mat ) )
				return false;
		}

		// if there is a valid child, create a children node and recurse
		if( numValidChildren > 0 )
		{
			if( isRootNode && !exportWorldNode )
			{
				for( int curChild = 0; curChild < expNode->children.size(); curChild++ )
				{
					if( expNode->children[curChild]->isLinkNode )
						ExportTree( expNode->children[curChild] );
				}
			}
			else
			{
				ltaWriter.BeginNode();	// push children node
				ltaWriter.Write( "children" );
				ltaWriter.BeginNode();	// push children list node

				for( int curChild = 0; curChild < expNode->children.size(); curChild++ )
				{
					if( expNode->children[curChild]->isLinkNode )
						ExportTree( expNode->children[curChild] );
				}

				ltaWriter.EndNode();	// pop children list node
				ltaWriter.EndNode();	// pop children node
			}
		}

		// don't write out this node if it's the world node and exportWorldNode is false
		if( !isRootNode || exportWorldNode )
		{
			ltaWriter.EndNode();	// pop transform node
		}
	}

	// if this is the root node, polish off the header
	if( isRootNode )
	{
		ltaWriter.EndNode();	// pop children list node
		ltaWriter.EndNode();	// pop children node
		ltaWriter.EndNode();	// pop hierarchy node
	}

	return true;
}


// export a skeletally animated piece
bool ModelExportCommand::ExportSkeletalPiece( ExpNode* expNode )
{
	int i;
	MStatus status;
	MFloatArray weights;
	unsigned numWeights;

	// get the skin cluster function set for this node
	SkinNode* skinNode = GetSkinNode( expNode->path );
	CheckError( skinNode );

	MFnSkinCluster* skin = skinNode->skin;
	CheckError( skin );

	// grab the bones for this skin cluster
	MDagPathArray bones;
	int numBones = skin->influenceObjects( bones, &status );
	CheckErrorMsg( status, "Error retrieving influence objects." );

	// initialize the influence map
	skinNode->numInfluences = numBones;
	skinNode->influenceMap.clear();
	skinNode->influenceMap.resize( numBones, -1 );
	vector<bool> influenceUsage;
	influenceUsage.resize( numBones, false );

	// check which influences actually influence any verts
	MItGeometry meshIter( skinNode->path );

	for( ; !meshIter.isDone(); meshIter.next() )
	{
		MObject curVertObj = meshIter.component( &status );
		CheckError( status );

		status = skin->getWeights( skinNode->path, curVertObj, weights, numWeights );
		assert( numWeights == numBones );

		for( int curWeight = 0; curWeight < numWeights; curWeight++ )
		{
			if( weights[curWeight] < minimumWeight )
				continue;

			// mark this influence as active
			influenceUsage[curWeight] = true;
		}
	}

	// put values in the influence map to remap to only the used bones
	int nextInfluenceMapping = 0;
	for( i = 0; i < numBones; i++ )
	{
		if( influenceUsage[i] )
		{
			skinNode->influenceMap[i] = nextInfluenceMapping++;
		}
	}

	// go through all influence bones and tag their parents as link nodes
	for( i = 0; i < numBones; i++ )
	{
		ExpNode* curExpNode = GetNode( bones[i] );
		CheckErrorMsg( curExpNode, "Error retrieving joint information." );

		while( curExpNode )
		{
			curExpNode->isLinkNode = true;
			curExpNode = curExpNode->parent;
		}
	}

	// only export the shape if append animations if off
	if( appendAnim )
		return true;

	// get the name of the exported piece
	MFnDagNode dagNode( expNode->path );
	MString pieceName = dagNode.name();

	if( !ExportPieceMesh( expNode, skinNode->origPath, pieceName, false ) )
		return false;

	return true;
}


// export a vertex animated piece
bool ModelExportCommand::ExportVertexPiece( ExpNode* expNode )
{
	// only export the piece if append animations is off
	if( !appendAnim )
	{
		MFnDagNode dagNode( expNode->path );
		MString pieceName = dagNode.name();

		if( !ExportPieceMesh( expNode, expNode->path, pieceName, true ) )
			return false;
	}

	// vertex pieces must be in the node hierarchy for now.  this shouldn't be
	// required, so this code may be removed in the future
	ExpNode* curExpNode = expNode;
	while( curExpNode )
	{
		curExpNode->isLinkNode = true;
		curExpNode = curExpNode->parent;
	}

	return true;
}


// export the shape info for this dag path
bool ModelExportCommand::ExportPieceMesh( ExpNode* expNode, const MDagPath& path, const MString& name, bool vertexAnimated )
{
	int i;
	char tmpStr[256];
	MStatus status;
	MPoint curVert;
	MFloatVector curNormal;

#ifdef USES_MAYA3_SDK
	long textureIndices[4];
	long renderStyleIndex;
	long renderPriority;
#else
	int textureIndices[4];
	int renderStyleIndex;
	int renderPriority;
#endif

	CheckError( ltaWriter.IsValid() );
	CheckError( path.hasFn( MFn::kMesh ) );

	MFnMesh mesh( path );

	ltaWriter.BeginNode();	// push shape node
	ltaWriter.Write( "shape" );
	ltaWriter.Write( name.asChar(), true );

	ltaWriter.BeginNode();	// push geometry node
	ltaWriter.Write( "geometry" );

	ltaWriter.BeginNode();	// push mesh node
	ltaWriter.Write( "mesh" );
	ltaWriter.Write( name.asChar(), true );

	// print out vertex info
	int numVerts = mesh.numVertices();
	if( numVerts > 0 )
	{
		ltaWriter.BeginNode();	// push vertex parent node
		ltaWriter.Write( "vertex" );

		ltaWriter.BeginNode();	// push vertex list node

		MPointArray verts;
		status = mesh.getPoints( verts, MSpace::kWorld );
		CheckError( status );

		// write the vertex data
		for( i = 0; i < numVerts; i++ )
		{
			ltaWriter.BeginNode();	// push current vertex node
			curVert = verts[i] * scale;
			sprintf( tmpStr, "%f %f %f", -curVert.x, curVert.y, curVert.z );
			ltaWriter.Write( tmpStr );
			ltaWriter.EndNode();	// pop current vertex node
		}

		ltaWriter.EndNode();	// pop vertex list node
		ltaWriter.EndNode();	// pop vertex parent node
	}

	// calculate vertex normals (multiple normals at one vert will overwrite each other since normals are one-to-one with verts)
	MFloatVectorArray normals( numVerts );

	MFloatVectorArray polyVertNormals;
	status = mesh.getNormals( polyVertNormals, MSpace::kWorld );
	CheckError( status );
	MItMeshPolygon polyIter( mesh.object() );

	for( ; !polyIter.isDone(); polyIter.next() )
	{
		MIntArray polyVerts;
		polyIter.getVertices( polyVerts );

		for( int curPolyVert = 0; curPolyVert < polyVerts.length(); ++curPolyVert )
		{
			int normalIndex = polyIter.normalIndex( curPolyVert );
			normals[polyVerts[curPolyVert]] = polyVertNormals[normalIndex];
			normals[polyVerts[curPolyVert]].normalize();
		}
	}

	// print out normal info
	if( (numVerts > 0) && exportNormals )
	{
		ltaWriter.BeginNode();	// push normals parent node
		ltaWriter.Write( "normals" );

		ltaWriter.BeginNode();	// push normals list node

		// write the normal data
		for( i = 0; i < numVerts; ++i )
		{
			ltaWriter.BeginNode();	// push current normal node
			curNormal = normals[i];
			sprintf( tmpStr, "%f %f %f", -curNormal.x, curNormal.y, curNormal.z );
			ltaWriter.Write( tmpStr );
			ltaWriter.EndNode();	// pop current normal node
		}

		ltaWriter.EndNode();	// pop normals list node
		ltaWriter.EndNode();	// pop normals parent node
	}

	// print out uv info
	int numUVs = mesh.numUVs();
	if( numUVs > 0 )
	{
		ltaWriter.BeginNode();	// push uv parent node
		ltaWriter.Write( "uvs" );

		ltaWriter.BeginNode();	// push uv list node

		MFloatArray uCoords;
		MFloatArray vCoords;
		status = mesh.getUVs( uCoords, vCoords );
		CheckError( status );

		for( i = 0; i < numUVs; i++ )
		{
			ltaWriter.BeginNode();	// push current uv node
			sprintf( tmpStr, "%f %f", uCoords[i], 1.0f - vCoords[i] );
			ltaWriter.Write( tmpStr );
			ltaWriter.EndNode();	// pop current uv node
		}

		ltaWriter.EndNode();	// pop uv list node
		ltaWriter.EndNode();	// pop uv parent node
	}

	// print out triangle info
	int numPolies = mesh.numPolygons();
	if( numPolies > 0 )
	{
		int numPolyVerts;
		MIntArray polyVerts;

		ltaWriter.BeginNode();	// push face parent node
		ltaWriter.Write( "tri-fs" );

		ltaWriter.BeginNode();	// push face list node

		for( i = 0; i < numPolies; i++ )
		{
			numPolyVerts = mesh.polygonVertexCount( i );
			mesh.getPolygonVertices( i, polyVerts );

			for( int j = 0; j < numPolyVerts-2; j++ )
			{
				sprintf( tmpStr, "%d %d %d", polyVerts[0], polyVerts[j+2], polyVerts[j+1] );
				ltaWriter.Write( tmpStr );
				ltaWriter.BreakLine();
			}
		}

		ltaWriter.EndNode();	// pop face list node
		ltaWriter.EndNode();	// pop face parent node
	}

	// print out face texture info
	if( numUVs > 0 )
	{
		int numPolyVerts;
		int curUVids[3];

		ltaWriter.BeginNode();	// push face texture parent node
		ltaWriter.Write( "tex-fs" );

		ltaWriter.BeginNode();	// push face texture list node

		for( i = 0; i < numPolies; i++ )
		{
			numPolyVerts = mesh.polygonVertexCount( i );

			for( int j = 0; j < numPolyVerts-2; j++ )
			{
				mesh.getPolygonUVid( i, 0, curUVids[0] );
				mesh.getPolygonUVid( i, j+2, curUVids[1] );
				mesh.getPolygonUVid( i, j+1, curUVids[2] );
				sprintf( tmpStr, "%d %d %d", curUVids[0], curUVids[1], curUVids[2] );
				ltaWriter.Write( tmpStr );
				ltaWriter.BreakLine();
			}
		}

		ltaWriter.EndNode();	// pop face texture list node
		ltaWriter.EndNode();	// pop face texture parent node
	}

	ltaWriter.EndNode();	// pop mesh node
	ltaWriter.EndNode();	// pop geometry node

	MDagPath tmpPath = path;
	tmpPath.extendToShape();
	tmpPath.pop();
	MFnDagNode dagNode( tmpPath );

	// check for texture custom attributes
	int maximumIndex = -1;

	for( i = 0; i < 4; i++ )
	{
		sprintf( tmpStr, "Texture%d", i );
		MPlug plug = dagNode.findPlug( tmpStr, &status );

		if( status && plug.getValue( textureIndices[i] ) )
			maximumIndex = i;
		else
			textureIndices[i] = 0;

		// range check
		if( textureIndices[i] > 500 )
			textureIndices[i] = 0;
	}

	if( maximumIndex >= 0 )
	{
		ltaWriter.BeginNode();	// push texture-indices node
		ltaWriter.Write( "texture-indices" );
		ltaWriter.BeginNode();	// push texture-indices list node

		for( i = 0; i <= maximumIndex; i++ )
		{
			sprintf( tmpStr, "%d", textureIndices[i] );
			ltaWriter.Write( tmpStr );
		}

		ltaWriter.EndNode();	// pop texture-indices list node
		ltaWriter.EndNode();	// pop texture-indices node

		if( !GetTextureInfo( expNode->path, textureIndices[0] ) )
			return false;
	}

	// check for renderstyle custom attribute
	MPlug plug = dagNode.findPlug( "RenderStyle", &status );

	if( status && plug.getValue( renderStyleIndex ) )
	{
		ltaWriter.BeginNode();	// push renderstyle-index node
		ltaWriter.Write( "renderstyle-index" );
		sprintf( tmpStr, "%d", renderStyleIndex );
		ltaWriter.Write( tmpStr );
		ltaWriter.EndNode();	// pop renderstyle-index node
	}

	// check for renderpriority custom attribute
	plug = dagNode.findPlug( "RenderPriority", &status );

	if( status && plug.getValue( renderPriority ) )
	{
		ltaWriter.BeginNode();	// push render-priority node
		ltaWriter.Write( "render-priority" );
		sprintf( tmpStr, "%d", renderPriority );
		ltaWriter.Write( tmpStr );
		ltaWriter.EndNode();	// pop render-priority node
	}

	ltaWriter.EndNode();	// pop shape node

	return true;
}


// export vertex animations
bool ModelExportCommand::ExportVertexAnimations( void )
{
	CheckError( ltaWriter.IsValid() );

	// start the animset
	char tmpStr[256];
	sprintf( tmpStr, "animset \"%sVA\"", animName.asChar() );
	ltaWriter.BeginNode();	// push animset node
	ltaWriter.Write( tmpStr );

	// write out the frame times
	WriteFrameList( keyTimesVA );

	// start the anims node
	ltaWriter.BeginNode();	// push anims node
	ltaWriter.Write( "anims" );
	ltaWriter.BeginNode();	// push anims list node

	// build up the keyframe information
	for( list<double>::iterator it = keyTimesVA.begin(); it != keyTimesVA.end(); it++ )
	{
		// set the time to this keyframe
		MTime curTime( *it, MTime::kMilliseconds );
		MGlobal::viewFrame( curTime );

		// append keyframe data for each piece
		for( int i = 0; i < vertexPieces.size(); i++ )
		{
			if( !BuildVertexAnimation( vertexPieces[i] ) )
				return false;
		}
	}

	// export the vertex animation for each piece
	for( int i = 0; i < vertexPieces.size(); i++ )
	{
		if( !ExportVertexAnimation( vertexPieces[i] ) )
			return false;
	}

	// clean up
	ltaWriter.EndNode();	// pop anims list node
	ltaWriter.EndNode();	// pop anims node
	ltaWriter.EndNode();	// pop animset node

	return true;
}


// add animation data for the current frame for a vertex animated piece
bool ModelExportCommand::BuildVertexAnimation( ExpNode* expNode )
{
	MStatus status;
	MString errorMsg( "Error retrieving vertex animated mesh information: " );

	MFnDagNode dagPath( expNode->path );
	errorMsg += dagPath.name();

	MFnMesh mesh( expNode->path, &status );
	CheckErrorMsg( status, errorMsg.asChar() );

	// get the vertex positions at this frame
	VertexFrame* frame = new VertexFrame;
	status = mesh.getPoints( frame->verts, MSpace::kWorld );
	
	if( !status )
	{
		delete frame;
		MGlobal::displayError( errorMsg );
		return false;
	}

	// append the new animation data
	expNode->vaData.push_back( frame );

	return true;
}


// export the vertex animation for a piece
bool ModelExportCommand::ExportVertexAnimation( ExpNode* expNode )
{
	char tmpStr[256];

	CheckError( ltaWriter.IsValid() );

	MFnDagNode dagPath( expNode->path );

	ltaWriter.BeginNode();	// push anim node
	ltaWriter.Write( "anim" );

	ltaWriter.BeginNode();	// push parent node
	ltaWriter.Write( "parent" );
	ltaWriter.Write( dagPath.name().asChar(), true );
	ltaWriter.EndNode();	// pop parent node

	ltaWriter.BeginNode();	// push frames node
	ltaWriter.Write( "frames" );
	ltaWriter.BeginNode();	// push vertex node
	ltaWriter.Write( "vertex" );
	ltaWriter.BeginNode();	// push vertex list node

	// add vertex information for each frame to the file
	for( list<VertexFrame*>::iterator it = expNode->vaData.begin(); it != expNode->vaData.end(); it++ )
	{
		VertexFrame* curFrame = *it;

		ltaWriter.BeginNode();	// push frame vertex list node

		MPoint curVert;
		int numVerts = curFrame->verts.length();

		for( int i = 0; i < numVerts; i++ )
		{
			curVert = curFrame->verts[i] * scale;
			sprintf( tmpStr, "%f %f %f", -curVert.x, curVert.y, curVert.z );

			ltaWriter.BeginNode();	// push vertex position node
			ltaWriter.Write( tmpStr );
			ltaWriter.EndNode();	// pop vertex position node
		}

		ltaWriter.EndNode();	// pop frame vertex list node
	}

	ltaWriter.EndNode();	// pop vertex list node
	ltaWriter.EndNode();	// pop vertex node
	ltaWriter.EndNode();	// pop frames node
	ltaWriter.EndNode();	// pop anim node

	return true;
}


// export animated positions of nodes
bool ModelExportCommand::ExportNodeAnimations( void )
{
	CheckError( ltaWriter.IsValid() );

	ltaWriter.BeginNode();	// push animset node
	ltaWriter.Write( "animset" );
	ltaWriter.Write( animName.asChar(), true );

	// write out the list of keyframe times
	if( !WriteFrameList( keyTimes ) )
		return false;

	ltaWriter.BeginNode();	// push anims node
	ltaWriter.Write( "anims" );
	ltaWriter.BeginNode();	// push anims list node

	// build up the keyframe information
	for( list<double>::iterator it = keyTimes.begin(); it != keyTimes.end(); it++ )
	{
		// set the time to this keyframe
		MTime curTime( *it, MTime::kMilliseconds );
		MGlobal::viewFrame( curTime );

		// recurse and append keyframe data
		BuildNodeAnimation( root );
	}

	// recurse and write out animation data
	if( !ExportNodeAnimation( root ) )
		return false;

	ltaWriter.EndNode();	// pop anims list node
	ltaWriter.EndNode();	// pop anims node
	ltaWriter.EndNode();	// pop animset node

	return true;
}


// add animation data for the current frame for a node and recurse
bool ModelExportCommand::BuildNodeAnimation( ExpNode* expNode )
{
	// stop recursing if this isn't a valid link node
	if( !expNode->isLinkNode )
		return true;

	// get the transform info and convert it to pos-quat format
	MFnDagNode dagNode( expNode->path );

	MMatrix xformMat = dagNode.transformationMatrix();
	MTransformationMatrix xform( xformMat );
	MVector xformVec = xform.translation( MSpace::kWorld );
	xformVec *= scale;
	LTVector pos( -xformVec.x, xformVec.y, xformVec.z );

	LTMatrix mat;
	ConvertMatrix( xformMat, mat );

	mat.SetTranslation( pos );

	LTRotation quat;
	quat.ConvertFromMatrix( mat );

	// append the new animation data
	SkelFrame* frame = new SkelFrame( pos, quat );
	expNode->skelData.push_back( frame );

	// recurse into children
	for( int i = 0; i < expNode->children.size(); i++ )
	{
		if( !BuildNodeAnimation( expNode->children[i] ) )
			return false;
	}

	return true;
}


// export animation data for a node and recurse
bool ModelExportCommand::ExportNodeAnimation( ExpNode* expNode )
{
	CheckError( ltaWriter.IsValid() );

	// stop recursing if this isn't a valid link node
	if( !expNode->isLinkNode )
		return true;

	// if this is the root node, don't write animation data, just recurse
	if( !expNode->parent )
	{
		for( int i = 0; i < expNode->children.size(); i++ )
		{
			if( !ExportNodeAnimation( expNode->children[i] ) )
				return false;
		}
		return true;
	}

	MFnDagNode dagNode( expNode->path );
	MString nodeName = dagNode.name();

	ltaWriter.BeginNode();	// push anim node
	ltaWriter.Write( "anim" );

	ltaWriter.BeginNode();	// push parent node
	ltaWriter.Write( "parent" );
	ltaWriter.Write( nodeName.asChar(), true );
	ltaWriter.EndNode();	// pop parent node
	
	ltaWriter.BeginNode();	// push frames node
	ltaWriter.Write( "frames" );
	ltaWriter.BeginNode();	// push posquat node
	ltaWriter.Write( "posquat" );
	ltaWriter.BeginNode();	// push posquat list node

	char tmpStr[256];

	for( list<SkelFrame*>::iterator it = expNode->skelData.begin(); it != expNode->skelData.end(); it++ )
	{
		SkelFrame* curFrame = *it;

		ltaWriter.BeginNode();	// push current frame posquat node

		ltaWriter.BeginNode();	// push pos node
		sprintf( tmpStr, "%f %f %f", curFrame->pos[0], curFrame->pos[1], curFrame->pos[2] );
		ltaWriter.Write( tmpStr );
		ltaWriter.EndNode();	// pop pos node

		ltaWriter.BeginNode();	// push quat node
		sprintf( tmpStr, "%f %f %f %f", curFrame->quat.m_Quat[0], curFrame->quat.m_Quat[1], curFrame->quat.m_Quat[2], curFrame->quat.m_Quat[3] );
		ltaWriter.Write( tmpStr );
		ltaWriter.EndNode();	// pop quat node

		ltaWriter.EndNode();	// pop current frame posquat node
	}

	ltaWriter.EndNode();	// pop posquat list node
	ltaWriter.EndNode();	// pop posquat node
	ltaWriter.EndNode();	// pop frames node
	ltaWriter.EndNode();	// pop anim node

	for( int i = 0; i < expNode->children.size(); i++ )
	{
		if( !ExportNodeAnimation( expNode->children[i] ) )
			return false;
	}

	return true;
}


// export on-load-cmds node
bool ModelExportCommand::ExportOnLoadCmds( void )
{
	CheckError( ltaWriter.IsValid() );

	ltaWriter.BeginNode();	// push on-load-cmds node
	ltaWriter.Write( "on-load-cmds" );

	// export the sockets
	if( !ExportSockets() )
		return false;

	// export the skeletal bindings
	if( !ExportSkeletalBindings() )
		return false;

	ltaWriter.EndNode();	// pop on-load-cmds node

	return true;
}


// export the sockets
bool ModelExportCommand::ExportSockets( void )
{
	CheckError( ltaWriter.IsValid() );

	// nothing to export
	if( socketNodes.size() <= 0 )
		return true;

	ltaWriter.BeginNode();	// push add-sockets node
	ltaWriter.Write( "add-sockets" );

	char tmpStr[256];

	for( int i = 0; i < socketNodes.size(); i++ )
	{
		ExpNode* curExpNode = socketNodes[i];
		MFnDagNode curNode( curExpNode->path );

		ExpNode* parentExpNode = curExpNode->parent;
		MFnDagNode parentNode( parentExpNode->path );

		LTMatrix curXForm;
		ConvertMatrix( curExpNode->path.inclusiveMatrix(), curXForm );

		LTMatrix parentXForm;
		ConvertMatrix( parentExpNode->path.inclusiveMatrix(), parentXForm );

		// remove the parents portion of the transform
		curXForm = (~parentXForm) * curXForm;

		// get the orientation
		LTRotation quat;
		quat.ConvertFromMatrix( curXForm );

		// get the position
		LTVector pos;
		curXForm.GetTranslation( pos );

		// get the scale
		double socketScale[3];
		MFnTransform scaleXForm( curExpNode->path );
		scaleXForm.getScale( socketScale );

		ltaWriter.BeginNode();	// push socket node
		ltaWriter.Write( "socket" );
		ltaWriter.Write( curNode.name().asChar(), true );

		ltaWriter.BeginNode();	// push parent node
		ltaWriter.Write( "parent" );
		ltaWriter.Write( parentNode.name().asChar(), true );
		ltaWriter.EndNode();	// pop parent node

		ltaWriter.BeginNode();	// push pos node
		ltaWriter.Write( "pos" );
		ltaWriter.BeginNode();	// push pos vector node
		sprintf( tmpStr, "%f %f %f", pos[0], pos[1], pos[2] );
		ltaWriter.Write( tmpStr );
		ltaWriter.EndNode();	// pop pos vector node
		ltaWriter.EndNode();	// pop pos node

		ltaWriter.BeginNode();	// push quat node
		ltaWriter.Write( "quat" );
		ltaWriter.BeginNode();	// push quat vector node
		sprintf( tmpStr, "%f %f %f %f", quat[0], quat[1], quat[2], quat[3] );
		ltaWriter.Write( tmpStr );
		ltaWriter.EndNode();	// pop quat vector node
		ltaWriter.EndNode();	// pop quat node

		ltaWriter.BeginNode();	// push scale node
		ltaWriter.Write( "scale" );
		ltaWriter.BeginNode();	// push scale vector node
		sprintf( tmpStr, "%f %f %f", socketScale[0] * scale, socketScale[1] * scale, socketScale[2] * scale );
		ltaWriter.Write( tmpStr );
		ltaWriter.EndNode();	// pop scale vector node
		ltaWriter.EndNode();	// pop scale node

		ltaWriter.EndNode();	// pop socket node
	}

	ltaWriter.EndNode();	// pop add-sockets node

	return true;
}


// export the skeletal bindings
bool ModelExportCommand::ExportSkeletalBindings( void )
{
	MStatus status;
	MFloatArray weights;
	unsigned numWeights;
	char tmpStr[256];

	CheckError( ltaWriter.IsValid() );

	for( int i = 0; i < skeletalPieces.size(); i++ )
	{
		// grab the current node and its skin cluster
		ExpNode* curPieceNode = skeletalPieces[i];
		SkinNode* curSkinNode = GetSkinNode( curPieceNode->path );
		CheckError( curSkinNode );

		MFnSkinCluster* skin = curSkinNode->skin;

		// get the influence objects for the skin cluster
		MDagPathArray influenceObjects;
		int numInfluences = skin->influenceObjects( influenceObjects, &status );
		CheckErrorMsg( status && (numInfluences == curSkinNode->numInfluences), "Error retrieving skin weights." );

		// get the name of this skeletal piece
		MFnDagNode dagNode( curPieceNode->path );
		MString pieceName = dagNode.name();

		// start writing the deformer to the file
		ltaWriter.BeginNode();	// push add-deformer node
		ltaWriter.Write( "add-deformer" );
		ltaWriter.BeginNode();	// push skel-deformer node
		ltaWriter.Write( "skel-deformer" );

		ltaWriter.BeginNode();	// push target node
		ltaWriter.Write( "target" );
		ltaWriter.Write( pieceName.asChar(), true );
		ltaWriter.EndNode();	// pop target node

		// print out the bone list
		ltaWriter.BeginNode();	// push influences node
		ltaWriter.Write( "influences" );
		ltaWriter.BeginNode();	// push bone list node

		for( int curBone = 0; curBone < curSkinNode->numInfluences; curBone++ )
		{
			// skip unused bones
			if( curSkinNode->influenceMap[curBone] == -1 )
				continue;

			// write the name of the bone
			MFnDagNode curBoneDag( influenceObjects[curBone] );
			MString boneName = curBoneDag.name();
			ltaWriter.Write( boneName.asChar(), true );
			ltaWriter.BreakLine();
		}

		ltaWriter.EndNode();	// pop bone list node
		ltaWriter.EndNode();	// pop influences node

		// print out the influence list
		ltaWriter.BeginNode();	// push the weightsets node
		ltaWriter.Write( "weightsets" );
		ltaWriter.BeginNode();	// push weightset list node

		MItGeometry meshIter( curSkinNode->path );
		vector<float> curWeights( curSkinNode->numInfluences, 0.0f );

		for( ; !meshIter.isDone(); meshIter.next() )
		{
			int curWeight;
			float totalWeight = 0.0f;

			MObject curVertObj = meshIter.component( &status );
			CheckErrorMsg( status, "Error retrieving skin weights." );

			status = skin->getWeights( curSkinNode->path, curVertObj, weights, numWeights );
			CheckErrorMsg( status, "Error retrieving skin weights." );
			assert( numWeights == curSkinNode->numInfluences );

			for( curWeight = 0; curWeight < numWeights; curWeight++ )
			{
				curWeights[curWeight] = 0.0f;
			}

			for( curWeight = 0; curWeight < numWeights; curWeight++ )
			{
				if( weights[curWeight] < minimumWeight )
					continue;

				curWeights[curWeight] += weights[curWeight];
				totalWeight += weights[curWeight];
			}

			// this shouldn't happen, but don't want a divide by zero
			if( totalWeight < minimumWeight )
				totalWeight = minimumWeight;

			ltaWriter.BeginNode();	// push the weightset node

			// write out the influences on this vert
			for( curWeight = 0; curWeight < numWeights; curWeight++ )
			{
				int mappedBone = curSkinNode->influenceMap[curWeight];
				if( (mappedBone == -1) || (curWeights[curWeight] < minimumWeight) )
					continue;

				sprintf( tmpStr, "%d %f", mappedBone, curWeights[curWeight]/totalWeight );
				ltaWriter.Write( tmpStr );
			}

			ltaWriter.EndNode();	// pop the weightset node
		}

		ltaWriter.EndNode();	// pop weightset list node
		ltaWriter.EndNode();	// pop weightsets node

		ltaWriter.EndNode();	// pop skel-deformer node
		ltaWriter.EndNode();	// pop add-deformer node
	}

	return true;
}


// export tools specific info
bool ModelExportCommand::ExportToolsInfo( void )
{
	char tmpStr[256];

	CheckError( ltaWriter.IsValid() );

	ltaWriter.BeginNode();	// push tools-info node
	ltaWriter.Write( "tools-info" );
	ltaWriter.BeginNode();	// push tools-info list node

	// export texture bindings if there are any
	if( texturePaths.size() > 0 )
	{
		ltaWriter.BeginNode();	// push texture-bindings node
		ltaWriter.Write( "texture-bindings" );
		ltaWriter.BeginNode();	// push texture-bindings list node

		for( int i = 0; i < texturePaths.size(); i++ )
		{
			// don't write out unbound indices
			if( !texturePaths[i] )
				continue;

			ltaWriter.BeginNode();	// push texture binding node
			sprintf( tmpStr, "%d", i );
			ltaWriter.Write( tmpStr );
			ltaWriter.Write( texturePaths[i], true );
			ltaWriter.EndNode();	// pop texture binding node
		}

		ltaWriter.EndNode();	// pop texture-bindings list node
		ltaWriter.EndNode();	// pop texture-bindings node
	}

	ltaWriter.EndNode();	// pop tools-info list node
	ltaWriter.EndNode();	// pop tools-info node

	return true;
}


// build the list of keyframes for skeletal and vertex animations
bool ModelExportCommand::BuildKeyTimes( void )
{
	MStatus status;
	MString errorMsg( "Error retrieving animcurve information." );

	// playback range
	MTime startTime = MAnimControl::minTime();
	MTime endTime = MAnimControl::maxTime();

	// iterator for finding all animcurve dependency nodes
	MItDependencyNodes animIt( MFn::kAnimCurve );

	// loop over all animcurves in the scene and add their keyframes to the master list
	for( ; !animIt.isDone(); animIt.next() )
	{
		// grab the object
		MObject animObj = animIt.item();
		CheckErrorMsg( animObj.hasFn( MFn::kAnimCurve ), errorMsg );

		MFnAnimCurve animCurve( animObj );

		// only care about time based animcurves
		if( !animCurve.isTimeInput() )
			continue;

		int numKeys = animCurve.numKeyframes( &status );
		CheckErrorMsg( status, errorMsg );

		MTime curFrameTime;

		// grab all the keys in this animcurve
		for( int curKey = 0; curKey < numKeys; curKey++ )
		{
			curFrameTime = animCurve.time( curKey );

			if( !usePlaybackRange || ((curFrameTime >= startTime) && (curFrameTime <= endTime)) )
			{
				float curTime = curFrameTime.as( MTime::kMilliseconds );
				keyTimes.push_back( curTime );
			}
		}
	}

	keyTimes.sort();
	keyTimes.unique();

	// make sure to bookend the keyframes
	if( MTime( keyTimes.front(), MTime::kMilliseconds ) > startTime )
		keyTimes.push_front( startTime.as( MTime::kMilliseconds ) );
	if( MTime( keyTimes.back(), MTime::kMilliseconds ) < endTime )
		keyTimes.push_back( endTime.as( MTime::kMilliseconds ) );

	// build up the vertex animation keyframes
	startTime = MTime( keyTimes.front(), MTime::kMilliseconds );
	endTime = MTime( keyTimes.back(), MTime::kMilliseconds );

	MTime increment;
	MTime curTime = startTime;
	while( curTime < endTime )
	{
		double time = curTime.as( MTime::kMilliseconds );
		keyTimesVA.push_back( time );
		curTime += increment;
	}

	// add the last vertex animated frame
	double time = endTime.as( MTime::kMilliseconds );
	keyTimesVA.push_back( time );

	return true;
}


// determine the piece type of a node
PieceType ModelExportCommand::DeterminePieceType( const MDagPath& path )
{
	// check if it is a non-hidden mesh object
	if( IsNodeMesh( path ) && IsVisible( path ) )
	{
		// check if we are only exporting selected
		if( selectedOnly && !selectionList.hasItem( path ) )
			return NotAPiece;

		// check for a vertex animated piece
		if( IsVertexAnimated( path ) )
			return Vertex;

		// check for a skeletally animated piece
		if( GetSkinNode( path ) )
			return Skeletal;
	}

	// check to see if this node (and its tree) should be forced to export
	if( IsForcedNode( path ) )
		return ForcedNode;

	// check to see if this node should be exported as a socket
	if( IsSocket( path ) )
		return Socket;

	return NotAPiece;
}


// returns true if the node is a mesh object
bool ModelExportCommand::IsNodeMesh( const MDagPath& path )
{
	if( path.hasFn( MFn::kMesh ) )
		return true;

	return false;
}


// returns true if the node should be forced to export
bool ModelExportCommand::IsForcedNode( const MDagPath& path )
{
	MFnDagNode dagNode( path );

	if( !strncmp( dagNode.name().asChar(), "n_", 2 ) )
		return true;

	// check for custom attribute "ForcedNode"
	MStatus status;
	MPlug plug = dagNode.findPlug( "ForcedNode", &status );

	if( status )
	{
		bool isForcedNode = false;
		status = plug.getValue( isForcedNode );
		if( status )
			return isForcedNode;
	}

	return false;
}


// returns true if the node should be vertex animated
bool ModelExportCommand::IsVertexAnimated( const MDagPath& path )
{
	MFnDagNode dagNode( path );

	if( !strncmp( dagNode.name().asChar(), "d_", 2 ) )
		return true;

	// check for custom attibute "VertexAnimated"
	_asm nop;
	// modeledit requires the d_, so ignore custom attribute for now

	return false;
}


// returns true if the node is a socket
bool ModelExportCommand::IsSocket( const MDagPath& path )
{
	MFnDagNode dagNode( path );

	if( !strncmp( dagNode.name().asChar(), "s_", 2 ) )
		return true;

	// check for custom attibute "Socket"
	MStatus status;
	MPlug plug = dagNode.findPlug( "Socket", &status );

	if( status )
	{
		bool isSocket = false;
		status = plug.getValue( isSocket );
		if( status )
			return isSocket;
	}

	return false;
}


// returns true if the node is visible
bool ModelExportCommand::IsVisible( const MDagPath& path )
{
	MFnDagNode dagNode( path );

	bool visible = true;
	bool intermediate = false;

	// check the visibility attribute
	MPlug plug = dagNode.findPlug( "visibility" );
	plug.getValue( visible );

	// intermediate objects aren't visible, but they have visibility set to true
	plug = dagNode.findPlug( "intermediateObject" );
	plug.getValue( intermediate );

	return visible && !intermediate;
}


// finds the texture assignment for this mesh and adds it to the map index
bool ModelExportCommand::GetTextureInfo( const MDagPath& dagPath, int index )
{
	MStatus status;

	if( !useTexInfo )
		return true;

	// get instance number required for determining shader connections
	unsigned instanceNum = 0;
	if( dagPath.isInstanced() )
		instanceNum = dagPath.instanceNumber();

	// get the mesh function set
	CheckError( dagPath.hasFn( MFn::kMesh ) );
	MFnMesh mesh( dagPath );

	// get shading groups connected to this mesh
	MObjectArray shadingGroups;
	MObjectArray shaders;
	MIntArray shaderIndices;
	status = mesh.getConnectedShaders( instanceNum, shadingGroups, shaderIndices );
	CheckErrorMsg( status, "Error retrieving shader information." );

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

	// find the first polygon with a valid shader index
	int shaderIndex = -1;
	for( int curShader = 0; (shaderIndex == -1) && (curShader < shaderIndices.length()); curShader++ )
	{
		shaderIndex = shaderIndices[curShader];
	}

	// get the texture filename, if any
	bool foundTexture = false;
	MString fileName;

	if( shaderIndex >= 0 )
	{
		MObject textureNode = MObject::kNullObj;
		MPlug colorPlug = MFnDependencyNode( shaders[shaderIndex] ).findPlug( "color", &status );
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

		// found a file texture, grab the filename
		if( !textureNode.isNull() )
		{
			bool gotValidTex = false;
			MPlug fileNamePlug;
			MFnDependencyNode textureDepNode( textureNode );

			// get the texture name
			fileNamePlug = textureDepNode.findPlug( "fileTextureName", &status );
			if( status && fileNamePlug.getValue( fileName ) )
			{
				foundTexture = true;
			}
		}
	}

	if( !foundTexture )
		return true;

	// convert the filename to a resource path relative dtx
	char newdir[_MAX_PATH];
	char fulldir[_MAX_PATH];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char path[_MAX_PATH];

	strcpy( path, fileName.asChar() );
	int pathLen = strlen( path );
	int basePathLen = baseTexPath.length();
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

	// make sure the base path isn't longer than the texture directory path
	if( fulldirLen >= basePathLen )
	{
		// copy off the last part of the directory
		strcpy( newdir, fulldir + basePathLen );

		// test to make sure the roots of each of the paths are identical
		fulldir[basePathLen] = 0;
		if( baseTexPath == fulldir )
		{
			// create the new texture name
			sprintf( path, "%s%s.dtx", newdir, fname );

			// add the texture name to the texture name indices
			int newPathLen = strlen( path );
			if( newPathLen )
			{
				char* newPath = new char[newPathLen];
				strcpy( newPath, path );

				// resize the vector as needed
				int oldSize = texturePaths.size();
				if( oldSize < (index + 1) )
				{
					texturePaths.resize( index + 1 );
					for( i = oldSize; i < index; i++ )
					{
						texturePaths[i] = NULL;
					}
				}

				texturePaths[index] = newPath;
			}
		}
	}

	return true;
}


// convert a maya matrix to a lithtech matrix
void ModelExportCommand::ConvertMatrix( const MMatrix& src, LTMatrix& dest )
{
	double quat[4];
	LTRotation ltquat;

	MTransformationMatrix mat( src );
	MVector trans = mat.translation( MSpace::kWorld );

	mat.getRotationQuaternion( quat[0], quat[1], quat[2], quat[3] );
	ltquat.Init( quat[0], quat[1], quat[2], quat[3] );
	ltquat.ConvertToMatrix( dest );

	dest.SetTranslation( trans.x, trans.y, trans.z );

	LTVector lttrans;
	dest.GetTranslation( lttrans );

	dest.m[0][1] *= -1.0;
	dest.m[0][2] *= -1.0;
	dest.m[1][0] *= -1.0;
	dest.m[2][0] *= -1.0;

	lttrans.x *= -1.0;
	lttrans *= scale;

	dest.SetTranslation( lttrans );
}


// write a matrix to the file
bool ModelExportCommand::WriteMatrix( const LTMatrix& mat )
{
	char tmpStr[256];

	CheckError( ltaWriter.IsValid() );

	ltaWriter.BeginNode();	// push the matrix node
	ltaWriter.Write( "matrix" );
	ltaWriter.BeginNode();	// push the row list node

	for( int row = 0; row < 4; row++ )
	{
		ltaWriter.BeginNode();	// push the row node

		sprintf( tmpStr, "%f %f %f %f", mat.m[row][0], mat.m[row][1], mat.m[row][2], mat.m[row][3] );
		ltaWriter.Write( tmpStr );

		ltaWriter.EndNode();	// pop the row node
	}

	ltaWriter.EndNode();	// pop the row list node
	ltaWriter.EndNode();	// pop the matrix node

	return true;
}


// write out a list of keyframes to the lta file (adjusts times to 0 based times)
bool ModelExportCommand::WriteFrameList( list<double>& frameList )
{
	CheckError( ltaWriter.IsValid() );

	ltaWriter.BeginNode();	// push outer keyframe node
	ltaWriter.Write( "keyframe" );
	ltaWriter.BeginNode();	// push inner keyframe node
	ltaWriter.Write( "keyframe" );
	ltaWriter.BeginNode();	// push times node
	ltaWriter.Write( "times" );
	ltaWriter.BeginNode();	// push times list node

	double offset = frameList.front();

	list<double>::iterator it;
	int numWritten = 0;
	char tmpStr[256];

	for( it = frameList.begin(); it != frameList.end(); it++ )
	{
		// insert a newline every five keyframes
		if( numWritten && (numWritten % 4 == 0) )
			ltaWriter.BreakLine();
		numWritten++;

		double time = *it;
		time -= offset;

		sprintf( tmpStr, "%f", time );
		ltaWriter.Write( tmpStr );
	}

	ltaWriter.EndNode();	// pop times list node
	ltaWriter.EndNode();	// pop times node
	ltaWriter.EndNode();	// pop inner keyframe node
	ltaWriter.EndNode();	// pop outer keyframe node

	return true;
}
