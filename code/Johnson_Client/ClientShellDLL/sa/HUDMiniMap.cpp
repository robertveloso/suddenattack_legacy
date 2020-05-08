#include "stdafx.h"
#include "SAHUDMgr.h"
#include "PlayerMgr.h"

#include "GameClientShell.h"
#include "CharacterFX.h"
#include "RadarTypeMgr.h"

//******************************************************************************************
//**
//** HUD MiniMap display
//**
//******************************************************************************************
namespace
{
	#define MINI_MAP_SIZE	512.0f

	#define RADAR_PLAYER_ALIVE_TYPE		"PlayerAlive"
	#define RADAR_PLAYER_DEAD_TYPE		"PlayerDead"
	#define RADAR_PLAYER_TALK_TYPE		"PlayerTalk"
	#define RADAR_PLAYER_BOMB_TYPE		"PlayerBomb"
}

CHUDMiniMap::CHUDMiniMap()
{
	m_UpdateFlags = kHUDFrame;

	m_bDraw = LTFALSE;
	m_eLevel = kHUDRenderGameOver;

	m_hTexMiniMap		= LTNULL;
	m_nMapSize_X		= 0;
	m_nMapSize_Y		= 0;
	m_fMapScale_X		= 0.0f;
	m_fMapScale_Y		= 0.0f;
}

LTBOOL CHUDMiniMap::Init()
{
	ScreenDimChanged();
	
	return LTTRUE;
}

void CHUDMiniMap::SetMapTexture(HTEXTURE hTex)
{
	if( !hTex ) return;

	m_hTexMiniMap = hTex;
	SetupQuadUVs( m_hPolyMiniMap, m_hTexMiniMap,  0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_hPolyMiniMap, 0x66FFFFFF );
	
	ScreenDimChanged();
}

void CHUDMiniMap::SetMapSize(int nx, int ny)
{
	m_nMapSize_X	= nx;
	m_nMapSize_Y	= ny;
	m_fMapScale_X	= MINI_MAP_SIZE / (float)nx;
	m_fMapScale_Y	= MINI_MAP_SIZE / (float)ny;
}

void CHUDMiniMap::Term()
{
}

void CHUDMiniMap::Render()
{
	if (!m_bDraw) return;

	// Map Render	
	g_pDrawPrim->SetTexture( m_hTexMiniMap );
	g_pDrawPrim->DrawPrim( &m_hPolyMiniMap );
	
	// Player Render
	MiniMapPlayerList::iterator iter = m_Players.begin();
	while(iter != m_Players.end())
	{
		g_pDrawPrim->SetTexture( (*iter)->hTex );
		g_pDrawPrim->DrawPrim( &(*iter)->hPoly );
		iter++;
	}
}

void CHUDMiniMap::Update()
{
	if (!m_bDraw) return;

	MiniMapPlayerList::iterator iter;
	for( iter = m_Players.begin(); iter != m_Players.end(); ++iter )
	{
		// Player State Change
	}
}


void CHUDMiniMap::ScreenDimChanged()
{
}

void CHUDMiniMap::MiniMapReset()
{
	for (uint16 i = 0; i < m_Players.size(); i++)
	{
		debug_delete(m_Players[i]);
	}
	m_Players.clear();
}

void CHUDMiniMap::AddPlayer( HOBJECT hObj, uint32 nId )
{
	if( !hObj ) return;

	T_PLAYER_INFOptr pPI = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByID( nId );

	if( !pPI || g_GameDoc.GetTeam() != pPI->nTeamID ) return;

	bool bDead = false;
	CCharacterFX *pCharacter = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(hObj);
	if(pCharacter && pCharacter->IsRadarPlayerDead())
	{
		bDead = true;
	}

	MiniMapPlayerList::iterator iter = m_Players.begin();
	while(iter != m_Players.end() && ((*iter)->hObj != hObj) )
	{
		iter++;
	}

	PLAYER_IN_MINIMAP* pPlayer	= LTNULL;	
	RADARTYPE *pType			= LTNULL;
	if( bDead )
	{
		pType = g_pRadarTypeMgr->GetRadarType( RADAR_PLAYER_DEAD_TYPE );
	}
	else
	{
		pType = g_pRadarTypeMgr->GetRadarType( RADAR_PLAYER_ALIVE_TYPE );
	}

	if ( !pType ) return;

	if( iter == m_Players.end() )
	{
		pPlayer = debug_new(PLAYER_IN_MINIMAP);
		m_Players.push_back(pPlayer);
	}
	else
	{
		pPlayer = (*iter);
	}

	pPlayer->nID	= nId;
	pPlayer->hObj	= hObj;
	pPlayer->bDead	= bDead;
	pPlayer->hTex	= g_pInterfaceResMgr->GetTexture( pType->szIcon );
}

void CHUDMiniMap::RemovePlayer( HOBJECT hObj )
{
	if (!hObj) return;

	MiniMapPlayerList::iterator iter = m_Players.begin();
	while (iter != m_Players.end() &&  ( (*iter)->hObj != hObj ) )
	{
		iter++;
	}

	if( iter != m_Players.end() )
	{
		debug_delete(*iter);
		m_Players.erase(iter);
	}
}

void CHUDMiniMap::SetPlayerState( HOBJECT hObj, bool bDead )
{
	MiniMapPlayerList::iterator iter = m_Players.begin();
	while( iter != m_Players.end() )
	{
		if( (*iter)->hObj == hObj )
		{
			(*iter)->bDead	= bDead;
			RADARTYPE *pType			= LTNULL;
			if( bDead )
			{
				pType = g_pRadarTypeMgr->GetRadarType( RADAR_PLAYER_DEAD_TYPE );
			}
			else
			{
				pType = g_pRadarTypeMgr->GetRadarType( RADAR_PLAYER_ALIVE_TYPE );
			}
			if( pType )
			{
				(*iter)->hTex	= g_pInterfaceResMgr->GetTexture( pType->szIcon );
			}			
			return;
		}

		iter++;
	}
}