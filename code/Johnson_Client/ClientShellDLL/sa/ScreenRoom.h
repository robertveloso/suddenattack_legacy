#ifndef _SCREEN_ROOM_H_
#define _SCREEN_ROOM_H_

#include "ScreenBase.h"
#include "SaListText.h"
#include "SaListTeam.h"
#include "PopupRoomOption.h"

class CSAMapList;
class CPopupSearchResult;
class CPopupUserInfo;
class CPopupInvite;
class CTextScroll;
class CPopupMenu;
class CPopupHelp; //[yaiin]

class CScreenRoom : public CScreenBase
{
	public :
		CScreenRoom();
		virtual ~CScreenRoom();

		// Build the screen
		LTBOOL  Build();
		void	Term();
		void	OnFocus(LTBOOL bFocus);
		void	Escape();

		void	BuildUserInfo( uint16 nPosX, uint16 nPosY, uint16 nWidth, uint16 nHeight );
		void	BuildRoomInfo( uint16 nPosX, uint16 nPosY, uint16 nWidth, uint16 nHeight );

		virtual void	CreateBackWindow(eScreenID id);
		
		LTBOOL   OnLButtonDown(int x, int y);
		LTBOOL   OnMouseMove(int x, int y);
		LTBOOL   OnLButtonUp(int x, int y);
		
		void	StartHost();
		LTBOOL	HandleLaunch();
		LTBOOL	LaunchGame();				
		
		virtual bool UpdateInterfaceSFX();

		LTBOOL	Render(HSURFACE hDestSurf);
		LTBOOL	RenderUserInfo();
		LTBOOL	RenderRoomInfo();

		// UI 다시 작업
		//----------------------------------------------------
		CSaListTeam*		GetBlueTeamUser() { return m_pListBlueTeam; }
		CSaListTeam*		GetRedTeamUser()  { return m_pListRedTeam; }
		void	AddChat( DWORD dwIdx, int32 nType=CHAT_TYPE_NORMAL );
		void	AddChat( const char* lpszChat, int32 nType=CHAT_TYPE_NORMAL );
		void	AddNotify( const char* lpszNoti );
		void	AddRoomUser( int nIndex, const char* szNick, int nGrade, int nTeam, int nState, LTBOOL bCaptain );
		void	ChangeTeam( int nIndex, int nTeam );
		void	RemoveRoomUser( int nIndex );
		void	EnterBufferRoom( LTBOOL bValue );
		void	SetRecvUserCount( int nCount, int nMyTeam, int nState );
		
		void	RemoveAllTeam();
		void	SetUserInfo( uint32 nGrade, 
							 const char* lpszNick,
							 uint32 nKill,
							 uint32 nDeath, 
							 uint32 nWin, uint32 nLose, uint32 nDraw,
							 __int64 nExp,
							 __int64 nGameMoney );
		
		void	UserInfoReply( const char* lpszNick, uint32 nGrade,
							   uint32 nKill, uint32 nDeath,
							   uint32 nWin, uint32 nLose, uint32 nDraw,
							   __int64 nExp,
							   uint8 nA_ModelID, uint8 nB_ModelID );

		void	SetStartReadyButton();

		void ChangeOption( unsigned char nGameType,
					   unsigned char nMapIndex,
					   unsigned char nMaxUsers, 
					   unsigned char nTimeLimit,
					   unsigned char nKillLimit, 
					   unsigned char nRoundLimit, 
					   bool bIntrusion,
					   bool bTeamBalance,
					   bool bFreeSpec );
		void	ChangeRoomTitle( const char* lpszTitle );
		void	ChangeOptMapIndex( uint8 nMapIndex, uint8 nGameType );
		void	ChangeOptMaxUsers( short nMaxUsers );
		void	ChangeGameLimit( unsigned char nTimeLimit, unsigned char nKillLimit, unsigned char nRoundLimit );
		void	ChangeOptInstrusion( LTBOOL bEnable );
		void	ChangeOptTeamBalance( LTBOOL bEnable );
		void	StartGameAsClient( const char* szIP, int nPort, bool bSpectator );
		void	CaptainChange( int nIndex );
		//----------------------------------------------------
		// Model
		//----------------------------------------------
		void	UpdateModel(int nModelID, int HeadSkinID, int nBodySkinID );
		//----------------------------------------------

		// NOVICE ____________________________________________
		void	SetSelectedValue();
		//____________________________________________________

		//[MURSUM]=============================================================
		void	EndGame();
		void	SetInstrution( LTBOOL bInstrution );
		void	PrePlaying();
		LTBOOL	EditCommand( const char* lpszChatMessage );
		void	ShowExitError();
		void	ShowEnterInvenError();
		void	EnterInven( int nIndex, bool bEnter );
		void	SetPlayerState( int nIndex, int nState, int nGrade );
		void	SearchResult( LTBOOL bSuccess, const char* lpszMsg );

		void	PrePopupState();
		void	PostPopupState();
		void	SetPlayerReady( int nIndex, LTBOOL bReady );
		void	ChangeRoomState( LTBOOL bPlaying );
		void	RecvComplete();

		void	SetLobbyUserCount( int nCount );
		void	AddLobbyUser( const char* lpszNick, uint8 nGrade );
		void	SetRanking();
		//=====================================================================

		//[SPIKE]===================
		LTBOOL	AreYouReady(){return m_bReady;}

	protected :
		LTBOOL	RenderCloud();
		LTBOOL	OnEnter();
		LTBOOL	OnEscape();
		uint32  OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
		LTBOOL	IsMissionMode( GameType eGameType );
		void	CheckDoBae();

		CLTGUIButton*		m_pBtnCancel;		
		//-----------------------------
		// UI 다시 작업
		//-----------------------------

		HTEXTURE		m_hTxCloud;
		LTPoly_GT4		m_hPolyCloud;

		HTEXTURE		m_hTxMiniMap;
		LTPoly_GT4		m_hPolyMiniMap;

		HTEXTURE			m_hStartIcon;
		LTPoly_GT4			m_PolyStartIcon;
		LTBOOL				m_bShowStartIcon;
		float				m_fStartIconLastUpdate;
		LTBOOL				m_bDrawStartIcon;

		HTEXTURE			m_hReadyBtnTexture[8];
		CLTGUIButton*		m_pBtnReady;
		CLTGUIButton*		m_pBtnBack;
		CLTGUIButton*		m_pBtnInventory;
		CLTGUIButton*		m_pBtnInvite;
		CLTGUIButton*		m_pBtnRoomOption;
		CLTGUIButton*		m_pBtnArrowTeamLeft;
		CLTGUIButton*		m_pBtnArrowTeamRight;
		CLTGUIButton*		m_pBtnRoomHelp; //[yaiin]
		
		char				m_szChatBuffer[512];
		LTBOOL				m_bReady;
		CLTGUIFrame*		m_pMapFrame;

		// NOVICE ________________________________________
		// 유저 정보
		CSAD3DText*			m_pInfoNick;
		CSAD3DText*			m_pInfoGrade;
		CSAD3DText*			m_pInfoWinPercent;
		CSAD3DText*			m_pInfoKillDeath;
		CSAD3DText*			m_pInfoExp;
		CSAD3DText*			m_pInfoCash;
		CSAD3DText*			m_pInfoGameMoney;
		
		CSAD3DText*			m_pInfoNickCap;
		CSAD3DText*			m_pInfoGradeCap;
		CSAD3DText*			m_pInfoWinPercentCap;
		CSAD3DText*			m_pInfoKillDeathCap;
		CSAD3DText*			m_pInfoExpCap;
		CSAD3DText*			m_pInfoCashCap;
		CSAD3DText*			m_pInfoGameMoneyCap;
		//________________________________________________

		// NOVICE ________________________________________
		// 방 옵션 정보
		CSAD3DText*			m_pPlayers;				// 인원수
		CSAD3DText*			m_pGameType;			// 게임타입
		CSAD3DText*			m_pGTGoal;				// 
		CSAD3DText*			m_pGTTimeLimit;			// 게임시간 제한
		CSAD3DText*			m_pIntrusion;			// 전투중 참여
		CSAD3DText*			m_pTeamBalance;			// 팀 밸런스
		
		CSAD3DText*			m_pPlayersCap;
		CSAD3DText*			m_pGameTypeCap;
		CSAD3DText*			m_pGTGoalCap;
		CSAD3DText*			m_pGTTimeLimitCap;
		CSAD3DText*			m_pIntrusionCap;
		CSAD3DText*			m_pTeamBalanceCap;
		
		CSAD3DText*			m_pMapTitle;
		CLTGUIButton*		m_pScrollUp;
		CLTGUIButton*		m_pScrollDown;
		CLTGUIDragButton*	m_pDragBtn;
		CSAMapList*			m_pMapComboBox;
		CLTGUIButton*		m_pBtnMapDown;
		int					m_nMapTotalCount;


		T_ROOM_OPTION		m_RoomOption;
		LTBOOL				m_bIsInstrusion;
		//________________________________________________

		//[MURSUM]========================================
		HTEXTURE			m_hGradeIcon;
		LTPoly_GT4			m_PolyGrade;
		HLTSOUND			m_hBGSound;
		int					m_nUpdateCount;
		LTBOOL				m_bRecvComplete;
		//================================================

		CSAD3DText*			m_pRoomName;
		
		// chatting
		CLTGUIDragButton*	m_pBtnTextScroll;
		CLTGUIButton*		m_pBtnTextScrollUp;
		CLTGUIButton*		m_pBtnTextScrollDown;
		CSaListText*		m_pTextList;
		CLTGUIEditBox*		m_pChatEdit;

		CSaListTeam*		m_pListBlueTeam;
		CSaListTeam*		m_pListRedTeam;

		CPopupSearchResult*	m_pPopupSearchResult;

		// Option
		CPopRoomOption*		m_pPopupRoomOption;
		CLTGUIFrame*		m_pViewGameType;	// frame
		HTEXTURE			m_hGTIcon[2];
		LTPoly_GT4			m_PolyGT;

		CPopupInvite*		m_pPopupInvite;
		CPopupUserInfo*		m_pPopupUserInfo;
		CPopupMenu*			m_pPopupMenu;
		CPopupMenu*			m_pPopupMenuCaptain;

		CPopupHelp*			m_pPopupRoomHelp; //[yaiin]
		//-----------------------------
		int					m_nCurrentModel;
		int					m_nCurrentSkinID;
		//---------------------------------------------
		
		int					m_nRecvPlayerCount;
		//[SPIKE]
		LTBOOL				m_bForceExitTimeStart;

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

		int					m_nInviteUserCount;

		HTEXTURE			m_hLightButton;
		LTPoly_GT4			m_PolyLightButton;
		float				m_fAlpha;
		LTBOOL				m_bIncrease;

};

extern CScreenRoom *g_pScreenRoom;

#endif // _SCREEN_ROOM_H_