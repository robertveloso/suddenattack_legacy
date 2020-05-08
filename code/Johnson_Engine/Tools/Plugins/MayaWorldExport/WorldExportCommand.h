#ifndef WORLDEXPORTCOMMAND_H
#define WORLDEXPORTCOMMAND_H

#pragma once

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MSelectionList.h>
#include <maya/MFloatArray.h>
#include <maya/MDagPath.h>
#include <maya/MMatrix.h>
#include <maya/MBoundingBox.h>
#include <maya/MTime.h>
#include "LTAFile.h"
#include <list>



// number of verts we use a static index array for, any over this switch to dynamic
#define STATIC_VERT_INDEX_SIZE		256


// forward declarations
struct AnimNode;


class WorldExportCommand : public MPxCommand
{
public:
	WorldExportCommand();
	virtual ~WorldExportCommand();

	bool ExportScene();

	// required methods for a mel command
	static void* creator();
	static MSyntax newSyntax();
	MStatus doIt( const MArgList& args );
	bool isUndoable() const;

private:
	bool exportAll;					// false if only the selected objects are to be exported
	bool exportUVs;					// generate texture mapping based on the maya materials
	MString projectPath;			// game project path
	MString fileName;				// full path of file to be exported to
	double scale;					// scale value for converting from Maya units to LithTech units
	bool detailBrushes;				// true if exported brushes should be marked as detail brushes
	bool triangulateBrushes;		// true if brushes should be triangulated (basically joined terrain)
	bool exportAnimations;			// true if we are exporting animated worldmodels
	bool forceKeys;					// true if keys are being exported at even intervals
	unsigned forceKeysFrames;		// the interval keys will be forced at
	bool includeKeys;				// if keys are being forced, should we also include user defined keys
	bool externalKeys;				// true if keys should be exported to an external file
	MString keyPath;				// path offset from project path in which to create the key file

	int numImproperFaces;			// number of faces with bad texture assignments

	MSelectionList selectionList;	// list of the current selection within maya
	LTAFile* ltaFile;				// class that represents the exported lta file

	MMatrix convertMat;					// the scaling and space conversion matrix

	MTime beginTime;					// start of the current animation range
	MTime endTime;						// end of the current animation range
	std::list<AnimNode*> animNodes;		// list of nodes with keyframes assigned (if exporting keyframed worldmodels)
	std::list<MTime> allKeys;			// union of all key times on all animated nodes

	MStatus parseArgs( const MArgList& args );	// parses the arguments passed into this mel command

	// recursively build a list of animated nodes in the scene
	bool FindAnimations( const MDagPath& path, AnimNode* parentAnimNode );

	bool NodeEnum( const MDagPath& path, MBoundingBox* bounds, bool checkForAnim );
	bool ExportNode( const MDagPath& path, MBoundingBox* bounds );
	bool ExportNodeAsBrush( const MDagPath& path, MBoundingBox* bounds, const MFloatArray& texWidth, const MFloatArray& texHeight, const MStringArray& texName, const MIntArray& shaderIndices );

	// given a dag node, find the matching animnode if one exists, otherwise NULL
	AnimNode* FindAnimNode( const MDagPath& path );

	// export the animated brushes
	bool ExportAnimNodes( const MString& keyFileRelative );

	// export group objects for the keyframers
	bool ExportKeyFramerGroups();

	// build the transform for the current time, returns false if this node doesn't have a matching key time
	bool BuildAnimNodeTransform( AnimNode* animNode, MTime time, int curFrame );

	// export a specific animnode
	bool ExportAnimNode( AnimNode* animNode, const MString& keyFileRelative );

	// export a key for this node
	bool ExportNodeKey( AnimNode* animNode, MTime timeInterval, const MMatrix& xform, const MVector& kfPos, const char* keyName );

	// take a dag node and fill in the keyframes that it uses, returns NULL if the node is not animated
	AnimNode* BuildAnimNode( const MDagPath& path, const AnimNode* parentAnimNode );

	// find the keys set for this node, returns false if no keys were found for the node (non-recursive check)
	bool BuildNodeKeyTimes( AnimNode* animNode );

	// returns true if the dag path is a parent of an exportable mesh (anywhere in the hierarchy at or below path)
	bool PathParentsValidMesh( const MDagPath& path );

	// take a maya texture path and get the dedit equivalent
	bool GenerateTextureName( MString& name );

	// convert uvs to opqs
	void GenerateOPQ( const MVector& v0, const MVector& v1, const MVector& v2, const MVector& tv0, const MVector& tv1, const MVector& tv2, int width, int height, MVector& o, MVector& p, MVector& q );

	// create the external key file name based on the filename and key path offset
	void ExternalKeyFileName( const char* name, MString& projectRelative, MString& fullPath, MString& fullPathDir );

	// convert a path into a friendly format
	void ConvertPath( MString& path );
};


struct AnimNode
{
	MDagPath path;				// the dag node at the start of the animated hierarchy
	std::list<MTime> keys;		// actual key times on this node
	std::vector<MMatrix> xform;	// array of transforms (one for each key)
};


#endif // WORLDEXPORTCOMMAND_H
