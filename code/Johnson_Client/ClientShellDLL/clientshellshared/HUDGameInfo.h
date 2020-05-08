// HUDGameInfo.h: interface for the CHUDGameInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HUDGAMEINFO_H__87D1D5E7_80D9_4CAB_BA37_B89F18F2825E__INCLUDED_)
#define AFX_HUDGAMEINFO_H__87D1D5E7_80D9_4CAB_BA37_B89F18F2825E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HUDItem.h"


class CHUDGameInfo  : public CHUDItem
{
public:
	CHUDGameInfo();
	virtual ~CHUDGameInfo();

    LTBOOL      Init();
	void		Term();

    void        Render();
    void        Update();

    void        UpdateLayout();

protected:
    LTIntPt		m_BasePos;

	float		m_fServerTime;
	std::string m_sRemainTime;
	char		m_szTimeFormat[128];
	LPD3DXFONT	m_lpFont;
//	CHUDPopup m_TimeTxt;
};

#endif // !defined(AFX_HUDGAMETIME_H__87D1D5E7_80D9_4CAB_BA37_B89F18F2825E__INCLUDED_)
