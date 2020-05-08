// ----------------------------------------------------------------------- //
//
// MODULE  : TronInterfaceMgr.cpp
//
// PURPOSE : Manage all interface related functionality
//
// CREATED : 4/6/99
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "SAInterfaceMgr.h"
#include "GameClientShell.h"
#include "MissionMgr.h"
#include "ScreenMain.h"

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAInterfaceMgr::CSAInterfaceMgr
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CSAInterfaceMgr::CSAInterfaceMgr() : CInterfaceMgr()
{
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAInterfaceMgr::~CSAInterfaceMgr
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CSAInterfaceMgr::~CSAInterfaceMgr()
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAInterfaceMgr::OnExitWorld()
//
//	PURPOSE:	Handle exiting a world
//
// ----------------------------------------------------------------------- //

void CSAInterfaceMgr::OnExitWorld()
{
	// Now handle the base class
	CInterfaceMgr::OnExitWorld();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSAInterfaceMgr::OnCommandOn()
//
//	PURPOSE:	Handle command on
//
// ----------------------------------------------------------------------- //
LTBOOL CSAInterfaceMgr::OnCommandOn(int command)
{
	// Take appropriate action

	//[SPIKE] 새로운 라디오 메세지 커멘드 
	if(g_pRadio->IsVisible())
	{
		uint8 nChoice = 100;
		switch(command) {
		case COMMAND_ID_STRAFE:
			nChoice = 0;
			break;
		case COMMAND_ID_NEXT_AMMO:
			nChoice = 1;
			break;
		case COMMAND_ID_HOLSTER:
			nChoice = 2;
			break;
		case COMMAND_ID_MOVE_BODY:
			nChoice = 3;
			break;
		case COMMAND_ID_DUCKLOCK:
			nChoice = 4;
			break;
		case COMMAND_ID_COMPASS:
			nChoice = 5;
			break;		
		default:
			break;
		}
		if(nChoice != 100)
		{
			g_pRadio->Choose(nChoice);
			return LTTRUE;
		}
	}

	switch (command)
	{
		case COMMAND_ID_MESSAGE :
		case COMMAND_ID_TEAM_MESSAGE :
		{
			if (!IsTeamGameType() && (command == COMMAND_ID_TEAM_MESSAGE))
				break;

			if( ::GetKeyState( VK_RCONTROL )&0x80 )
			{
				command = COMMAND_ID_TEAM_MESSAGE;
			}
			else if( ::GetKeyState( VK_LCONTROL )&0x80 )
			{
				command = COMMAND_ID_TEAM_MESSAGE;
			}

			if (m_eGameState == GS_PLAYING && !g_pPlayerMgr->IsSpectatorMode() && !FadingScreen())
			{
				if (m_AmmoChooser.IsOpen())
				{
					m_AmmoChooser.Close();
				}
				if (m_WeaponChooser.IsOpen())
				{
					m_WeaponChooser.Close();
				}

                if( g_pChatInput->IsVisible() )
				{
					g_pChatInput->OnEnter();
				}
				else
				{
#ifndef _STANDALONE
					switch( g_pGameClientShell->GetGameType() )
					{
					case eGameTypeSurvival:
						g_pChatInput->Show( (command==COMMAND_ID_MESSAGE)?kMsgChat:kMsgTeam );
						break;

					default:
						g_pChatInput->Show( (command==COMMAND_ID_MESSAGE)?kMsgChat:kMsgTeam );
						break;
					}
					
#endif
				}
			}
			
            return LTTRUE;
		}
		break;

		case COMMAND_ID_RADIO :
		{
			if (m_eGameState == GS_PLAYING && (g_pGameClientShell->GetGameType() == eGameTypeCooperative || IsTeamGameType() )  )
			{
				// [KLS 9/9/02] Made toggle and added sounds...
				if( !g_pPlayerMgr->IsPlayerDead() )
				{
					g_pRadio->Show( RADIO_FIRST );				
				}
			}

            return LTTRUE;
		}
		break;

		case COMMAND_ID_RADIO_2 :
		{
			if (m_eGameState == GS_PLAYING && (g_pGameClientShell->GetGameType() == eGameTypeCooperative || IsTeamGameType() )  )
			{
				// [KLS 9/9/02] Made toggle and added sounds...
				if( !g_pPlayerMgr->IsPlayerDead() )
				{
					g_pRadio->Show( RADIO_SECOND );				
				}
			}

            return LTTRUE;
		}
		break;

		case COMMAND_ID_RADIO_3 :
		{
			if (m_eGameState == GS_PLAYING && (g_pGameClientShell->GetGameType() == eGameTypeCooperative || IsTeamGameType() )  )
			{
				// [KLS 9/9/02] Made toggle and added sounds...
				if( !g_pPlayerMgr->IsPlayerDead() )
				{
					g_pRadio->Show( RADIO_THIRD );				
				}
			}

            return LTTRUE;
		}
		break;
		
		case COMMAND_ID_SENSITIVITY_LOW:
		{
			if( this->GetGameState() == GS_PLAYING )
			{
				//ChangeSensitive(-1);
				g_pHUDSensiCtrl->CalculateSensitivity(LTTRUE); // 감소 
			}
		}
		break;
		case COMMAND_ID_SENSITIVITY_HIGH:
		{
			if( this->GetGameState() == GS_PLAYING )
			{
				//ChangeSensitive(1);
				g_pHUDSensiCtrl->CalculateSensitivity(LTFALSE); // 증가  
			}
		}
		break;


		default :
		{
			return CInterfaceMgr::OnCommandOn(command);
		}
		break;
	}
    return LTFALSE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::OnKeyDown()
//
//	PURPOSE:	Handle OnKeyDown messages
//
// ----------------------------------------------------------------------- //

LTBOOL CSAInterfaceMgr::OnKeyDown(int key, int rep)
{
	//handle stuff before default handling
	if (g_pRadio->IsVisible() && m_eGameState == GS_PLAYING && key == VK_ESCAPE)
	{
		g_pRadio->Show( RADIO_HIDE );
		return LTTRUE;
	}
	
	//default handling
	if (CInterfaceMgr::OnKeyDown(key, rep))
		return LTTRUE;

	//handle stuff after default handling
	// (nothing needed here yet)
	

	return LTFALSE;
}

