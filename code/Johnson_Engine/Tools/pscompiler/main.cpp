#include "commandlineparser.h"

#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <stdarg.h>

//for temporary file support
#include <windows.h>

#include <d3dx8.h>

//file version of the pixel shader
#define PIXELSHADER_FILE_VERSION		1

//Given an input and an output filename, this will compile the file and save it out
//if successful
static bool CompileFile(const char* pszFile, const char* pszOut, bool bPrecompile)
{
	char pszActualFile[_MAX_PATH + 1];
	strncpy(pszActualFile, pszFile, _MAX_PATH);

	//first off see if we need to precompile the file
	if(bPrecompile)
	{
		//we do. Get the temporary file name
		char pszPath[_MAX_PATH + 1];
		GetTempPath(sizeof(pszPath), pszPath);

		char pszTemp[_MAX_PATH + 1];
		GetTempFileName(pszPath, "ps", 0, pszTemp);

		char pszCmd [_MAX_PATH + 1];
		_snprintf(pszCmd, sizeof(pszCmd), "cl /nologo /E %s > %s", pszFile, pszTemp);
	
		//prepend our precompiling since the compiler will spit out the filename
		cout << "Precompiling ";
		//make sure that the io stream is valid though
		cout.flush();

		if(system(pszCmd))
		{
			cout << "Error precompiling the pixel shader, make sure that VC is installed and setup correctly." << endl;
			return false;
		}
		//make sure our output won't be mangled with the compilers
		cout.flush();

		//copy over the temp name to our actual file
		strncpy(pszActualFile, pszTemp, _MAX_PATH);
	}


	LPD3DXBUFFER pOutputBuffer;
	LPD3DXBUFFER pErrors;

	HRESULT hr = D3DXAssembleShaderFromFile(pszActualFile, 0, NULL, &pOutputBuffer, &pErrors);

	//see if we failed
	if(FAILED(hr) || !pOutputBuffer)
	{
		//display the errors
		cout << "Compile failure: " << hr << endl;
		cout << "-----------------------------" << endl;
		
		if(pErrors)
		{
			cout << ((const char*)pErrors->GetBufferPointer());
			cout << endl;
			pErrors->Release();
		}

		if(pOutputBuffer)
			pOutputBuffer->Release();

		return false;
	}
	
	//we don't need our error buffer anymore
	if(pErrors)
		pErrors->Release();

	//we succeeded, build up the file header
	uint32 nFileCode	= MAKEFOURCC('L', 'T', 'P', 'S');
	uint32 nVersion		= PIXELSHADER_FILE_VERSION;
	uint32 nBufferSize	= pOutputBuffer->GetBufferSize();

	//open up the output file
	ofstream OutFile(pszOut);

	if(!OutFile.good())
	{
		cout << "Error opening " << pszOut << " for output. Please make sure the directory exists and the file is not read only." << endl;
		pOutputBuffer->Release();
		return false;
	}

	//file is opened, lets write it out
	OutFile.write((const char*)&nFileCode, sizeof(nFileCode));
	OutFile.write((const char*)&nVersion, sizeof(nVersion));
	OutFile.write((const char*)&nBufferSize, sizeof(nBufferSize));
	OutFile.write((const char*)pOutputBuffer->GetBufferPointer(), pOutputBuffer->GetBufferSize());

	//all done
	OutFile.close();
	pOutputBuffer->Release();

	cout << pszOut << " successfully compiled." << endl;
	return true;
}

//displays proper command line usage of this tools
static void DisplayUsage(const char* pszError)
{
	cout << "PSCompiler /file <file to compile> [/out <name of output file>] [/help] [/noprecompile]" << endl;
	cout << endl;
	cout << pszError << endl;
}

//Main, handles parameter parsing and verification
int main(int nArgCount, char** ppArgs)
{
	//init our command line parser
	CCommandLineParser Parser;
	Parser.Init(nArgCount - 1, &(ppArgs[1]), '/');

	//setup our options
	// /file <filename>
	Parser.SetNumParameters("file", 1);

	// /out <filename>
	Parser.SetNumParameters("out", 1);

	// /help
	Parser.SetNumParameters("help", 0);

	// /precompile
	Parser.SetNumParameters("noprecompile", 0);

	//see if the user wants help
	if(Parser.IsOptionSet("help"))
	{
		//display command line usage and bail
		DisplayUsage("");
		return 1;
	}

	//--------------------------
	// Option verification

	//scan for unexpected options
	for(uint32 nCurrOption = 0; nCurrOption < Parser.GetNumOptions(); nCurrOption++)
	{
		const char* pszOptName = Parser.GetOptionName(nCurrOption);

		if(	stricmp(pszOptName, "file") &&
			stricmp(pszOptName, "out") &&
			stricmp(pszOptName, "help") &&
			stricmp(pszOptName, "noprecompile"))
		{
			DisplayUsage("");
			cout << "Unknown option: " << pszOptName << endl;
			return 1;
		}
	}

	//make sure that we have a file to compile
	if(!Parser.IsOptionSet("file") || (Parser.GetNumParameters("file") < 1))
	{
		DisplayUsage("A file name must be specified for compiling");
		return 1;
	}


	//make sure that if they are outputting, that they have a file
	if(Parser.IsOptionSet("out") && (Parser.GetNumParameters("out") < 1))
	{
		DisplayUsage("A file name must be specified for output");
		return 1;
	}

	//determine the output file name
	char pszOutFile[_MAX_PATH + 1];

	if(Parser.IsOptionSet("out"))
	{
		//just read it in from what was specified
		strncpy(pszOutFile, Parser.GetParameter("out", 0), _MAX_PATH);
	}
	else
	{
		//not specified, just use the input name and switch the extension
		strncpy(pszOutFile, Parser.GetParameter("file", 0), _MAX_PATH);

		//start at the end, and work back to the period
		char* pszEnd = pszOutFile + strlen(pszOutFile);

		while(	(pszEnd >= pszOutFile) && 
				(*pszEnd != '.') && 
				(*pszEnd != '\\') &&
				(*pszEnd != '/'))
					pszEnd--;

		//see if we hit the beginning or a period
		if(*pszEnd == '.')
		{
			strcpy(pszEnd, ".ps");
		}
		else
		{
			strncat(pszOutFile, ".ps", _MAX_PATH);
		}
	}
			
	//Ok, we are good to go. Compile away.
	if(!CompileFile(Parser.GetParameter("file", 0), pszOutFile, !Parser.IsOptionSet("noprecompile")))
		return 1;

	return 0;
}