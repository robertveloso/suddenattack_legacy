// ------------------------------------------------------------------ //
//
//	FILE	  : ClientMgr.h
//
//	PURPOSE	  : This is the Mgr for the client side DirectEngine
//              libraries.
//
//	CREATED	  : November 7 1996
//
//	COPYRIGHT : LithTech Inc., 1996-2000
//
// ------------------------------------------------------------------ //

#ifndef __CLIENTMGR_H__
#define __CLIENTMGR_H__

// ESD Includes
#ifdef LITHTECH_ESD
#include "iltesd.h"
class CLTRealAudioMgr;
class CLTRealVideoMgr;
#endif // LITHTECH_ESD

//----------------------------------------------------------------------------
//Below here are headers that probably wont be needed after certain things 
//are removed from the client mgr.
//----------------------------------------------------------------------------
    #ifndef __OBJECTMGR_H__
    #include "objectmgr.h"
    #endif

    #ifndef __MOTION_H__
    #include "motion.h"
    #endif

    #ifndef __SOUNDMGR_H__
    #include "soundmgr.h"
    #endif

    #ifndef __MUSICMGR_H__
    #include "musicmgr.h"
    #endif

    #ifndef __CONCOMMAND_H__
    #include "concommand.h"
    #endif

    #ifndef __PACKETDEFS_H__
    #include "packetdefs.h"
    #endif

    #ifndef __NETMGR_H__
    #include "netmgr.h"
    #endif

//----------------------------------------------------------------------------
//Above here are headers that probably wont be needed after certain things 
//are removed from the client mgr.
//----------------------------------------------------------------------------

#include "version_info.h"

class CNetMgr;
class VideoMgr;
class CClientShell;
class CMoveAbstract;
class ILTClient;
class InternalObjectSetup;
class CSoundInstance;
class InputMgr;
class CLTBenchmarkMgr;
class ILTMultiProtocolMgr;
class CLTMultiProtocolMgrClient;
class CSoundMgr;
struct SMusicMgr;
class LThreadMessage;
class FileRef;
class WorldTree;
class IClientShell;
class ILTCursor;
class ILTDirectMusicMgr;
class CBindModuleType;
class CShellModuleType;


CSoundMgr *GetSoundMgr( );
SMusicMgr *GetMusicMgr( );

#define	OBJID_CLIENTCREATED		0xFFFF


#define MAX_DLLNAME_LEN			255
#define MAX_CLIENTNAME_LEN		200
#define MAX_RESTREES			20


#define MAX_CLIENTERRORSTRING_LEN	300


// These control how often the client sends the input state to the server.
#define MAX_CHECKED_COMMANDS_ON		64
#define MAX_CHECKED_COMMANDS_WAIT	(1.0f / 5.0f)


// All the resource types used in the FileMgr.
#define PCX_TYPECODE		1


// Leeches.
extern LeechDef g_ModelInheritLeechDef;





class CClientMgr
{
	// Main stuff.
	public:

					CClientMgr();
					~CClientMgr();

		// Initialize the client.. the client will hang onto pResources, so
		// allocate it if necessary..
		LTRESULT					Init(const char *resTrees[MAX_RESTREES], uint32 nResTrees, const char *pConfigFileName);

		// Loads the clientshell.dll stuff.  If pDirectory is NULL, it looks
		// for a console variable and in the registry.
		void					TermClientShellDE();

		// Called by the client shell telling when you enter a world.
		void					OnEnterWorld(CClientShell *pShell);
		void					OnExitWorld(CClientShell *pShell);

		// Tries to start a new shell with the given parameters.
		// If it succeedes, it shuts down the currently running shell.
		LTRESULT				StartShell(StartGameRequest *pRequest);
		void					EndShell();

		// Reads messages from the threads.
		void					ProcessThreadMessage(LThreadMessage &msg);
		void					CheckThreads();

		// DE_OK if everything's ok, otherwise, shutdown.
		LTRESULT				Update();

		int						GetErrorCode();
		

	///////////// Screen functions ///////////////////////
	public:

		LTBOOL					BindClientShellWorlds();
		void					UnbindClientShellWorlds();

		void					BindSharedTextures();
		void					UnbindSharedTextures(bool bUnLoad_EngineData);

		void					InitConsole();
		void					TermConsole();
		

	///////////// Net functions ///////////////////////

		LTRESULT				ClearInput();
		void					ProcessAllInput(LTBOOL bForceClear);
		void					SendUpdateUnGuaranteed(CNetMgr *pNetMgr, CBaseConn *pConnID, int32 *pCommandsOn, int32 nCommandsOn);


	///////////// Utility functions ///////////////////////
	public:

		LTRESULT				PlaySound(PlaySoundInfo *pPlaySoundInfo, FileRef *pFile, float fStartTime );

	public: 

		void					KillSound( HLTSOUND hSound );
		void					UpdateAllSounds( float fFrameTime );
		CSoundInstance*			FindSoundInstance( HLTSOUND hSound, LTBOOL bClientSound );

		void					UpdateFrameRate();

		void					ForwardMessagesToScript();
		void					ForwardCommandChanges(int32 *pChanges, int32 nChanges);

		LTRESULT				AppInitMusic( char *pszMusicDLL );
		void					AppTermMusic( );

		// Sets up a packet so the app can read from it.
		CPacket*				GetPacket();
		void					SetupPacket(CPacket *pPacket);

		void					UpdateObjects();

		// Increment the current texture frame code.
		// It's guaranteed that the number it returns will not be set on any SharedTextures.
		uint16					IncCurTextureFrameCode();
		
		// Free unused model files
		LTRESULT				FreeUnusedModels();

		// Returns the world tree if it's valid.
		//inline WorldTree*		GetClientWorldTree()		{return &m_ClientWorld.m_WorldTree;}
		//inline MainWorld*		GetClientWorld()			{return &m_ClientWorld;}

#ifdef __PS2
        void                    SlurpConsoleFile();
#endif

	// Accessors.
	public:

		CSoundMgr*				GetSoundMgr()		{ return &m_SoundMgr; }
		SMusicMgr*				GetMusicMgr()		{ return &m_MusicMgr; }
		IClientShell*			GetClientShell()	{ return m_pClientShell; }

        //not in this class anymore.  see IClientDebugGraphMgr.
		//CDebugGraphMgr*			GetGraphMgr()		{ return &m_DebugGraphMgr; }

		ILTCursor*				GetCursorMgr()		{ return m_pCursorMgr; }


    //-------------------------------------------------------------------------
    //Below here is stuff will eventually be split out of this class and put 
    //in their own standalone modules.
    //-------------------------------------------------------------------------

    private:
		// The main world structure.  Stores all the current world data.
		//MainWorld				m_ClientWorld;

    public:

		// The object manager.  This holds the lists of all the objects (which aren't
		// necessarily always in the world tree).
		ObjectMgr				m_ObjectMgr;

		MotionInfo				m_MotionInfo;

		CSoundMgr				m_SoundMgr;

		SMusicMgr				m_MusicMgr;

		CNetMgr					m_NetMgr;

		// Debug graphs.  moved to IClientDebugGraphMgr
        //CDebugGraphMgr			m_DebugGraphMgr;

		// The server mirror console state.
		ConsoleState			m_ServerConsoleMirror;
		
		// Commands for the current and previous frames.
		uint8					m_Commands[2][MAX_CLIENT_COMMANDS];
		uint32					m_iCurInputSlot; // Alternates between the two slots.

        //moved to it's own module. IClientLoaderThread.
		//CLoaderThread			m_LoaderThread;

		// The main pixel format manager.  IClientFormatMgr
        //moved to it's own module.
        //FormatMgr				m_FormatMgr;

    //-------------------------------------------------------------------------
    //Above here is stuff will eventually be split out of this class and put 
    //in their own standalone modules.
    //-------------------------------------------------------------------------

    public:


		LMessageHelper			*m_pPacketHelper;
		ILTClient				*m_pClientDE;
		CollisionInfo *			m_pCollisionInfo; // Info on the last collision.
		CMoveAbstract			*m_MoveAbstract;

		
		// The ClientShellDE we're using.
		CBindModuleType         *m_hClientResourceModule;
		CBindModuleType         *m_hLocalizedClientResourceModule;
		#ifdef __PS2
		void*					m_hShellModule;
		#else
		CShellModuleType        *m_hShellModule;
		#endif
		IClientShell			*m_pClientShell;

		char					m_ErrorString[MAX_CLIENTERRORSTRING_LEN+1];

	// Screen stuff.
	public:

		void					ShowDrawSurface(uint flags = NULL);

		// Global lighting (values 0-1).
		LTVector				m_GlobalLightDir;
		LTVector				m_GlobalLightColor;
		LTVector				m_GlobalLightScale;


	// Sound stuff...
	public:

		char					m_MusicDLLName[MAX_DLLNAME_LEN+1];

	// Data collections.
	public:

		LTLink					m_Models;
		
		LTList					m_Sprites;
		LTList					m_SharedTextures;

		StructBank				m_FileIDInfoBank;
		ObjectBank<SharedTexture>	m_SharedTextureBank;


		// Sky stuff. 
		uint16					m_SkyObjects[MAX_SKYOBJECTS];
		uint16					m_nSkyObjects;
		SkyDef					m_SkyDef;


	// Multi-protocol manager functions...
	public:
		// Get a reference to our MPM instance.
		static ILTMultiProtocolMgr * GetMultiProtocolMgr() { return (ILTMultiProtocolMgr *)m_pMPM; }

	private:
		// this must be static because of the way that the LithTech interface
		// mechanism works.  CClientMgr must always be a singleton in the engine.
		static CLTMultiProtocolMgrClient * m_pMPM; // to our own (client-side) instance

	// Other.
	public:

		// Executable version info.
		LTVersionInfo			m_VersionInfo;

		// Registered surface effects.
		SurfaceEffectReg		*m_pSurfaceEffects;

		// Maps object IDs to object pointers.
		LTRecord				*m_ObjectMap;
		uint32					m_ObjectMapSize;

		// Timing.
		RateTracker				m_FramerateTracker;

		// These tell how long the current frame took.
		LTFLOAT					m_LastTime, m_FrameTime, m_CurTime;

		// cm_Ping needs to keep track of the last time it got called
		LTFLOAT					m_fLastPingTime;


		SpriteSurf				*m_SpriteSurfaces;
			

		// The last command state we sent to the server.
		uint8					m_LastInputState[MAX_CHECKED_COMMANDS_ON];
		int						m_nLastCommandsOn;
		float					m_InputSendCounter; 
		uint8					m_LastUpdateRate;

		float					m_AxisOffsets[NUM_AXIS_OFFSETS];
		InputMgr				*m_InputMgr; // Gotten right at startup.

		// The current client shell.  This is used for error handling only!  This is set
		// before calling any client shell functions that could throw exceptions.  If the 
		// shell throws an exception that says it wants the shell shutdown, it'll know which one to shutdown.
		CClientShell			*m_pCurShell;
		
		int						m_LastErrorCode;
		uint32					m_ExceptionFlags;	// Flags from the last exception thrown.

		LTBOOL					m_bCanSaveConfigFile; // Set to LTTRUE when it's ok to save the config file.

		// If this is FALSE, the client will tell the server to ignore
		// its input in the CMSG_UPDATE packet.
		LTBOOL					m_bInputState;

		// If this is LTTRUE, we're tracking potentially all device input 
		// and ReadInput will not be called.
		LTBOOL					m_bTrackingInputDevices;

		// Model hook stuff.
		ModelHookFn				m_ModelHookFn;
		void					*m_ModelHookUser;

		// Used so we don't render multiple times at once.
		LTBOOL					m_bRendering;
		
		// Don't do CF_NOTIFYREMOVEs when shutting down.
		LTBOOL					m_bNotifyRemoves;		


		// File manager..
		ClientFileMgr           *m_hFileMgr;

		const char				*m_ResTrees[MAX_RESTREES];
		uint32					m_nResTrees;

		// Placeholder model.
		Model					*m_pModelPlaceholder;

		VideoMgr				*m_pVideoMgr;

		ILTCursor				*m_pCursorMgr;

		uint16					m_CurTextureFrameCode;

		// DirectMusic manager.
		ILTDirectMusicMgr		*m_pDirectMusicMgr;

		// Benchmark manager.
		CLTBenchmarkMgr			*m_pBenchmarkMgr;

		// ESD Member Variables
		#ifdef LITHTECH_ESD
		CLTRealAudioMgr			*m_pRealAudioMgr;
		ILTRealConsoleMgr		*m_pRealConsoleMgr;
		CLTRealVideoMgr			*m_pRealVideoMgr;
		#endif // LITHTECH_ESD



    // ------------------------------------------------------------------
    //
    //
    //  Functions that used to be called cm_*, now they are member functions.
    //
    //
    // ------------------------------------------------------------------

    public:

        // ------------------------------------------------------------------ //
        // clientmgr.cpp
        // ------------------------------------------------------------------ //

        // Called when a connection to a server is established 
        // (m_pCurShell) and when it is broken.
        void OnEnterServer();
        void OnExitServer(CClientShell *pShell);

        void RunCommandLine(uint32 flags);
        void RunAutoConfig(const char *pFilename, uint32 flags);
        void SaveAutoConfig(const char *pFilename);

        // Start the renderer from the global options.
        LTRESULT StartRenderFromGlobals();

        // Render through a camera.
        LTBOOL Render(CameraInstance *pCamera, int drawMode, LTObject **pObjects, int nObjects);

        void RebindTextures();

        // Normal add.. fully sets up the object.
        // Pass OBJID_CLIENTCREATED for id if the object doesn't come from the server.
        LTRESULT AddObjectToClientWorld(uint16 id, InternalObjectSetup *pSetup, LTObject **ppObject, LTBOOL bMove, LTBOOL bRotate);

        LTRESULT RemoveObjectFromClientWorld(LTObject *pObject);

        void RemoveObjectsInList(LTList *pList, LTBOOL bServerOnly);

        // Update animating sprite surfaces.
        void UpdateSpriteSurfaces();

        // Clears out the m_SpriteSurfaces list.
        void TermSpriteSurfaces();

        // Called while loading stuff so streaming sounds can be updated.
        void Ping();


        void UpdateModels();
        void UpdateLineSystems();
        void UpdatePolyGrids();
        void UpdateAnimations();
        void UpdateParticleSystems();


        // ------------------------------------------------------------------ //
        // c_util.cpp
        // ------------------------------------------------------------------ //

        // Add a surface effect.
        LTRESULT AddSurfaceEffect(SurfaceEffectDesc *pDesc);

        // Sets up the error string for the error.
        LTRESULT SetupError(LTRESULT theError, ...);

        // Processes an error (disconnects, prints error message, etc).  This will 
        // usually return LT_OK, but may return LT_ERROR if the error was fatal 
        // and it needs to shutdown.
        LTRESULT ProcessError(LTRESULT theError);

        // Sets up a CSharedTexture for the texture (if one doesn't exist already).
        SharedTexture* AddSharedTexture(FileRef *pRef);
        LTRESULT AddSharedTexture2(FileRef *pRef, SharedTexture* &pTexture);
        LTRESULT AddSharedTexture3(FileIdentifier *pIdent, SharedTexture* &pTexture);

        void FreeSharedTexture(SharedTexture *pTexture);

        // Frees all SharedTextures.
        void FreeSharedTextures();

        void UntagAllTextures();
        void TagUsedTextures();
        void BindUnboundTextures();
        void UnbindUnusedSharedTextures();

        // This function gets rid of as many textures as it can.
        void TagAndFreeTextures();

        // Free untagged textures.
        void FreeUnusedSharedTextures();
        // Free the textures associated with a model if nobody else is using them
        void FreeUnusedModelTextures(LTObject *pObject);

        void AddToObjectMap(uint16 id);
        void ClearObjectMapEntry(uint16 id);
        LTObject* FindObject(uint16 id);
        LTRecord* FindRecord(uint16 id);

        void RelocateObject(LTObject *pObject);
        void ScaleObject(LTObject *pObject, LTVector *pNewScale);
        void MoveObject(LTObject *pObject, LTVector *pNewPos, LTBOOL bForce);
        void RotateObject(LTObject *pObject, LTRotation *pNewRot);
        void MoveAndRotateObject(LTObject *pObject, LTVector *pNewPos, LTRotation *pNewRot);

        void UpdateModelDims(ModelInstance *pInstance);



        //----------------------------------------------------
        //  setupobject.cpp
        //----------------------------------------------------

	    // Gets a pointer to the given model, or loads it if it's not loaded yet.
	    // If bLoad is FALSE, it will return it if it exists, otherwise it will
	    // return LT_INPROGRESS.
	    // If bBlock is LTTRUE, it will wait for the load to complete.  If LTFALSE, it will
	    // start loading in the background thread and return with LT_INPROGRESS.
	    LTRESULT LoadModel(FileIdentifier *pIdent, Model* &pModel, LTBOOL bBlock);
	    LTRESULT LoadModel2(FileRef *pFileRef, Model* &pModel, 
		    FileIdentifier* &pIdent, LTBOOL bLoad, LTBOOL bBlock);
	    
	    // Used by the thread and model loading routines.
	    LTRESULT LoadModelData(char *pFilename, FileIdentifier *pIdent, Model* &pModel);
	    LTRESULT InstallModel(Model *pModel, FileIdentifier *pIdent, 
		    LTBOOL bUpdateDependencies=LTTRUE);

    	void UnbindModel(Model *pModel, FileIdentifier *pIdent);

        LTBOOL IsModelDataInherited(Model *pModel);

};	

extern CClientMgr *g_pClientMgr;

// ------------------------------------------------------------------ //
// C routines.
// ------------------------------------------------------------------ //


// ------------------------------------------------------------------ //
// clientmgr.cpp
// ------------------------------------------------------------------ //

void cm_Init();

#endif  // __CLIENTMGR_H__






