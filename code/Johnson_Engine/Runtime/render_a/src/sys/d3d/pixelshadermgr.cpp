#include "precompile.h"
#include "pixelshadermgr.h"
#include "d3d_device.h"

//Interface for the client file manager
#include "client_filemgr.h"		
static IClientFileMgr* g_pIClientFileMgr;
define_holder(IClientFileMgr, g_pIClientFileMgr);

//the current file version of the pixel shaders
#define PIXELSHADER_FILE_VERSION		1

CPixelShaderMgr::~CPixelShaderMgr()
{
	//free all existing shaders
	for(uint32 nCurrShader = 0; nCurrShader < m_cShaderList.size(); nCurrShader++)
	{
		FreeShader(nCurrShader);
	}
}

//singleton access
CPixelShaderMgr& CPixelShaderMgr::GetSingleton()
{
	static CPixelShaderMgr s_Singleton;
	return s_Singleton;
}

//loads a pixel shader from disk and assigns it a unique id which it can be referred to by. This
//will return an invalid handle if the load failed or the device doesn't support it. Specifying true
//for the auto cleanup indicates that it should not worry about reference counting and that there
//is basically a static shader being used that is safe to be removed at the end of the application
CPixelShaderHandle	CPixelShaderMgr::LoadPixelShader(const char* pszFilename, bool bAutoCleanup)
{
	//run through and see if we have a pixel shader that is this name (also looking for a place
	//to put it if we don't)
	uint32 nPutAt = (uint32)-1;

	CPixelShaderHandle rv;

	for(uint32 nCurrShader = 0; nCurrShader < m_cShaderList.size(); nCurrShader++)
	{
		if(m_cShaderList[nCurrShader])
		{
			if(stricmp(m_cShaderList[nCurrShader]->m_pFilename, pszFilename) == 0)
			{
				//we have a match
				rv.m_nID = nCurrShader;

				//update our reference count
				m_cShaderList[nCurrShader]->m_nRefCount++;

				return rv;
			}
		}
		else
		{
			nPutAt = nCurrShader;
		}
	}

	//alright, we need to create a new shader, so first we load it
	FileRef Ref; 
	Ref.m_FileType			= TYPECODE_UNKNOWN;
	Ref.m_pFilename			= pszFilename;
	FileIdentifier* pIdent	= g_pIClientFileMgr->GetFileIdentifier(&Ref, TYPECODE_UNKNOWN);

	//open up the stream
	ILTStream* pStream = g_pIClientFileMgr->OpenFile(&Ref);

	//see if the stream was successfully opened
	if (!pStream)
	{
		//failed to open the file
		return rv;
	}

	//alright, we now have the file opened, we need to read in our tag
	uint32 nTag;
	*pStream >> nTag;

	if(nTag != MAKEFOURCC('L', 'T', 'P', 'S'))
	{
		//invalid file
		pStream->Release();
		return rv;
	}

	//read in the version
	uint32 nVersion;
	*pStream >> nVersion;

	//sanity check on the file version
	if(nVersion != PIXELSHADER_FILE_VERSION)
	{
		pStream->Release();
		return rv;
	}

	//alright, now we can read the data size
	uint32 nDataSize;
	*pStream >> nDataSize;

	//we now have the size of the data, allocate room for it
	uint8* pData;
	LT_MEM_TRACK_ALLOC(pData = new uint8[nDataSize], LT_MEM_TYPE_RENDERER);

	//make sure we have the memory
	if(!pData)
	{
		pStream->Release();
		return rv;
	}

	//read in the data
	pStream->Read(pData, nDataSize);

	//we can close the file now
	pStream->Release();

	//we have an object here, we need to create this pixel shader
	DWORD nShaderHandle;
	if(FAILED(PD3DDEVICE->CreatePixelShader((DWORD*)pData, &nShaderHandle)))
	{
		delete [] pData;
		return rv;
	}

	//alright, so the device was even created successfully, lets try to install it once for good measure
	if(FAILED(PD3DDEVICE->SetPixelShader(nShaderHandle)))
	{
		//failed to setup the shader, it isn't supported
		delete [] pData;
		PD3DDEVICE->DeletePixelShader(nShaderHandle);
		return rv;
	}

	//we successfully installed it, so clear it out now
	PD3DDEVICE->SetPixelShader(NULL);
	
	//alright, everything is working out, so lets go ahead and create our entry into the table
	SPixelShader* pNewShader;
	LT_MEM_TRACK_ALLOC(pNewShader = new SPixelShader(pszFilename, pData, nShaderHandle, bAutoCleanup), LT_MEM_TYPE_RENDERER);

	//see if we can reuse an existing slot
	if(nPutAt < m_cShaderList.size())
	{
		rv.m_nID = nPutAt;
		m_cShaderList[nPutAt] = pNewShader;
	}
	else
	{
		//we can't
		rv.m_nID = m_cShaderList.size();
		m_cShaderList.push_back(pNewShader);
	}			

	return rv;
}

//frees all the device pixel shaders
void CPixelShaderMgr::FreeDeviceObjects()
{
	for(TShaderList::iterator it = m_cShaderList.begin(); it != m_cShaderList.end(); it++)
	{
		//see if this is an object and it has a valid shader
		if(*it && (*it)->m_bValidShader)
		{
			//we have an object here, we need to free its device object
			PD3DDEVICE->DeletePixelShader((*it)->m_nShaderHandle);
			(*it)->m_bValidShader = false;
		}
	}
}

//recreates all the pixel shaders. This is necessary when the device changes
void CPixelShaderMgr::RecreatePixelShaders()
{
	//we need to run through the list and recreate each and every pixel shader
	for(TShaderList::iterator it = m_cShaderList.begin(); it != m_cShaderList.end(); it++)
	{
		//see if this is an object
		if(*it)
		{
			//we have an object here, we need to create this pixel shader
			if(SUCCEEDED(PD3DDEVICE->CreatePixelShader((DWORD*)(*it)->m_pData, &((*it)->m_nShaderHandle))))
			{
				(*it)->m_bValidShader = true;
			}
		}
	}
}

//sets the pixel shader constants. This takes the number of pixel shader constants you want to supply,
//and a list of those constants (each constant is a set of 4 floats)
bool CPixelShaderMgr::SetPixelShaderConstants(uint32 nNumConstants, const float* pConstants, uint32 nStartReg) const
{
	//setup the constants
	if((nNumConstants > 0) && pConstants)
	{
		if(SUCCEEDED(PD3DDEVICE->SetPixelShaderConstant(nStartReg, pConstants, nNumConstants)))
		{
			return true;
		}
	}
	return false;
}

//installs the specified pixel shader into the device, returning a pointer to the pixel shader
//that was already installed. This takes the number of pixel shader constants you want to supply,
//and a list of those constants (each constant is a set of 4 floats)
bool CPixelShaderMgr::InstallPixelShader(const CPixelShaderHandle& Shader) const
{
	//make sure that the shader is valid
	assert(Shader.m_nID < m_cShaderList.size());
	assert(m_cShaderList[Shader.m_nID] != NULL);

	if(m_cShaderList[Shader.m_nID]->m_bValidShader)
	{
		//now setup the pixel shader
		if(SUCCEEDED(PD3DDEVICE->SetPixelShader(m_cShaderList[Shader.m_nID]->m_nShaderHandle)))
		{
			return true;
		}
	}
	return false;
}

//uninstalls any currently installed pixel shaders
void CPixelShaderMgr::UninstallPixelShaders() const
{
	PD3DDEVICE->SetPixelShader(NULL);
}

//releases a pixel shader
bool CPixelShaderMgr::ReleasePixelShader(const CPixelShaderHandle& Shader)
{
	//make sure the shader is valid
	assert(Shader.m_nID < m_cShaderList.size());
	assert(m_cShaderList[Shader.m_nID] != NULL);

	//decrement the reference count
	m_cShaderList[Shader.m_nID]->m_nRefCount--;

	//see if we have hit 0
	if(m_cShaderList[Shader.m_nID]->m_nRefCount == 0)
	{
		FreeShader(Shader.m_nID);
	}

	//success
	return true;
}

//frees the specified shader
void CPixelShaderMgr::FreeShader(uint32 nIndex)
{
	if(!m_cShaderList[nIndex])
		return;

	//we need to remove this one from the list

	//first reclaim any device objects
	if(m_cShaderList[nIndex]->m_bValidShader)
	{
		//it is a valid shader, reclaim it
		PD3DDEVICE->DeletePixelShader(m_cShaderList[nIndex]->m_nShaderHandle);
	}

	//reclaim the memory
	delete m_cShaderList[nIndex];
	m_cShaderList[nIndex] = NULL;
}


CPixelShaderMgr::SPixelShader::SPixelShader(const char* pszFilename, uint8* pData, DWORD nShaderHandle, bool bAutoCleanup) :
	m_nShaderHandle(nShaderHandle),
	m_pData(NULL),
	m_pFilename(NULL),
	m_nRefCount(1),
	m_bValidShader(true),
	m_bAutoCleanup(bAutoCleanup)
{
	//allocate memory for the filename
	uint32 nStrLen = strlen(pszFilename);
	LT_MEM_TRACK_ALLOC(m_pFilename = new char [nStrLen + 1], LT_MEM_TYPE_RENDERER);

	//copy over the string so we now own our filename
	if(m_pFilename)
		LTStrCpy(m_pFilename, pszFilename, nStrLen + 1);

	//adopt the data passed in to us
	m_pData = pData;
}

CPixelShaderMgr::SPixelShader::~SPixelShader()
{
	//we should only free this when there are no references to it
	assert(m_bAutoCleanup || (m_nRefCount == 0));
	delete [] m_pData;
	m_pData = NULL;

	delete [] m_pFilename;
	m_pFilename = NULL;

	m_nRefCount		= 0;
	m_nShaderHandle = 0;
}

