#ifndef WORLDEXPORTFILETRANSLATOR_H
#define WORLDEXPORTFILETRANSLATOR_H

#pragma once

#include <maya/MPxFileTranslator.h>

class WorldExportFileTranslator : public MPxFileTranslator
{
public:
	WorldExportFileTranslator();
	virtual ~WorldExportFileTranslator();

	static void* creator();

	MStatus writer( const MFileObject& file, const MString& options, MPxFileTranslator::FileAccessMode mode );
	bool haveReadMethod() const;
	bool haveWriteMethod() const;
	MString defaultExtension() const;
	bool canBeOpened() const;
	MFileKind identifyFile( const MFileObject& fileName, const char* buffer, short size ) const;
};

#endif // WORLDEXPORTFILETRANSLATOR_H
