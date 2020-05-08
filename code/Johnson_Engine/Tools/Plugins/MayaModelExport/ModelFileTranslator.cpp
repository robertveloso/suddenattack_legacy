#include <maya\MGlobal.h>
#include "ModelFileTranslator.h"

ModelFileTranslator::ModelFileTranslator()
{
}

ModelFileTranslator::~ModelFileTranslator()
{
}

void* ModelFileTranslator::creator( void )
{
	return new ModelFileTranslator();
}

bool ModelFileTranslator::haveReadMethod( void ) const
{
	return false;
}

bool ModelFileTranslator::haveWriteMethod( void ) const
{
	return true;
}

MString ModelFileTranslator::defaultExtension( void ) const
{
	return "lta";
}

bool ModelFileTranslator::canBeOpened( void )const
{
	return true;
}

MPxFileTranslator::MFileKind ModelFileTranslator::identifyFile( const MFileObject& fileName, const char* buffer, short size ) const
{
	MFileKind retVal = kNotMyFileType;

	return retVal;
}

MStatus ModelFileTranslator::writer( const MFileObject& file, const MString& options, MPxFileTranslator::FileAccessMode mode )
{
	int cmdResult = 0;

	bool ignoreBindPose = false;
	bool exportNormals = false;
	bool exportWorldNode = false;
	bool usePlaybackRange = false;
	bool useTextureInfo = false;
	MString baseTexPath( "" );
	MString animName( "baseAnim" );
	MString scale( "1.0" );

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
				if( curOption[1].asInt() > 0 )
					useTextureInfo = true;
			}
			else if( curOption[0] == MString( "usePlaybackRange" ) )
			{
				if( curOption[1].asInt() > 0 )
					usePlaybackRange = true;
			}
			else if( curOption[0] == MString( "ignoreBindPose" ) )
			{
				if( curOption[1].asInt() > 0 )
					ignoreBindPose = true;
			}
			else if( curOption[0] == MString( "exportNormals" ) )
			{
				if( curOption[1].asInt() > 0 )
					exportNormals = true;
			}
			else if( curOption[0] == MString( "exportWorldNode" ) )
			{
				if( curOption[1].asInt() > 0 )
					exportWorldNode = true;
			}
			else if( curOption[0] == MString( "baseTexPath" ) )
			{
				baseTexPath = curOption[1];
			}
			else if( curOption[0] == MString( "scale" ) )
			{
				scale = curOption[1];
			}
			else if( curOption[0] == MString( "animName" ) )
			{
				animName = curOption[1];
			}
		}
	}

	MString cmdString( "lithtechModelExport " );

	if( (mode == MPxFileTranslator::kExportAccessMode) || (mode == MPxFileTranslator::kSaveAccessMode) )
		cmdString += "-all ";
	else if( mode != MPxFileTranslator::kExportActiveAccessMode )
		return MStatus::kFailure;

	if( useTextureInfo )
		cmdString += "-useTexInfo \"" + baseTexPath + "\" ";

	if( usePlaybackRange )
		cmdString += "-usePlaybackRange ";

	if( ignoreBindPose )
		cmdString += "-ignoreBindPose ";

	if( exportNormals )
		cmdString += "-exportNormals ";

	if( exportWorldNode )
		cmdString += "-exportWorldNode ";

	cmdString += "-scale " + scale + " ";

	cmdString += "\"" + animName + "\" ";

	cmdString += "\"" + file.fullName() + "\"";

	if( !MGlobal::executeCommand( cmdString, cmdResult, true, false ) )
		cmdResult = 0;

	if( cmdResult )
		return MStatus::kSuccess;
	else
		return MStatus::kFailure;
}
