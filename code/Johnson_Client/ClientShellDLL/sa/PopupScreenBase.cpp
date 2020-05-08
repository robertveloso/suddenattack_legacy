#include "stdafx.h"
#include "ScreenCommands.h"
#include "PopupScreenBase.h"

CPopScreenBase::CPopScreenBase()
{
	m_pParentScreen	= LTNULL;
}

CPopScreenBase::~CPopScreenBase()
{
}

void CPopScreenBase::CtrlShow()
{
	if( m_pParentScreen ) m_pParentScreen->EnablePopup(LTTRUE, (CLTGUICtrl*)this);
	OnFocus(LTTRUE);
	Show(LTTRUE);
}

void CPopScreenBase::CtrlHide()
{
	if( m_pParentScreen ) m_pParentScreen->EnablePopup(LTFALSE, LTNULL );
	OnFocus(LTFALSE);
	Show(LTFALSE);
}

void CPopScreenBase::SetParentScreen( CScreenBase* pScreen )
{
	m_pParentScreen	= pScreen;
}

CLTGUIFrame* CPopScreenBase::AddFrameRect(char *pScreenTag, char *szRectTag, char *szTexTag,LTBOOL bStretch)
{
	CLTGUIFrame* pFrame = debug_new(CLTGUIFrame);
	//char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);

	char szStr[128];
	HTEXTURE hTexture(LTNULL);
	LTRect rect;
	uint32 w,h;
	
	rect = g_pLayoutMgr->GetRect(pScreenTag, szRectTag);
	LTIntPt pos(rect.left, rect.top);
	
	if (szTexTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szTexTag,szStr ,128 );
		hTexture = g_pInterfaceResMgr->GetTexture(szStr);
	}

	if (hTexture)
	{
		g_pTexInterface->GetTextureDims(hTexture, w, h);
		if (!pFrame->Create(hTexture,w,h,bStretch))
		{
			debug_delete(pFrame);
			return LTNULL;
		}
	}
	else
	{
		w=rect.right-rect.left;
		h=rect.bottom-rect.top;
		if (!pFrame->Create(0xFFFFFFFF,w,h))
		{
			debug_delete(pFrame);
			return LTNULL;
		}
	}
	pFrame->SetBasePos(pos);
	pFrame->SetSize(rect.right-rect.left,rect.bottom-rect.top);
	
	AddControl(pFrame, pos);
	
	return pFrame;
}

LTBOOL CPopScreenBase::AddFrame( char* pScreenTag, CLTGUIFrame* pFrame, char* szPosTag, HTEXTURE hTexture )
{
	if( pFrame == LTNULL ) return LTFALSE;

	LTIntPt pos;
	uint32 w,h;
	g_pTexInterface->GetTextureDims(hTexture, w, h);
	if( !pFrame->Create(hTexture, w, h, LTTRUE) )
	{
		return LTFALSE;
	}
	pos = g_pLayoutMgr->GetPoint(pScreenTag, szPosTag);
	pFrame->SetBasePos(pos);
	AddControl(pFrame, pos);

	return LTTRUE;
}

LTBOOL CPopScreenBase::AddFrame (char* pScreenTag, CLTGUIFrame* pFrame, char* szPosTag, char* szTextureTag )
{
	char szStr[128] = {0, };
	g_pLayoutMgr->GetString(pScreenTag, szTextureTag, szStr, 128 );
	HTEXTURE hTexture	  = g_pInterfaceResMgr->GetTexture(szStr);
	return AddFrame(pScreenTag, pFrame, szPosTag, hTexture);
}

LTBOOL CPopScreenBase::AddFrame(char* pScreenTag, CLTGUIFrame* pFrame, char* szTextureRectTag, char* szRenderRectTag, char* szPosTag, char* szTextureTag)
{
	if( pFrame == LTNULL ) return LTFALSE;
	
	char szStr[128] = {0, };
	g_pLayoutMgr->GetString(pScreenTag, szTextureTag, szStr, 128 );
	HTEXTURE hTexture	  = g_pInterfaceResMgr->GetTexture(szStr);
	LTRect rcFrameTexRect = g_pLayoutMgr->GetRect(pScreenTag, szTextureRectTag);
	LTRect rcFrame		  = g_pLayoutMgr->GetRect( pScreenTag, szRenderRectTag);
	LTIntPt TxPos		  = g_pLayoutMgr->GetPoint(pScreenTag, szPosTag);
	float xRate = (float)rcFrame.right / (float)rcFrameTexRect.right;
	float yRate = (float)rcFrame.bottom / (float)rcFrameTexRect.bottom;

	pFrame->Create(hTexture, rcFrame.right, rcFrame.bottom, xRate, yRate, LTTRUE);
	pFrame->SetBasePos(TxPos);

	AddControl(pFrame, TxPos);

	return LTTRUE;
}

void CPopScreenBase::CreateBackeFrame( eScreenID nScreen )
{
	char szStr[128] = {0, };
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName( nScreen );
	
	/*
	g_pLayoutMgr->GetString(pScreenTag, "BGTexture", szStr, 128 );
	HTEXTURE hFrameTex	  = g_pInterfaceResMgr->GetTexture(szStr);
	LTRect rcFrameTexRect = g_pLayoutMgr->GetRect(pScreenTag, "BGTextureRect");
	LTRect rcFrame		  = g_pLayoutMgr->GetRect( pScreenTag, "BGRect");
	float xRate = (float)rcFrame.right / (float)rcFrameTexRect.right;
	float yRate = (float)rcFrame.bottom / (float)rcFrameTexRect.bottom;
	Create(hFrameTex, rcFrame.right, rcFrame.bottom, xRate, yRate, LTTRUE);	
	LTIntPt basePos		  = g_pLayoutMgr->GetPoint(pScreenTag, "BGBasePos");
	SetBasePos(basePos);	
	*/
	g_pLayoutMgr->GetString(pScreenTag, "BGTexture", szStr, 128 );
	HTEXTURE hFrameTex	  = g_pInterfaceResMgr->GetTexture(szStr);
	LTRect rcFrame		  = g_pLayoutMgr->GetRect(pScreenTag, "BGRect");
	Create(hFrameTex, rcFrame.right, rcFrame.bottom, LTTRUE);
	LTIntPt basePos		  = g_pLayoutMgr->GetPoint(pScreenTag, "BGBasePos");
	SetBasePos(basePos);
	Show(LTFALSE);
	
	CLTGUIFrame* pBackFrame1	= new CLTGUIFrame();
	AddFrame(pScreenTag, pBackFrame1, "BackFrame1TxPos", "BGTexture_1");
	CLTGUIFrame* pBackFrame2	= new CLTGUIFrame();
	AddFrame (pScreenTag, pBackFrame2, "BackFrame2TxRect", "BackFrame2TxRender", "BackFrame2TxPos", "BGTexture_2" );
	CLTGUIFrame* pBackFrame3	= new CLTGUIFrame();
	AddFrame (pScreenTag, pBackFrame3, "BackFrame3TxRect", "BackFrame3TxRender", "BackFrame3TxPos", "BGTexture_3" );	
	CLTGUIFrame* pBackFrame4	= new CLTGUIFrame();
	AddFrame (pScreenTag, pBackFrame4, "BackFrame4TxRect", "BackFrame4TxRender", "BackFrame4TxPos", "BGTexture_4" );
}

LTBOOL CPopScreenBase::AddCount(char* pScreenTag, CLTGUICount* pCount, char* szRectTag, char* szTexTag, uint8 nDigit )
{
	if( pCount == LTNULL ) return LTFALSE;

	LTIntPt pos;
	LTRect	rect;
	rect = g_pLayoutMgr->GetRect( pScreenTag, szRectTag );
	pos.x = rect.left;
	pos.y = rect.top;
	HTEXTURE hTextureCnt = g_pInterfaceResMgr->GetTexture( TX_COUNT );
	if( !pCount->Create( pos, rect.right-rect.left, rect.bottom-rect.top, hTextureCnt,2) ) // 2 자리까지 표시
	{
		return LTFALSE;
	}

	pCount->SetScale( g_pInterfaceResMgr->GetXRatio() );
	AddControl(pCount, pos);

	return LTTRUE;
}

LTBOOL CPopScreenBase::AddButton(char* pScreenTag, CLTGUIButton* pButton, uint32 nCommandID, char* NormalTx, char* SelectedTx, char* pBtnRect)
{
	if( pButton == LTNULL ) return LTFALSE;
	
	char szStr[128] = {0, };
	HTEXTURE hBtnNormalTx = LTNULL;
	HTEXTURE hBtnSelectedTx = LTNULL;
	if(NormalTx)
	{
		g_pLayoutMgr->GetString( pScreenTag, NormalTx, szStr, 128 );
		hBtnNormalTx = g_pInterfaceResMgr->GetTexture(szStr);
	}
	if(SelectedTx)
	{
		g_pLayoutMgr->GetString( pScreenTag, SelectedTx, szStr, 128 );
		hBtnSelectedTx = g_pInterfaceResMgr->GetTexture(szStr);
	}
	if( !pButton->Create(nCommandID, 0, hBtnNormalTx, hBtnSelectedTx, LTNULL, this) )
	{
		return LTFALSE;
	}

	LTRect BtnRect = g_pLayoutMgr->GetRect(pScreenTag, pBtnRect);
	LTIntPt BtnPos;
	BtnPos.x = BtnRect.left;
	BtnPos.y = BtnRect.top;
	pButton->SetRect(BtnRect);
	pButton->SetScale(g_pInterfaceResMgr->GetXRatio());
	AddControl(pButton, BtnPos);

	pButton->SetOverSound( (int)IS_OVER );
	pButton->SetClickSound( (int)IS_CLICK_FLAT );

	return LTTRUE;
}

LTBOOL CPopScreenBase::AddButton(char* pScreenTag,
								 CLTGUIButton* pButton,
								 uint32 nCommandID,
								 char* szNormalTx,
								 char* szSelectedTx,
								 char* szDisabledTx,
								 char* szClickedTx,
								 char* pBtnRect)
{
	if( pButton == LTNULL ) return LTFALSE;

	LTIntPt BtnPos;
	uint32 w, h;
	LTRect BtnRect = g_pLayoutMgr->GetRect(pScreenTag, pBtnRect);
	char szStr[128] = {0, };

	g_pLayoutMgr->GetString( pScreenTag, szNormalTx, szStr, 128 );
	HTEXTURE hBtnNormalTx = g_pInterfaceResMgr->GetTexture(szStr);
	if( hBtnNormalTx )
	{
		g_pTexInterface->GetTextureDims(hBtnNormalTx, w, h);
	}

	g_pLayoutMgr->GetString( pScreenTag, szSelectedTx, szStr, 128 );
	HTEXTURE hBtnSelectedTx = g_pInterfaceResMgr->GetTexture(szStr);

	g_pLayoutMgr->GetString( pScreenTag, szDisabledTx, szStr, 128 );
	HTEXTURE hBtnDisabledTx = g_pInterfaceResMgr->GetTexture(szStr);

	g_pLayoutMgr->GetString( pScreenTag, szClickedTx, szStr, 128 );
	HTEXTURE hBtnClickedTx = g_pInterfaceResMgr->GetTexture(szStr);


	float width	 = float(BtnRect.right - BtnRect.left);
	float height = float(BtnRect.bottom - BtnRect.top);	
	
	if( !pButton->Create(nCommandID, LTNULL,
		width/(float)w, height/(float)h, hBtnNormalTx, hBtnSelectedTx, hBtnDisabledTx, hBtnClickedTx, this) )
	{
		debug_delete(pButton);
		return LTNULL;
	}
	
	BtnPos.x = BtnRect.left;
	BtnPos.y = BtnRect.top;
	pButton->SetRect(BtnRect);
	pButton->SetScale(g_pInterfaceResMgr->GetXRatio());
	AddControl(pButton, BtnPos);

	pButton->SetOverSound( (int)IS_OVER );
	pButton->SetClickSound( (int)IS_CLICK_FLAT );

	return LTTRUE;
}

LTBOOL CPopScreenBase::AddCheckButton(char* pScreenTag, CLTGUIButton* pButton, uint32 nCommandID, char* szRectTag, char* szNormalTxTag, char* szSelectTxTag)
{
	if( pButton == LTNULL ) return LTFALSE;

	char szStr[128] = {0, };
	LTIntPt pos;
	LTRect  rect;
	rect = g_pLayoutMgr->GetRect(pScreenTag, szRectTag);
	pos.x = rect.left;
	pos.y = rect.top;
	g_pLayoutMgr->GetString(pScreenTag, szNormalTxTag, szStr ,128 );
	HTEXTURE hOffTexture =  g_pInterfaceResMgr->GetTexture(szStr);
	g_pLayoutMgr->GetString(pScreenTag, szSelectTxTag, szStr ,128 );
	HTEXTURE hOnTexture =  g_pInterfaceResMgr->GetTexture(szStr);
	pButton->SetCheckedTexture(hOnTexture);
	if( !pButton->Create(nCommandID, 0, hOffTexture, LTNULL, LTNULL, this) )
	{
		return LTFALSE;
	}
	pButton->SetRect(rect);
	pButton->SetScale(g_pInterfaceResMgr->GetXRatio());
	AddControl(pButton, pos);

	pButton->SetOverSound( (int)IS_NONE );
	pButton->SetClickSound( (int)IS_CLICK_RADIO );
	
	return LTTRUE;
}

LTBOOL CPopScreenBase::AddSlider(char* pScreenTag, CLTGUISlider* pSlider, LTIntPt pos, char* pString, int helpID, int nHeaderWidth, int nBarWidth, int nBarHeight, int* pnValue)
{
	if( pSlider == LTNULL ) return LTFALSE;

	char szStr[128] = {0, };
	g_pLayoutMgr->GetSliderTex(szStr,128);
	HTEXTURE hBar = g_pInterfaceResMgr->GetTexture(szStr);
	
	if( !pSlider->Create(helpID, hBar, nHeaderWidth, nBarWidth, nBarHeight, pnValue) )
	{
		return LTFALSE;
	}

	pSlider->SetBasePos(pos);
	pSlider->SetColors(argbWhite,argbBlack,argbGray);
	AddControl(pSlider, pos);

	return LTTRUE;
}


LTBOOL
CPopScreenBase::AddDragButton(char* pScreenTag, CLTGUIDragButton* pButton, uint32 nCommandID, char* NormalTx, char* SelectedTx, char* pBtnRect)
{
	if( pButton == LTNULL ) return LTFALSE;

	char szStr[128] = {0, };
	g_pLayoutMgr->GetString( pScreenTag, NormalTx, szStr, 128 );
	HTEXTURE hBtnNormalTx = g_pInterfaceResMgr->GetTexture(szStr);
	g_pLayoutMgr->GetString( pScreenTag, SelectedTx, szStr, 128 );
	HTEXTURE hBtnSelectedTx = g_pInterfaceResMgr->GetTexture(szStr);
	
	if( !pButton->Create(nCommandID, 0, hBtnNormalTx, hBtnSelectedTx, LTNULL, this) )
	{
		return LTFALSE;
	}

	LTRect BtnRect = g_pLayoutMgr->GetRect(pScreenTag, pBtnRect);
	LTIntPt BtnPos;
	BtnPos.x = BtnRect.left;
	BtnPos.y = BtnRect.top;
	pButton->SetRect(BtnRect);
	pButton->SetScale(g_pInterfaceResMgr->GetXRatio());
	AddControl(pButton, BtnPos);

	return LTTRUE;
}

void CPopScreenBase::PlaySelectSound( CLTGUICtrl* pCtrl )
{
	if( pCtrl && pCtrl->IsPlaySound() )
		g_pInterfaceMgr->RequestInterfaceSound( (InterfaceSound)pCtrl->GetOverSound() );	
}

void CPopScreenBase::PlayClickSound( CLTGUICtrl* pCtrl )
{
	if( pCtrl && pCtrl->IsPlaySound() )
		g_pInterfaceMgr->RequestInterfaceSound( (InterfaceSound)pCtrl->GetClickSound() );	
}