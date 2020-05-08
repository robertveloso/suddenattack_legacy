// LTGUICount.h: interface for the CLTGUICount class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LTGUICOUNT_H__0F2021F3_3119_4F12_A239_89CD39EB736B__INCLUDED_)
#define AFX_LTGUICOUNT_H__0F2021F3_3119_4F12_A239_89CD39EB736B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include "LTGUICtrl.h"
#define GUI_COUNT_WIDTH 0.2f
#define GUI_COUNT_HEIGHT 0.5f

class CLTGUICount : public CLTGUICtrl  
{
public:
	void DecCount();
	void IncCount();
	int GetMax() {return m_nMax;}
	int GetMin() {return m_nMin;}
	void SetLimitCount(int nMin, int nMax, int nIncCount = 1);
	uint16 GetCount()  {return m_nCount;}
	void SetCount(uint16 nCount);
	CLTGUICount();
	virtual ~CLTGUICount();

	LTBOOL Create(LTIntPt pos, uint16 nWidth, uint nHeight, HTEXTURE hCountTexture, uint8 nDigit = 1);

	virtual void	SetScale(float fScale);
	// Render the control
	virtual void	Render ();

	// Width/Height calculations
	virtual uint16	GetWidth ( )						{ return m_nWidth; }
	virtual uint16	GetHeight ( )						{ return m_nHeight; }

protected:
	void InitPoly();
	void ScalePoly();
	void SetRenderState();
	
	HTEXTURE m_hTexture;
	LT_POLYGT4*	m_pPoly;

	uint16 m_nWidth;
	uint16 m_nHeight;

	uint8 m_nDigit;
	int16 m_nCount;
	uint8* m_pDigit;

	int m_nMin;
	int m_nMax;
	int m_nIncCount;

//	float m_fTextureX;
//	float m_fTextureY;
};

#endif // !defined(AFX_LTGUICOUNT_H__0F2021F3_3119_4F12_A239_89CD39EB736B__INCLUDED_)
