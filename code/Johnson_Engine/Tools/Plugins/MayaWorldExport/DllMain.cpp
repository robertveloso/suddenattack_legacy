#include <maya\MFnPlugin.h>
#include <maya\MStatus.h>
#include "WorldImportCommand.h"
#include "WorldExportCommand.h"
#include "WorldImportFileTranslator.h"
#include "WorldExportFileTranslator.h"


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

	// register the lithtechWorldExport command
	plugin.registerCommand( "lithtechWorldExport", WorldExportCommand::creator, WorldExportCommand::newSyntax );

	// register the lithtechWorldImport command
	plugin.registerCommand( "lithtechWorldImport", WorldImportCommand::creator, WorldImportCommand::newSyntax );

	// register the export file translator
	plugin.registerFileTranslator( "LithTechWorldExport", "", WorldExportFileTranslator::creator, "LithTechWorldExportOptions", "", false );

	// register the import file translator
	plugin.registerFileTranslator( "LithTechWorldImport", "", WorldImportFileTranslator::creator, "LithTechWorldImportOptions", "", true );

	return MStatus::kSuccess;
}


// unload the plugin
__declspec(dllexport) MStatus uninitializePlugin( MObject obj )
{
	MFnPlugin plugin( obj );

	plugin.deregisterFileTranslator( "LithTechWorldImport" );
	plugin.deregisterFileTranslator( "LithTechWorldExport" );
	plugin.deregisterCommand( "lithtechWorldImport" );
	plugin.deregisterCommand( "lithtechWorldExport" );

	return MStatus::kSuccess;
}
