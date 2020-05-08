// ----------------------------------------------------------------------- //
//
// MODULE  : LTGUISlider.cpp
//
// PURPOSE : Control to display a full/empty style slider bar.
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "ltguimgr.h"


namespace
{
	enum eBarElements
	{
		eFull = 0,
		eEmpty,
		eLeft,
		eRight
	};
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLTGUISlider::CLTGUISlider()
{
	m_pnValue = LTNULL;
	m_nMinSlider = 0;
	m_nMaxSlider = 10;
	m_nSliderPos = 0;
	m_nSliderIncrement = 1;

	m_nHeight = 0;
	m_nWidth = 0;
	m_nArrowWidth = 0;
	m_nArrowHeight = 0;

	m_bOverLeft = LTFALSE;
	m_bOverRight = LTFALSE;

	m_bDisplayOnly = false;

	memset(m_Bar,0,sizeof(m_Bar)); 

	m_pCallback = NULL;

}

CLTGUISlider::~CLTGUISlider()
{
	Destroy();
}


LTBOOL CLTGUISlider::Create ( uint32 nHelpID, HTEXTURE hBarTexture, 
						uint16 nBarOffset, uint16 nBarWidth, uint16 nBarHeight, int *pnValue)
{

	if (!hBarTexture) return LTFALSE;
	m_hBar = hBarTexture;

	m_pnValue = pnValue;
	m_nBarWidth = nBarWidth;
	m_nBarHeight = nBarHeight;
	m_nBarOffset = nBarOffset;

	uint32 w,h;
	g_pTexInterface->GetTextureDims(m_hBar,w,h);

	m_nArrowWidth = w / 2;
	m_nArrowHeight = h / 2;


	SetHelpID(nHelpID);

	InitBar();

	return LTTRUE;
}

// Update data
void CLTGUISlider::UpdateData(LTBOOL bSaveAndValidate)
{
	//sanity check
	if (!m_pnValue)
	{
		return;
	}

	if (bSaveAndValidate)
	{
		*m_pnValue=GetSliderPos();
	}
	else
	{
		SetSliderPos(*m_pnValue);
	}
}

// Sets the min and max of the slider bar
void CLTGUISlider::SetSliderRange(int nMin, int nMax)
{
	if (nMax <= nMin) return;

	m_nMinSlider = nMin;
	m_nMaxSlider = nMax;

	// Call this to make sure that the slider stays within range
	SetSliderPos(m_nSliderPos);
}

// Sets the slider position
void CLTGUISlider::SetSliderPos(int nPos)
{
	m_nSliderPos=nPos;
	if ( m_nSliderPos > m_nMaxSlider )
	{
		m_nSliderPos=m_nMaxSlider;
	}
	if ( m_nSliderPos < m_nMinSlider )
	{
		m_nSliderPos=m_nMinSlider;
	}

	ScaleBar();
}


// Handle the left command
LTBOOL CLTGUISlider::OnLeft()
{
	if (m_bDisplayOnly) return LTFALSE;

	int nPos = m_nSliderPos-m_nSliderIncrement;

	if ( nPos < m_nMinSlider )
	{
		nPos=m_nMinSlider;
	}
	if (nPos != m_nSliderPos)
	{
		if (m_pCallback && !m_pCallback(this,nPos,m_nSliderPos))
			return LTFALSE;

		SetSliderPos(nPos);
        return LTTRUE;
	}
	return LTFALSE;
}

// Handle the right command
LTBOOL CLTGUISlider::OnRight()
{
	if (m_bDisplayOnly) return LTFALSE;

	int nPos = m_nSliderPos+m_nSliderIncrement;
	if ( nPos > m_nMaxSlider )
	{
		nPos = m_nMaxSlider;
	}
	if (nPos != m_nSliderPos)
	{
		if (m_pCallback && !m_pCallback(this,nPos,m_nSliderPos))
			return LTFALSE;
		SetSliderPos(nPos);
       return LTTRUE;
	}
    return LTFALSE;
}

void CLTGUISlider::OnSelChange()
{
	if (!IsSelected())
	{
		m_bOverLeft = LTFALSE;
		m_bOverRight = LTFALSE;
	}
}

// Destroys the control
void CLTGUISlider::Destroy ( )
{
}

// Render the control
void CLTGUISlider::Render ()
{
	g_pDrawPrim->SetTexture(m_hBar);

//	draw bar

	if (m_bEnabled && m_nSliderPos > m_nMinSlider)
	{
		g_pDrawPrim->SetTexture(m_hBar);
		g_pDrawPrim->DrawPrim(&m_Bar[eFull]);
	}

	if (m_bEnabled && m_nSliderPos < m_nMaxSlider)
	{
		g_pDrawPrim->SetTexture(m_hBar);
		g_pDrawPrim->DrawPrim(&m_Bar[eEmpty]);
	}
	
//	draw arrows
	if (!m_bDisplayOnly)
	{
		if (m_bOverLeft)
			g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MUL_SRCALPHA_ONE);
		else
			g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
		g_pDrawPrim->SetTexture(m_hBar);
		g_pDrawPrim->DrawPrim(&m_Bar[eLeft]);

		if (m_bOverRight)
			g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MUL_SRCALPHA_ONE);
		else
			g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
		g_pDrawPrim->SetTexture(m_hBar);
		g_pDrawPrim->DrawPrim(&m_Bar[eRight]);
	}	
}

LTBOOL CLTGUISlider::OnMouseMove(int x, int y)
{
	LTBOOL bWasOverLeft  = m_bOverLeft;
	LTBOOL bWasOverRight = m_bOverRight;

	m_bOverLeft = (x >= m_rcLeft.left && y >= m_rcLeft.top && x <= m_rcLeft.right && y <= m_rcLeft.bottom);
	m_bOverRight = (x >= m_rcRight.left && y >= m_rcRight.top && x <= m_rcRight.right && y <= m_rcRight.bottom);

	//determine if we have changed controls
	return ((!bWasOverLeft && m_bOverLeft) || 
			(!bWasOverRight && m_bOverRight));
}


LTBOOL CLTGUISlider::OnLButtonUp(int x, int y)
{
	if (m_bDisplayOnly) return LTFALSE;
	if (m_bOverLeft) return OnLeft();
	if (m_bOverRight) return OnRight();
	if (x >= m_rcBar.left && y >= m_rcBar.top && x <= m_rcBar.right && y <= m_rcBar.bottom)
	{
		float fPercent = (float)(x - m_rcBar.left) / (float)(m_rcBar.right - m_rcBar.left);
		int nPos = m_nSliderIncrement * (int)((fPercent * (float)(m_nMaxSlider - m_nMinSlider) / (float)m_nSliderIncrement) + 0.5f);
		if (nPos+m_nMinSlider != m_nSliderPos)
		{
			if (m_pCallback && !m_pCallback(this,(nPos+m_nMinSlider),m_nSliderPos))
				return LTFALSE;

			SetSliderPos(nPos+m_nMinSlider);
			return LTTRUE;
		}

	}
	return LTFALSE;

}

void CLTGUISlider::SetBasePos ( LTIntPt pos )
{ 
	CLTGUICtrl::SetBasePos(pos);
	ScaleBar();
}

void CLTGUISlider::SetScale(float fScale)
{
	CLTGUICtrl::SetScale(fScale);
	ScaleBar();
}

void CLTGUISlider::SetTextureScale(float fScale)
{
//	m_fTextureScale = fScale;
	ScaleBar();
}


void CLTGUISlider::InitBar()
{
	ScaleBar();
	SetupQuadUVs(m_Bar[eFull], m_hBar, 0.0f,0.0f,0.5f,0.5f);
	SetupQuadUVs(m_Bar[eEmpty], m_hBar, 0.5f,0.0f,0.5f,0.5f);
	SetupQuadUVs(m_Bar[eLeft], m_hBar, 0.0f,0.5f,0.5f,0.5f);
	SetupQuadUVs(m_Bar[eRight], m_hBar, 0.5f,0.5f,0.5f,0.5f);

	for (int i = 0; i < 4; i++) 
		g_pDrawPrim->SetRGBA(&m_Bar[i],0xFFFFFFFF);

}

void CLTGUISlider::ScaleBar()
{
	//determine height of bar
	uint16 nHt = 0;
	if (m_nBarHeight > nHt) nHt = m_nBarHeight;
	if (m_nArrowHeight > nHt) nHt = m_nArrowHeight;
	m_nHeight = (uint16)((float)nHt * m_fScale);
	m_nWidth = (uint16)((float)(m_nBarOffset + m_nBarWidth) * m_fScale);

	float fPercent = (float)(m_nSliderPos - m_nMinSlider) / (float)(m_nMaxSlider - m_nMinSlider);

	//left arrow
	float x = (float)m_pos.x + ((float)m_nBarOffset * m_fScale);
	float y = (float)m_pos.y;
	float fw = (float)m_nArrowWidth * m_fScale;
	float fh = (float)m_nArrowHeight * m_fScale;

	float yo = ((float)m_nHeight - fh) / 2.0f;
	float arrowGap = fw / 4.0f;
	float arrowSpace = fw + arrowGap;

	if (m_bDisplayOnly)
	{
		arrowGap = 0.0f;
		arrowSpace = 0.0f;
	}
	float barWidth = ((float)m_nBarWidth * m_fScale) - 2 * arrowSpace;

	g_pDrawPrim->SetXYWH(&m_Bar[eLeft],x,y + yo,fw,fh);

	m_rcLeft.left = (int)x;
	m_rcLeft.top = (int)(y + yo);
	m_rcLeft.right = (int)(x + fw);
	m_rcLeft.bottom = m_rcLeft.top + (int)fh;


	//full part of bar
	x += arrowSpace;
	fw = fPercent * barWidth;
	fh = (float)m_nBarHeight * m_fScale;
	yo = ((float)m_nHeight - fh) / 2.0f;
	g_pDrawPrim->SetXYWH(&m_Bar[eFull],x,y+yo,fw,fh);
	m_rcBar.left = (int)x;
	m_rcBar.top = (int)(y + yo);

	//empty part of bar
	x += fw;
	fw = barWidth * (1.0f - fPercent);
	g_pDrawPrim->SetXYWH(&m_Bar[eEmpty],x,y+yo,fw,fh);
	m_rcBar.right = (int)(x + fw);
	m_rcBar.bottom = m_rcBar.top + (int)fh;

	//right arrow
	fw = (float)m_nArrowWidth * m_fScale;
	fh = (float)m_nArrowHeight * m_fScale;
	x = (float)(m_pos.x + m_nWidth) - fw;
	yo = ((float)m_nHeight - fh) / 2.0f;
	g_pDrawPrim->SetXYWH(&m_Bar[eRight],x,y+yo,fw,fh);

	m_rcRight.left = (int)x;
	m_rcRight.top = (int)(y + yo);
	m_rcRight.right = (int)(x + fw);
	m_rcRight.bottom = m_rcRight.top + (int)fh;
}

void CLTGUISlider::SetRenderState()
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


//used to calculate the x offset of the slider at a given position
float CLTGUISlider::CalculateSliderOffset(int pos)
{

	float fPercent = (float)(pos - m_nMinSlider) / (float)(m_nMaxSlider - m_nMinSlider);

	//left arrow
	float x = ((float)m_nBarOffset * m_fScale);
	float fw = (float)m_nArrowWidth * m_fScale;

	float arrowGap = fw / 4.0f;
	float arrowSpace = fw + arrowGap;
	float barWidth = ((float)m_nBarWidth * m_fScale) - 2 * arrowSpace;

	//full part of bar
	x += arrowSpace;
	x += fPercent * barWidth;

	return x;
}

// for controls to display a value, but not accept input
void CLTGUISlider::SetDisplayOnly(bool bDisplayOnly)
{
	m_bDisplayOnly = bDisplayOnly;
	ScaleBar();
}
