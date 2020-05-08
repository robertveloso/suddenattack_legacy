#include "commandlineparser.h"
#include "ltamgr.h"
#include <iostream.h>

#define OPTION_DELIMITER		'-'

//prints out the help screen for the tool
void PrintHelp()
{
	cout << "LTC - A tool for compression and decompression of LTA files" << endl;
	cout << "Usage: LTC <filename> [-out <file>] [-view] [-verbose] [-help]" << endl;
	cout << endl;
	cout << "<filename>         - The file to be changed. It must have the" << endl;
	cout << "                     extension LTA or LTC." << endl;
	cout << "-out <file>        - Specifies the file to output the data (if" << endl;
	cout << "                     none is specified, it will toggle the" << endl;
	cout << "                     extension between LTA and LTC)." << endl;
	cout << "-view              - Prints out the text version of the file" << endl;
	cout << "                     to standard output." << endl;
	cout << "-verbose           - Enables printing out of information" << endl;
	cout << "-help              - Displays help" << endl;
	cout << endl;

}

int main(int nArgCount, char** ppArgs)
{

	//get the command line parameters
	CCommandLineParser	Parser;

	//initialize the parser, skipping over the program name
	Parser.Init(nArgCount - 1, &(ppArgs[1]), OPTION_DELIMITER);

	//trim off extra parameters from all the options

	// /out <filename>
	Parser.SetNumParameters("out", 1);

	// /verbose
	Parser.SetNumParameters("verbose", 0);

	// /help
	Parser.SetNumParameters("help", 0);

	// /view
	Parser.SetNumParameters("view", 0);


	//see if the user wants help
	if(Parser.IsOptionSet("help"))
	{
		PrintHelp();
		return 0;
	}

	//--------------------------
	// Option verification

	//scan for unexpected options
	for(uint32 nCurrOption = 0; nCurrOption < Parser.GetNumOptions(); nCurrOption++)
	{
		const char* pszOptName = Parser.GetOptionName(nCurrOption);

		if(	stricmp(pszOptName, "out") &&
			stricmp(pszOptName, "verbose") &&
			stricmp(pszOptName, "help") &&
			stricmp(pszOptName, "view"))
		{
			PrintHelp();
			cout << "Unknown option: " << pszOptName << endl;
			return 1;
		}
	}

	//make sure that we have a filename
	if(Parser.GetNumGlobalParameters() == 0)
	{
		PrintHelp();
		cout << "A file to modify must be specified" << endl;
		return 1;
	}

	//make sure that if they are outputting, that they have a file
	if(Parser.IsOptionSet("out") && (Parser.GetNumParameters("out") < 1))
	{
		PrintHelp();
		cout << "A file name must be specified with -out" << endl;
		return 1;
	}


	//make sure that they aren't trying to view, and output
	if(Parser.IsOptionSet("view") && Parser.IsOptionSet("out"))
	{
		PrintHelp();
		cout << "-view and -out cannot be set. Please select only one." << endl;
		return 1;
	}

	//determine if we want verbose mode
	bool bVerbose = Parser.IsOptionSet("verbose");

	//see if they want to view the file
	bool bView = Parser.IsOptionSet("view");

	//get the filename
	const char* pszFilename = Parser.GetGlobalParameter(0);

	//determine if the input file is compressed
	bool bInFileCompressed = CLTAUtil::IsFileCompressed(pszFilename);

	//see if we can even open up the file
	CLTAFile	InFile;

	InFile.Open(pszFilename, true, bInFileCompressed);

	//make sure we opened it okay
	if(InFile.IsValid() == false)
	{
		cout << "Error: Unable to open the file " << pszFilename << endl;
		return 1;
	}

	//now we need to see if we are printing it out, or are writing it out to a file
	if(bView)
	{
		uint8 nVal;

		//print out the entire file
		while(InFile.ReadByte(nVal))
		{
			cout << (char)nVal;
		}

		//close it out
		InFile.Close();

		return 0;
	}

	//they don't want to view, they want to compress, lets figure out the file name
	char pszOutFile[_MAX_PATH];

	if(Parser.IsOptionSet("out"))
	{
		strcpy(pszOutFile, Parser.GetParameter("out", 0));
	}
	else
	{
		//strip out the extension
		int32 nStrLen = strlen(pszFilename);

		for(int32 nCurrChar = nStrLen - 1; nCurrChar >= 0; nCurrChar--)
		{
			if(pszFilename[nCurrChar] == '.')
			{
				break;
			}
		}

		//make sure we have a valid extension
		if((nCurrChar < 0) || (	stricmp(&pszFilename[nCurrChar], ".LTA") && 
								stricmp(&pszFilename[nCurrChar], ".LTC")))
		{
			cout << "Error: The input file does not have a proper extension." << endl;
			cout << "Either specify an output filename or correct the extension." << endl;
			return 1;
		}

		//now compose the final string
		strcpy(pszOutFile, pszFilename);
		strcpy(&(pszOutFile[nCurrChar]), bInFileCompressed ? ".lta" : ".ltc");
	}

	//now we open up the output stream and convert the file
	CLTAFile OutFile(pszOutFile, false, bInFileCompressed ? false : true);

	//make sure it succeeded
	if(OutFile.IsValid() == false)
	{
		cout << "Error: Unable to open file " << pszOutFile << " for writing." << endl;
		return 1;
	}

	//now we read in bytes until we must stop, then write out the bytes
	uint8 nVal;

	//convert the file
	while(InFile.ReadByte(nVal))
	{
		if(OutFile.WriteByte(nVal) == false)
		{
			cout << "Error: Unable to write to " << pszOutFile << endl;
			cout << "Disk may be full." << endl;
			return 1;
		}
	}

	//close it out
	OutFile.Close();
	InFile.Close();

	return 0;
}