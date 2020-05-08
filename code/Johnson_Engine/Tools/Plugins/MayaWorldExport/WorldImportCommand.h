#ifndef WORLDIMPORTCOMMAND_H
#define WORLDIMPORTCOMMAND_H

#pragma once

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MSelectionList.h>
#include <maya/MFloatArray.h>
#include "WorldImport.h"


class WorldImportCommand : public MPxCommand
{
public:
	WorldImportCommand();
	virtual ~WorldImportCommand();

	// required methods for a mel command
	static void* creator();
	static MSyntax newSyntax();
	MStatus doIt( const MArgList& args );
	bool isUndoable() const;

private:
	double scale;					// scale value for converting from LithTech to Maya units
	bool importObjects;				// true if objects should be imported as locators
	bool importPrefabs;				// true if prefabs should be imported as references
	bool importTexInfo;				// true if texture information should be imported
	bool importTexTga;				// true if imported textures are created using .tga extension, otherwise .dtx
	MString baseTexPath;			// path to the root of the texture tree to import
	MString projectPath;			// path to the project directory (for prefabs)
	MString fileName;				// full path of file to be imported

	WorldImport world;				// the generic world loader

	std::vector<MString> shaders;	// list of shading group names associated with texture indices
	typedef std::pair<float,float> floatPair;
	std::vector<floatPair> texDims;	// dimensions of the textures

	int curPrefab;					// number of the next prefab to be created (to prevent namespace collision)

	int numFailedTextureLoads;		// number of textures that we couldn't find

	MStatus parseArgs( const MArgList& args );	// parses the arguments passed into this mel command

	bool LoadTextures();
	bool BuildHierarchy();
	bool AddNode( WINode* node, MObject& parentXForm );

	bool AddBrush( WINode* node, MObject& parentXForm, MObject& curXForm );
	bool AddObject( WINode* node, MObject& parentXForm, MObject& curXForm );
	bool AddPrefab( WINode* node, MObject& parentXForm, MObject& curXForm );

	void GenerateTextureName( MString& name, MString& simpleName );
};


#endif // WORLDIMPORTCOMMAND_H
