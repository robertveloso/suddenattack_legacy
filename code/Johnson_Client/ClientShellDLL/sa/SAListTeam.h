#ifndef __MURSUM_TEAMLIST_H__
#define __MURSUM_TEAMLIST_H__

#include "LTGUIMgr.h"

typedef struct _tagTEAMITEM
{
	int			nIndex;
	char		szNick[MAX_NICK_LEN];
	char		szClan[MAX_CLAN_LEN];
	int			nGrade;
	LTBOOL		bCaptain;
	int			nState;
	int			nTeamType;
	HTEXTURE	hMark;
} T_TEAM_ITEM, *T_TEAM_ITEMptr;

class CSaListTeam : public CLTGUIListView
{
public:
	CSaListTeam();
	virtual ~CSaListTeam();

	LTBOOL			Create( uint16 nPosX, uint16 nPosY,
							uint16 nWidth, uint16 nHeight,
							uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler = LTNULL,
							LTBOOL	bDrawBack = LTFALSE,
							uint32	nBackColor = 0xFF000000,
							uint32	nBorderColor = 0xFFFFFFFF );

	LTBOOL			AddUser( T_TEAM_ITEMptr pUserItem );
	LTBOOL			RemoveUser( int nIndex );
	T_TEAM_ITEMptr	GetUserData( int nIndex, int* pnIndex = LTNULL );
	T_TEAM_ITEMptr	GetUserDataFromVecIdx( int nIndex );
	T_TEAM_ITEMptr	GetSelectedUserData();
	
	//[SPIKE] 레디유져 존재 유무판단
	LTBOOL			IsExistReadyUser();

	int				GetPlayerCount();

	LTBOOL			SetUserState( int nIndex, int nState, int nGrade );

	void			ChangeCaptain( int nIndex );

	LTBOOL			OnRButtonDown( int x, int y );
	LTBOOL			OnLButtonDblClick( int x, int y );
	
	void			PrePlayingState();

protected:
	LTBOOL			RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
	void			RenderItemText();
	void			RenderItemText( int nIndex );
				
	CSAD3DText*		m_pIDList;
	CSAD3DText*		m_pClanList;
	
	HTEXTURE		m_hGradeList[MAX_GRADE_TEXTURE];
	HTEXTURE		m_hReady;
	HTEXTURE		m_hPlay;
	HTEXTURE		m_hInven;
	HTEXTURE		m_hMark;
	HTEXTURE		m_hCaptain;
		
	LTPoly_GT4		m_PolyBar;
	LTPoly_GT4		m_PolyGrade;
	LTPoly_GT4		m_PolyCaptain;
	LTPoly_GT4		m_PolyMark;
};

#endif // __MURSUM_TEAMLIST_H__
