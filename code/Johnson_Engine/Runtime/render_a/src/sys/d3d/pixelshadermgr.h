#ifndef __PIXELSHADERMGR_H__
#define __PIXELSHADERMGR_H__

//a handle to a pixel shader
class CPixelShaderHandle
{
public:

	enum { INVALID_PIXEL_SHADER	= 0xFFFFFFFF };

	CPixelShaderHandle() :
		m_nID(INVALID_PIXEL_SHADER)
	{
	}

	bool IsValid() const		{ return m_nID != INVALID_PIXEL_SHADER; }
	void Invalidate()			{ m_nID = INVALID_PIXEL_SHADER; }
	uint32 m_nID;
};

//the actual manager for the pixel shaders.
class CPixelShaderMgr
{
public:

	~CPixelShaderMgr();

	//singleton access
	static CPixelShaderMgr&	GetSingleton();

	//loads a pixel shader from disk and assigns it a unique id which it can be referred to by. This
	//will return an invalid handle if the load failed or the device doesn't support it. Specifying true
	//for the auto cleanup indicates that it should not worry about reference counting and that there
	//is basically a static shader being used that is safe to be removed at the end of the application
	CPixelShaderHandle		LoadPixelShader(const char* pszFilename, bool bAutoCleanup = false);

	//frees all the device pixel shaders
	void					FreeDeviceObjects();

	//recreates all the pixel shaders. This is necessary when the device changes
	void					RecreatePixelShaders();

	//sets the pixel shader constants. This takes the number of pixel shader constants you want to supply,
	//and a list of those constants (each constant is a set of 4 floats)
	bool					SetPixelShaderConstants(uint32 nNumConstants, const float* pConstants, uint32 nStartReg = 0) const;

	//installs the specified pixel shader into the device, returning a pointer to the pixel shader
	//that was already installed. 
	bool					InstallPixelShader(const CPixelShaderHandle& Shader) const;

	//uninstalls any currently installed pixel shaders
	void					UninstallPixelShaders() const;

	//releases a pixel shader
	bool					ReleasePixelShader(const CPixelShaderHandle& Shader);
	
private:

	//frees the specified shader
	void					FreeShader(uint32 nIndex);

	CPixelShaderMgr()			{}

	//a pixel shader object
	struct SPixelShader
	{
		SPixelShader(const char* pszFilename, uint8* pData, DWORD nShaderHandle, bool bAutoCleanup);
		~SPixelShader();

		//the D3D handle to this shader
		DWORD		m_nShaderHandle;

		//the actual byte code loaded in from the file
		uint8*		m_pData;

		//the name of the file this came from
		char*		m_pFilename;

		//the reference count
		uint32		m_nRefCount;

		//whether or not the application is going to clean this up. This is usually set for
		//global pixel shaders so that they can just be allocated and will be cleaned up
		//at the end
		bool		m_bAutoCleanup;

		//whether or not the pixel shader is valid
		bool		m_bValidShader;
	};

	//the list of pixel shaders
	typedef vector<SPixelShader*>	TShaderList;
	TShaderList						m_cShaderList;
};

#endif
