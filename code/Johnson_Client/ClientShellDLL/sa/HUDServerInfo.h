// HUDServerInfo.h: interface for the CHUDServerInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HUDSERVERINFO_H__3A035DEE_9C3F_4CD8_9DE1_74CF9D9D08B1__INCLUDED_)
#define AFX_HUDSERVERINFO_H__3A035DEE_9C3F_4CD8_9DE1_74CF9D9D08B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "HUDItem.h"

class CHUDServerInfo  : public CHUDItem
{
public:
	CHUDServerInfo();
	virtual ~CHUDServerInfo();

    LTBOOL      Init();
	void		Term();

    void        Render();
    void        Update();

    void        UpdateLayout();

private:
    LTIntPt		m_BasePos;

};

#endif // !defined(AFX_HUDSERVERINFO_H__3A035DEE_9C3F_4CD8_9DE1_74CF9D9D08B1__INCLUDED_)
