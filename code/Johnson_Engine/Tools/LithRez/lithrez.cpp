
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <string.h>
#include <direct.h>
#include <time.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <fcntl.h>
#include "rezutils.h"

#define kMaxStr 2048

//---------------------------------------------------------------------------------------------------
// Get the size of a file
unsigned long int GetFileSize(char* sFileName) {
   struct _stat buf;
   int fh, result;
   char buffer[] = "A line to output";

   // open the file
   if( (fh = _open( sFileName, _O_RDONLY )) ==  -1 ) {
       return 0;
   }

   // get file information
   result = _fstat( fh, &buf );

   _close( fh );

   // check if stat worked ok
   if( result != 0 ) {
      printf( "ERROR! Unable to get file size!\n" );
      return 0;
   }

   // close the file
   _close( fh );

   return buf.st_size;
}


//---------------------------------------------------------------------------------------------------
// Display program help
void DisplayHelp() {
  printf("\nLITHREZ 1.02 (Feb-28-2002) Copyright (C) 2002 Monolith Productions Inc.\n");
  printf("\nUsage: LITHREZ <command> <rez file name> [parameters]\n");
  printf("\nCommands: c <rez file name> <root directory to read> - Create");
  printf("\n          v <rez file name>                          - View");
  printf("\n          x <rez file name> <directory to output to> - Extract");
  printf("\nOption:   v                                          - Verbose");
  printf("\nOption:   z                                          - Warn zero len");
  printf("\nOption:   l                                          - Lower case ok\n");
  printf("\nExample: lithrez cv foo.rez c:\\foo");
  printf("\n         (would create rez file foo.rez from the contents of the");
  printf("\n          directory \"c:\\foo\" the verbose opion would be turned on)\n\n");  
};

//---------------------------------------------------------------------------------------------------
// The main program
int main ( int argc, char *argv[ ], char *envp[ ]) {

  // if not enough input parameters
  if (argc < 3){
    DisplayHelp();
    return 1;
  };

  // check again for last parameter if necessary

  char cModeParam = toupper(argv[1][0]);

  if ((argc < 4) && ( IsCommandSet('X', argv[1]) || IsCommandSet('C', argv[1]) ) )
  {
    DisplayHelp();
    return 1;
  }

  // get input parameters
  char sRezFile[kMaxStr];
  strcpy(sRezFile,argv[2]);
  strupr(sRezFile);
  
  // call the main processing function
  int nNumItems = RezCompiler(argv[1], argv[2], argv[3], TRUE);

  // output file size
  printf("Rez File Size = %lu\n",GetFileSize(sRezFile));

  // wait for keypress to end program (for debugging!)
#ifdef _DEBUG
  printf("\nPress a key to exit program!");
  _getch();
#endif

  return 0;
}
