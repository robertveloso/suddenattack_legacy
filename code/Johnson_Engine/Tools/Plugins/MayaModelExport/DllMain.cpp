#include <maya\MFnPlugin.h>
#include <maya\MStatus.h>
#include "ModelExportCommand.h"
#include "ModelFileTranslator.h"


// verify that we are compiling against the correct Maya SDK
#if !defined(MAYA_API_VERSION)
	#error Couldn't find Maya SDK files, check to see if they are installed correctly.
#elif defined(USES_MAYA3_SDK)
	#if MAYA_API_VERSION != 300
		#error Wrong Maya SDK version found, check to see if the SDK is installed correctly.
	#endif
#elif defined(USES_MAYA4_SDK)
	#if MAYA_API_VERSION != 400
		#error Wrong Maya SDK version found, check to see if the SDK is installed correctly.
	#endif
#else
	#error No valid Maya SDK version specified in plug-in project.
#endif


// make the plugin visible to maya
__declspec(dllexport) MStatus initializePlugin( MObject obj )
{
	MFnPlugin plugin( obj, "LithTech, Inc.", "3.2", "Any" );

	// register the lithtechModelExport command
	plugin.registerCommand( "lithtechModelExport", ModelExportCommand::creator, ModelExportCommand::newSyntax );

	// register the file translator (last parameter may neeed to be true if an importer is added)
	plugin.registerFileTranslator( "LithTechModel", "", ModelFileTranslator::creator, "LithTechModelExportOptions", "", false );

	return MStatus::kSuccess;
}


// unload the plugin
__declspec(dllexport) MStatus uninitializePlugin( MObject obj )
{
	MFnPlugin plugin( obj );

	plugin.deregisterFileTranslator( "LithTechModel" );
	plugin.deregisterCommand( "lithtechModelExport" );

	return MStatus::kSuccess;
}
