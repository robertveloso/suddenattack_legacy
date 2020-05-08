// ----------------------------------------------------------------------- //
//
// MODULE  : LTGUIButton.cpp
//
// PURPOSE : button control with three states (normal, selected, and disabled)
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "ltguimgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLTGUIButton::CLTGUIButton()
{

	m_hNormal		= LTNULL;
	m_hSelected		= LTNULL;
	m_hDisabled		= LTNULL;

	m_hClick		= LTNULL;

	m_nWidth			= 0;
	m_nHeight			= 0;

    m_pCommandHandler   = LTNULL;

	m_fTextureScale = 1.0f;

	m_hChecked	= LTNULL;
	m_bCheck = LTFALSE;
	// ±¤½Ä
	m_xRate		= 1.0f;
	m_yRate		= 1.0f;

	m_nOverSound	= 0;
	m_nClickSound	= 0;
	
	memset(&m_Poly,0,sizeof(m_Poly));	
}

CLTGUIButton::~CLTGUIButton()
{
	Destroy();
}

// Create the control
LTBOOL CLTGUIButton::Create (	uint32 nCommandID, 
								uint32 nHelpID, 
								HTEXTURE hNormal, 
								HTEXTURE hSelected, 
								HTEXTURE hDisabled, 
								CLTGUICommandHandler *pCommandHandler, 
								uint32 nParam1, 
								uint32 nParam2)
{
	//if (!hNormal) return LTFALSE;

	m_pCommandHandler	= pCommandHandler;

	SetTexture(hNormal,hSelected,hDisabled);

	InitPoly();

	CLTGUICtrl::Create(nCommandID, nHelpID, nParam1,nParam2);

    return LTTRUE;
}

// ±¤½Ä
LTBOOL CLTGUIButton::Create ( uint32 nCommandID,
							  uint32 nHelpID,
							  float xRate,
							  float yRate,
							  HTEXTURE hNormal, 
							  HTEXTURE hSelected, 
							  HTEXTURE hDisabled, 
							  CLTGUICommandHandler *pCommandHandler,
							  uint32 nParam1,
							  uint32 nParam2 )
{
	//if(!hNormal) return LTFALSE;

	m_pCommandHandler	= pCommandHandler;

	SetTexture(hNormal,hSelected,hDisabled);

	m_xRate		= xRate;
	m_yRate		= yRate;

	InitPoly( xRate, yRate );

	CLTGUICtrl::Create(nCommandID, nHelpID, nParam1,nParam2);

	return LTTRUE;
}

// click ÀÖ´Â ³à¼®
LTBOOL CLTGUIButton::Create ( uint32 nCommandID,
							  uint32 nHelpID,
							  float xRate,
							  float yRate,
							  HTEXTURE hNormal,
							  HTEXTURE hSelected,
							  HTEXTURE hDisabled,
							  HTEXTURE hClicked,
							  CLTGUICommandHandler *pCommandHandler,
							  uint32 nParam1,
							  uint32 nParam2 )
{
	//if(!hNormal) return LTFALSE;

	m_pCommandHandler		= pCommandHandler;

	SetTexture(hNormal, hSelected, hDisabled, hClicked);

	m_xRate		= xRate;
	m_yRate		= yRate;

	InitPoly( xRate, yRate );

	CLTGUICtrl::Create(nCommandID, nHelpID, nParam1,nParam2);

	return LTTRUE;
}

// Set the font
LTBOOL CLTGUIButton::SetFont(CUIFont *pFont, uint8 nFontSize)
{
	return LTTRUE;
}

LTBOOL CLTGUIButton::SetText(const char* pText, LTBOOL bHighlightText)
{
	return LTTRUE;
}


void CLTGUIButton::SetTexture(HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, LTBOOL bFreeOld)
{
	if (bFreeOld)
	{
		if (m_hNormal && m_hNormal != hNormal)
			g_pTexInterface->ReleaseTextureHandle(m_hNormal);
		if (m_hSelected && m_hSelected != hSelected)
			g_pTexInterface->ReleaseTextureHandle(m_hSelected);
		if (m_hDisabled && m_hDisabled != hDisabled)
			g_pTexInterface->ReleaseTextureHandle(m_hDisabled);
	}

	m_hNormal	= hNormal;
	m_hSelected = hSelected;
	m_hDisabled = hDisabled;

}

// ±¤½Ä
void CLTGUIButton::SetTexture(HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, HTEXTURE hClick, LTBOOL bFreeOld)
{
	if( bFreeOld )
	{
		if (m_hNormal && m_hNormal != hNormal)
			g_pTexInterface->ReleaseTextureHandle(m_hNormal);
		if (m_hSelected && m_hSelected != hSelected)
			g_pTexInterface->ReleaseTextureHandle(m_hSelected);
		if (m_hDisabled && m_hDisabled != hDisabled)
			g_pTexInterface->ReleaseTextureHandle(m_hDisabled);
		if( m_hClick && m_hClick != hClick )
			g_pTexInterface->ReleaseTextureHandle(m_hClick);
	}

	m_hNormal	= hNormal;
	m_hSelected = hSelected;
	m_hDisabled = hDisabled;
	m_hClick	= hClick;
}

// Enter was pressed
LTBOOL CLTGUIButton::OnEnter ( )
{
	// Send the command
	if ( m_pCommandHandler && m_pCommandHandler->SendCommand(m_nCommandID, m_nParam1, m_nParam2) )
	{
		return LTTRUE;
	}
    return LTFALSE;
}

LTBOOL CLTGUIButton::OnDblClick()
{
	// Send the command
	if ( m_pCommandHandler && m_pCommandHandler->SendCommand(m_nCommandID2, m_nParam1, m_nParam2) )
		return LTTRUE;
    return LTFALSE;
}

void CLTGUIButton::SetBasePos ( LTIntPt pos )
{ 
	CLTGUICtrl::SetBasePos(pos);
	ScalePoly();	
}

void CLTGUIButton::SetScale(float fScale)
{
	CLTGUICtrl::SetScale(fScale);
	ScalePoly();
}

void CLTGUIButton::SetTextureScale(float fScale)
{
	m_fTextureScale = fScale;
	ScalePoly();
}

void CLTGUIButton::InitPoly()
{
	ScalePoly();
	g_pDrawPrim->SetUVWH(&m_Poly,0.0f,0.0f,1.0f,1.0f);
	g_pDrawPrim->SetRGBA(&m_Poly,0xFFFFFFFF);

}

// ±¤½Ä
void CLTGUIButton::InitPoly(float xRate, float yRate)
{
	ScalePoly();
	g_pDrawPrim->SetUVWH(&m_Poly,0.0f,0.0f,xRate,yRate);
	g_pDrawPrim->SetRGBA(&m_Poly,0xFFFFFFFF);
}

void CLTGUIButton::ScalePoly()
{
//	if (!m_hNormal) return;
//	uint32 w,h;
//	g_pTexInterface->GetTextureDims(m_hNormal,w,h);
//	float x = (float)m_basePos.x * m_fScale;
//	float y = (float)m_basePos.y * m_fScale;
//	float fw = (float)w * m_fScale * m_fTextureScale;
//	float fh = (float)h * m_fScale * m_fTextureScale;
//
//	g_pDrawPrim->SetXYWH(&m_Poly,x,y,fw,fh);
//
//	m_nWidth = (uint16)fw;
//	m_nHeight = (uint16)fh;

	if (!m_baseWidth || !m_baseHeight) return;

	float x = (float)m_basePos.x * m_fScale;
	float y = (float)m_basePos.y * m_fScale;
	float fw = (float)m_baseWidth * m_fScale * m_fTextureScale;
	float fh = (float)m_baseHeight * m_fScale * m_fTextureScale;

	g_pDrawPrim->SetXYWH(&m_Poly,x,y,fw,fh);

	m_nWidth = (uint16)fw;
	m_nHeight = (uint16)fh;
}

void CLTGUIButton::SetRenderState()
{
	/*
	g_pDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
	g_pDrawPrim->SetZBufferMode(DRAWPRIM_NOZ); 
	g_pDrawPrim->SetClipMode(DRAWPRIM_NOCLIP);
	g_pDrawPrim->SetFillMode(DRAWPRIM_FILL);
	g_pDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
	g_pDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
	*/
		
}

// added by IN'T ==>
void CLTGUIButton::SetSize(int16 nWidth, int16 nHeight)
{
	m_baseWidth = nWidth;
	m_baseHeight = nHeight;
}

void CLTGUIButton::SetRect(LTRect rect)
{
	SetSize(rect.right-rect.left, rect.bottom-rect.top);
	SetBasePos(rect.left, rect.top);
}

//[yaiin] : ¹öÆ°À» Checked°¡ °¡´ÉÇÏ°Ô ÇÑ´Ù.
void CLTGUIButton::SetCheckedTexture(void)
{
	if(m_hClick && NULL == m_hChecked)
	{
		m_hChecked = m_hClick;
	}
}

void CLTGUIButton::SetCheckedTexture(HTEXTURE hCheckedTexture)
{
	if (m_hChecked)
		g_pTexInterface->ReleaseTextureHandle(m_hChecked);
	m_hChecked = hCheckedTexture;
}

void CLTGUIButton::SetCheck(LTBOOL bCheck)
{
	m_bCheck = bCheck;
}

//[MURSUM]
void CLTGUIButton::SetPolyColor( uint32 nColor )
{
	g_pDrawPrim->SetRGB( &m_Poly, nColor );
}


// Render the control
void CLTGUIButton::Render()
{
	// Sanity checks...
	if (!IsVisible()) return;

	HTEXTURE hCurrTex = NULL;
	if (IsSelected() && m_hSelected)
		hCurrTex = m_hSelected;
	else if (IsDisabled() && m_hDisabled)
		hCurrTex = m_hDisabled;
	else if (IsClicked() && m_hClick )	// ±¤½Ä
		hCurrTex = m_hClick;
	else
		hCurrTex = m_hNormal;

// Added By IN'T ==>
	if (m_bCheck && m_hChecked)
		hCurrTex = m_hChecked;
// Added By IN'T <==

	//setup the texture
	if( hCurrTex )
	{
		g_pDrawPrim->SetTexture(hCurrTex);

	//setup the UV coordinates on our quad, since it is dependant upon
	//the texture for correct filtering
	//SetupQuadUVs(m_Poly, hCurrTex, 0.0f, 0.0f, 1.0f, 1.0f);
		SetupQuadUVs(m_Poly, hCurrTex, 0.0f, 0.0f, m_xRate, m_yRate);

	// draw our button
		g_pDrawPrim->DrawPrim(&m_Poly);
	}
}

void CLTGUIButton::SetBasePos(int x, int y)
{
	LTIntPt pos;
	pos.x = x; pos.y = y;
	CLTGUICtrl::SetBasePos(pos);
	ScalePoly();
}


// added by IN'T <===
