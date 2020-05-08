#include "StdAfx.h"
#include "HUDList.h"

//------------------------------------------- (B) Novice (minjin) [03/12/2005]
#include "GameText.h"
#include "GameMessage.h"
//------------------------------------------- (E) Novice (minjin) [03/12/2005]

#define A_TEAM_COLOR RGB(238,120,24)
#define B_TEAM_COLOR RGB(88,142,248)

CHUDList::CHUDList()
{
}

LTBOOL CHUDList::Init( int32 nPosX, int32 nPosY, int32 nItemWidth, int32 nItemHeight, int32 nMaxItemCount, float fLifeTime )
{
	m_nMaxItemCount	= nMaxItemCount;
	m_fLifeTime		= fLifeTime;
	m_fLastCheckTime= 0.0f;

	m_nOriginPosX	= nPosX;
	m_nOriginPosY	= nPosY;
	m_nPosX			= int(nPosX*g_pInterfaceResMgr->GetXRatio());
	m_nPosY			= int(nPosY*g_pInterfaceResMgr->GetYRatio());
	m_nItemWidth	= nItemWidth;
	m_nItemHeight	= nItemHeight;

	Clear();

	ScreenDimChanged();

	return LTTRUE;
}

void CHUDList::Term()
{
	Clear();
}

void CHUDList::Clear()
{
	if( !m_HudList.empty() )
	{
		for( uint16 i=0 ; i<m_HudList.size() ; ++i )
		{
			SAFE_DELETE( m_HudList[i] );
		}
		m_HudList.clear();
	}
}

LTIntPt CHUDList::AddListItem( float fLifeTime )
{
	T_HUD_LISTptr pList = new T_HUD_LIST;

	pList->fLifeTime	= g_pLTClient->GetTime() + ( (fLifeTime<=0.0f) ? m_fLifeTime : fLifeTime );
	pList->nBasePosX	= m_nPosX;
	pList->nBasePosY	= m_nPosY+(m_HudList.size()*m_nItemHeight);
	m_TempPt.x			= pList->nBasePosX;
	m_TempPt.y			= pList->nBasePosY;

	m_HudList.push_back( pList );

	return m_TempPt;
}

void CHUDList::CheckDeleteMessage( float fCurTime )
{
	if( m_HudList.empty() ) return;

	HUD_LIST_ITR itr = m_HudList.begin();
	T_HUD_LISTptr pList = (T_HUD_LISTptr)(*itr);

	if( pList->fLifeTime <= fCurTime )
	{
		DeleteMessage();
		return;
	}
}

void CHUDList::DeleteMessage()
{
	HUD_LIST_ITR itr = m_HudList.begin();
	T_HUD_LISTptr pList = (T_HUD_LISTptr)(*itr);

	if (itr != m_HudList.end())
	{
		m_HudList.erase(itr);
		SAFE_DELETE( pList );
	}
}

void CHUDList::Update()
{
	if( m_HudList.empty() ) return;
	
	float	fCurTime = g_pLTClient->GetTime();
	if( fCurTime - m_fLastCheckTime > 0.5f )
	{
		m_fLastCheckTime	= fCurTime;
		CheckDeleteMessage( fCurTime );
	}
}

void CHUDList::ScreenDimChanged()
{
	m_nPosX = int(m_nOriginPosX*g_pInterfaceResMgr->GetXRatio());
	m_nPosY = int(m_nOriginPosY*g_pInterfaceResMgr->GetYRatio());
	this->SetPosition();
}




CHUDChatMsg::CHUDChatMsg()
:CHUDList()
{
	m_UpdateFlags	= kHUDAll;
	m_eLevel		= kHUDRenderGameOver;
	m_pTempText		= LTNULL;
}

LTBOOL CHUDChatMsg::Init( int32 nPosX, int32 nPosY, int32 nItemWidth, int32 nItemHeight, int32 nMaxItemCount, float fLifeTime )
{
	CHUDList::Init( nPosX, nPosY, nItemWidth, nItemHeight, nMaxItemCount, fLifeTime );

	m_fStartAlpha = 255.0f;

	m_pTempText	= new CSAD3DText();
	m_pTempText->CreateD3DText( 16, 16, false );
	return LTTRUE;
}

HDC CHUDChatMsg::GetDC()
{
	if( m_pTempText ) 
	{
		return m_pTempText->GetDC();
	}
	return LTNULL;
}

void CHUDChatMsg::Term()
{
	CHUDList::Term();
	
	SAFE_DELETE( m_pTempText );
}

void CHUDChatMsg::Clear()
{
	CHUDList::Clear();

	if( !m_ChatMsg.empty() )
	{
		for( uint16 i=0 ; i<m_ChatMsg.size() ; ++i )
		{
			SAFE_DELETE( m_ChatMsg[i]->pText );
			SAFE_DELETE( m_ChatMsg[i] );
		}
		m_ChatMsg.clear();
	}
}

void CHUDChatMsg::AddMessage( const char* lpszMessage, eChatMsgType type )
{
	static char szBuffer[1024];
	
	SIZE cSize;
	
	if( type == kMsgGhost || type == kMsgTeamGhost )
	{
		if( LTFALSE == g_pPlayerMgr->IsPlayerDead() ) return;
	}

	HDC hDC = this->GetDC();	

	strcpy( szBuffer, lpszMessage );
	char* start = szBuffer;
	char* end	= szBuffer;

	if( !hDC ) return;

	::GetTextExtentPoint32( hDC, start, strlen(start), &cSize );
		
	if( cSize.cx <= m_nItemWidth-5 )
	{	
		AddLine( start, type );		
	} 
	else
	{
		while( *end != '\0' )
		{
			end = ::CharNext(end);
			::GetTextExtentPoint32( hDC, start, end-start, &cSize );
			if( cSize.cx > m_nItemWidth-5 ) 
			{
				end = ::CharPrev(start,end);
				char tmpChar = *end;
				*end = '\0';
				AddLine( start, type );
				*end = tmpChar;
				start = end;
			}
		}
		if( *start != '\0' )
		{
			AddLine( start, type );
		}
	}
}

void CHUDChatMsg::AddLine( const char* lpszMessage, eChatMsgType type )
{
	if( (int32)m_ChatMsg.size() >= m_nMaxItemCount )
	{
		DeleteMessage();
	}

	LTIntPt pt = AddListItem();

	T_CHAT_MSGptr pChatMsg = new T_CHAT_MSG;
	
	pChatMsg->pText = new CSAD3DTextEx();
	pChatMsg->pText->CreateD3DText( m_nItemWidth, 16, (type==kMsgRadio)?true:false );
	pChatMsg->pText->SetTransparent(true);
	pChatMsg->pText->SetAlpha( (int)m_fStartAlpha );
	pChatMsg->fAlpha	= m_fStartAlpha;
	
	SetData( lpszMessage, type, pChatMsg->pText );
	
	m_ChatMsg.push_back( pChatMsg );

	m_nPosY = m_nBottomPosY - m_ChatMsg.size() * m_nItemHeight;
	SetPosition();
}

void CHUDChatMsg::DeleteMessage()
{
	if( m_ChatMsg.empty() ) return;
	
	CHUDList::DeleteMessage();
	
	CHAT_LIST_ITR itr = m_ChatMsg.begin();

	if (itr != m_ChatMsg.end())
	{
		T_CHAT_MSGptr pChatMsg = (T_CHAT_MSGptr)(*itr);
		SAFE_DELETE( pChatMsg->pText );
		SAFE_DELETE( pChatMsg );
		m_ChatMsg.erase(itr);		
	}
}

void CHUDChatMsg::CheckDeleteMessage( float fCurTime )
{
	if( m_HudList.empty() ) return;

	HUD_LIST_ITR itr1 = m_HudList.begin();	
	T_HUD_LISTptr pList = (T_HUD_LISTptr)(*itr1);
	
	CHAT_LIST_ITR itr2 = m_ChatMsg.begin();
	T_CHAT_MSGptr pChatMsg = (T_CHAT_MSGptr)(*itr2);

	if( pList->fLifeTime <= fCurTime && pChatMsg->fAlpha <= 0.0f )
	{
		DeleteMessage();
		return;
	}
}

void CHUDChatMsg::Update()
{
	CHUDList::Update();
}

void CHUDChatMsg::Render()
{
	if( m_ChatMsg.empty() ) return;

	float	fCurTime	= g_pLTClient->GetTime();
	T_CHAT_MSGptr pChatMsg;
	T_HUD_LISTptr pList;

	for( uint16 i=0 ; i<m_ChatMsg.size() ; ++i )
	{
		pChatMsg = (T_CHAT_MSGptr)m_ChatMsg[i];
		pList = (T_HUD_LISTptr)m_HudList[i];

		pChatMsg->fAlpha = (fCurTime - pList->fLifeTime) * 255.0f;
		if( pChatMsg->fAlpha >= 0.0f )
		{
			pChatMsg->fAlpha = m_fStartAlpha - pChatMsg->fAlpha;
			if( pChatMsg->fAlpha < 0.0f ) pChatMsg->fAlpha = 0.0f;
			
			pChatMsg->pText->SetAlpha( (int)pChatMsg->fAlpha );
		}

		pChatMsg->pText->Render();
	}
}

void CHUDChatMsg::SetPosition()
{
	T_CHAT_MSGptr pChatMsg;
	for( uint16 i=0 ; i<m_ChatMsg.size() ; ++i )
	{
		pChatMsg = (T_CHAT_MSGptr)m_ChatMsg[i];
		pChatMsg->pText->SetPosition( m_nPosX, m_nPosY+(i*m_nItemHeight) );
	}
}

void CHUDChatMsg::SetData( const char* lpszMessage, eChatMsgType type, CSAD3DTextEx* pText )
{
	switch( type )
	{
	case kMsgChat:		//Chat
		pText->SetTextColor( RGB(213,230,247), RGB(10,31,52) );
		break;

	case kMsgTeam:		//Team Chat
	case kMsgTeamGhost:
		pText->SetTextColor( RGB(243,216,152), RGB(10,31,52) );
		break;

	case kMsgRadio:		// Radio Msg
		pText->SetTextColor( RGB(246,193,98), RGB(0,0,0) );
		break;
		
	case kMsgGhost:	
		pText->SetTextColor( RGB(168,168,168), RGB(56,56,56) );
		break;
		
	case kMsgCheatConfirm:
		pText->SetTextColor( RGB(168,168,168), RGB(56,56,56) );
		break;
		
	case kMsgScmd:
		pText->SetTextColor( RGB(0,0,0), RGB(255,255,255) );
		break;

	case kMsgHelpMsg:
	case kMsgDefault:
	default:
		pText->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
		break;
	}
	pText->DrawD3DText( lpszMessage, strlen(lpszMessage), DT_LEFT, D3D_TEXT_OUTLINE );
}

void CHUDChatMsg::ScreenDimChanged()
{
	m_nBottomPosY = int(530.0f*g_pInterfaceResMgr->GetYRatio()) - 45;//////////////
	CHUDList::ScreenDimChanged();
}


CHUDHelpMsg::CHUDHelpMsg()
:CHUDChatMsg()
{
}

LTBOOL CHUDHelpMsg::Init( int32 nPosX, int32 nPosY, int32 nItemWidth, int32 nItemHeight, int32 nMaxItemCount, float fLifeTime )
{
	CHUDList::Init( nPosX, nPosY, nItemWidth, nItemHeight, nMaxItemCount, fLifeTime );

	m_fStartAlpha	= 180.0f;

	ScreenDimChanged();
	return LTTRUE;
}

void CHUDHelpMsg::Term()
{
	SAFE_DELETE( m_pTempText );
	CHUDChatMsg::Term();
}

void CHUDHelpMsg::AddMessage( const char* lpszMessage, int nFontSize, eChatMsgType type, float fLifeTime )
{
	SAFE_DELETE( m_pTempText );
	m_pTempText	= new CSAD3DText();
	m_pTempText->CreateD3DTextWithFontSize( m_nItemWidth, nFontSize+2, nFontSize, true );

	char* lpszToken;
	strcpy( m_szTokBuffer, lpszMessage );

	lpszToken = strtok( m_szTokBuffer, "\r\n" );
    while( lpszToken != NULL )
	{
		AddLine1( lpszToken, nFontSize, type, fLifeTime );
		lpszToken = strtok( NULL, "\r\n" );
	}	
}

void CHUDHelpMsg::AddLine1( const char* lpszMessage, int nFontSize, eChatMsgType type, float fLifeTime )
{
	SIZE cSize;
	static char szBuffer[1024];	
	HDC hDC = m_pTempText->GetDC();

	strcpy( szBuffer, lpszMessage );
	char* start = szBuffer;
	char* end	= szBuffer;

	if( !hDC )
	{
		SAFE_DELETE( m_pTempText );
		return;
	}

	::GetTextExtentPoint32( hDC, start, strlen(start), &cSize );
		
	if( cSize.cx <= m_nItemWidth-5 )
	{	
		AddLine2( start, nFontSize, type, fLifeTime );		
	} 
	else
	{
		while( *end != '\0' )
		{
			end = ::CharNext(end);
			::GetTextExtentPoint32( hDC, start, end-start, &cSize );
			if( cSize.cx > m_nItemWidth-5 ) 
			{
				end = ::CharPrev(start,end);
				char tmpChar = *end;
				*end = '\0';
				AddLine2( start, nFontSize, type, fLifeTime );
				*end = tmpChar;
				start = end;
			}
		}
		if( *start != '\0' )
		{
			AddLine2( start, nFontSize, type, fLifeTime );
		}
	}
}

void CHUDHelpMsg::AddLine2( const char* lpszMessage, int nFontSize, eChatMsgType type, float fLifeTime )
{
	if( (int32)m_ChatMsg.size() >= m_nMaxItemCount )
	{
		DeleteMessage();
	}

	LTIntPt pt = AddListItem( fLifeTime );

	T_CHAT_MSGptr pChatMsg = new T_CHAT_MSG;
	
	pChatMsg->pText = new CSAD3DTextEx();
	pChatMsg->pText->CreateD3DTextWithFontSize( m_nItemWidth, nFontSize+2, nFontSize, true );
	pChatMsg->pText->SetTransparent(true);
	pChatMsg->pText->SetAlpha( (int)m_fStartAlpha );
	pChatMsg->fAlpha	= m_fStartAlpha;
	pChatMsg->nFontSize	= nFontSize;
	
	SetData( lpszMessage, type, pChatMsg->pText );
	
	m_ChatMsg.push_back( pChatMsg );

	m_nPosY = m_nBottomPosY - m_ChatMsg.size() * m_nItemHeight;
	SetPosition();
}

void CHUDHelpMsg::SetData( const char* lpszMessage, eChatMsgType type, CSAD3DTextEx* pText )
{
	switch( type )
	{
	case kMsgMission:
		pText->SetTextColor( RGB(255,195,13), RGB(0,0,0) );
		break;
		
	case kMsgNotice:	//Server Notice
		pText->SetTextColor( RGB(56,183,248), RGB(10,31,52) );
		break;

	case kMsgHelpMsg:
		pText->SetTextColor( RGB(163,247,30), RGB(0,0,0) );
		break;

	case kMsgDefault:
	default:
		pText->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
		break;
	}
	pText->DrawD3DText( lpszMessage, strlen(lpszMessage), DT_CENTER, D3D_TEXT_OUTLINE );
}

void CHUDHelpMsg::SetPosition()
{
	if( m_ChatMsg.empty() ) return;
	T_CHAT_MSGptr pChatMsg;
	
	int nPosY = m_nBottomPosY;

	for( int16 i=m_ChatMsg.size()-1 ; i>=0 ; --i )
	{
		pChatMsg = (T_CHAT_MSGptr)m_ChatMsg[i];

		nPosY -= ( pChatMsg->nFontSize + 2 );

		pChatMsg->pText->SetPosition( m_nPosX, nPosY );
	}
}

void CHUDHelpMsg::ScreenDimChanged()
{
	CHUDList::ScreenDimChanged();

	m_nBottomPosY = int(150.0f*g_pInterfaceResMgr->GetYRatio());
	m_nPosX = ( g_pInterfaceResMgr->GetScreenWidth() - m_nItemWidth ) / 2 ;
}



CHUDKillMsg::CHUDKillMsg()
:CHUDList()
{	
	m_UpdateFlags	= kHUDAll;
	m_eLevel		= kHUDRenderGameOver;
}

LTBOOL CHUDKillMsg::Init( int32 nPosX, int32 nPosY, int32 nItemWidth, int32 nItemHeight, int32 nMaxItemCount, float fLifeTime )
{
	CHUDList::Init( nPosX, nPosY, nItemWidth, nItemHeight, nMaxItemCount, fLifeTime );
	return LTTRUE;
}

void CHUDKillMsg::Term()
{
	CHUDList::Term();
}

void CHUDKillMsg::Clear()
{
	CHUDList::Clear();

	if( !m_KillMsg.empty() )
	{
		for( uint16 i=0 ; i<m_KillMsg.size() ; ++i )
		{
			SAFE_DELETE( m_KillMsg[i] );
		}
		m_KillMsg.clear();
	}
}


void CHUDKillMsg::AddMessage( const char* lpszKiller, int32 nKillerTeam,
							  const char* lpszVictim, int32 nVictimTeam,
							  const char* lpszWeaponName, int32 nHitNode,
							  const char* lpszDeathType )
{	
	if( LTFALSE == g_pGameClientShell->IsMainWindowFocused() ) return;

	if( (int32)m_KillMsg.size() >= m_nMaxItemCount )
	{
		DeleteMessage();
	}

	LTIntPt pt = AddListItem();

	T_KILL_MSGptr pKill = new T_KILL_MSG;

	pKill->HeadShot.CreateD3DText( HEADSHOT_MSG_W, 16, true );
	pKill->HeadShot.SetTransparent(true);
		
	pKill->Killer.CreateD3DText( KILL_MSG_NICK_W, 16, true );
	pKill->Killer.SetTransparent(true);
	
	pKill->Victim.CreateD3DText( KILL_MSG_NICK_W, 16, true );
	pKill->Victim.SetTransparent(true);
	
	if( NODE_HEAD == nHitNode )
	{
		//								"[HEAD SHOT] "
		char* szHeadShotMsg = NF_GetMsg(SA_TEXT_KILL_BY_HEADSHOT);
		pKill->HeadShot.DrawD3DText( szHeadShotMsg, strlen(szHeadShotMsg), DT_LEFT, D3D_TEXT_SHADOW, false );
	}
	else
	{
		pKill->HeadShot.DrawD3DText( "", strlen("") );
	}

	SetData( lpszKiller, nKillerTeam, lpszVictim, nVictimTeam, lpszWeaponName, nHitNode, pKill, &pt, lpszDeathType );
	m_KillMsg.push_back( pKill );
}

void CHUDKillMsg::DeleteMessage()
{
	CHUDList::DeleteMessage();
	
	KILL_LIST_ITR itr = m_KillMsg.begin();

	if (itr != m_KillMsg.end())
	{
		T_KILL_MSGptr pKill = (T_KILL_MSGptr)(*itr);
		SAFE_DELETE( pKill );
		m_KillMsg.erase(itr);
	}

	SetPosition();
}

void CHUDKillMsg::Update()
{
	CHUDList::Update();
}

void CHUDKillMsg::Render()
{
	if( m_KillMsg.empty() ) return;

	T_KILL_MSGptr pKill;

	for( uint16 i=0 ; i<m_KillMsg.size() ; ++i )
	{
		pKill = (T_KILL_MSGptr)m_KillMsg[i];
		pKill->HeadShot.Render();
		pKill->Killer.Render();
		pKill->Victim.Render();

		if( pKill->hTexture )
		{
			g_pDrawPrim->SetTexture( pKill->hTexture );
			g_pDrawPrim->DrawPrim( &pKill->Poly );
		}
	}
}

void CHUDKillMsg::SetPosition()
{
	T_KILL_MSGptr pKill = NULL;

	for( uint16 i=0 ; i<m_KillMsg.size() ; ++i )
	{
		pKill = (T_KILL_MSGptr)m_KillMsg[i];
		pKill->HeadShot.SetPosition( (float)pKill->nBaseX,
									 (float)(m_nPosY+(i*m_nItemHeight)+(pKill->nTexH-12)*0.5f) );
		pKill->Killer.SetPosition( (float)pKill->nBaseX+pKill->nHeadShotLen, 
								   (float)(m_nPosY+(i*m_nItemHeight)+(pKill->nTexH-12)*0.5f) );
		pKill->Victim.SetPosition( (float)pKill->nBaseX+pKill->nHeadShotLen+pKill->nKillerLen+pKill->nTexW+KILL_MSG_TEMP_W+KILL_MSG_TEMP_W, 
								   (float)(m_nPosY+(i*m_nItemHeight)+(pKill->nTexH-12)*0.5f) );
		g_pDrawPrim->SetXYWH( &pKill->Poly,
						      (float)pKill->nBaseX+pKill->nHeadShotLen+pKill->nKillerLen+KILL_MSG_TEMP_W,
							  (float)(m_nPosY+(i*m_nItemHeight)),
							  (float)pKill->nTexW, (float)pKill->nTexH );
	}
}

void CHUDKillMsg::SetData( const char* lpszKiller, int32 nKillerTeam,
						   const char* lpszVictim, int32 nVictimTeam,
						   const char* lpszWeaponName, int32 nHitNode,
						   T_KILL_MSGptr pKill, LTIntPt* pPt,
						   const char* lpszDeathType )
{
	//Weapon Texture
	if( lpszWeaponName )
	{	
		pKill->hTexture	= g_pInterfaceResMgr->GetTexture( lpszWeaponName );	
		g_pTexInterface->GetTextureDims( pKill->hTexture,
										 pKill->nTexW,
										 pKill->nTexH );
	}
	else
	{
		pKill->hTexture = LTNULL;
		pKill->nTexW	= 0;
		pKill->nTexH	= 12;
	}
	
	//Head Shot
	pKill->HeadShot.SetTextColor( RGB(255,0,0), RGB(0,0,0) );
	
	//Killer Nick
	if( nKillerTeam == TEAM_A )	// RED
	{
		pKill->Killer.SetTextColor( A_TEAM_COLOR, RGB(0,0,0) );
	}
	else if( nKillerTeam == TEAM_B )	// BLUE
	{
		pKill->Killer.SetTextColor( B_TEAM_COLOR, RGB(0,0,0) );		
	}
	else
	{
		pKill->Killer.SetTextColor( RGB(255,255,255), RGB(0,0,0) );		
	}
		
	pKill->Killer.DrawD3DText( lpszKiller, strlen(lpszKiller), DT_LEFT, D3D_TEXT_SHADOW, true );
	pKill->nKillerLen	= KILL_MSG_NICK_W < pKill->Killer.GetTextLength() ? KILL_MSG_NICK_W : pKill->Killer.GetTextLength();

	//Poly
	SetupQuadUVs( pKill->Poly, pKill->hTexture, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &pKill->Poly, 0xFFFFFFFF );
	
	//Victim Nick
	if( nVictimTeam == TEAM_A )
	{
		pKill->Victim.SetTextColor( A_TEAM_COLOR, RGB(0,0,0) );
	}
	else if( nVictimTeam == TEAM_B )
	{
		pKill->Victim.SetTextColor( B_TEAM_COLOR, RGB(0,0,0) );
	}
	else
	{
		pKill->Victim.SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	}
	
	if( lpszDeathType ) 
	{
		pKill->Victim.DrawD3DText( lpszDeathType, strlen(lpszDeathType), DT_LEFT, D3D_TEXT_SHADOW, true );
	}
	else if( strcmp( lpszKiller, lpszVictim ) == 0 )
	{
		//"ÀÚ»ì"
		pKill->Victim.DrawD3DText( SA_TEXT_SUICIDE, DT_LEFT, D3D_TEXT_SHADOW, true );
	}
	else
	{
		pKill->Victim.DrawD3DText( lpszVictim, strlen(lpszVictim), DT_LEFT, D3D_TEXT_SHADOW, true );
	}

	int nTotalLen = 10;
	
	//Set Position
	pKill->nHeadShotLen	= pKill->HeadShot.GetTextLength();
	pKill->nKillerLen	= KILL_MSG_NICK_W < pKill->Killer.GetTextLength() ? KILL_MSG_NICK_W : pKill->Killer.GetTextLength();
	pKill->nVictimLen	= KILL_MSG_NICK_W < pKill->Victim.GetTextLength() ? KILL_MSG_NICK_W : pKill->Victim.GetTextLength();
	nTotalLen	+= pKill->nHeadShotLen + pKill->nKillerLen + pKill->nTexW + pKill->nVictimLen + KILL_MSG_TEMP_W + KILL_MSG_TEMP_W;
	pKill->nBaseX = g_pInterfaceResMgr->GetScreenWidth() - nTotalLen;

	pKill->HeadShot.SetPosition( (float)pKill->nBaseX,
							   (float)pPt->y+(pKill->nTexH-12)*0.5f );

	pKill->Killer.SetPosition( (float)pKill->nBaseX+pKill->nHeadShotLen,
							   (float)pPt->y+(pKill->nTexH-12)*0.5f );
	g_pDrawPrim->SetXYWH( &pKill->Poly,
						  (float)pKill->nBaseX+pKill->nHeadShotLen+pKill->nKillerLen+KILL_MSG_TEMP_W,
						  (float)pPt->y,
						  (float)pKill->nTexW, (float)pKill->nTexH );
	pKill->Victim.SetPosition( (float)pKill->nBaseX+pKill->nHeadShotLen+pKill->nKillerLen+pKill->nTexW+KILL_MSG_TEMP_W+KILL_MSG_TEMP_W,
							   (float)pPt->y+(pKill->nTexH-12)*0.5f );
}

void CHUDKillMsg::ScreenDimChanged()
{
}





CHUDNickList::CHUDNickList()
{
	m_UpdateFlags	= kHUDNone;
	m_eLevel		= kHUDRenderGameOver; //kHUDRenderDead
}

LTBOOL CHUDNickList::Init()
{
	ScreenDimChanged();
	return LTTRUE;
}

void CHUDNickList::Term()
{
	Clear();
}

void CHUDNickList::AddNick( HOBJECT hObj, const char* lpszNick, int nTeamID )
{
	T_CHAR_NICKptr pCharNick = NULL;

	for( uint16 i=0 ; i<m_NickList.size() ; ++i )
	{
		pCharNick = (T_CHAR_NICKptr)m_NickList[i];

		if( pCharNick && pCharNick->pNick && pCharNick->hObj == hObj )
		{
			pCharNick->pNick->DrawD3DText( lpszNick, strlen(lpszNick), DT_LEFT, D3D_TEXT_SHADOW );
			pCharNick->fTextW		= (float)pCharNick->pNick->GetTextLength();
			pCharNick->fTextHalfW	= pCharNick->fTextW*0.5f;
			return;			
		}		
	}

	pCharNick			= new T_CHAR_NICK;
	pCharNick->hObj		= hObj;
	pCharNick->bRender	= false;

	pCharNick->pNick	= new CSAD3DTextEx();
	pCharNick->pNick->CreateD3DText( 256, 16, true );
	pCharNick->pNick->SetTransparent(true);
#ifdef GAME_SHOW
	if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
	{
		pCharNick->pNick->SetAlpha( 255 );
		if( TEAM_A == nTeamID )
		{
			pCharNick->pNick->SetTextColor( A_TEAM_COLOR, RGB(0,0,0) );
		}
		else
		{
			pCharNick->pNick->SetTextColor( B_TEAM_COLOR, RGB(0,0,0) );
		}
	}
	else
	{
		pCharNick->pNick->SetAlpha( 220 );
		pCharNick->pNick->SetTextColor( B_TEAM_COLOR, RGB(0,0,0) );	
	}
#else
	pCharNick->pNick->SetAlpha( 220 );
	pCharNick->pNick->SetTextColor( B_TEAM_COLOR, RGB(0,0,0) );	
#endif
	pCharNick->pNick->DrawD3DText( lpszNick, strlen(lpszNick), DT_LEFT, D3D_TEXT_SHADOW );
	pCharNick->fTextW		= (float)pCharNick->pNick->GetTextLength();
	pCharNick->fTextHalfW	= pCharNick->fTextW*0.5f;

	pCharNick->pRadio	= new CSAD3DTextEx();
	pCharNick->pRadio->CreateD3DText( 256, 16, true );
	pCharNick->pRadio->SetAlpha( 220 );
	pCharNick->pRadio->SetTextColor( RGB(144,168,224), RGB(0,0,0) );
	pCharNick->pRadio->DrawD3DText( "", 0 );
	pCharNick->bRenderRadio	= LTFALSE;
	
	m_NickList.push_back(pCharNick);
}

void CHUDNickList::RemoveNick( HOBJECT hObj )
{
	if( m_NickList.empty() ) return;

	T_CHAR_NICKptr pCharNick = NULL;

	NICK_LIST_ITR iter = m_NickList.begin();
	
	while( iter != m_NickList.end() && (*iter)->hObj != hObj )
		iter++;

	if (iter != m_NickList.end())
	{
		pCharNick = (T_CHAR_NICKptr)(*iter);
		SAFE_DELETE( pCharNick->pNick );
		SAFE_DELETE( pCharNick->pRadio );
		SAFE_DELETE( pCharNick );

		m_NickList.erase(iter);
	}
}

void CHUDNickList::Clear()
{
	if( m_NickList.empty() ) return;

	T_CHAR_NICKptr pCharNick = NULL;

	for( uint16 i=0 ; i<m_NickList.size() ; ++i )
	{
		pCharNick = (T_CHAR_NICKptr)m_NickList[i];

		if( pCharNick && pCharNick->pNick )
		{
			SAFE_DELETE( pCharNick->pNick );
			SAFE_DELETE( pCharNick->pRadio );
			SAFE_DELETE( pCharNick );
		}		
	}

	m_NickList.clear();
}

void CHUDNickList::SetRadioMsg( HOBJECT hObj, const char* lpszMsg )
{
	if( LTFALSE == IsVisible() || m_NickList.empty() ) return;
	
	T_CHAR_NICKptr pCharNick = NULL;

	for( uint16 i=0 ; i<m_NickList.size() ; ++i )
	{
		pCharNick = (T_CHAR_NICKptr)m_NickList[i];

		if( pCharNick && pCharNick->hObj == hObj && pCharNick->pRadio )
		{
			pCharNick->pRadio->DrawD3DText( lpszMsg, strlen(lpszMsg), DT_LEFT, D3D_TEXT_SHADOW );
			pCharNick->fRadioTextW		= (float)pCharNick->pRadio->GetTextLength() + 10.0f;
			pCharNick->bRenderRadio		= LTTRUE;
			pCharNick->fRadioLifeTime	= g_pLTClient->GetTime() + 2.5f;
		}		
	}
}

void CHUDNickList::Update( HOBJECT hObj, LTVector* pPos )
{
	if( LTFALSE == IsVisible() || m_NickList.empty() ) return;

	T_CHAR_NICKptr pCharNick = NULL;

	for( uint16 i=0 ; i<m_NickList.size() ; ++i )
	{
		pCharNick = (T_CHAR_NICKptr)m_NickList[i];

		if( pCharNick && pCharNick->pNick && pCharNick->hObj == hObj )
		{
			if( pPos->z < 0 )
			{
				pPos->x = (float)g_pInterfaceResMgr->GetScreenWidth()-pPos->x;
				pPos->y = m_fMaxY;
			}

			pPos->x -= pCharNick->fTextHalfW;
			if( pPos->x < m_fMinX )
			{
				pPos->x = m_fMinX;
			}
			else if( pPos->x+pCharNick->fTextW > m_fMaxX )
			{
				pPos->x = m_fMaxX-pCharNick->fTextW;
			}

			if( pPos->y < m_fMinY )
			{
				pPos->y = m_fMinY;
			}
			else if( pPos->y+16.0f > m_fMaxY )
			{
				pPos->y = m_fMaxY-16.0f;
			}

			pCharNick->pNick->SetPosition( pPos->x, pPos->y );
			pCharNick->bRender = true;

#ifdef GAME_SHOW
			if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
			{
				if( pPos->x > g_pInterfaceResMgr->GetScreenWidth()-pCharNick->fRadioTextW+256 )
				{
					pPos->x = g_pInterfaceResMgr->GetScreenWidth()-pCharNick->fRadioTextW+256;
				}
			}
			else
			{
				if( pPos->x > g_pInterfaceResMgr->GetScreenWidth()-pCharNick->fRadioTextW )
				{
					pPos->x = g_pInterfaceResMgr->GetScreenWidth()-pCharNick->fRadioTextW;
				}
			}
			pCharNick->pRadio->SetPosition( pPos->x, pPos->y+16 );
#else
			if( pPos->x > g_pInterfaceResMgr->GetScreenWidth()-pCharNick->fRadioTextW )
			{
				pPos->x = g_pInterfaceResMgr->GetScreenWidth()-pCharNick->fRadioTextW;
			}
			pCharNick->pRadio->SetPosition( pPos->x, pPos->y+16 );			
#endif
			return;
		}
	}
}

void CHUDNickList::Render()
{
	if( LTFALSE == IsVisible() || m_NickList.empty() ) return;

	T_CHAR_NICKptr pCharNick = NULL;

	for( uint16 i=0 ; i<m_NickList.size() ; ++i )
	{
		pCharNick = (T_CHAR_NICKptr)m_NickList[i];

		if( pCharNick && pCharNick->pNick && pCharNick->bRender )
		{
			pCharNick->pNick->Render();
			if( pCharNick->bRenderRadio )
			{
				pCharNick->pRadio->Render();
				if( pCharNick->fRadioLifeTime < g_pLTClient->GetTime() )
				{
					pCharNick->bRenderRadio	= LTFALSE;
				}
			}
		}

		pCharNick->bRender	= false;
	}
}

void CHUDNickList::ScreenDimChanged()
{
#ifdef GAME_SHOW
	if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
	{
		m_fMinX = -500.0f;
		m_fMaxX = (float)g_pInterfaceResMgr->GetScreenWidth()+500.0f;

		m_fMinY = -30.0f;
		m_fMaxY = (float)g_pInterfaceResMgr->GetScreenHeight()+30.0f;
	}
	else
	{
		m_fMinX = 10.0f;
		m_fMaxX = (float)g_pInterfaceResMgr->GetScreenWidth()-10.0f;

		m_fMinY = 10.0f;
		m_fMaxY = (float)g_pInterfaceResMgr->GetScreenHeight()-60.0f;
	}
#else
	m_fMinX = 10.0f;
	m_fMaxX = (float)g_pInterfaceResMgr->GetScreenWidth()-10.0f;

	m_fMinY = 10.0f;
	m_fMaxY = (float)g_pInterfaceResMgr->GetScreenHeight()-60.0f;
#endif
}




/*
CHUDPickupMsg::CHUDPickupMsg()
:CHUDList()
{
	m_UpdateFlags	= kHUDAll;
	m_eLevel		= kHUDRenderGameOver;
}

LTBOOL CHUDPickupMsg::Init( int32 nPosX, int32 nPosY, int32 nItemWidth, int32 nItemHeight, int32 nMaxItemCount, float fLifeTime )
{
	CHUDList::Init( nPosX, nPosY, nItemWidth, nItemHeight, nMaxItemCount, fLifeTime );
	return LTTRUE;
}

void CHUDPickupMsg::Term()
{
	CHUDList::Term();
}

void CHUDPickupMsg::Clear()
{
	CHUDList::Clear();

	if( !m_PickupMsg.empty() )
	{
		for( uint16 i=0 ; i<m_PickupMsg.size() ; ++i )
		{
			SAFE_DELETE( m_PickupMsg[i] );
		}
		m_PickupMsg.clear();
	}
}

void CHUDPickupMsg::AddMessage( const char* lpszMessage, const char* lpszIconName )
{	
	if( (int32)m_PickupMsg.size() >= m_nMaxItemCount )
	{
		DeleteMessage();
	}

	LTIntPt pt = AddListItem();

	T_PICKUP_MSGptr pPickup = new T_PICKUP_MSG;

	pPickup->Msg.CreateD3DText( 64, 16, true );
	pPickup->Msg.SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	pPickup->Msg.SetTransparent(true);
		
	SetData( lpszMessage, lpszIconName, pPickup, &pt );

	m_PickupMsg.push_back( pPickup );
}

void CHUDPickupMsg::DeleteMessage()
{
	CHUDList::DeleteMessage();

	PICKUP_LIST_ITR itr = m_PickupMsg.begin();
	T_PICKUP_MSGptr pPickup = (T_PICKUP_MSGptr)(*itr);

	if (itr != m_PickupMsg.end())
	{
		m_PickupMsg.erase(itr);
	}

	SetPosition();

	SAFE_DELETE( pPickup );
}

void CHUDPickupMsg::Update()
{
	CHUDList::Update();
}

void CHUDPickupMsg::Render()
{
	if( m_PickupMsg.empty() ) return;

	T_PICKUP_MSGptr pPickup;

	for( uint16 i=0 ; i<m_PickupMsg.size() ; ++i )
	{
		pPickup = (T_PICKUP_MSGptr)m_PickupMsg[i];
		pPickup->Msg.Render();
		pPickup->Msg.Render();

		if( pPickup->hTexture )
		{
			g_pDrawPrim->SetTexture( pPickup->hTexture );
			g_pDrawPrim->DrawPrim( &pPickup->Poly );
		}
	}
}

void CHUDPickupMsg::SetPosition()
{
	T_PICKUP_MSGptr pPickup;

	for( uint16 i=0 ; i<m_PickupMsg.size() ; ++i )
	{
		pPickup = (T_PICKUP_MSGptr)m_PickupMsg[i];
		pPickup->Msg.SetPosition( (float)m_nPosX+pPickup->nTexW, 
								  (float)(m_nPosY+(i*m_nItemHeight)+(pPickup->nTexH-12)*0.5f) );
		g_pDrawPrim->SetXYWH( &pPickup->Poly,
						      (float)m_nPosX,
							  (float)(m_nPosY+(i*m_nItemHeight)),
							  (float)pPickup->nTexW, (float)pPickup->nTexH );
	}
}

void CHUDPickupMsg::SetData( const char* lpszMessage, const char* lpszIconName, T_PICKUP_MSGptr pPickup, LTIntPt* pPt )
{
	
	pPickup->hTexture	= g_pInterfaceResMgr->GetTexture( lpszIconName );
	
	g_pTexInterface->GetTextureDims( pPickup->hTexture,
									 pPickup->nTexW,
									 pPickup->nTexH );
	SetupQuadUVs( pPickup->Poly, pPickup->hTexture, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &pPickup->Poly, 0xFFFFFFFF );
	g_pDrawPrim->SetXYWH( &pPickup->Poly,
						  (float)pPt->x,
						  (float)pPt->y,
						  (float)pPickup->nTexW, (float)pPickup->nTexH );
	
	pPickup->Msg.SetPosition( (float)pPt->x+pPickup->nTexW,
							   (float)pPt->y+(pPickup->nTexH-12)*0.5f );
	pPickup->Msg.DrawD3DText( lpszMessage, strlen(lpszMessage), DT_LEFT, D3D_TEXT_OUTLINE );
}

void CHUDPickupMsg::ScreenDimChanged()
{
	CHUDList::ScreenDimChanged();
}
*/