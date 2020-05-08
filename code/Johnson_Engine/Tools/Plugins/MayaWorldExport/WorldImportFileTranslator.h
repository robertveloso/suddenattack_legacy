#ifndef WORLDIMPORTFILETRANSLATOR_H
#define WORLDIMPORTFILETRANSLATOR_H

#pragma once

#include <maya/MPxFileTranslator.h>

class WorldImportFileTranslator : public MPxFileTranslator
{
public:
	WorldImportFileTranslator();
	virtual ~WorldImportFileTranslator();

	static void* creator();

	MStatus reader( const MFileObject& file, const MString& options, MPxFileTranslator::FileAccessMode mode );
	bool haveReadMethod() const;
	bool haveWriteMethod() const;
	MString defaultExtension() const;
	bool canBeOpened() const;
	MFileKind identifyFile( const MFileObject& fileName, const char* buffer, short size ) const;
};

#endif // WORLDIMPORTFILETRANSLATOR_H
