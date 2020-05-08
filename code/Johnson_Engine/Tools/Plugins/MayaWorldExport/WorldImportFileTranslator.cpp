#include <maya\MGlobal.h>
#include "WorldImportFileTranslator.h"

WorldImportFileTranslator::WorldImportFileTranslator()
{
}

WorldImportFileTranslator::~WorldImportFileTranslator()
{
}

void* WorldImportFileTranslator::creator( void )
{
	return new WorldImportFileTranslator();
}

bool WorldImportFileTranslator::haveReadMethod( void ) const
{
	return true;
}

bool WorldImportFileTranslator::haveWriteMethod( void ) const
{
	return false;
}

MString WorldImportFileTranslator::defaultExtension( void ) const
{
	return "lta";
}

bool WorldImportFileTranslator::canBeOpened( void )const
{
	return true;
}

MPxFileTranslator::MFileKind WorldImportFileTranslator::identifyFile( const MFileObject& fileName, const char* buffer, short size ) const
{
	MFileKind retVal = kIsMyFileType;

	return retVal;
}

MStatus WorldImportFileTranslator::reader( const MFileObject& file, const MString& options, MPxFileTranslator::FileAccessMode mode )
{
	int cmdResult = 0;

	bool importObjects = false;
	bool useTexInfo = false;
	bool importTexDtx = false;
	bool importPrefabs = false;
	MString baseTexPath( "" );
	MString projectPath( "" );
	MString scale( "1.0" );

	if( options.length() > 0 )
	{
		MStringArray optionList;
		MStringArray curOption;

		const char* tmp = options.asChar();

		options.split( ';', optionList );

		int length = optionList.length();
		for( int i = 0; i < length; i++ )
		{
			curOption.clear();
			optionList[i].split( '=', curOption );

			if( curOption.length() < 2 )
				continue;

			if( curOption[0] == MString( "useTexInfo" ) )
			{
				if( curOption[1].asInt() > 0 )
					useTexInfo = true;
			}
			else if( curOption[0] == MString( "objects" ) )
			{
				if( curOption[1].asInt() > 0 )
					importObjects = true;
			}
			else if( curOption[0] == MString( "baseTexPath" ) )
			{
				baseTexPath = curOption[1];
			}
			else if( curOption[0] == MString( "scale" ) )
			{
				scale = curOption[1];
			}
			else if( curOption[0] == MString( "importTexDtx" ) )
			{
				if( curOption[1].asInt() > 0 )
					importTexDtx = true;
			}
			else if( curOption[0] == MString( "prefabs" ) )
			{
				if( curOption[1].asInt() > 0 )
					importPrefabs = true;
			}
			else if( curOption[0] == MString( "projectPath" ) )
			{
				projectPath = curOption[1];
			}
		}
	}

	MString cmdString( "lithtechWorldImport " );

	if( (mode != MPxFileTranslator::kOpenAccessMode) && (mode != MPxFileTranslator::kImportAccessMode) )
		return MStatus::kFailure;

	if( useTexInfo )
		cmdString += "-useTexInfo \"" + baseTexPath + "\" ";

	if( importTexDtx )
		cmdString += "-dtxTextures ";

	if( importObjects )
		cmdString += "-objects ";

	if( importPrefabs )
		cmdString += "-prefabs \"" + projectPath + "\" ";
	
	cmdString += "-scale " + scale + " ";

	cmdString += "\"" + file.fullName() + "\"";

	if( !MGlobal::executeCommand( cmdString, cmdResult, true, false ) )
		cmdResult = 0;

	if( cmdResult )
		return MStatus::kSuccess;
	else
		return MStatus::kFailure;
}
