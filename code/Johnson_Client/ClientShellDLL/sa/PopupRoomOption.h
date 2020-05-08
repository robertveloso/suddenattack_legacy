#ifndef _POPUP_ROOM_OPTION_H_
#define _POPUP_ROOM_OPTION_H_

#include "ScreenBase.h"
#include "..\\..\\Libs\\LTGUIMgr\\ltguiwindow.h"
#include "PopupScreenBase.h"


#define ROOM_OPT_PLAYERS	0
#define ROOM_OPT_TIME		1
#define ROOM_OPT_KILL		2
#define ROOM_OPT_ROUND		3
#define ROOM_OPT_MAP_IDX	4
#define MAX_ROOM_OPT		5

/* - GameDocument ∑Œ ¿Ãµø..
typedef struct _tagROOMOPTION
{
	int nGameType;
	int	nMapIndex;
	int nMaxUsers;
	int nTimeLimit;
	int nKillLimit;
	int nRoundLimit;
	LTBOOL bInstrusion;
	LTBOOL bTeamBalance;
	bool bFriendlyFire;
} T_ROOM_OPTION, *T_ROOM_OPTIONptr;
*/

typedef struct _tagROOM_OPTIONS
{
	char		szString[64];
	int			nValue;
	CSAD3DText*	pD3DText;
} T_ROOM_OPTIONS, *T_ROOM_OPTIONSptr;

typedef struct _tagROOM_OPTION_MAP
{
	CSAD3DText*	pD3DName;
	CSAD3DText*	pD3DType;
	int			nTextPosX;
	int			nTextPosY;
	int			nMapIndex;
	int			nGameType;
} T_ROOM_OPTION_MAP, *T_ROOM_OPTION_MAPptr;

class CSaListOptions;
class CSaListMap;
struct MISSION;

class CPopRoomOption : public CPopScreenBase
{
public : 
	CPopRoomOption();
	virtual ~CPopRoomOption();

	void		Init( CLTGUICommandHandler*	pCommandHandler );
	void		Render();
	void		OnFocus(LTBOOL bFocus);
	void		CtrlShow( T_ROOM_OPTIONptr pRoomOption );
	LTBOOL		OnLButtonUp(int x, int y);
	LTBOOL		OnWheelUp(int x, int y);
	LTBOOL		OnWheelDown(int x, int y);
	
protected:
	LTBOOL		OnEnter();
	LTBOOL		OnEscape();
	void		ApplyOption();
	void		SetDefaultVal( T_ROOM_OPTIONptr pRoomOption );	
    uint32		OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
		
	//Back
	HTEXTURE				m_hBack;
	LTPoly_GT4				m_Poly;

	CLTGUIEditBox*			m_pEditRoomTitle;

	// Button
	CLTGUIButton*			m_pBtnOK;
	CLTGUIButton*			m_pBtnCancel;

	CLTGUIDragButton*		m_pBtnScroll;
	CLTGUIButton*			m_pBtnScrollUp;
	CLTGUIButton*			m_pBtnScrollDown;

	CLTGUIButton*			m_pBtnMapTypeSort;

	CLTGUIButton*			m_pBtnComboPlayers;
	CLTGUIButton*			m_pBtnComboTime;
	CLTGUIButton*			m_pBtnComboKill;
	CLTGUIButton*			m_pBtnComboRound;

	CLTGUIButton*			m_pCkIntrusionOn;
	CLTGUIButton*			m_pCkIntrusionOff;
	CLTGUIButton*			m_pCkTeamBanlanceOn;
	CLTGUIButton*			m_pCkTeamBanlanceOff;
	CLTGUIButton*			m_pCkFreeSpectatorOn;
	CLTGUIButton*			m_pCkFreeSpectatorOff;
	
	//List
	CSaListOptions*			m_pOptionList;
	CSaListMap*				m_pMapList;
	
	CLTGUICommandHandler*	m_pCommandHandler;

	LTRect					m_rcComboTime;
	LTRect					m_rcComboKill;
	LTRect					m_rcComboRound;
	
	T_ROOM_OPTIONS			m_OptionInfo[MAX_ROOM_OPT];
	T_ROOM_OPTIONptr		m_pCurOption;
};


class CSaListOptions : public CLTGUIListView
{
public:
	CSaListOptions();
	virtual ~CSaListOptions();

	LTBOOL			Create( uint16 nPosX, uint16 nPosY,
							uint16 nWidth, uint16 nHeight,
							uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler = LTNULL,
							LTBOOL	bDrawBack = LTFALSE,
							uint32	nBackColor = 0xFF000000,
							uint32	nBorderColor = 0xFFFFFFFF );	
	LTBOOL			OnLButtonDown(int x, int y);
	void			RenderItemText();
	const char*		GetSelectedString();
	int				GetSelectedValue();
	int				GetType();

	void			ShowOption( int nType, int nValue );
	

protected:
	void			SetDefaultItem( int nType );
	void			AddItem( const char* lpszString, int nValue );
	int				GetSelFromValue( int nValue );
	
	LTBOOL			RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
					
	CSAD3DText*		m_pTextList;
	LTPoly_GT4		m_PolyBar;

	int				m_nCurType;
	int				m_nPreIndex;
};


class CSaListMap : public CLTGUIListView
{
public:
	CSaListMap();
	virtual ~CSaListMap();

	LTBOOL			Create( uint16 nPosX, uint16 nPosY,
							uint16 nWidth, uint16 nHeight,
							uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler = LTNULL,
							LTBOOL	bDrawBack = LTFALSE,
							uint32	nBackColor = 0xFF000000,
							uint32	nBorderColor = 0xFFFFFFFF );
	LTBOOL			OnLButtonDown(int x, int y);
	int				GetSelectedMapIndex();	
	void			SetCurrentIndex( int nIndex );
	void			SortMapType();

protected:
	void			AddItem( MISSION* pMission );
	LTBOOL			RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
	CSAD3DText*		m_pNameList;
	CSAD3DText*		m_pTypeList;

	HTEXTURE		m_hCheck;
	LTPoly_GT4		m_PolyCheck;

	int				m_nPreIndex;
	LTBOOL			m_bLessType;
};


extern CPopRoomOption* g_pPopRoomOption;

#endif // _POPUP_ROOM_OPTION_H_


