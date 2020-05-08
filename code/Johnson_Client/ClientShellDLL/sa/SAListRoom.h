#ifndef __MURSUM_ROOMLIST_H__
#define __MURSUM_ROOMLIST_H__

#include "LTGUIMgr.h"

#define ROOM_TYPE_TD	0
#define ROOM_TYPE_MC	1

typedef struct _tagROOMITEM
{
	int				nIndex;
	char			szTitle[MAX_R_TITLE_LEN];
	char			szCaptain[MAX_NICK_LEN];
	int				nMapIndex;
	char			szMapName[64];
	int				nCurrentUser;
	int				nMaxUser;
	int				nType;		//TD, MC
	char			szType[64];
	uint8			nPing;
	LTBOOL			bPlaying;
	LTBOOL			bPrivate;
	int				nRoundLimit;
	int				nKillLimit;
	LTBOOL			bInstrusion;
	LTBOOL			bTeamBalance;
	HTEXTURE		hScreenShot;

	CSAD3DText*		pNumber;
	CSAD3DText*		pTitle;
	CSAD3DText*		pMapName;
	CSAD3DText*		pPlayers;
	CSAD3DText*		pGameType;
	CSAD3DText*		pRoomState;

} T_ROOM_ITEM, *T_ROOM_ITEMptr;


	

class CSaListRoom : public CLTGUIListView
{
public:
	CSaListRoom();
	virtual ~CSaListRoom();

	LTBOOL			Create( uint16 nPosX, uint16 nPosY,
							uint16 nWidth, uint16 nHeight,
							uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler = LTNULL,
							LTBOOL	bDrawBack = LTFALSE,
							uint32	nBackColor = 0xFF000000,
							uint32	nBorderColor = 0xFFFFFFFF );
	
	LTBOOL			AddRoom( T_ROOM_ITEMptr pRoomItem );
	LTBOOL			RemoveRoom( int nRoomIndex );

	void			DeleteAllItem();

	T_ROOM_ITEMptr	GetRoomData( int nRoomIndex );
	T_ROOM_ITEMptr	GetSelectedRoomData();
	
	LTBOOL			SetRoomUser( int nRoomIndex, int nMaxUser, int nCurrentUser );
	LTBOOL			SetMaxUser( int nRoomIndex, int nMaxUser );
	LTBOOL			SetRoomState( int nRoomIndex, LTBOOL bPlaying );
	LTBOOL			SetMapName( int nRoomIndex, int nMapIndex, HTEXTURE hTex, const char* lpszMapName, int nGameType, const char* lpszGameType );
	LTBOOL			SetGameLimit( int nRoomIndex, int nTimeLimit, int nKillLimit, int nRoundLimit );
	LTBOOL			SetInstrusion( int nRoomIndex, LTBOOL bEnable );
	LTBOOL			SetTeamBalance( int nRoomIndex, LTBOOL bEnable );
	LTBOOL			SetCaptain( int nRoomIndex, const char* lpszCaptain );
	LTBOOL			SetRoomOption( int nRoomIndex,
								   unsigned char nGameType,
								   unsigned char nMaxUsers,
								   unsigned char nKillLimit, 
								   unsigned char nRoundLimit, 
								   bool bIntrusion, 
								   bool bTeamBalance );
	LTBOOL			SetRoomTitle( int nRoomIndex, const char* lpszTitle );
	void			SetPing( int nRoomIndex, uint8	nPing );

	LTBOOL			IncRoomUser( int nRoomIndex );
	LTBOOL			DecRoomUser( int nRoomIndex );
	
	void			SortRoomNumber();
	void			SortRoomTitle();
	void			SortMapIndex();
	void			SortState( LTBOOL bInit = LTFALSE );
	void			SortUserCount();
	void			SortGameType( LTBOOL bInit = LTFALSE );
	void			SortPing();
		
	void			OnOverChange( int nPreIndex, int nNewIndex );
	
	LTBOOL			OnLButtonDblClick( int x, int y );

	LTBOOL			OnWheelUp( int x, int y );
	LTBOOL			OnWheelDown( int x, int y );
	
protected:
	LTBOOL			RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
	const char*		GetGameTypeText( int nType );

	HTEXTURE		m_hPrivate;

	LTPoly_GT4		m_PolyBar;
	LTPoly_GT4		m_PolyLock;	
	LTPoly_GT4		m_PolyPing;

	LTBOOL			m_bLessNo;
	LTBOOL			m_bLessTitle;
	LTBOOL			m_bLessMap;
	LTBOOL			m_bLessUserCnt;
	LTBOOL			m_bLessType;
	LTBOOL			m_bLessState;
	LTBOOL			m_bLessPing;
};




class CLTGUITooltipRoom : public CLTGUITooltip
{
public:
	CLTGUITooltipRoom();
	virtual ~CLTGUITooltipRoom();
	
	LTBOOL	CreateTooltip( uint16 nWidth, uint16 nHeight );
	void	Term();
	LTBOOL	SetRoomInfo( T_ROOM_ITEMptr pRoomItem );
	void	OnPosChanged();
	
	void	Render();

protected:
	HTEXTURE	m_hBack;
	LTPoly_GT4	m_Poly;

	HTEXTURE	m_hScreenShot;
	LTPoly_GT4	m_PolyScreenShot;

	HTEXTURE	m_hPlay;
	HTEXTURE	m_hWait;
	HTEXTURE	m_hState;
	LTPoly_GT4	m_PolyState;

	CSAD3DText*	m_pTitle;
	CSAD3DText*	m_pCaptain;
	CSAD3DText*	m_pType;
	CSAD3DText*	m_pMap;
	CSAD3DText*	m_pInstrusion;
	CSAD3DText*	m_pTeamBalance;
};

extern CLTGUITooltipRoom* g_pToolTipRoom;

#endif //#ifndef __MURSUM_ROOMLIST_H__
