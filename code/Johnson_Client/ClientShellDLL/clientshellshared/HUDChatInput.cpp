#include "stdafx.h"
#include "HUDMgr.h"
#include "InterfaceResMgr.h"
#include "MissionMgr.h"
#include "../SA/CurseFilter.h"
#include "GameText.h"
#include "GameMessage.h"

CHUDChatInput::CHUDChatInput()
: CHUDItem()
{
	m_UpdateFlags	= kHUDNone;
	m_eLevel		= kHUDRenderNone;
	m_eChatMsgType	= kMsgChat;
	m_pChatEdit		= LTNULL;
}

LTBOOL CHUDChatInput::Init()
{
	m_pChatEdit = new CLTGUIEditBox();
	m_pChatEdit->Create( 13, 540, 320, 20,
						RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
						D3D_TEXT_SHADOW,
						LTFALSE,
						LTNULL, LTNULL );
	m_pChatEdit->SetCommandHandler(this);

	m_hChatBack				= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/ChatInput/ChatBack.dtx" );

	m_hChatTypeAll			= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/ChatInput/ChatTypeAll.dtx" );
	m_hChatTypeTeam			= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/ChatInput/ChatTypeTeam.dtx" );
	m_hChatTypeGhost		= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/ChatInput/ChatTypeGhost.dtx" );
	m_hChatTypeTeamGhost	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/ChatInput/ChatTypeTeamGhost.dtx" );
	
	SetupQuadUVs( m_Poly, m_hChatBack, 0.0f, 0.0f, 322.0f/512.0f, 22.0f/32.0f );
	g_pDrawPrim->SetRGBA( &m_Poly, 0xFFFFFFFF );
	
	SetupQuadUVs( m_PolyType, m_hChatTypeAll, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyType, 0xFFFFFFFF );

	m_bVisible	= LTFALSE;
	this->ScreenDimChanged();

	return LTTRUE;
}

void CHUDChatInput::InitGame()
{
	m_nChatCount = 0;
	m_bDisableChat	= LTFALSE;
	m_fDoBae	= m_fChatTime = g_pLTClient->GetTime();
}

void CHUDChatInput::Term()
{
	if( m_pChatEdit )
	{
		Hide();
		delete m_pChatEdit;
		m_pChatEdit	= LTNULL;
	}	
}

void CHUDChatInput::Update()
{
}

void CHUDChatInput::Render()
{
	if( m_bVisible )
	{
		g_pDrawPrim->SetTexture( m_hTypeTex );
		g_pDrawPrim->DrawPrim( &m_PolyType );
		g_pDrawPrim->SetTexture( m_hChatBack );
		g_pDrawPrim->DrawPrim( &m_Poly );
		if( m_pChatEdit ) m_pChatEdit->Render();
	}

	if( m_bDisableChat )
	{
		if( g_pLTClient->GetTime() - m_fDoBae > 20.0f )
		{
			m_bDisableChat = LTFALSE;
		}
	}
}

void CHUDChatInput::Show( eChatMsgType eType )
{
	m_bVisible		= LTTRUE;
	m_eChatMsgType	= eType;

	switch( g_pGameClientShell->GetGameType() )
	{
	case eGameTypeSurvival:
		if( kMsgChat == m_eChatMsgType )
		{
			if( g_pPlayerMgr->IsPlayerDead() && false == g_pMissionMgr->IsRoundOver() )
			{
				m_hTypeTex	= m_hChatTypeGhost;
			}
			else
			{
				m_hTypeTex	= m_hChatTypeAll;
			}
		}
		else
		{
			if( g_pPlayerMgr->IsPlayerDead() && false == g_pMissionMgr->IsRoundOver() )
			{
				m_hTypeTex	= m_hChatTypeTeamGhost;
			}
			else
			{
				m_hTypeTex	= m_hChatTypeTeam;
			}
		}
		break;
		
	default:
		if( kMsgChat == m_eChatMsgType )
		{
			m_hTypeTex	= m_hChatTypeAll;
		}
		else
		{
			m_hTypeTex	= m_hChatTypeTeam;
		}
	}

	m_pChatEdit->ForceAddEditBox();

	g_pGameClientShell->SetInputState(false);
	g_pLTClient->ClearInput();
}

void CHUDChatInput::Hide()
{
	m_bVisible	= LTFALSE;
	m_pChatEdit->OnLostFocus();

	g_pGameClientShell->SetInputState(true);
	g_pLTClient->ClearInput();
}

void CHUDChatInput::OnChar()
{
	if( stricmp( m_pChatEdit->GetText(), "/w" ) == 0 || 
		//"/ㅈ"
		strcmp( m_pChatEdit->GetText(), NF_GetMsg(SA_TEXT_CMD_WHISPER) ) == 0 )
	{
		if( false == m_pChatEdit->IsLastBack() && strlen( g_GameDoc.GetLastSendNick() ) > 0 )
		{
			m_pChatEdit->StrCat( " " );
			m_pChatEdit->StrCat( g_GameDoc.GetLastSendNick() );
			m_pChatEdit->StrCat( " " );
		}
	}
	else if( stricmp( m_pChatEdit->GetText(), "/r" ) == 0 ||
		//"/ㄱ"
		strcmp( m_pChatEdit->GetText(), NF_GetMsg(SA_TEXT_CMD_REPLY) ) == 0 )
	{
		if( strlen( g_GameDoc.GetLastRecvNick() ) > 0 )
		{
			m_pChatEdit->ClearText();
			m_pChatEdit->StrCat( "/W " );
			m_pChatEdit->StrCat( g_GameDoc.GetLastRecvNick() );
			m_pChatEdit->StrCat( " " );
		}
	}
}

void CHUDChatInput::OnEnter()
{
	static char szChatMsg[1025];

	if( m_pChatEdit )
	{
		if( m_pChatEdit->GetChar(0) == '/' )
		{
			if( LTTRUE == EditCommand( m_pChatEdit->GetText() ) ) return;
		}

		if( LTTRUE == m_bDisableChat )
		{
			m_pChatEdit->ClearText();
			Hide();
			return;
		}
		
#ifdef GAME_SHOW
		if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
		{
			sprintf( szChatMsg, "[%s] %s", g_GameDoc.GetNick(), m_pChatEdit->GetText() );
			g_Network.CS_GameChat( szChatMsg );
			g_pChatMsgs->AddMessage( szChatMsg, kMsgChat );
			m_pChatEdit->ClearText();
			Hide();
			return;
		}
#endif

		if( strlen( m_pChatEdit->GetText() ) > 0 )
		{
			if( LTFALSE == IsCurse( m_pChatEdit->GetText() ) ) //욕 필터링
			{
				sprintf( szChatMsg, "[%s] %s", g_GameDoc.GetNick(), m_pChatEdit->GetText() );

				switch( g_pGameClientShell->GetGameType() )
				{
				case eGameTypeSurvival:
					if( kMsgChat == m_eChatMsgType )
					{
						if( g_pPlayerMgr->IsPlayerDead() && false == g_pMissionMgr->IsRoundOver() )
						{
							m_eChatMsgType = kMsgGhost;
							g_Network.CS_GhostChat( szChatMsg );
							sprintf( szChatMsg, NF_GetMsg(SA_TEXT_TO_GHOST), m_pChatEdit->GetText() );
						}
						else
						{
							g_Network.CS_GameChat( szChatMsg );
						}
					}
					else
					{
						if( g_pPlayerMgr->IsPlayerDead() && false == g_pMissionMgr->IsRoundOver() )
						{
							m_eChatMsgType = kMsgTeamGhost;
							g_Network.CS_GhostTeamChat( szChatMsg );
							sprintf( szChatMsg, NF_GetMsg(SA_TEXT_TO_GHOST_TEAM), m_pChatEdit->GetText() );
						}
						else
						{
							g_Network.CS_TeamChat( szChatMsg );
							sprintf( szChatMsg, NF_GetMsg(SA_TEXT_TO_TEAM), m_pChatEdit->GetText() );
						}
					}
					break;

				default:
					if( kMsgChat == m_eChatMsgType )
					{	
						g_Network.CS_GameChat( szChatMsg );						
					}
					else
					{
						g_Network.CS_TeamChat( szChatMsg );
						sprintf( szChatMsg, NF_GetMsg(SA_TEXT_TO_TEAM), m_pChatEdit->GetText() );
					}
					break;
				}			
			}
			g_pChatMsgs->AddMessage( szChatMsg, m_eChatMsgType );
			CheckDoBae();
			m_pChatEdit->ClearText();
		}
	}

	Hide();
}

void CHUDChatInput::OnEscape()
{
	Hide();
}

void CHUDChatInput::ScreenDimChanged()
{
	g_pDrawPrim->SetXYWH( &m_Poly,
						  13.0f*g_pInterfaceResMgr->GetXRatio(),
						  540.0f*g_pInterfaceResMgr->GetYRatio() - 30.0f,///////////////////////
						  322.0f, 22.0f );

	LTIntPt Pt;
	Pt.x = int(13.0f*g_pInterfaceResMgr->GetXRatio());
	Pt.y = int(540.0f*g_pInterfaceResMgr->GetYRatio()) - 30;///////////////////////
	m_pChatEdit->SetBasePos( Pt );

	g_pDrawPrim->SetXYWH( &m_PolyType,
						  13.0f*g_pInterfaceResMgr->GetXRatio(),
						  540.0f*g_pInterfaceResMgr->GetYRatio() - 52.0f,
						  128.0f, 32.0f );
}

LTBOOL CHUDChatInput::EditCommand( const char* lpszChatMessage )
{
	if( lpszChatMessage == NULL ) return LTFALSE;
	char szChatMessage[MAX_TEXT_BUFFER];
	char szNick[MAX_NICK_LEN];
	LPSTR lpszToken;
	unsigned int iOffset = 0;

	memset( szChatMessage, 0, MAX_TEXT_BUFFER );
	strcpy( szChatMessage, lpszChatMessage );
	lpszToken = strtok( szChatMessage, TEXT(" ") ); 
	if( lpszToken == NULL ) return FALSE;

	iOffset += strlen( lpszToken )+1;
	
	if( stricmp( lpszToken, "/W" ) == 0 || strcmp( lpszToken, "/ㅈ" ) == 0 )
	{
		lpszToken = strtok( NULL, TEXT(" ") ); // <- ID
		if( lpszToken == NULL )
		{
			//"유효하지 않은 명령입니다."
			g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_INVALID_COMMAND), kMsgNotice );
			//"[귓말하기 ] /w <아이디> <메세지>"
			g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_HELP_WHISPER), kMsgNotice );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}
		iOffset += strlen( lpszToken )+1;
		strncpy( szNick, lpszToken, MAX_NICK_LEN-1 );
		char szWhisper[MAX_TEXT_BUFFER];
		memset( szWhisper, 0, MAX_TEXT_BUFFER );
		if( iOffset >= strlen( lpszChatMessage ) )
		{
			//"유효하지 않은 명령입니다."
			g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_INVALID_COMMAND), kMsgNotice );
			//"[귓말하기 ] /w <아이디> <메세지>"
			g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_HELP_WHISPER), kMsgNotice );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}
		memcpy( szWhisper, lpszChatMessage+iOffset, strlen( lpszChatMessage )-iOffset );
		if( strlen( szWhisper ) == 0 )
		{
			//"유효하지 않은 명령입니다."
			g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_INVALID_COMMAND), kMsgNotice );
			//"[귓말하기 ] /w <아이디> <메세지>"
			g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_HELP_WHISPER), kMsgNotice );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}
		char szTemp[256];
		if( strcmp( szNick, g_GameDoc.GetNick() ) == 0 )
		{
			//"자신에게는 귓말을 보낼 수 없습니다."
			g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_SELF_WHISPER_FAIL), kMsgNotice );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}
		else if( LTTRUE == CheckSameRoom( szNick ) )
		{
			g_pChatMsgs->AddMessage( "죽은 사람은 산 사람에게 귓말을 보낼 수 없습니다. ", kMsgNotice );
			m_pChatEdit->ClearText();
			return LTFALSE;
		}
		

		wsprintf( szTemp, "  [To:%s] %s", szNick, szWhisper );
		g_pChatMsgs->AddMessage( szTemp, kMsgWhisper );
		
		if( LTFALSE == IsCurse( szWhisper ) )
		{	
			g_Network.CS_Whisper( szNick, szWhisper );
		}

		m_pChatEdit->ClearText();
		return LTTRUE;
	}
	else if( stricmp( lpszToken, "/C" ) == 0 ||
		//"/ㅊ"
		strcmp( lpszToken, NF_GetMsg(SA_TEXT_CMD_CHAT_CLEAR) ) == 0 )
	{
		g_pChatMsgs->Clear();
		m_pChatEdit->ClearText();
		return TRUE;
	}	
	else if( strcmp( lpszToken, "/?" ) == 0 )
	{
		//"[귓말하기] /W <아이디> <메세지>"
		g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_HELP_WHISPER), kMsgNotice );
		//"[답변하기] /R"
		g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_HELP_REPLY), kMsgNotice );
		//"[채팅창 글씨지우기] /C"
		g_pChatMsgs->AddMessage( NF_GetMsg(SA_TEXT_HELP_CLEAR_CHAT), kMsgNotice );
		m_pChatEdit->ClearText();
	}
	else if( strcmp( lpszToken, "/##TEST_INFO##" ) == 0 )
	{
		g_Network.CS_DbgPlayerInfo();
		m_pChatEdit->ClearText();
	}
		
	return LTFALSE;
}


void CHUDChatInput::CheckDoBae()
{
	if( g_pLTClient->GetTime() - m_fChatTime < 0.8f )
	{
		++m_nChatCount;
	}
	else
	{
		m_nChatCount = 0;
	}

	m_fChatTime = g_pLTClient->GetTime();
	
	if( m_nChatCount > 3 )
	{
		g_pChatMsgs->AddMessage( "도배하셨으므로 20초간 채팅을 금지합니다.", kMsgNotice );
		m_fDoBae = m_fChatTime;
		m_nChatCount = 0;
		m_bDisableChat = LTTRUE;
	}
}

LTBOOL CHUDChatInput::CheckSameRoom( const char* lpszNick )
{
	if( LTNULL == lpszNick ) return LTFALSE;
	
	T_PLAYER_INFOptr pPI = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByNick( lpszNick );
	if( pPI )
	{
		if( pPI->bInGame && pPI->bRecvInfo && PS_ALIVE == pPI->ePlayerState ) return LTTRUE;
	}

	return LTFALSE;
}