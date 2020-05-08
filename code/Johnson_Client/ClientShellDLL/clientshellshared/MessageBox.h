// ----------------------------------------------------------------------- //
//
// MODULE  : MessageBox.h
//
// PURPOSE : Handle the display of a simple message box
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#if !defined(_MESSAGEBOX_H_)
#define _MESSAGEBOX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LTGUIMgr.h"

enum eMBType
{	
	LTMB_OK,
	LTMB_YESNO,
	LTMB_MESSAGE,
	LTMB_EDIT,		// 삭제 
	LTMB_WEAPON,	// 삭제
};

//edit boxes return a pointer to their strings in pData
//YesNo and OK boxes return their passed data untouched
typedef void (*MBCallBackFn)(LTBOOL bReturn, void *pData);



typedef struct MBCreate_t
{
	eMBType			eType;
	MBCallBackFn	pFn;
	void*			pData;

	MBCreate_t() 	{	eType = LTMB_OK;	pFn = LTNULL;	pData = LTNULL;		};
} MBCreate;

class CMsgDlg : public CLTGUIWindow
{
public :
	CMsgDlg();
	virtual ~CMsgDlg();

	void		PlaySelectSound( CLTGUICtrl* pCtrl );
	void		PlayClickSound( CLTGUICtrl* pCtrl );
};

class CMessageBox : CLTGUICommandHandler
{
public:
	CMessageBox();
	virtual ~CMessageBox();

    LTBOOL	Init();
	void	Term();

	void	Draw();

    void	Show(int nStringID, MBCreate* pCreate, uint8 nFontSize = 0, LTBOOL bDefaultReturn = LTTRUE, LTBOOL bIgnoreAfter = LTFALSE );
    void	Show(const char *pString, MBCreate* pCreate, uint8 nFontSize = 0, LTBOOL bDefaultReturn = LTTRUE, LTBOOL bIgnoreAfter = LTFALSE );

    void	Close(LTBOOL bReturn);
	
	//[MURSUM]====================================================
	void	ScreenDimsChanged();
	//============================================================

    LTBOOL	HandleKeyDown(int key, int rep);
    LTBOOL	HandleChar(unsigned char c);
    LTBOOL	OnLButtonDown(int x, int y);
    LTBOOL	OnLButtonUp(int x, int y);
    LTBOOL	OnMouseMove(int x, int y);

    LTBOOL	IsVisible() {return m_bVisible;}
	LTBOOL	IsIgnoreAfter() {return m_bIgnoreAfter;}
	void	SetIgnoreAfter( LTBOOL bIgnoreAfter ) { m_bIgnoreAfter = bIgnoreAfter; }

	CLTGUIWindow	GetMessageWindow()		{ return m_Dlg; }
	void			SetMessageBackTexture(HTEXTURE	hTexture);

protected:

	void ApplyCarriageReturnToLongSentence();
	
	uint32 OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);

	uint32 OnCommand_CmdOk( uint32 dwParam1, uint32 dwParam2 );
	uint32 OnCommand_CmdCancel( uint32 dwParam1, uint32 dwParam2 );
	uint32 OnCommand_CmdMessageOk( uint32 dwParam1, uint32 dwParam2 );
	uint32 OnCommand_CmdMessageCancel( uint32 dwParam1, uint32 dwParam2 );
	
	//////////////////////////////////////////////////////////////////////////
	// NOVICE
	enum EN_BTN_TEXTURE						// Yes / No 버튼..
	{
		BTN_NORMAL		= 0,
		BTN_HIGHLIGHT	= 1,
		BTN_DISABLED	= 2,
		BTN_CLICKED		= 3,
	};

	enum EN_MSGBOX_BACK_FRAME
	{
		MBF_TOP					= 0,
		MBF_MIDDLE				= 1,
		MBF_BOTTOM				= 2,
		MBF_BOTTOM_ONLY_NOTICE	= 3,
	};
	
	HTEXTURE	m_hYes[4];
	HTEXTURE	m_hNo[4];
	LTRect		m_rtYes;
	LTRect		m_rtNo;	
	//////////////////////////////////////////////////////////////////////////
	
	LTBOOL		m_bIgnoreAfter;

	float		m_fTextGap;
	int			m_nControlInterval;		// YES 와 NO 사이 간격

	float		m_fOffsetXOfAllPoly;	// 폴리곤 위치

	float		m_fOffsetYOfTopPoly;
	float		m_fOffsetYOfMidPoly;
	float		m_fOffsetYOfBotPoly;
	
	float		m_fWidthOfAllPoly;		// 폴리곤 넓이
	
	float		m_fHeightOfTopPoly;		// 폴리곤 높이	
	float		m_fHeightOfMidPoly;
	float		m_fHeightOfBotPoly;
	float		m_fHeightOfAllPoly;
	
	HTEXTURE	m_TxMsgBoxBackFrame[4];
	LTPoly_GT4	m_PolyMsgBoxBackFrame[4];

	//----------------------------------------------------
	CMsgDlg	    m_Dlg;
	
	CLTGUIButton*		m_pBtnOk;
	CLTGUIButton*		m_pBtnCancel;
	

	eMBType				m_eType;
	LTBOOL				m_bOnlyMessage;
    LTBOOL              m_bVisible;
    LTBOOL              m_bGameWasPaused;
	MBCallBackFn		m_pCallback;
	void*				m_pData;

	CSAD3DText*			m_pMsgFont;
	int					m_nWidth;
	int					m_nHeight;
	int					m_nStrLen;

	char				m_szStrBuf[1024];
	
	LTPoly_GT4			m_PolyBack;

};

#endif // _MESSAGEBOX_H_