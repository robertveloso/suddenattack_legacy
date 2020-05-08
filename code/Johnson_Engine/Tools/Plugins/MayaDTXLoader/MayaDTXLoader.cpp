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
#include "image.h"


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


struct DTXFile
{
	DStream* stream;		// file stream
	POINTER* bufPtr;		// line buffers
	bool noAlpha;			// true if the image should be forced to 24-bits
	int width;				// image width
	int height;				// image height
	uint32* data;			// image data (RGBA)
};


static DStream* dtxOpen( IMF_INFO* info );
static int dtxClose( IMF_OBJECT* imf );
static int dtxScanRead( POINTER data, int scan, POINTER** lineBuf );
static bool dtxReadDtx( DTXFile* dtxFile );


char* program = "Wavefront";					// all wavefront apps can use this plugin
char* version = IMF_PROTOCOL_CURRENT;			// api compiled against
char* type = "image";							// type of plug-in
char* imageKey = "dtx";							// unique identifier
char* imageExtension = ".dtx";					// image file extension
char* imageName = "LithTechDTX";				// verbose name of image type
char* imageFormatString = "%s.%04.4d%s";		// name format for saved images
char* imageNameSyntax = "Name.####.Ext";		// Maya uses imageFormatString instead

int imageAddExtension = TRUE;					// should the file extension be added
int imageUsage = IMF_C_GENERIC;					// might need to be IMF_C_IMAGE_ANY
int imageOrientation = IMF_C_ORIENT_BOT_LEFT;	// 0,0 is upper left corner
int imageSupportsActiveWindow = FALSE;			// active win not supported

int imageNumberOfLuts = 0;						// no look up tables
int imageNumberOfChannels = 3;					// red green blue
int imageNumberOfMattes = 1;					// alpha
int imageNumberOfZChannels = 0;					// no depth

unsigned int imageBitsPerLut = 0x00000000;		// 0-bits of look up table
unsigned int imageBitsPerChannel = 0x00000080;	// 8-bits per color channel
unsigned int imageBitsPerMatte = 0x00000080;	// 8-bits of alpha
unsigned int imageBitsPerZChannel = 0x00000000;	// 0-bits of depth

unsigned int imageAccess = IMF_C_READ_RANDOM;	// we are read only


int imageInit( void )
{
#ifdef _DEBUG
	char buf[256];
	sprintf( buf, "dtx loader compiled: %s", __TIMESTAMP__ );
	ERR_printf( ERR__INFO, buf );
#endif // _DEBUG
	return TRUE;
}


// we don't support saving dtx files
int imageWriteOpen( IMF_OBJECT* imf )
{
	// read-only format
	imf__err = IMF_C_NO_SUPPORT;
	ERR_printf( ERR__ERROR, "DTX format is read-only" );
	return FALSE;
}


// check to see if the file is a valid .dtx
BOOLEAN imageIsFile( char* fn, FILE* fp )
{
	uint32 zero;
	int32 version;	// -5

	// check if the file was already open
	bool wasOpen = (fp != NULL );

	if( fp )
	{
		rewind( fp );
	}
	else
	{
		// check if this is even a .dtx file
		int fnLen = strlen( fn );
		if( fnLen >= 3 )
		{
			if( !(tolower(fn[fnLen-3]) == 'd' && tolower(fn[fnLen-2]) == 't' && tolower(fn[fnLen-1]) == 'x') )
				return FALSE;
		}

		if( (fp = fopen( fn, "rb" )) == NULL )
		{
			ERR_printf( ERR__ERROR, "Can't open \"%s\" : %s", fn, strerror( errno ) );
			return FALSE;
		}
	}

	bool success = false;

	if( fread( &zero, 4, 1, fp ) == 1 )
	{
		if( fread( &version, 4, 1, fp ) == 1 )
			success = true;
	}

	if( !success )
	{
		ERR_printf( ERR__ERROR, "Can't read \"%s\" : %s", fn, strerror( errno ) );
		return FALSE;
	}

	if( !wasOpen )
		fclose( fp );

	return ((zero == 0) && (version == -5));
}


// open the file (actually reads the image as well)
int imageReadOpen( IMF_OBJECT* imf )
{
#ifdef _DEBUG
	char buf[256];
	sprintf( buf, "imageReadOpen: %s", imf->info.name );
	ERR_printf( ERR__INFO, buf );
#endif // _DEBUG

	// check for environment variable specifying that we should ignore alpha and make a 24-bit texture
	bool noAlpha = false;
	char* noAlphaEnv = getenv( "MAYA_DTX_NOALPHA" );
	if( noAlphaEnv && (strcmp( noAlphaEnv, "TRUE" ) == 0) )
		noAlpha = true;

#ifdef _DEBUG
	sprintf( buf, "MAYA_DTX_NOALPHA=%s", noAlpha ? "TRUE" : "FALSE" );
	ERR_printf( ERR__INFO, buf );
#endif // _DEBUG

	DStream* stream = dtxOpen( &imf->info );
	if( !stream )
		return FALSE;

	imf->info.count = 1;
	imf->info.image = (IMF_IMAGE*)malloc( sizeof(IMF_IMAGE) );
	imf__init_ifd( imf );

	DTXFile** dtxFilePtr = (DTXFile**)malloc( sizeof(DTXFile*) );
	imf->data = (POINTER*)dtxFilePtr;

	DTXFile* dtxFile = (DTXFile*)malloc( sizeof(DTXFile) );
	memset( dtxFile, 0, sizeof(DTXFile) );
	dtxFilePtr[0] = dtxFile;
	dtxFile->bufPtr = NULL;
	dtxFile->stream = stream;
	dtxFile->noAlpha = noAlpha;
	if( !dtxReadDtx( dtxFile ) )
		return FALSE;

	imf->scan = (IMF_scanProc)dtxScanRead;
	imf->close = (IMF_closeProc)dtxClose;

	IMF_INFO* info = &imf->info;
	IMF_IMAGE* image = &info->image[0];

	info->lut_exists = FALSE;

	char* unknown = "unknown";

	image->aspect.name = _strdup( unknown );
	image->aspect.width = dtxFile->width;
	image->aspect.height = dtxFile->height;
	image->aspect.ratio = dtxFile->width / (float)dtxFile->height;
	image->window.left = 0;
	image->window.right = dtxFile->width - 1;
	image->window.bottom = 0;
	image->window.top = dtxFile->height - 1;
	image->active = image->window;
	image->curve.gamma = 1.0;
	image->curve.usage = IMF_C_GENERIC;
	image->chan_bits = 8;
	image->chan_count = 3;
	image->chan_format = _strdup( "rgb" );
	image->chan_type = 1;
	image->matte_bits = 8;
	image->matte_count = noAlpha ? 0 : 1;
	image->matte_type = 1;
	image->aux_bits = 8;
	image->aux_count = 0;
	image->aux_format = _strdup( unknown );
	image->aux_type = 1;

	dtxFile->bufPtr = IMF_chan_alloc( info->image, dtxFile->width, info->key, (int*)NULL );

	return TRUE;
}


static DStream* dtxOpen( IMF_INFO* info )
{
	char* fileName;
	DStream* stream;

	if( info->handle_complete )
		fileName = info->handle;
	else
		fileName = imf__build_handle( NULL, info->handle, info->ext );

	stream = streamsim_Open( fileName, "rb" );
	if( !stream )
	{
		imf__err = IMF_C_CANNOT_OPEN;
		ERR_printf( ERR__ERROR, "Can't open \"%s\" : %s", fileName, strerror( errno ) );
		return NULL;
	}

	return stream;
}


static int dtxClose( IMF_OBJECT* imf )
{
#ifdef _DEBUG
	char buf[256];
	sprintf( buf, "dtxClose: %s", imf->info.name );
	ERR_printf( ERR__INFO, buf );
#endif // _DEBUG

	DTXFile* dtxFile = *(DTXFile**)imf->data;
	IMF_chan_free( dtxFile->bufPtr );
	dtxFile->bufPtr = NULL;
	dtxFile->stream->Release();
	dtxFile->stream = NULL;
	free( dtxFile->data );
	dtxFile->data = NULL;
	free( imf->data[0] );
	free( imf->data );
	imf->info.image = NULL;
	imf->data = NULL;
//	imf__free_obj( imf );		// causes crash due to memory manager mismatch (Maya deleting stuff I allocated)

	return IMF_C_NORMAL;
}


// read one scanline
static int dtxScanRead( POINTER data, int scan, POINTER** lineBuf )
{
	DTXFile* dtxFile = (DTXFile*)data;

	bool noAlpha = dtxFile->noAlpha;

	// check for a valid scanline
	if( scan < 0 || scan >= dtxFile->height )
		return IMF_C_BAD_SCAN;

	*lineBuf = dtxFile->bufPtr;

	uint8* pa;
	uint8* pr = (uint8*)(*lineBuf)[0];
	uint8* pg = (uint8*)(*lineBuf)[1];
	uint8* pb = (uint8*)(*lineBuf)[2];
	if( !noAlpha )
		pa = (uint8*)(*lineBuf)[3];

	uint8* line = (uint8*)(dtxFile->data + dtxFile->width * (dtxFile->height - scan - 1));

	for( int x = 0; x < dtxFile->width; x++ )
	{
		*(pb++) = *(line++);
		*(pg++) = *(line++);
		*(pr++) = *(line++);
		if( !noAlpha )
			*(pa++) = *line;
		line++;
	}

	return IMF_C_NORMAL;
}


// read in the dtx image data and convert it to flat 32-bit RGBA
static bool dtxReadDtx( DTXFile* dtxFile )
{
	TextureData* texData;

	if( dtx_Create( dtxFile->stream, &texData, TRUE ) != DE_OK )
		return false;

	if( texData->m_Header.GetBPPIdent() == BPP_32P )
	{
		dtx_Destroy( texData );
		imf__err = IMF_C_CANNOT_OPEN;
		ERR_printf( ERR__ERROR, "Cannot currently open 32P DTX files." );
		return false;
	}
	else
	{
		TextureMipData* mip = &texData->m_Mips[0];

		dtxFile->data = (uint32*)malloc( mip->m_Width * mip->m_Height * sizeof(uint32) );
		dtxFile->width = mip->m_Width;
		dtxFile->height = mip->m_Height;

		ConvertRequest request;
		request.m_pSrc = (BYTE*)mip->m_Data;
		dtx_SetupDTXFormat( texData, request.m_pSrcFormat );
		request.m_SrcPitch = mip->m_Pitch;
		request.m_pDestFormat->Init( BPP_32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF );
		request.m_pDest = (BYTE*)dtxFile->data;
		request.m_DestPitch = mip->m_Width * sizeof(uint32);
		request.m_Width = mip->m_Width;
		request.m_Height = mip->m_Height;
		request.m_Flags = 0;
		g_FormatMgr.ConvertPixels( &request );
	}

	dtx_Destroy( texData );
	return true;
}
