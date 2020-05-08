#ifndef __MURSUM_TEXT_BUTTON_H__
#define __MURSUM_TEXT_BUTTON_H__

#include "..\..\ClientShellDLL\clientshellshared\Hantext.h"
class CLTGUITextButton : public CLTGUIButton
{
protected:
	CSAD3DText m_ButtonText;
	COLORREF	m_clrNormal;			// normal texture
	COLORREF	m_clrSelected;		// highlighted texture
	COLORREF	m_clrDisabled;		// disabled texture
	COLORREF	m_clrClick;			// click texture	// ±¤½Ä
	COLORREF	m_clrChecked;			// Checked texture
	LTBOOL		m_bCheck;
public:
	// click ÀÖ´Â ³ð
	LTBOOL           Create ( uint32 nCommandID, uint32 nHelpID, float xRate, float yRate,
								HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, HTEXTURE hClicked,
								COLORREF clrNormal =-1, COLORREF clrSelected =-1, COLORREF clrDisabled =-1, COLORREF clrClicked =-1, COLORREF clrChecked =-1,
								CLTGUICommandHandler *pCommandHandler = LTNULL, uint32 nParam1 = 0, uint32 nParam2 = 0);

	LTBOOL	SetText(LTRect rect, const char* lpszText,
		int nLen,
		unsigned int uFormat=DT_LEFT,
		unsigned long dwDrawType=D3D_TEXT_NORMAL,
		bool bDotDot=false,
		int  nDotDotWidth = -1);
	// Render the control
	virtual void	Render ();
	void SetTextColor( COLORREF rgbTextColor, COLORREF rgbExtColor=0 );
	// Click a control
	virtual void	Click(LTBOOL  bClicked);
};

#endif