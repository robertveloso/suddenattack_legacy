#ifndef __MURSUM_LISTVIEW_H__
#define __MURSUM_LISTVIEW_H__

#define	LTLV_DEFAULT_OVER_COLOR	0x35757575
#define	LTLV_DEFAULT_SEL_COLOR	0x70757575
#define LTLV_CHANGEED	0x00000002

class CLTGUIButton;
class CLTGUIDragButton;

typedef std::vector<void*> LTLV_ITEM_LIST;
typedef LTLV_ITEM_LIST::iterator LTLV_ITEM_LIST_ITR;

class CLTGUIListView : public CLTGUICtrl
{
public:
	CLTGUIListView();
	virtual ~CLTGUIListView();

	virtual LTBOOL		Create( uint16 nPosX, uint16 nPosY,
								uint16 nWidth, uint16 nHeight,
								uint16 nItemHeight,
								CLTGUICommandHandler* pCommandHandler = LTNULL,
								LTBOOL	bDrawBack = LTFALSE,
								uint32	nBackColor = 0xFF000000,
								uint32	nBorderColor = 0xFFFFFFFF );
	
	void				Term();
	// Render the control
	void				Render();

	// Width/Height calculations
	uint16				GetWidth()	{ return m_nWidth; }
	uint16				GetHeight()	{ return m_nHeight; }

	// Handle a keypress
    LTBOOL				HandleKeyDown(int key, int rep);

	// Mouse messages
    virtual	LTBOOL		OnLButtonDown(int x, int y);
	virtual LTBOOL		OnRButtonDown(int x, int y);
    virtual LTBOOL		OnMouseMove(int x, int y);
	virtual LTBOOL		OnWheelUp(int x, int y);
	virtual LTBOOL		OnWheelDown(int x, int y);
	
	virtual void		OnSelChange();	
	virtual void		OnOverChange( int nPreIndex, int nNewIndex ) {};
	virtual void		SetBasePos( LTIntPt pos );
	
	// SPIKE
	VOID				SetSelectIndex(int32 nIndex){m_nSelIndex = nIndex;}
	int32				GetSelectIndex() { return m_nSelIndex; }	
	// <== 
	//Item
	virtual	LTBOOL		InsertItem( void* pItem );
	virtual void		DeleteAllItem();

	//Scroll (SPIKE: AccelerationÃß°¡)
	LTBOOL				ScrollUp(int8 nAcceleration = 0);
	LTBOOL				ScrollDown(int8 nAcceleration = 0);
	LTBOOL				PageUp();
	LTBOOL				PageDown();
	LTBOOL				ScrollFromRatio( float fRatio );

	LTBOOL				IsEnableScroll() { return m_bEnableScroll; }
	float				GetRatio();
	
	int32				GetFirstIndex() { return m_nFirstItemIndex; }
	// SPIKE
	VOID				SetInitIndex(int32	nFirst, int32 nLast){ m_nFirstItemIndex = nFirst;m_nLastItemIndex = nLast;}
	int32				GetLastIndex() { return m_nLastItemIndex; }
	LTBOOL				IsVisibleItem( int32 nIndex );
	
	void				SetBackPolySize( int nLines = -1 );

protected:
	virtual	LTBOOL		RenderItem( void* pListItem, const LTIntPt* ptfPosition,
								int32 nIndex, bool bSelect, bool bOver );
	virtual void		RenderItemText();

	LTBOOL				DeleteItemPtr( void* pItem, int nIndex );
	LTBOOL				PointInMe( int x, int y );
		
	LTLV_ITEM_LIST		m_ItemList;

	uint16				m_nWidth;
	uint16				m_nHeight;
	uint16				m_nItemHeight;
	uint16				m_nPageItemCount;
	uint16				m_nItemLeft;
	uint16				m_nItemRight;

	int32				m_nTotalItemCount;
	int32				m_nFirstItemIndex;
	int32				m_nLastItemIndex;
	int32				m_nOverIndex;
	int32				m_nSelIndex;

	LTIntPt				m_ptItemPos;

	//Scroll
	LTBOOL				m_bEnableScroll;
	CLTGUIButton*		m_pBtnScrollUp;
	CLTGUIButton*		m_pBtnScrollDown;
	CLTGUIDragButton*	m_pBtnScroll;

	//Command Handler
	CLTGUICommandHandler	*m_pCommandHandler;

	LTBOOL				m_bDrawBack;
	LTPoly_GT4			m_PolyBack;
	LTPoly_GT4			m_PolyBorder;
};

#endif //#ifndef __MURSUM_LISTVIEW_H__
