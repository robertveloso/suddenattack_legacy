#ifndef __MURSUM_CHANNELLIST_H__
#define __MURSUM_CHANNELLIST_H__

#include "LTGUIMgr.h"

typedef struct _tagCHANNELITEM
{
	int		nIndex;
	char	szName[128];
	int		nCurrentUsers;
	int		nMaxUsers;
	CSAD3DText*	pTextIndex;
	CSAD3DText*	pTextName;
	CSAD3DText*	pTextUsers;
} T_CHANNEL_ITEM, *T_CHANNEL_ITEMptr;

class CSaListChannel : public CLTGUIListView
{
public:
	CSaListChannel();
	virtual ~CSaListChannel();

	LTBOOL			Create( uint16 nPosX, uint16 nPosY,
							uint16 nWidth, uint16 nHeight,
							uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler = LTNULL,
							LTBOOL	bDrawBack = LTFALSE,
							uint32	nBackColor = 0xFF000000,
							uint32	nBorderColor = 0xFFFFFFFF );
	
	LTBOOL			AddChannel( T_CHANNEL_ITEMptr pChannelItem );
	T_CHANNEL_ITEMptr	GetSelectedChannelData();

	LTBOOL			OnLButtonDown(int x, int y);
	LTBOOL			OnRButtonDown(int x, int y);
	LTBOOL			OnLButtonDblClick( int x, int y );
	void			OnOverChange( int nPreIndex, int nNewIndex );

	void			SortNDrawText();

	void			DeleteAllItem();
	
protected:
	LTBOOL			RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
	void			RenderItemText();
				
	HTEXTURE		m_hSelected;
	LTPoly_GT4		m_PolyBar;	
	LTPoly_GT4		m_PolyChecked;
	LTBOOL			m_bRecvComplete;
};

#endif //#ifndef __MURSUM_USERLIST_H__
