#include "bdefs.h"
#include "dtxmgr.h"
#include "load_pcx.h"
#include "pixelformat.h"
#include "streamsim.h"
#include "load_pcx.h"
#include "LTTexture.h"
#include <direct.h> // for _mkdir

// =====================================================================
// BEGIN SPOOF -- this is really evil, this is the fastest way to get 
//                everything compiling for a standalone command line 
//                dtx utility (joseph@lith.com)
// =====================================================================

FormatMgr	g_FormatMgr;
LAlloc		g_DefAlloc;
int			g_LithExceptionType;

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
void dsi_OnMemoryFailure(void)
{
	;
}/*
void cnt_StartCounter(class Counter *)
{
	;
}

unsigned long cnt_EndCounter(class Counter *)
{
	return 0;
}


Counter::Counter(unsigned long startMode)
{
	;
}*/

// =====================================================================
// END SPOOF
// =====================================================================
#ifdef _DEBUG
	#define DEBUG_MODE
#endif
#ifdef DEBUG_MODE
	#define DEBUG_EXEC(arg) (arg)
#else
	#define DEBUG_EXEC(arg)
#endif

#define DELETE_FILE(arg) if( _unlink((arg)) )  DEBUG_EXEC(printf("\nerror deleting %s", (arg) ));



// =====================================================================
// PROTOTYPES
// =====================================================================

BOOL Write8BitDTX(CAbstractIO &file, TextureData *pData, CString* pFilename, bool retainHeader);
BOOL SaveDtxAs8Bit(DStream *pDtxFile, CAbstractIO &outFile, CString* pFilename, bool retainHeader=false);
BOOL WriteTga(CAbstractIO &file, TextureData *pData);
BOOL SaveDtxAsTga(DStream *pDtxFile, CAbstractIO &outFile);
BOOL SavePcxAsTexture(LoadedBitmap *pPcx, DStream *pStream, DWORD textureFlags);
bool FillTextureWithPcx(LoadedBitmap *pcx, TextureData *dtx);
int  NumColorsWithAlpha(TextureData *pData, uint8* alpha );
void FillDTX(const char* in, const char* out);

// per file ops
void TGA2DTXhandler( const char* inputfile, const char* outputfile);
BOOL DTX2TGAhandler(const char* inputfile, const char* outputfile);
void DTX2BPP_32Phandler(const char* inputfile, const char* outputfile);
void Quantizehandler(const char* inputfile, const char* outputfile, char* command);
void QuantizeAllhandler(const char* inputfile, const char* outputfile, char* command);
void FillDTXHandler(const char* input, const char* output, char* command);
// recursive ops
void R_Quantizehandler(const char* startdir, const char* outputdir, char* command);
void R_QuantizeAllhandler(const char* startdir, const char* outputdir, char* command);

void RecursiveHandler(const char* startdir, const char* outputdir, char* command, int option );
void RecursiveHandlerWrapper(const char* startdir, const char* outputdir, char* command, int option );



enum 
{
	TGA2DTX,
	TGA2BPP_32P,
	DTX2TGA,
	DTX2BPP_32P,
	QUANTIZE,
	QUANTIZE_ALL,
	R_QUANTIZE,
	R_QUANTIZE_ALL,
	R_DTX2TGA,
	R_FILLDTX,
	NUM_ARGS
};

static char g_dtxUtilArgs [NUM_ARGS][30] = 
{
	"-tga2dtx",
	"-tga2bpp32p",
	"-dtx2tga",
	"-dtx2bpp32p",
	"-q",
	"-qa",
	"-r_q",
	"-r_qa",
	"-r_dtx2tga",
	"-r_filldtx"
};

void printUsage(void)
{
	int i = 0;
	printf("\nUsage: dtxutil "); 

	printf("\n\t[");
	for( i = 0; i < NUM_ARGS; i++ )
	{
		if( i > 0 && i < NUM_ARGS ) printf( "|\n\t" );
		printf("%s", g_dtxUtilArgs[i]);
	}
	printf("] ");
	
	printf("[input filename] [output filename]");
}

#define VERSION 5
int main(int argc, char** argv)
{
	printf("\n------------------------------ ");
	printf("\n%s ver: %d", argv[0], VERSION);
	if(argc != 4)  // not the right number of arguments
	{
		printUsage();
		return 1;
	}
	
	// read command line arguments
	// process command line arguments (they are all mutually exclusive)
	// per file ops: 
	//		-tga2dtx
	//		-tga2bpp32p
	//		-dtx2tga
	//		-dtx2bpp32p
	//		-q			// quantize     (selective, leaves textures with alpha as true 32 bit)
	//		-qa			// quantize all (forces everything to be quantized and turned into BPP_32P) 
	// recursive ops:
	//		-r_q        // quantize     (selective, leaves textures with alpha as true 32 bit)
	//		-r_qa	    // quantize all (forces everything to be quantized and turned into BPP_32P) 
	//		-r_dtx2tga	// recursively convert dtx to tga
	//		-r_filldtx	// recursively replace image data in dtx with corresponding tga
	//		-r_filldtx_nocompress	// same as -r_filldtx, but force format to 32 bit

	char* option		= argv[1];
	char* inputfile		= argv[2];
	char* outputfile	= argv[3];

	int choice = 0;
	for(  choice = 0; choice < NUM_ARGS; choice++ )
	{
		if( stricmp(g_dtxUtilArgs[choice], option ) == 0 )
		{
			printf(" (%s) ", option);
			break;
		}
	}

	if( choice == NUM_ARGS )
	{
		printUsage();  // the choice was not found
		return 1;
	}
	
	if( (strcmp(outputfile, inputfile) == 0) && !(choice == R_FILLDTX) )
	{
		printf("\nError: input file and output file can't have the same name");
		return 1;
	}

	printf("\n%s -> %s", inputfile, outputfile );

	
	switch(choice)
	{
		
	case TGA2DTX:
		{
			TGA2DTXhandler(inputfile, outputfile);	
			break;
		}
	case TGA2BPP_32P:
		{
			CString tempFile = outputfile;
			tempFile += ".tmp";
			TGA2DTXhandler(inputfile, LPCTSTR(tempFile));
			DTX2BPP_32Phandler(LPCTSTR(tempFile), outputfile);
			DELETE_FILE(LPCTSTR(tempFile));
			break;
		}
	case DTX2TGA: 
		{
			DTX2TGAhandler(inputfile, outputfile);
			break;
		}
	case DTX2BPP_32P:
		{
			DTX2BPP_32Phandler(inputfile, outputfile);
			break;
		}
	case QUANTIZE:
		{
			Quantizehandler(inputfile, outputfile, argv[0] );
			break;
		}
	case QUANTIZE_ALL:
		{
			QuantizeAllhandler(inputfile, outputfile, argv[0] );
			break;
		}
	case R_FILLDTX:
		{
			FillDTXHandler(inputfile, outputfile, argv[0] );
			break;
		}
	default:
		{
			RecursiveHandlerWrapper(inputfile, outputfile, argv[0], choice );
			break;
		}
		
	};

	return 0;
}
// =======================================================
void TGA2DTXhandler( const char* inputfile, const char* outputfile)
{
	DEBUG_EXEC(printf("\nTGA2DTX"));
	LoadedBitmap bitmap;
	CString str;
	DStream *pStream, *pInFile;


	if( pInFile = streamsim_Open(inputfile, "rb") )
	{
		tga_Create2(pInFile, &bitmap);
		if( dtx_IsTextureSizeValid(bitmap.m_Width) && dtx_IsTextureSizeValid(bitmap.m_Height) )
		{
			pStream = streamsim_Open(outputfile, "wb");
			if(pStream)
			{
				SavePcxAsTexture(&bitmap, pStream, 0);
				pStream->Release();
			}
		}
	}
}
// =======================================================
BOOL DTX2TGAhandler(const char* inputfile, const char* outputfile)
{
	CMoFileIO	outFile;
	DStream		*pStream;

	if(!outFile.Open(outputfile, "wb"))
	{
		printf("\nError: can't open %s", outputfile);
		return FALSE;
	}

	if(!(pStream = streamsim_Open((LPCTSTR)inputfile, "rb")))
	{
		printf("\nError: can't open %s", inputfile);
		outFile.Close();
		return FALSE;
	}

	DEBUG_EXEC(printf("\nDTX2TGA"));
	if(!SaveDtxAsTga(pStream, outFile ))
	{
		printf("\nError: operation unsuccessful");
		return FALSE;
	}
	pStream->Release();
	return TRUE;
}
// =======================================================
void DTX2BPP_32Phandler(const char* inputfile, const char* outputfile)
{
	CMoFileIO	outFile;
	DStream		*pStream;

	if(!outFile.Open(outputfile, "wb"))
	{
		printf("\nError: can't open %s", outputfile);
		return;
	}

	if(!(pStream = streamsim_Open((LPCTSTR)inputfile, "rb")))
	{
		printf("\nError: can't open %s", inputfile);
		outFile.Close();
		return;
	}
	DEBUG_EXEC(printf("\nDTX2BPP_32P"));
	CString dtxfilename = outputfile;
	// _asm int 3;
	if(!SaveDtxAs8Bit(pStream, outFile, &dtxfilename ))
	{
		printf("\nError: operation unsuccessful");
	}
	pStream->Release();
}

int NumColorsWithAlphaInDTX(const char* inputfile, uint8* alpha)
{
	TextureData *pData		= NULL;
	int			numAlpha	= 0;
	DStream		*pStream	= NULL;
	//uint8		alpha		= 0;
	if(!(pStream = streamsim_Open((LPCTSTR)inputfile, "rb")))
	{
		printf("\nError: can't open %s", inputfile);
		return 0;
	}
	if(dtx_Create(pStream, &pData, FALSE) != DE_OK)
		return 0;

	numAlpha = NumColorsWithAlpha(pData, alpha);
	dtx_Destroy(pData);
	pStream->Release();
	return numAlpha;
}

void Quantizehandler(const char* inputfile, const char* outputfile, char* command )
{
	// determine the number of distinct colors with alpha
	// 1         -> just do normal quantization to 256 colors
	// 2         -> chromakey, quantize to 255, transparent to (0,0,0,0)
	// n <= 255  -> quant to 256 - n
	// n >  256  -> true 32 bit

	uint8	alpha	 = 0;
	// determine the number of distinct colors with alpha
	int		numAlpha = NumColorsWithAlphaInDTX(inputfile,&alpha);

	DEBUG_EXEC(printf("\nnumAlpha = %d alpha = %d", numAlpha, alpha));

	char buffer[1024];
	// 1         -> just do normal quantization to 256 colors
	if( 1 == numAlpha )
	{
		CString tgaFile = "tmp.tga";//inputfile;  tgaFile += ".tga";
		CString tmpFile = inputfile;  tmpFile += ".dtx";
		// DTX->TGA
		if( DTX2TGAhandler(inputfile, LPCTSTR(tgaFile)) )
		{
			// TGA 32bit -> TGA256
			sprintf( buffer,"imgconvert -colors 256 -colorspace Transparent %s %s", LPCTSTR(tgaFile), LPCTSTR(tgaFile) );
			system( buffer );
			// TGA->DTX
			TGA2DTXhandler(LPCTSTR(tgaFile), LPCTSTR(tmpFile));
			// DTX->BPP32P
			DTX2BPP_32Phandler(LPCTSTR(tmpFile), outputfile);

			DELETE_FILE(LPCTSTR(tgaFile));
			DELETE_FILE(LPCTSTR(tmpFile));
		}
		else
		{
			DELETE_FILE(LPCTSTR(tgaFile));
		}
		// sprintf( buffer,"echo %cCONVERT\%c", '%', '%' );
		// system( buffer );
	}
	// 2         -> chromakey, quantize to 255, transparent to (0,0,0,0)
	else if( 2 == numAlpha)
	{
		CString tgaFile = inputfile;  tgaFile += ".tga";
		CString tmpFile = inputfile;  tmpFile += ".dtx";
		// DTX->TGA
		if( DTX2TGAhandler(inputfile, LPCTSTR(tgaFile)) )
		{
			// TGA 32bit -> TGA256
			sprintf( buffer,"imgconvert -colors 255 -colorspace Transparent %s %s", LPCTSTR(tgaFile), LPCTSTR(tgaFile) );
			system( buffer );
			// TGA->DTX
			TGA2DTXhandler(LPCTSTR(tgaFile), LPCTSTR(tmpFile));
			// DTX->BPP32P
			DTX2BPP_32Phandler(LPCTSTR(tmpFile), outputfile);
			DELETE_FILE(LPCTSTR(tgaFile));
			DELETE_FILE(LPCTSTR(tmpFile));
		}
		else
		{
			DELETE_FILE(LPCTSTR(tgaFile));
		}
	}
	// n >  256  -> true 32 bit
	else
	{
		CString tgaFile = inputfile;  tgaFile += ".tga";
		CString tmpFile = inputfile;  tmpFile += ".dtx";
		// DTX->TGA
		if( DTX2TGAhandler(inputfile, LPCTSTR(tgaFile)) )
		{
			printf("\nwarning: %s has too many gradients of alpha, forcing 32 bit", inputfile );
			sprintf( buffer,"\ncopy /Y %s %s", inputfile, outputfile );
			printf(buffer);
			system( buffer );
		}
		else
		{
			DELETE_FILE(LPCTSTR(tgaFile));
		}
	}
}

void QuantizeAllhandler(const char* inputfile, const char* outputfile, char* command)
{
	uint8	alpha	 = 0;
	char buffer[1024];
	// determine the number of distinct colors with alpha
	int	 numAlpha = NumColorsWithAlphaInDTX(inputfile,&alpha);

	DEBUG_EXEC(printf("\nnumAlpha = %d", numAlpha));
	CString tgaFile = "tmp.tga";
	CString tmpFile = inputfile;  tmpFile += ".tmp";

	// DTX->TGA
	if( DTX2TGAhandler(inputfile, LPCTSTR(tgaFile)) )
	{
		// TGA 32bit -> TGA256		
		if( numAlpha == 1 ) sprintf( buffer,"imgconvert -colors 256 -colorspace Transparent %s %s", LPCTSTR(tgaFile), LPCTSTR(tgaFile) );
		else   				sprintf( buffer,"imgconvert -colors 240 -colorspace Transparent %s %s", LPCTSTR(tgaFile), LPCTSTR(tgaFile) );
		
		DEBUG_EXEC(printf("\n%s", buffer));
		system( buffer );
		// TGA->DTX
		TGA2DTXhandler(LPCTSTR(tgaFile), LPCTSTR(tmpFile));
		// DTX->BPP32P
		DTX2BPP_32Phandler(LPCTSTR(tmpFile), outputfile);

		DELETE_FILE(LPCTSTR(tgaFile));
		DELETE_FILE(LPCTSTR(tmpFile));
	}
	else
	{
		DELETE_FILE(LPCTSTR(tgaFile));
	}
}

void RecursiveHandlerWrapper(const char* startdir, const char* outputdir, char* command, int option )
{
	char buffer[1024];
	if (option != R_DTX2TGA) {
	sprintf(buffer,"rmdir /S /Q %s", outputdir); DEBUG_EXEC(printf("\nsystem(%s)",buffer ));
	system(buffer);
	_mkdir(outputdir);
	sprintf(buffer,"%s//%s", outputdir, startdir ); 
	_mkdir(buffer);
	}
	RecursiveHandler(startdir, outputdir, command, option );
}

void RecursiveHandler(const char* startdir, const char* outputdir, char* command, int option )
{
	WIN32_FIND_DATA	findData;
	HANDLE			findHandle;
	CString			startdirDyn;

	startdirDyn		= startdir;
	startdirDyn		+= "\\*.*";
	findHandle		= FindFirstFile(LPCTSTR(startdirDyn), &findData );

	DEBUG_EXEC(printf("\ndir: %s", LPCTSTR(startdirDyn)));
	if( INVALID_HANDLE_VALUE != findHandle )
	{
		int count = 0;
		do
		{
			if( count > 1 )
			{
				CString	 subDirDyn;
				subDirDyn = startdir;
				subDirDyn += "\\";
				subDirDyn += findData.cFileName;
				CString	 outputDirDyn;
				outputDirDyn  = outputdir;
				outputDirDyn += "\\";	
				outputDirDyn += subDirDyn;
				
				if( findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
				{
					DEBUG_EXEC(printf("\ndir:	")); 
					DEBUG_EXEC(printf("(%s)", findData.cFileName ));
					_mkdir(LPCTSTR(outputDirDyn)); DEBUG_EXEC(printf("\nmkdir: %s", LPCTSTR(outputDirDyn)));
					RecursiveHandler(LPCTSTR(subDirDyn), outputdir, command, option);
				
				}
				else
				{
					DEBUG_EXEC(printf("\nprocess: ")); 
					CString	 fileNameDyn;
					fileNameDyn = startdir;
					fileNameDyn += "\\";
					fileNameDyn += findData.cFileName;
					DEBUG_EXEC(printf("%s -> %s", LPCTSTR(fileNameDyn), LPCTSTR(outputDirDyn) ));
					char buffer[1024];
					switch(option)
					{
					case R_QUANTIZE:
						sprintf(buffer, "%s -q %s %s", command, LPCTSTR(fileNameDyn), LPCTSTR(outputDirDyn)  );
						DEBUG_EXEC(printf("\n%s",buffer));
						system(buffer);
						break;
					case R_QUANTIZE_ALL:
						sprintf(buffer, "%s -qa %s %s", command, LPCTSTR(fileNameDyn), LPCTSTR(outputDirDyn)  );
						DEBUG_EXEC(printf("\n%s",buffer));
						system(buffer);
						break;
					case R_DTX2TGA:
						char tempBuf[256], tempBuf2[256];
						memset(tempBuf, '\0', 256);
						memset(tempBuf2, '\0', 256);
						strncpy(tempBuf, fileNameDyn, strlen(fileNameDyn)-3);
						// printf("\n\n\n%s\n\n", tempBuf);
						sprintf(tempBuf2, "%stga", tempBuf);
						// printf("\n\n%s\n\n\n", tempBuf2);
						sprintf(buffer, "%s -dtx2tga %s %s", command, LPCTSTR(fileNameDyn), LPCTSTR(tempBuf2));
						DEBUG_EXEC(printf("\n%s",buffer));
						system(buffer);
						break;
					};
					// QuantizeAllhandler(LPCTSTR(fileNameDyn), LPCTSTR(outputDirDyn) );
				}
			}
			count++;
		} while( FindNextFile(findHandle,&findData ) );
			
		FindClose(findHandle);
	}
}

void FillDTXHandler( const char* input, const char* output, char* command )
{
	char inputDir[_MAX_PATH];
	char outputDir[_MAX_PATH];

	strcpy( inputDir, input );
	strcpy( outputDir, output );

	int inputLen = strlen( inputDir );
	int outputLen = strlen( outputDir );

	if( !inputLen || !outputLen )
		return;

	if( inputDir[inputLen-1] != '\\' )
	{
		inputDir[inputLen] = '\\';
		inputDir[inputLen+1] = 0;
		inputLen++;
	}

	if( outputDir[outputLen-1] != '\\' )
	{
		outputDir[outputLen] = '\\';
		outputDir[outputLen+1] = 0;
		outputLen++;
	}

	// loop over all .tgas in input directory and child directories
	WIN32_FIND_DATA findData;
	HANDLE findHandle;
	CString startDir;

	startDir = inputDir;
	startDir += "*.*";
	findHandle = FindFirstFile( LPCTSTR(startDir), &findData );

	if( findHandle != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				// ignore directories starting with .
				if( findData.cFileName[0] != '.' )
				{
					// found a directory, recurse into it
					CString newInDir = inputDir;
					newInDir += findData.cFileName;
					CString newOutDir = outputDir;
					newOutDir += findData.cFileName;

					FillDTXHandler( newInDir, newOutDir, command );
				}
			}
			else
			{
				// it's a file, check to see if it's a tga
				char ext[_MAX_EXT];
				char file[_MAX_FNAME];

				_splitpath( findData.cFileName, NULL, NULL, file, ext );

				// make sure the extension is lower case
				for( unsigned i = 0; i < strlen( ext ); i++ )
				{
					if( isupper( ext[i] ) )
						ext[i] = tolower( ext[i] );
				}

				// found a tga, process it
				if( !strcmp( ext, ".tga" ) )
				{
					CString inFile = inputDir;
					inFile += file;
					inFile += ext;
					CString outFile = outputDir;
					outFile += file;
					outFile += ".dtx";
					
					FillDTX( inFile, outFile );
				}
			}
		}
		while( FindNextFile( findHandle, &findData ) );

		FindClose( findHandle );
	}
}

void FillDTX( const char* in, const char* out )
{
	DStream* outFile;
	DStream* inFile;

	// open the destination file
	outFile = streamsim_Open( out, "rb" );
	if( !outFile )
	{
		printf( "\nCouldn't find: %s", out );
		return;
	}

	// read the original dtx
	TextureData* dtxData = NULL;
	if( dtx_Create( outFile, &dtxData, LTTRUE, LTFALSE ) != LT_OK )
	{
		printf( "\nError opening: %s", out );
		outFile->Release();
		return;
	}

	outFile->Release();

	// open the input file
	inFile = streamsim_Open( in, "rb" );
	if( !inFile )
	{
		printf( "\nError opening: %s", in );
		dtx_Destroy( dtxData );
		return;
	}

	// load the tga file
	LoadedBitmap bitmap;
	bool createdTga = tga_Create2( inFile, &bitmap ) > 0;
	inFile->Release();

	if( !createdTga )
	{
		printf( "\nUnsupported format: %s", in );
		dtx_Destroy( dtxData );
		return;
	}

	if( dtx_IsTextureSizeValid( bitmap.m_Width ) && dtx_IsTextureSizeValid( bitmap.m_Height ) )
	{
		outFile = streamsim_Open( out, "wb" );
		if( outFile )
		{
			if( FillTextureWithPcx( &bitmap, dtxData ) )
			{
				if( LT_OK != dtx_Save( dtxData, outFile ) )
				{
					printf( "\nError saving: %s", out );
				}
			}
			else
			{
				printf( "\nCouldn't convert: %s", in );
			}

			outFile->Release();
		}
		else
		{
			printf( "\nCouldn't write to: %s", out );
		}
	}
	else
	{
		printf( "\nBad texture dimensions: %s", in );
	}

	dtx_Destroy( dtxData );
}

// I know, this looks bad but there's a need to share some of these functions
// with dtxutil and it's a command line utility... getting that to compile was
// already a spoofing effort (ask terry what spoofing means).  Therefore I 
// didn't want TextureHelper.cpp to include any include files and the file that 
// includes TextureHelper.cpp should take care of that.  joseph@lith.com
#include "TextureHelper.cpp"

