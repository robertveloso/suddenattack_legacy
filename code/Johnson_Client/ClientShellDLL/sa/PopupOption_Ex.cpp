#include "StdAfx.h"
#include "PopupOption_Ex.h"
#include "InterfaceResMgr.h"

CSaListResolution::CSaListResolution()
:m_pResolutionText(LTNULL)
,m_iSelIndex(-1)
,m_bJustOnce(LTFALSE)
,CLTGUIListView() {}
CSaListResolution::~CSaListResolution()
{
	this->Term();

	if( m_pResolutionText )
	{
		delete [] m_pResolutionText;
		m_pResolutionText = NULL;
	}
}
LTBOOL				
CSaListResolution::Create( uint16 nPosX, uint16 nPosY, uint16 nWidth, 
					   uint16 nHeight, uint16 nItemHeight, uint32 nCommandID )
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight );
	CLTGUICtrl::Create(nCommandID, LTNULL, LTNULL, LTNULL);

	m_pResolutionText = new CSAD3DText[RESOLUTION_MAXLINE];

	for(int i=0 ; i<RESOLUTION_MAXLINE ; ++i )
	{
		m_pResolutionText[i].CreateD3DText( nWidth, nItemHeight, false );
		m_pResolutionText[i].SetTextColor( RGB(210,210,210), RGB(0,0,0) );
		m_pResolutionText[i].SetTransparent(true);
		m_pResolutionText[i].SetPosition( (float)nPosX+13.0f,
										  (float)nPosY+75.0f+(i*m_nItemHeight) );
		m_pResolutionText[i].DrawD3DText( "", 0 );
	}
	m_hBar	= g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\Menu\\Textures\\Main\\RoomBar.dtx");
	g_pDrawPrim->SetUVWH( &m_PolyBar, 0.0f, 0.0f, 577.0f/1024.0f, 21.0/32.0f );
	return  LTTRUE;
}

LTBOOL 
CSaListResolution::OnWheelUp(int x, int y)
{
	return LTFALSE;
}

LTBOOL 
CSaListResolution::OnWheelDown(int x, int y)
{
	return LTFALSE;
}

LTBOOL 
CSaListResolution::AddResolution( ResolutionItem_Ptr pResolution_Item )
{
	if(!pResolution_Item) return LTFALSE;

	LTBOOL bResult;
	bResult = InsertItem( (void *)pResolution_Item );
	if( !bResult )
	{
		return LTFALSE;
	}
	else if( bResult == LTLV_CHANGEED )
	{
		AddTextData(pResolution_Item);
	}

	return LTTRUE;
}

LTBOOL
CSaListResolution::SetResolutionText(uint8 nIndex, char* szText)
{
	if(nIndex >= RESOLUTION_MAXLINE || nIndex < 0 || !szText) return LTFALSE;

	m_pResolutionText[nIndex].DrawD3DText(szText, strlen(szText), DT_CENTER, D3D_TEXT_SHADOW, true);
	
	return LTTRUE;
}

LTBOOL 
CSaListResolution::RenderItem( void* pListItem, const LTIntPt* ptItemPos, int32 nIndex, bool bSelect, bool bOver )
{
	ResolutionItem_Ptr pResolution_Item = (ResolutionItem_Ptr)pListItem;

	g_pDrawPrim->BeginDrawPrim();
	if( bSelect )
	{
		m_iSelIndex = nIndex;
		g_pDrawPrim->SetRGBA( &m_PolyBar, 0xFFFFFFFF );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x+12.f, (float)ptItemPos->y, m_nWidth, m_nItemHeight+1.2f );
		g_pDrawPrim->SetTexture( m_hBar );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
		if(!m_bJustOnce) m_bJustOnce = LTTRUE;
	}
	else if( bOver )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, 0x70FFFFFF );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x+12.f, (float)ptItemPos->y, m_nWidth, m_nItemHeight+1.2f );
		g_pDrawPrim->SetTexture( m_hBar );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}
	
	g_pDrawPrim->EndDrawPrim();
	m_pResolutionText[nIndex].Render();

	if(m_bJustOnce)  // 만약에 하나도 선택이 안됐다면(원인:마지막 Item이 선택이 안되는 경우가 있슴.)
	{
		if(m_nSelIndex == -1)
			m_nSelIndex = 5;
	}	
	return LTTRUE;
}

ResolutionItem_Ptr	
CSaListResolution::GetResolutionItem(int iIndex)
{
	if( iIndex < 0 ) iIndex = m_nSelIndex;
	
	if( iIndex < 0 || iIndex >= (int)m_ItemList.size() )
	{
		iIndex = 1; //Default 800 * 600
	}

	return (ResolutionItem_Ptr)m_ItemList[iIndex];
}

VOID
CSaListResolution::RenderItemText()
{
	int					i, nIndex = 0;
	ResolutionItem_Ptr	pResolution;
	char szTemp[256] = {LTNULL}, szChangeForm[64] = {LTNULL};
	if( m_nFirstItemIndex < 0 || m_nLastItemIndex < 0 ) return;
	
	i = m_nFirstItemIndex;
	while (i <= m_nLastItemIndex)
	{
		if(pResolution = (ResolutionItem_Ptr)m_ItemList[i])
		{
			if(pResolution->m_dwWidth >= 1000)
				strcpy(szChangeForm, "%d  x  %d  x  %d(bit)");
			else
				strcpy(szChangeForm, " %d  x  %d  x  %d(bit)");
			wsprintf( szTemp, szChangeForm, pResolution->m_dwWidth, pResolution->m_dwHeight,
				pResolution->m_dwBitDepth);
			
			SetResolutionText(nIndex, szTemp);
		}
		nIndex++;
		i++;
	}
}

VOID
CSaListResolution::AddTextData(ResolutionItem_Ptr pResolution_Item)
{
	char szTemp[256] = {LTNULL}, szChangeForm[64] = {LTNULL};
	if(pResolution_Item->m_dwWidth >= 1000)
		strcpy(szChangeForm, "%d  x  %d  x  %d(bit)");
	else
		strcpy(szChangeForm, " %d  x  %d  x  %d(bit)");

	wsprintf( szTemp, szChangeForm, pResolution_Item->m_dwWidth, pResolution_Item->m_dwHeight,
							pResolution_Item->m_dwBitDepth);
	SetResolutionText(pResolution_Item->m_dwIndex, szTemp);
}



// CSaListControlText Class ===================================================
CSaListControlText::CSaListControlText()
: CLTGUIListView() {}

CSaListControlText::~CSaListControlText()
{
	this->Term();
}

void CSaListControlText::DeleteAllItem()
{
	ControlTextItem_Ptr ptr;

	if( !m_ItemList.empty() )
	{
		for( LTLV_ITEM_LIST_ITR itr=m_ItemList.begin() ; itr!=m_ItemList.end() ; ++itr )
		{
			ptr = (ControlTextItem_Ptr)(*itr);
			if( ptr )
			{
				delete ptr;
				ptr = LTNULL;
			}			
		}

		m_ItemList.clear();
	}

	CLTGUIListView::DeleteAllItem();
}

LTBOOL				
CSaListControlText::Create( uint16 nPosX, uint16 nPosY, uint16 nWidth, 
							uint16 nHeight, uint16 nItemHeight, uint32 nCommandID)
{
	CLTGUIListView::Create( nPosX, nPosY, nWidth, nHeight, nItemHeight );
	CLTGUICtrl::Create(nCommandID, LTNULL, LTNULL, LTNULL);

	m_hBar	= g_pInterfaceResMgr->GetTexture("SA_INTERFACE\\Menu\\Textures\\Main\\RoomBar.dtx");
	g_pDrawPrim->SetUVWH( &m_PolyBar, 0.0f, 0.0f, 577.0f/1024.0f, 21.0/32.0f );	
	
	return  LTTRUE;
}

LTBOOL 
CSaListControlText::OnWheelUp(int x, int y)
{
	if(!m_bWaitingKey)
		return ScrollUp();
	else return LTFALSE;
}

LTBOOL 
CSaListControlText::OnWheelDown(int x, int y)
{
	if(!m_bWaitingKey)
		return ScrollDown();
	else return LTFALSE;
}

LTBOOL 
CSaListControlText::AddControlText( ControlTextItem_Ptr pControlText_Item )
{
	return InsertItem( (void *)pControlText_Item );
}

LTBOOL	
CSaListControlText::SetCommandText(uint8 nIndex, char* szText)
{
	if(nIndex >= m_ItemList.size() || nIndex < 0 || !szText) 
		return LTFALSE;
	
	ControlTextItem_Ptr pKeySetting = (ControlTextItem_Ptr)m_ItemList[nIndex];
	if( pKeySetting )
	{
		pKeySetting->Ctrl_Command.DrawD3DText(szText, strlen(szText), DT_LEFT, D3D_TEXT_SHADOW, true);
	}
		
	return LTTRUE;
}

LTBOOL 
CSaListControlText::RenderItem( void* pListItem, const LTIntPt* ptItemPos, int32 nIndex, bool bSelect, bool bOver )
{
	ControlTextItem_Ptr pControl_Item = (ControlTextItem_Ptr)pListItem;
	pControl_Item->Ctrl_Action.SetTextColor(RGB(250,210,160));
	pControl_Item->Ctrl_Command.SetTextColor(RGB(130,130,130));
	
	if( bSelect && !m_bWaitingKey ) 
	{
		LTRESULT hResult = g_pLTClient->StartDeviceTrack (DEVICETYPE_KEYBOARD | DEVICETYPE_MOUSE | DEVICETYPE_JOYSTICK, 9);
		if (hResult != LT_OK)
			g_pLTClient->EndDeviceTrack();
		else
		{
			m_bSelect	= bSelect;
			m_iIndex	= nIndex + m_nFirstItemIndex;
			m_iListVectorIndex = nIndex + m_nFirstItemIndex;
			strcpy(m_szSaveText, pControl_Item->Ctrl_Command.GetText());  // Text 저장 
			pControl_Item->Ctrl_Command.DrawD3DText( "", 0 );
			
			// 준비 완료 
			m_bWaitingKey	= TRUE;
		}		
	}
	else if( bSelect && m_bWaitingKey )
	{
		g_pDrawPrim->SetRGBA( &m_PolyBar, 0xFF505050 );
		g_pDrawPrim->SetXYWH( &m_PolyBar, (float)ptItemPos->x-5.0f, (float)ptItemPos->y-3.0f, m_nWidth-5.0f, m_nItemHeight-1.f );
		g_pDrawPrim->DrawPrim(&m_PolyBar);
	}
	else if( bOver && !m_bWaitingKey)
	{
		pControl_Item->Ctrl_Action.SetTextColor(RGB(250,170,60));
		pControl_Item->Ctrl_Command.SetTextColor(RGB(210,210,210));
		m_bOver		= bOver;
	}

	pControl_Item->Ctrl_Action.SetPosition( ptItemPos->x, ptItemPos->y );
	pControl_Item->Ctrl_Command.SetPosition( ptItemPos->x+150, ptItemPos->y );

	pControl_Item->Ctrl_Action.Render();
	pControl_Item->Ctrl_Command.Render();

	return LTTRUE;
}


ControlTextItem_Ptr 
CSaListControlText::GetSelectedKeySettingData(int iIndex)
{
	if( iIndex < 0 ) return LTNULL;
	return (ControlTextItem_Ptr)m_ItemList[iIndex];
}


/////////////////////////////////////////////////////////////////////////////////////
//	드레그 보조버튼	/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
CDragSupportButton::CDragSupportButton()
:CLTGUIButton(){} 
CDragSupportButton::~CDragSupportButton()
{
}
LTFLOAT	
CDragSupportButton::CalculateDragButtonRatio(int x)
{
	LTFLOAT fRatio = 0.0f;
	fRatio = (LTFLOAT)(x - m_pos.x)/(LTFLOAT)m_baseWidth;

	return fRatio;
}
