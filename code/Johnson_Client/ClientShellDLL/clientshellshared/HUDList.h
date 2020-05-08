#ifndef __MURSUM_HUDLIST_H__
#define __MURSUM_HUDLIST_H__

#include "HUDItem.h"

#define HEADSHOT_MSG_W	100
#define KILL_MSG_NICK_W	128
#define KILL_MSG_TEMP_W	10

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _tagHUDLIST
{
	int32	nBasePosX;
	int32	nBasePosY;
	float	fLifeTime;
} T_HUD_LIST, *T_HUD_LISTptr;

typedef std::vector<T_HUD_LISTptr> HUD_LIST;
typedef HUD_LIST::iterator HUD_LIST_ITR;

class CHUDList : public CHUDItem
{
public:
	CHUDList();
	
	virtual LTBOOL      Init( int32 nPosX, int32 nPosY, int32 nItemWidth, int32 nItemHeight, int32 nMaxItemCount, float fLifeTime );
	virtual void		Term();
	virtual void		Clear();
    virtual void        Render() {}
    virtual void        Update();

	virtual LTIntPt		AddListItem( float fLifeTime = -1.0f );

	void				ScreenDimChanged();
	

protected:
	virtual void		SetPosition() {}
	virtual void		DeleteMessage();
	virtual void		CheckDeleteMessage( float fCurTime );
	
	HUD_LIST			m_HudList;
	
	int32				m_nMaxItemCount;
	float				m_fLifeTime;

	int32				m_nOriginPosX;
	int32				m_nOriginPosY;
	int32				m_nPosX;
	int32				m_nPosY;
	int32				m_nItemWidth;
	int32				m_nItemHeight;

	float				m_fLastCheckTime;

	LTIntPt				m_TempPt;
};
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _tagCHATMSG
{
	CSAD3DTextEx*	pText;
	int32			nFontSize;
	float			fAlpha;
} T_CHAT_MSG, *T_CHAT_MSGptr;

typedef std::vector<T_CHAT_MSGptr> CHAT_LIST;
typedef CHAT_LIST::iterator CHAT_LIST_ITR;

class CHUDChatMsg : public CHUDList
{
public:
	CHUDChatMsg();
	LTBOOL				Init( int32 nPosX, int32 nPosY, int32 nItemWidth, int32 nItemHeight, int32 nMaxItemCount, float fLifeTime );
	void				Term();	
	void				Update();
    void				Render();
	void				Clear();
	void				ScreenDimChanged();
	
	virtual void		AddMessage( const char* lpszMessage, eChatMsgType type=kMsgDefault );
	virtual void		AddLine( const char* lpszMessage, eChatMsgType type );
	HDC					GetDC();
	LTBOOL				ReleaseDC( HDC hDC );

protected:	
	void				CheckDeleteMessage( float fCurTime );
	virtual void		SetPosition();
	virtual void		SetData( const char* lpszMessage, eChatMsgType type, CSAD3DTextEx* pText );
	void				DeleteMessage();

	CHAT_LIST			m_ChatMsg;
	
	CSAD3DText*			m_pTempText;
	
	int					m_nBottomPosY;
	float				m_fStartAlpha;
};
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
class CHUDHelpMsg : public CHUDChatMsg
{
public:
	CHUDHelpMsg();
	LTBOOL				Init( int32 nPosX, int32 nPosY, int32 nItemWidth, int32 nItemHeight, int32 nMaxItemCount, float fLifeTime );
	void				Term();
	void				AddMessage( const char* lpszMessage, int nFontSize, eChatMsgType type, float fLifeTime );
	void				AddLine1( const char* lpszMessage, int nFontSize, eChatMsgType type, float fLifeTime );
	void				AddLine2( const char* lpszMessage, int nFontSize, eChatMsgType type, float fLifeTime );
	void				ScreenDimChanged();
	
protected:
	void				SetData( const char* lpszMessage, eChatMsgType type, CSAD3DTextEx* pText );
	void				SetPosition();
	
	char				m_szTokBuffer[1024];
};
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _tagKILLMSG
{
	CSAD3DText	HeadShot;
	CSAD3DText	Killer;
	CSAD3DText	Victim;
	HTEXTURE	hTexture;
	LTPoly_GT4	Poly;
	uint32		nTexW;
	uint32		nTexH;
	uint32		nBaseX;
	uint32		nHeadShotLen;
	uint32		nKillerLen;
	uint32		nVictimLen;
} T_KILL_MSG, *T_KILL_MSGptr;

typedef std::vector<T_KILL_MSGptr> KILL_LIST;
typedef KILL_LIST::iterator KILL_LIST_ITR;

class CHUDKillMsg : public CHUDList
{
public:
	CHUDKillMsg();
	LTBOOL				Init( int32 nPosX, int32 nPosY, int32 nItemWidth, int32 nItemHeight, int32 nMaxItemCount, float fLifeTime );
	void				Term();	
	void				Update();
    void				Render();
	void				Clear();
	void				ScreenDimChanged();

	void				AddMessage( const char* lpszKiller,
									int32 nKillerTeam,
									const char* lpszVictim,
									int32 nVictimTeam,
									const char* lpszWeaponName,
									int32 nHitNode,
									const char* lpszDeathType=LTNULL );
	
protected:	
	void				SetPosition();
	void				SetData( const char* lpszKiller,
								 int32 nKillerTeam,
								 const char* lpszVictim,
								 int32 nVictimTeam,
								 const char* lpszWeaponName,
								 int32 nHitNode,
								 T_KILL_MSGptr pKill,
								 LTIntPt* pPt,
								 const char* lpszDeathType );
	void				DeleteMessage();

	KILL_LIST			m_KillMsg;
};
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _tagCHAR_NICK
{
	bool			bRender;
	HOBJECT			hObj;
	CSAD3DTextEx*	pNick;
	float			fTextW;
	float			fTextHalfW;

	CSAD3DTextEx*	pRadio;
	LTBOOL			bRenderRadio;
	float			fRadioLifeTime;
	float			fRadioTextW;
} T_CHAR_NICK, *T_CHAR_NICKptr;

typedef std::vector<T_CHAR_NICKptr> NICK_LIST;
typedef NICK_LIST::iterator NICK_LIST_ITR;

class CHUDNickList : public CHUDItem
{
public:
	CHUDNickList();
	LTBOOL				Init();
	void				Term();	
	void				Clear();
	void				Update( HOBJECT hObj, LTVector* pPos );
	void				Update() {};
    void				Render();
	void				AddNick( HOBJECT hObj,
								 const char* lpszNick,
								 int nTeamID );
	void				RemoveNick( HOBJECT hObj );
	void				ScreenDimChanged();

	void				SetRadioMsg( HOBJECT hObj, const char* lpszMsg );
	void				SetVisible( LTBOOL bVisible ) { m_bVisible = bVisible; }
	LTBOOL				IsVisible() { return m_bVisible; }

protected:	

	NICK_LIST	m_NickList;
	
	LTBOOL		m_bVisible;

	float		m_fMinX;
	float		m_fMaxX;
	float		m_fMinY;
	float		m_fMaxY;
};
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
typedef struct _tagPICKUPMSG
{
	CSAD3DText	Msg;
	HTEXTURE	hTexture;
	LTPoly_GT4	Poly;
	uint32		nTexW;
	uint32		nTexH;
} T_PICKUP_MSG, *T_PICKUP_MSGptr;

typedef std::vector<T_PICKUP_MSGptr> PICKUP_LIST;
typedef PICKUP_LIST::iterator PICKUP_LIST_ITR;

class CHUDPickupMsg : public CHUDList
{
public:
	CHUDPickupMsg();
	LTBOOL				Init( int32 nPosX, int32 nPosY, int32 nItemWidth, int32 nItemHeight, int32 nMaxItemCount, float fLifeTime );
	void				Term();	
	void				Update();
    void				Render();
	void				AddMessage( const char* lpszMessage, const char* lpszIconName );
	void				Clear();
	void				ScreenDimChanged();

protected:	
	void				SetPosition();
	void				SetData( const char* lpszMessage, const char* lpszIconName, T_PICKUP_MSGptr pPickup, LTIntPt* pPt );
	void				DeleteMessage();

	PICKUP_LIST			m_PickupMsg;
};
*/
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif