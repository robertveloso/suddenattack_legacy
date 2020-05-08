#include "StdAfx.h"
#include "SaListRoom.h"
#include "InterfaceResMgr.h"
#include "ScreenCommands.h"
#include "MissionButeMgr.h"
#include "../ClientShellShared/ScreenCommands.h"
#include <algorithm>

#define ROOM_TEXT_COLOR_WAIT	RGB(255,255,255)
#define ROOM_TEXT_COLOR_GAMING	RGB(180,180,180)

//------------------------------------------- (B) Novice (minjin) [03/09/2005]
#include "GameText.h"
#include "GameMessage.h"
//------------------------------------------- (E) Novice (minjin) [03/09/2005]

CLTGUITooltipRoom* g_pToolTipRoom;

using namespace std;

struct LessRoomIndex
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_ITEMptr)p)->nIndex < ((T_ROOM_ITEMptr)q)->nIndex;
	}
};

struct GreatRoomIndex
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_ITEMptr)p)->nIndex > ((T_ROOM_ITEMptr)q)->nIndex;
	}
};

struct LessMapIndex
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_ITEMptr)p)->nMapIndex < ((T_ROOM_ITEMptr)q)->nMapIndex;
	}
};

struct GreatMapIndex
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_ITEMptr)p)->nMapIndex > ((T_ROOM_ITEMptr)q)->nMapIndex;
	}
};

struct LessUserCount
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_ITEMptr)p)->nMaxUser < ((T_ROOM_ITEMptr)q)->nMaxUser;
	}
};

struct GreatUserCount
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_ITEMptr)p)->nMaxUser > ((T_ROOM_ITEMptr)q)->nMaxUser;
	}
};

struct LessGameType
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_ITEMptr)p)->nType < ((T_ROOM_ITEMptr)q)->nType;
	}
};

struct GreatGameType
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_ITEMptr)p)->nType > ((T_ROOM_ITEMptr)q)->nType;
	}
};

struct LessPing
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_ITEMptr)p)->nPing < ((T_ROOM_ITEMptr)q)->nPing;
	}
};

struct GreatPing
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return ((T_ROOM_ITEMptr)p)->nPing > ((T_ROOM_ITEMptr)q)->nPing;
	}
};

struct LessRoomTitle
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return strcmp(((T_ROOM_ITEMptr)p)->szTitle, ((T_ROOM_ITEMptr)q)->szTitle) < 0;
	}
};
struct GreatRoomTitle
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		return strcmp(((T_ROOM_ITEMptr)p)->szTitle, ((T_ROOM_ITEMptr)q)->szTitle) > 0;
	}
};

struct LessEntranceRoom
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		T_ROOM_ITEMptr pR1	= (T_ROOM_ITEMptr)p;
		T_ROOM_ITEMptr pR2	= (T_ROOM_ITEMptr)q;

		if( !pR1->bPlaying && pR2->bPlaying )
		{
			return TRUE;
		}		
		return FALSE;
	}
};

struct GreatEntranceRoom
{
	bool operator()( const PVOID p, const PVOID q ) const
	{
		T_ROOM_ITEMptr pR1	= (T_ROOM_ITEMptr)p;
		T_ROOM_ITEMptr pR2	= (T_ROOM_ITEMptr)q;

		if( pR1->bPlaying && !pR2->bPlaying )
		{
			return TRUE;
		}		
		return FALSE;
	}
};

CSaListRoom::CSaListRoom()
:m_bLessNo(LTTRUE)
,m_bLessTitle(LTTRUE)
,m_bLessMap(LTTRUE)
,m_bLessUserCnt(LTFALSE)
,m_bLessType(LTTRUE)
,m_bLessState(LTTRUE)
,m_bLessPing(LTTRUE)
, CLTGUIListView() {}

CSaListRoom::~CSaListRoom()
{
	this->Term();
}

LTBOOL CSaListRoom::Create(uint16 nPosX, uint16 nPosY,
						   uint16 nWidth, uint16 nHeight, uint16 nItemHeight,
						   CLTGUICommandHandler* pCommandHandler,
						   LTBOOL bDrawBack, uint32 nBackColor, uint32 nBorderColor )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight, 
							pCommandHandler, bDrawBack, nBackColor, nBorderColor );

	m_hPrivate	= g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\Menu\\Textures\\Main\\LockIcon.dtx");

	g_pDrawPrim->SetUVWH( &m_PolyLock, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyLock, 0xFFFFFFFF );

	g_pDrawPrim->SetUVWH( &m_PolyPing, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyPing, 0xFFFFFFFF );
	
	return  LTTRUE;
}

LTBOOL CSaListRoom::AddRoom( T_ROOM_ITEMptr pRoomItem )
{
	LTBOOL bResult;
	char			szTemp[256];

	if( pRoomItem->bPlaying == LTFALSE ) pRoomItem->nPing = 0xFF;
	bResult = InsertItem( (void *)pRoomItem );
	if( !bResult )
	{
		return LTFALSE;
	}
	
	pRoomItem->pNumber = new CSAD3DText();
	pRoomItem->pNumber->CreateD3DText( 32, 16, true );
	pRoomItem->pNumber->SetTransparent(true);

	pRoomItem->pTitle = new CSAD3DText();
	pRoomItem->pTitle->CreateD3DText( 190, 16, true );
	pRoomItem->pTitle->SetTransparent(true);

	pRoomItem->pMapName = new CSAD3DText();
	pRoomItem->pMapName->CreateD3DText( 81, 16, true );
	pRoomItem->pMapName->SetTransparent(true);

	pRoomItem->pPlayers = new CSAD3DText();
	pRoomItem->pPlayers->CreateD3DText( 54, 16, true );
	pRoomItem->pPlayers->SetTransparent(true);

	pRoomItem->pGameType = new CSAD3DText();
	pRoomItem->pGameType->CreateD3DText( 81, 16, true );
	pRoomItem->pGameType->SetTransparent(true);

	pRoomItem->pRoomState = new CSAD3DText();
	pRoomItem->pRoomState->CreateD3DText( 54, 16, true );
	pRoomItem->pRoomState->SetTransparent(true);

	wsprintf( szTemp, "%3d", pRoomItem->nIndex );
	pRoomItem->pNumber->DrawD3DText( szTemp,
									 strlen(szTemp),
									 DT_LEFT, D3D_TEXT_SHADOW, true );
	
	pRoomItem->pTitle->DrawD3DText( pRoomItem->szTitle,
									strlen(pRoomItem->szTitle),
									DT_LEFT, D3D_TEXT_SHADOW, true );

	pRoomItem->pMapName->DrawD3DText( pRoomItem->szMapName,
									  strlen(pRoomItem->szMapName),
									  DT_CENTER, D3D_TEXT_SHADOW, false );

	wsprintf( szTemp, "%2d/%2d", pRoomItem->nCurrentUser, pRoomItem->nMaxUser );
	pRoomItem->pPlayers->DrawD3DText( szTemp,
									  strlen(szTemp),
									  DT_CENTER, D3D_TEXT_SHADOW, false );

	pRoomItem->pGameType->DrawD3DText( pRoomItem->szType,
									   strlen(pRoomItem->szType),
									   DT_CENTER, D3D_TEXT_SHADOW, false );

	wsprintf( szTemp, "%s", pRoomItem->bPlaying ? NF_GetMsg(SA_TEXT_PLAYING) : NF_GetMsg(SA_TEXT_READY) );
	pRoomItem->pRoomState->DrawD3DText( szTemp,
										strlen(szTemp),
										DT_CENTER, D3D_TEXT_SHADOW, false );

	if( !pRoomItem->bPlaying )
	{
		pRoomItem->pNumber->SetTextColor( ROOM_TEXT_COLOR_WAIT );
		pRoomItem->pTitle->SetTextColor( ROOM_TEXT_COLOR_WAIT );
		pRoomItem->pMapName->SetTextColor( ROOM_TEXT_COLOR_WAIT );
		pRoomItem->pPlayers->SetTextColor( ROOM_TEXT_COLOR_WAIT );
		pRoomItem->pGameType->SetTextColor( ROOM_TEXT_COLOR_WAIT );
		pRoomItem->pRoomState->SetTextColor( ROOM_TEXT_COLOR_WAIT );
	}
	else
	{
		pRoomItem->pNumber->SetTextColor( ROOM_TEXT_COLOR_GAMING );
		pRoomItem->pTitle->SetTextColor( ROOM_TEXT_COLOR_GAMING );
		pRoomItem->pMapName->SetTextColor( ROOM_TEXT_COLOR_GAMING );
		pRoomItem->pPlayers->SetTextColor( ROOM_TEXT_COLOR_GAMING );
		pRoomItem->pGameType->SetTextColor( ROOM_TEXT_COLOR_GAMING );
		pRoomItem->pRoomState->SetTextColor( ROOM_TEXT_COLOR_GAMING );
	}
	
	return LTTRUE;
}

LTBOOL CSaListRoom::RemoveRoom( int nRoomIndex )
{
	int					nIndex;
	LTLV_ITEM_LIST_ITR	itr;
	BOOL				bResult;
	T_ROOM_ITEMptr		pRoomItem;	

	for( itr=m_ItemList.begin(), nIndex=0 ; itr!=m_ItemList.end() ; ++itr, ++nIndex )
	{
		pRoomItem = (T_ROOM_ITEMptr)(*itr);
		if( pRoomItem->nIndex == nRoomIndex )
		{
			SAFE_DELETE( pRoomItem->pNumber );
			SAFE_DELETE( pRoomItem->pTitle );
			SAFE_DELETE( pRoomItem->pMapName );
			SAFE_DELETE( pRoomItem->pPlayers );
			SAFE_DELETE( pRoomItem->pGameType );
			SAFE_DELETE( pRoomItem->pRoomState );
						
			bResult = DeleteItemPtr( pRoomItem, nIndex );
			if( bResult ) m_ItemList.erase(itr);
			break;
		}
	}
	
	return LTTRUE;
}

void CSaListRoom::DeleteAllItem()
{
	for( int i=0 ; i<m_nTotalItemCount ; ++i )
	{
		T_ROOM_ITEMptr pItem = (T_ROOM_ITEMptr)m_ItemList[i];

		SAFE_DELETE( pItem->pNumber );
		SAFE_DELETE( pItem->pTitle );
		SAFE_DELETE( pItem->pMapName );
		SAFE_DELETE( pItem->pPlayers );
		SAFE_DELETE( pItem->pGameType );
		SAFE_DELETE( pItem->pRoomState );
	}

	CLTGUIListView::DeleteAllItem();
}

T_ROOM_ITEMptr CSaListRoom::GetRoomData( int nRoomIndex )
{
	LTLV_ITEM_LIST_ITR	itr;
	T_ROOM_ITEMptr		pRoomItem;

	for( itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
	{
		pRoomItem = (T_ROOM_ITEMptr)(*itr);
		if( pRoomItem->nIndex == nRoomIndex )
		{
			return pRoomItem;
		}
	}

	return LTNULL;
}

T_ROOM_ITEMptr CSaListRoom::GetSelectedRoomData()
{
	if( m_nSelIndex < 0 ) return LTNULL;
	return (T_ROOM_ITEMptr)m_ItemList[m_nSelIndex];
}

LTBOOL CSaListRoom::SetRoomUser( int nRoomIndex, int nMaxUser, int nCurrentUser )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		pRoomItem->nMaxUser		= nMaxUser;
		pRoomItem->nCurrentUser	= nCurrentUser;
	
		char szTemp[16];
		wsprintf( szTemp, "%2d/%2d", pRoomItem->nCurrentUser, pRoomItem->nMaxUser );
		pRoomItem->pPlayers->DrawD3DText( szTemp,
										  strlen(szTemp),
										  DT_CENTER, D3D_TEXT_SHADOW, false );
		return LTTRUE;
	}
	return LTFALSE;
}

LTBOOL CSaListRoom::IncRoomUser( int nRoomIndex )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		if( pRoomItem->nCurrentUser < pRoomItem->nMaxUser )	++pRoomItem->nCurrentUser;
		
		char szTemp[16];
		wsprintf( szTemp, "%2d/%2d", pRoomItem->nCurrentUser, pRoomItem->nMaxUser );
		pRoomItem->pPlayers->DrawD3DText( szTemp,
										  strlen(szTemp),
										  DT_CENTER, D3D_TEXT_SHADOW, false );
		return LTTRUE;
	}
	return LTFALSE;
}

LTBOOL CSaListRoom::DecRoomUser( int nRoomIndex )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		if( pRoomItem->nCurrentUser > 0 ) --pRoomItem->nCurrentUser;
		
		char szTemp[16];
		wsprintf( szTemp, "%2d/%2d", pRoomItem->nCurrentUser, pRoomItem->nMaxUser );
		pRoomItem->pPlayers->DrawD3DText( szTemp,
										  strlen(szTemp),
										  DT_CENTER, D3D_TEXT_SHADOW, false );
		return LTTRUE;
	}
	return LTFALSE;
}

LTBOOL CSaListRoom::SetMaxUser( int nRoomIndex, int nMaxUser )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		pRoomItem->nMaxUser		= nMaxUser;
		
		char szTemp[16];
		wsprintf( szTemp, "%2d/%2d", pRoomItem->nCurrentUser, pRoomItem->nMaxUser );
		pRoomItem->pPlayers->DrawD3DText( szTemp,
										  strlen(szTemp),
										  DT_CENTER, D3D_TEXT_SHADOW, false );
		return LTTRUE;
	}
	return LTFALSE;
}

void CSaListRoom::SetPing( int nRoomIndex, uint8 nPing )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		pRoomItem->nPing		= nPing;
		return;
	}
}

LTBOOL CSaListRoom::SetMapName( int nRoomIndex, int nMapIndex, HTEXTURE hTex, const char* lpszMapName, int nGameType, const char* lpszGameType )
{
	if( !lpszMapName ) return LTFALSE;
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		pRoomItem->nMapIndex = nMapIndex;
		strcpy( pRoomItem->szMapName, lpszMapName );
		pRoomItem->hScreenShot	= hTex;
		pRoomItem->nType	= nGameType;
		strcpy( pRoomItem->szType, lpszGameType );
		
		pRoomItem->pMapName->DrawD3DText( pRoomItem->szMapName,
										  strlen(pRoomItem->szMapName),
										  DT_CENTER, D3D_TEXT_SHADOW, false );
		return LTTRUE;
	}
	return LTFALSE;
}

LTBOOL CSaListRoom::SetRoomOption( int nRoomIndex,
								   unsigned char nGameType,
								   unsigned char nMaxUsers,
								   unsigned char nKillLimit, 
								   unsigned char nRoundLimit, 
								   bool bIntrusion, 
								   bool bTeamBalance )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		pRoomItem->nMaxUser		= nMaxUsers;
		pRoomItem->nRoundLimit	= nRoundLimit;
		pRoomItem->nKillLimit	= nKillLimit;
		pRoomItem->bInstrusion	= bIntrusion;
		pRoomItem->bTeamBalance	= bTeamBalance;

		char szTemp[16];
		wsprintf( szTemp, "%2d/%2d", pRoomItem->nCurrentUser, pRoomItem->nMaxUser );
		pRoomItem->pPlayers->DrawD3DText( szTemp,
										  strlen(szTemp),
										  DT_CENTER, D3D_TEXT_SHADOW, false );
		
		switch( (GameType)nGameType )
		{
		case eGameTypeTeamDeathmatch:
			pRoomItem->pGameType->DrawD3DText( NF_GetMsg(SA_TEXT_TEAM_DM),
											   strlen(NF_GetMsg(SA_TEXT_TEAM_DM)),
											   DT_CENTER, D3D_TEXT_SHADOW, false );
			break;
		case eGameTypeSurvival:
			pRoomItem->pGameType->DrawD3DText( NF_GetMsg(SA_TEXT_BOMB_MISSION),
											   strlen(NF_GetMsg(SA_TEXT_BOMB_MISSION)),
											   DT_CENTER, D3D_TEXT_SHADOW, false );
			break;
		}
		//

		return LTTRUE;
	}
	return LTFALSE;
}

LTBOOL CSaListRoom::SetRoomTitle( int nRoomIndex, const char* lpszTitle )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		strcpy( pRoomItem->szTitle, lpszTitle );
		
		pRoomItem->pTitle->DrawD3DText( pRoomItem->szTitle,
										strlen(pRoomItem->szTitle),
										DT_LEFT, D3D_TEXT_SHADOW, true );
		return LTTRUE;
	}
	return LTFALSE;
}

LTBOOL CSaListRoom::SetGameLimit( int nRoomIndex, int nTimeLimit, int nKillLimit, int nRoundLimit )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		pRoomItem->nRoundLimit	= nRoundLimit;
		pRoomItem->nKillLimit	= nKillLimit;
		return LTTRUE;
	}
	return LTFALSE;
}

LTBOOL CSaListRoom::SetRoomState( int nRoomIndex, LTBOOL bPlaying )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		pRoomItem->bPlaying	= bPlaying;	
		if( pRoomItem->bPlaying == LTFALSE ) pRoomItem->nPing = 0xFF;
		
		char szTemp[16];
		if( pRoomItem->bPlaying )
		{
			wsprintf( szTemp, "%s", NF_GetMsg(SA_TEXT_PLAYING) );
			pRoomItem->pNumber->SetTextColor( ROOM_TEXT_COLOR_GAMING );
			pRoomItem->pTitle->SetTextColor( ROOM_TEXT_COLOR_GAMING );
			pRoomItem->pMapName->SetTextColor( ROOM_TEXT_COLOR_GAMING );
			pRoomItem->pPlayers->SetTextColor( ROOM_TEXT_COLOR_GAMING );
			pRoomItem->pGameType->SetTextColor( ROOM_TEXT_COLOR_GAMING );
			pRoomItem->pRoomState->SetTextColor( ROOM_TEXT_COLOR_GAMING );
			
		}
		else
		{
			wsprintf( szTemp, "%s", NF_GetMsg(SA_TEXT_READY) );
			pRoomItem->pNumber->SetTextColor( ROOM_TEXT_COLOR_WAIT );
			pRoomItem->pTitle->SetTextColor( ROOM_TEXT_COLOR_WAIT );
			pRoomItem->pMapName->SetTextColor( ROOM_TEXT_COLOR_WAIT );
			pRoomItem->pPlayers->SetTextColor( ROOM_TEXT_COLOR_WAIT );
			pRoomItem->pGameType->SetTextColor( ROOM_TEXT_COLOR_WAIT );
			pRoomItem->pRoomState->SetTextColor( ROOM_TEXT_COLOR_WAIT );
		}

		pRoomItem->pRoomState->DrawD3DText( szTemp,
											strlen(szTemp),
											DT_CENTER, D3D_TEXT_SHADOW, false );
		return LTTRUE;
	}
	return LTFALSE;
}

LTBOOL CSaListRoom::SetInstrusion( int nRoomIndex, LTBOOL bEnable )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		pRoomItem->bInstrusion	= bEnable;
		return LTTRUE;
	}
	return LTFALSE;
}

LTBOOL CSaListRoom::SetTeamBalance( int nRoomIndex, LTBOOL bEnable )
{
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		pRoomItem->bTeamBalance	= bEnable;
		return LTTRUE;
	}
	return LTFALSE;
}

LTBOOL CSaListRoom::SetCaptain( int nRoomIndex, const char* lpszCaptain )
{
	if( !lpszCaptain ) return LTFALSE;
	T_ROOM_ITEMptr pRoomItem = GetRoomData(nRoomIndex);
	if( pRoomItem )
	{
		strcpy( pRoomItem->szCaptain, lpszCaptain );
		return LTTRUE;
	}
	return LTFALSE;
}

void CSaListRoom::SortRoomNumber()
{
	if( m_bLessNo )
	{
		m_bLessNo = LTFALSE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), LessRoomIndex() );		
	}
	else
	{
		m_bLessNo = LTTRUE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), GreatRoomIndex() );
	}
}

void CSaListRoom::SortGameType( LTBOOL bInit )
{
	if( bInit ) m_bLessType = LTFALSE;

	if( m_bLessType )
	{
		m_bLessType = LTFALSE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), LessGameType() );		
	}
	else
	{
		m_bLessType = LTTRUE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), GreatGameType() );
	}
}

void CSaListRoom::SortMapIndex()
{
	if( m_bLessMap )
	{
		m_bLessMap = LTFALSE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), LessMapIndex() );		
	}
	else
	{
		m_bLessMap = LTTRUE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), GreatMapIndex() );
	}
}

void CSaListRoom::SortPing()
{
	if( m_bLessPing )
	{
		m_bLessPing = LTFALSE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), LessPing() );		
	}
	else
	{
		m_bLessPing = LTTRUE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), GreatPing() );
	}
}

void CSaListRoom::SortRoomTitle()
{
	if( m_bLessTitle )
	{
		m_bLessTitle = LTFALSE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), LessRoomTitle() );		
	}
	else
	{
		m_bLessTitle = LTTRUE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), GreatRoomTitle() );
	}
}

void CSaListRoom::SortUserCount()
{
	if( m_bLessUserCnt )
	{
		m_bLessUserCnt = LTFALSE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), LessUserCount() );		
	}
	else
	{
		m_bLessUserCnt = LTTRUE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), GreatUserCount() );
	}
}

void CSaListRoom::SortState( LTBOOL bInit )
{
	if( bInit ) m_bLessState = LTTRUE;

	if( m_bLessState )
	{
		m_bLessState = LTFALSE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), LessEntranceRoom() );
	}
	else
	{
		m_bLessState = LTTRUE;
		std::sort( m_ItemList.begin(), m_ItemList.end(), GreatEntranceRoom() );
	}
}

void CSaListRoom::OnOverChange( int nPreIndex, int nNewIndex )
{
	// nPreIndex는 포커싱을 잃고 nNewIndex는 포코스를 얻는다
	// nCurIndex 가 -1인 경우는 아무것도 선택되지 않은 상태.

	if( !g_pToolTipRoom ) return;

	if( nNewIndex < 0 )
	{
		if( g_pToolTipRoom->IsVisible() ) g_pToolTipRoom->CtrlHide();
	}
	else
	{
		T_ROOM_ITEMptr pRoomItem = (T_ROOM_ITEMptr)m_ItemList[nNewIndex];
		if( !pRoomItem ) 
		{
			if( g_pToolTipRoom->IsVisible() ) g_pToolTipRoom->CtrlHide();
			return;
		}

		LTIntPt CurPos = g_pInterfaceMgr->GetCursorPos();
		CurPos.y = (nNewIndex-m_nFirstItemIndex) * m_nItemHeight + m_pos.y + m_nItemHeight;
		
		g_pToolTipRoom->SetRoomInfo( pRoomItem );
		g_pToolTipRoom->CtrlShow( CurPos );	
	}
	
}

LTBOOL CSaListRoom::RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver )
{
	T_ROOM_ITEMptr pRoomItem = (T_ROOM_ITEMptr)pListItem;

	if( bSelect )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, 0xB02B384C );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y, (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim( &m_PolyBar );
	}
	else if( bOver )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, 0x702B384C );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x, (float)ptItemPos->y, (float)m_nWidth, (float)m_nItemHeight );
		g_pDrawPrim->SetTexture( LTNULL );
		g_pDrawPrim->DrawPrim( &m_PolyBar );
	}
	
	if( pRoomItem->bPrivate ) 
	{
		g_pDrawPrim->SetXYWH( &m_PolyLock, (float)ptItemPos->x-2, (float)ptItemPos->y+4, 32.0f, 32.0f );
		g_pDrawPrim->SetTexture( m_hPrivate );
		g_pDrawPrim->DrawPrim( &m_PolyLock );
	}

	if( pRoomItem->bPlaying )
	{
		g_pDrawPrim->SetXYWH( &m_PolyPing, (float)ptItemPos->x+500, (float)ptItemPos->y+9, 32.0f, 32.0f );
		g_pDrawPrim->SetTexture( g_Global_Data.GetPingTexture(pRoomItem->nPing) );
		g_pDrawPrim->DrawPrim( &m_PolyPing );
	}

	pRoomItem->pNumber->SetPosition( ptItemPos->x+7, ptItemPos->y+8 );
	pRoomItem->pTitle->SetPosition( ptItemPos->x+40, ptItemPos->y+8 );
	pRoomItem->pMapName->SetPosition( ptItemPos->x+237, ptItemPos->y+8 );
	pRoomItem->pPlayers->SetPosition( ptItemPos->x+313, ptItemPos->y+8 );
	pRoomItem->pGameType->SetPosition( ptItemPos->x+364, ptItemPos->y+8 );
	pRoomItem->pRoomState->SetPosition( ptItemPos->x+441, ptItemPos->y+8 );

	pRoomItem->pNumber->Render();
	pRoomItem->pTitle->Render();
	pRoomItem->pMapName->Render();
	pRoomItem->pPlayers->Render();
	pRoomItem->pGameType->Render();
	pRoomItem->pRoomState->Render();

	return LTTRUE;
}

LTBOOL CSaListRoom::OnLButtonDblClick( int x, int y )
{
	if( m_nSelIndex < 0 ) return LTFALSE;	//Not Handled.
	
	//Handle Msg
	T_ROOM_ITEMptr pRoomItem = GetSelectedRoomData();

	if( pRoomItem )
	{
		if( pRoomItem->bPrivate )
		{
			//비밀번호 입력!
			if( m_pCommandHandler )
			{
				m_pCommandHandler->SendCommand( CMD_SHOW_PASSWORD_DLG, LTNULL, LTNULL );
			}						
		}
		else
		{
			g_GameDoc.SetTitle( pRoomItem->szTitle );
			g_GameDoc.SetPassword( "" );
			g_Network.CS_JoinRoom( pRoomItem->nIndex );
		}		
	}
	return LTTRUE;	
}

const char* CSaListRoom::GetGameTypeText( int nType )
{
	switch( (GameType)nType )
	{
	case eGameTypeTeamDeathmatch:
		//					"데스매치"
		return NF_GetMsg(SA_TEXT_TEAM_DM);
	case eGameTypeSurvival:
		//					"폭파미션"
		return NF_GetMsg(SA_TEXT_BOMB_MISSION);		
	}
	

	//				"??"
	return NF_GetMsg(SA_TEXT_LIST_ROOM_GAMETYPE_UNKNOWN);
}

LTBOOL CSaListRoom::OnWheelUp( int x, int y )
{
	LTBOOL bResult = CLTGUIListView::OnWheelUp( x, y );
	m_pCommandHandler->SendCommand( CMD_MAIN_ROOM_WHELL, LTNULL, LTNULL );
	return bResult;
}

LTBOOL CSaListRoom::OnWheelDown( int x, int y )
{
	LTBOOL bResult = CLTGUIListView::OnWheelDown( x, y );
	m_pCommandHandler->SendCommand( CMD_MAIN_ROOM_WHELL, LTNULL, LTNULL );
	return bResult;
}




CLTGUITooltipRoom::CLTGUITooltipRoom()
{
	g_pToolTipRoom	= this;

	m_hBack			= LTNULL;
	m_hScreenShot	= LTNULL;
	m_pTitle		= LTNULL;
	m_pCaptain		= LTNULL;
	m_pType			= LTNULL;
	m_pMap			= LTNULL;
	m_pInstrusion	= LTNULL;
	m_pTeamBalance	= LTNULL;
}

CLTGUITooltipRoom::~CLTGUITooltipRoom()
{
	g_pToolTipRoom	= LTNULL;
	this->Term();
}

LTBOOL CLTGUITooltipRoom::CreateTooltip( uint16 nWidth, uint16 nHeight )
{
	m_hBack	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Main\\TxTooltipRoom.dtx" );
	g_pDrawPrim->SetUVWH( &m_Poly, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_Poly, 0xFFFFFFFF );

	g_pDrawPrim->SetUVWH( &m_PolyScreenShot, 0.0f, 0.0f, 182.0f/256.0f, 137.0f/256.0f );
	g_pDrawPrim->SetRGBA( &m_PolyScreenShot, 0xFFFFFFFF );

	m_hPlay	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Main\\TxTooltipPlay.dtx" );
	m_hWait	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE\\Menu\\Textures\\Main\\TxTooltipWait.dtx" );
	g_pDrawPrim->SetUVWH( &m_PolyState, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyState, 0xFFFFFFFF );

	m_pTitle	= new CSAD3DText();
	m_pTitle->CreateD3DText( 256, 16, true );
	m_pTitle->SetTextColor( RGB(183,157,111), RGB(0,0,0) );
	m_pTitle->SetTransparent(true);

	m_pCaptain	= new CSAD3DText();
	m_pCaptain->CreateD3DText( 170, 16, false );
	m_pCaptain->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_pCaptain->SetTransparent(true);

	m_pType	= new CSAD3DText();
	m_pType->CreateD3DText( 170, 16, false );
	m_pType->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_pType->SetTransparent(true);

	m_pMap	= new CSAD3DText();
	m_pMap->CreateD3DText( 128, 16, false );
	m_pMap->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_pMap->SetTransparent(true);

	m_pInstrusion	= new CSAD3DText();
	m_pInstrusion->CreateD3DText( 128, 16, false );
	m_pInstrusion->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_pInstrusion->SetTransparent(true);
	m_pInstrusion->DrawD3DText( NF_GetMsg(SA_TEXT_ROOM_INFO_INTRUSION), strlen(NF_GetMsg(SA_TEXT_ROOM_INFO_INTRUSION)) );

	m_pTeamBalance	= new CSAD3DText();
	m_pTeamBalance->CreateD3DText( 128, 16, false );
	m_pTeamBalance->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	m_pTeamBalance->SetTransparent(true);
	m_pTeamBalance->DrawD3DText( NF_GetMsg(SA_TEXT_ROOM_INFO_TEAM_BALANCE), strlen(NF_GetMsg(SA_TEXT_ROOM_INFO_TEAM_BALANCE)) );
	
	return CLTGUITooltip::CreateTooltip( nWidth, nHeight );
}

void CLTGUITooltipRoom::Term()
{
	SAFE_DELETE( m_pTitle );
	SAFE_DELETE( m_pCaptain );
	SAFE_DELETE( m_pType );
	SAFE_DELETE( m_pMap );
	SAFE_DELETE( m_pInstrusion );
	SAFE_DELETE( m_pTeamBalance );
}

LTBOOL CLTGUITooltipRoom::SetRoomInfo( T_ROOM_ITEMptr pRoomItem )
{
	if( !pRoomItem ) return LTFALSE;
	char	szTemp[256];

	m_pTitle->DrawD3DText( pRoomItem->szTitle, strlen(pRoomItem->szTitle), DT_CENTER, D3D_TEXT_OUTLINE, true );
	sprintf( szTemp, "%s : %s", NF_GetMsg(SA_TEXT_ROOM_CAPTINE), pRoomItem->szCaptain );
	m_pCaptain->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_NORMAL, true );
	
	sprintf( szTemp, "%s ( %d : %d )", pRoomItem->szMapName, pRoomItem->nMaxUser/2, pRoomItem->nMaxUser/2 );
	m_pMap->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_NORMAL, false );

	MISSION* pMisson = g_pMissionButeMgr->GetMission(pRoomItem->nMapIndex);
	if( pMisson )
	{
		if( pRoomItem->nKillLimit > 0 )
		{
			sprintf( szTemp, "%s - %d %s", NF_GetMsg(pMisson->nGameTypeID), pRoomItem->nKillLimit, NF_GetMsg(SA_TEXT_ROOM_KILL) );
		}
		else
		{
			sprintf( szTemp, "%s - %d %s", NF_GetMsg(pMisson->nGameTypeID), pRoomItem->nRoundLimit, NF_GetMsg(SA_TEXT_ROOM_ROUND) );
		}
		m_pType->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_NORMAL, false );
	}

	if( pRoomItem->bInstrusion )
	{
		m_pInstrusion->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	}
	else
	{
		m_pInstrusion->SetTextColor( RGB(60,60,60), RGB(0,0,0) );
	}

	if( pRoomItem->bTeamBalance )
	{
		m_pTeamBalance->SetTextColor( RGB(255,255,255), RGB(0,0,0) );
	}
	else
	{
		m_pTeamBalance->SetTextColor( RGB(60,60,60), RGB(0,0,0) );
	}

	m_hScreenShot	= pRoomItem->hScreenShot;
	m_hState		= pRoomItem->bPlaying ? m_hPlay : m_hWait;
	return LTTRUE;
}


void CLTGUITooltipRoom::OnPosChanged()
{
	uint32 nW, nH;
	g_pTexInterface->GetTextureDims( m_hBack, nW, nH );
	g_pDrawPrim->SetXYWH( &m_Poly, (float)m_basePos.x, (float)m_basePos.y, (float)nW, (float)nH );

	g_pDrawPrim->SetXYWH( &m_PolyScreenShot,
						   (float)m_basePos.x+13.0f, (float)m_basePos.y+38.0f,
						   143.0f, 107.0f );

	g_pDrawPrim->SetXYWH( &m_PolyState,
						   (float)m_basePos.x+291.0f, (float)m_basePos.y+8.0f,
						   64.0f, 16.0f );

	m_pTitle->SetPosition( m_basePos.x+3, m_basePos.y + 7 );
	m_pCaptain->SetPosition( m_basePos.x + 161, m_basePos.y + 49 );
	m_pType->SetPosition( m_basePos.x + 161, m_basePos.y + 73 );
	m_pMap->SetPosition( m_basePos.x + 161, m_basePos.y + 97 );
	m_pInstrusion->SetPosition( m_basePos.x + 161, m_basePos.y + 121 );
	m_pTeamBalance->SetPosition( m_basePos.x + 261, m_basePos.y + 121 );
}

void CLTGUITooltipRoom::Render()
{
	if( !this->IsVisible() ) return;

	g_pDrawPrim->SetTexture( m_hBack );
	g_pDrawPrim->DrawPrim( &m_Poly );
	g_pDrawPrim->SetTexture( m_hState );
	g_pDrawPrim->DrawPrim( &m_PolyState );

	if( m_hScreenShot )
	{
		g_pDrawPrim->SetTexture( m_hScreenShot );
		g_pDrawPrim->DrawPrim( &m_PolyScreenShot );
	}

	m_pTitle->Render();
	m_pCaptain->Render();
	m_pType->Render();
	m_pMap->Render();
	m_pInstrusion->Render();
	m_pTeamBalance->Render();
}