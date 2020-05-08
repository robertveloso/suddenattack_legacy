// ------------------------------------------------------------------ //
//
//  FILE      : ClientShell.cpp
//
//  PURPOSE   : 
//
//  CREATED   : February 25 1997
//                           
//  COPYRIGHT : Microsoft 1997 All Rights Reserved
//
// ------------------------------------------------------------------ //

// Includes....
#include "bdefs.h"

#include "clientshell.h"
#include "de_world.h"
#include "setupobject.h"
#include "clientmgr.h"
#include "sprite.h"
#include "consolecommands.h"
#include "servermgr.h"
#include "predict.h"
#include "moveobject.h"
#include "server_interface.h"
#include "dhashtable.h"
#include "console.h"

#include "build_options.h"
#include "render.h"

#include "ltobjectcreate.h"

#include "client_ticks.h"


//------------------------------------------------------------------
//------------------------------------------------------------------
// Holders and their headers.
//------------------------------------------------------------------
//------------------------------------------------------------------

//IWorldClientBSP holder
#include "world_client_bsp.h"
static IWorldClientBSP *world_bsp_client;
define_holder(IWorldClientBSP, world_bsp_client);

//IWorldSharedBSP holder
#include "world_shared_bsp.h"
static IWorldSharedBSP *world_bsp_shared;
define_holder(IWorldSharedBSP, world_bsp_shared);

//IClientFileMgr
#include "client_filemgr.h"
static IClientFileMgr *client_file_mgr;
define_holder(IClientFileMgr, client_file_mgr);

//IClientShell game client shell object.
#include "iclientshell.h"
static IClientShell *i_client_shell;
define_holder(IClientShell, i_client_shell);


//IWorldBlindObjectData holder
#include "world_blind_object_data.h"
static IWorldBlindObjectData *g_iWorldBlindObjectData = LTNULL;
define_holder(IWorldBlindObjectData, g_iWorldBlindObjectData);

//[MURSUM] SpecialEffect Packet을 ILTMessage_Read로 캐스팅.
#include "ltmessage_client.h"
class CCast_Client : public CLTMsgRef_Read
{
public:
	CCast_Client(const CPacket_Read &cPacket) :
		CLTMsgRef_Read(CLTMessage_Read_Client::Allocate_Client(CPacket_Read(cPacket, cPacket.Tell())))
	{}
	CCast_Client(const CPacket_Read &cPacket, uint32 nLength) :
		CLTMsgRef_Read(CLTMessage_Read_Client::Allocate_Client(CPacket_Read(cPacket, cPacket.Tell(), nLength)))
	{}
};


extern int32	g_CV_UpdateRate;
extern LTBOOL	g_bUpdateServer;
extern int32	g_bForceRemote;
extern LTFLOAT	g_CV_LockSendUUF, g_CV_LockFPS;

#ifndef _FINAL
extern int32 g_CV_ShowFrameRate;
#endif

CClientShell *g_pClientShell;



// ------------------------------------------------------------------ //
// Static functions..
// ------------------------------------------------------------------ //

static void AddAllObjectsToBSP(LTList *pList)
{
    LTLink *pCur, *pListHead;
    LTObject *pObj;
    WorldTree *pWorldTree;

    pWorldTree = world_bsp_client->ClientTree();

    pListHead = &pList->m_Head;
    for (pCur=pListHead->m_pNext; pCur != pListHead; pCur=pCur->m_pNext)
    {
        pObj = (LTObject*)pCur->m_pData;
    
        pWorldTree->InsertObject(pObj);
    }
}


static void RemoveAllObjectsFromWorldTree(LTList *pList)
{
    LTLink *pCur, *pListHead;
    LTObject *pObj;

    pListHead = &pList->m_Head;
    for (pCur=pListHead->m_pNext; pCur != pListHead; pCur=pCur->m_pNext)
    {
        pObj = (LTObject*)pCur->m_pData;
        pObj->RemoveFromWorldTree();
    }
}


static bool cs_ShouldUseTCPIP() {
    LTCommandVar *pVar;

    if (pVar = cc_FindConsoleVar(&g_ClientConsoleState, "tcpip"))
    {
        if (pVar->floatVal != 0.0f)
            return true;
    }

    return false;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
// Console stuff
//------------------------------------------------------------------
//------------------------------------------------------------------

CONCOLOR    g_ShellMsgColor = CONRGB(0, 255, 0);




////////////////////////////////////////////////////////
// ClientShell functions
////////////////////////////////////////////////////////

CClientShell::CClientShell()
{
    uint32 i;

    m_pLastWorld = NULL;
    m_bWorldOpened = false;
    m_GameTime = m_LastGameTime = m_GameFrameTime = 0.0f;

    m_HostID = INVALID_CONNID;

    m_pDriver = NULL;

    m_KillTag = 0;
    g_pServerMgr = NULL;
    m_ClientID = (uint16)-1;
    
    m_ClientObjectID = (uint16)-1;
    dl_TieOff(&m_MovingObjects);
    dl_TieOff(&m_RotatingObjects);
    m_pFrameClientObject = NULL;

    for (i=0; i < 16; i++)
    {
        m_ColorSignExtend[i] = (uint8)(((float)i * 255.0f) / 15.0f);
    }

	//[MURSUM]
	m_fLastUpdateServer	= 0.0f;
	m_fLastUpdateClient	= 0.0f;

	m_fFrameTimeServer	= 0.1f;
	m_fFrameTimeClient	= 0.1f;
}


CClientShell::~CClientShell()
{
    Term();
}


bool CClientShell::Init()
{
	dsi_ConsolePrint( "[ENGINE] CClientShell::Init" );
    // Setup net stuff.
    InitHandlers();
    g_pClientMgr->m_ClientNetMgr.SetNetHandler(this);
    g_pClientShell = this;

    // Initialize the fileid info list.  Some information sent to the client doesn't change based on fileid.
    // This is used to reduce the amount of info sent to the client, by just comparing the new info to what
    // was sent last.  As an example, sound radii is typically the same for one particular file, so the server
    // only needs to send this info once.  Other info can be added to the FileIDInfo structure as needed.
    m_hFileIDTable = hs_CreateHashTable(100, HASH_2BYTENUMBER);

    return true;
}


// Clear the fileid's we took from the server for all of our client loaded models.
// The client can load files before the server, then the server assigns an id to
// it.  If the server goes away before the client is done with it, we have to
// clear the fileid we got from the server.
static
void Client_ResetModelFileIds( const Model & model , void *user_dat )
{
	const_cast<Model&>( model ).m_FileID = ( uint32 )-1;
}


void CClientShell::Term()
{
	HHashIterator *hIterator;
    HHashElement *hElement;

	dsi_ConsolePrint( "[ENGINE] CClientShell::Term" );

    // Let the client manager do its thing..
    g_pClientMgr->OnExitServer(this);

	if( g_pClientMgr->m_ClientNetMgr.IsConnected() )
		SendGoodbye();

    g_pClientMgr->m_ClientNetMgr.Disconnect(m_HostID, DISCONNECTREASON_VOLUNTARY_CLIENTSIDE);
    m_HostID = INVALID_CONNID;

    g_pClientMgr->m_ClientNetMgr.SetNetHandler(NULL);


    // Uninit the server mugger if there is one.
	__try
	{
		if (g_pServerMgr) {
			delete g_pServerMgr;
			g_pServerMgr = NULL;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		g_pServerMgr = LTNULL;
		dsi_ConsolePrint("[ERROR] Delete ServerMgr");
	}
	
	dsi_SendPingToGameServer();

    // Uninit object stuff.
    RemoveAllObjects();

	// Clear out the sounds.
	GetClientILTSoundMgrImpl()->RemoveAllUnusedSoundInstances( );
	GetClientILTSoundMgrImpl()->SetReverbFilter(false);

	// Get rid of the world references because the (local) server will 
    // delete the world.
    NotifyWorldClosing();
    CloseWorlds();

	dsi_SendPingToGameServer();

	if (m_pDriver)
		g_pClientMgr->m_ClientNetMgr.RemoveDriver(m_pDriver);
    m_pDriver = NULL;
	
    m_ClientObjectID = (uint16)-1;

    m_pFrameClientObject = NULL;

    // Free the fileid info structures...
    hIterator = hs_GetFirstElement(m_hFileIDTable);
    while (hIterator)
    {
        hElement = hs_GetNextElement(hIterator);
        if (hElement)
            sb_Free(&g_pClientMgr->m_FileIDInfoBank, hs_GetElementUserData(hElement));
    }
    hs_DestroyHashTable(m_hFileIDTable);

	// Clear the fileid's we took from the server for all of our client loaded models.
	g_ModelMgr.ForEach( Client_ResetModelFileIds, NULL );
	
    g_pClientShell = NULL;

	//[MURSUM]
	m_fLastUpdateServer	= 0.0f;
	m_fLastUpdateClient	= 0.0f;

	m_fFrameTimeServer	= 0.1f;
	m_fFrameTimeClient	= 0.1f;
}

void CClientShell::TermNetDriver()
{
	dsi_ConsolePrint( "[ENGINE] CClientShell::TermNetDriver" );

	if( g_pClientMgr->m_ClientNetMgr.IsConnected() )
		SendGoodbye();

    g_pClientMgr->m_ClientNetMgr.Disconnect(m_HostID, DISCONNECTREASON_VOLUNTARY_CLIENTSIDE);
    m_HostID = INVALID_CONNID;

    g_pClientMgr->m_ClientNetMgr.SetNetHandler(NULL);

	for (int i=0; i < NUM_OBJECTTYPES; i++)
    {
		LTList *pList = &g_pClientMgr->m_ObjectMgr.m_ObjectLists[i];
		LTLink *pCur, *pNext, *pListHead;
		LTObject *pObj;

		pListHead = &pList->m_Head;
		pCur = pListHead->m_pNext;
		while( pCur != pListHead )
		{
			pNext = pCur->m_pNext;

			// Only remove objects that came from the server.
			pObj = (LTObject*)pCur->m_pData;

			if( pObj->m_ObjectID < OBJID_CLIENTCREATED_TYPE2 )
			{
				g_pClientMgr->RemoveObjectFromClientWorld(pObj);
			}			
			pCur = pNext;
		}
    }

	if (m_pDriver)
		g_pClientMgr->m_ClientNetMgr.RemoveDriver(m_pDriver);
    m_pDriver = NULL;

	m_ClientObjectID = (uint16)-1;

	m_fLastUpdateServer	= 0.0f;
	m_fLastUpdateClient	= 0.0f;

	m_fFrameTimeServer	= 0.1f;
	m_fFrameTimeClient	= 0.1f;

	m_bLocal = false;
}

LTRESULT CClientShell::StartupClientTCP( const char* pWorldName )
{
	if( !pWorldName || pWorldName[0] == 0 ) return LT_OK;
	if( g_pServerMgr ) g_pServerMgr->StartClientTCP( pWorldName );    

	return LT_OK;
}

LTRESULT CClientShell::StartupClient(StartGameRequest *pRequest, CBaseDriver *pDriver)
{
	LTRESULT dResult;

	dsi_ConsolePrint( "[ENGINE] CClientShell::StartupClient" );

    m_pDriver = pDriver;

	if( !pRequest || g_pServerMgr )
	{
		m_bLocal = false;
		return LT_OK;
	}

	if ((dResult = CreateServerMgr()) != LT_OK)
        return dResult;

	 // Give it the game info data.
    if (pRequest->m_pGameInfo && pRequest->m_GameInfoLen > 0)
    {
        g_pServerMgr->SetGameInfo(pRequest->m_pGameInfo, pRequest->m_GameInfoLen);
    }

	// Add the resources to it and listen locally by default.
    if (!g_pServerMgr->AddResources(g_pClientMgr->m_ResTrees, g_pClientMgr->m_nResTrees))
    {
		delete g_pServerMgr;
		g_pServerMgr = LTNULL;
		return LT_OK;
    }

	// Load the object.lto file.
	if( !g_pServerMgr->LoadBinaries( ))
	{
		delete g_pServerMgr;
		g_pServerMgr = LTNULL;
		return LT_OK;
	}
	
	m_bLocal = false;

    return LT_OK;
}

LRESULT CClientShell::MandateHost( StartGameRequest *pRequest, bool bHost, CBaseDriver *pServerDriver)
{
	char errorString[256];
	CBaseDriver *pServerLocalDriver;
		
	ASSERT(!m_pDriver);
	dsi_ConsolePrint( "[ENGINE] CClientShell::MandateHost" );

	if( !g_pServerMgr ) return StartupLocal( pRequest, bHost, pServerDriver );

	if (!g_pServerMgr->Listen("local", "LithTech Session"))
    {
		g_pServerMgr->GetErrorString(errorString, sizeof(errorString));
		RETURN_ERROR_PARAM(1, CClientShell::StartupLocal, LT_SERVERERROR, errorString);
        //GetClientMgr()->ThrowClientException(EXC_SHUTDOWN_SHELL|EXC_MODAL_MESSAGE, Cli_ServerError, errorString);
    }

	m_bLocal = true;

	if (pServerDriver)
    {
        g_pServerMgr->TransferNetDriver(pServerDriver);
    }

	// Setup a local driver.
    con_PrintString(g_ShellMsgColor, 1, "Connecting locally");
    m_pDriver = g_pClientMgr->m_ClientNetMgr.AddDriver("local");
    
    // Connect locally.
    pServerLocalDriver = g_pServerMgr->GetLocalDriver();
    m_pDriver->LocalConnect(pServerLocalDriver);
    
    // Sort of a hack.. new connection notifications don't come in  (and thus 
    // m_HostID doesn't get set) until you do an update.
    g_pClientMgr->m_ClientNetMgr.Update("Client: ", g_pClientMgr->m_CurTime);

	return LT_OK;
}


LTRESULT CClientShell::StartupLocal(StartGameRequest *pRequest,
    bool bHost, CBaseDriver *pServerDriver)
{
    bool bSelfConnect;
    LTRESULT dResult;
    CBaseDriver *pServerLocalDriver;
    char errorString[256];
    
	ASSERT(!m_pDriver);
	dsi_ConsolePrint( "[ENGINE] CClientShell::StartupLocal" );    

    // This can be false if you just want to serv a game, so it should be a parameter.
    bSelfConnect = true;

	
	// Setup a server mugger.
	if ((dResult = CreateServerMgr()) != LT_OK)
		return dResult;

	// Give it the game info data.
	if (pRequest->m_pGameInfo && pRequest->m_GameInfoLen > 0)
	{
		g_pServerMgr->SetGameInfo(pRequest->m_pGameInfo, pRequest->m_GameInfoLen);
	}


	// Add the resources to it and listen locally by default.
	if (!g_pServerMgr->AddResources(g_pClientMgr->m_ResTrees, g_pClientMgr->m_nResTrees))
	{
		g_pServerMgr->GetErrorString(errorString, 256);
		RETURN_ERROR_PARAM(1, CClientShell::StartupLocal, LT_SERVERERROR, errorString);
	}

	// Load the object.lto file.
	if( !g_pServerMgr->LoadBinaries( ))
	{
		g_pServerMgr->GetErrorString(errorString, 256);
		RETURN_ERROR_PARAM(1, CClientShell::StartupLocal, LT_SERVERERROR, errorString);
	}

    if (!g_pServerMgr->Listen("local", "LithTech Session"))
    {
		g_pServerMgr->GetErrorString(errorString, sizeof(errorString));
        RETURN_ERROR_PARAM(1, CClientShell::StartupLocal, LT_SERVERERROR, errorString);
        //GetClientMgr()->ThrowClientException(EXC_SHUTDOWN_SHELL|EXC_MODAL_MESSAGE, Cli_ServerError, errorString);
    }

    m_bLocal = true;

    // Give the server the CBaseDriver we have setup.
    if (pServerDriver)
    {
        g_pServerMgr->TransferNetDriver(pServerDriver);
    }

    if (bSelfConnect)
    {
        // Setup a local driver.
        con_PrintString(g_ShellMsgColor, 1, "Connecting locally");
        m_pDriver = g_pClientMgr->m_ClientNetMgr.AddDriver("local");
        
        // Connect locally.
        pServerLocalDriver = g_pServerMgr->GetLocalDriver();
        m_pDriver->LocalConnect(pServerLocalDriver);
        
        // Sort of a hack.. new connection notifications don't come in  (and thus 
        // m_HostID doesn't get set) until you do an update.
        g_pClientMgr->m_ClientNetMgr.Update("Client: ", g_pClientMgr->m_CurTime);
    }

	return LT_OK;
}


LTRESULT CClientShell::CreateServerMgr()
{
    IFBREAKRETURNVAL(g_pServerMgr != NULL, LT_ERROR);

	dsi_ConsolePrint( "[ENGINE] CClientShell::CreateServerMgr" );

    LT_MEM_TRACK_ALLOC(g_pServerMgr = new CServerMgr,LT_MEM_TYPE_MISC);
    if (!g_pServerMgr || !g_pServerMgr->Init())
    {
        delete g_pServerMgr;
        g_pServerMgr = NULL;

        RETURN_ERROR(1, CClientShell::CreateServerMgr, LT_CANTCREATESERVER);
    }

    g_pServerMgr->m_NetMgr.SetAppGuid(g_pClientMgr->m_ClientNetMgr.GetAppGuid());

    return LT_OK;
}


LTRESULT CClientShell::Update()
{
    long updateFlags;
    char errorString[256];
    LTCommandVar *pVar;
    LTObject *pClientObject;
    LTRESULT dResult;

	bool	bUpdateUnguaranteed	= false;
	
	m_GameTime += g_pClientMgr->m_FrameTime;
	
	if( m_GameTime-m_fLastUpdateClient < g_CV_LockFPS ) return LT_OK;
	

	m_fFrameTimeClient = m_GameTime-m_fLastUpdateClient;
	m_fLastUpdateClient	= m_GameTime;			
#ifndef _FINAL
	m_FramerateTrackerClient.Add(1.0f);
	m_FramerateTrackerClient.Update( m_fFrameTimeClient );
#endif
	
	if( m_bLocal && m_GameTime-m_fLastUpdateServer > g_CV_LockSendUUF )
	{
		m_fFrameTimeServer = m_GameTime-m_fLastUpdateServer;
		m_fLastUpdateServer	= m_GameTime;			
		bUpdateUnguaranteed	= true;
#ifndef _FINAL
		m_FramerateTrackerServer.Add(1.0f);
		m_FramerateTrackerServer.Update( m_fFrameTimeServer );
#endif
	}

#ifndef _FINAL
	if (g_CV_ShowFrameRate)
    {
        con_Printf(CONRGB(255,100,100), 0, "S: %.2f   C: %.2f",
					m_FramerateTrackerServer.GetRate(),
					m_FramerateTrackerClient.GetRate() );
    }
#endif

	{
        CountAdder cntAdd(&g_Ticks_NetUpdate);
        g_pClientMgr->m_ClientNetMgr.Update("Client: ", g_pClientMgr->m_CurTime);
    }

	if( m_bLocal )
    {
		CountAdder cntAdd(&g_Ticks_ServerUpdate);

        // Update our local server manager if it's there.
        // (If the update fails an error will be returned.)
        if (g_pServerMgr && g_bUpdateServer)
        {
            updateFlags = 0;
            if (!dsi_IsClientActive() && m_ShellMode != STARTGAME_HOST && m_ShellMode != STARTGAME_HOSTTCP)
            {
                // Console variable 'alwaysfocused' overrides this.
                pVar = cc_FindConsoleVar(&g_ClientConsoleState, "alwaysfocused");
                if (!pVar ||
                    (pVar->floatVal != 1.0f))
                {
                    updateFlags |= UPDATEFLAG_NONACTIVE;
                }
            }

            if (!g_pServerMgr->Update(updateFlags, g_pClientMgr->m_nCurUpdateTimeMS,bUpdateUnguaranteed))
            {
                g_pServerMgr->GetErrorString(errorString, 256);
                g_pClientMgr->SetupError(LT_SERVERERROR, errorString);
				RETURN_ERROR_PARAM(1, CClientShell::Update, LT_SERVERERROR|ERROR_DISCONNECT, errorString);
                //GetClientMgr()->ThrowClientException(EXC_SHUTDOWN_SHELL|EXC_MODAL_MESSAGE, Cli_ServerError, errorString);
            }
        }
    }


    m_GameFrameTime = 0.0f;
	
	// Process all packets. 
    dResult = ProcessPackets();
	if (dResult != LT_OK)
	{
		return dResult | ERROR_DISCONNECT;	
	}
    
    // Update the game frame time.
    m_GameFrameTime = m_GameTime - m_LastGameTime;
    m_LastGameTime = m_GameTime;

	// Call PreUpdate and Update functions.
	if (dsi_IsClientActive())
	{
		g_pClientMgr->ProcessAllInput(false);
		
		// Give the predictive views some time!
		// 광식
		//dsi_ConsolePrint( "!!!!!!!!!!!!!!!!!!!!!!! pd_Update ");
		pd_Update(this);
	}

	{
		CountAdder cntAdd(&g_Ticks_ClientShell);

		i_client_shell->PreUpdate();

		i_client_shell->Update();
	}


	if (dsi_IsClientActive())
	{
		// Update all the client object structures from their script counterparts.
		g_pClientMgr->UpdateObjects( uint32(m_fFrameTimeClient * 1000.0f) );
	}

	{
		CountAdder cntAdd(&g_Ticks_ClientShell);

		i_client_shell->PostUpdate();
	}
	

    // Update client obect pointer...
    pClientObject = GetClientObject();
    if (pClientObject)
    {
        if (m_pFrameClientObject != pClientObject)
        {
            m_pFrameClientObject = pClientObject;
        }
    }
	
	ctik_ShowTickStatus();
    return LT_OK;
}


void CClientShell::SendCommandToServer(char *pCommand)
{
	CPacket_Write cPacket;

	cPacket.Writeuint8(CMSG_COMMANDSTRING);
	cPacket.WriteString(pCommand);

    SendPacketToServer(CPacket_Read(cPacket));
}


void CClientShell::SendPacketToServer(const CPacket_Read &cPacket)
{
	g_pClientMgr->m_ClientNetMgr.SendPacket(cPacket, m_HostID);
}



////////////////////////////////////////////////////////
// Main high-level functions
////////////////////////////////////////////////////////

void CClientShell::RemoveAllObjects()
{
	dsi_ConsolePrint( "[ENGINE] CClientShell::RemoveAllObjects" );
    // Remove all the server objects.
	for (int i=0; i < NUM_OBJECTTYPES; i++)
    {
		g_pClientMgr->RemoveObjectsInList(&g_pClientMgr->m_ObjectMgr.m_ObjectLists[i], true);
    }
}


void CClientShell::NotifyWorldClosing()
{
	if (m_bWorldOpened)
    {
		dsi_ConsolePrint( "[ENGINE] CClientShell::NotifyWorldClosing" );
        g_pClientMgr->OnExitWorld(this);
    }
    
    m_bWorldOpened = false;
}



bool CClientShell::CreateVisContainerObjects() {
    uint32 i;
    const WorldData *pWorldData;
    ObjectCreateStruct ocs;
    InternalObjectSetup internalSetup;
    LTObject *pObject;
    LTRESULT dResult;

    internalSetup.m_pSetup = &ocs;

    for (i=0; i < world_bsp_client->NumWorldModels(); i++)
    {
        pWorldData = world_bsp_client->GetWorldModel(i);

        dResult = LT_OK;
        if (pWorldData->OriginalBSP()->m_WorldInfoFlags & (WIF_PHYSICSBSP | WIF_VISBSP))
        {
            // Ok, make an object for it.
            ocs.Clear();
            ocs.m_ObjectType = OT_WORLDMODEL;
            ocs.m_Flags = FLAG_SOLID | FLAG_RAYHIT | FLAG_VISIBLE;
            SAFE_STRCPY(ocs.m_Filename, pWorldData->m_pValidBsp->m_WorldName);
            ocs.m_Pos = pWorldData->OriginalBSP()->m_WorldTranslation;
            
            dResult = g_pClientMgr->AddObjectToClientWorld(OBJID_CLIENTCREATED,
                &internalSetup, &pObject, true, false);

            if (dResult != LT_OK)
            {
                return false;
            }
        }
    }

    return true;
}


bool CClientShell::BindWorlds()
{
	dsi_ConsolePrint( "[ENGINE] CClientShell::BindWorlds" );
    if (!r_IsRenderInitted())
        return true;

    if (!world_bsp_client->RenderContext())
    {
        world_bsp_client->RenderContext() = r_GetRenderStruct()->CreateContext();
        if (!world_bsp_client->RenderContext())
            return false;
    }

    return true;
}



void CClientShell::UnbindWorlds()
{
	dsi_ConsolePrint( "[ENGINE] CClientShell::UnbindWorlds" );
    if (world_bsp_client->RenderContext())
    {
        r_GetRenderStruct()->DeleteContext((HRENDERCONTEXT)world_bsp_client->RenderContext());
        world_bsp_client->RenderContext() = NULL;
    }
}


void clienthack_UnloadWorld()
{
    if (!g_pClientShell)
        return;

	dsi_ConsolePrint( "[ENGINE] clienthack_UnloadWorld" );
            
    g_pClientShell->m_ClientObjectID = (uint16)-1;
    cs_UnloadWorld(g_pClientShell);
}


// Get the FileIDInfo for this file id.
FileIDInfo *CClientShell::GetClientFileIDInfo(uint16 wFileID)
{
    HHashElement *hElement;
    FileIDInfo *pFileIDInfo = NULL;

    // See if it's in the hash table.
    hElement = hs_FindElement(m_hFileIDTable, &wFileID, 2);
    if (hElement)
    {
        pFileIDInfo = (FileIDInfo *)hs_GetElementUserData(hElement);
        if (pFileIDInfo)
            return pFileIDInfo;
    }

    // Create a new fileidinfo...
    pFileIDInfo = (FileIDInfo *)sb_Allocate_z(&g_pClientMgr->m_FileIDInfoBank);
    if (!pFileIDInfo)
        return NULL;

    // Make a new one...
    hElement = hs_AddElement(m_hFileIDTable, &wFileID, 2);
    if (!hElement)
    {
        dfree(pFileIDInfo);
        return NULL;
    }

    // Have the hash table own the pointer...
    hs_SetElementUserData(hElement, (void *)pFileIDInfo);

    return pFileIDInfo;

}


void WorldBsp::SetPolyTexturePointers() {
    uint32 i;
    Surface *pSurface;
    FileRef ref;

    for (i=0; i < m_nSurfaces; i++) {
        pSurface = &m_Surfaces[i];

        ref.m_FileType = FILE_ANYFILE;
        ref.m_pFilename = m_TextureNames[pSurface->m_iTexture];
    
        pSurface->m_pTexture = g_pClientMgr->AddSharedTexture(&ref);

        if (!pSurface->m_pTexture && (g_DebugLevel >= 1))
        {
            dsi_ConsolePrint("Unable to find world texture %s", ref.m_pFilename);
        }

        // Give it a default texture if it's missing its texture.
        if (!pSurface->m_pTexture)
        {
            ref.m_FileType = FILE_CLIENTFILE;
            ref.m_pFilename = "textures\\default_texture.dtx";
            pSurface->m_pTexture = g_pClientMgr->AddSharedTexture(&ref);
        }
    }
}

void cs_UnloadWorld(CClientShell *pShell)
{
	dsi_ConsolePrint( "[ENGINE] cs_UnloadWorld" );
	pShell->NotifyWorldClosing();
	pShell->RemoveAllObjects();
	pShell->CloseWorlds();
}

void CClientShell::CloseWorlds()
{
    int i;

	dsi_ConsolePrint( "[ENGINE] CClientShell::CloseWorlds" );

    // Remove all objects from the BSP.
    for (i=0; i < NUM_OBJECTTYPES; i++)
    {
        RemoveAllObjectsFromWorldTree(&g_pClientMgr->m_ObjectMgr.m_ObjectLists[i]);
    }

    LTLink *pListHead, *pCur;
    LTObject *pObject;

    // Clear their standing-on status.
    for (i=0; i < NUM_OBJECTTYPES; i++)
    {
        pListHead = &g_pClientMgr->m_ObjectMgr.m_ObjectLists[i].m_Head;
        for (pCur=pListHead->m_pNext; pCur != pListHead; pCur=pCur->m_pNext)
        {
            pObject = (LTObject*)pCur->m_pData;

            // Detach it from whatever it's standing on.
            DetachObjectStanding(pObject);
            DetachObjectsStandingOn(pObject);
        }
    }
    
    // Remove all world model objects.
    g_pClientMgr->RemoveObjectsInList(&g_pClientMgr->m_ObjectMgr.m_ObjectLists[OT_WORLDMODEL], false);

    g_pClientMgr->RemoveObjectsInList(&g_pClientMgr->m_ObjectMgr.m_ObjectLists[OT_CONTAINER], false);

    // Unbind all the worlds.
    UnbindWorlds();

    // Shut down the sounds.
    GetClientILTSoundMgrImpl()->StopAllSounds();
    
    // Close any open world files.
    world_bsp_client->Term();
}

LTRESULT CClientShell::DoLoadWorld(const CPacket_Read &cPacket, bool bLocal)
{
	CPacket_Read cLoadPacket(cPacket);
    int i;
    ILTStream *pStream;
    FileRef ref;
    FileIdentifier *pIdent;
    const char *pWorldName;
    ELoadWorldStatus loadStatus;

	dsi_ConsolePrint( "[ENGINE] CClientShell::DoLoadWorld   bLoad(%d)", m_bLoadWorld );
	
	if( LTFALSE == m_bLoadWorld )
	{
		m_GameFrameTime = 0.0f;		
		m_ServerPeriodTrack = g_pClientMgr->m_CurTime;
		m_ServerPeriod = 1.0f / g_CV_UpdateRate;

		//[MURSUM]
		m_fLastUpdateServer	= 0.0f;
		m_fLastUpdateClient	= 0.0f;

		m_fFrameTimeServer	= 0.1f;
		m_fFrameTimeClient	= 0.1f;

		pd_InitialServerUpdate(this, m_GameTime);

		float	fTemp	= cLoadPacket.Readfloat();
		uint16	nTemp	= cLoadPacket.Readuint16();

		g_pClientMgr->OnEnterWorld(this);
		m_bWorldOpened = true;

		if( g_pServerMgr )
		{
			g_pServerMgr->m_GameTime = m_GameTime;
			dsi_ConsolePrint( "Set ServerTime : %f", g_pServerMgr->m_GameTime );
		}

		DoLoadWorldClient( OT_NORMAL, OBJID_CLIENTCREATED_TYPE1 );		//0
		DoLoadWorldClient( OT_CONTAINER, OBJID_CLIENTCREATED_TYPE1 );	//9
		return LT_OK;
	}

	// Cleanup...
	cs_UnloadWorld(this);

	// Tell the client shell to stop rendering for a sec..
	dsi_SendPingToGameServer();
	if (i_client_shell != NULL)
	{
		i_client_shell->OnLockRenderer();
	}


	// UnBind any textures we were using
	g_pClientMgr->UnbindSharedTextures(false);


	// Get the game time.
	m_GameTime = m_LastGameTime = cLoadPacket.Readfloat();
	m_GameFrameTime = 0.0f;

	m_ServerPeriodTrack = g_pClientMgr->m_CurTime;

	m_ServerPeriod = 1.0f / g_CV_UpdateRate;
	
	// Get the world file ID and world pointer.
	ref.m_FileType = FILE_SERVERFILE;
	ref.m_FileID = cLoadPacket.Readuint16();
	pWorldName = client_file_mgr->GetFilename(&ref);

	pIdent = client_file_mgr->GetFileIdentifier(&ref, TYPECODE_WORLD);

	// If we're local and it's the same world, don't reload all the textures.
	bool bFlushUnusedTextures = (pIdent != m_pLastWorld);

	// Get rid of unused Sprites before the world is loaded...
	if (bFlushUnusedTextures)
	{
		g_pClientMgr->TagAndFreeSprites();
	}

	//[MURSUM]
	m_fLastUpdateServer	= 0.0f;
	m_fLastUpdateClient	= 0.0f;

	m_fFrameTimeServer	= 0.1f;
	m_fFrameTimeClient	= 0.1f;
	
	//Init the prediction stuff with the server's time.
	pd_InitialServerUpdate(this, m_GameTime);

	// Load the client data
	pStream = client_file_mgr->OpenFile(&ref);
	if (!pStream)
	{
		// Tell the client shell it can render again..
		if (i_client_shell != NULL)
			i_client_shell->OnUnLockRenderer();

		RETURN_ERROR(1, CClientShell::DoLoadWorld, LT_MISSINGWORLDFILE);
	}

//BEGIN/////////////////////////////////////////////////////////////////////

	dsi_SendPingToGameServer();

	//check if we have a local server.
	if (m_bLocal)
	{
		//inherit our client world from the server world.
		if (!world_bsp_client->InheritFromServer())
		{
			// Tell the client shell it can render again..
			if (i_client_shell != NULL)
				i_client_shell->OnUnLockRenderer();

			RETURN_ERROR_PARAM(1, ClientShell::DoLoadWorld, LT_ERROR, "Inherit failed");
		}

		dsi_SendPingToGameServer();

		loadStatus = world_bsp_client->LoadClientData(pStream);
	}
	else
	{
		// Notify the client shell and show the draw surface so they can put up a bitmap.
		if (i_client_shell != NULL) {
			i_client_shell->PreLoadWorld(pWorldName);
		}

		if (!world_bsp_client->InheritFromServer())
		{
			// Tell the client shell it can render again..
			if (i_client_shell != NULL)
				i_client_shell->OnUnLockRenderer();

			RETURN_ERROR_PARAM(1, ClientShell::DoLoadWorld, LT_ERROR, "Inherit failed");
		}

		loadStatus = world_bsp_client->LoadClientData(pStream);
	}

	// Release the stream, we're done loading
	pStream->Release();

	dsi_SendPingToGameServer();

//END/////////////////////////////////////////////////////////////////////


    // Invalid file?
    if (loadStatus != LoadWorld_Ok)
    {
		// Tell the client shell it can render again..
		if (i_client_shell != NULL)
			i_client_shell->OnUnLockRenderer();

        g_pClientMgr->SetupError(LT_INVALIDWORLDFILE, pWorldName);
        RETURN_ERROR_PARAM(1, CClientShell::DoLoadWorld, LT_INVALIDWORLDFILE, pWorldName);
    }

    // Try to bind to the worlds.
    if (!BindWorlds())
	{
		// Tell the client shell it can render again..
		if (i_client_shell != NULL)
			i_client_shell->OnUnLockRenderer();


        RETURN_ERROR(1, CClientShell::DoLoadWorld, LT_ERRORBINDINGWORLD);
    }

//BEGIN/////////////////////////////////////////////////////////////////////

	dsi_SendPingToGameServer();
	
    // Get rid of unused shared textures.
    if (bFlushUnusedTextures)
    {
        g_pClientMgr->TagAndFreeTextures();
    }

    // Set poly texture pointers.
    world_bsp_client->SetWorldModelOriginalBSPPolyTexturePointers();

    // Bind textures so we're ready to go.
    g_pClientMgr->BindUnboundTextures();

	// Tell the client shell it can render again..
	dsi_SendPingToGameServer();
	if (i_client_shell != NULL)
	{
		i_client_shell->OnUnLockRenderer();
	}

    // Create objects for all the vis containers.
    if (!CreateVisContainerObjects())
    {
        CloseWorlds();
        RETURN_ERROR_PARAM(1, CClientShell::DoLoadWorld, LT_ERROR, "CreateVisContainerObjects failed");
    }
    
    // Add all objects back into the BSP.
    for (i=0; i < NUM_OBJECTTYPES; i++)
    {
        AddAllObjectsToBSP(&g_pClientMgr->m_ObjectMgr.m_ObjectLists[i]);
    }

//END/////////////////////////////////////////////////////////////////////

	DoLoadWorldClient( OT_NORMAL, OBJID_CLIENTCREATED_TYPE1 );		//0
	DoLoadWorldClient( OT_SPRITE, OBJID_CLIENTCREATED_TYPE2 );		//3
	DoLoadWorldClient( OT_CONTAINER, OBJID_CLIENTCREATED_TYPE1 );	//9

    m_pLastWorld = pIdent;
          
    // Call the OnEnterWorld function.
    g_pClientMgr->OnEnterWorld(this);
    m_bWorldOpened = true;

	return LT_OK;
}

LTRESULT CClientShell::DoLoadWorldClient( uint8 nObjectType, uint16 nObjectID )
{
	LTLink		*pListHead, *pCur;
	LTObject	*pServerObj, *pClientObj;

	LTRESULT	dResult;
	
	pListHead = &g_pServerMgr->m_ObjectMgr.m_ObjectLists[nObjectType].m_Head;
    for( pCur=pListHead->m_pNext ; pCur != pListHead ; pCur=pCur->m_pNext )
	{
		pServerObj = (LTObject*)pCur->m_pData;	

		//*
		if (!(pServerObj->m_Flags & FLAG_FORCECLIENTUPDATE)) 
		{
			if((pServerObj->m_ObjectType == OT_NORMAL) && (pServerObj->sd->m_cSpecialEffectMsg.Size() == 0))
			{
				continue;
			}

			if (!(pServerObj->m_Flags & (FLAG_VISIBLE|FLAG_SOLID|FLAG_RAYHIT) ) )
			{
				continue;
			}
		}
		if (!(pServerObj->m_InternalFlags & IFLAG_INWORLD))
		{
			continue;
		}		
		if(pServerObj->IsMainWorldModel())
		{
			continue;
		}
		//*/

		ObjectCreateStruct createStruct;
		InternalObjectSetup setup;
		setup.m_pSetup	= &createStruct;
		g_pServerMgr->SetInternalObjectSetup( &setup, pServerObj );
		dResult = g_pClientMgr->AddObjectToClientWorld( nObjectID, 
														&setup,
														&pClientObj,
														true,
														true );
		if (dResult != LT_OK) return dResult;
		
		PostLoadClient( pServerObj, pClientObj );
		
		if( !pServerObj->sd->m_cSpecialEffectMsg.Empty() && i_client_shell != NULL )
		{
			i_client_shell->SpecialEffectNotify( pClientObj, CCast_Client(pServerObj->sd->m_cSpecialEffectMsg) );
        }        
	}

	return LT_OK;
}

LTRESULT CClientShell::PostLoadClient( LTObject* pServerObj, LTObject* pClientObj )
{
	pClientObj->m_UserFlags		= pServerObj->m_UserFlags;
	pClientObj->m_ColorA		= pServerObj->m_ColorA;
	pClientObj->m_ColorR		= pServerObj->m_ColorR;
	pClientObj->m_ColorG		= pServerObj->m_ColorG;
	pClientObj->m_ColorB		= pServerObj->m_ColorB;
	pClientObj->m_nRenderGroup	= pServerObj->m_nRenderGroup;

	switch( pClientObj->m_ObjectType )
	{
	case OT_NORMAL:
		break;
	case OT_WORLDMODEL:
		break;
	case OT_SPRITE:
		((SpriteInstance*)pClientObj)->m_ClipperPoly = ToSprite(pServerObj)->m_ClipperPoly;//((SpriteInstance*)pServerObj)->m_ClipperPoly;
		break;
	case OT_CONTAINER:
		break;
	}

	return LT_OK;
}

LTRESULT CClientShell::DoLoadSoundClient() 
{
	LTLink *pCur;
	FileRef fileRef;
	PlaySoundInfo playSoundInfo;

	GetClientILTSoundMgrImpl()->SetEnable(true);

	for( pCur=g_pServerMgr->m_SoundTrackList.m_Head.m_pNext ; pCur!=&g_pServerMgr->m_SoundTrackList.m_Head ; pCur=pCur->m_pNext )
	{
		g_pServerMgr->SetPlaySoundInfo( &playSoundInfo, &fileRef, pCur->m_pData );

		if( !(playSoundInfo.m_dwFlags & PLAYSOUND_AMBIENT) )
			continue;

		g_pClientMgr->PlaySound( &playSoundInfo, &fileRef, 0.0f );
	}
	return LT_OK;
}