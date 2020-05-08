#ifndef MODELEXPORTCOMMAND_H
#define MODELEXPORTCOMMAND_H

#pragma once

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MTime.h>
#include <maya/MPointArray.h>
#include <vector>
#include <list>
#include "ltawriter.h"

using namespace std;


// the type of animation for a model piece
enum PieceType { NotAPiece=0, Vertex, Skeletal, Socket, ForcedNode };

// forward declarations
class ExpNode;
class SkinNode;
class VertexFrame;
class SkelFrame;


class ModelExportCommand : public MPxCommand
{
public:
	ModelExportCommand();
	virtual ~ModelExportCommand();

	bool ExportScene();

	// required methods for a mel command
	static void* creator();
	static MSyntax newSyntax();
	MStatus doIt( const MArgList& args );
	bool isUndoable() const;

private:
	bool useTexInfo;					// export map assignments for each piece
	MString baseTexPath;				// path to the root of the texture tree to export
	MString animName;					// name of the exported animation
	MString fileName;					// full path of file to be exported to
	double scale;						// scale value for converting from Maya units to LithTech units
	bool usePlaybackRange;				// true if the animation playback range should be used
	bool ignoreBindPose;				// true if the shape is taken from the first frame rather than the bind pose (also allows post-deformer tweaks)
	bool exportNormals;					// true if normals should be written to the file
	bool exportWorldNode;				// true if a world node should be added at the top of the hierarchy (good for multiple skeletons, etc.)
	bool appendAnim;					// true if we are appending an animation to the file
	bool selectedOnly;					// true if we are exporting selected nodes only
	float minimumWeight;				// bones with influence less than this are ignored

	CLTAWriter ltaWriter;				// the file we are exporting to

	MSelectionList selectionList;		// list of currently selected nodes

	vector<char*> texturePaths;			// flat list of texture paths relative to texture indices

	list<double> keyTimes;				// keyframe times (in milliseconds)
	list<double> keyTimesVA;			// keyframe times (in milliseconds) for vertex animations

	ExpNode* root;						// root of a tree of nodes representing the maya scene

	vector<SkinNode*> skinNodes;		// flat list of all dag nodes with smooth skin bindings

	vector<ExpNode*> vertexPieces;		// flat list of vertex animated pieces
	vector<ExpNode*> skeletalPieces;	// flat list of skeletally animated pieces
	vector<ExpNode*> forcedNodes;		// flat list of nodes that should be forced to export (along with their parents)
	vector<ExpNode*> socketNodes;		// flat list of nodes that should be exported as sockets


	MStatus parseArgs( const MArgList& args );	// parses the arguments passed into this mel command
	bool ExportModel();							// export the model

	// reset all the local storage
	bool CleanUp();

	// fill in skinNodes with path / skin cluster associations
	bool EnumSkinNodes();

	// get the skin node for this dag node, or NULL if there is none
	SkinNode* GetSkinNode( const MDagPath& path );

	// locate a node within a subtree, returns NULL if nothing is found
	ExpNode* GetNode( const MDagPath& path, ExpNode* start=NULL );

	// build up the tree and piece lists recursively
	ExpNode* EnumNode( const MDagPath& path, ExpNode* parent );

	// tag any forced nodes or socket nodes and their parents
	bool TagLinkNodes();

	// recursively write the scene link hierarchy to the file
	bool ExportTree( ExpNode* expNode );

	// export a skeletally animated piece
	bool ExportSkeletalPiece( ExpNode* expNode );

	// export a vertex animated piece
	bool ExportVertexPiece( ExpNode* expNode );

	// export the shape info for this dag path
	bool ExportPieceMesh( ExpNode* expNode, const MDagPath& path, const MString& name, bool vertexAnimated );

	// export vertex animations
	bool ExportVertexAnimations();

	// add animation data for the current frame for a vertex animated piece
	bool BuildVertexAnimation( ExpNode* expNode );

	// export the vertex animation for a piece
	bool ExportVertexAnimation( ExpNode* expNode );

	// export animated positions of nodes
	bool ExportNodeAnimations();

	// add animation data for the current frame for a node and recurse
	bool BuildNodeAnimation( ExpNode* expNode );

	// export animation data for a node and recurse
	bool ExportNodeAnimation( ExpNode* expNode );

	// build the list of keyframes for skeletal and vertex animations
	bool BuildKeyTimes();

	// export on-load-cmds node
	bool ExportOnLoadCmds();

	// export the skeletal bindings
	bool ExportSkeletalBindings();

	// export the sockets
	bool ExportSockets();

	// export tools specific info
	bool ExportToolsInfo();

	// determine the piece type of a node
	PieceType DeterminePieceType( const MDagPath& path );

	// returns true if the node is a mesh object
	bool IsNodeMesh( const MDagPath& path );

	// returns true if the node should be forced to export
	bool IsForcedNode( const MDagPath& path );

	// returns true if the node is a socket
	bool IsSocket( const MDagPath& path );

	// returns true if the node should be vertex animated
	bool IsVertexAnimated( const MDagPath& path );

	// returns true if the node is visible
	bool IsVisible( const MDagPath& path );

	// finds the texture assignment for this mesh and adds it to the map index
	bool GetTextureInfo( const MDagPath& dagPath, int index );

	// convert a maya matrix to a lithtech matrix
	void ConvertMatrix( const MMatrix& src, LTMatrix& dest );

	// write a matrix to the file
	bool WriteMatrix( const LTMatrix& mat );

	// write out a list of keyframes to the lta file (adjusts times to 0 based times)
	bool WriteFrameList( list<double>& frameList );
};


// a node in the node tree
class ExpNode
{
public:
	MDagPath path;				// the maya dag node that this node represents
	MMatrix xform;				// base xform (binding pose for skeletal) of this node
	ExpNode* parent;			// parent of this node
	bool isLinkNode;			// this is a node that needs to store it's transform info
	PieceType pieceType;		// what type of model piece, if any, this node is
	vector<ExpNode*> children;	// children of this node
	list<VertexFrame*> vaData;	// vertex animation data (one per keyframe)
	list<SkelFrame*> skelData;	// skeletal animation data (one per keyframe)	

	ExpNode() : parent(NULL), isLinkNode(false), pieceType(NotAPiece) {};
	~ExpNode();
};


// a dag node with the associated skin cluster function set
class SkinNode
{
public:
	MDagPath path;				// the maya dag node that this node represents
	MDagPath origPath;			// path to the bind pose shape
	MFnSkinCluster* skin;		// the skin cluster function set
	vector<int> influenceMap;	// maps a sparse array of influences to a packed array
	int numInfluences;			// number of influences for this skin cluster

	SkinNode() : skin(NULL), numInfluences(0) {}
	~SkinNode() { delete skin; }
};


// information for one frame of a vertex animation for a piece
class VertexFrame
{
public:
	MPointArray verts;
};


// information for one frame of a skeletal animation for a link node
class SkelFrame
{
public:
	LTVector pos;
	LTRotation quat;

	SkelFrame( const LTVector& pos, const LTRotation& quat ) : pos( pos ), quat( quat ) {}
};


// error checking macro that displays specified error message in script editor
#define CheckErrorMsg(status,msg)							\
if( !(status) )												\
{															\
	MGlobal::displayError( msg );							\
	return false;											\
}


// error checking macro that displays default error message in script editor
#define CheckError(status)									\
if( !(status) )												\
{															\
	MGlobal::displayError( "Export operation failed." );	\
	return false;											\
}															\


#endif // MODELEXPORTCOMMAND_H
