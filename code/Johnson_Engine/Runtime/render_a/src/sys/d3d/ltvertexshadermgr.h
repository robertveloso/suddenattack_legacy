//------------------------------------------------------------------
//
//  FILE      : LTVertexShaderMgr.h
//
//  PURPOSE   :	Vertex shader manager singleton
//
//  COPYRIGHT : LithTech Inc., 1996-2002
//
//------------------------------------------------------------------

#ifndef __LTVERTEXSHADERMGR_H__
#define __LTVERTEXSHADERMGR_H__


#include "ltbasedefs.h"
#include "ltidtoobjecttable.h"



// forwards:
class CDIModelDrawable;



// LTVertexShaderImp -- implementation of vertex shader class.
//                      This class contains the vertex shader byte code loaded from file.
class LTVertexShaderImp : public LTVertexShader
{
public:

	LTVertexShaderImp()
		: m_pByteCode(NULL),
		  m_Handle(0),
		  m_pDeclaration(NULL)
	{
	}

	~LTVertexShaderImp()
	{
		Term();
	}

	bool				Init(ILTStream *pStream, const uint32 *pDeclaration, uint32 DeclarationSize, bool bHardwareSupport);
	bool				Recreate(bool bHardwareSupport);
	void				Term();
	void				FreeDeviceObject();

	// is the shader valid
	virtual bool		IsValidShader() const					{ return m_Handle != 0; }

	// id
	void				SetID(int ShaderID)						{ m_ShaderID = ShaderID; }

	// name
	void				SetName(const char *pName);

	// next pointer
	void				SetNext(LTVertexShaderImp *pNext)		{ m_pNext = pNext; }

	// byte code
	uint8*				GetByteCode()							{ return m_pByteCode; }

	// get the values in a constant register
	virtual bool		GetConstant(unsigned RegisterNum, float *pf0, float *pf1, float *pf2, float *pf3);

	// set the values in a constant register
	virtual bool		SetConstant(unsigned RegisterNum, float f0, float f1, float f2, float f3);

	// copies the values in the given matrix to the four constant registers starting at RegisterNum
	virtual bool		SetConstant(unsigned RegisterNum, const LTMatrix &Matrix);

	// constants
	virtual float*		GetConstants()							{ return m_Constants; }

	// d3d vertex shader handle
	DWORD				GetHandle()								{ return m_Handle; }

private:

	uint8*				m_pByteCode;			// byte code loaded in from the file
	DWORD				m_Handle;				// D3D handle to this shader
	DWORD*				m_pDeclaration;			// vertex shader input declaration
	float				m_Constants[LTVertexShader::MAX_CONSTANT_REGISTERS*4];			// user-defined constants
};



// maps vertex shader ID to vertex shader pointer
typedef std::map<int, LTVertexShaderImp*> LTVertexShaderMap;


// lookup table for vertex shaders
typedef LTIDToObjectTable<LTVertexShaderImp, LTVertexShaderMap>	LTVertexShaders;



// LTVertexShaderMgr -- the actual manager for the vertex shaders.
class LTVertexShaderMgr
{
public:

	~LTVertexShaderMgr();

	void						Term();

	// singleton access
	static LTVertexShaderMgr&	GetSingleton();

	// add a vertex shader
	bool						AddVertexShader(ILTStream *pStream, const char *ShaderName, int ShaderID,
												const uint32 *pDeclaration, uint32 DeclarationSize);

	// remove a vertex shader
	void						RemoveVertexShader(int ShaderID);

	// remove all vertex shaders
	void						RemoveAllVertexShaders();

	// vertex shader access
	LTVertexShader*				GetVertexShader(int ShaderID);

	// frees all the device shader handles
	void						FreeDeviceObjects();

	// recreates all the shaders. This is necessary when the device changes.
	void						RecreateVertexShaders();

	// sets the shader constants
	bool						SetVertexShaderConstants(LTVertexShader *pShader);

	// installs the specified shader into the device
	bool						InstallVertexShader(LTVertexShader *pShader) const;

	// uninstalls any currently installed shaders
	void						UninstallVertexShader() const;

private:

	LTVertexShaderMgr();

private:

	LTVertexShaders				m_VertexShaders;			// list of vertex shaders loaded from file

	bool						m_bHardwareSupport;
};



#endif // __LTVERTEXSHADERMGR_H__
