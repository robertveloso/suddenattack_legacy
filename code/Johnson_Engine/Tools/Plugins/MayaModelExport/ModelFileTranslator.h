#ifndef MODELFILETRANSLATOR_H
#define MODELFILETRANSLATOR_H

#pragma once

#include <maya/MPxFileTranslator.h>

class ModelFileTranslator : public MPxFileTranslator
{
public:
	ModelFileTranslator();
	virtual ~ModelFileTranslator();

	static void* creator();

	MStatus writer( const MFileObject& file, const MString& optionsString, MPxFileTranslator::FileAccessMode mode );
	bool haveReadMethod() const;
	bool haveWriteMethod() const;
	MString defaultExtension() const;
	bool canBeOpened() const;
	MFileKind identifyFile( const MFileObject& fileName, const char* buffer, short size ) const;
};

#endif // MODELFILETRANSLATOR_H
