#include "Exporter.h"
#include "ltautil.h"
#include "MAXNonSDKHeaders\Phyexp.h"
#include "MAXNonSDKHeaders\Bipexp.h"
#include "iparamm2.h"
#include "stdmat.h"
#include "decomp.h"
#include <algorithm>
#ifndef USES_MAX3_SDK
#include "ISkin.h"
#endif


//---------------
//--- ExpNode ---
//---------------

ExpNode::~ExpNode()
{
	// clean up the tree
	for( unsigned i = 0; i < children.size(); i++ )
	{
		delete children[i];
	}
}


//---------------------
//--- ModelExporter ---
//---------------------

ModelExporter::ModelExporter( ExporterOptions* options, Interface* ip, MAXModelExport* exporter, bool selected )
{
	assert( options && ip && exporter );

	selectedOnly = selected;
	this->options = options;
	this->ip = ip;
	this->exporter = exporter;

	minimumWeight = 0.0001f;

	root = NULL;

	// starting animation time
	startTime = ip->GetAnimRange().Start();
}


ModelExporter::~ModelExporter()
{
	// delete the ExpNode tree
	delete root;

	// delete the texture path strings
	for( unsigned i = 0; i < texturePaths.size(); i++ )
	{
		if( texturePaths[i] )
			delete [] texturePaths[i];
	}
}


// export the model (it all begins here)
bool ModelExporter::Export( const TCHAR* fileName )
{
	// get the root of the scene
	INode* maxRoot = ip->GetRootNode();
	BailOutIfFalse( maxRoot );

	// open the file
	if( !ltaWriter.Open( fileName, CLTAUtil::IsFileCompressed( fileName ), options->appendAnimation ) )
		return false;

	// build the tree
	if( !(root = EnumNode( maxRoot, NULL )) )
		return false;

	// find all link nodes related to forced nodes and sockets
	if( !TagLinkNodes() )
		return false;

	// export the vertex animated pieces
	ip->ProgressUpdate( 10, FALSE, GetString(IDS_PROGRESS_VERTEXPIECES) );
	for( unsigned i = 0; i < vertexPieces.size(); i++ )
	{
		if( !ExportVertexPiece( vertexPieces[i] ) )
			return false;
	}

	// export the skeletal pieces
	ip->ProgressUpdate( 30, FALSE, GetString(IDS_PROGRESS_SKELETALPIECES) );
	for( i = 0; i < skeletalPieces.size(); i++ )
	{
		if( !ExportSkeletalPiece( skeletalPieces[i] ) )
			return false;
	}

	// export the link tree if we aren't appending an animation
	if( !options->appendAnimation )
	{
		if( !ExportTree( root ) )
			return false;
	}

	// export the vertex animations
	ip->ProgressUpdate( 50, FALSE, GetString(IDS_PROGRESS_VERTEXANIMS) );
	if( !ExportVertexAnimations() )
		return false;

	// build the key frame list
	BuildKeyTimes( root );

	// export the node animations
	ip->ProgressUpdate( 75, FALSE, GetString(IDS_PROGRESS_SKELETALANIMS) );
	if( keyTimes.size() > 0 )
	{
		if( !ExportNodeAnimations() )
			return false;
	}

	// export the skeletal bindings and sockets
	ip->ProgressUpdate( 90, FALSE, GetString(IDS_PROGRESS_SKELETALBINDINGS) );
	if( !options->appendAnimation )
	{
		if( !ExportOnLoadCmds() )
			return false;

		// export the texture binding info
		if( !ExportToolsInfo() )
			return false;
	}

	return true;
}


// build up the tree and piece lists recursively
ExpNode* ModelExporter::EnumNode( INode* maxNode, ExpNode* parent )
{
	assert( maxNode );
	if( !maxNode )
		return NULL;

	ExpNode* expNode = new ExpNode();
	if( !expNode )
		return NULL;

	expNode->node = maxNode;
	expNode->parent = parent;
	expNode->pieceType = DeterminePieceType( maxNode );
	expNode->xform = maxNode->GetObjTMBeforeWSM( startTime );

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
	for( int i = 0; i < maxNode->NumberOfChildren(); i++ )
	{
		ExpNode* childNode = EnumNode( maxNode->GetChildNode( i ), expNode );
		if( !childNode )
			return NULL;
		expNode->children.push_back( childNode );
	}

	return expNode;
}


// locate a node within a subtree, returns NULL if nothing is found
ExpNode* ModelExporter::FindNode( INode* maxNode, ExpNode* start )
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
	if( curNode->node == maxNode )
		return curNode;

	// recurse into children
	ExpNode* foundNode;
	for( unsigned i = 0; i < curNode->children.size(); i++ )
	{
		if( foundNode = FindNode( maxNode, curNode->children[i] ) )
			return foundNode;
	}

	// nothing found
	return NULL;
}


// recursively write the scene link hierarchy to the file
bool ModelExporter::ExportTree( ExpNode* expNode )
{
	BailOutIfFalse( ltaWriter.IsValid() );
	BailOutIfFalse( expNode->node );

	// if this is the root node, write out the header for the hierarchy
	if( !expNode->parent )
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
		INode* node = expNode->node;
		BailOutIfFalse( node );

		ltaWriter.BeginNode();	// push transform node
		ltaWriter.Write( "transform" );
		ltaWriter.Write( node->GetName(), true );

		// get initial transform and adjust it for the export scale factor
		LTMatrix mat = ConvertMatrix( expNode->xform );
		LTVector t;
		mat.GetTranslation( t );
		t *= options->scale;
		mat.SetTranslation( t );

		// write out the initial transform
		if( !WriteMatrix( mat ) )
			return false;

		// check if any of the children are a valid link
		bool validChild = false;
		for( unsigned curChild = 0; curChild < expNode->children.size(); curChild++ )
		{
			if( expNode->children[curChild]->isLinkNode )
			{
				validChild = true;
				break;
			}
		}

		// if there is a valid child, create a children node and recurse
		if( validChild )
		{
			ltaWriter.BeginNode();	// push children node
			ltaWriter.Write( "children" );
			ltaWriter.BeginNode();	// push children list node

			for( unsigned curChild = 0; curChild < expNode->children.size(); curChild++ )
			{
				if( expNode->children[curChild]->isLinkNode )
					ExportTree( expNode->children[curChild] );
			}

			ltaWriter.EndNode();	// pop children list node
			ltaWriter.EndNode();	// pop children node
		}

		ltaWriter.EndNode();	// pop transform node
	}

	// if this is the root node, polish off the header
	if( !expNode->parent )
	{
		ltaWriter.EndNode();	// pop children list node
		ltaWriter.EndNode();	// pop children node
		ltaWriter.EndNode();	// pop hierarchy node
	}

	return true;
}


// tag any forced nodes or socket nodes and their parents
bool ModelExporter::TagLinkNodes( void )
{
	// tag forced nodes and their parents
	for( unsigned i = 0; i < forcedNodes.size(); i++ )
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


// export vertex animations
bool ModelExporter::ExportVertexAnimations( void )
{
	BailOutIfFalse( ltaWriter.IsValid() );

	// build the list of frame times
	list<int> frameTimes;	// max frame times
	int start = ip->GetAnimRange().Start() / GetTicksPerFrame() + 1;
	int end = ip->GetAnimRange().End() / GetTicksPerFrame() + 1;
	int frames = (end - start) + 1;
	for( int i = 0; i < frames; i++ )
	{
		int curTime = start + (i * GetTicksPerFrame() + 1);
		frameTimes.push_back( curTime );
	}

	// start the animset
	char tmpStr[256];
	sprintf( tmpStr, "animset \"%sVA\"", options->animationName );
	ltaWriter.BeginNode();	// push animset node
	ltaWriter.Write( tmpStr );

	// write out the frame times to the lta
	WriteFrameList( frameTimes );

	// start the anims node
	ltaWriter.BeginNode();	// push anims node
	ltaWriter.Write( "anims" );
	ltaWriter.BeginNode();	// push anims list node

	// export the vertex animation for each piece
	for( unsigned curPiece = 0; curPiece < vertexPieces.size(); curPiece++ )
	{
		if( !ExportVertexAnimation( vertexPieces[curPiece], frameTimes ) )
			return false;
	}

	// clean up
	ltaWriter.EndNode();	// pop anims list node
	ltaWriter.EndNode();	// pop anims node
	ltaWriter.EndNode();	// pop animset node

	return true;
}

// export the vertex animation for a piece
bool ModelExporter::ExportVertexAnimation( ExpNode* expNode, list<int>& frameTimes )
{
	char tmpStr[256];

	BailOutIfFalse( ltaWriter.IsValid() );

	INode* node = expNode->node;
	BailOutIfFalse( node );

	ltaWriter.BeginNode();	// push anim node
	ltaWriter.Write( "anim" );

	ltaWriter.BeginNode();	// push parent node
	ltaWriter.Write( "parent" );
	ltaWriter.Write( node->GetName(), true );
	ltaWriter.EndNode();	// pop parent node

	int numFrames = frameTimes.size();

	ltaWriter.BeginNode();	// push frames node
	ltaWriter.Write( "frames" );
	ltaWriter.BeginNode();	// push vertex node
	ltaWriter.Write("vertex" );
	ltaWriter.BeginNode();	// push vertex list node

	// print out the verts for each frame
	for( list<int>::iterator it = frameTimes.begin(); it != frameTimes.end(); it++ )
	{
		TimeValue curTime = *it;

		Object* obj = node->EvalWorldState( curTime ).obj;
		TriObject* tri = (TriObject*)obj->ConvertToType( curTime, triObjectClassID );
		bool deleteTriObj = (tri != obj);	// must delete tri if it is a new object
		BailOutIfFalse( tri );

		Matrix3 xform = node->GetObjTMBeforeWSM( curTime );
		xform = ConvertMatrixSpace( xform );
		float scale = options->scale;

		Mesh& mesh = tri->GetMesh();

		int numVerts = mesh.getNumVerts();

		ltaWriter.BeginNode();	// push frame vertex list node

		Point3 pos;

		for( int i = 0; i < numVerts; i++ )
		{
			pos = xform * mesh.verts[i];
			ltaWriter.BeginNode();	// push vertex position node
			sprintf( tmpStr, "%f %f %f", pos.x * scale, pos.z * scale, pos.y * scale );
			ltaWriter.Write( tmpStr );
			ltaWriter.EndNode();	// pop vertex position node
		}

		ltaWriter.EndNode();	// pop frame vertex list node

		// delete tri if it is a new object
		if( deleteTriObj )
			delete tri;
	}

	ltaWriter.EndNode();	// pop vertex list node
	ltaWriter.EndNode();	// pop vertex node
	ltaWriter.EndNode();	// pop frames node
	ltaWriter.EndNode();	// pop anim node

	return true;
}


// export animated positions of nodes
bool ModelExporter::ExportNodeAnimations( void )
{
	BailOutIfFalse( ltaWriter.IsValid() );

	ltaWriter.BeginNode();	// push animset node
	ltaWriter.Write( "animset" );
	ltaWriter.Write( options->animationName, true );

	// write out the list of keyframe times
	if( !WriteFrameList( keyTimes ) )
		return false;

	ltaWriter.BeginNode();	// push anims node
	ltaWriter.Write( "anims" );
	ltaWriter.BeginNode();	// push anims list node

	// recurse and write out animation data
	if( !ExportNodeAnimation( root ) )
		return false;

	ltaWriter.EndNode();	// pop anims list node
	ltaWriter.EndNode();	// pop anims node
	ltaWriter.EndNode();	// pop animset node

	return true;
}


// export animation data for a node and recurse
bool ModelExporter::ExportNodeAnimation( ExpNode* expNode )
{
	BailOutIfFalse( ltaWriter.IsValid() );

	// stop recursing if this isn't a valid link node
	if( !expNode->isLinkNode )
		return true;

	// if this is the root node, don't write animation data, just recurse
	if( !expNode->parent )
	{
		for( unsigned i = 0; i < expNode->children.size(); i++ )
		{
			if( !ExportNodeAnimation( expNode->children[i] ) )
				return false;
		}
		return true;
	}

	INode* node = expNode->node;
	BailOutIfFalse( node );

	ltaWriter.BeginNode();	// push anim node
	ltaWriter.Write( "anim" );

	ltaWriter.BeginNode();	// push parent node
	ltaWriter.Write( "parent" );
	ltaWriter.Write( node->GetName(), true );
	ltaWriter.EndNode();	// pop parent node

	ltaWriter.BeginNode();	// push frames node
	ltaWriter.Write( "frames" );
	ltaWriter.BeginNode();	// push posquat node
	ltaWriter.Write( "posquat" );
	ltaWriter.BeginNode();	// push posquat list node

	INode* parent = node->GetParentNode();
	BailOutIfFalse( parent == expNode->parent->node );	// sanity check

	char tmpStr[256];

	for( list<int>::iterator it = keyTimes.begin(); it != keyTimes.end(); it++ )
	{
		int curTime = *it;

		LTMatrix mat = ConvertMatrix( node->GetObjTMBeforeWSM( curTime ) );

		// remove the parents portion of the transform
		if( parent && !parent->IsRootNode() )
			mat = (~ConvertMatrix( parent->GetObjTMBeforeWSM( curTime ) )) * mat;

		// retrieve and scale the translation
		LTVector pos;
		mat.GetTranslation( pos );
		pos *= options->scale;

		// get the orientation
		float quat[4];
		MatrixToQuaternion( mat, quat );

		ltaWriter.BeginNode();	// push current frame posquat node

		ltaWriter.BeginNode();	// push pos node
		sprintf( tmpStr, "%f %f %f", pos[0], pos[1], pos[2] );
		ltaWriter.Write( tmpStr );
		ltaWriter.EndNode();	// pop pos node

		ltaWriter.BeginNode();	// push quat node
		sprintf( tmpStr, "%f %f %f %f", quat[0], quat[1], quat[2], quat[3] );
		ltaWriter.Write( tmpStr );
		ltaWriter.EndNode();	// pop quat node

		ltaWriter.EndNode();	// pop current frame posquat node
	}

	ltaWriter.EndNode();	// pop posquat list node
	ltaWriter.EndNode();	// pop posquat node
	ltaWriter.EndNode();	// pop frames node
	ltaWriter.EndNode();	// pop anim node

	for( unsigned i = 0; i < expNode->children.size(); i++ )
	{
		if( !ExportNodeAnimation( expNode->children[i] ) )
			return false;
	}

	return true;
}


// write out a list of keyframes to the lta file
bool ModelExporter::WriteFrameList( list<int>& frameList )
{
	BailOutIfFalse( ltaWriter.IsValid() );

	ltaWriter.BeginNode();	// push outer keyframe node
	ltaWriter.Write( "keyframe" );
	ltaWriter.BeginNode();	// push inner keyframe node
	ltaWriter.Write( "keyframe" );
	ltaWriter.BeginNode();	// push times node
	ltaWriter.Write( "times" );
	ltaWriter.BeginNode();	// push times list node

	list<int>::iterator it;
	int numWritten = 0;
	char tmpStr[256];

	for( it = frameList.begin(); it != frameList.end(); it++ )
	{
		// insert a newline every five keyframes
		if( numWritten++ % 4 == 3 )
			ltaWriter.BreakLine();

		// convert the time to microseconds
		int time = *it;
		time = (int)((10.0f * time) / 48.0f);

		sprintf( tmpStr, "%d", time );
		ltaWriter.Write( tmpStr );
	}

	ltaWriter.EndNode();	// pop times list node
	ltaWriter.EndNode();	// pop times node
	ltaWriter.EndNode();	// pop inner keyframe node
	ltaWriter.EndNode();	// pop outer keyframe node

	return true;
}


// export a skeletally animated piece
bool ModelExporter::ExportSkeletalPiece( ExpNode* expNode )
{
	INode* node = expNode->node;
	assert( node );

	if( !node )
		return false;

	// grab the skin or physique modifier
	Object* obj = node->GetObjectRef();

	while( obj && obj->SuperClassID() == GEN_DERIVOB_CLASS_ID )
	{
		IDerivedObject* derObj = (IDerivedObject*)obj;

		for( int modStackIndex = 0; modStackIndex < derObj->NumModifiers(); modStackIndex++ )
		{
			Modifier* mod = derObj->GetModifier( modStackIndex );

			// found a physique modifier
			if( mod->ClassID() == Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) )
				return ExportSkeletalPiecePhysique( expNode, mod );

#ifndef USES_MAX3_SDK
			// found a skin modifier
			if( mod->ClassID() == SKIN_CLASSID )
				return ExportSkeletalPieceSkin( expNode, mod );
#endif
		}
	}

	return false;
}


// export a skeletally animated piece (using physique modifier)
bool ModelExporter::ExportSkeletalPiecePhysique( ExpNode* expNode, Modifier* mod )
{
	// double check the modifier type
	BailOutIfFalse( mod->ClassID() == Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) );

	INode* node = expNode->node;
	BailOutIfFalse( node );

	// get the physique export interface
	IPhysiqueExport* phyExport = (IPhysiqueExport*)mod->GetInterface( I_PHYEXPORT );
	BailOutIfFalse( phyExport );

	// get the physique export context interface (context is specific to a node, as
	// the modifier may be applied across several nodes)
	IPhyContextExport* phyContext = (IPhyContextExport*)phyExport->GetContextInterface( node );
	BailOutIfFalse( phyContext );

	phyContext->ConvertToRigid( TRUE );

	// get the nodes used for the skeletal deformation
	int numVerts = phyContext->GetNumberVertices();

	IPhyVertexExport* vert;				// physique base vertex interface
	IPhyBlendedRigidVertex* blendVert;	// physique rigid blended vertex interface
	IPhyRigidVertex* rigidVert;			// physique rigid vertex interface
	IPhyFloatingVertex* floatVert;		// physique floating bone vertex interface
	bool canDoFloatingVerts = false;	// true if this is not physique 2.x

	// check the physique version
	if( phyExport->Version() >= 300 )
		canDoFloatingVerts = true;

	set<INode*> localSkel;				// skeletal nodes used by this piece
	set<INode*>::iterator localSkelIt;	// iterator for skeletal nodes

	int curNode;

	// loop through verts adding bones to the skeletal nodes set as found
	for( int i = 0; i < numVerts; i++ )
	{
		vert = phyContext->GetVertexInterface( i );
		if( vert )
		{
			int type = vert->GetVertexType();
			switch( type )
			{
			case RIGID_BLENDED_TYPE:
				blendVert = (IPhyBlendedRigidVertex*)vert;
				for( curNode = 0; curNode < blendVert->GetNumberNodes(); curNode++ )
				{
					localSkel.insert( blendVert->GetNode( curNode ) );
				}
				break;
			case RIGID_TYPE:
				rigidVert = (IPhyRigidVertex*)vert;
				localSkel.insert( rigidVert->GetNode() );
				break;
			default:
				break;
			}
		}
		phyContext->ReleaseVertexInterface( vert );

		// floating vert interfaces only exist in Physique 3 and higher
		if( canDoFloatingVerts )
		{
			floatVert = (IPhyFloatingVertex*)phyContext->GetFloatingVertexInterface( i );
			if( floatVert )
			{
				for( curNode = 0; curNode < floatVert->GetNumberNodes(); curNode++ )
				{
					localSkel.insert( floatVert->GetNode( curNode ) );
				}
			}
			phyContext->ReleaseVertexInterface( floatVert );
		}
	}

	// get the initial transforms for the nodes in the hierarchy and all parent nodes
	for( localSkelIt = localSkel.begin(); localSkelIt != localSkel.end(); localSkelIt++ )
	{
		Matrix3 curMat;
		ExpNode* curExpNode = FindNode( *localSkelIt );
		BailOutIfFalse( curExpNode );

		// traverse all parents getting initial transform
		while( curExpNode )
		{
			if( MATRIX_RETURNED == phyExport->GetInitNodeTM( curExpNode->node, curMat ) )
				curExpNode->xform = curMat;
			else
				break;

			curExpNode->isLinkNode = true;
			curExpNode = curExpNode->parent;
		}
		// continue up chain for links that aren't bound to this modifier
		while( curExpNode )
		{
			curExpNode->isLinkNode = true;
			curExpNode = curExpNode->parent;
		}
	}

	// only export the shape if append animations if off
	bool retVal = true;

	if( !options->appendAnimation )
	{
		// disable the physique modifier (a better approach might be needed for complex modifier stacks)
		mod->DisableMod();

		Object* obj = node->EvalWorldState( startTime ).obj;
		BailOutIfFalse( obj );

		// get a triangle mesh object from the node
		TriObject* triObj = (TriObject*)obj->ConvertToType( startTime, triObjectClassID );
		bool deleteTriObj = (triObj != obj);	// must delete triObj if it is a new object
		BailOutIfFalse( triObj );

		// get binding transform of this piece
		Matrix3 xform;
		phyExport->GetInitNodeTM( node, xform );
		xform = ConvertMatrixSpace( xform );

		// get mesh info
		Mesh& mesh = triObj->GetMesh();

		retVal = ExportPieceMesh( expNode, mesh, xform );

		// delete the mesh object if it isn't the same as the node object
		if( deleteTriObj )
			triObj->DeleteThis();

		// enable the physique modifer
		mod->EnableMod();
	}

	return retVal;
}


#ifndef USES_MAX3_SDK
// export a skeletally animated piece (using skin modifier)
bool ModelExporter::ExportSkeletalPieceSkin( ExpNode* expNode, Modifier* mod )
{
	// double check the modifier type
	BailOutIfFalse( mod->ClassID() == SKIN_CLASSID );

	INode* node = expNode->node;
	BailOutIfFalse( node );

	// get the skin interface
	ISkin* skinExport = (ISkin*)mod->GetInterface( I_SKIN );
	BailOutIfFalse( skinExport );

	// get the skin export context interface (context is specific to a node, as
	// the modifier may be applied across several nodes)
	ISkinContextData* skinContext = (ISkinContextData*)skinExport->GetContextInterface( node );
	BailOutIfFalse( skinContext );

	int numVerts = skinContext->GetNumPoints();

	set<INode*> localSkel;				// skeletal nodes used by this piece
	set<INode*>::iterator localSkelIt;	// iterator for skeletal nodes

	int curNode;
	int curBoneIndex;

	// loop through verts adding bones to the skeletal nodes set as found
	for( int i = 0; i < numVerts; i++ )
	{
		for( curNode = 0; curNode < skinContext->GetNumAssignedBones( i ); curNode++ )
		{
			curBoneIndex = skinContext->GetAssignedBone( i, curNode );
			localSkel.insert( skinExport->GetBone( curBoneIndex ) );
		}
	}

	// get the initial transforms for the nodes in the hierarchy and all parent nodes
	for( localSkelIt = localSkel.begin(); localSkelIt != localSkel.end(); localSkelIt++ )
	{
		Matrix3 curMat;
		ExpNode* curExpNode = FindNode( *localSkelIt );
		BailOutIfFalse( curExpNode );

		// traverse all parents getting initial transform
		while( curExpNode )
		{
			if( SKIN_OK == skinExport->GetBoneInitTM( curExpNode->node, curMat ) )
				curExpNode->xform = curMat;
			else
				break;

			curExpNode->isLinkNode = true;
			curExpNode = curExpNode->parent;
		}
		// continue up chain for links that aren't bound to this modifier
		while( curExpNode )
		{
			curExpNode->isLinkNode = true;
			curExpNode = curExpNode->parent;
		}
	}

	bool retVal = true;

	// only export the shape if append animations if off
	if( !options->appendAnimation )
	{
		// disable the skin modifier (a better approach might be needed for complex modifier stacks)
		mod->DisableMod();

		Object* obj = node->EvalWorldState( startTime ).obj;
		BailOutIfFalse( obj );

		// get a triangle mesh object from the node
		TriObject* triObj = (TriObject*)obj->ConvertToType( startTime, triObjectClassID );
		bool deleteTriObj = (triObj != obj);	// must delete triObj if it is a new object
		BailOutIfFalse( triObj );

		// get transform
		Matrix3 xform;
		int skinTMRes = skinExport->GetSkinInitTM( node, xform );
		BailOutIfFalse( skinTMRes == SKIN_OK );
		xform = ConvertMatrixSpace( xform );

		// get mesh info
		Mesh& mesh = triObj->GetMesh();

		retVal = ExportPieceMesh( expNode, mesh, xform );

		// delete the mesh object if it isn't the same as the node object
		if( deleteTriObj )
			triObj->DeleteThis();

		// enable the skin modifier
		mod->EnableMod();
	}

	return retVal;
}
#endif


// export a vertex animated piece
bool ModelExporter::ExportVertexPiece( ExpNode* expNode )
{
	bool retVal = true;

	// only export the piece if append animations is off
	if( !options->appendAnimation )
	{
		INode* node = expNode->node;
		BailOutIfFalse( node );

		Object* obj = node->EvalWorldState( startTime ).obj;
		BailOutIfFalse( obj );

		// get a triangle mesh object from the node
		TriObject* triObj = (TriObject*)obj->ConvertToType( startTime, triObjectClassID );
		bool deleteTriObj = (triObj != obj);	// must delete triObj if it is a new object
		BailOutIfFalse( triObj );

		// get transform
		Matrix3 xform = ConvertMatrixSpace( node->GetObjTMBeforeWSM( startTime ) );

		// get mesh info
		Mesh& mesh = triObj->GetMesh();

		retVal = ExportPieceMesh( expNode, mesh, xform );

		// delete the mesh object if it isn't the same as the node object
		if( deleteTriObj )
			triObj->DeleteThis();
	}

	// vertex pieces must be in the node hierarchy for now.  this shouldn't be
	// required, so this code may be removed in the future
	ExpNode* curExpNode = expNode;
	while( curExpNode )
	{
		curExpNode->isLinkNode = true;
		curExpNode = curExpNode->parent;
	}

	return retVal;
}


// export a shape given a mesh and a transform
bool ModelExporter::ExportPieceMesh( ExpNode* expNode, Mesh& mesh, const Matrix3& xform )
{	
	char tmpStr[256];
	Point3 curVert;
	VertColor curVertColor;
	UVVert curVertUV;

	INode* node = expNode->node;
	BailOutIfFalse( node );

	BailOutIfFalse( ltaWriter.IsValid() );

	int numVerts = mesh.getNumVerts();
	int numTris = mesh.getNumFaces();
	int numCVerts = mesh.getNumVertCol();
	int numTVerts = mesh.getNumTVerts();

	ltaWriter.BeginNode();	// push shape node
	ltaWriter.Write( "shape" );
	ltaWriter.Write( node->GetName(), true );

	ltaWriter.BeginNode();	// push parent node
	ltaWriter.Write( "parent" );
	if( expNode->pieceType == Vertex )
		ltaWriter.Write( node->GetName(), true );	// vertex animated pieces are parented to themselves...
	else
		ltaWriter.Write( "Scene Root", true );		// ...but skeletal pieces are parented to the scene root
	ltaWriter.EndNode();	// pop parent node

	ltaWriter.BeginNode();	// push geometry node
	ltaWriter.Write( "geometry" );

	ltaWriter.BeginNode();	// push mesh node
	ltaWriter.Write( "mesh" );
	ltaWriter.Write( node->GetName(), true );

	// print out vertex info
	if( numVerts > 0 )
	{
		ltaWriter.BeginNode();	// push vertex parent node
		ltaWriter.Write( "vertex" );

		ltaWriter.BeginNode();	// push vertex list node

		// write the vertex data
		for( int i = 0; i < numVerts; i++ )
		{
			ltaWriter.BeginNode();	// push current vertex node
			curVert = xform * mesh.verts[i];
			sprintf( tmpStr, "%f %f %f", curVert.x * options->scale, curVert.z * options->scale, curVert.y * options->scale );
			ltaWriter.Write( tmpStr );
			ltaWriter.EndNode();	// pop current vertex node
		}

		ltaWriter.EndNode();	// pop vertex list node
		ltaWriter.EndNode();	// pop vertex parent node
	}

	// print out vertex color info
	if( numCVerts > 0 )
	{
		ltaWriter.BeginNode();	// push vertex color parent node
		ltaWriter.Write( "colors" );

		ltaWriter.BeginNode();	// push vertex color list node

		for( int i = 0; i < numCVerts; i++ )
		{
			ltaWriter.BeginNode();	// push current vertex color node
			curVertColor = mesh.vertCol[i];
			sprintf( tmpStr, "%f %f %f", curVertColor.x, curVertColor.y, curVertColor.z );
			ltaWriter.Write( tmpStr );
			ltaWriter.EndNode();	// pop current vertex color node
		}

		ltaWriter.EndNode();	// pop vertex color list node
		ltaWriter.EndNode();	// pop vertex color parent node
	}

	// print out vertex uv info
	if( numTVerts > 0 )
	{
		ltaWriter.BeginNode();	// push vertex uv parent node
		ltaWriter.Write( "uvs" );

		ltaWriter.BeginNode();	// push vertex uv list node

		for( int i = 0; i < numTVerts; i++ )
		{
			ltaWriter.BeginNode();	// push current vertex uv node
			curVertUV = mesh.getTVert( i );
			sprintf( tmpStr, "%f %f", curVertUV.x, 1.0f - curVertUV.y );
			ltaWriter.Write( tmpStr );
			ltaWriter.EndNode();	// pop current vertex uv node
		}

		ltaWriter.EndNode();	// pop vertex uv list node
		ltaWriter.EndNode();	// pop vertex uv parent node
	}

	// face winding order
	int vertIndex0, vertIndex1, vertIndex2;
	if( xform.Parity() )
	{
		vertIndex0 = 0;
		vertIndex1 = 1;
		vertIndex2 = 2;
	}
	else
	{
		vertIndex0 = 2;
		vertIndex1 = 1;
		vertIndex2 = 0;
	}

	// print out faces
	if( numTris > 0 )
	{
		ltaWriter.BeginNode();	// push face parent node
		ltaWriter.Write( "tri-fs" );
		
		ltaWriter.BeginNode();	// push face list node

		for( int i = 0; i < numTris; i++ )
		{
			if( !(mesh.faces[i].flags & FACE_HIDDEN) )
			{
				sprintf( tmpStr, "%d %d %d", mesh.faces[i].v[vertIndex0], mesh.faces[i].v[vertIndex1], mesh.faces[i].v[vertIndex2] );
				ltaWriter.Write( tmpStr );
				ltaWriter.BreakLine();
			}
		}

		ltaWriter.EndNode();	// pop face list node
		ltaWriter.EndNode();	// pop face parent node
	}

	// print out face texture info
	if( numTVerts > 0 )
	{
		ltaWriter.BeginNode();	// push face texture parent node
		ltaWriter.Write( "tex-fs" );

		ltaWriter.BeginNode();	// push face texture list node

		for( int i = 0; i < numTris; i++ )
		{
			sprintf( tmpStr, "%d %d %d", mesh.tvFace[i].t[vertIndex0], mesh.tvFace[i].t[vertIndex1], mesh.tvFace[i].t[vertIndex2] );
			ltaWriter.Write( tmpStr );
			ltaWriter.BreakLine();
		}

		ltaWriter.EndNode();	// pop face texture list node
		ltaWriter.EndNode();	// pop face texture parent node
	}

	// print out face color info
	if( numCVerts > 0 )
	{
		ltaWriter.BeginNode();	// push face color parent node
		ltaWriter.Write( "col-fs" );

		ltaWriter.BeginNode();	// push face color list node

		for( int i = 0; i < numTris; i++ )
		{
			sprintf( tmpStr, "%d %d %d", mesh.vcFace[i].t[vertIndex0], mesh.vcFace[i].t[vertIndex1], mesh.vcFace[i].t[vertIndex2] );
			ltaWriter.Write( tmpStr );
			ltaWriter.BreakLine();
		}

		ltaWriter.EndNode();	// pop face color list node
		ltaWriter.EndNode();	// pop face color parent node
	}

	ltaWriter.EndNode();	// pop mesh node
	ltaWriter.EndNode();	// pop geometry node

	// check for texture user properties
	int maximumIndex = -1;
	int textureIndices[4];

	for( int i = 0; i < 4; i++ )
	{
		sprintf( tmpStr, "Texture%d", i );
		if( node->GetUserPropInt( tmpStr, textureIndices[i] ) )
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

		if( !GetTextureInfo( node, textureIndices[0] ) )
			return false;
	}

	// check for renderstyle user property
	int renderStyleIndex;
	if( node->GetUserPropInt( "RenderStyle", renderStyleIndex ) )
	{
		ltaWriter.BeginNode();	// push renderstyle-index node
		ltaWriter.Write( "renderstyle-index" );
		sprintf( tmpStr, "%d", renderStyleIndex );
		ltaWriter.Write( tmpStr );
		ltaWriter.EndNode();	// pop renderstyle-index node
	}

	ltaWriter.EndNode();	// pop shape node

	return true;
}


// export tools specific info
bool ModelExporter::ExportToolsInfo( void )
{
	char tmpStr[255];

	BailOutIfFalse( ltaWriter.IsValid() );

	ltaWriter.BeginNode();	// push tools-info node
	ltaWriter.Write( "tools-info" );
	ltaWriter.BeginNode();	// push tools-info list node

	// export texture bindings if there are any
	if( texturePaths.size() > 0 )
	{
		ltaWriter.BeginNode();	// push texture-bindings node
		ltaWriter.Write( "texture-bindings" );
		ltaWriter.BeginNode();	// push texture-bindings list node

		for( unsigned i = 0; i < texturePaths.size(); i++ )
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
		ltaWriter.EndNode();	// pop texture-bidings node
	}

	ltaWriter.EndNode();	// pop tools-info list node
	ltaWriter.EndNode();	// pop tools-info node

	return true;
}


// export the on-load-cmds node
bool ModelExporter::ExportOnLoadCmds( void )
{
	BailOutIfFalse( ltaWriter.IsValid() );

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
bool ModelExporter::ExportSockets( void )
{
	BailOutIfFalse( ltaWriter.IsValid() );

	// nothing to export
	if( socketNodes.size() <= 0 )
		return true;

	ltaWriter.BeginNode();	// push add-sockets node
	ltaWriter.Write( "add-sockets" );

	char tmpStr[256];

	for( unsigned i = 0; i < socketNodes.size(); i++ )
	{
		// get the relevant nodes
		ExpNode* curExpNode = socketNodes[i];
		INode* curNode = curExpNode->node;
		ExpNode* parentExpNode = curExpNode->parent;
		INode* parentNode = parentExpNode->node;

		LTMatrix mat = ConvertMatrix( curNode->GetObjTMBeforeWSM( startTime ) );

		// remove the parents portion of the transform
		if( parentNode && !parentNode->IsRootNode() )
			mat = (~ConvertMatrix( parentNode->GetObjTMBeforeWSM( startTime ) )) * mat;

		// retrieve and scale the translation
		LTVector pos;
		mat.GetTranslation( pos );
		pos *= options->scale;

		// get the orientation
		float quat[4];
		MatrixToQuaternion( mat, quat );

		ltaWriter.BeginNode();	// push socket node
		ltaWriter.Write( "socket" );
		ltaWriter.Write( curNode->GetName(), true );

		ltaWriter.BeginNode();	// push parent node
		ltaWriter.Write( "parent" );
		ltaWriter.Write( parentNode->GetName(), true );
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

		ltaWriter.EndNode();	// pop socket node
	}

	ltaWriter.EndNode();	// pop add-scokets node

	return true;
}


// export the skeletal bindings
bool ModelExporter::ExportSkeletalBindings( void )
{
	ExpNode* curPiece;
	INode* curNode;

	for( unsigned i = 0; i < skeletalPieces.size(); i++ )
	{
		curPiece = skeletalPieces[i];
		curNode = curPiece->node;
		BailOutIfFalse( curNode );

		// grab the skin or physique modifier
		Object* obj = curNode->GetObjectRef();
		bool foundModifier = false;
		bool retVal;

		while( !foundModifier && obj && obj->SuperClassID() == GEN_DERIVOB_CLASS_ID )
		{
			IDerivedObject* derObj = (IDerivedObject*)obj;

			for( int modStackIndex = 0; modStackIndex < derObj->NumModifiers(); modStackIndex++ )
			{
				Modifier* mod = derObj->GetModifier( modStackIndex );

				// found a physique modifier
				if( mod->ClassID() == Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) )
				{
					retVal = ExportSkeletalBindingPhysique( curPiece, mod );
					foundModifier = true;
					break;
				}
	#ifndef USES_MAX3_SDK
				// found a skin modifier
				else if( mod->ClassID() == SKIN_CLASSID )
				{
					retVal = ExportSkeletalBindingSkin( curPiece, mod );
					foundModifier = true;
					break;
				}
	#endif
			}

			// check for error
			if( foundModifier && !retVal )
				return false;
		}
	}

	return true;
}


// export the skeletal bindings (using physique modifier)
bool ModelExporter::ExportSkeletalBindingPhysique( ExpNode* expNode, Modifier* mod )
{
	BailOutIfFalse( ltaWriter.IsValid() );

	// double check the modifier type
	BailOutIfFalse( mod->ClassID() == Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) );

	ltaWriter.BeginNode();	// push add-deformer node
	ltaWriter.Write( "add-deformer" );
	ltaWriter.BeginNode();	// push skel-deformer node
	ltaWriter.Write( "skel-deformer" );

	ltaWriter.BeginNode();	// push target node
	ltaWriter.Write( "target" );
	ltaWriter.Write( expNode->node->GetName(), true );
	ltaWriter.EndNode();	// pop target node

	// get the physique export interface
	IPhysiqueExport* phyExport = (IPhysiqueExport*)mod->GetInterface( I_PHYEXPORT );
	BailOutIfFalse( phyExport );

	// get the physique export context interface (context is specific to a node, as
	// the modifier may be applied across several nodes)
	IPhyContextExport* phyContext = (IPhyContextExport*)phyExport->GetContextInterface( expNode->node );
	BailOutIfFalse( phyContext );

	phyContext->ConvertToRigid( TRUE );

	// get the nodes used for the skeletal deformation
	int numVerts = phyContext->GetNumberVertices();

	IPhyVertexExport* vert;				// physique base vertex interface
	IPhyBlendedRigidVertex* blendVert;	// physique rigid blended vertex interface
	IPhyRigidVertex* rigidVert;			// physique rigid vertex interface
	IPhyFloatingVertex* floatVert;		// physique floating vertex interface
	float floatingTmpWeight;			// temporary weight for floating bones
	bool canDoFloatingVerts = false;	// true if this is not physique 2.x
	

	// check the physique version
	if( phyExport->Version() >= 300 )
		canDoFloatingVerts = true;

	set<INode*> localSkel;				// skeletal nodes used by this piece
	set<INode*>::iterator localSkelIt;	// iterator for skeletal nodes

	int curNode;

	// loop through verts adding bones to the skeletal nodes set as found
	for( int i = 0; i < numVerts; i++ )
	{
		vert = phyContext->GetVertexInterface( i );
		if( vert )
		{
			int type = vert->GetVertexType();
			switch( type )
			{
			case RIGID_BLENDED_TYPE:
				blendVert = (IPhyBlendedRigidVertex*)vert;
				for( curNode = 0; curNode < blendVert->GetNumberNodes(); curNode++ )
				{
					// skip bones that don't have enough influence
					if( blendVert->GetWeight( curNode ) < minimumWeight )
						continue;
					localSkel.insert( blendVert->GetNode( curNode ) );
				}
				break;
			case RIGID_TYPE:
				rigidVert = (IPhyRigidVertex*)vert;
				localSkel.insert( rigidVert->GetNode() );
				break;
			default:
				break;
			}
		}
		phyContext->ReleaseVertexInterface( vert );

		if( canDoFloatingVerts )
		{
			floatVert = (IPhyFloatingVertex*)phyContext->GetFloatingVertexInterface( i );
			if( floatVert )
			{
				for( curNode = 0; curNode < floatVert->GetNumberNodes(); curNode++ )
				{
					// skip bones that don't have enough influence
					if( floatVert->GetWeight( curNode, floatingTmpWeight ) < minimumWeight )
						continue;
					localSkel.insert( floatVert->GetNode( curNode ) );
				}
				phyContext->ReleaseVertexInterface( floatVert );
			}
		}
	}

	// convert node set to node vector
	vector<INode*> skelVec;				// skeletal nodes used by this piece
	vector<INode*>::iterator skelVecIt;	// iterator for skeletal nodes
	skelVec.reserve( localSkel.size() );
	for( localSkelIt = localSkel.begin(); localSkelIt != localSkel.end(); localSkelIt++ )
	{
		skelVec.push_back( *localSkelIt );
	}

	// print out bone list
	ltaWriter.BeginNode();	// push influences node
	ltaWriter.Write( "influences" );
	ltaWriter.BeginNode();	// push bone list node
	for( skelVecIt = skelVec.begin(); skelVecIt != skelVec.end(); skelVecIt++ )
	{
		ltaWriter.Write( (*skelVecIt)->GetName(), true );
		ltaWriter.BreakLine();
	}
	ltaWriter.EndNode();	// pop bone list node
	ltaWriter.EndNode();	// pop influences node

	// print out influence list
	ltaWriter.BeginNode();	// push weightsets node
	ltaWriter.Write( "weightsets" );
	ltaWriter.BeginNode();	// push weightset list node

	int numBones = skelVec.size();
	INode* curBone;
	float curWeight;
	float totalWeight;
	char tmpStr[256];

	// an array containing the influence of each bone on a vert
	float* weights = NULL;
	if( numBones )
		weights = new float[numBones];

	// get the influences for each vert
	for( i = 0; i < numVerts; i++ )
	{
		// initialize the weight table
		for( int j = 0; j < numBones; j++ )
		{
			weights[j] = 0.0f;
		}
		totalWeight = 0.0f;

		vert = phyContext->GetVertexInterface( i );
		if( vert )
		{
			int type = vert->GetVertexType();
			switch( type )
			{
			case RIGID_BLENDED_TYPE:
				blendVert = (IPhyBlendedRigidVertex*)vert;
				// add the influences for this vert to the weight array
				for( j = 0; j < blendVert->GetNumberNodes(); j++ )
				{
					curBone = blendVert->GetNode( j );
					curWeight = blendVert->GetWeight( j );
					// skip bones without enough influence
					if( curWeight < minimumWeight )
						continue;
					if( skelVec.end() == (skelVecIt = std::find( skelVec.begin(), skelVec.end(), curBone )) )
					{
						assert( 0 );	// couldn't find the referenced bone
						return false;
					}
					weights[skelVecIt-skelVec.begin()] += curWeight;
					totalWeight += curWeight;
				}
				break;
			case RIGID_TYPE:
				rigidVert = (IPhyRigidVertex*)vert;
				curBone = rigidVert->GetNode();
				if( skelVec.end() == (skelVecIt = std::find( skelVec.begin(), skelVec.end(), curBone )) )
				{
					assert( 0 );	// couldn't find the referenced bone
					return false;
				}
				weights[skelVecIt-skelVec.begin()] += 1.0f;
				totalWeight += 1.0f;
				break;
			default:
				break;
			}
		}
		phyContext->ReleaseVertexInterface( vert );

		// this code only works in Physique 3 and higher
		if( canDoFloatingVerts )
		{
			floatVert = (IPhyFloatingVertex*)phyContext->GetFloatingVertexInterface( i );
			if( floatVert )
			{
				for( j = 0; j < floatVert->GetNumberNodes(); j++ )
				{
					curBone = floatVert->GetNode( j );
					curWeight = floatVert->GetWeight( j, floatingTmpWeight );
					// skip bones without enough influence
					if( curWeight < minimumWeight )
						continue;
					if( skelVec.end() == (skelVecIt = std::find( skelVec.begin(), skelVec.end(), curBone )) )
					{
						assert( 0 );	// couldn't find the referenced bone
						return false;
					}
					weights[skelVecIt-skelVec.begin()] += curWeight;
					totalWeight += curWeight;
				}
			}
			phyContext->ReleaseVertexInterface( floatVert );
		}

		ltaWriter.BeginNode();	// push the weightset node

		// this shouldn't happen, but don't want a divide by zero
		if( totalWeight < minimumWeight )
			totalWeight = minimumWeight;

		// write out the influences on this vert
		for( j = 0; j < numBones; j++ )
		{
			if( weights[j] > minimumWeight )
			{
				sprintf( tmpStr, "%d %f", j, weights[j]/totalWeight );
				ltaWriter.Write( tmpStr );
			}
		}

		ltaWriter.EndNode();	// pop the weightset node
	}

	// delete the weight array
	delete [] weights;

	ltaWriter.EndNode();	// pop weightset list node
	ltaWriter.EndNode();	// pop weightsets node

	// wrap up the deformer
	ltaWriter.EndNode();	// pop skel-deformer node
	ltaWriter.EndNode();	// pop add-deformer node

	return true;
}


#ifndef USES_MAX3_SDK
// export the skeletal bindings (using skin modifier)
bool ModelExporter::ExportSkeletalBindingSkin( ExpNode* expNode, Modifier* mod )
{
	BailOutIfFalse( ltaWriter.IsValid() );

	// double check the modifier type
	BailOutIfFalse( mod->ClassID() == SKIN_CLASSID );

	ltaWriter.BeginNode();	// push add-deformer node
	ltaWriter.Write( "add-deformer" );
	ltaWriter.BeginNode();	// push skel-deformer node
	ltaWriter.Write( "skel-deformer" );

	ltaWriter.BeginNode();	// push target node
	ltaWriter.Write( "target" );
	ltaWriter.Write( expNode->node->GetName(), true );
	ltaWriter.EndNode();	// pop target node

	// get the skin export interface
	ISkin* skinExport = (ISkin*)mod->GetInterface( I_SKIN );
	BailOutIfFalse( skinExport );

	// get the skin export context interface (context is specific to a node, as
	// the modifier may be applied across several nodes)
	ISkinContextData* skinContext = (ISkinContextData*)skinExport->GetContextInterface( expNode->node );
	BailOutIfFalse( skinContext );

	int numVerts = skinContext->GetNumPoints();

	set<INode*> localSkel;				// skeletal nodes used by this piece
	set<INode*>::iterator localSkelIt;	// iterator for skeletal nodes

	int curNode;
	int curBoneIndex;

	// loop through verts adding bones to the skeletal nodes set as found
	for( int i = 0; i < numVerts; i++ )
	{
		for( curNode = 0; curNode < skinContext->GetNumAssignedBones( i ); curNode++ )
		{
			// skip bones that don't have enough influence
			if( skinContext->GetBoneWeight( i, curNode ) < minimumWeight )
				continue;
			curBoneIndex = skinContext->GetAssignedBone( i, curNode );
			localSkel.insert( skinExport->GetBone( curBoneIndex ) );
		}
	}

	// convert node set to node vector
	vector<INode*> skelVec;				// skeletal nodes used by this piece
	vector<INode*>::iterator skelVecIt;	// iterator for skeletal nodes
	skelVec.reserve( localSkel.size() );
	for( localSkelIt = localSkel.begin(); localSkelIt != localSkel.end(); localSkelIt++ )
	{
		skelVec.push_back( *localSkelIt );
	}

	// print out bone list
	ltaWriter.BeginNode();	// push influences node
	ltaWriter.Write( "influences" );
	ltaWriter.BeginNode();	// push bone list node
	for( skelVecIt = skelVec.begin(); skelVecIt != skelVec.end(); skelVecIt++ )
	{
		ltaWriter.Write( (*skelVecIt)->GetName(), true );
		ltaWriter.BreakLine();
	}
	ltaWriter.EndNode();	// pop bone list node
	ltaWriter.EndNode();	// pop influences node

	// print out influence list
	ltaWriter.BeginNode();	// push weightsets node
	ltaWriter.Write( "weightsets" );
	ltaWriter.BeginNode();	// push weightset list node

	int numBones = skelVec.size();
	INode* curBone;
	float curWeight;
	float totalWeight;
	char tmpStr[256];

	// an array containing the influence of each bone on a vert
	float* weights = NULL;
	if( numBones )
		weights = new float[numBones];

	// get the influences for each vert
	for( i = 0; i < numVerts; i++ )
	{
		for( int j = 0; j < numBones; j++ )
		{
			weights[j] = 0.0f;
		}
		totalWeight = 0.0f;

		for( curNode = 0; curNode < skinContext->GetNumAssignedBones( i ); curNode++ )
		{
			curWeight = skinContext->GetBoneWeight( i, curNode );
			// skip bones that don't have enough influence
			if( curWeight < minimumWeight )
				continue;
			curBoneIndex = skinContext->GetAssignedBone( i, curNode );
			curBone = skinExport->GetBone( curBoneIndex );
			if( skelVec.end() == (skelVecIt = std::find( skelVec.begin(), skelVec.end(), curBone )) )
			{
				assert( 0 );	// couldn't find the referenced bone
				return false;
			}
			weights[skelVecIt-skelVec.begin()] += curWeight;
			totalWeight += curWeight;
		}

		ltaWriter.BeginNode();	// push the weightset node

		// this shouldn't happen, but don't want a divide by zero
		if( totalWeight < minimumWeight )
			totalWeight = minimumWeight;

		// write out the influences on this vert
		for( j = 0; j < numBones; j++ )
		{
			if( weights[j] > minimumWeight )
			{
				sprintf( tmpStr, "%d %f", j, weights[j]/totalWeight );
				ltaWriter.Write( tmpStr );
			}
		}

		ltaWriter.EndNode();	// pop the weightset node
	}

	delete [] weights;

	ltaWriter.EndNode();	// pop weightset list node
	ltaWriter.EndNode();	// pop weightsets node

	// wrap up the deformer
	ltaWriter.EndNode();	// pop skel-deformer node
	ltaWriter.EndNode();	// pop add-deformer node

	return true;
}
#endif


// returns whether this piece is animated skeletally, vertex animated, or not exported at all
PieceType ModelExporter::DeterminePieceType( INode* node )
{
	assert( node );

	// make sure this is a mesh object
	if( IsNodeMesh( node ) && node->Renderable() && !node->IsHidden() )
	{
		// check if we are only exporting selected
		if( selectedOnly && !node->Selected() )
			return NotAPiece;

		// check for a vertex animated piece
		if( IsVertexAnimated( node ) )
			return Vertex;

		// check for a skeletally animated piece by looking for the appropriate modifiers
		Object* obj = node->GetObjectRef();

		while( obj && obj->SuperClassID() == GEN_DERIVOB_CLASS_ID )
		{
			IDerivedObject* derObj = (IDerivedObject*)obj;

			// drop through the modifier stack looking for skin or physique
			for( int modStackIndex = 0; modStackIndex < derObj->NumModifiers(); modStackIndex++ )
			{
				Modifier* mod = derObj->GetModifier( modStackIndex );

				if( mod->ClassID() == Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) )
					return Skeletal;

	#ifndef USES_MAX3_SDK
				if( mod->ClassID() == SKIN_CLASSID )
					return Skeletal;
	#endif
			}

			obj = derObj->GetObjRef();
		}
	}

	// check to see if this node (and its tree) should be forced to export
	if( IsForcedNode( node ) )
		return ForcedNode;

	// check to see if this node should be exported as a socket
	if( IsSocket( node ) )
		return Socket;

	return NotAPiece;
}


// returns true if the node can be converted to a triangle mesh object
bool ModelExporter::IsNodeMesh( INode* node )
{
	Object* obj = node->EvalWorldState( startTime ).obj;

	if( obj )
		return (TRUE == obj->CanConvertToType( triObjectClassID ));
	else
		return false;
}


// returns true if the node should be forced to export
bool ModelExporter::IsForcedNode( INode* node )
{
	if( !strncmp( node->GetName(), "n_", 2 ) )
		return true;

	// check the user properties
	BOOL val;
	if( node->GetUserPropBool( "ForcedNode", val ) && val )
		return true;

	return false;
}

// returns true if the node should be vertex animated
bool ModelExporter::IsVertexAnimated( INode* node )
{
	if( !strncmp( node->GetName(), "d_", 2 ) )
		return true;

	// check the user properties
	BOOL val;
	if( node->GetUserPropBool( "VertexAnimated", val ) && val )
		return true;

	return false;
}


// returns true if the node is a socket
bool ModelExporter::IsSocket( INode* node )
{
	if( !strncmp( node->GetName(), "s_", 2 ) )
		return true;

	// check the user properties
	BOOL val;
	if( node->GetUserPropBool( "Socket", val ) && val )
		return true;

	return false;
}


// finds the texture assignment for this face and adds it to the map index
bool ModelExporter::GetTextureInfo( INode* node, int index )
{
	BailOutIfFalse( node );

	// check to see if we should even bother
	if( !options->useTexInfo )
		return true;

	// get the material for this node
	Mtl* material = node->GetMtl();
	if( !material )
		return true;

	// make sure the material is a standard material
	if( material->ClassID() != Class_ID(DMTL_CLASS_ID,0) )
		return true;

	// get the diffuse map
	Texmap* tex = material->GetSubTexmap( ID_DI );
	if( !tex )
		return true;

	// make sure the map is a bitmap texmap
	if( tex->ClassID() != Class_ID(BMTEX_CLASS_ID,0) )
		return true;

	TCHAR* texName = ((BitmapTex*)tex)->GetMapName();

	char newdir[_MAX_PATH];
	char fulldir[_MAX_PATH];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char path[_MAX_PATH];

	strcpy( path, texName );
	int pathLen = strlen( path );
	int basePathLen = strlen( options->baseTexPath );
	for( int i = 0; i < pathLen; i++ )
	{
		if( isupper( path[i] ) )
			path[i] = tolower( path[i] );
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
		if( !strcmp( fulldir, options->baseTexPath ) )
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


// take a max matrix and convert it to lithtech space
Matrix3 ModelExporter::ConvertMatrixSpace( const Matrix3& original )
{
	Matrix3 result( 1 );

	// #burumal
	// {
	// result.RotateZ( MATH_PI );
	// }

	result = original * result;
	return result;
}


// take a max matrix and convert it to a lithtech matrix
LTMatrix ModelExporter::ConvertMatrix( const Matrix3& original )
{
	LTMatrix result;

	Matrix3 tmpMat = ConvertMatrixSpace( original );

	// decompose the matrix
	AffineParts affineParts;
	decomp_affine( tmpMat, &affineParts );
	
	// start with the rotation
	float q[4];
	q[0] = affineParts.q.x;
	q[1] = affineParts.q.z;
	q[2] = affineParts.q.y;
	q[3] = affineParts.q.w;
	QuaternionToMatrix( q, result );

	// add in the translation
	result.SetTranslation( affineParts.t.x, affineParts.t.z, affineParts.t.y );

	return result;
}


// convert a quaternion to a lithtech matrix
void ModelExporter::QuaternionToMatrix( float* quat, LTMatrix& mat )
{
	float s = 2.0f / ( (quat[0]*quat[0]) + (quat[1]*quat[1]) + (quat[2]*quat[2]) + (quat[3]*quat[3]) );

	float xs = quat[0] * s;
	float ys = quat[1] * s;
	float zs = quat[2] * s;

	float wx = quat[3] * xs;
	float wy = quat[3] * ys;
	float wz = quat[3] * zs;

	float xx = quat[0] * xs;
	float xy = quat[0] * ys;
	float xz = quat[0] * zs;

	float yy = quat[1] * ys;
	float yz = quat[1] * zs;

	float zz = quat[2] * zs;

	// fill in the matrix
	mat.m[0][0] = 1.0f - (yy + zz);
	mat.m[0][1] = xy - wz;
	mat.m[0][2] = xz + wy;

	mat.m[1][0] = xy + wz;
	mat.m[1][1] = 1.0f - (xx + zz);
	mat.m[1][2] = yz - wx;

	mat.m[2][0] = xz - wy;
	mat.m[2][1] = yz + wx;
	mat.m[2][2] = 1.0f - (xx + yy);

	mat.m[3][3] = 1.0f;
	mat.m[0][3] = mat.m[1][3] = mat.m[2][3] = mat.m[3][0] = mat.m[3][1] = mat.m[3][2] = 0.0f;
}


// convert a lithtech matrix to a quaternion
void ModelExporter::MatrixToQuaternion( const LTMatrix& mat, float* quat )
{
	const int qNext[3] = { QY, QZ, QX };

	float diag, s;
	int i, j, k;

	diag = mat.m[0][0] + mat.m[1][1] + mat.m[2][2] + 1.0f;

	if( diag < 0.00001f )
	{
		i = QX;
		if( mat.m[QY][QY] > mat.m[QX][QX] )
			i = QY;
		if( mat.m[QZ][QZ] > mat.m[i][i] )
			i = QZ;

		j = qNext[i];
		k = qNext[j];

		s = (float)sqrt( mat.m[i][i] - (mat.m[j][j] + mat.m[k][k]) + 1.0f );

		quat[i] = s * 0.5f;
		s = 0.5f / s;
		quat[QW] = (mat.m[k][j] - mat.m[j][k]) * s;
		quat[j] = (mat.m[j][i] + mat.m[i][j]) * s;
		quat[k] = (mat.m[k][i] + mat.m[i][k]) * s;

		return;
	}

	s = (float)sqrt( diag );

	quat[3] = s * 0.5f;
	s = 0.5f / s;

	quat[0] = (mat.m[2][1] - mat.m[1][2]) * s;
	quat[1] = (mat.m[0][2] - mat.m[2][0]) * s;
	quat[2] = (mat.m[1][0] - mat.m[0][1]) * s;
}


bool ModelExporter::WriteMatrix( const LTMatrix& mat )
{
	char tmpStr[256];

	BailOutIfFalse( ltaWriter.IsValid() );

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


// recursively build the list of key frames for the animated nodes
bool ModelExporter::BuildKeyTimes( ExpNode* expNode )
{
//--- removed by jjh. for now we should just grab all keyframes ---
//	if( !expNode->isLinkNode )
//		return true;

	INode* node = expNode->node;
	BailOutIfFalse( node );

	Control* controller = node->GetTMController();
	Control* curController;

	if( controller )
	{
		// insert the key frames for the controller
		InsertAnimatableKeyFrames( controller );

		if( (controller->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) ||
			(controller->ClassID() == BIPBODY_CONTROL_CLASS_ID) ||
			(controller->ClassID() == FOOTPRINT_CLASS_ID) )
		{
			InsertBipedKeyFrames( controller );
		}

		curController = controller->GetPositionController();
		if( curController )
		{
			InsertPositionKeyFrames( curController );
		}

		curController = controller->GetRotationController();
		if( curController )
		{
			InsertRotationKeyFrames( curController );
		}

		curController = controller->GetScaleController();
		if( curController )
		{
			InsertScaleKeyFrames( curController );
		}
	}

	// recurse into the children
	for( unsigned i = 0; i < expNode->children.size(); i++ )
	{
		BuildKeyTimes( expNode->children[i] );
	}

	// if this is the root of the tree, sort and remove duplicates
	if( !expNode->parent )
	{
		// add the last time into the range
		keyTimes.push_back( ip->GetAnimRange().End() );

		keyTimes.sort();
		keyTimes.unique();
	}

	return true;
}

void ModelExporter::InsertAnimatableKeyFrames( Animatable* animatable )
{
	int numKeyFrames = animatable->NumKeys();
	if( numKeyFrames == NOT_KEYFRAMEABLE )
		return;

	Interval range = ip->GetAnimRange();
	TimeValue curTime;

	for( int i = 0; i < numKeyFrames; i++ )
	{
		curTime = animatable->GetKeyTime( i );

		if( curTime >= range.Start() && curTime <= range.End() )
			keyTimes.push_back( curTime );
	}
}

void ModelExporter::InsertBipedKeyFrames( Control* controller )
{
	Interval range = ip->GetAnimRange();

	for( int i = 0; i < controller->NumSubs(); i++ )
	{
		Animatable* subAnim = controller->SubAnim( i );

		if( !subAnim )
			continue;

		InsertAnimatableKeyFrames( subAnim );
	}
}

void ModelExporter::InsertPositionKeyFrames( Control* controller )
{
	ITCBPoint3Key tcbPosKey;
	IBezPoint3Key bezPosKey;
	ILinPoint3Key linPosKey;

	IKeyControl* keyController = GetKeyControlInterface( controller );
	if( !keyController )
		return;

	IKey* key = NULL;

	if( controller->ClassID() == Class_ID(TCBINTERP_POSITION_CLASS_ID, 0) )
		key = &tcbPosKey;
	else if( controller->ClassID() == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0) )
		key = &bezPosKey;
	else if( controller->ClassID() == Class_ID(LININTERP_POSITION_CLASS_ID, 0) )
		key = &linPosKey;

	if( key )
		InsertKeyFrames( keyController, key );
}

void ModelExporter::InsertRotationKeyFrames( Control* controller )
{
	ITCBRotKey tcbRotKey;
	IBezQuatKey bezRotKey;
	ILinRotKey linRotKey;

	IKeyControl* keyController = GetKeyControlInterface( controller );
	if( !keyController )
		return;

	IKey* key = NULL;

	if( controller->ClassID() == Class_ID(TCBINTERP_ROTATION_CLASS_ID, 0) )
		key = &tcbRotKey;
	else if( controller->ClassID() == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0) )
		key = &bezRotKey;
	else if( controller->ClassID() == Class_ID(LININTERP_ROTATION_CLASS_ID, 0) )
		key = &linRotKey;

	if( key )
		InsertKeyFrames( keyController, key );
}

void ModelExporter::InsertScaleKeyFrames( Control* controller )
{
	ITCBScaleKey tcbScaleKey;
	IBezScaleKey bezScaleKey;
	ILinScaleKey linScaleKey;

	IKeyControl* keyController = GetKeyControlInterface( controller );
	if( !keyController )
		return;

	IKey* key = NULL;

	if( controller->ClassID() == Class_ID(TCBINTERP_SCALE_CLASS_ID, 0) )
		key = &tcbScaleKey;
	else if( controller->ClassID() == Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0) )
		key = &bezScaleKey;
	else if( controller->ClassID() == Class_ID(LININTERP_SCALE_CLASS_ID, 0) )
		key = &linScaleKey;

	if( key )
		InsertKeyFrames( keyController, key );
}

void ModelExporter::InsertKeyFrames( IKeyControl* keyController, IKey* key )
{
	int numKeys = keyController->GetNumKeys();
	Interval range = ip->GetAnimRange();
	TimeValue curTime;

	for( int i = 0; i < numKeys; i++ )
	{
		keyController->GetKey( i, key );

		curTime = key->time;

		if( curTime >= range.Start() && curTime <= range.End() )
			keyTimes.push_back( curTime );
	}
}
