#ifndef EXPORTER_H
#define EXPORTER_H
#pragma once

#pragma warning( disable : 4786 )	// excessively long STL symbol name warning

#include <algorithm>
using std::min;
using std::max;

#include <vector>
#include <set>
#include <list>
#include "max.h"
#include "ltawriter.h"
#include "MaxModelExport.h"

using namespace std;


// the type of animation for a model piece
enum PieceType { NotAPiece=0, Vertex, Skeletal, Socket, ForcedNode };

// forward declaration
class ExpNode;


// internal version of the max node tree
class ModelExporter
{
public:
	ModelExporter( ExporterOptions* options, Interface* ip, MAXModelExport* exporter, bool selected );
	~ModelExporter();

	// export the model
	bool Export( const TCHAR* fileName );

private:
	Interface* ip;						// max interface
	ExporterOptions* options;			// user configurable exporter options
	MAXModelExport* exporter;			// pointer back into the exporter interface class
	bool selectedOnly;					// true if only exporting selected objects
	TimeValue startTime;				// starting animation time
	float minimumWeight;				// bones with influence less than this are ignored

	CLTAWriter ltaWriter;				// the file we are exporting to

	ExpNode* root;						// root of a tree of nodes representing the max scene

	vector<char*> texturePaths;			// flat list of texture paths relative to texture indices

	list<int> keyTimes;					// keyframe times

	vector<ExpNode*> vertexPieces;		// flat list of vertex animated pieces
	vector<ExpNode*> skeletalPieces;	// flat list of skeletally animated pieces
	vector<ExpNode*> forcedNodes;		// flat list of nodes that should be forced to export (along with their parents)
	vector<ExpNode*> socketNodes;		// flat list of nodes that should be exported as sockets
	set<INode*> skelNodes;				// flat list of links that are directly referenced by a skeletal vertex

	// build up the tree and piece lists recursively
	ExpNode* EnumNode( INode* maxNode, ExpNode* parent );

	// recursively write the scene link hierarchy to the file
	bool ExportTree( ExpNode* expNode );

	// tag any forced nodes or socket nodes and their parents
	bool TagLinkNodes();

	// export vertex animations
	bool ExportVertexAnimations();

	// export the vertex animation for a piece
	bool ExportVertexAnimation( ExpNode* expNode, list<int>& frameTimes );

	// export animated positions of nodes
	bool ExportNodeAnimations();

	// export animation data for a node and recurse
	bool ExportNodeAnimation( ExpNode* expNode );

	// write out a list of keyframes to the lta file
	bool WriteFrameList( list<int>& frameList );

	// export a skeletally animated piece
	bool ExportSkeletalPiece( ExpNode* expNode );
	bool ExportSkeletalPiecePhysique( ExpNode* expNode, Modifier* mod );
#ifndef USES_MAX3_SDK
	bool ExportSkeletalPieceSkin( ExpNode* expNode, Modifier* mod );
#endif

	// export on-load-cmds node
	bool ExportOnLoadCmds();

	// export the sockets
	bool ExportSockets();

	// export the skeletal bindings
	bool ExportSkeletalBindings();
	bool ExportSkeletalBindingPhysique( ExpNode* expNode, Modifier* mod );
#ifndef USES_MAX3_SDK
	bool ExportSkeletalBindingSkin( ExpNode* expNode, Modifier* mod );
#endif

	// export a vertex animated piece
	bool ExportVertexPiece( ExpNode* expNode );

	// export a shape given a mesh and a transform
	bool ExportPieceMesh( ExpNode* expNode, Mesh& mesh, const Matrix3& xform );

	// export tools specific info
	bool ExportToolsInfo();

	// locate a node within a subtree, returns NULL if nothing is found
	ExpNode* FindNode( INode* maxNode, ExpNode* start=NULL );

	// recursively build the list of key frames for the animated nodes
	bool BuildKeyTimes( ExpNode* expNode );
	void InsertAnimatableKeyFrames( Animatable* animatable );
	void InsertBipedKeyFrames( Control* controller );
	void InsertPositionKeyFrames( Control* controller );
	void InsertRotationKeyFrames( Control* controller );
	void InsertScaleKeyFrames( Control* controller );
	void InsertKeyFrames( IKeyControl* keyController, IKey* key );

	// determine the piece type of a node
	PieceType DeterminePieceType( INode* node );

	// returns true if the node can be converted to a triangle mesh object
	bool IsNodeMesh( INode* node );

	// returns true if the node should be forced to export
	bool IsForcedNode( INode* node );

	// returns true if the node is a socket
	bool IsSocket( INode* node );

	// returns true if the node should be vertex animated
	bool IsVertexAnimated( INode* node );

	// finds the texture assignment for this face and adds it to the map index
	bool GetTextureInfo( INode* node, int index );

	// take a max matrix and convert it to lithtech space
	Matrix3 ConvertMatrixSpace( const Matrix3& original );

	// take a max matrix and convert it to a lithtech matrix
	LTMatrix ConvertMatrix( const Matrix3& original );

	// convert a quaternion to a lithtech matrix
	void QuaternionToMatrix( float* quat, LTMatrix& mat );

	// convert a lithtech matrix to a quaternion
	void MatrixToQuaternion( const LTMatrix& mat, float* quat );

	// write out a matrix to the lta file
	bool WriteMatrix( const LTMatrix& mat );
};


// a node in the node tree
class ExpNode
{
public:
	INode* node;				// the max node that this node represents
	Matrix3 xform;				// base xform (binding pose for skeletal) of this node
	ExpNode* parent;			// parent of this node
	bool isLinkNode;			// this is a node that needs to store it's transform info
	PieceType pieceType;		// what type of model piece, if any, this node is
	vector<ExpNode*> children;	// children of this node

	ExpNode() : node(NULL), parent(NULL), isLinkNode(false), pieceType(NotAPiece), xform(1) {};
	~ExpNode();
};


// checks a value, asserts and returns false if it is false
#define BailOutIfFalse(expr) if(!(expr)) { assert(0); return false; }


#endif // EXPORTER_H
