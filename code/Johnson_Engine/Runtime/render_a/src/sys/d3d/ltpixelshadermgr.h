//------------------------------------------------------------------
//
//  FILE      : LTPixelShaderMgr.h
//
//  PURPOSE   :	Pixel shader manager singleton
//
//  COPYRIGHT : LithTech Inc., 1996-2003
//
//------------------------------------------------------------------

#ifndef __LTPIXELSHADERMGR_H__
#define __LTPIXELSHADERMGR_H__


#include "ltbasedefs.h"
#include "ltidtoobjecttable.h"



// LTPixelShaderImp -- implementation of pixel shader class.
//                     This class contains the pixel shader byte code (directx) or pixel shader struct (xbox).
class LTPixelShaderImp : public LTPixelShader
{
public:

	LTPixelShaderImp()
		: m_pByteCode(NULL),
	  	  m_Handle(0)
	{
	}

	~LTPixelShaderImp()
	{
		Term();
	}

	// initialize from file
	bool						Init(ILTStream *pStream);
	void						Term();
	bool						Recreate();
	void						FreeDeviceObject();

	// is the shader valid
	virtual bool				IsValidShader() const					{ return m_Handle != 0; }

	// id
	void						SetID(int ShaderID)						{ m_ShaderID = ShaderID; }

	// name
	void						SetName(const char *pName);

	// next pointer
	void						SetNext(LTPixelShaderImp *pNext)		{ m_pNext = pNext; }

	// byte code
	uint8*						GetByteCode()							{ return m_pByteCode; }

	// d3d pixel shader handle
	DWORD						GetHandle()								{ return m_Handle; }

	// get the values in a constant register
	virtual bool				GetConstant(unsigned RegisterNum, float *pf0, float *pf1, float *pf2, float *pf3);

	// set the values in a constant register
	virtual bool				SetConstant(unsigned RegisterNum, float f0, float f1, float f2, float f3);

	// copies the values in the given matrix to the four constant registers starting at RegisterNum
	virtual bool				SetConstant(unsigned RegisterNum, const LTMatrix &Matrix);

	// constants
	virtual float*				GetConstants()							{ return m_Constants; }

private:

	uint8*				m_pByteCode;			// byte code loaded in from the file
	DWORD				m_Handle;				// D3D handle to this shader
	float				m_Constants[LTPixelShader::MAX_CONSTANT_REGISTERS*4];			// user-defined constants
};



// maps pixel shader ID to pixel shader pointer
typedef std::map<int, LTPixelShaderImp*> LTPixelShaderMap;


// lookup table for pixel shaders
typedef LTIDToObjectTable<LTPixelShaderImp, LTPixelShaderMap>	LTPixelShaders;



// LTPixelShaderMgr -- the actual manager for the pixel shaders.
class LTPixelShaderMgr
{
public:

	~LTPixelShaderMgr();

	void						Term();

	// singleton access
	static LTPixelShaderMgr&	GetSingleton();

	// add a pixel shader from file
	bool						AddPixelShader(ILTStream *pStream, const char *ShaderName, int ShaderID);

	// remove a pixel shader
	void						RemovePixelShader(int ShaderID);

	// remove all pixel shaders
	void						RemoveAllPixelShaders();

	// pixel shader access
	LTPixelShader*				GetPixelShader(int ShaderID);

	// frees all the device shader handles
	void						FreeDeviceObjects();

	// recreates all the shaders. This is necessary when the device changes.
	void						RecreatePixelShaders();

	// sets the shader constants
	bool						SetPixelShaderConstants(LTPixelShader *pShader);

	// installs the specified shader into the device
	bool						InstallPixelShader(LTPixelShader *pShader) const;

	// uninstalls any currently installed shaders
	void						UninstallPixelShader() const;

private:

	LTPixelShaderMgr();

private:

	LTPixelShaders				m_PixelShaders;			// list of pixel shaders loaded from file

	bool						m_bHardwareSupport;
};



#endif // __LTPIXELSHADERMGR_H__
