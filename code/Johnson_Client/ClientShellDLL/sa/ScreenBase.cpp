// ScreenBase.cpp: implementation of the CScreenBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScreenBase.h"
#include "ScreenMgr.h"
#include "ScreenCommands.h"
#include "GameClientShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenBase::CScreenBase()
{
	m_pLogoTiniwini = LTNULL;
	m_pLogoSmall = LTNULL;
	m_pBackFrame = LTNULL;
}

CScreenBase::~CScreenBase()
{

}

CLTGUIFrame* CScreenBase::AddFrame(char *szPosTag, HTEXTURE hTexture)
{
	CLTGUIFrame* pFrame = debug_new(CLTGUIFrame);
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);

//	char szStr[128];
//	HTEXTURE hFrame;
	LTRect rect;
	uint32 w,h;
	LTIntPt pos;
	
	pos = g_pLayoutMgr->GetPoint(pScreenTag, szPosTag);
	
	g_pTexInterface->GetTextureDims(hTexture, w, h);
	pFrame->Create(hTexture,w,h,LTTRUE);
	pFrame->SetBasePos(pos);
	
	AddControl(pFrame);
	
	return pFrame;
}
// ±¤½Ä
CLTGUIFrame* CScreenBase::AddFrame(char* szRectTag, char* szTexTag, LTBOOL bRect )
{
	CLTGUIFrame* pFrame = debug_new(CLTGUIFrame);
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);

	char szStr[128];
	HTEXTURE hTexture;
	LTRect rect;
	uint32 w,h;
	LTIntPt pos;	

	rect = g_pLayoutMgr->GetRect(pScreenTag, szRectTag);
	pos.x = rect.left;
	pos.y = rect.top;

	if (szTexTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szTexTag,szStr ,128 );
		hTexture = g_pInterfaceResMgr->GetTexture(szStr);
	}

	if( hTexture )
	{
		g_pTexInterface->GetTextureDims(hTexture, w, h);
		if(!pFrame->Create( hTexture,
			                (rect.right-rect.left),
							(rect.bottom-rect.top),
							((float)(rect.right-rect.left)/(float)w),
							((float)(rect.bottom-rect.top)/(float)h),
							LTTRUE) )
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
	
	AddControl(pFrame);
	
	return pFrame;
}

CLTGUIFrame* CScreenBase::AddFrame(char *szPosTag, char *szTexTag)
{
	CLTGUIFrame* pFrame = debug_new(CLTGUIFrame);
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);

	char szStr[128];
	HTEXTURE hTexture;
	LTRect rect;
	uint32 w,h;
	LTIntPt pos;
	
	pos = g_pLayoutMgr->GetPoint(pScreenTag, szPosTag);
	
	if (szTexTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szTexTag,szStr ,128 );
		hTexture = g_pInterfaceResMgr->GetTexture(szStr);
	}

	if (hTexture)
	{
		g_pTexInterface->GetTextureDims(hTexture, w, h);
		if (!pFrame->Create(hTexture,w,h,LTTRUE))
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
	
	AddControl(pFrame);
	
	return pFrame;
}

CLTGUIFrame* CScreenBase::AddFrameRect(char *szRectTag, char *szTexTag,LTBOOL bStretch)
{
	CLTGUIFrame* pFrame = debug_new(CLTGUIFrame);
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);

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
	
	AddControl(pFrame);
	
	return pFrame;
}

// ±¤½Ä
CLTGUIButton* CScreenBase::AddButton( uint32 nCommandID,
									  char* szRectTag,
									  char* szNormalTxTag,
									  char* szSelectTxTag,
									  char* szDisableTxTag,
									  char* szClickTxTag,
									  LTBOOL bApply )
{
	CLTGUIButton* pButton = debug_new(CLTGUIButton);

	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);

	char szStr[128];
	HTEXTURE hTexture = LTNULL, hTextureH = LTNULL, hTextureD = LTNULL, hTextureC = LTNULL, hCheckTexture = LTNULL;
	LTIntPt pos;
	LTRect rect;
	uint32 w, h;

	// Get Rect
	rect = g_pLayoutMgr->GetRect(pScreenTag, szRectTag);

	// Get Normal Texture
	if (szNormalTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szNormalTxTag, szStr ,128 );
		hTexture =  g_pInterfaceResMgr->GetTexture(szStr);
		g_pTexInterface->GetTextureDims(hTexture, w, h);
	}

	// Get Select Texture
	if (szSelectTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szSelectTxTag, szStr ,128 );
		hTextureH =  g_pInterfaceResMgr->GetTexture(szStr);
	}

	// Get Disable Texture
	if (szDisableTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szDisableTxTag, szStr ,128 );
		hTextureD =  g_pInterfaceResMgr->GetTexture(szStr);
	}
	// Get Click Texture
	if (szClickTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szClickTxTag, szStr, 128);
		hTextureC = g_pInterfaceResMgr->GetTexture(szStr);
	}

	float width	 = float(rect.right - rect.left);
	float height = float(rect.bottom - rect.top);	
	
	if( !pButton->Create(nCommandID, LTNULL, width/(float)w, height/(float)h, hTexture, hTextureH, hTextureD, hTextureC, this) )
	{
		debug_delete(pButton);
		return LTNULL;
	}

	pButton->SetRect(rect);
	pButton->SetScale(g_pInterfaceResMgr->GetXRatio());

	AddControl(pButton);

	pButton->SetOverSound( (int)IS_OVER );
	pButton->SetClickSound( (int)IS_CLICK_NORMAL );

	return pButton;
}

CLTGUIButton* CScreenBase::AddButton(uint32 nCommandID, char *szRectTag, char *szNormalTxTag, char *szHoverTxTag, uint32 nHelpID, char *szSelectTxTag, char *szDisableTxTag)
{
	CLTGUIButton* pButton = debug_new(CLTGUIButton);
	
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);

	char szStr[128];
	HTEXTURE hTexture = LTNULL, hTextureH = LTNULL, hTextureD = LTNULL, hCheckTexture = LTNULL;
	LTIntPt pos;
	LTRect rect;
	uint32 w, h;
	
	// Get Rect
	rect = g_pLayoutMgr->GetRect(pScreenTag, szRectTag);

	// Get Normal Texture
	if (szNormalTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szNormalTxTag, szStr ,128 );
		hTexture =  g_pInterfaceResMgr->GetTexture(szStr);
		g_pTexInterface->GetTextureDims(hTexture, w, h);
	}

	// Get Hover Texture
	if (szHoverTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szHoverTxTag, szStr ,128 );
		hTextureH =  g_pInterfaceResMgr->GetTexture(szStr);
	}

	// Get Selected Texture
	if (szSelectTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szSelectTxTag, szStr ,128 );
		hCheckTexture = g_pInterfaceResMgr->GetTexture(szStr);
		pButton->SetCheckedTexture(hCheckTexture);
	}

	// Get Disable Texture
	if (szDisableTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szDisableTxTag, szStr ,128 );
		hTextureD =  g_pInterfaceResMgr->GetTexture(szStr);
	}

	float width	 = float(rect.right - rect.left);
	float height = float(rect.bottom - rect.top);
	
	if( !pButton->Create(nCommandID, LTNULL, width/(float)w, height/(float)h, hTexture, hTextureH, LTNULL, this) )
	//if (!pButton->Create(nCommandID, nHelpID, hTexture, hTextureH, hTextureD, this))
	{
		debug_delete(pButton);
		return LTNULL;
	}

	pButton->SetRect(rect);
	pButton->SetScale(g_pInterfaceResMgr->GetXRatio());

	AddControl(pButton);

	pButton->SetOverSound( (int)IS_NONE );
	pButton->SetClickSound( (int)IS_CLICK_RADIO );
	
	return pButton;
}

//[MURSUM]================================================================================
CLTGUIDragButton* CScreenBase::AddDragButton( uint32 nCommandID,
											  char* szRectTag,
											  char* szNormalTxTag,
											  char* szHoverTxTag, 
											  uint32 nHelpID,
											  char* szSelectTxTag, 
											  char* szDisableTxTag )
{
	CLTGUIDragButton* pButton = debug_new(CLTGUIDragButton);
	
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);

	char szStr[128];
	HTEXTURE hTexture = LTNULL, hTextureH = LTNULL, hTextureD = LTNULL, hCheckTexture = LTNULL;
	LTIntPt pos;
	LTRect rect;
	
	// Get Rect
	rect = g_pLayoutMgr->GetRect(pScreenTag, szRectTag);

	// Get Normal Texture
	if (szNormalTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szNormalTxTag, szStr ,128 );
		hTexture =  g_pInterfaceResMgr->GetTexture(szStr);
	}

	// Get Hover Texture
	if (szHoverTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szHoverTxTag, szStr ,128 );
		hTextureH =  g_pInterfaceResMgr->GetTexture(szStr);
	}

	// Get Selected Texture
	if (szSelectTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szSelectTxTag, szStr ,128 );
		hCheckTexture = g_pInterfaceResMgr->GetTexture(szStr);
		pButton->SetCheckedTexture(hCheckTexture);
	}

	// Get Disable Texture
	if (szDisableTxTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szDisableTxTag, szStr ,128 );
		hTextureD =  g_pInterfaceResMgr->GetTexture(szStr);
	}

	if (!pButton->Create(nCommandID, nHelpID, hTexture, hTextureH, hTextureD, this))
	{
		debug_delete(pButton);
		return LTNULL;
	}

	pButton->SetRect(rect);
	pButton->SetScale(g_pInterfaceResMgr->GetXRatio());

	AddControl( pButton );

	return pButton;
}
//========================================================================================


/*
CLTGUIButton* CScreenBase::AddButton(CLTGUIButton * pButton, uint32 nCommandID, uint32 nHelpID, char* pPosTag, char * pPosAtt, char * pTexTag, char * pTexAtt, char * pTexHAtt)
{
	char szStr[128];
	HTEXTURE hTexture, hTextureH;
	LTIntPt pos;
	
	pos = g_pLayoutMgr->GetPoint(pPosTag, pPosAtt);
	g_pLayoutMgr->GetString(pTexTag, pTexAtt,szStr ,128 );
	hTexture =  g_pInterfaceResMgr->GetTexture(szStr);
	g_pLayoutMgr->GetString(pTexTag, pTexHAtt,szStr ,128 );
	hTextureH =  g_pInterfaceResMgr->GetTexture(szStr);
	pButton->Create(nCommandID, nHelpID, hTexture, hTextureH, LTNULL, this);
//---------------------------------------------------------------------------
// bestline
	uint32 w,h;
	g_pTexInterface->GetTextureDims(hTexture,w,h);
	pButton->SetSize(w,h);	
//---------------------------------------------------------------------------
	pButton->SetBasePos(pos);
	pButton->SetScale(g_pInterfaceResMgr->GetXRatio());

	AddControl(pButton);
	return pButton;
}


CLTGUIButton* CScreenBase::AddButtonR(CLTGUIButton * pButton, uint32 nCommandID, uint32 nHelpID, char* pPosTag, char * pPosAtt, char * pTexTag, char * pTexAtt, char * pTexHAtt)
{
	char szStr[128];
	HTEXTURE hTexture, hTextureH;
	LTIntPt pos;
	LTRect rect;
	
	rect = g_pLayoutMgr->GetRect(pPosTag, pPosAtt);
	g_pLayoutMgr->GetString(pTexTag, pTexAtt,szStr ,128 );
	hTexture =  g_pInterfaceResMgr->GetTexture(szStr);
	g_pLayoutMgr->GetString(pTexTag, pTexHAtt,szStr ,128 );
	hTextureH =  g_pInterfaceResMgr->GetTexture(szStr);
	pButton->Create(nCommandID, nHelpID, hTexture, hTextureH, LTNULL, this);
	pButton->SetRect(rect);
//	pButton->SetSize(rect.right-rect.left, rect.bottom-rect.top);
//	pos.x = rect.left;
//	pos.y = rect.top;
//	pButton->SetBasePos(pos);
	pButton->SetScale(g_pInterfaceResMgr->GetXRatio());
	AddControl(pButton);
	return pButton;
}
*/

CLTGUIFrame* CScreenBase::AddFrame(CLTGUIFrame * pFrame, char* pPosTag, char * pPosAtt, char * pTexTag, char * pTexAtt)
{
	char szStr[128];
	HTEXTURE hFrame;
	LTRect rect;
	uint32 w,h;
	LTIntPt pos;
	
	pos = g_pLayoutMgr->GetPoint(pPosTag, pPosAtt);
	
	g_pLayoutMgr->GetString(pTexTag, pTexAtt,szStr ,128 );
	hFrame = g_pInterfaceResMgr->GetTexture(szStr);

	g_pTexInterface->GetTextureDims(hFrame, w, h);
	if (!pFrame->Create(hFrame,w,h,LTTRUE))
		return LTNULL;

	pFrame->SetBasePos(pos);
	
	AddControl(pFrame);
	
	return pFrame;
}

void CScreenBase::CreateLogos()
{
	m_pLogoSmall = AddFrame("LogoSmallPos", "LogoSmallTx");
	m_pLogoTiniwini = AddFrame("LogoTiniwiniPos", "LogoTiniwiniTx");
}

HTEXTURE CScreenBase::GetTexture(char *szTextureTag)
{
	HTEXTURE hTexture = NULL;
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);
	char szStr[128];
	g_pLayoutMgr->GetString(pScreenTag, szTextureTag, szStr ,128 );
	hTexture = g_pInterfaceResMgr->GetTexture(szStr);
	return hTexture;
}

CLTGUICount* CScreenBase::AddCount(char* szRectTag, char *szTexTag, uint8 nDigit)
{
	CLTGUICount* pCount= debug_new(CLTGUICount);
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);

	char szStr[128];
	HTEXTURE hTexture = LTNULL;
	LTIntPt pos;
	LTRect rect;
	
	// Get Rect
	rect = g_pLayoutMgr->GetRect(pScreenTag, szRectTag);
	pos.x = rect.left;
	pos.y = rect.top;

	// Get Normal Texture
	if (szTexTag)
	{
		g_pLayoutMgr->GetString(pScreenTag, szTexTag, szStr ,128 );
		hTexture =  g_pInterfaceResMgr->GetTexture(szStr);
	}

	if (!pCount->Create(pos, rect.right-rect.left, rect.bottom-rect.top, hTexture, nDigit))
		return LTNULL;

	pCount->SetScale(g_pInterfaceResMgr->GetXRatio());

	AddControl(pCount);

	return pCount;
}

CLTGUICount* CScreenBase::AddCount(char* szRectTag, HTEXTURE hTexture, uint8 nDigit)
{
	CLTGUICount* pCount= debug_new(CLTGUICount);
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)m_nScreenID);

//	char szStr[128];
	LTIntPt pos;
	LTRect rect;
	
	// Get Rect
	rect = g_pLayoutMgr->GetRect(pScreenTag, szRectTag);
	pos.x = rect.left;
	pos.y = rect.top;

	if (!pCount->Create(pos, rect.right-rect.left, rect.bottom-rect.top, hTexture, nDigit))
		return LTNULL;

	pCount->SetScale(g_pInterfaceResMgr->GetXRatio());

	AddControl(pCount);

	return pCount;
}

void CScreenBase::CreateBackFrame(eScreenID id)
{
	LTIntPt pos, pos1(0,0);
	char szStr[128] = {0, };
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName( id );
	g_pLayoutMgr->GetString(pScreenTag, "BGTexture", szStr, 128 );
	pos.x = (int)((float)g_pInterfaceResMgr->GetScreenWidth()/g_pInterfaceResMgr->GetXRatio());
	pos.y = (int)((float)g_pInterfaceResMgr->GetScreenHeight()/g_pInterfaceResMgr->GetYRatio());
	HTEXTURE hFrameTex	  = g_pInterfaceResMgr->GetTexture(szStr);
	m_pBackFrame = debug_new(CLTGUIFrame);
	m_pBackFrame->Create(hFrameTex,pos.x,pos.y,TRUE);
	m_pBackFrame->SetBasePos(pos1);
	AddControl(m_pBackFrame);
	
	CLTGUIFrame* pBackFrame0 = AddFrame("BackFrame0TxPos", "BGTexture_0");
	CLTGUIFrame* pBackFrame1 = AddFrame("BackFrame1TxPos", "BGTexture_1");
	CLTGUIFrame* pBackFrame2 = AddFrame("BackFrame2TxPos", "BGTexture_2");
	CLTGUIFrame* pBackFrame3 = AddFrame("BackFrame3TxPos", "BGTexture_3");
	/*
	CLTGUIFrame* pBackFrame0 = AddFrame("BackFrame0TxPos", "BGTexture_0", 1.0f, 1.0f);
	CLTGUIFrame* pBackFrame1 = AddFrame("BackFrame1TxPos", "BGTexture_1", 288.0f/512.0f, 1.0f);
	CLTGUIFrame* pBackFrame2 = AddFrame("BackFrame2TxPos", "BGTexture_2", 1.0f, 88.0f/128.0f);
	CLTGUIFrame* pBackFrame3 = AddFrame("BackFrame3TxPos", "BGTexture_3", 228.0f/512.0f, 88.0f/128.0f);
	*/
}

void CScreenBase::CreateBackFrame(int x, int y)
{
	LTIntPt pos;
	LTIntPt pos1(x,y);
	// Back Frame
	char szFrame[128];
	g_pLayoutMgr->GetScreenCustomString((eScreenID)m_nScreenID,"BGTexture",szFrame,sizeof(szFrame));
	pos.x = (int)((float)g_pInterfaceResMgr->GetScreenWidth()/g_pInterfaceResMgr->GetXRatio()); // = g_pLayoutMgr->GetScreenCustomPoint((eScreenID)m_nScreenID, "PageRect");
	pos.y = (int)((float)g_pInterfaceResMgr->GetScreenHeight()/g_pInterfaceResMgr->GetYRatio()); // = g_pLayoutMgr->GetScreenCustomPoint((eScreenID)m_nScreenID, "PageRect");
	HTEXTURE hFrame = g_pInterfaceResMgr->GetTexture(szFrame);
	m_pBackFrame = debug_new(CLTGUIFrame);
	//m_pBackFrame->Create(hFrame,pos.x,pos.y,TRUE);
	m_pBackFrame->Create(hFrame, pos.x, pos.y, 0.78125f, 0.5859f, TRUE);

	m_pBackFrame->SetBasePos(pos1);
	TRACE("%s(%d) CreateBackFrame... PosX(%d) PosY(%d) ScreenWidth(%d) ScreenHeight(%d)\n", __FILE__, __LINE__, x, y, pos.x, pos.y);

	AddControl(m_pBackFrame);
}

void CScreenBase::CreateBackWindow(eScreenID id)
{
	// »õ·Î ¸¸µç back Àº 9Àå.
	LTIntPt pos, pos1(0,0);
	char szStr[128] = {0, };
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName( id );
	g_pLayoutMgr->GetString(pScreenTag, "BGTexture", szStr, 128 );
	HTEXTURE hFrameTex	  = g_pInterfaceResMgr->GetTexture(szStr);
	pos.x = (int)((float)g_pInterfaceResMgr->GetScreenWidth()/g_pInterfaceResMgr->GetXRatio());
	pos.y = (int)((float)g_pInterfaceResMgr->GetScreenHeight()/g_pInterfaceResMgr->GetYRatio());
	m_pBackFrame = debug_new(CLTGUIFrame);
	m_pBackFrame->Create(hFrameTex,pos.x,pos.y,TRUE);
	m_pBackFrame->SetBasePos(pos1);
	AddControl(m_pBackFrame);

	CLTGUIFrame* pBackFrame0 = AddFrame("BackFrameTxPos_0", "BGTexture_0");
	CLTGUIFrame* pBackFrame1 = AddFrame("BackFrameTxPos_1", "BGTexture_1");
	CLTGUIFrame* pBackFrame2 = AddFrame("BackFrameTxPos_2", "BGTexture_2");
	CLTGUIFrame* pBackFrame3 = AddFrame("BackFrameTxPos_3", "BGTexture_3");
	CLTGUIFrame* pBackFrame4 = AddFrame("BackFrameTxPos_4", "BGTexture_4");
	CLTGUIFrame* pBackFrame5 = AddFrame("BackFrameTxPos_5", "BGTexture_5");
	CLTGUIFrame* pBackFrame6 = AddFrame("BackFrameTxPos_6", "BGTexture_6");
	CLTGUIFrame* pBackFrame7 = AddFrame("BackFrameTxPos_7", "BGTexture_7");
	CLTGUIFrame* pBackFrame8 = AddFrame("BackFrameTxPos_8", "BGTexture_8");
}