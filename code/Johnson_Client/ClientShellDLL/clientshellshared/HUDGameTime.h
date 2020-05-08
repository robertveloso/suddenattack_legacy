// HUDGameTime.h: interface for the CHUDGameTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HUDGAMETIME_H__87D1D5E7_80D9_4CAB_BA37_B89F18F2825E__INCLUDED_)
#define AFX_HUDGAMETIME_H__87D1D5E7_80D9_4CAB_BA37_B89F18F2825E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHUDGameTime  
{
public:
	void DrawLevelTimeRemain();
	CHUDGameTime();
	virtual ~CHUDGameTime();

protected :
	float m_fServerTime;
	CHUDPopup m_TimeTxt;
};

#endif // !defined(AFX_HUDGAMETIME_H__87D1D5E7_80D9_4CAB_BA37_B89F18F2825E__INCLUDED_)
