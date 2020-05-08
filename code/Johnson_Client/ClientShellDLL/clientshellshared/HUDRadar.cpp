// ----------------------------------------------------------------------- //
//
// MODULE  : HUDRadar.cpp
//
// PURPOSE : HUDItem to display radar
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

//
// Includes...
//

	#include "stdafx.h"
	#include "HUDMgr.h"
	#include "HUDRadar.h"
	#include "GameClientShell.h"
	#include "CharacterFX.h"
	#include "RadarTypeMgr.h"

// LTPoly_GT4	teamPoly;
uint32 nTeamColors[2][3] = {	{argbBlack,argbBlack,argbBlack},
								{argbBlack,argbBlack,argbBlack} };


float	fFlashSpeed = 0.25f;
uint32	nLiveColor = 0xFFFFFF00;
uint32	nDeadColor = 0xFF808080;
uint32	nTalkColor = 0xFFFFFFFF;
float	fTotalFlashTime = 2.0f;

typedef RADAROBJECT* LPRADAROBJECT;
class RadarObjectLesser
{
public:
	
	bool operator()(const LPRADAROBJECT & x, const LPRADAROBJECT & y) const
	{
		return (x->m_nDrawOrder < y->m_nDrawOrder );
	}
};


#define RADAR_PLAYER_ALIVE_TYPE		"PlayerAlive"
#define RADAR_PLAYER_DEAD_TYPE		"PlayerDead"
#define RADAR_PLAYER_TALK_TYPE		"PlayerTalk"
#define RADAR_PLAYER_ENEMY_TYPE		"PlayerEnemy"
#define RADAR_MISSION_POINT			"MissionPoint_"
#define RADAR_MISSION_POINT_A		"MissionPoint_A"
#define RADAR_MISSION_POINT_B		"MissionPoint_B"
#define RADAR_MISSION_POINT_C		"MissionPoint_C"
#define RADAR_MISSION_POINT_D		"MissionPoint_D"



// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::CHUDRadar
//
//  PURPOSE:	Constructor...
//
// ----------------------------------------------------------------------- //

CHUDRadar::CHUDRadar()
:	CHUDItem		(),
	m_BasePos		( 0, 0 ),
	m_nBaseSize		( 0 ),
	m_bDraw			( false ),
	m_nMaxShowDist	( 0 )
{
	m_UpdateFlags	= kHUDFrame;
	m_eLevel		= kHUDRenderGameOver;
	m_fLastRotation	= 0.0f;
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::CHUDRadar
//
//  PURPOSE:	Destructor...
//
// ----------------------------------------------------------------------- //

CHUDRadar::~CHUDRadar()
{	
	Term();
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::Term
//
//  PURPOSE:	Release the list of radar objects...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::Term()
{
	Reset();
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::Init
//
//  PURPOSE:	Setup the icon and poly we are going to draw...
//
// ----------------------------------------------------------------------- //

LTBOOL CHUDRadar::Init()
{
	UpdateLayout();

	//[MURSUM] 레이다 타입 이미지를 미리 로딩
	int nRadarTypeCount = g_pRadarTypeMgr->GetNumRadarTypes();
	RADARTYPE *pType;
	for( int i=0 ; i<nRadarTypeCount ; ++i )
	{
		pType = g_pRadarTypeMgr->GetRadarType(i);
		if( pType && pType->szIcon )
		{
			g_pInterfaceResMgr->GetTexture( pType->szIcon );
		}
	}

//	g_pDrawPrim->SetRGBA( &teamPoly, SET_ARGB(0x7F,0xFF,0xFF,0xFF));

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::Render
//
//  PURPOSE:	Draw the radar...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::Render()
{
	if( !m_bDraw )
	{
		return;
	}

	RADAROBJECT *pRadarObj = LTNULL;
	RadarObjectSortList::iterator iter;

	uint8 nCount = 0;
	for( iter = m_ROSortList.begin(); iter != m_ROSortList.end(); ++iter )
	{	
		pRadarObj = (*iter);

		// Check if the radar object has team information in it.  Only
		// show on same team's radar.
		
		// Check if the radar is for an opposing player, show them on radar.
#ifdef GAME_SHOW
		if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
		{
		}
		else
		{
			if( pRadarObj->m_nVisibleTeamId != INVALID_TEAM &&
				g_GameDoc.GetTeam() != pRadarObj->m_nVisibleTeamId ) continue;
		}
#else
		if( pRadarObj->m_nVisibleTeamId != INVALID_TEAM &&
			g_GameDoc.GetTeam() != pRadarObj->m_nVisibleTeamId ) continue;
#endif

		
		if( pRadarObj->m_bDraw )
		{
			g_pDrawPrim->SetTexture( pRadarObj->m_hTex );
			g_pDrawPrim->DrawPrim( &pRadarObj->m_Poly );

			if( pRadarObj->m_bFlashOn )
			{
				g_pDrawPrim->SetTexture( pRadarObj->m_hTexRing );
				g_pDrawPrim->DrawPrim( &pRadarObj->m_PolyRing );
			}

			++nCount;
		}
	}
	
	RadarPlayerList::iterator sIter = m_Players.begin();
	while (sIter != m_Players.end())
	{
		(*sIter)->Render();
		sIter++;
	}
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::Update
//
//  PURPOSE:	Decide if we should draw the radar...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::Update()
{
	// No need to update if we aren't going to render...

	if( !m_bDraw || (m_mapRadarObjects.size() == 0 && m_Players.size() == 0) )
		return;

	LTVector vLocalPos, vObjPos, vDir;
	g_pLTClient->GetObjectPos( g_pLTClient->GetClientObject(), &vLocalPos );

	float fYaw = g_pPlayerMgr->GetYaw() - MATH_PI;

	if (g_pPlayerMgr->IsCameraAttachedToHead())
	{
		fYaw = m_fLastRotation;
	}
	else
	{
		m_fLastRotation = fYaw;
	}

	LTRotation rRot( 0.0f, fYaw, 0.0f );
	LTMatrix mMat;

	rRot.ConvertToMatrix( mMat );
	mMat = ~mMat;

	RADAROBJECT *pRadarObj = LTNULL;
	HOBJECT		hObj = LTNULL;
	RadarObjectList::iterator iter;

	for( iter = m_mapRadarObjects.begin(); iter != m_mapRadarObjects.end(); ++iter )
	{
		hObj = iter->first;
		pRadarObj = iter->second;
		
		g_pLTClient->GetObjectPos( hObj, &vObjPos );
		
		float fDist = vLocalPos.Dist( vObjPos );

		// Focus on all objects within maxshowdist.  All objects father than maxshowdist
		// are pushed to within 1/e^2 of the edge.
		// This will scale fDist between 0 and 1.
		fDist = 1.0f - 1.0f / ( float )( exp( 2.0f * fDist / m_nMaxShowDist ));

		// This will scale fDist within the hud element size.
		fDist = ( fDist * (float)m_nBaseSize / 2.0f);
		
		vDir = vLocalPos - vObjPos;
		MatVMul_InPlace_3x3( &mMat, &vDir );

		vDir.y = 0.0f;
		if( vDir.LengthSquared() > 0.01f )
		{
			vDir.Normalize();
		}
		else
		{
			vDir.Init();
		}

		vDir *= fDist;

		float fx = (pRadarObj->m_fBasePosX + vDir.x) * g_pInterfaceResMgr->GetXRatio() - 0.5f;
		float fy = (pRadarObj->m_fBasePosY - vDir.z) * g_pInterfaceResMgr->GetXRatio() - 0.5f;
		float fw = pRadarObj->m_fObjectSize * g_pInterfaceResMgr->GetXRatio();
		float fNew_W;
		
		pRadarObj->m_fRadius	= fw*0.5f;
		pRadarObj->m_fCenterX	= fx+pRadarObj->m_fRadius;
		pRadarObj->m_fCenterY	= fy+pRadarObj->m_fRadius;		

		if( pRadarObj->m_bFlashOn )
		{
			float	fX[4];
			float	fY[4];				
			float	fRot	= g_pLTClient->GetTime()*2.0f;

			for ( int i=0 ; i<4 ; ++i )
			{
				fX[i] = pRadarObj->m_fCenterX + (float)cos(fRot) * pRadarObj->m_fRadius * g_pInterfaceResMgr->GetXRatio();
				fY[i] = pRadarObj->m_fCenterY + (float)sin(fRot) * pRadarObj->m_fRadius * g_pInterfaceResMgr->GetYRatio();

				fRot += (MATH_PI / 2);
			}
			g_pDrawPrim->SetXY4( &pRadarObj->m_Poly,
								 fX[0], fY[0],
								 fX[1], fY[1],
								 fX[2], fY[2],
								 fX[3], fY[3] );

			pRadarObj->m_fRingAlpha -= g_pLTClient->GetFrameTime()*350.0f;
			if( pRadarObj->m_fRingAlpha < 0.0f )
			{
				pRadarObj->m_fRingAlpha = 255.0f;				
			}

			fNew_W	= fw * (2.0f - pRadarObj->m_fRingAlpha/255.0f);
			fx		-= (fNew_W-fw)*0.5f;
			fy		-= (fNew_W-fw)*0.5f;

			g_pDrawPrim->SetXYWH( &pRadarObj->m_PolyRing, fx, fy, fNew_W, fNew_W );
			g_pDrawPrim->SetALPHA( &pRadarObj->m_PolyRing, (uint8)pRadarObj->m_fRingAlpha );
		}
		else
		{
			g_pDrawPrim->SetXYWH( &pRadarObj->m_Poly, fx, fy, fw, fw );
		}
	}
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::UpdateLayout
//
//  PURPOSE:	Get data for this HUDItem from layouts.txt...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::UpdateLayout()
{
	int nCurrentLayout = GetConsoleInt("HUDLayout",0);
	
	// Use the compass values to determine where the radar should go...

	//g_pLayoutMgr->GetCompassPos( nCurrentLayout );
	//m_nBaseSize		= g_pLayoutMgr->GetCompassSize( nCurrentLayout );
	m_BasePos.x		= 11;
	m_BasePos.y		= 11;
	m_nBaseSize		= 106;
	

	m_nMaxShowDist	= g_pLayoutMgr->GetRadarMaxShowDist( nCurrentLayout );
	
	nLiveColor		= g_pLayoutMgr->GetRadarLivePlayerColor(nCurrentLayout);
	nDeadColor		= g_pLayoutMgr->GetRadarDeadPlayerColor(nCurrentLayout);
	nTalkColor		= g_pLayoutMgr->GetRadarTalkPlayerColor(nCurrentLayout);
	fTotalFlashTime	= g_pLayoutMgr->GetRadarFlashTime(nCurrentLayout);


	LTVector vCol = g_pLayoutMgr->GetVector("Scores","Team1Color");
	uint8 nR = (uint8)vCol.x;
	uint8 nG = (uint8)vCol.y;
	uint8 nB = (uint8)vCol.z;
	nTeamColors[0][0]=  SET_ARGB(0x80,nR,nG,nB);
	nTeamColors[0][1]=  SET_ARGB(0x20,nR,nG,nB);
	nTeamColors[0][2]=  SET_ARGB(0x00,nR,nG,nB);

	vCol = g_pLayoutMgr->GetVector("Scores","Team2Color");
	nR = (uint8)vCol.x;
	nG = (uint8)vCol.y;
	nB = (uint8)vCol.z;
	nTeamColors[1][0]=  SET_ARGB(0x80,nR,nG,nB);
	nTeamColors[1][1]=  SET_ARGB(0x20,nR,nG,nB);
	nTeamColors[1][2]=  SET_ARGB(0x00,nR,nG,nB);
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::AddObject
//
//  PURPOSE:	Add a new object to the radar and display with the specified type...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::AddObject( HOBJECT hObj, uint8 nRadarType, uint8 nVisibleTeamId )
{
	if( !hObj || !g_pRadarTypeMgr )
		return;

	// Don't add the object more than once...

	if( m_mapRadarObjects.find( hObj ) != m_mapRadarObjects.end() )
	{
		g_pLTClient->CPrint("HUDRadar: failed adding object (duplicate)");
		return;
	}

	RADARTYPE *pType = g_pRadarTypeMgr->GetRadarType( nRadarType );
	if( pType && pType->szIcon )
	{
		HTEXTURE hTex = g_pInterfaceResMgr->GetTexture( pType->szIcon );
		if( !hTex )
		{
			g_pLTClient->CPrint("HUDRadar: failed adding object (no texture %s)", pType->szIcon);
			return;
		}
		
		RADAROBJECT *pRadObj = debug_new( RADAROBJECT );
		if( !pRadObj )
			return;


		float fCenterOffset = m_nBaseSize*0.5f - pType->nObjectSize*0.5f;

		if( strnicmp( pType->szName, RADAR_MISSION_POINT, strlen(RADAR_MISSION_POINT) ) == 0 )
		{
			pRadObj->m_bDraw			= true;
		}
		else
		{
			pRadObj->m_bDraw			= false;
		}
		pRadObj->m_hTex				= hTex;
		pRadObj->m_nDrawOrder		= pType->nDrawOrder;
		pRadObj->m_nVisibleTeamId	= nVisibleTeamId;
		pRadObj->m_fObjectSize		= (float)pType->nObjectSize;
		pRadObj->m_fBasePosX		= m_BasePos.x + fCenterOffset;
		pRadObj->m_fBasePosY		= m_BasePos.y + fCenterOffset;

		g_pDrawPrim->SetRGBA( &pRadObj->m_Poly, argbWhite );
		SetupQuadUVs( pRadObj->m_Poly, hTex, 0.0f, 0.0f, 1.0f, 1.0f );


		hTex = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/RADAR/RADAR_PLAYER_RING.DTX" );
		if( !hTex )
		{
			g_pLTClient->CPrint("HUDRadar: failed adding object");
			return;
		}
		pRadObj->m_hTexRing		= hTex;
		g_pDrawPrim->SetRGBA( &pRadObj->m_PolyRing, argbWhite );
		SetupQuadUVs( pRadObj->m_PolyRing, hTex, 0.0f, 0.0f, 1.0f, 1.0f );

		m_mapRadarObjects[hObj] = pRadObj;
		m_ROSortList.push_back(pRadObj);
		m_ROSortList.sort(RadarObjectLesser());
	}
	else
	{
		g_pLTClient->CPrint("HUDRadar: failed adding object (unknown type %d)",nRadarType);
	}
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::AddObject
//
//  PURPOSE:	Add a new object to the radar and display with the specified type...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::AddObject( HOBJECT hObj, const char *pRadarType, uint8 nVisibleTeamId )
{
	if( !hObj || !pRadarType || !g_pRadarTypeMgr )
		return;

	RADARTYPE *pType = g_pRadarTypeMgr->GetRadarType( pRadarType );
	if( pType )
	{
		AddObject( hObj, pType->nId, nVisibleTeamId );
	}
	else
	{
		g_pLTClient->CPrint("HUDRadar: failed adding object (unknown type %s)",pRadarType);
	}
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::RemoveObject
//
//  PURPOSE:	Remove the object from the radar...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::RemoveObject( HOBJECT hObj )
{
	if( !hObj )
		return;

	//if we have a player record for this object, remove it
	RemovePlayer(hObj);

	// Make sure we actually have the object in our list...

	RadarObjectList::iterator iter = m_mapRadarObjects.find( hObj );
	if(  iter == m_mapRadarObjects.end() )
		return;

	m_ROSortList.remove(iter->second);

	debug_delete( iter->second );
	m_mapRadarObjects.erase( iter );


}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::ChangeRadarType
//
//  PURPOSE:	Change the texture used to display the object...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::ChangeRadarType( HOBJECT hObj, const char *pRadarType )
{
	if( !hObj || !pRadarType || !g_pRadarTypeMgr )
		return;

	RadarObjectList::iterator iter = m_mapRadarObjects.find( hObj );
	if( iter == m_mapRadarObjects.end() )
		return;

	RADARTYPE *pType = g_pRadarTypeMgr->GetRadarType( pRadarType );
	if( pType && pType->szIcon )
	{
		HTEXTURE hTex = g_pInterfaceResMgr->GetTexture( pType->szIcon );
		if( !hTex )
			return;
		
		iter->second->m_bDraw		= true;

		if( stricmp( pRadarType, RADAR_PLAYER_DEAD_TYPE ) == 0 )	//죽은사람
		{
			iter->second->m_bDraw		= false;
		}
		if( stricmp( pRadarType, RADAR_PLAYER_TALK_TYPE ) == 0 )
		{
			iter->second->m_bFlashOn	= true;
		}
		else
		{
			iter->second->m_bFlashOn	= false;
			iter->second->m_fRingAlpha	= 255.0f;
		}

		iter->second->m_hTex = hTex;

		iter->second->m_nDrawOrder = pType->nDrawOrder;
		m_ROSortList.sort(RadarObjectLesser());
	}
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::AddPlayer
//
//  PURPOSE:	Add a player name and icon to the radar...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::AddPlayer( HOBJECT hObj, uint32 nId )
{
	if (!hObj) return;
	T_PLAYER_INFOptr pPI = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByID( nId );
	
	if( !pPI ) return;
	
	HOBJECT hLocalObj = g_pLTClient->GetClientObject();

	uint8 nTeamID = INVALID_TEAM;

	// Check for teams and only display players of the same team...
	if( IsTeamGameType() && hLocalObj != hObj)
	{
#ifdef GAME_SHOW
		if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
		{
		}
		else
		{
			if( g_GameDoc.GetTeam() != pPI->nTeamID ) return;
		}
#else
		if( g_GameDoc.GetTeam() != pPI->nTeamID ) return;
#endif

		nTeamID = pPI->nTeamID;		
	}
	
	bool bDead = false;

	if (hLocalObj != hObj)
	{
#ifdef GAME_SHOW
		if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
		{
			if( TEAM_A == nTeamID )
			{
				AddObject( hObj, RADAR_PLAYER_ENEMY_TYPE, nTeamID );
			}
			else
			{
				AddObject( hObj, RADAR_PLAYER_ALIVE_TYPE, nTeamID );
			}
		}
		else
		{
			AddObject( hObj, RADAR_PLAYER_ALIVE_TYPE, nTeamID );
		}
#else
		AddObject( hObj, RADAR_PLAYER_ALIVE_TYPE, nTeamID );
#endif
	}

	CCharacterFX *pCharacter = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(hObj);
	if (pCharacter && pCharacter->IsRadarPlayerDead())
	{
		bDead = true;
	}


	RadarPlayerList::iterator iter = m_Players.begin();
	while (iter != m_Players.end() &&  ( (*iter)->hObj != hObj ) )
	{
		iter++;
	}

	RADAR_PLAYER_OBJ* pPlayer = NULL;

	//new player...
	if (iter == m_Players.end())
	{
		pPlayer = debug_new(RADAR_PLAYER_OBJ);
		m_Players.push_back(pPlayer);
	}
	else
	{
		pPlayer = (*iter);
	}

	pPlayer->nID = nId;
	pPlayer->hObj = hObj;
	
	SetPlayerDead(hObj,bDead);
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::RemovePlayer
//
//  PURPOSE:	Remove a player name from the radar...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::RemovePlayer(HOBJECT hObj)
{
	if (!hObj) return;

	RadarPlayerList::iterator iter = m_Players.begin();
	while (iter != m_Players.end() &&  ( (*iter)->hObj != hObj ) )
	{
		iter++;
	}

	if (iter != m_Players.end())
	{
		debug_delete(*iter);
		m_Players.erase(iter);
	}
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CHUDRadar::SetPlayerDead
//
//  PURPOSE:	Mark a player's name as dead...
//
// ----------------------------------------------------------------------- //

void CHUDRadar::SetPlayerDead( HOBJECT hObj, bool bDead )
{
	if (!hObj) return;

	if (bDead)
	{
		ChangeRadarType(hObj, RADAR_PLAYER_DEAD_TYPE );
	}
	else
	{
#ifdef GAME_SHOW
		if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
		{
			T_PLAYER_INFOptr pPI = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByHandle(hObj);
			if( TEAM_A == pPI->nTeamID )
			{
				ChangeRadarType(hObj, RADAR_PLAYER_ENEMY_TYPE );
			}
			else if( TEAM_B == pPI->nTeamID )
			{
				ChangeRadarType(hObj, RADAR_PLAYER_ALIVE_TYPE );
			}
		}
		else
		{
			ChangeRadarType(hObj, RADAR_PLAYER_ALIVE_TYPE );
		}
#else
		ChangeRadarType(hObj, RADAR_PLAYER_ALIVE_TYPE );
#endif
	}

	RadarPlayerList::iterator iter = m_Players.begin();
	while (iter != m_Players.end() &&  (*iter)->hObj != hObj )
	{
		iter++;
	}

	if (iter != m_Players.end())
	{
		(*iter)->bDead = bDead;
	}
}

void CHUDRadar::SetPlayerTalk( uint32 nId )
{
	RadarPlayerList::iterator iter = m_Players.begin();
	while (iter != m_Players.end() &&  (*iter)->nID != nId )
	{
		iter++;
	}

	if (iter != m_Players.end())
	{
		(*iter)->fFlashTime = fTotalFlashTime;
	}
}



void CHUDRadar::UpdatePlayerID(HOBJECT hObj, uint32 nId)
{
	if (!hObj) return;

	RadarPlayerList::iterator iter = m_Players.begin();
	while (iter != m_Players.end() &&  (*iter)->hObj != hObj )
	{
		iter++;
	}

	if (iter != m_Players.end())
	{
		(*iter)->nID = nId;
	}
}

void CHUDRadar::Reset()
{

	m_ROSortList.clear();

	RadarObjectList::iterator iter;
	for( iter = m_mapRadarObjects.begin(); iter != m_mapRadarObjects.end(); ++iter )
	{
		debug_delete( iter->second );
	}
	m_mapRadarObjects.clear();

	for (uint16 i = 0; i < m_Players.size(); i++)
	{
		debug_delete(m_Players[i]);
	}
	m_Players.clear();
}

void CHUDRadar::ScreenDimChanged()
{
}

void RADAR_PLAYER_OBJ::Render()
{
	if (fFlashTime > 0.0f)
	{
		fFlashTime -= g_pLTClient->GetFrameTime();

		if (fFlashTime < 0.0f)
		{
			bFlashOn	= false;
		}
		else
		{
			bFlashOn	= true;
		}

		if (bFlashOn)
		{
			g_pRadar->ChangeRadarType(hObj, RADAR_PLAYER_TALK_TYPE );
		}
		else
		{
			if (bDead)
			{
				g_pRadar->ChangeRadarType(hObj, RADAR_PLAYER_DEAD_TYPE );
			}
			else
			{
				g_pRadar->ChangeRadarType(hObj, RADAR_PLAYER_ALIVE_TYPE );
			}
		}

	}
}