// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenMain.h
//
// PURPOSE : Top level interface screen
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef _SCREEN_MAIN_H_
#define _SCREEN_MAIN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScreenBase.h"
#include "BaseScaleFX.h"
#include "GameClientShell.h"

class CPopScreenCreateRoom;
class CPopScreenOption;
class CPopupHelp; //[yaiin]
class CPopupPassword;
class CPopupMenu;
class CPopupInviteMsg;

//[MURSUM]==============================================
class CSaListRoom;
class CSaListUser;
class CSaListText;
class CSaListFriend;
class CLTGUITooltipRoom;
class CPopupSearchUser;
class CPopupSearchResult;
class CPopupUserInfo;
class CTextScroll;
//======================================================

class CScreenMain : public CScreenBase
{
public:
	CScreenMain();
	virtual ~CScreenMain();

	// Build the screen
    LTBOOL			Build();
	void			BuildUserInfo( uint16 nPosX, uint16 nPosY, uint16 nWidth, uint16 nHeight );

	// This is called when the screen gets or loses focus
    virtual void    OnFocus(LTBOOL bFocus);
	virtual void    Escape();
	
	void			SortRoomList();
	void			OnRecvComplete( int nJoinRoomIndex=-1 );
	
	void			ShowPasswordDlg();
	virtual void	CreateBackWindow(eScreenID id);	

	LTBOOL			OnLButtonDown(int x, int y);

	void			ShowInviteMsg( const char* lpszNick, int nRoomIndex, const char* lpszPass );
			
	// Model
	//----------------------------------------------
	void	UpdateModel(int nModelID, int HeadSkinID, int nBodySkinID );
	//----------------------------------------------

protected:
	LTBOOL	OnEnter();
	LTBOOL	OnEscape();
	//virtual void	CreateInterfaceSFX() {};
    uint32			OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
    LTBOOL			Render(HSURFACE hDestSurf);
	LTBOOL			RenderCloud();
	LTBOOL			RenderUserInfo();
	
	bool			CheckPassword();
	
// SPIKE		>> NOVICE 
	HTEXTURE		m_hTxCloud;
	LTPoly_GT4		m_hPolyCloud;

	CLTGUIButton*	m_pBtnQuickJoin;
	CLTGUIButton*	m_pBtnCreateRoom;
	CLTGUIButton*	m_pBtnInventory;
	CLTGUIButton*	m_pBtnOption;
	CLTGUIButton*	m_pBtnHelp; //[yaiin]

	CLTGUIButton*	m_pBtnShop;
	CLTGUIButton*	m_pBtnClan;
	CLTGUIButton*	m_pBtnExit;

	CLTGUIButton*	m_pBtnUser;
	CLTGUIButton*	m_pBtnFriend;
	CLTGUIButton*	m_pBtnFind;

	CLTGUIButton*	m_pBtnLH_1;
	CLTGUIButton*	m_pBtnLH_2;
	CLTGUIButton*	m_pBtnLH_3;
	CLTGUIButton*	m_pBtnLH_4;
	CLTGUIButton*	m_pBtnLH_5;
	CLTGUIButton*	m_pBtnLH_6;
	CLTGUIButton*	m_pBtnLH_7;

	CLTGUIFrame*	m_pFmRoomList;
	CLTGUIFrame*	m_pFmOption;
	
// <==

//-----------------------------------------------------------------------

	//[MURSUM]==============================================
	CLTGUIDragButton*	m_pBtnRoomScroll;
	CLTGUIButton*		m_pBtnRoomScrollUp;
	CLTGUIButton*		m_pBtnRoomScrollDown;
	CSaListRoom*		m_pRoomList;

	CLTGUIDragButton*	m_pBtnUserScroll;
	CLTGUIButton*		m_pBtnUserScrollUp;
	CLTGUIButton*		m_pBtnUserScrollDown;
	CSaListUser*		m_pUserList;
	CSaListFriend*		m_pFriendList;

	CLTGUIDragButton*	m_pBtnTextScroll;
	CLTGUIButton*		m_pBtnTextScrollUp;
	CLTGUIButton*		m_pBtnTextScrollDown;
	CSaListText*		m_pTextList;

	CLTGUIEditBox*		m_pChatEdit;
	//=======================================================

public:	
	//[MURSUM]==============================================
	void ClearScreenMain();
	void AddRoom( int nIndex,
				  const char* lpszTitle,
				  const char* lpszCaptain,
				  int nMapIndex,
				  int nCurrentUser,
				  int nMaxUser,
				  int nType,
				  unsigned char nPing,
				  LTBOOL bPlaying,
				  BOOL bPrivate,
				  int nRoundLimit,
				  int nKillLimit,
				  BOOL bInstrusion,
				  BOOL bTeamBalance );

	void RemoveRoom( int nRoomIndex );

	void AddUser( int nIndex, const char* lpszNick, uint32 nGrade );	
	void RemoveUser( int nIndex );

	void AddFriend( int nIndex, const char* lpszNick, uint32 nGrade, uint16 nStatus );
	void RemoveFriend( int nIndex );

	void AddChat( DWORD dwIdx, int32 nType=CHAT_TYPE_NORMAL );
	void AddChat( const char *lpszChat, int32 nType=CHAT_TYPE_NORMAL );
	void AddNotify( const char* lpszNoti );

	void SetUserInfo( uint32 nGrade, const char* lpszNick,
					  uint32 nKill, uint32 nDeath, uint32 nWin, 
					  uint32 nLose, uint32 nDraw, __int64 nExp, __int64 nMoney );

	void UserInfoReply( const char* lpszNick, uint32 nGrade,
					    uint32 nKill, uint32 nDeath,
						uint32 nWin, uint32 nLose, uint32 nDraw,
						__int64 nExp,
						uint8 nA_ModelID, uint8 nB_ModelID );
	
	void ChangeOption( int nRoomIndex, 
					   unsigned char nGameType,
					   unsigned char nMapIndex,
					   unsigned char nMaxUsers, 
					   unsigned char nTimeLimit,
					   unsigned char nKillLimit, 
					   unsigned char nRoundLimit, 
					   bool bIntrusion,
					   bool bTeamBalance,
					   bool bFreeSpec );
	void ChangeRoomTitle( int nRoomIndex, const char* lpszTitle );
	
	void ChangeOptMapIndex( int nRoomIndex, uint8 nMapIndex, uint8 nGameType );
	void ChangeOptMaxUsers( int nRoomIndex, short nMaxUsers );
	void ChangeGameLimit( int nRoomIndex, unsigned char nTimeLimit, unsigned char nKillLimit, unsigned char nRoundLimit );
	void ChangeOptInstrusion( int nRoomIndex, LTBOOL bEnable );
	void ChangeOptTeamBalance( int nRoomIndex, LTBOOL bEnable );
	void SetPing( int nRoomIndex, uint8 nPing );
	void IncRoomUser( int nRoomIndex );
	void DecRoomUser( int nRoomIndex );
	void ChangeRoomState( int nRoomIndex, LTBOOL bPlaying );
	void ChangeCaptain( int nRoomIndex, const char* lpszCaptain );
	void ShowListTab();
	void ShowOptionTab();
	LTBOOL EditCommand( const char* lpszChatMessage );

	LTBOOL JoinRoom( void* pRoomItem );
	LTBOOL JoinRoom( int nChannelIndex, int nRoomIndex=-1 );
	LTBOOL JoinPrivateRoom( void* pRoomItem );
	
	void SearchResult( LTBOOL bSuccess, const char* lpszMsg, int nChannelIndex, int nRoomIndex );
	
	void	PrePopupState();
	void	PostPopupState();

	LTBOOL	IsMissionMode( GameType eGameType );
	
	void	EnterShop();
	void	SetRanking();
	//=======================================================

	void	CheckDoBae();


	//-------------------------------------------------
	// ±¤½Ä [popup screen test]
	//-------------------------------------------------
	int m_nCreateRoomIndex;
	CPopScreenCreateRoom*	m_pPopupWindow;
	//-------------------------------------------------

	//SPIKE---
	CPopScreenOption*	m_pPopupOption;
	//---

	//[yaiin]
	CPopupHelp*			m_pPopupHelp;
	//---

	CPopupPassword*		m_pPopupPassword;
	CPopupSearchUser*	m_pPopupSearchUser;
	CPopupSearchResult*	m_pPopupSearchResult;
	CPopupUserInfo*		m_pPopupUserInfo;
	CPopupMenu*			m_pPopupMenu;
	CPopupInviteMsg*	m_pInviteMsg;


	//[MURSUM]
	CLTGUITooltipRoom*	m_pTooltipRoom;
	
	int					m_nChatCount;
	float				m_fChatTime;
	float				m_fDoBae;
	LTBOOL				m_bDisableChat;

	CSAD3DText*			m_pChannelInfo;
	HTEXTURE			m_hUIBottomL;
	HTEXTURE			m_hUIBottomR;
	LTPoly_GT4			m_PolyUIBottomL;
	LTPoly_GT4			m_PolyUIBottomR;

	CTextScroll*		m_pTextScroll;
};

#endif // _SCREEN_MAIN_H_

#ifdef	SCREENMAIN 
CScreenMain *g_pScreenMain;
#else
extern CScreenMain *g_pScreenMain;

#endif