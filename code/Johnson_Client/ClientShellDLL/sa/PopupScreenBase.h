#ifndef _POPUP_SCREEN_BASE_H_
#define _POPUP_SCREEN_BASE_H_

#include "ScreenBase.h"
#include "..\\..\\Libs\\LTGUIMgr\\ltguiwindow.h"
#include "../ClientShellShared/HanText.h"

class CPopScreenBase : public CLTGUIWindow
{
public :
	CPopScreenBase();
	virtual ~CPopScreenBase();

	virtual void		Init()	{}

	virtual void		OnFocus(LTBOOL bFocus) {}
	void		CtrlShow();
	void		CtrlHide();

	void		SetParentScreen( CScreenBase* pScreen );


	void		PlaySelectSound( CLTGUICtrl* pCtrl );
	void		PlayClickSound( CLTGUICtrl* pCtrl );

	virtual void		CreateBackeFrame( eScreenID nScreen );
	virtual LTBOOL		AddButton(char* pScreenTag, CLTGUIButton* pButton, uint32 nCommandID, char* NormalTx, char* SelectedTx, char* pBtnRect);
	virtual LTBOOL		AddButton(char* pScreenTag, CLTGUIButton* pButton, uint32 nCommandID, char* szNormalTx, char* szSelectedTx, char* szDisabledTx, char* szClickedTx, char* pBtnRect);
	virtual LTBOOL		AddDragButton(char* pScreenTag, CLTGUIDragButton* pButton, uint32 nCommandID, char* NormalTx, char* SelectedTx, char* pBtnRect);
	virtual LTBOOL		AddCount (char* pScreenTag, CLTGUICount* pCount, char* szRectTag, char* szTexTag, uint8 nDigit );
	virtual LTBOOL		AddFrame (char* pScreenTag, CLTGUIFrame* pFrame, char* szPosTag, HTEXTURE hTexture );
	virtual LTBOOL		AddFrame (char* pScreenTag, CLTGUIFrame* pFrame, char* szPosTag, char* szTextureTag );
	virtual LTBOOL		AddFrame (char* pScreenTag, CLTGUIFrame* pFrame, char* szTextureRectTag, char* szRenderRectTag, char* szPosTag, char* szTextureTag );
	virtual CLTGUIFrame* AddFrameRect(char *pScreenTag, char *szRectTag, char *szTexTag,LTBOOL bStretch);
	virtual LTBOOL		AddCheckButton(char* pScreenTag, CLTGUIButton* pButton, uint32 nCommandID, char* szRectTag, char* szNormalTxTag, char* szSelectTxTag);
	virtual LTBOOL		AddSlider(char* pScreenTag, CLTGUISlider* pSlider, LTIntPt pos, char* pString, int helpID, int nHeaderWidth, int nBarWidth, int nBarHeight, int* pnValue = LTNULL);

protected:
	CScreenBase*		m_pParentScreen;
};

#endif // _POPUP_SCREEN_BASE_H_