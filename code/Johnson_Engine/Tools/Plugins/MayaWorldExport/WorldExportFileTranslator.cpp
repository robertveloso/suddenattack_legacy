#include <maya\MGlobal.h>
#include "WorldExportFileTranslator.h"

WorldExportFileTranslator::WorldExportFileTranslator()
{
}

WorldExportFileTranslator::~WorldExportFileTranslator()
{
}

void* WorldExportFileTranslator::creator( void )
{
	return new WorldExportFileTranslator();
}

bool WorldExportFileTranslator::haveReadMethod( void ) const
{
	return false;
}

bool WorldExportFileTranslator::haveWriteMethod( void ) const
{
	return true;
}

MString WorldExportFileTranslator::defaultExtension( void ) const
{
	return "lta";
}

bool WorldExportFileTranslator::canBeOpened( void )const
{
	return true;
}

MPxFileTranslator::MFileKind WorldExportFileTranslator::identifyFile( const MFileObject& fileName, const char* buffer, short size ) const
{
	MFileKind retVal = kNotMyFileType;

	return retVal;
}

MStatus WorldExportFileTranslator::writer( const MFileObject& file, const MString& options, MPxFileTranslator::FileAccessMode mode )
{
	int cmdResult = 0;

	bool useTextureInfo = false;
	MString projectPath( "" );
	MString scale( "1.0" );
	bool detailBrushes = true;
	bool triangulateBrushes = false;
	bool exportAnimations = false;
	bool forceKeys = false;
	MString forceKeysFrames( "5" );
	bool includeKeys = false;
	bool externalKeys = false;
	MString keyPath( "" );

	if( options.length() > 0 )
	{
		MStringArray optionList;
		MStringArray curOption;

		options.split( ';', optionList );
		
		int length = optionList.length();
		for( int i = 0; i < length; i++ )
		{
			curOption.clear();
			optionList[i].split( '=', curOption );

			if( curOption.length() < 2 )
				continue;

			if( curOption[0] == MString( "useTextureInfo" ) )
			{
				useTextureInfo = curOption[1].asInt() > 0;
			}
			else if( curOption[0] == MString( "projectPath" ) )
			{
				projectPath = curOption[1];
			}
			else if( curOption[0] == MString( "scale" ) )
			{
				scale = curOption[1];
			}
			else if( curOption[0] == MString( "detailBrushes" ) )
			{
				detailBrushes = curOption[1].asInt() > 0;
			}
			else if( curOption[0] == MString( "triangulateBrushes" ) )
			{
				triangulateBrushes = curOption[1].asInt() > 0;
			}
			else if( curOption[0] == MString( "exportAnimations" ) )
			{
				exportAnimations = curOption[1].asInt() > 0;
			}
			else if( curOption[0] == MString( "forceKeys" ) )
			{
				forceKeys = curOption[1].asInt() > 0;
			}
			else if( curOption[0] == MString( "forceKeysFrames" ) )
			{
				forceKeysFrames = curOption[1];
			}
			else if( curOption[0] == MString( "includeKeys" ) )
			{
				includeKeys = curOption[1].asInt() > 0;
			}
			else if( curOption[0] == MString( "externalKeys" ) )
			{
				externalKeys = curOption[1].asInt() > 0;
			}
			else if( curOption[0] == MString( "keyPath" ) )
			{
				keyPath = curOption[1];
			}
		}
	}

	MString cmdString( "lithtechWorldExport " );

	cmdString += "-projectPath \"" + projectPath + "\" ";

	if( (mode == MPxFileTranslator::kExportAccessMode) || (mode == MPxFileTranslator::kSaveAccessMode) )
		cmdString += "-all ";
	else if( mode != MPxFileTranslator::kExportActiveAccessMode )
		return MStatus::kFailure;

	if( useTextureInfo )
		cmdString += "-useTexInfo ";

	cmdString += "-scale " + scale + " ";

	if( detailBrushes )
		cmdString += "-detailBrushes ";

	if( triangulateBrushes )
		cmdString += "-triangulateBrushes ";

	if( exportAnimations )
	{
		cmdString += "-exportAnimations ";

		if( forceKeys )
		{
			cmdString += "-forceKeys " + forceKeysFrames + " ";

			if( includeKeys )
				cmdString += "-includeKeys ";
		}

		if( externalKeys )
		{
			cmdString += "-externalKeyFile \"" + keyPath + "\" ";
		}
	}

	cmdString += "\"" + file.fullName() + "\"";

	if( !MGlobal::executeCommand( cmdString, cmdResult, true, false ) )
		cmdResult = 0;

	if( cmdResult )
		return MStatus::kSuccess;
	else
		return MStatus::kFailure;
}
