// ScreenBase.h: interface for the CScreenBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENBASE_H__F26B1699_CCC9_4390_AAD7_F76E1DDE7296__INCLUDED_)
#define AFX_SCREENBASE_H__F26B1699_CCC9_4390_AAD7_F76E1DDE7296__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InterfaceMgr.h"
#include "BaseScreen.h"
#include "ControlConstant.h"


#define CHAT_TYPE_NORMAL	0
#define CHAT_TYPE_WHISPER	1
#define CHAT_TYPE_TEAM		2
#define CHAT_TYPE_ADMIN		3
#define CHAT_TYPE_NOTICE	4

//////////////////////////////////////////////////////////////////////////

class CScreenBase : public CBaseScreen 
{
public:
	HTEXTURE GetTexture(char* szTextureTag);
	void CreateLogos();
	
	CLTGUIFrame*  AddFrame(CLTGUIFrame * pFrame, char * pPosTag, char * pPosAtt, char * pTexTag, char * pTexAtt); 

	// ±¤½Ä
	CLTGUIButton* AddButton(uint32 nCommandID, char* szRectTag, char* szNormalTxTag, char* szHoverTxTag, uint32 nHelpID = LTNULL, char* szSelectTxTag = LTNULL, char* szDisableTxTag = LTNULL);
	CLTGUIButton* AddButton(uint32 nCommandID, char* szRectTag, char* szNormalTxTag, char* szSelectTxTag, char*szDisableTxTag, char* szClickTxTag, LTBOOL bApply );
	//[MURSUM]===================================================================
	CLTGUIDragButton* AddDragButton( uint32 nCommandID,
									 char* szRectTag,
									 char* szNormalTxTag,
									 char* szHoverTxTag,
									 uint32 nHelpID = LTNULL,
									 char* szSelectTxTag = LTNULL,
									 char* szDisableTxTag = LTNULL );
	//===========================================================================

	CLTGUIFrame* AddFrame(char* szPosTag, HTEXTURE hTexture);
	CLTGUIFrame* AddFrame(char* szPosTag, char* szTexTag);
	//--------------------------------------------------------------------------------
	// ±¤½Ä
	//--------------------------------------------------------------------------------
	//CLTGUIFrame* AddFrame(char* szPosTag, char* szTexTag, float xRate, float yRate);
	CLTGUIFrame* AddFrame(char* szRectTag, char* szTexTag, LTBOOL bRect );
	//--------------------------------------------------------------------------------
	CLTGUIFrame* AddFrameRect(char *szRectTag, char *szTexTag,LTBOOL bStretch = LTTRUE);
	CLTGUICount* AddCount(char* szRectTag, HTEXTURE hTexture, uint8 nDigit = 1);
	CLTGUICount* AddCount(char* szRectTag, char* szTexTag, uint8 nDigit = 1);

	void CreateBackFrame(eScreenID id);
	void CreateBackFrame(int x = 0, int y = 0);
	virtual void CreateBackWindow(eScreenID id);

	CScreenBase();
	virtual ~CScreenBase();

protected:
	CLTGUIFrame*	m_pBackFrame;
	CLTGUIFrame*	m_pLogoTiniwini;
	CLTGUIFrame*	m_pLogoSmall;
};

#endif
