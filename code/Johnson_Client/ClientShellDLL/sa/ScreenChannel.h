#ifndef _SCREEN_CHANNLE_H_
#define _SCREEN_CHANNLE_H_

#include "ScreenBase.h"
#include "LayoutMgr.h"

class CSaListChannel;
class CTextScroll;
class CPopupNickName;

class CScreenChannel : public CScreenBase
{
public :
	CScreenChannel();
	virtual ~CScreenChannel();

	LTBOOL  Build();
	void	Term();

	void	OnFocus(LTBOOL bFocus);

	void	SetChannelCount( int nCount );
	void	AddChannel( int nIndex, const char* lpszName, int nCurr, int nMax );
	void	SetNickName( LTBOOL bShow=LTFALSE );
	void	ResponseNickName( int nResult, const char* lpszNick=LTNULL, const char* lpszMsg=LTNULL );
	void	ShowPopupNick();

	virtual void	CreateBackWindow(eScreenID id);	
	void	SetRanking();
	void	SendNickName();

protected :
	LTBOOL   OnUp();
    LTBOOL   OnDown();
    LTBOOL   OnEnter();
	LTBOOL   OnEscape();
	LTBOOL	 Render(HSURFACE hDestSurf);
	LTBOOL	 RenderCloud();
	
	uint32  OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
	virtual void	CreateInterfaceSFX();	
	
	HTEXTURE			m_hTxCloud;
	LTPoly_GT4			m_hPolyCloud;

	CLTGUIButton*		m_pBtnJoin;
	CLTGUIButton*		m_pBtnRefresh;
	CLTGUIButton*		m_pBtnExit;
	
	CSaListChannel*		m_pChannelList;

	int					m_nChannelCount;
	
	HTEXTURE			m_hUIBottomL;
	HTEXTURE			m_hUIBottomR;
	LTPoly_GT4			m_PolyUIBottomL;
	LTPoly_GT4			m_PolyUIBottomR;

	CTextScroll*		m_pTextScroll;

	LTBOOL				m_bShowNickName;

	CPopupNickName*		m_pPopupNickName;

	char				m_szBackupNick[MAX_NICK_LEN];
};

extern CScreenChannel *g_pScreenChannel;

#endif // _SCREEN_CHANNLE_H_