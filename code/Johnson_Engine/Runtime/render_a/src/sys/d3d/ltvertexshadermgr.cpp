//------------------------------------------------------------------
//
//  FILE      : LTVertexShaderMgr.cpp
//
//  PURPOSE   :	Vertex shader manager singleton
//
//  COPYRIGHT : LithTech Inc., 1996-2002
//
//------------------------------------------------------------------

#include "precompile.h"
#include "ltvertexshadermgr.h"
#include "d3d_device.h"
#include "d3d_utils.h"



//-------------------------------------------------------------------------------------------------
// LTVertexShaderImp
//-------------------------------------------------------------------------------------------------


bool LTVertexShaderImp::Init(ILTStream *pStream, const uint32 *pDeclaration, uint32 DeclarationSize, bool bHardwareSupport)
{
	Term();

	ZeroMemory(m_Constants, sizeof(m_Constants));

	// Allocate memory to read the vertex shader file.
    DWORD dwSize = pStream->GetLen();
	LT_MEM_TRACK_ALLOC(m_pByteCode = new uint8[dwSize + 4], LT_MEM_TYPE_RENDER_SHADER);
    if (m_pByteCode == NULL)
	{
		return false;
	}
    ZeroMemory(m_pByteCode, dwSize + 4);

    // Read the pre-compiled vertex shader microcode
	if (pStream->Read(m_pByteCode, dwSize) != LT_OK)
	{
		return false;
	}

	// Copy the declaration so that we can reconstruct the shader if needed.
	if (DeclarationSize == 0 ||
	    pDeclaration == NULL)
	{
		return false;
	}
	unsigned NumDecl = DeclarationSize/sizeof(uint32);
	LT_MEM_TRACK_ALLOC(m_pDeclaration = new DWORD[NumDecl], LT_MEM_TYPE_RENDER_SHADER);
	if (m_pDeclaration == NULL)
	{
		return false;
	}
	memcpy(m_pDeclaration, pDeclaration, DeclarationSize);

	// Create the vertex shader handle.
	return Recreate(bHardwareSupport);
}



bool LTVertexShaderImp::Recreate(bool bHardwareSupport)
{
	FreeDeviceObject();

	if (m_pByteCode == NULL ||
		m_pDeclaration == NULL)
	{
		return false;
	}

    // Create the vertex shader.
	HRESULT hr = PD3DDEVICE->CreateVertexShader(m_pDeclaration, (const DWORD*)m_pByteCode, &m_Handle,
												(bHardwareSupport ? 0 : D3DUSAGE_SOFTWAREPROCESSING));
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}



void LTVertexShaderImp::Term()
{
	FreeDeviceObject();

	if (m_pByteCode != NULL)
	{
		delete [] m_pByteCode;
		m_pByteCode = NULL;
	}

	if (m_pDeclaration != NULL)
	{
		delete [] m_pDeclaration;
		m_pDeclaration = NULL;
	}
}



void LTVertexShaderImp::FreeDeviceObject()
{
	if (m_Handle != 0)
	{
		PD3DDEVICE->DeleteVertexShader(m_Handle);
		m_Handle = 0;
	}
}



void LTVertexShaderImp::SetName(const char *pName)
{
	LTStrCpy(m_FileName, pName, MAX_PATH);
}



bool LTVertexShaderImp::GetConstant(unsigned RegisterNum, float *pf0, float *pf1, float *pf2, float *pf3)
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



bool LTVertexShaderImp::SetConstant(unsigned RegisterNum, float f0, float f1, float f2, float f3)
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



bool LTVertexShaderImp::SetConstant(unsigned RegisterNum, const LTMatrix &Matrix)
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
// LTVertexShaderMgr
//-------------------------------------------------------------------------------------------------


LTVertexShaderMgr::LTVertexShaderMgr()
{
	m_bHardwareSupport = g_Device.GetDeviceCaps()->VertexShaderVersion >= 10;
}



LTVertexShaderMgr::~LTVertexShaderMgr()
{
	Term();
}



void LTVertexShaderMgr::Term()
{
	// Delete the vertex shaders.
	m_VertexShaders.Term();
}



LTVertexShaderMgr& LTVertexShaderMgr::GetSingleton()
{
	static LTVertexShaderMgr s_Singleton;
	return s_Singleton;
}



bool LTVertexShaderMgr::AddVertexShader(ILTStream *pStream, const char *ShaderName, int ShaderID,
										const uint32 *pDeclaration, uint32 DeclarationSize)
{
	// Check to see if this vertex shader has already been added.
	if (m_VertexShaders.Get(ShaderID) != LTNULL)
	{
		return false;
	}

	// Create and add the vertex shader to the list.
	LTVertexShaderImp *pShader = m_VertexShaders.Add(ShaderName, ShaderID);
	if (pShader == LTNULL)
	{
		return false;
	}

	// Initialize the shader.
	return pShader->Init(pStream, pDeclaration, DeclarationSize, m_bHardwareSupport);
}



void LTVertexShaderMgr::RemoveVertexShader(int ShaderID)
{
	LTVertexShaderImp *pShader = m_VertexShaders.Get(ShaderID);
	if (pShader != LTNULL)
	{
		// Now remove the vertex shader.
		m_VertexShaders.Remove(ShaderID);
	}
}



void LTVertexShaderMgr::RemoveAllVertexShaders()
{
	m_VertexShaders.Term();
}



LTVertexShader*	LTVertexShaderMgr::GetVertexShader(int ShaderID)
{
	LTVertexShaderImp *pShader = m_VertexShaders.Get(ShaderID);
	return pShader;
}



void LTVertexShaderMgr::FreeDeviceObjects()
{
	LTVertexShaderImp *pShader = (LTVertexShaderImp*)(m_VertexShaders.GetFront());
	while (pShader != NULL)
	{
		pShader->FreeDeviceObject();
		pShader = (LTVertexShaderImp*)(pShader->GetNext());
	}
}



void LTVertexShaderMgr::RecreateVertexShaders()
{
	LTVertexShaderImp *pShader = (LTVertexShaderImp*)(m_VertexShaders.GetFront());
	while (pShader != NULL)
	{
		pShader->Recreate(m_bHardwareSupport);

		pShader = (LTVertexShaderImp*)(pShader->GetNext());
	}
}



bool LTVertexShaderMgr::SetVertexShaderConstants(LTVertexShader *pShader)
{
	LTVertexShaderImp *pShaderImp = (LTVertexShaderImp*)pShader;

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

	// Set the constants.
	HRESULT hr = PD3DDEVICE->SetVertexShaderConstant(0, pConstants, LTVertexShader::MAX_CONSTANT_REGISTERS);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}



bool LTVertexShaderMgr::InstallVertexShader(LTVertexShader *pShader) const
{
	LTVertexShaderImp *pShaderImp = (LTVertexShaderImp*)pShader;
	if (pShaderImp == NULL ||
	    !pShaderImp->IsValidShader())
	{
		return false;
	}

	// Setup the vertex shader.
	HRESULT hr = PD3DDEVICE->SetVertexShader(pShaderImp->GetHandle());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}



void LTVertexShaderMgr::UninstallVertexShader() const
{
	// For DirectX, you can only trump a vertex shader with another vertex shader,
	// so leave the current vertex shader in the queue.
}
