#include "ltguiTextButton.h"

LTBOOL CLTGUITextButton::SetText(LTRect rect, const char* lpszText,
							 int nLen,
							 unsigned int uFormat,
							 unsigned long dwDrawType,
							 bool bDotDot,
							 int  nDotDotWidth )
{
	m_ButtonText.CreateD3DText( rect.right -rect.left, rect.bottom -rect.top, true );
	
	m_ButtonText.SetTransparent(true);
	m_ButtonText.SetPosition( rect.left, rect.top );
	m_ButtonText.DrawD3DText( lpszText, nLen, uFormat, dwDrawType, bDotDot, nDotDotWidth );
	return LTTRUE;
}

void CLTGUITextButton::SetTextColor( COLORREF rgbTextColor, COLORREF rgbExtColor )
{
	m_ButtonText.SetTextColor( rgbTextColor, rgbExtColor );
}


LTBOOL CLTGUITextButton::Create ( uint32 nCommandID, uint32 nHelpID, float xRate, float yRate,
						 HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, HTEXTURE hClicked,
						 COLORREF clrNormal =-1, COLORREF clrSelected =-1, COLORREF clrDisabled =-1, COLORREF clrClicked =-1, COLORREF clrChecked =-1,
						 CLTGUICommandHandler *pCommandHandler = LTNULL, uint32 nParam1 = 0, uint32 nParam2 = 0)
{
	__super::Create(nCommandID, nHelpID, xRate, yRate, hNormal, hSelected, hDisabled, hClicked, pCommandHandler, nParam1, nParam2);
	m_clrNormal = clrNormal;
	m_clrSelected = clrSelected;
	m_clrDisabled = clrDisabled;
	m_clrClick = clrClicked;
	m_clrChecked = clrChecked;
}


void CLTGUITextButton::Click(LTBOOL  bClicked)
{
	__super::Click(bClicked);
	m_clrChecked = LTTRUE;
}

void CLTGUITextButton::Render ()
{
	__super::Render();
	// Sanity checks...
	if (!IsVisible()) return;
	
	COLORREF clrCurrTex = -1;
	if (IsSelected() && m_clrSelected !=-1)
		clrCurrTex = m_clrSelected;
	else if (IsDisabled() && m_clrDisabled !=-1)
		clrCurrTex = m_clrDisabled;
	else if (IsClicked() && m_clrClick !=-1 )	// ±¤½Ä
		clrCurrTex = m_clrClick;
	else
		clrCurrTex = m_clrNormal;
	
	// Added By IN'T ==>
	if (clrCurrTex && m_clrChecked)
		clrCurrTex = m_clrChecked;
	// Added By IN'T <==
	
	//setup the texture
	if( clrCurrTex !=-1 )
	{
		g_pDrawPrim->SetTexture(clrCurrTex);
		
		//setup the UV coordinates on our quad, since it is dependant upon
		//the texture for correct filtering
		//SetupQuadUVs(m_Poly, hCurrTex, 0.0f, 0.0f, 1.0f, 1.0f);
		SetupQuadUVs(m_Poly, clrCurrTex, 0.0f, 0.0f, m_xRate, m_yRate);
		
		// draw our button
		g_pDrawPrim->DrawPrim(&m_Poly);
	}
}