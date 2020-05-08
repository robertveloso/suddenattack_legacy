#ifndef __MURSUM_TEXTLIST_H__
#define __MURSUM_TEXTLIST_H__

#include "LTGUIMgr.h"

typedef struct _tagTEXTITEM
{
	char*			pszText;
	unsigned long	dwDrawType;
	COLORREF		rgbTextColor;
	COLORREF		rgbExtColor;
	unsigned long	dwDeleteTick;
} T_TEXT_ITEM, * T_TEXT_ITEMptr;

class CSaListText : public CLTGUIListView
{
public:
	CSaListText();
	virtual ~CSaListText();

	LTBOOL			Create( uint16 nPosX, uint16 nPosY,
							uint16 nWidth, uint16 nHeight,
							uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler = LTNULL,
							LTBOOL	bDrawBack = LTFALSE,
							uint32	nBackColor = 0xFF000000,
							uint32	nBorderColor = 0xFFFFFFFF );

	LTBOOL			SetMaxItemCount( int nCount );

	LTBOOL			Append( LPCSTR lpszText,
							unsigned long dwDrawType,
							COLORREF rgbTextColor,
							COLORREF rgbExtColor,
							unsigned long dwDeleteTick );
	LTBOOL			AddText( LPCSTR lpszText,
							 unsigned long dwDrawType,
							 COLORREF rgbTextColor,
							 COLORREF rgbExtColor,
							 unsigned long dwDeleteTick=0 );
	void			ScrollBottom();

	void			DeleteTop();
	void			DeleteAllItem();
	void			DeleteText( unsigned long dwCurrentTick );
	
	LTBOOL			OnWheelUp( int x, int y );
	LTBOOL			OnWheelDown( int x, int y );

protected:
	LTBOOL			RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
	void			RenderItemText();
		
	CSAD3DText*		m_pTextList;
	int32			m_nMaxItemCount;
	char			m_szBuffer[2048];
};

#endif //#ifndef __MURSUM_TEXTLIST_H__