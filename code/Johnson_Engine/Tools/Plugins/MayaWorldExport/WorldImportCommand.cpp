#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include <maya/MFileIO.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnSet.h>
#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnLambertShader.h>
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <maya/MEulerRotation.h>
#include <maya/MPlug.h>
#include <maya/MItDependencyNodes.h>
#include "WorldImportCommand.h"
#include <set>
#include <utility>


WorldImportCommand::WorldImportCommand()
{
	scale = 1.0;
	importObjects = false;
	importPrefabs = false;
	importTexInfo = false;
	importTexTga = true;
	baseTexPath = "";
	projectPath = "";
	numFailedTextureLoads = 0;
	curPrefab = 0;
}


WorldImportCommand::~WorldImportCommand()
{
}


void* WorldImportCommand::creator( void )
{
	return new WorldImportCommand();
}


bool WorldImportCommand::isUndoable( void ) const
{
	return false;
}


MSyntax WorldImportCommand::newSyntax( void )
{
	MSyntax syntax;

	// add the optional flags
	syntax.addFlag( "ob", "objects" );
	syntax.addFlag( "ti", "useTexInfo", MSyntax::kString );
	syntax.addFlag( "sc", "scale", MSyntax::kDouble );
	syntax.addFlag( "dt", "dtxTextures" );
	syntax.addFlag( "pr", "prefabs", MSyntax::kString );

	// add the filename argument
	syntax.addArg( MSyntax::kString );

	return syntax;
}


// parses the arguments passed into this mel command
MStatus WorldImportCommand::parseArgs( const MArgList& args )
{
	MStatus status;

	// initialize the parser
	MArgDatabase argData( syntax(), args );

	// should texture info be read from the lta file, and the base path for the textures
	if( argData.isFlagSet( "ti" ) )
	{
		MString tmp;
		status = argData.getFlagArgument( "ti", 0, tmp );
		if( !status )
			return MStatus::kFailure;

		importTexInfo = true;

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

		// base path must end with a backslash
		if( tmpPath[tmpPathLen-1] != '\\' )
		{
			tmpPath[tmpPathLen] = '\\';
			tmpPath[tmpPathLen+1] = '\0';
		}

		baseTexPath = tmpPath;
	}

	// should prefabs by referenced, and the project path for the prefabs
	if( argData.isFlagSet( "pr" ) )
	{
		MString tmp;
		status = argData.getFlagArgument( "pr", 0, tmp );
		if( !status )
			return MStatus::kFailure;

		importPrefabs = true;

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

		// base path must end with a backslash
		if( tmpPath[tmpPathLen-1] != '\\' )
		{
			tmpPath[tmpPathLen] = '\\';
			tmpPath[tmpPathLen+1] = '\0';
		}

		projectPath = tmpPath;
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

	// should objects be imported as locators
	if( argData.isFlagSet( "ob" ) )
	{
		importObjects = true;
	}

	// should textures be imported as .dtx or .tga
	if( argData.isFlagSet( "dt" ) )
	{
		importTexTga = false;
	}

	// get the full path of the file to be imported
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


MStatus WorldImportCommand::doIt( const MArgList& args )
{
	MStatus status;

	// default to failure
	setResult( false );

	status = parseArgs( args );

	if( status && world.LoadFile( fileName.asChar() ) )
	{
		if( !LoadTextures() )
			return MStatus::kFailure;

		if( BuildHierarchy() )
			setResult( true );
	}

	return MStatus::kSuccess;
}


float coolColors[6][3] = 
{
	{1.0,0.0,0.0},
	{0.0,1.0,0.0},
	{0.0,0.0,1.0},
	{1.0,0.0,1.0},
	{1.0,1.0,0.0},
	{0.0,1.0,1.0}
};


bool WorldImportCommand::LoadTextures( void )
{
	MStatus status;

	int numTextures = world.NumTextures();
	shaders.reserve( numTextures );

	MFnLambertShader shaderFn;

	// for now, all materials are added to the global namespace
	MString oldNamespace;
	MGlobal::executeCommand( "namespaceInfo -cur", oldNamespace );
	MGlobal::executeCommand( "namespace -set \":\"" );

	for( int i = 0; i < numTextures; i++ )
	{
		// get the path to the .tga file and the shader name
		MString simpleTexName;
		MString texName = world.TextureName( i );
		GenerateTextureName( texName, simpleTexName );

		// create the shader
		MObject curShader = shaderFn.create();
		shaderFn.setName( simpleTexName );
		shaderFn.setColor( MColor( coolColors[i%6][0], coolColors[i%6][1], coolColors[i%6][2] ) );

		// get the actual name of the shader (since it may not have been unique)
		MString shaderName = shaderFn.name();
		MString shadingGroupName, cmd;
		
		// create the shading group for this shader
		cmd = "sets -renderable true -noSurfaceShader true -empty -name " + shaderName + "SG";
		MGlobal::executeCommand( cmd, shadingGroupName );

		// attach this shader to the shading group
//		cmd = "connectAttr -f " + shaderName + ".outColor " + shadingGroupName + ".surfaceShader";

		cmd = "defaultNavigation -ce -s " + shaderName + " -d " + shadingGroupName;
		MGlobal::executeCommand( cmd );

		// get the object for the set we just created (there has to be a better way...)
/*		MItDependencyNodes findSG( MFn::kSet );
		bool foundSG = false;
		MObject shadingGroup;
		while( !foundSG && !findSG.isDone() )
		{
			shadingGroup = findSG.item();

			MFnDependencyNode curObjFn( shadingGroup );
			if( curObjFn.name() == shadingGroupName )
				foundSG = true;

			findSG.next();
		}*/

		// map the shading group to the texture index
		shaders.push_back( shadingGroupName );
		texDims.push_back( floatPair(256.0,256.0) );

		// we need to attempt to load texture information
		if( importTexInfo )
		{
			// check to see if the file exists
			const char* texNameChar = texName.asChar();
			FILE* fileExists = fopen( texNameChar, "rb" );
			if( !fileExists )
			{
				numFailedTextureLoads++;
			}
			else
			{
				fclose( fileExists );

				// the texture exists, create it and try to load it
				MFnDependencyNode fileNode;
				MString fileTexName = simpleTexName + "Tex";
				fileNode.create( "file", fileTexName, &status );
				if( !status )
					return false;

				// set the filename attribute
				MPlug curPlug = fileNode.findPlug( "fileTextureName", &status );
				if( !status )
					return false;
				curPlug.setValue( texName );

				float texWidth, texHeight;
				bool gotValidTex = false;

				// get the texture width
				curPlug = fileNode.findPlug( "outSizeX", &status );
				if( status && curPlug.getValue( texWidth ) )
				{
					// get the texture height
					curPlug = fileNode.findPlug( "outSizeY", &status );
					if( status && curPlug.getValue( texHeight ) )
						gotValidTex = true;
				}

				if( gotValidTex )
				{
					// maya 3 has a bug that causes width and height plugs to be swapped
#ifdef USES_MAYA3_SDK
					texDims.back() = floatPair( texHeight, texWidth );
#else
					texDims.back() = floatPair( texWidth, texHeight );
#endif

					// loaded a valid texture, hook it up to the shader
					MString cmd = "connectAttr -f " + fileNode.name() + ".outColor " + shaderName + ".color";
					MGlobal::executeCommand( cmd );

					// create and attach a place2dTexture node
					MFnDependencyNode placeNode;
					MString placeName = simpleTexName + "Place2d";
					placeNode.create( "place2dTexture", placeName, &status );
					if( !status )
						return false;

					placeName = placeNode.name();
					fileTexName = fileNode.name();
					cmd = "defaultNavigation -ce -s " + placeName + " -d " + fileTexName;
					MGlobal::executeCommand( cmd );
				}
				else
					numFailedTextureLoads++;
			}
		}
	}

	MGlobal::executeCommand( "namespace -set \"" + oldNamespace + "\"" );

	return true;
}


void WorldImportCommand::GenerateTextureName( MString& name, MString& simpleName )
{
	// strip off the starting backslash if any
	if( name.index( '\\' ) == 0 )
		name = name.substring( 1, name.length() - 1 );

	// add on the base texture path
	name = baseTexPath + name;

	// replace the extension
	int extPos = name.rindex( '.' );
	if( extPos > 0 )
		name = name.substring( 0, extPos - 1 );
	if( importTexTga )
		name += ".tga";
	else
		name += ".dtx";

	// get the simple name
	int slashPos = name.rindex( '\\' );
	if( extPos > 0 )
		simpleName = name.substring( 0, extPos - 1 );
	simpleName = simpleName.substring( slashPos + 1, simpleName.length() - 1 );
}


bool WorldImportCommand::BuildHierarchy( void )
{
	WINode* root = world.GetWorldRoot();
	if( !root )
		return false;

	// build up the scene from children of the root
	int numChildren = root->NumChildren();

	for( int i = 0; i < numChildren; i++ )
	{
		WINode* curChild = root->GetChild( i );
		if( !AddNode( curChild, MObject::kNullObj ) )
		{
			assert(0);		// this should never happen, and will result in a partially imported scene
			return false;
		}
	}

	return true;
}


bool WorldImportCommand::AddNode( WINode* node, MObject& parentXForm )
{
	assert( node );

	// create and name the transform dag node
	MFnTransform curXFormFn;
	MObject curXForm = curXFormFn.create( parentXForm );
	curXFormFn.setName( node->Name() );

	switch( node->Type() )
	{
	case eContainer:
		// nothing to do
		break;
	case eBrush:
		if( !AddBrush( node, parentXForm, curXForm ) )
			return false;
		break;
	case eObject:
		if( importObjects && !AddObject( node, parentXForm, curXForm ) )
			return false;
		break;
	case ePrefabRef:
		if( importPrefabs && !AddPrefab( node, parentXForm, curXForm ) )
			return false;
		break;
	default:
		assert( 0 );
		return false;
	}

	// recurse into children
	int numChildren = node->NumChildren();

	for( int i = 0; i < numChildren; i++ )
	{
		WINode* curChild = node->GetChild( i );
		if( !AddNode( curChild, curXForm ) )
			return false;
	}

	return true;
}


bool WorldImportCommand::AddBrush( WINode* node, MObject& parentXForm, MObject& curXForm )
{
	MStatus status;
	LTVector3f ltVec;
	int i, j;

	// grab the brush
	WIBrush* brush = world.GetBrush( node->BrushIndex() );

	int numVerts = brush->numVerts;
	int numPolies = brush->numPolies;

	if( !numVerts || !numPolies )
		return true;

	// get the transformation matrix for the parent
	MMatrix parentInvMatrix;

	if( !parentXForm.isNull() )
	{
		MDagPath parentPath;

		if( !MDagPath::getAPathTo( parentXForm, parentPath ) )
			return false;

		parentInvMatrix = parentPath.inclusiveMatrix().inverse();
	}

	// create the transformation matrix for the current object
	MTransformationMatrix curXFormMat;
	MMatrix curMatrix;

	MVector center( brush->center.x, brush->center.y, -brush->center.z );
	center *= scale;

	curXFormMat.setTranslation( center, MSpace::kTransform );
	curMatrix = curXFormMat.asMatrix();

	curMatrix *= parentInvMatrix;

	// update the current transform
	MDagPath curPath;
	if( !MDagPath::getAPathTo( curXForm, curPath ) )
		return false;

	MFnTransform curXFormFn( curPath, &status );
	curXFormFn.set( curMatrix );

	// create the vert array
	MPointArray verts( numVerts );

	for( i = 0; i < numVerts; i++ )
	{
		LTVector3f vec = brush->verts[i];
		MVector mvec( vec.x, vec.y, -vec.z );
		verts[i] = mvec * scale;
		verts[i] -= center;
	}

	// create the poly vert counts array
	MIntArray polyCounts( numPolies );
	int numPolyVerts = 0;

	for( i = 0; i < numPolies; i++ )
	{
		int curNumPolyVerts = brush->polies[i].numIndices;
		polyCounts[i] = curNumPolyVerts;
		numPolyVerts += curNumPolyVerts;
	}

	// create the polygon vert indices array
	MIntArray vertIndices( numPolyVerts );
	int curVertIndex = 0;

	for( i = 0; i < numPolies; i++ )
	{
		int curNumPolyVerts = brush->polies[i].numIndices;

		for( j = curNumPolyVerts - 1; j >= 0; j-- )
		{
			vertIndices[curVertIndex++] = brush->polies[i].vertIndices[j];
		}
	}

	// create the uv arrays
	MFloatArray uCoords( numPolyVerts );
	MFloatArray vCoords( numPolyVerts );

	int curPolyVert = 0;

	for( i = 0; i < numPolies; i++ )
	{
		WIPoly& curPoly = brush->polies[i];

		int curNumPolyVerts = curPoly.numIndices;
		float texWidth = texDims[curPoly.textureIndex].first;
		float texHeight = texDims[curPoly.textureIndex].second;
		MVector o( curPoly.O.x, curPoly.O.y, -curPoly.O.z );
		MVector p( curPoly.P.x, curPoly.P.y, -curPoly.P.z );
		MVector q( curPoly.Q.x, curPoly.Q.y, -curPoly.Q.z );
		o *= scale;
		o += -center;
		p /= scale;
		q /= scale;

		for( j = curNumPolyVerts - 1; j >= 0; j-- )
		{
			MVector curVert = verts[ curPoly.vertIndices[j] ];

			curVert *= -1.0;
			curVert += o;
			float u = curVert * p;
			float v = curVert * q;

			u /= texWidth;
			v /= texHeight;

			uCoords[curPolyVert] = -u;
			vCoords[curPolyVert] = v;

			curPolyVert++;
		}
	}

	// create the poly indices into the uv arrays
	MIntArray uvIndices( numPolyVerts );

	for( i = 0; i < numPolyVerts; i++ )
	{
		uvIndices[i] = i;
	}

	// create the mesh
	MFnMesh meshFn;
	meshFn.create( numVerts, numPolies, verts, polyCounts, vertIndices, curXForm, &status );
	if( !status )
		return false;

	// assign the UVs
	meshFn.setUVs( uCoords, vCoords );
	meshFn.assignUVs( polyCounts, uvIndices );

	// make the edges non-smooth
	int numEdges = meshFn.numEdges();
	for( i = 0; i < numEdges; i++ )
	{
		meshFn.setEdgeSmoothing( i, false );
	}
	meshFn.cleanupEdgeSmoothing();

	// name the mesh
	MFnDependencyNode xFormNameFn( curXForm );
	MString meshName = xFormNameFn.name();
	meshName += "Shape";
	meshFn.setName( meshName );

	// grab all the texture indices in this mesh
	std::set<int> texIndices;
	for( i = 0; i < numPolies; i++ )
	{
		texIndices.insert( brush->polies[i].textureIndex );
	}

	MDagPath meshp;
	meshFn.getPath( meshp );
	meshp.pop( 1 );
	MString meshn = meshp.fullPathName();

	// hook up the entire mesh to the first shading group
	// (use this instead of the code chunk below until we figure out why Maya doesn't recognize the faces as renderable)
	MGlobal::selectByName( ":" + shaders[*(texIndices.begin())], MGlobal::kReplaceList );
	MSelectionList selList;
	MGlobal::getActiveSelectionList( selList );
	MObject shadingObject;
	selList.getDependNode( 0, shadingObject );
	MFnSet shadingGroup( shadingObject );
	shadingGroup.addMember( meshFn.object() );

	// loop over the mesh for each texture index on it
	// (this code should work, but the face isn't considered to be of the right type for the set)
/*	for( std::set<int>::iterator indexIt = texIndices.begin(); indexIt != texIndices.end(); indexIt++ )
	{
		int curIndex = *indexIt;

		// get the shading group object
		MGlobal::selectByName( shaders[curIndex], MGlobal::kReplaceList );
		MSelectionList selList;
		MGlobal::getActiveSelectionList( selList );
		MObject shadingObject;
		selList.getDependNode( 0, shadingObject );
		MFnSet shadingGroup( shadingObject );

		MItMeshPolygon meshIt( meshFn.object() );
		int curPolyIndex = 0;

		// this could be pretty bad, as it assumes that the polygons were not mangled
		// in any way from what we specified at creation time
		while( !meshIt.isDone() )
		{
			assert( curPolyIndex < numPolies );

			if( brush->polies[curPolyIndex].textureIndex == curIndex )
			{
				MObject curPoly = meshIt.polygon();
				shadingGroup.addMember( meshp, curPoly );
			}

			curPolyIndex++;
			meshIt.next();
		}
	}*/

	return true;
}


bool WorldImportCommand::AddObject( WINode* node, MObject& parentXForm, MObject& curXForm )
{
	MStatus status;

	// create a name for the locator object
	MFnDependencyNode xFormNameFn( curXForm );
	MString locatorName = xFormNameFn.name();
	locatorName += "Shape";

	// create the locator
	MFnDagNode locatorFn;
	locatorFn.create( "locator", locatorName, curXForm, &status );
	if( !status )
		return false;

	// get the properties for this node
	WIProp* property = world.GetProperty( node->PropIndex() );
	LTVector3f ltRot = property->Rot();
	LTVector3f ltPos = property->Pos();
	ltPos.z *= -1.0;
	ltPos *= scale;

	MFnDagNode curXFormDag( curXForm );
	MDagPath curXFormPath;
	status = curXFormDag.getPath( curXFormPath );
	if( !status )
		return false;

	// set the xform based on the properties
	MFnTransform transform( curXFormPath, &status );
	if( !status )
		return false;

	MVector pos( ltPos.x, ltPos.y, ltPos.z );
	if( !transform.setTranslation( pos, MSpace::kWorld ) )
		return false;

	MEulerRotation rot( ltRot.x, ltRot.y, ltRot.z );
	if( !transform.rotateBy( rot, MSpace::kObject ) )
		return false;

	return true;
}


bool WorldImportCommand::AddPrefab( WINode* node, MObject& parentXForm, MObject& curXForm )
{
	WIPrefabRef* prefab = world.GetPrefab( node->PrefabIndex() );

	// build the name of the prefab file
	MString fileName( prefab->File().c_str() );
	fileName = projectPath + fileName;

	const char* tmpFile = fileName.asChar();
	char fixedFile[_MAX_PATH];
	int i;

	for( i = 0; i < fileName.length(); i++ )
	{
		if( tmpFile[i] == '\\' )
			fixedFile[i] = '/';
		else
			fixedFile[i] = tmpFile[i];
	}
	fixedFile[i] = '\0';

	if( !i )
		return true;
	fileName = fixedFile;

	// check to see if the file exists
	int exists = 0;
	MGlobal::executeCommand( "file -q -ex \"" + fileName + "\"", exists );

	if( !exists )
	{
		// file doesn't exist, try changing the extension
		if( fixedFile[i-1] == 'a' )
			fixedFile[i-1] = 'c';
		else
			fixedFile[i-1] = 'a';
		fileName = fixedFile;
	}

	// check if the renamed file exists
	MGlobal::executeCommand( "file -q -ex \"" + fileName + "\"", exists );
	if( !exists )
		return true;

	MFnDagNode curXFormDag( curXForm );
	MDagPath curXFormPath;
	MStatus status = curXFormDag.getPath( curXFormPath );
	if( !status )
		return false;

	// set the xform based on the properties
	MFnTransform transform( curXFormPath, &status );
	if( !status )
		return false;

	// get the position and orientation of this prefab
	LTVector3f ltRot = prefab->Rot();
	LTVector3f ltPos = prefab->Pos();
	ltPos.z *= -1.0f;
	ltPos *= scale;

	// convert eulers to maya space
	MEulerRotation rotX( -ltRot.x, 0.0, 0.0, MEulerRotation::kYXZ );
	MEulerRotation rotY( 0.0, -ltRot.y, 0.0, MEulerRotation::kYXZ );
	MEulerRotation rotZ( 0.0, 0.0, ltRot.z, MEulerRotation::kYXZ );

	MTransformationMatrix rotMat = rotZ.asMatrix() * rotX.asMatrix();
	rotMat = rotMat.asMatrix() * rotY.asMatrix();

	transform.set( rotMat );

	// position the prefab
	MVector pos( ltPos.x, ltPos.y, ltPos.z );
	if( !transform.setTranslation( pos, MSpace::kWorld ) )
		return false;

	// scale the prefab
	double convertScale[3] = { scale, scale, scale };
	if( !transform.setScale( convertScale ) )
		return false;

	MString pfNameSpace( "pf" );
	pfNameSpace += curPrefab++;

	// add the prefab to the scene
	// references don't load correctly when created inside a file translator plug-in
	// so instead we schedule the reference to occur at the next idle,
	// which hopefully happens after loading is complete
	MString cmd = "scriptJob -runOnce true -event \"idle\" \""
							"namespace -add \\\"" + pfNameSpace + "\\\"; "
							"namespace -set \\\"" + pfNameSpace + "\\\"; "
							"file -r -ns \\\"" + pfNameSpace + "\\\" \\\"" + fileName + "\\\"; "
							// get the nodes that came from this referenced file
							"$refNodes = `ls \\\"" + pfNameSpace + ":*\\\"`; "
							"for( $curNode in $refNodes )"
							"{"
								// we're only interested in transform nodes...
								"if( (`size( $curNode )` != 0) && `objectType -i \\\"transform\\\" $curNode` )"
								"{"
									// ... that are at the root level
									"$curNodeParent = `listRelatives -p $curNode`;"
									"if( `size( $curNodeParent )` == 0 )"
									"{"
										// move these root level transforms below the current xform node
										"parent -r $curNode " + curXFormPath.fullPathName() + ";"
									"}"
								"}"
							"} "
							"namespace -set \\\":\\\"; "
							"\";";
	MGlobal::executeCommand( cmd );

	return true;
}
