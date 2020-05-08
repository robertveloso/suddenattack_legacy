// ------------------------------------------------------------------------
// ltbhead
// read the ltb header information
// this program is cross-platform, and engine indepenent
// lithtech (c) 2001
// ------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

const char *kVersion ="3.0";

// File types...
enum E_LTB_FILE_TYPES {
	LTB_D3D_MODEL_FILE			= 1,
	LTB_PS2_MODEL_FILE			= 2,
	LTB_XBOX_MODEL_FILE			= 3,
	LTB_ABC_MODEL_FILE			= 4,
	LTB_D3D_RENDERSTYLE_FILE	= 5
};

// matching string descriptions
char  *STR_LTB_FILE_TYPES[]=  {
	"Unknown file type",
	"D3D_MODEL_FILE",
	"PS2_MODEL_FILE",
	"XBOX_MODEL_FILE",
	"ABC_MODEL_FILE",
	"D3D_RENDERSTYLE_FILE"
};

// number of file types
const int kFileTypesSize = 5;

// LTB header 
struct LTB_Header {
	LTB_Header()							{ m_iFileType = 0; m_iVersion = 0; }
	unsigned char  m_iFileType;						// Should be one of E_LTB_FILE_TYPES
	unsigned short m_iVersion;						// Version number...
	unsigned char  m_iReserved1;					// Reserved for stuff we thing of later...
	unsigned int m_iReserved2;
	unsigned int m_iReserved3;
	unsigned int m_iReserved4;
};

// ------------------------------------------------------------------------
// program usage 
// ------------------------------------------------------------------------
void PrintUsage( char *program_name )
{
	printf("Usage :\n%s ltb-file\n", program_name);
	printf("\tThis program prints out the ltb header information of an ltb file.\n");
	printf("\tProgram output : \n\t\tFile_type file_version, i.e D3D_MODEL_FILE 2.\n");
	printf("\t\tUNKNOWN 0, if ltb header info is not recognized.\n");
	printf("\t\tERROR -1, if there is no ltb header.\n\n");
	printf("%s\t\t[%s]\n",__DATE__, kVersion);
}

// ------------------------------------------------------------------------
// prints out the ltb header .
// ------------------------------------------------------------------------
void PrintHeader( LTB_Header & header )
{
	int iFileTypeIndex = header.m_iFileType;

	if( iFileTypeIndex > 0 && iFileTypeIndex <= kFileTypesSize )
	{
		printf("%s %i\n",  STR_LTB_FILE_TYPES[header.m_iFileType] ,
										header.m_iVersion );
	}else 
	{
		printf("UNKNOWN 0\n");
	}
}


// ------------------------------------------------------------------------
// read lt style bin string
// format = { uint16(size) char[size](data) }
// returns size of string read.
// ------------------------------------------------------------------------
int ReadString( FILE *file, char *pStr, int maxBytes )
{
  unsigned int  maxChars;
  unsigned short len;
  
  fread( &len, sizeof(unsigned short) , 1, file );

  if(len == 0)
  {
          pStr[0] = 0;
          return 0;
  }

  if(maxBytes == 0)
  {
	  char *dead_buf = new char [ len ];
	  fread( dead_buf, len, 1, file );
      delete [] dead_buf ;    
      return 0;
  }
  else
  {
	maxChars = maxBytes - 1;
	int read_size = 0;
	if(len > maxChars)
	{
		  read_size = fread( pStr,  maxChars, 1, file );
		  pStr[maxChars] = 0;
		  len -= (unsigned short)maxChars;

		  char *dead_buf = new char [ len ];
		  fread( dead_buf , len, 1, file );
		  delete [] dead_buf ;
	}
	else
	{
	  read_size = fread( pStr, len, 1, file );    
	  pStr[len] = 0;
	}

	return read_size ;
  }
}
 
// ------------------------------------------------------------------------
// seek_to_section( file , section-name )
// finds the named section in the ltb file.
// returns 0 on failure.
// ------------------------------------------------------------------------
bool SeekToSection( FILE *file, char *pSectionName)
{
	unsigned int nextSectionPos, i, maxIterations;
	char sectionName[256];

	fseek( file, 0,0);	
	// Prevent it from going forever.
	maxIterations = 512;

	for(i=0; i < maxIterations; i++)
	{
		ReadString( file, sectionName, sizeof(sectionName) );
		fread( &nextSectionPos, sizeof(unsigned int), 1, file );
		if(strcmp(pSectionName, sectionName) == 0) {
			return 1;
		}
		if(nextSectionPos == (unsigned int)-1)
			return 0;
		fseek( file, nextSectionPos , 0 );
	}
	return 1;
}


// ------------------------------------------------------------------------
// read in file, open file, get header, print out header, stop
// 
// * what if  the file is not really an ltb file, but is binary?
// ------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	if( argc <= 1 )
	{
		PrintUsage( argv[0] );
		return 1;
	}

	char *filename = argv[1];
	FILE *ltb_file = fopen( filename, "rb" );

	if( ltb_file != NULL )
	{
		LTB_Header header ;
	
		if( SeekToSection( ltb_file, "LTBHeader" ) )
		{
			fread( (void*)(&header), sizeof(LTB_Header),1,ltb_file );
			PrintHeader( header );
		}
		else 
		{
			//printf("Error %s has no LTB HEADER info. \n" , filename );
			printf("ERROR -1\n");
		}
		return 0;

	}else
	{
		printf("Could not open file : %s", filename );
		return 1;
	}

	return 0;
}
