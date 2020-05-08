// ----------------------------------------------------------------------- //
//
// MODULE  : SAGameClientShell.cpp
//
// PURPOSE : Game Client Shell - Implementation
//
// CREATED : 11/5/01
//
// (c) 1997-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "SAGameClientShell.h"

#include "msgids.h"

#include "clientmultiplayermgr.h"
#include "iserverdir.h"
#include "CurseFilter.h"

// ----------------------------------------------------------------------- 
// [bestline] 2004.04.16 
//  Gamehi client 
//#define SAGameClientShell
//#include "..\\gamehiclient\\GameHiClient.h"
// [bestline] 2004.04.16 
// ----------------------------------------------------------------------- 

// Sample rate
int g_nSampleRate = 22050;

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAGameClientShell::CSAGameClientShell()
//
//	PURPOSE:	Initialization
//
// ----------------------------------------------------------------------- //

CSAGameClientShell::CSAGameClientShell() : CGameClientShell()
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAGameClientShell::~CSAGameClientShell()
//
//	PURPOSE:	Destruction
//
// ----------------------------------------------------------------------- //

CSAGameClientShell::~CSAGameClientShell()
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAGameClientShell::OnEngineInitialized
//
//	PURPOSE:	Wrapper for OnEngineInitialized so we can do something extra
//
// ----------------------------------------------------------------------- //

uint32 CSAGameClientShell::OnEngineInitialized(RMode *pMode, LTGUID *pAppGuid)
{
	uint32 nResult = CGameClientShell::OnEngineInitialized(pMode, pAppGuid);
	m_VersionMgr.Update();

	//with the update of the version manager, it might have changed the gore setting,
	//so make sure that the FX managers know about the change
	UpdateGoreSettings();

	LoadCursesWithNoCurse( "./ist.dat", "./nist.dat" );
	return nResult;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAGameClientShell::ProcessPacket
//
//	PURPOSE:	Handling for connectionless networking packets
//
// ----------------------------------------------------------------------- //

LTRESULT CSAGameClientShell::ProcessPacket(ILTMessage_Read *pMsg, uint8 senderAddr[4], uint16 senderPort)
{
	// Gotta have some data...
	if (pMsg->Size() < 16)
		return LT_OK;

	// Skip the engine-side portion of the message header
	CLTMsgRef_Read cSubMsg(pMsg->SubMsg(8));

	// Remember what address this came from
	g_pClientMultiplayerMgr->SetCurMessageSource(senderAddr, senderPort);

	// Hand it off as if it were a message
	// Note : This has to go through the parent's OnMessage so we handle the message source properly
	CGameClientShell::OnMessage(cSubMsg);

	return LT_OK;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAGameClientShell::OnMessage
//
//	PURPOSE:	Message from the server
//
// ----------------------------------------------------------------------- //
void CSAGameClientShell::OnMessage(ILTMessage_Read *pMsg)
{
	// Store the server's address as where this message came from
	uint8 aAddrBuffer[4];
	uint16 nPort;
	g_pLTClient->GetServerIPAddress(aAddrBuffer, &nPort);
	g_pClientMultiplayerMgr->SetCurMessageSource(aAddrBuffer, nPort);
	
	CGameClientShell::OnMessage(pMsg);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAGameClientShell::OnEnterWorld()
//
//	PURPOSE:	Handle entering world
//
// ----------------------------------------------------------------------- //

void CSAGameClientShell::OnEnterWorld()
{
	CGameClientShell::OnEnterWorld();

#ifndef _FINAL
	//override for debugging purposes
	float fVal = 0.0f;
    if (!g_pLTClient) return;

    HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar("LowViolence");
	if(hVar)
	{
        fVal = g_pLTClient->GetVarValueFloat(hVar);
		char buf[128] = "";
		sprintf(buf, "serv LowViolence %f",fVal);
		g_pLTClient->RunConsoleString(buf);
	}


#endif
}