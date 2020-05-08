// ----------------------------------------------------------------------- //
//
// MODULE  : HUDRadio.cpp
//
// PURPOSE : Implementation of CHUDRadio to display messages
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //
#include "stdafx.h"
#include "HUDMgr.h"
#include "HUDRadio.h"
#include "InterfaceMgr.h"
#include "ClientRes.h"
#include "ClientMultiplayerMgr.h"
#include "GameClientShell.h"

#define MAX_RADIO_DELAY	1.0f
#define MAX_RADIO_TEXT	256
#define MAX_PAGE_RADIO_COUNT	6
#define MAX_USER_RADIO_COUNT	MAX_PAGE_RADIO_COUNT * 3

static char	g_szRadioArray[][MAX_RADIO_TEXT] =
{
	//HotKey 1st
	"Go Go",
	"Covering fire",
	"Fall back",
	"Need backup",
	"Hold position",
	"Fallow me",

	//Hotkey 2nd
	"Enemy spotted",
	"Taking fire",
	"Enemy down",
	"Area clear",
	"Enemy sniper down",
	"Moving target",

	//Hotkey 3rd
	"Roger",
	"Ok! Let's move out",
	"Negative",
	"Nice shot",
	"Bomb site entry",
	"Guard bomb site",

	//Etc
	"Fire in the hole",
	
	/*
	//HotKey 1st
	"Go Go(가라)",
	"Covering fire(엄호하라)",
	"Fall back(후퇴하라)",
	"Need backup(지원요망)",
	"Hold position(위치고정)",
	"Fallow me(동행바람)",

	//Hotkey 2nd
	"Enemy spotted(적발견)",
	"Taking fire(도움요청)",
	"Enemy down(적제거)",
	"Area clear(구역확보)",
	"Enemy sniper(저격병발견)",
	"Moving target(목표지점이동)",

	//Hotkey 3rd
	"Roger(승낙)",
	"Ok! Let's move out(진격하자)",
	"Negative(거절)",
	"Nice shot(훌륭해)",
	"Bomb site entry(폭파지점도착)",
	"Guard bomb site(폭파지점경계)",

	//Etc
	"Fire in the hole(수류탄투척)",*/
};

CHUDRadio::CHUDRadio()
{
	m_eLevel		= kHUDRenderGameOver;
	m_UpdateFlags	= kHUDNone;
	m_bVisible		= false;
	m_pRadioList	= LTNULL;
	m_fPlayTauntSoundTime	= 0.0f;
	m_bCurRadioKeyDown = LTFALSE;
}
	

LTBOOL CHUDRadio::Init()
{
	int	nSet		= 0;
	int	nMaxTextLen	= 0;
	m_nFirstIndex	= 0;
	m_nMaxCount		= m_nRadioCount = sizeof(g_szRadioArray) / MAX_RADIO_TEXT;

	if( m_nMaxCount	> MAX_USER_RADIO_COUNT ) m_nMaxCount = MAX_USER_RADIO_COUNT;
	
	char	szTemp[MAX_RADIO_TEXT];

	m_pRadioList	= new CSAD3DTextEx[m_nMaxCount];

	for( int i=0 ; i<m_nMaxCount ; ++i )
	{
		sprintf( szTemp, "%d. %s", (i%MAX_PAGE_RADIO_COUNT)+1, g_szRadioArray[i] );
		m_pRadioList[i].CreateD3DText( 512, 16, true );
		m_pRadioList[i].SetTransparent( true );
		m_pRadioList[i].SetTextColor( RGB(246,193,98), RGB(0,0,0) );
		m_pRadioList[i].SetAlpha(180);
		m_pRadioList[i].DrawD3DText( szTemp,
									 strlen(szTemp),
									 DT_LEFT,
									 D3D_TEXT_OUTLINE,
									 false );
		nMaxTextLen = LTMAX( nMaxTextLen, m_pRadioList[i].GetTextLength() );

		if( (i%MAX_PAGE_RADIO_COUNT) == (MAX_PAGE_RADIO_COUNT-1) )
		{
			m_nMaxTextLen[nSet] = nMaxTextLen;
			++nSet;
			nMaxTextLen	= 0;
		}
	}
	this->ScreenDimChanged();
	return LTTRUE;
}

void CHUDRadio::Term()
{
	if( m_pRadioList )
	{
		delete [] m_pRadioList;
		m_pRadioList = LTNULL;
	}
}

void CHUDRadio::Render()
{
	if( !m_bVisible || m_nMaxCount <=0 || LTNULL == m_pRadioList ) return;

	for( int i=m_nFirstIndex ; i<m_nFirstIndex+MAX_PAGE_RADIO_COUNT ; ++i )
	{
		m_pRadioList[i].Render();
	}
}

void CHUDRadio::Show( int nFirstIndex )
{
	if( nFirstIndex == RADIO_HIDE ) 
	{
		m_bVisible = false;
		m_nFirstIndex = 0;
	}
	else if( m_bVisible )
	{
		if( nFirstIndex == m_nFirstIndex )
		{
			m_bVisible = false;
			m_nFirstIndex = 0;
		}
		else
		{
			if( g_pLTClient->GetTime() < m_fPlayTauntSoundTime ) return;
			m_nFirstIndex = nFirstIndex;
		}
	}
	else
	{
		if( g_pLTClient->GetTime() < m_fPlayTauntSoundTime ) return;
		m_nFirstIndex	= nFirstIndex;
		m_bVisible = true;
	}
}

void CHUDRadio::Choose(uint8 nChoice)
{
	nChoice += m_nFirstIndex;
	if( nChoice < MAX_USER_RADIO_COUNT && !m_bVisible ) return;
	
	m_bVisible = false;
	m_nFirstIndex = 0;

	uint32 nLocalID;
	g_pLTClient->GetLocalClientID(&nLocalID);
	g_pInterfaceMgr->DoPlayerRadio( nLocalID, nChoice );

	m_fPlayTauntSoundTime	= g_pLTClient->GetTime()+MAX_RADIO_DELAY;
}

void CHUDRadio::ScreenDimChanged()
{
	if( m_nMaxCount <=0 || LTNULL == m_pRadioList ) return;

	int nSet = 0;
	
	m_BasePos.y	= g_pInterfaceResMgr->GetScreenHeight()-(MAX_PAGE_RADIO_COUNT*20)-120;

	for( int i=0 ; i<m_nMaxCount ; ++i )
	{
		if( (i%MAX_PAGE_RADIO_COUNT) == 0 )
		{
			m_BasePos.x	= g_pInterfaceResMgr->GetScreenWidth()-m_nMaxTextLen[nSet++]-30;
		}
		
		m_pRadioList[i].SetPosition( m_BasePos.x, m_BasePos.y+( (i%MAX_PAGE_RADIO_COUNT)*20) );		
	}
}

const char* CHUDRadio::GetRadioString( uint8 nIndex )
{
	if( 0 > nIndex || nIndex > m_nRadioCount  ) return "";
	return g_szRadioArray[nIndex];
}