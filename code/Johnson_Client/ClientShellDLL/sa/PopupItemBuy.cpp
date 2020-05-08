#include "stdafx.h"
#include "..\\clientshellshared\\GameText.h"
#include "..\\clientshellshared\\ClientSoundMgr.h"
#include "ScreenCommands.h"
#include "PopupItemBuy.h"
#include "ScreenInventory.h"
#include "GameText.h"
#include "GameMessage.h"


CPopUpItemBuy*	g_pPopupItemBuy	= LTNULL;

namespace
{
	enum eLocalCommands
	{
		CMD_POPUP_ITEM_BUY_OK	= CMD_CUSTOM+300,
		CMD_POPUP_ITEM_BUY_CANCEL,
		CMD_POPUP_ITEM_CHECK_0,
		CMD_POPUP_ITEM_CHECK_1,
		CMD_POPUP_ITEM_CHECK_2,
		CMD_POPUP_ITEM_CHECK_3,
	};

	LTIntPt		g_BtnOkPos[2];
	LTIntPt		g_BtnCancelPos[2];
	LTIntPt		g_CheckPos[2];
	LTIntPt		g_FontPos[2];
}

CPopUpItemBuy::CPopUpItemBuy()
{
	memset( m_pFontPrice,		0,	sizeof(CSAD3DText*)*MAX_PRICE_SET );
	memset( m_pCheckPrice,		0,	sizeof(CLTGUIButton*)*MAX_PRICE_SET );
	m_pFontTitle		= LTNULL;
	m_pBtnOk			= LTNULL;
	m_pBtnCancel		= LTNULL;	
	m_pFrmItemWp		= LTNULL;
	m_pFrmItemCh		= LTNULL;
	m_pFrmItemChBack	= LTNULL;
	m_pFrmItemWpBack	= LTNULL;

	g_pPopupItemBuy = this;
}

CPopUpItemBuy::~CPopUpItemBuy()
{
	Term();
}

void CPopUpItemBuy::Init(CLTGUICommandHandler* pCommandHandler)
{
	m_pCommandHandler = pCommandHandler;
	char szStr[128] = {0, };
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName( SCREEN_ID_INVENTORY );
	g_pLayoutMgr->GetString(pScreenTag, "ItemBuyBackTx", szStr, 128 );
	HTEXTURE hFrameTex	  = g_pInterfaceResMgr->GetTexture(szStr);
	Create(hFrameTex, 800, 600, 1, 1, LTTRUE);
	LTIntPt basePos(0,0);
	SetBasePos(basePos);
	Show(LTFALSE);

	m_pFrmItemChBack	= AddFrameRect(pScreenTag, "FrmChBackRect", "FrmChBackTx", LTTRUE);
	m_pFrmItemChBack->Show(LTFALSE);
	m_pFrmItemWpBack	= AddFrameRect(pScreenTag, "FrmWpBackRect", "FrmWpBackTx", LTTRUE);
	m_pFrmItemWpBack->Show(LTFALSE);

	m_pFrmItemWp		= AddFrameRect(pScreenTag, "FrmEmptyWpRt", "FrmEmptyWpTx", LTTRUE);
	m_pFrmItemCh		= AddFrameRect(pScreenTag, "FrmEmptyChRt", "FrmEmptyChTx", LTTRUE);

	for( int i = 0; i < MAX_PRICE_SET; i++)
	{		
		m_pCheckPrice[i] = new CLTGUIButton();
		sprintf( szStr, "PopCheck%d", i);
		//AddCheckButton(pScreenTag, m_pCheckPrice[i], CMD_POPUP_ITEM_CHECK_0+i, szStr, "CheckOff", "CheckOn");
		AddCheckButton(pScreenTag, m_pCheckPrice[i], CMD_POPUP_ITEM_CHECK_0+i, szStr, "PopNonCheckTx", "PopCheckTx");
		m_pCheckPrice[i]->SetClickSound( (int)IS_CLICK_NORMAL );
		m_pCheckPrice[i]->SetParam1(i);
	}

	g_BtnOkPos[0].x =  312;		g_BtnOkPos[0].y = 365;
	g_BtnOkPos[1].x =  301;		g_BtnOkPos[1].y = 434;
	g_BtnCancelPos[0].x = 433;	g_BtnCancelPos[0].y = 365;
	g_BtnCancelPos[1].x = 401;	g_BtnCancelPos[1].y = 434;
	g_CheckPos[0].x = 412;		g_CheckPos[0].y = 284;
	g_CheckPos[1].x = 372;		g_CheckPos[1].y = 295;
	g_FontPos[0].x = 433;		g_FontPos[0].y = 284;
	g_FontPos[1].x = 393;		g_FontPos[1].y = 295;

	m_pBtnOk		= new CLTGUIButton();
	AddButton(pScreenTag, m_pBtnOk, CMD_POPUP_ITEM_BUY_OK, "BtnBuyOkNTx", "BtnBuyOkHTx", "BtnBuyOkDTx", "BtnBuyOkCTx", "BtnBuyOkRt");
	m_pBtnOk->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtnCancel	= new CLTGUIButton();
	AddButton(pScreenTag, m_pBtnCancel, CMD_POPUP_ITEM_BUY_CANCEL, "BtnBuyCancelNTx", "BtnBuyCancelHTx", "BtnBuyCancelDTx", "BtnBuyCancelCTx", "BtnBuyCancelRt");
	m_pBtnCancel->SetClickSound( (int)IS_CLICK_NORMAL );

	for( i = 0; i < MAX_PRICE_SET; i++)
	{
		m_pFontPrice[i] = new CSAD3DText();
		m_pFontPrice[i]->CreateD3DText( 128, 16, true );
		m_pFontPrice[i]->SetPosition( 433, 284 + i*17 );
		m_pFontPrice[i]->SetTransparent( true );
		m_pFontPrice[i]->SetTextColor(RGB(153,193,190));
		m_pFontPrice[i]->DrawD3DText("", lstrlen(""));
	}

	m_pFontTitle = new CSAD3DText();
	m_pFontTitle->CreateD3DText( 128, 16, true );
	m_pFontTitle->SetPosition( 300, 255 );
	m_pFontTitle->SetTransparent( true );
	m_pFontTitle->SetTextColor(RGB(255,255,255));
	m_pFontTitle->DrawD3DText("", lstrlen(""));
}

void CPopUpItemBuy::Term()
{
	SAFE_DELETE(m_pFontTitle);
	for( int i = 0; i < 4; i++ )
	{
		SAFE_DELETE(m_pFontPrice[i]);
	}
}

void CPopUpItemBuy::CreateBackeFrame( eScreenID nScreen )
{
}

void CPopUpItemBuy::OnFocus(LTBOOL bFocus)
{
	if( bFocus )
	{
		CheckButtonOrder(0);
	}
	else
	{
	}
}

void CPopUpItemBuy::CheckButtonOrder(uint8 nOrder)
{
	for( int i = 0; i < 4; i++ )
	{
		if( i == nOrder )
		{
			m_pCheckPrice[i]->SetCheck(LTTRUE);
		}
		else
		{
			m_pCheckPrice[i]->SetCheck(LTFALSE);
		}
		
	}
}

void CPopUpItemBuy::Render()
{
	if( !IsVisible() ) return;

	// test
	//ShowItemWeapon(LTTRUE);
	//ShowItemChar(LTTRUE);

	CLTGUIWindow::Render();

	for( int i = 0; i < 4; i++ )
	{
		m_pFontPrice[i]->Render();
	}
	m_pFontTitle->Render();
}

uint32 CPopUpItemBuy::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch( dwCommand )
	{
		case CMD_POPUP_ITEM_BUY_OK :
			{
#ifndef _FINAL
#ifdef DEBUG_SHOP
				CtrlHide();
				return 1;
#endif
#endif
				if( m_pFrmItemWpBack->IsVisible() )
				{
					int nDuration = m_pWeaponSet->PriceSet[GetButtonOrder()].nDuration;
					int nPrice	  = m_pWeaponSet->PriceSet[GetButtonOrder()].nPrice;

					if( nPrice > g_GameDoc.GetMoney() )
					{
						g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_FAIL_DEFICIT,
														 LTMB_OK, LTNULL, LTNULL );
					}
					else
					{
						g_Network.CS_BuyItem( m_pWeaponSet->szClass1[0],
											  m_pWeaponSet->szClass2[0], m_pWeaponSet->szClass2[1],
											  m_pWeaponSet->nWeaponId,
											  nDuration,
											  m_pWeaponSet->bDefaultItem,
											  m_pWeaponSet->bBuyItem );
						g_pInterfaceMgr->ShowMessageBox( SA_TEXT_WAIT_ITEM_BUY, LTMB_MESSAGE, LTNULL, LTNULL );
					}
				}
				else if( m_pFrmItemChBack->IsVisible() )
				{	
					int nDuration = m_pCharacterSet->PriceSet[GetButtonOrder()].nDuration;
					int nPrice	  = m_pCharacterSet->PriceSet[GetButtonOrder()].nPrice;

					if( nPrice > g_GameDoc.GetMoney() )
					{
						g_pInterfaceMgr->ShowMessageBox( SA_TEXT_ITEM_FAIL_DEFICIT,
														 LTMB_OK, LTNULL, LTNULL );
					}
					else
					{
						g_Network.CS_BuyItem( m_pCharacterSet->szClass1[0],
											  m_pCharacterSet->szClass2[0], m_pCharacterSet->szClass2[1],
											  m_pCharacterSet->nItemId,
											  nDuration,
											  m_pCharacterSet->bDefaultItem,
											  m_pCharacterSet->bBuyItem );
						g_pInterfaceMgr->ShowMessageBox( SA_TEXT_WAIT_ITEM_BUY, LTMB_MESSAGE, LTNULL, LTNULL );
					}
				}
				CtrlHide();
			}break;
		case CMD_POPUP_ITEM_BUY_CANCEL :
			{
				this->CtrlHide();
			}break;
		case CMD_POPUP_ITEM_CHECK_0 :
		case CMD_POPUP_ITEM_CHECK_1 :
		case CMD_POPUP_ITEM_CHECK_2 :
		case CMD_POPUP_ITEM_CHECK_3 :
			{
				CheckButtonOrder(dwParam1);
			}break;
	}
	return 1;
}

void CPopUpItemBuy::ShowItemWeapon(LTBOOL bShow)
{
	m_pFrmItemWpBack->Show(bShow);	
	m_pFrmItemWp->Show(bShow);	
}

void CPopUpItemBuy::ShowItemChar(LTBOOL bShow)
{
	m_pFrmItemChBack->Show(bShow);
	m_pFrmItemCh->Show(bShow);
}

void CPopUpItemBuy::SetWeaponItemInfo( sShopWeapon_Set* pWeapon )
{
	ShowItemWeapon(LTTRUE);
	ShowItemChar(LTFALSE);

	m_pWeaponSet	= pWeapon;
	
	m_pBtnOk->SetBasePos(g_BtnOkPos[0].x, g_BtnOkPos[0].y);
	m_pBtnCancel->SetBasePos(g_BtnCancelPos[0].x, g_BtnCancelPos[0].y);

	m_pFontTitle->DrawD3DText(m_pWeaponSet->szName, lstrlen(m_pWeaponSet->szName), DT_LEFT, D3D_TEXT_OUTLINE );

	if( m_pWeaponSet->hSelectTx != LTNULL )
	{
		m_pFrmItemWp->SetFrame(m_pWeaponSet->hSelectTx);		
	}

	char szTemp[64] = {0, };
	for( int i=0 ; i<MAX_PRICE_SET ; ++i )
	{
		if( i<=m_pWeaponSet->nCount )
		{
			m_pCheckPrice[i]->Show(LTTRUE);
			m_pCheckPrice[i]->SetBasePos(g_CheckPos[0].x, g_CheckPos[0].y + i*17 );
			m_pFontPrice[i]->SetPosition(g_FontPos[0].x, g_FontPos[0].y + i*17 );

			sprintf(szTemp, NF_GetMsg(SA_TEXT_PRICE),
							m_pWeaponSet->PriceSet[i].nDuration, m_pWeaponSet->PriceSet[i].nPrice );
			m_pFontPrice[i]->DrawD3DText(szTemp, lstrlen(szTemp));	
		}
		else
		{
			m_pCheckPrice[i]->Show(LTFALSE);
			m_pFontPrice[i]->DrawD3DText( "", 0 );
		}
	}
}

void CPopUpItemBuy::SetCharItemInfo( sShopChar_Set* pCharacter )
{
	ShowItemWeapon(LTFALSE);
	ShowItemChar(LTTRUE);

	m_pCharacterSet	= pCharacter;

	m_pBtnOk->SetBasePos(g_BtnOkPos[1].x, g_BtnOkPos[1].y);
	m_pBtnCancel->SetBasePos(g_BtnCancelPos[1].x, g_BtnCancelPos[1].y);

	m_pFontTitle->DrawD3DText(m_pCharacterSet->szName, lstrlen(m_pCharacterSet->szName), DT_LEFT, D3D_TEXT_OUTLINE );

	if( m_pCharacterSet->hNormalTx != LTNULL )
	{
		m_pFrmItemCh->SetFrame(m_pCharacterSet->hNormalTx);		
	}

	char szTemp[64] = {0, };
	for( int i=0 ; i<MAX_PRICE_SET ; ++i )
	{
		if( i<=m_pCharacterSet->nCount )
		{
			m_pCheckPrice[i]->Show(LTTRUE);
			m_pCheckPrice[i]->SetBasePos(g_CheckPos[1].x, g_CheckPos[1].y + i*24 );
			m_pFontPrice[i]->SetPosition(g_FontPos[1].x, g_FontPos[1].y + i*24 );

			sprintf(szTemp, NF_GetMsg(SA_TEXT_PRICE),
							m_pCharacterSet->PriceSet[i].nDuration, m_pCharacterSet->PriceSet[i].nPrice );
			m_pFontPrice[i]->DrawD3DText(szTemp, lstrlen(szTemp));	
		}
		else
		{
			m_pCheckPrice[i]->Show(LTFALSE);
			m_pFontPrice[i]->DrawD3DText( "", 0 );
		}
	}
}

uint8 CPopUpItemBuy::GetButtonOrder()
{
	for( int i = 0; i < MAX_PRICE_SET; i++ )
	{
		if( m_pCheckPrice[i]->GetCheck() )
		{
			return i;
		}
	}
	return 0;
}