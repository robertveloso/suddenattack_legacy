#ifndef __POPUPMENU_H__
#define __POPUPMENU_H__

#include "PopupScreenBase.h"

class CPopupMenu : public CLTGUIListView
{
public:
	
	CPopupMenu();
	~CPopupMenu();

	LTBOOL Create( uint16 nPosX, uint16 nPosY,
				   uint16 nWidth, uint16 nHeight,
				   uint16 nItemHeight,
				   CLTGUICommandHandler* pCommandHandler,
				   uint32	nCommandID,
				   LTBOOL	bDrawBack = LTFALSE,
				   uint32	nBackColor = 0xFF000000,
				   uint32	nBorderColor = 0xFFFFFFFF );

	void		ShowPopupMenu( LTIntPt pt, int nSaveData1, int nSaveData2 );
	void		HidePopupMenu();
	void		AddButton( const char* lpszText, uint32 nCommand );

	LTBOOL		RenderItem( void* pListItem, const LTIntPt* ptfPosition,
								int32 nIndex, bool bSelect, bool bOver );
	LTBOOL	OnLButtonDown( int x, int y );
	
protected:
	CSAD3DText*	m_pText;
	LTPoly_GT4	m_PolyBar;
	int32		m_nSaveData1;
	int32		m_nSaveData2;
};

#endif