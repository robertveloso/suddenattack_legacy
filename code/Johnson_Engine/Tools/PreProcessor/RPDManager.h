//----------------------------------------------------------------------
// RPDManager.h
//
// Provides the definition for the CRPDManager class, which handles
// the saving to and from disk of RPD (resumable processing database) 
// files. 
//
// Author: John O'Rorke
// Created: 3/22/01
// Modification History:
//
//----------------------------------------------------------------------

#ifndef __RPDMANAGER_H__
#define __RPDMANAGER_H__

//function prototype for the function used to print status information
//to the display
typedef void (*FnRPDManagerDisplay)(const char* pszMessage, ...);

//forward declarations
class CPreMainWorld;
class CAbstractIO;

class CRPDManager
{
public:

	//given a filename and a CPreMainWorld, it will save it to disk so that the
	//world can be reloaded again from disk
	static bool		Save(CAbstractIO& file, CPreMainWorld* pPreMainWorld, FnRPDManagerDisplay pfnDisplay);

	//given a filename, it will open that file, and use it to recreate the 
	//CPreWorld that was saved to disk
	static bool		Load(CAbstractIO& file, CPreMainWorld* pPreMainWorld, FnRPDManagerDisplay pfnDisplay);

	//given a filename, it will produce the appropriate filename for the RPD,
	static void		GetFilename(const char* pszInFileName, char* pszOutFileName, uint32 nLen);
private:

	//don't allow instantiation
	CRPDManager()			{}
};

#endif

