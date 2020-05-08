#include "bdefs.h"
#include "dtxmgr.h"
#include "load_pcx.h"
#include "pixelformat.h"
#include "streamsim.h"
#include "load_pcx.h"
#include "LTTexture.h"
#include <windows.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <errno.h>
//#include <ctype.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <time.h>
#include <assert.h>
#undef ABS
#undef HALFPI
#undef __LINKLIST_H__		// joy
#include "MaxDTXLoader.h"


// =====================================================================
// BEGIN SPOOF -- this is really evil, this is the fastest way to get 
//                everything compiling for a standalone command line 
//                dtx utility (joseph@lith.com)
// =====================================================================

FormatMgr	g_FormatMgr;

void* dalloc(unsigned int size)
{
	return (char*)malloc((size_t)size);
}

void dfree(void *ptr)
{
	free(ptr);
}

void* DefStdlithAlloc(unsigned int size)
{
	return dalloc(size);
}

void DefStdlithFree(void * ptr)
{
	dfree(ptr);
}

void dsi_PrintToConsole(char *,...)
{
	;
}
char *  g_ReturnErrString;
int32	g_DebugLevel;

void dsi_OnReturnError(int)
{
	;
}

// =====================================================================
// END SPOOF
// =====================================================================


// class descriptor stuff to let MAX know a little something about this plug-in
class DTXLoaderClassDesc : public ClassDesc2
{
public:
	int IsPublic()					{ return TRUE; }
	void* Create( BOOL loading )	{ return new DTXLoader; }
	const TCHAR* ClassName()		{ return GetString(IDS_CLASSNAME); }
	SClass_ID SuperClassID()		{ return BMM_IO_CLASS_ID; }
	Class_ID ClassID()				{ return DTXLOADER_CLASSID; }
	const TCHAR* Category()			{ return GetString(IDS_BITMAP_IO); }
	HINSTANCE HInstance()			{ return hInstance; }

	// fixed MAXScript name
	const TCHAR* InternalName()		{ return _T("dtxio"); }
};

static DTXLoaderClassDesc DTXLoaderCD;
ClassDesc* GetDTXLoaderDesc( void ) { return &DTXLoaderCD; }



DTXLoader::DTXLoader()
{
}

DTXLoader::~DTXLoader()
{
}

// number of extensions supported by this bitmap format
int DTXLoader::ExtCount( void )
{
	return 1;
}

const TCHAR* DTXLoader::Ext( int n )
{
	switch( n )
	{
	case 0: return _T("dtx");
	}

	return _T("");
}

// basic information about the image (size, aspect, etc.)
BMMRES DTXLoader::GetImageInfo( BitmapInfo* bi )
{
	TextureData* texData = NULL;
	DStream* stream = NULL;

	if( !(stream = streamsim_Open( bi->Name(), "rb" )) )
		return ProcessImageIOError( bi, GetString(IDS_READ_ERROR) );

	if( dtx_Create( stream, &texData, TRUE ) != DE_OK )
	{
		stream->Release();
		return ProcessImageIOError( bi, GetString(IDS_READ_ERROR) );
	}

	stream->Release();
	stream = NULL;

	TextureMipData* mip = &texData->m_Mips[0];

	uint32 width = mip->m_Width;
	uint32 height = mip->m_Height;

	dtx_Destroy( texData );

	bi->SetWidth( width );
	bi->SetHeight( height );
	bi->SetAspect( 1.0f );
	bi->SetType( BMM_TRUE_32 );
	bi->SetFirstFrame( 0 );
	bi->SetLastFrame( 0 );
	bi->SetFlags( MAP_HAS_ALPHA );

	return BMMRES_SUCCESS;
}

// load the image
BitmapStorage* DTXLoader::Load( BitmapInfo* bi, Bitmap* map, BMMRES* status )
{
	BitmapStorage* s = NULL;
	DStream* stream = NULL;
	TextureData* texData = NULL;
	uint32* data = NULL;

	*status = BMMRES_SUCCESS;

	if( openMode != BMM_NOT_OPEN )
	{
		*status = ProcessImageIOError( bi, BMMRES_INTERNALERROR );
		return NULL;
	}

	if( !(s = BMMCreateStorage( map->Manager(), BMM_TRUE_32 )) )
	{
		*status = ProcessImageIOError( bi, BMMRES_INTERNALERROR );
		return NULL;
	}

	stream = streamsim_Open( bi->Name(), "rb" );
	if( !stream )
	{
		*status = ProcessImageIOError( bi, GetString(IDS_READ_ERROR) );
bailOut:
		delete s;
		if( stream )
			stream->Release();
		if( texData )
			dtx_Destroy( texData );
		if( data )
			delete [] data;
		return NULL;
	}

	if( dtx_Create( stream, &texData, TRUE ) != DE_OK )
	{
		*status = ProcessImageIOError( bi, GetString(IDS_READ_ERROR) );
		goto bailOut;
	}

	stream->Release();
	stream = NULL;

	if( texData->m_Header.GetBPPIdent() == BPP_32P )
	{
		*status = ProcessImageIOError( bi, GetString(IDS_NO_32P) );
		goto bailOut;
	}

	TextureMipData* mip = &texData->m_Mips[0];

	uint32 width = mip->m_Width;
	uint32 height = mip->m_Height;

	bi->SetWidth( width );
	bi->SetHeight( height );
	bi->SetAspect( 1.0f );
	bi->SetType( BMM_TRUE_32 );
	bi->SetFirstFrame( 0 );
	bi->SetLastFrame( 0 );
	bi->SetFlags( MAP_HAS_ALPHA );

	if( 0 == s->Allocate( bi, map->Manager(), BMM_OPEN_R ) )
	{
		*status = ProcessImageIOError( bi, BMMRES_MEMORYERROR );
		goto bailOut;
	}

	data = new uint32[width*height];
	if( !data )
	{
		*status = ProcessImageIOError( bi, BMMRES_MEMORYERROR );
		goto bailOut;
	}

	ConvertRequest request;
	request.m_pSrc = (BYTE*)mip->m_Data;
	dtx_SetupDTXFormat( texData, request.m_pSrcFormat );
	request.m_SrcPitch = mip->m_Pitch;
	request.m_pDestFormat->Init( BPP_32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF );
	request.m_pDest = (BYTE*)data;
	request.m_DestPitch = mip->m_Width * sizeof(uint32);
	request.m_Width = mip->m_Width;
	request.m_Height = mip->m_Height;
	request.m_Flags = 0;
	g_FormatMgr.ConvertPixels( &request );

	dtx_Destroy( texData );
	texData = NULL;

	PixelBuf line64( width );

	for( uint32 y = 0; y < height; y++ )
	{
		BMM_Color_64* l64 = line64.Ptr();
		uint8* l8 = (uint8*)(data + width*y);

		for( uint32 x = 0; x < width; x++, l64++ )
		{
			// might need to premultiply
			l64->b = *(l8++) << 8;
			l64->g = *(l8++) << 8;
			l64->r = *(l8++) << 8;
			l64->a = *(l8++) << 8;
		}
		if( s->PutPixels( 0, y, width, line64.Ptr() ) != 1 )
			goto bailOut;
	}

	delete [] data;

	s->bi.CopyImageInfo( bi );

	return s;
}
