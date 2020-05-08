//------------------------------------------------------------------
//
//  FILE      : LTPixelShaderMgr.cpp
//
//  PURPOSE   :	Pixel shader manager singleton
//
//  COPYRIGHT : LithTech Inc., 1996-2003
//
//------------------------------------------------------------------

#include "precompile.h"
#include "ltpixelshadermgr.h"
#include "d3d_device.h"
#include "d3d_utils.h"


// the current file version of the pixel shaders (only needed for old lithtech pixel shader files)
#define PIXELSHADER_FILE_VERSION		1



//-------------------------------------------------------------------------------------------------
// LTPixelShaderImp
//-------------------------------------------------------------------------------------------------


// Init -- load the pixel shader byte code from file.
//
bool LTPixelShaderImp::Init(ILTStream *pStream)
{
	Term();

	ZeroMemory(m_Constants, sizeof(m_Constants));

	// Try to load the old lithtech pixel shader format.
	uint32 nTag;
	*pStream >> nTag;
	if (nTag == MAKEFOURCC('L', 'T', 'P', 'S'))
	{
		//
		// This is the old lithtech pixel shader format.
		//

		// Read in the version.
		uint32 nVersion;
		*pStream >> nVersion;

		// Sanity check on the file version.
		if (nVersion != PIXELSHADER_FILE_VERSION)
		{
			return false;
		}

		// Alright, now we can read the data size.
		uint32 nDataSize;
		*pStream >> nDataSize;

		// We now have the size of the data, allocate room for it.
		LT_MEM_TRACK_ALLOC(m_pByteCode = new uint8[nDataSize], LT_MEM_TYPE_RENDER_SHADER);
		if (m_pByteCode == NULL)
		{
			return false;
		}
    	ZeroMemory(m_pByteCode, nDataSize);

    	// Read the pre-compiled pixel shader microcode
		if (pStream->Read(m_pByteCode, nDataSize) != LT_OK)
		{
			return false;
		}
	}
	else
	{
		//
		// This is a regular microsoft pixel shader.
		//

		pStream->SeekTo(0);

		// Allocate memory to read the pixel shader file.
    	DWORD dwSize = pStream->GetLen();
		LT_MEM_TRACK_ALLOC(m_pByteCode = new uint8[dwSize + 4], LT_MEM_TYPE_RENDER_SHADER);
    	if (m_pByteCode == NULL)
		{
			return false;
		}
    	ZeroMemory(m_pByteCode, dwSize + 4);

    	// Read the pre-compiled pixel shader microcode
		if (pStream->Read(m_pByteCode, dwSize) != LT_OK)
		{
			return false;
		}
	}

	// Create the pixel shader handle.
	return Recreate();
}



bool LTPixelShaderImp::Recreate()
{
	FreeDeviceObject();

	if (m_pByteCode == NULL)
	{
		return false;
	}

    // Create the pixel shader.
	HRESULT hr = PD3DDEVICE->CreatePixelShader((const DWORD*)m_pByteCode, &m_Handle);
	if (FAILED(hr))
	{
		return false;
	}

	// Try to install the pixel sahder once for good measure.
	if (FAILED(PD3DDEVICE->SetPixelShader(m_Handle)))
	{
		// Failed to setup the shader; it isn't supported.
		FreeDeviceObject();
		return false;
	}

	// We successfully installed it, so clear it out now.
	PD3DDEVICE->SetPixelShader(NULL);


	return true;
}



void LTPixelShaderImp::Term()
{
	FreeDeviceObject();

	if (m_pByteCode != NULL)
	{
		delete [] m_pByteCode;
		m_pByteCode = NULL;
	}
}



void LTPixelShaderImp::FreeDeviceObject()
{
	if (m_Handle != 0)
	{
		PD3DDEVICE->DeletePixelShader(m_Handle);
		m_Handle = 0;
	}
}



void LTPixelShaderImp::SetName(const char *pName)
{
	LTStrCpy(m_FileName, pName, MAX_PATH);
}



bool LTPixelShaderImp::GetConstant(unsigned RegisterNum, float *pf0, float *pf1, float *pf2, float *pf3)
{
	if (RegisterNum >= MAX_CONSTANT_REGISTERS)
	{
		return false;
	}

	unsigned i = RegisterNum*4;
	*pf0 = m_Constants[i++];
	*pf1 = m_Constants[i++];
	*pf2 = m_Constants[i++];
	*pf3 = m_Constants[i++];

	return true;
}



bool LTPixelShaderImp::SetConstant(unsigned RegisterNum, float f0, float f1, float f2, float f3)
{
	if (RegisterNum >= MAX_CONSTANT_REGISTERS)
	{
		return false;
	}

	unsigned i = RegisterNum*4;
	m_Constants[i++] = f0;
	m_Constants[i++] = f1;
	m_Constants[i++] = f2;
	m_Constants[i++] = f3;

	return true;
}



bool LTPixelShaderImp::SetConstant(unsigned RegisterNum, const LTMatrix &Matrix)
{
	if (RegisterNum + 3 >= MAX_CONSTANT_REGISTERS)
	{
		return false;
	}

	unsigned iReg = RegisterNum*4;
	for (unsigned i = 0; i < 4; ++i)
	{
		for (unsigned j = 0; j < 4; ++j)
		{
			m_Constants[iReg++] = Matrix.m[i][j];
		}
	}

	return true;
}



//-------------------------------------------------------------------------------------------------
// LTPixelShaderMgr
//-------------------------------------------------------------------------------------------------


LTPixelShaderMgr::LTPixelShaderMgr()
{
	m_bHardwareSupport = g_Device.GetDeviceCaps()->PixelShaderVersion >= 10;
}



LTPixelShaderMgr::~LTPixelShaderMgr()
{
	Term();
}



void LTPixelShaderMgr::Term()
{
	// Delete the pixel shaders.
	m_PixelShaders.Term();
}



LTPixelShaderMgr& LTPixelShaderMgr::GetSingleton()
{
	static LTPixelShaderMgr s_Singleton;
	return s_Singleton;
}



bool LTPixelShaderMgr::AddPixelShader(ILTStream *pStream, const char *ShaderName, int ShaderID)
{
	// Check to see if this pixel shader has already been added.
	if (m_PixelShaders.Get(ShaderID) != LTNULL)
	{
		return false;
	}

	// Create and add the pixel shader to the list.
	LTPixelShaderImp *pShader = m_PixelShaders.Add(ShaderName, ShaderID);
	if (pShader == LTNULL)
	{
		return false;
	}

	// Initialize the shader.
	return pShader->Init(pStream);
}



void LTPixelShaderMgr::RemovePixelShader(int ShaderID)
{
	LTPixelShaderImp *pShader = m_PixelShaders.Get(ShaderID);
	if (pShader != LTNULL)
	{
		// Now remove the pixel shader.
		m_PixelShaders.Remove(ShaderID);
	}
}



void LTPixelShaderMgr::RemoveAllPixelShaders()
{
	m_PixelShaders.Term();
}



LTPixelShader*	LTPixelShaderMgr::GetPixelShader(int ShaderID)
{
	LTPixelShaderImp *pShader = m_PixelShaders.Get(ShaderID);
	return pShader;
}



void LTPixelShaderMgr::FreeDeviceObjects()
{
	LTPixelShaderImp *pShader = (LTPixelShaderImp*)(m_PixelShaders.GetFront());
	while (pShader != NULL)
	{
		pShader->FreeDeviceObject();
		pShader = (LTPixelShaderImp*)(pShader->GetNext());
	}
}



void LTPixelShaderMgr::RecreatePixelShaders()
{
	LTPixelShaderImp *pShader = (LTPixelShaderImp*)(m_PixelShaders.GetFront());
	while (pShader != NULL)
	{
		pShader->Recreate();
		pShader = (LTPixelShaderImp*)(pShader->GetNext());
	}
}



bool LTPixelShaderMgr::SetPixelShaderConstants(LTPixelShader *pShader)
{
	LTPixelShaderImp *pShaderImp = (LTPixelShaderImp*)pShader;

	// Make sure we have valid data.
	if (pShaderImp == NULL ||
		!pShaderImp->IsValidShader())
	{
		return false;
	}

	// Get the constants.
	float *pConstants = pShaderImp->GetConstants();
	if (pConstants == NULL)
	{
		return false;
	}

	// Set the user-defined constants.
	HRESULT hr = PD3DDEVICE->SetPixelShaderConstant(0, pConstants, LTPixelShader::MAX_CONSTANT_REGISTERS);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}



bool LTPixelShaderMgr::InstallPixelShader(LTPixelShader *pShader) const
{
	LTPixelShaderImp *pShaderImp = (LTPixelShaderImp*)pShader;
	if (pShaderImp == NULL ||
	    !pShaderImp->IsValidShader())
	{
		return false;
	}

	// Setup the pixel shader.
	HRESULT hr = PD3DDEVICE->SetPixelShader(pShaderImp->GetHandle());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}



void LTPixelShaderMgr::UninstallPixelShader() const
{
	PD3DDEVICE->SetPixelShader(NULL);
}
