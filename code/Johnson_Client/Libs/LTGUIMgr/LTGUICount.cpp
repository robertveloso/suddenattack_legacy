// LTGUICount.cpp: implementation of the CLTGUICount class.
//
//////////////////////////////////////////////////////////////////////

#include "ltguimgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLTGUICount::CLTGUICount()
{
	m_hTexture = LTNULL;
	m_nCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_fScale = 1.0f;
//	m_fTextureX = 0.0f;
//	m_fTextureY = 0.0f;
	m_pDigit = LTNULL;
	m_nDigit = 1;
	m_pPoly = LTNULL;
	m_nMin = 0;
	m_nMax = 9;
	m_nIncCount = 1;
}

CLTGUICount::~CLTGUICount()
{
	if (m_nDigit>1)
	{
		debug_deletea(m_pPoly);
		debug_deletea(m_pDigit);
	}
	else if (m_nDigit==1)
	{
		debug_delete(m_pPoly);
		debug_delete(m_pDigit);
	}
}

LTBOOL CLTGUICount::Create(LTIntPt pos, uint16 nWidth, uint nHeight, HTEXTURE hCountTexture, uint8 nDigit)
{
	if (!hCountTexture) return LTFALSE;

	if (nDigit>1)
	{
		m_pDigit = debug_newa(uint8, nDigit);
		m_pPoly =  debug_newa(LT_POLYGT4, nDigit);
	}
	else if (nDigit == 1)
	{
		m_pDigit = debug_new(uint8);
		m_pPoly = debug_new(LT_POLYGT4);
	}
	else
		return LTFALSE;

	m_nDigit = nDigit;

	m_hTexture = hCountTexture;
	m_baseWidth = nWidth;
	m_baseHeight = nHeight;

	InitPoly();

	CLTGUICtrl::SetBasePos(pos);
	ScalePoly();

	SetCount(0);

	return LTTRUE;
}

void CLTGUICount::InitPoly()
{
	for (uint8 i = 0; i <m_nDigit; ++i)
	{
		g_pDrawPrim->SetUVWH(&m_pPoly[i],0.0f,0.0f,1.0f,1.0f);
		g_pDrawPrim->SetRGBA(&m_pPoly[i],0xFFFFFFFF);
	}
}

void CLTGUICount::ScalePoly()
{
	if (!m_baseWidth || !m_baseHeight) return;

	float fw2 = 0.0f;
	float fh2 = 0.0f;
	int nDigitRev = m_nDigit-1;
	for (int i = nDigitRev; i >= 0; i--)
	{
		float fw = (float)m_baseWidth * m_fScale;
		float fh = (float)m_baseHeight * m_fScale;
		float x = (float)m_basePos.x * m_fScale + (fw*(nDigitRev-i));
		float y = (float)m_basePos.y * m_fScale;
		
		g_pDrawPrim->SetXYWH(&m_pPoly[i],x,y,fw,fh);
		fw2 += fw;
		fh2 = fh;
	}
	m_nWidth = (uint16)fw2;
	m_nHeight = (uint16)fh2;
}

void CLTGUICount::SetScale(float fScale)
{
	CLTGUICtrl::SetScale(fScale);
	ScalePoly();
}

void CLTGUICount::SetCount(uint16 nCount)
{
	m_nCount = nCount;

	// 자리수마다 숫자값 얻어오기
	for (int i = m_nDigit-1; i >= 0 ; --i)
	{
		int t = (int)pow(10,i);
		m_pDigit[i] = (int) (nCount / t);
		nCount -= (m_pDigit[i]*t);
	}
}

void CLTGUICount::Render()
{
	// Sanity checks...
	if (!IsVisible()) return;

	for (uint8 i = 0; i <m_nDigit; ++i)
	{
		g_pDrawPrim->SetTexture(m_hTexture);
		//setup the UV coordinates on our quad, since it is dependant upon
		//the texture for correct filtering
		float fTextureX = (float)(m_pDigit[i]%5) * GUI_COUNT_WIDTH;
		float fTextureY = (float)(m_pDigit[i]/5) * GUI_COUNT_HEIGHT;
		SetupQuadUVs(m_pPoly[i], m_hTexture, fTextureX, fTextureY, GUI_COUNT_WIDTH, GUI_COUNT_HEIGHT);

		// set up the render state	

		g_pDrawPrim->DrawPrim(&m_pPoly[i]);
	}
}

void CLTGUICount::SetRenderState()
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


void CLTGUICount::SetLimitCount(int nMin, int nMax, int nIncCount)
{
	m_nMin = nMin; 
	m_nMax = nMax;
	m_nIncCount = nIncCount;
	if (m_nMax <= m_nCount)
		m_nCount = m_nMax;
	if (m_nMin >= m_nCount)
		m_nCount = m_nMin;
	SetCount(m_nCount);
}

void CLTGUICount::IncCount()
{
	m_nCount += m_nIncCount;
	if (m_nMax <= m_nCount)
		m_nCount = m_nMax;
	SetCount(m_nCount);
}

void CLTGUICount::DecCount()
{
	m_nCount -= m_nIncCount;
	if (m_nMin >= m_nCount)
		m_nCount = m_nMin;
	SetCount(m_nCount);
}
