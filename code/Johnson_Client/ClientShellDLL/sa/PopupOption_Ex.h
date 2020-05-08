#ifndef __SPIKE_POPOPTION_EX_H__
#define __SPIKE_POPOPTION_EX_H__

#include "LTGUIMgr.h"

CONST uint8 RESOLUTION_MAXLINE = 6;
CONST uint8 CTRL_MAXLINE = 11;

typedef struct Resolution_Item
{
	int	   m_dwIndex;
    uint32 m_dwWidth;       // Screen width
    uint32 m_dwHeight;      // Screen height
    uint32 m_dwBitDepth;    // Screen bitdepth
	
	bool   m_bHWTnL;
	char   m_internalName[200];	// This is what the DLLs use to identify a card
	char   m_description[200];	// The description of the renderer

	Resolution_Item()
	{
		m_dwIndex	 =-1;
		m_dwWidth	 = 0;
		m_dwHeight	 = 0;
		m_dwBitDepth = 0;
		m_bHWTnL	 = FALSE;
	}
}*ResolutionItem_Ptr;
typedef struct ControlText_Item
{
	int  m_iIndex;
	//char pControl_Command[128];
	CSAD3DText	Ctrl_Action;
	CSAD3DText	Ctrl_Command;
	ControlText_Item()
	{
		m_iIndex = -1;
	}
}*ControlTextItem_Ptr;

// Resolution ========================================================
class CSaListResolution : public CLTGUIListView
{
public:
	CSaListResolution();
	virtual ~CSaListResolution();
	LTBOOL				Create( uint16 nPosX,
								uint16 nPosY,
								uint16 nWidth,
								uint16 nHeight,
								uint16 nItemHeight,
								uint32 nCommandID = LTNULL);
	
	virtual LTBOOL OnWheelUp(int x, int y);
	virtual LTBOOL OnWheelDown(int x, int y);

	LTBOOL				RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
	LTBOOL				AddResolution( ResolutionItem_Ptr pResolution_Item );
	VOID				AddTextData(ResolutionItem_Ptr pResolution_Item);
	ResolutionItem_Ptr	GetResolutionItem(int iIndex = -1);
	LTBOOL				SetResolutionText(uint8 nIndex, char* szText);

	int32				m_iSelIndex;
	LTBOOL				m_bJustOnce;
protected:
	VOID				RenderItemText();
	HTEXTURE			m_hBar;
	LTPoly_GT4			m_PolyBar;
	CSAD3DText*			m_pResolutionText;
};
//=====================================================================

// Control ==================================
class CSaListControlText : public CLTGUIListView
{
public:
	CSaListControlText();
	virtual ~CSaListControlText();
	LTBOOL				Create( uint16 nPosX,
								uint16 nPosY,
								uint16 nWidth,
								uint16 nHeight,
								uint16 nItemHeight,
								uint32 nCommandID = LTNULL );
	
	virtual LTBOOL OnWheelUp(int x, int y);
	virtual LTBOOL OnWheelDown(int x, int y);
	virtual void	DeleteAllItem();
	
	LTBOOL				RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
	LTBOOL				AddControlText( ControlTextItem_Ptr pControlTextItem );
	ControlTextItem_Ptr GetSelectedKeySettingData(int iIndex);

	VOID				SetInitControlText();
	LTBOOL				SetCommandText(uint8 nIndex, char* szText);
	
	int32				m_iIndex;
	int32				m_iListVectorIndex;
	BOOL				m_bSelect, m_bOver, m_bWaitingKey;	//	
	char				m_szSaveText[64];
	
protected:
	HTEXTURE			m_hBar;
	LTPoly_GT4			m_PolyBar;
};



/////////////////////////////////////////////////////////////////////////////////////
//	드레그 보조버튼	/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

class CDragSupportButton : public CLTGUIButton
{
public:
	CDragSupportButton();
	virtual ~CDragSupportButton();

	LTFLOAT	CalculateDragButtonRatio(int x);

private:
};

#endif // __SPIKE_POPOPTION_H__