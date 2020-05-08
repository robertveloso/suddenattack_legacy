// ----------------------------------------------------------------------- //
//
// MODULE  : BaseScreen.cpp
//
// PURPOSE : Base class for interface screens
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "ScreenMgr.h"
#include "ScreenCommands.h"
#include "SoundMgr.h"
#include "VKDefs.h"
#include "GameClientShell.h"
#include "TransitionFXMgr.h"
#include "FXButeMgr.h"

#include "BaseScreen.h"

LTVector	CBaseScreen::s_vPos; 
LTVector	CBaseScreen::s_vU; 
LTVector	CBaseScreen::s_vR; 
LTVector	CBaseScreen::s_vF;
LTRotation	CBaseScreen::s_rRot;

namespace
{
	LTIntPt offscreen(-64,-64);

	typedef std::map<std::string,CBaseScaleFX*> ScaleFXMap;
	ScaleFXMap g_ScaleFXMap;
	bool bEditSFXMode = false;
	CBaseScaleFX* pEditSFX = NULL;

	LTVector vSFXPos;
	LTVector vSFXScale;
	float fEditDelta = 1.0f;
	// Model
	#define INVALID_ANI			((HMODELANIM)-1)
	int g_ModelAniCount = 0;
}

void EditFXFn(int argc, char **argv)
{
	if(argc != 1)
	{
		g_pLTClient->CPrint("EditFX <fxname>");
		return;
	}

	char szName[128];
	SAFE_STRCPY(szName,argv[0]);
	_strupr(szName);
	ScaleFXMap::iterator iter = g_ScaleFXMap.find(szName);
	if (iter == g_ScaleFXMap.end())
	{
		g_pLTClient->CPrint("Could not find ScaleFX %s",argv[0]);
		return;
	}

	g_pLTClient->CPrint("Entering SFX edit mode for %s",argv[0]);
	pEditSFX = (*iter).second;

	HOBJECT hObj = pEditSFX->GetObject();

	g_pLTClient->GetObjectPos(hObj,&vSFXPos);
	g_pLTClient->GetObjectScale(hObj,&vSFXScale);
	bEditSFXMode = true;
	fEditDelta = 1.0f;
}

void HandleEditKey(int key);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseScreen::CBaseScreen()
{
    m_bInit = LTFALSE;
    m_bBuilt = LTFALSE;
    m_bHaveFocus = LTFALSE;
	m_bHaveLights = LTFALSE;

    m_pScreenMgr = LTNULL;

    m_nScreenID = SCREEN_ID_NONE;
	m_nContinueID = SCREEN_ID_NONE;

	// Array of controls that this screen owns
	m_controlArray.reserve(5);

	m_nSelection = kNoSelection;
	m_nOldSelection = kNoSelection;
    m_pCaptureCtrl = LTNULL;
	m_nRMouseDownItemSel =  kNoSelection;
	m_nRMouseDownItemSel =  kNoSelection;

	m_nNumAttachments = 0;

	m_fLastScale = 1.0f;
	m_bSelectFXCenter = LTTRUE;

	m_bVisited = LTFALSE;

	m_bUseScreenAfterFX = false;

	m_nTooltipSelIndex	= kNoSelection;
	m_bDrawTooltip		= LTFALSE;
	m_pTooltipText		= LTNULL;
	m_MousePt.x			= 0;
	m_MousePt.y			= 0;
	m_pDummyCtrl		= LTNULL;
}

CBaseScreen::~CBaseScreen()
{
	if ( m_bInit )
	{
		Term();
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::Init
//
//	PURPOSE:	initialize the screen
//
// ----------------------------------------------------------------------- //

LTBOOL CBaseScreen::Init(int nScreenID)
{
	m_nScreenID=nScreenID;
	m_pScreenMgr = g_pInterfaceMgr->GetScreenMgr();

	/*
	char szAttName[30];
	char szIntroFXName[128] = "";
	char szShortFXName[128] = "";
	char szLoopFXName[128] = "";
	int nFXNum = 0;
	bool bFound = false;
	do
	{
		szIntroFXName[0] = 0;
		szShortFXName[0] = 0;
		szLoopFXName[0] = 0;
		bFound = false;

		sprintf(szAttName,"IntroFX%d",nFXNum);
		if (g_pLayoutMgr->HasCustomValue((eScreenID)m_nScreenID,szAttName))
		{
			g_pLayoutMgr->GetScreenCustomString((eScreenID)m_nScreenID,szAttName,szIntroFXName,128);
		}
		sprintf(szAttName,"ShortIntroFX%d",nFXNum);
		if (g_pLayoutMgr->HasCustomValue((eScreenID)m_nScreenID,szAttName))
		{
			g_pLayoutMgr->GetScreenCustomString((eScreenID)m_nScreenID,szAttName,szShortFXName,128);
		}
		sprintf(szAttName,"LoopFX%d",nFXNum);
		if (g_pLayoutMgr->HasCustomValue((eScreenID)m_nScreenID,szAttName))
		{
			g_pLayoutMgr->GetScreenCustomString((eScreenID)m_nScreenID,szAttName,szLoopFXName,128);
		}
		if (strlen(szIntroFXName) || strlen(szShortFXName) || strlen(szLoopFXName))
		{
			nFXNum++;
			bFound = true;

			CChainedFX *pChain = debug_new(CChainedFX);
			pChain->Init(szIntroFXName,szShortFXName,szLoopFXName);
			m_Chains.push_back(pChain);
		}

	} while (bFound);
	*/

	m_pTooltipText = new CSAD3DText();
	m_pTooltipText->CreateD3DText( 256, 16, false );
	m_pTooltipText->SetTextColor( RGB(255,255,255) );
	m_pTooltipText->SetTransparent(true);

	g_pDrawPrim->SetRGBA( &m_PolyTooltipBorder, 0xFFFFFFFF );
	g_pDrawPrim->SetRGBA( &m_PolyTooltipBack, 0xFF000000 );

	m_bInit=TRUE;
    return LTTRUE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::Term
//
//	PURPOSE:	Terminate the screen
//
// ----------------------------------------------------------------------- //

void CBaseScreen::Term()
{
	RemoveAll();

	/*
	ChainFXList::iterator iter = m_Chains.begin();
	while (iter != m_Chains.end())
	{
		debug_delete(*iter);
		iter++;
	}
	m_Chains.clear();
	*/

	if( m_pTooltipText )
	{
		delete m_pTooltipText;
		m_pTooltipText = LTNULL;
	}

	m_bBuilt	= FALSE;
	m_bInit		= FALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::Escape
//
//	PURPOSE:	back out of the screen
//
// ----------------------------------------------------------------------- //

void CBaseScreen::Escape()
{
	if (!m_pScreenMgr->PreviousScreen())
	{
        HLOCALOBJ hPlayerObj = g_pLTClient->GetClientObject();
		if (g_pPlayerMgr->IsPlayerInWorld() &&
			(!g_pPlayerMgr->IsPlayerDead() || IsMultiplayerGame( )) )
		{
			g_pInterfaceMgr->ChangeState(GS_PLAYING);
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::Render
//
//	PURPOSE:	Renders the screen to a surface
//
// ----------------------------------------------------------------------- //

LTBOOL CBaseScreen::Render(HSURFACE hDestSurf)
{
	if (!hDestSurf)
	{
        return LTFALSE;
	}

	LTIntPt tmpPos;

	for (uint16 i = 0; i < m_controlArray.size(); i++ )
	{
		m_controlArray[i]->Render ();
	}

	if( GetSelection() != kNoSelection && g_pLTClient->GetTime() - m_fStartSel > 1.0f )
	{
		CLTGUICtrl* pCtrl = GetControl(GetSelection());
		if( pCtrl && pCtrl->IsEnableTooltip() && m_nTooltipSelIndex != GetSelection() )
		{
			float fX, fY, fW, fH=15.0f;

			m_nTooltipSelIndex = GetSelection();
			m_pTooltipText->DrawD3DText( pCtrl->GetTooptip(), strlen(pCtrl->GetTooptip()) );
			fW = (float)m_pTooltipText->GetTextLength()+4.0f;
						
			fX = m_MousePt.x - (fW*0.5f);
			fY = m_MousePt.y - fH - 5.0f;

			if( fX < 0.0f ) fX = 0.0f;
			if( fX+fW > USABLE_WIDTH_F ) fX = USABLE_WIDTH_F-fW;
			if( fY < 0.0f ) fY = 0.0f;
			if( fY+fH > USABLE_HEIGHT_F ) fY = USABLE_HEIGHT_F-fH;

			m_pTooltipText->SetPosition( fX+1.0f, fY+1.0f );
			g_pDrawPrim->SetXYWH( &m_PolyTooltipBack, fX, fY, fW, fH );
			g_pDrawPrim->SetXYWH( &m_PolyTooltipBorder, fX-1.0f, fY-1.0f, fW+2.0f, fH+2.0f );
		}

		//Render
		if( m_bDrawTooltip && GetSelection() == m_nTooltipSelIndex )
		{
			g_pDrawPrim->SetTexture( LTNULL );
			g_pDrawPrim->DrawPrim( &m_PolyTooltipBorder );
			g_pDrawPrim->DrawPrim( &m_PolyTooltipBack );
			m_pTooltipText->Render();
		}
	}

	g_pLTClient->EndOptimized2D();
	g_pInterfaceMgr->DrawSFX();
	g_pLTClient->StartOptimized2D();

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::Build
//
//	PURPOSE:	Construct the basic screen elements
//
// ----------------------------------------------------------------------- //

LTBOOL CBaseScreen::Build()
{
    m_bBuilt=LTTRUE;
	return TRUE;
}

// Handles a user entered character
LTBOOL CBaseScreen::HandleChar(unsigned char c)
{
    LTBOOL handled = LTFALSE;

	if (m_pCaptureCtrl)
	{
		if (m_pCaptureCtrl->HandleChar(c))
            handled = LTTRUE;
	}
	return handled;
}

LTBOOL CBaseScreen::HandleHotKey( unsigned char c )
{
	if( m_pDummyCtrl && m_pDummyCtrl->IsVisible() ) return LTFALSE;

	CLTGUICtrl* pCtrl;

	for( uint32 i=0 ; i<m_controlArray.size() ; ++i )
	{
		pCtrl = (CLTGUICtrl*)m_controlArray[i];
		if( pCtrl )
		{
			if( pCtrl->IsHotKey( c ) )
			{
				this->OnCommand( pCtrl->GetCommandID(), LTNULL, LTNULL );
				return LTTRUE;
			}
		}
	}
	
	return LTFALSE;
}

void CBaseScreen::AddDummyCtrl()
{
	if( !m_pDummyCtrl )
	{
		m_pDummyCtrl = new CLTGUIDummyCtrl();
		m_pDummyCtrl->Show(LTFALSE);
		AddControl( m_pDummyCtrl );
	}
}

void CBaseScreen::EnablePopup( LTBOOL bEnable, CLTGUICtrl* pCaptureCtrl )
{
	if( bEnable )
	{
		m_pCaptureCtrl = pCaptureCtrl;
		PrePopupState();
	}
	else
	{
		m_pCaptureCtrl = LTNULL;
		PostPopupState();
	}

	if( m_pDummyCtrl )
	{
		m_pDummyCtrl->Show( bEnable );
	}
}


// Handles a key press.  Returns FALSE if the key was not processed through this method.
// Left, Up, Down, Right, and Enter are automatically passed through OnUp(), OnDown(), etc.
LTBOOL CBaseScreen::HandleKeyDown(int key, int rep)
{
    LTBOOL handled = LTFALSE;

	if (bEditSFXMode)
	{
		HandleEditKey(key);
		return LTTRUE;
	}


	switch (key)
	{
	case VK_UP:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->OnUp();
			else
				handled = OnUp();
			break;
		}
	case VK_DOWN:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->OnDown();
			else
				handled = OnDown();
			break;
		}
		
	case VK_RETURN:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->OnEnter();
			else
				handled = OnEnter();
			break;
		}
		
	case VK_ESCAPE:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->OnEscape();
			else
			{
				//[SPIKE] KeySetting 변경시 Escape기능 추가 
				if(LTFALSE == OnEscape())
				{
					CLTGUICtrl* pCtrl = GetSelectedControl();
					if( pCtrl )
					{
						handled = pCtrl->HandleKeyDown(key,rep);
					}
					else
					{
						handled = LTFALSE;
					}
				}
				else
				{
					handled = LTTRUE;
				}
			}
			break;			
		}
	case VK_F1: //[yaiin]
		{
			//ScreenMain이나 ScreenRoom에 HandleKeyDown()을 재정의하면 다른 팝업에 문제가 생김
			if(SCREEN_ID_MAIN == this->GetScreenID())
				OnCommand(CMD_HELP, LTNULL, LTNULL);
			else if(SCREEN_ID_SCREEN_ROOM == this->GetScreenID())
				OnCommand(CMD_ROOM_BTN_HELP, LTNULL, LTNULL);

			break;
		}
	default:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->HandleKeyDown(key,rep);
			else
			{
				CLTGUICtrl* pCtrl = GetSelectedControl();
				if (pCtrl)
				{
					handled = pCtrl->HandleKeyDown(key,rep);
				}
				else
					handled = LTFALSE;
			}
			break;
		}
	}

	// Handled the key
	return handled;
}


/******************************************************************/


/******************************************************************/

LTBOOL CBaseScreen::OnUp()
{
	return LTFALSE;
}

/******************************************************************/

LTBOOL CBaseScreen::OnDown()
{
	return LTFALSE;
}

/******************************************************************/

LTBOOL CBaseScreen::OnLeft()
{
   return LTFALSE;
}


/******************************************************************/

LTBOOL CBaseScreen::OnRight()
{
    return LTFALSE;
}


/******************************************************************/

LTBOOL CBaseScreen::OnEnter()
{
	LTBOOL handled = LTFALSE;
    
	CLTGUICtrl* pCtrl = GetSelectedControl();
	if (pCtrl)
	{
		handled = pCtrl->OnEnter();
	}
	return handled;
}

LTBOOL CBaseScreen::OnEscape()
{
	return LTFALSE;
}

uint16 CBaseScreen::NextSelection()
{
	uint16 select = m_nSelection;
	if (select == kNoSelection)
		select = m_controlArray.size()-1;
	uint16 oldSelect = select;
	
	CLTGUICtrl* pCtrl = LTNULL;	
	do
	{
		select++;
		if (select >= m_controlArray.size())
		{
			select = 0;
		}
	
		pCtrl = GetControl(select);	

	} while (select != oldSelect && pCtrl && !pCtrl->IsEnabled() );


	if (!pCtrl || !pCtrl->IsEnabled() )
		select = m_nSelection;

	return SetSelection(select);

}

uint16 CBaseScreen::PreviousSelection()
{
	uint16 select = m_nSelection;
	if (select == kNoSelection)
		select = 0;
	uint16 oldSelect = select;
	
	CLTGUICtrl* pCtrl = LTNULL;	
	do
	{
		if (select == 0)
		{
			select = m_controlArray.size()-1;
		}
		else
			select--;
	
		pCtrl = GetControl(select);	

	} while (select != oldSelect && pCtrl && !pCtrl->IsEnabled() );


	if (!pCtrl || !pCtrl->IsEnabled() )
		select = m_nSelection;

	return SetSelection(select);

}

/******************************************************************/
LTBOOL CBaseScreen::OnLButtonDown(int x, int y)
{
	// Get the control that the click was on
	uint16 nControlIndex=0;
	if (GetControlUnderPoint(x, y, &nControlIndex))
	{
		CLTGUICtrl* pCtrl = GetControl(nControlIndex);
		if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
            return LTFALSE;

		// Select the control
		SetSelection(nControlIndex);

		// Record this control as the one being selected from the mouse click.
		// If the mouse is still over it on the UP message, then the "enter" message will be sent.
		m_nLMouseDownItemSel=nControlIndex;
		m_bDrawTooltip	= LTFALSE;
		
		if( pCtrl->IsPlaySound() )
			g_pInterfaceMgr->RequestInterfaceSound( (InterfaceSound)pCtrl->GetClickSound() );
		
		return pCtrl->OnLButtonDown(x,y);

	}
	else
		m_nLMouseDownItemSel=kNoSelection;

	return LTFALSE;
}

/******************************************************************/
LTBOOL CBaseScreen::OnLButtonUp(int x, int y)
{
	// Get the control that the click was on
	uint16 nControlIndex=0;
	if (GetControlUnderPoint(x, y, &nControlIndex))
	{
		CLTGUICtrl* pCtrl = GetControl(nControlIndex);
		if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
            return LTFALSE;
		// If the mouse is over the same control now as it was when the down message was called
		// then send the "enter" message to the control.
		if (nControlIndex == m_nLMouseDownItemSel)
		{
			if (pCtrl->IsEnabled() )
			{
				SetSelection(nControlIndex);
				return pCtrl->OnLButtonUp(x,y);

			}
		}
	}
	else
	{
		m_nLMouseDownItemSel= kNoSelection;
	}

    return LTFALSE;
}

/******************************************************************/
LTBOOL CBaseScreen::OnRButtonDown(int x, int y)
{
	// Get the control that the click was on
	uint16 nControlIndex=0;
	if (GetControlUnderPoint(x, y, &nControlIndex))
	{
		CLTGUICtrl* pCtrl = GetControl(nControlIndex);
		if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
            return LTFALSE;

		// Select the control
		SetSelection(nControlIndex);

		// Record this control as the one being selected from the mouse click.
		// If the mouse is still over it on the UP message, then the "enter" message will be sent.
		m_nRMouseDownItemSel=nControlIndex;

		return pCtrl->OnRButtonDown(x,y);
	}
	else
		m_nRMouseDownItemSel=kNoSelection;

    return LTFALSE;
}

/******************************************************************/
LTBOOL CBaseScreen::OnRButtonUp(int x, int y)
{
	// Get the control that the click was on
	uint16 nControlIndex=0;
	if (GetControlUnderPoint(x, y, &nControlIndex))
	{
		CLTGUICtrl* pCtrl = GetControl(nControlIndex);
		if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
            return LTFALSE;

		// If the mouse is over the same control now as it was when the down message was called
		// then send the "left" message to the control.
		if (nControlIndex == m_nRMouseDownItemSel)
		{
			if (pCtrl->IsEnabled())
			{
				SetSelection(nControlIndex);
				return pCtrl->OnRButtonUp(x,y);
			}
		}
	}
	else
	{
		m_nRMouseDownItemSel= kNoSelection;
	}
    return LTFALSE;
}

/******************************************************************/
LTBOOL CBaseScreen::OnLButtonDblClick(int x, int y)
{
	CLTGUICtrl* pCtrl = GetSelectedControl();
	if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
        return LTFALSE;

	if (pCtrl)
		return pCtrl->OnLButtonDblClick(x, y);
	else
        return LTFALSE;
}

/******************************************************************/
LTBOOL CBaseScreen::OnRButtonDblClick(int x, int y)
{
	CLTGUICtrl* pCtrl = GetSelectedControl();
	if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
        return LTFALSE;

	if (pCtrl)
		return pCtrl->OnRButtonDblClick(x, y);
	else
        return LTFALSE;
}

// SPIKE
LTBOOL
CBaseScreen::OnWheelUp(int x, int y)
{
	CLTGUICtrl* pCtrl = GetSelectedControl();
	if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
        return LTFALSE;
	if (pCtrl)
		return pCtrl->OnWheelUp(x, y);
	else
        return LTFALSE;
}
LTBOOL
CBaseScreen::OnWheelDown(int x, int y)
{
	CLTGUICtrl* pCtrl = GetSelectedControl();
	if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
        return LTFALSE;
	if (pCtrl)
		return pCtrl->OnWheelDown(x, y);
	else
        return LTFALSE;
}

void CBaseScreen::GetHelpString(uint32 dwHelpId, uint16 nControlIndex, char *buffer, int bufLen)
{
    LoadString(dwHelpId,buffer,bufLen);
}


/******************************************************************/
LTBOOL CBaseScreen::OnMouseMove(int x, int y)
{
	if (m_MousePt.x == x && m_MousePt.y == y) return LTTRUE;

	m_MousePt.x	= x;
	m_MousePt.y = y;

	uint16 nControlUnderPoint=kNoSelection;
    LTBOOL onCtrl = GetControlUnderPoint(x,y,&nControlUnderPoint);
	if (onCtrl)
	{
		CLTGUICtrl* pCtrl = GetControl(nControlUnderPoint);
		if (m_pCaptureCtrl && m_pCaptureCtrl != pCtrl)
            return LTFALSE;

		pCtrl->OnMouseMove(x,y);
	}
	else
    {
		if (m_pCaptureCtrl)
			return LTFALSE;

// Added By IN'T ==>
		SetSelection(kNoSelection);
// Added By IN'T <==
	}

	if (onCtrl)
	{				
		if (GetSelection() != nControlUnderPoint)
		{
			if (GetControl(nControlUnderPoint)->IsEnabled())
			{
				SetSelection(nControlUnderPoint);				
			}
		}
        return LTTRUE;
	}
    return LTFALSE;
}

uint16 CBaseScreen::SetSelection(uint16 select, LTBOOL bFindSelectable)
{
	if (select == m_nSelection) return select;

	int nOldSelect=m_nSelection;

	if (select == kNoSelection)
	{
		if (nOldSelect != kNoSelection)
		{
			GetControl(nOldSelect)->Select(LTFALSE);
			OnSelectionChange();
		}
		m_nOldSelection = m_nSelection;
		m_nSelection = kNoSelection;
		return kNoSelection;
	}


	CLTGUICtrl *pSelCtrl;


	if (select >= 0)
	{
		if (select >= m_controlArray.size())
			select = m_controlArray.size()-1;
	}


	pSelCtrl = GetControl(select);
	if (!pSelCtrl)
	{
		return nOldSelect;
	}
	//check to see if we can select this item
	if (!pSelCtrl->IsEnabled())
	{
		//if we don't need to find a selectable item return
		if (!bFindSelectable)
		{
			return nOldSelect;
		}

		//keep looking until we run out of on screen items or find a selectable one
		while (pSelCtrl && !pSelCtrl->IsEnabled())
		{
			select++;
			pSelCtrl = GetControl(select);
		}
		if (!pSelCtrl || !pSelCtrl->IsEnabled())
		{
			return nOldSelect;
		}
	}


	if (nOldSelect != kNoSelection)
	{
		GetControl(nOldSelect)->Select(LTFALSE);
	}

	m_nOldSelection = m_nSelection;
	m_nSelection = select;

	if (m_nSelection == kNoSelection)
	{
		return nOldSelect;
	}

	LTIntPt pos = pSelCtrl->GetPos();
	if (m_bSelectFXCenter)
		pos.x += (pSelCtrl->GetWidth() / 2);
	pos.y += (pSelCtrl->GetHeight() / 2);
	g_pInterfaceMgr->ShowSelectFX(pos);
	pSelCtrl->Select(LTTRUE);

	if (m_nSelection != nOldSelect && pSelCtrl->IsPlaySound() )
		g_pInterfaceMgr->RequestInterfaceSound( (InterfaceSound)pSelCtrl->GetOverSound() );


	m_fStartSel		= g_pLTClient->GetTime();
	m_bDrawTooltip	= LTTRUE;

	OnSelectionChange();
	return m_nSelection;
}


// Gets the index of the control that is under the specific screen point.
// Returns FALSE if there isn't one under the specified point.
LTBOOL CBaseScreen::GetControlUnderPoint(int xPos, int yPos, uint16 *pnIndex)
{
	_ASSERT(pnIndex);

	LTBOOL	bResult = LTFALSE;

	if (m_pCaptureCtrl && m_pCaptureCtrl->IsOnMe(xPos,yPos))
	{
		*pnIndex = GetIndex(m_pCaptureCtrl);
		return LTTRUE;
	}


	// See if the user clicked on any of the controls.
	for (uint16 i=0; i < m_controlArray.size() ; i++)
	{
		//start with last control
		int ndx = (m_controlArray.size()-1) - i;

		//[MURSUM]=================================================================
		//벡터의 뒤부터 검색~
		/*
		if (m_controlArray[i]->IsOnMe(xPos,yPos) && m_controlArray[i]->IsEnabled())
		{
			*pnIndex=i;

            return LTTRUE;
		}
		*/
		if ( m_controlArray[ndx]->IsOnMe(xPos,yPos) && m_controlArray[ndx]->IsEnabled() && !bResult )
		{
			*pnIndex=ndx;

			bResult = LTTRUE;
		}
		//=========================================================================
	}
	

    return bResult;
}

// Return a control at a specific index
CLTGUICtrl *CBaseScreen::GetControl ( uint16 nIndex )
{
	if (nIndex < m_controlArray.size() )
		return m_controlArray[nIndex];
    return LTNULL;
}





void CBaseScreen::RemoveAll(LTBOOL bDelete)
{
	// Terminate the ctrls
	if (bDelete)
	{
		for (uint16 i=0; i < m_controlArray.size(); i++)
		{
			CLTGUICtrl* pCtrl = (CLTGUICtrl*)m_controlArray[i];
			m_controlArray[i]->Destroy();
			debug_delete(m_controlArray[i]);
		}
	}
	m_controlArray.clear();
	if (m_nSelection >= 0)
		m_nSelection = kNoSelection;
}

uint16 CBaseScreen::AddControl(CLTGUICtrl* pCtrl)
{
	m_controlArray.push_back(pCtrl);
	uint16 num = m_controlArray.size();
	if (num == m_nSelection+1)
        pCtrl->Select(LTTRUE);
	if (num > 0)
		return num-1;
	else
		return kNoSelection;

}


// Calls UpdateData on each control in the menu
void CBaseScreen::UpdateData(LTBOOL bSaveAndValidate)
{
	for (uint16 i=0; i < m_controlArray.size(); i++)
	{
		m_controlArray[i]->UpdateData(bSaveAndValidate);
	}
}

void CBaseScreen::RemoveControl(CLTGUICtrl* pControl,LTBOOL bDelete)
{
	if (!pControl) return;

	ControlArray::iterator iter = m_controlArray.begin();

	while (iter != m_controlArray.end() && (*iter) != pControl)
		iter++;

	if (iter != m_controlArray.end())
	{
		m_controlArray.erase(iter);
	}

	if (bDelete)
	{
		debug_delete(pControl);
	}


}

uint32 CBaseScreen::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	return 0;
}


void CBaseScreen::ForceMouseUpdate()
{
//	SetSelection(kNoSelection);
    LTIntPt cPos = g_pInterfaceMgr->GetCursorPos();
	OnMouseMove(cPos.x,cPos.y);
}


// This is called when the screen gets or loses focus
void CBaseScreen::OnFocus(LTBOOL bFocus)
{
	//[MURSUM] InterfaceMgr.cpp로 이동
	// NOVICE : 뿌옇지 않은 유아이를 위해
	//WriteConsoleInt( "Anisotropic", 0 );
	//WriteConsoleInt( "Bilinear", 0 );

    m_pCaptureCtrl = LTNULL;

	bEditSFXMode = false;
	
	m_nTooltipSelIndex	= kNoSelection;	

	if (bFocus)
	{
		/*
		//[MURSUM] Screen은 800 * 600 으로 고정
		if (m_fLastScale != g_pInterfaceResMgr->GetXRatio())
		{
			ScreenDimsChanged();
		}
		*/
		m_nOldSelection = kNoSelection;
		//if (m_nSelection == kNoSelection)
		//{
			SetSelection(kNoSelection,LTTRUE);
		//}

		
		ForceMouseUpdate();
		CreateInterfaceSFX();
		m_bHaveFocus = LTTRUE;
		m_bVisited = LTTRUE; //set this last
	}
	else
	{
		SetSelection(kNoSelection);
		RemoveInterfaceSFX();
		m_bHaveFocus = LTFALSE;

	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::CreateInterfaceSFX
//
//	PURPOSE:	Create the SFX to render on this screen
//
// ----------------------------------------------------------------------- //

void CBaseScreen::CreateInterfaceSFX()
{

	int n = 0;
	char szAttName[30];
	char szFXName[128];

	m_bHaveLights = LTFALSE;


	HOBJECT hCamera = g_pInterfaceMgr->GetInterfaceCamera();
	if (!hCamera) return;


    g_pLTClient->GetObjectPos(hCamera, &s_vPos);
    g_pLTClient->GetObjectRotation(hCamera, &s_rRot);
	s_vU = s_rRot.Up();
	s_vR = s_rRot.Right();
	s_vF = s_rRot.Forward();


	sprintf(szAttName,"Light%d",n);
	while (g_pLayoutMgr->HasCustomValue((eScreenID)m_nScreenID,szAttName))
	{
		g_pLayoutMgr->GetScreenCustomString((eScreenID)m_nScreenID,szAttName,szFXName,128);
		if (strlen(szFXName))
		{
			CreateLightFX(szFXName);
		}

		n++;
		sprintf(szAttName,"Light%d",n);

	}

	//[MURSUM]
	/*
	n = 0;
	g_ScaleFXMap.clear();
	sprintf(szAttName,"ScaleName%d",n);
	while (g_pLayoutMgr->HasCustomValue((eScreenID)m_nScreenID,szAttName))
	{
		g_pLayoutMgr->GetScreenCustomString((eScreenID)m_nScreenID,szAttName,szFXName,128);
		if (strlen(szFXName))
		{
			CBaseScaleFX *pSFX = CreateScaleFX(szFXName);
		}

		n++;
		sprintf(szAttName,"ScaleName%d",n);

	}
	*/	

	// 광식 [05.03.18] : Model 을 User 가 선택하도록 수정
	//-------------------------------------------------------------------------------------------------------
	char szCharacter[128] = {0,};
	for( int nChar = 0; nChar < 2; nChar++ )
	{
		sprintf(szCharacter, "Character%d", nChar);
		INT_CHAR *pChar = g_pLayoutMgr->GetScreenCustomCharacter((eScreenID)m_nScreenID, szCharacter);
		if (pChar)
		{
			CreateCharFX(pChar, nChar);
			if( m_CharacterSFX[nChar].GetObject())
			{
				int reqID[MAX_INT_ATTACHMENTS];
				int numReq = g_pAttachButeMgr->GetRequirementIDs(pChar->szModel,reqID,MAX_INT_ATTACHMENTS);
				int i;
				for (i = 0; i < numReq; i++)
				{
					INT_ATTACH acs;
					acs.fScale = pChar->fScale;
					acs.nAttachmentID = g_pAttachButeMgr->GetRequirementAttachment(reqID[i]);
					g_pAttachButeMgr->GetRequirementSocket(reqID[i],acs.szSocket,sizeof(acs.szSocket));

					CreateAttachFX(&acs, nChar);
				}

				int numAtt = g_pLayoutMgr->GetScreenNumAttachments((eScreenID)m_nScreenID);
				for (i = 0; i < numAtt; i++)
				{
					char szTemp[128];
					char *pName = LTNULL;
					char *pSocket = LTNULL;
					g_pLayoutMgr->GetScreenAttachment( (eScreenID)m_nScreenID, i, szTemp, 128);

					pName = strtok(szTemp,";");
					pSocket = strtok(NULL,";");

					INT_ATTACH acs;

					acs.fScale = pChar->fScale;
					acs.nAttachmentID = g_pAttachButeMgr->GetAttachmentIDByName(pName);
					SAFE_STRCPY(acs.szSocket,pSocket);

					CreateAttachFX(&acs, nChar);
				}

			}
		}
	}
	//-------------------------------------------------------------------------------------------------------

	//[MURSUM]
	/*
	INT_CHAR *pChar = g_pLayoutMgr->GetScreenCharacter((eScreenID)m_nScreenID);
	if (pChar)
	{
		CreateCharFX(pChar);
		if (m_CharSFX.GetObject())
		{
			int reqID[MAX_INT_ATTACHMENTS];
			int numReq = g_pAttachButeMgr->GetRequirementIDs(pChar->szModel,reqID,MAX_INT_ATTACHMENTS);
			int i;
			for (i = 0; i < numReq; i++)
			{
				INT_ATTACH acs;
				acs.fScale = pChar->fScale;
				acs.nAttachmentID = g_pAttachButeMgr->GetRequirementAttachment(reqID[i]);
				g_pAttachButeMgr->GetRequirementSocket(reqID[i],acs.szSocket,sizeof(acs.szSocket));

				CreateAttachFX(&acs);
			}

			int numAtt = g_pLayoutMgr->GetScreenNumAttachments((eScreenID)m_nScreenID);
			for (i = 0; i < numAtt; i++)
			{
				char szTemp[128];
				char *pName = LTNULL;
				char *pSocket = LTNULL;
				g_pLayoutMgr->GetScreenAttachment( (eScreenID)m_nScreenID, i, szTemp, 128);

				pName = strtok(szTemp,";");
				pSocket = strtok(NULL,";");

				INT_ATTACH acs;

				acs.fScale = pChar->fScale;
				acs.nAttachmentID = g_pAttachButeMgr->GetAttachmentIDByName(pName);
				SAFE_STRCPY(acs.szSocket,pSocket);

				CreateAttachFX(&acs);
			}

		}
	}
	
	szFXName[0] = 0;
	g_pLayoutMgr->GetScreenSelectFX((eScreenID)m_nScreenID,szFXName,sizeof(szFXName));
	m_bSelectFXCenter = g_pLayoutMgr->GetScreenSelectFXCenter((eScreenID)m_nScreenID);
	g_pInterfaceMgr->SetSelectFX(szFXName);

	n = 0;
	sprintf(szAttName,"FX%d",n);
	while (g_pLayoutMgr->HasCustomValue((eScreenID)m_nScreenID,szAttName))
	{
		g_pLayoutMgr->GetScreenCustomString((eScreenID)m_nScreenID,szAttName,szFXName,128);
		if (strlen(szFXName))
		{
			INT_FX* pFX = g_pLayoutMgr->GetFX(szFXName);
			if (pFX)
			{
				g_pInterfaceMgr->AddInterfaceFX(LTNULL, pFX->szFXName,pFX->vPos,pFX->bLoop);
			}
		}

		
		n++;
		sprintf(szAttName,"FX%d",n);

	}

	
	if (m_pScreenMgr->GetTransitionFXMgr()->HasTransitionFX())
	{
		m_pScreenMgr->GetTransitionFXMgr()->StartTransitionFX(!!m_bVisited);
	}
	else
	{
		// ABM 2/7/02 TODO rip these lines out in favor of letting the TransitionFXMgr handle
		// all the FX.  So take these effects and move their creation into TransitionFXMgr.
		// As a first step, take the Start and Update out, and let BaseScreen still create
		// them but then pass them to the TransitionFXMgr.  As a second step, move their creation
		// into TransitionFXMgr as well.

		ChainFXList::iterator iter = m_Chains.begin();

		while (iter != m_Chains.end())
		{
			(*iter)->Start(!!m_bVisited);
			iter++;
		}
	}
	*/
}

void CBaseScreen::RemoveInterfaceSFX()
{
	g_ScaleFXMap.clear();
	for (uint16 i=0; i < m_SFXArray.size(); i++)
	{
		CSpecialFX *pSFX = m_SFXArray[i];
		g_pInterfaceMgr->RemoveInterfaceSFX(pSFX);
		debug_delete(pSFX);
	}
	m_SFXArray.clear();

	//g_pInterfaceMgr->RemoveInterfaceSFX(&m_CharSFX);	
	
	g_pInterfaceMgr->RemoveInterfaceLights();
	m_bHaveLights = LTFALSE;

	//m_CharSFX.Reset();
	//m_CharSFX.Term();

	// 광식
	//--------------------------------------------------
	for( i = 0; i < 2; i++ )
	{
		g_pInterfaceMgr->RemoveInterfaceSFX(&m_CharacterSFX[i]);
		m_CharacterSFX[i].Reset();
		m_CharacterSFX[i].Term();
	}	
	//--------------------------------------------------

	ClearAttachFX();
	

	/*
	ChainFXList::iterator iter = m_Chains.begin();
	while (iter != m_Chains.end())
	{
		(*iter)->End();
		iter++;
	}
	*/


	g_pInterfaceMgr->RemoveInterfaceFX();


}

void CBaseScreen::ClearAttachFX()
{
	for (int i = 0; i < MAX_INT_ATTACHMENTS; i++)
	{
		g_pInterfaceMgr->RemoveInterfaceSFX(&m_aAttachment[i].sfx);
		m_aAttachment[i].sfx.Reset();
		m_aAttachment[i].sfx.Term();
		m_aAttachment[i].socket = INVALID_MODEL_SOCKET;
	}
	m_nNumAttachments = 0;

}

bool CBaseScreen::UpdateInterfaceSFX()
{
	if( g_ModelAniCount <= 5 )
	{
		for( int i = 0; i < 2; i++ )
		{
			uint32 dwAni = g_pLTClient->GetAnimIndex(m_CharacterSFX[i].GetObject(), "IF_SELECT");
			if( dwAni != INVALID_ANI )
			{
				g_pLTClient->SetModelAnimation(m_CharacterSFX[i].GetObject(), dwAni);
			}
		}
	}
	else if( g_ModelAniCount > 10 )
	{
		g_ModelAniCount = 10;
	}
	g_ModelAniCount++;
	/*
	for (int i = 0; i < m_nNumAttachments; i++)
	{
		CBaseScaleFX *pSFX = &m_aAttachment[i].sfx;
		
		HMODELSOCKET hSocket = m_aAttachment[i].socket;
		LTransform transform;
		if (g_pModelLT->GetSocketTransform(m_CharSFX.GetObject(), hSocket, transform, LTTRUE) == LT_OK)
		{
            g_pLTClient->SetObjectPosAndRotation(pSFX->GetObject(), &transform.m_Pos, &transform.m_Rot);

		}
	}
	*/

	// 광식
	//-------------------------------------------------------------------------------------------------------------
	for (int i = 0; i < m_nNumAttachments; i++)
	{
		CBaseScaleFX *pSFX = &m_aAttachment[i].sfx;
		
		HMODELSOCKET hSocket = m_aAttachment[i].socket;
		LTransform transform;
		if (g_pModelLT->GetSocketTransform(m_CharacterSFX[0].GetObject(), hSocket, transform, LTTRUE) == LT_OK)
		{
            g_pLTClient->SetObjectPosAndRotation(pSFX->GetObject(), &transform.m_Pos, &transform.m_Rot);

		}
	}
	
	for (i = 0; i < m_nNumAttachments; i++)
	{
		CBaseScaleFX *pSFX = &m_aAttachment[i].sfx;
		
		HMODELSOCKET hSocket = m_aAttachment[i].socket;
		LTransform transform;
		if (g_pModelLT->GetSocketTransform(m_CharacterSFX[1].GetObject(), hSocket, transform, LTTRUE) == LT_OK)
		{
            g_pLTClient->SetObjectPosAndRotation(pSFX->GetObject(), &transform.m_Pos, &transform.m_Rot);

		}
	}
	//-------------------------------------------------------------------------------------------------------------

	//[MURSUM]
	/*
	if (m_pScreenMgr->GetTransitionFXMgr()->HasTransitionFX())
	{
		m_pScreenMgr->GetTransitionFXMgr()->UpdateTransitionFX();
	}
	else
	{
		// ABM TODO 2/7/02 remove this in favor of letting the TransitionFXMgr handle all details.
		ChainFXList::iterator iter = m_Chains.begin();
		while (iter != m_Chains.end())
		{
			(*iter)->Update();
			iter++;
		}
	}
	*/

	return true;
}

void CBaseScreen::Update_Model()
{
	g_ModelAniCount = 0;
}

uint16 CBaseScreen::GetIndex(CLTGUICtrl* pCtrl)
{
	ControlArray::iterator iter = m_controlArray.begin();

	uint16 dwIndex = 0;
	while (iter != m_controlArray.end() && (*iter) != pCtrl)
	{
		++dwIndex;
		iter++;
	}
	if (dwIndex < m_controlArray.size())
		return dwIndex;
	return kNoSelection;
}


CBaseScaleFX* CBaseScreen::CreateScaleFX(char *szFXName)
{
	CScaleFX* pScaleFX = g_pFXButeMgr->GetScaleFX(szFXName);
	if (pScaleFX)
	{
		pScaleFX->bUseLight = m_bHaveLights;
		CBaseScaleFX *pSFX = debug_new(CBaseScaleFX);
		g_pFXButeMgr->CreateScaleFX(pScaleFX,s_vPos, s_vF, LTNULL, &s_rRot, pSFX);
		m_SFXArray.push_back(pSFX);
		g_pInterfaceMgr->AddInterfaceSFX(pSFX, IFX_NORMAL);				

		char szTmp[64];
		SAFE_STRCPY(szTmp,szFXName);
		_strupr(szTmp);
		g_ScaleFXMap[szTmp] = pSFX;

		return pSFX;
	}
	return NULL;
}
void CBaseScreen::CreateLightFX(char *szFXName)
{
	INT_LIGHT* pLight = g_pLayoutMgr->GetLight(szFXName);
	if (pLight)
	{
//		pScaleFX->bUseLight = LTTRUE;
//		CBaseScaleFX *pSFX = debug_new(CBaseScaleFX);
//		g_pFXButeMgr->CreateScaleFX(pScaleFX,s_vPos, s_vF, LTNULL, &s_rRot, pSFX);
//		m_SFXArray.push_back(pSFX);
//		g_pInterfaceMgr->AddInterfaceSFX(pSFX, IFX_NORMAL);


		ObjectCreateStruct createStruct;
		INIT_OBJECTCREATESTRUCT(createStruct);

		createStruct.m_ObjectType	= OT_LIGHT;
		createStruct.m_Flags		= FLAG_VISIBLE | FLAG_ONLYLIGHTOBJECTS;
		
        createStruct.m_Pos = s_vPos;

		createStruct.m_Pos += pLight->vPos;

        HOBJECT hLight = g_pLTClient->CreateObject(&createStruct);

		if (hLight)
		{
			g_pLTClient->SetLightColor(hLight, pLight->vColor.x, pLight->vColor.y, pLight->vColor.z);
			g_pLTClient->SetLightRadius(hLight, pLight->fRadius);

			g_pInterfaceMgr->AddInterfaceLight(hLight);
			m_bHaveLights = LTTRUE;
		}

	}
}
/*
void CBaseScreen::CreateCharFX(INT_CHAR *pChar)
{
	if (pChar)
	{

		BSCREATESTRUCT bcs;
	    LTVector vPos, vTemp, vScale(1.0f,1.0f,1.0f);
	    LTRotation rRot = s_rRot;

		char modName[128];

		SAFE_STRCPY(modName, pChar->szModel);

		VEC_COPY(vPos,s_vPos);
		VEC_SET(vScale,1.0f,1.0f,1.0f);
		VEC_MULSCALAR(vScale, vScale, pChar->fScale);

		LTVector vModPos = pChar->vPos;
	    LTFLOAT fRot = pChar->fRot;
		fRot  = MATH_PI + DEG2RAD(fRot);
		rRot.Rotate(s_vU, fRot);

		VEC_MULSCALAR(vTemp, s_vF, vModPos.z);
//		VEC_MULSCALAR(vTemp, vTemp, g_pInterfaceResMgr->GetXRatio());
		VEC_ADD(vPos, vPos, vTemp);

		VEC_MULSCALAR(vTemp, s_vR, vModPos.x);
		VEC_ADD(vPos, vPos, vTemp);

		VEC_MULSCALAR(vTemp, s_vU, vModPos.y);
		VEC_ADD(vPos, vPos, vTemp);

		VEC_COPY(bcs.vPos, vPos);
		bcs.rRot = rRot;
		VEC_COPY(bcs.vInitialScale, vScale);
		VEC_COPY(bcs.vFinalScale, vScale);
		VEC_SET(bcs.vInitialColor, 1.0f, 1.0f, 1.0f);
		VEC_SET(bcs.vFinalColor, 1.0f, 1.0f, 1.0f);
		bcs.bUseUserColors = LTTRUE;

		bcs.pFilename = modName;
		bcs.pSkinReader = &(pChar->blrSkins);
		bcs.pRenderStyleReader = &(pChar->blrRenderStyles);
		bcs.dwFlags = FLAG_VISIBLE | FLAG_FOGDISABLE;// | FLAG_NOLIGHT;

		bcs.nType = OT_MODEL;
		bcs.fInitialAlpha = 0.99f;
		bcs.fFinalAlpha = 0.99f;
		bcs.fLifeTime = 1000000.0f;
		bcs.bLoop = LTTRUE;

		bcs.fMinRotateVel = 1.0f;
		bcs.fMaxRotateVel = 1.0f;

		bcs.nMenuLayer = pChar->nMenuLayer;

		if (m_CharSFX.Init(&bcs))
		{
			m_CharSFX.CreateObject(g_pLTClient);
			if (m_CharSFX.GetObject())
			{
//				g_pLTClient->SetModelAnimation(m_CharSFX.GetObject(), 1);
				g_pInterfaceMgr->AddInterfaceSFX(&m_CharSFX, IFX_NORMAL);
			}
		}

	}
}
*/
// 광식
void CBaseScreen::CreateCharFX( INT_CHAR *pChar, int nTeam )
{
	if (pChar)
	{

		BSCREATESTRUCT bcs;
	    LTVector vPos, vTemp, vScale(1.0f,1.0f,1.0f);
	    LTRotation rRot = s_rRot;

		char modName[128];

		SAFE_STRCPY(modName, pChar->szModel);

		VEC_COPY(vPos,s_vPos);
		VEC_SET(vScale,1.0f,1.0f,1.0f);
		VEC_MULSCALAR(vScale, vScale, pChar->fScale);

		LTVector vModPos = pChar->vPos;
	    LTFLOAT fRot = pChar->fRot;
		fRot  = MATH_PI + DEG2RAD(fRot);
		rRot.Rotate(s_vU, fRot);

		VEC_MULSCALAR(vTemp, s_vF, vModPos.z);
		VEC_ADD(vPos, vPos, vTemp);

		VEC_MULSCALAR(vTemp, s_vR, vModPos.x);
		VEC_ADD(vPos, vPos, vTemp);

		VEC_MULSCALAR(vTemp, s_vU, vModPos.y);
		VEC_ADD(vPos, vPos, vTemp);

		VEC_COPY(bcs.vPos, vPos);
		bcs.rRot = rRot;
		VEC_COPY(bcs.vInitialScale, vScale);
		VEC_COPY(bcs.vFinalScale, vScale);
		VEC_SET(bcs.vInitialColor, 1.0f, 1.0f, 1.0f);
		VEC_SET(bcs.vFinalColor, 1.0f, 1.0f, 1.0f);
		bcs.bUseUserColors = LTTRUE;

		bcs.pFilename = modName;
		bcs.pSkinReader = &(pChar->blrSkins);
		bcs.pRenderStyleReader = &(pChar->blrRenderStyles);
		bcs.dwFlags = FLAG_VISIBLE | FLAG_FOGDISABLE;// | FLAG_NOLIGHT;

		bcs.nType = OT_MODEL;
		bcs.fInitialAlpha = 0.99f;
		bcs.fFinalAlpha = 0.99f;
		bcs.fLifeTime = 1000000.0f;
		bcs.bLoop = LTTRUE;

		bcs.fMinRotateVel = 1.0f;
		bcs.fMaxRotateVel = 1.0f;

		bcs.nMenuLayer = pChar->nMenuLayer;

		if( m_CharacterSFX[nTeam].Init(&bcs))
		{
			m_CharacterSFX[nTeam].CreateObject(g_pLTClient);
			if( m_CharacterSFX[nTeam].GetObject() )
			{
				g_pInterfaceMgr->AddInterfaceSFX(&m_CharacterSFX[nTeam], IFX_NORMAL);
			}
		}
	}
}

void CBaseScreen::CreateAttachFX(INT_ATTACH *pAttach, int nTeam)
{
	if (m_nNumAttachments < MAX_INT_ATTACHMENTS)
	{

		BSCREATESTRUCT bcs;
	    LTVector vPos, vTemp, vScale(1.0f,1.0f,1.0f);
	    LTRotation rRot = s_rRot;


		char szModel[128];

		g_pAttachButeMgr->GetAttachmentModel(pAttach->nAttachmentID,szModel,sizeof(szModel));

		VEC_SET(vScale,1.0f,1.0f,1.0f);
		VEC_MULSCALAR(vScale, vScale, pAttach->fScale);

		VEC_COPY(bcs.vInitialScale, vScale);
		VEC_COPY(bcs.vFinalScale, vScale);
		VEC_SET(bcs.vInitialColor, 1.0f, 1.0f, 1.0f);
		VEC_SET(bcs.vFinalColor, 1.0f, 1.0f, 1.0f);
		bcs.bUseUserColors = LTTRUE;

		bcs.pFilename = szModel;
		bcs.dwFlags = FLAG_VISIBLE | FLAG_FOGDISABLE;// | FLAG_NOLIGHT;

		CButeListReader blrSkinReader;
		g_pAttachButeMgr->GetAttachmentSkins(pAttach->nAttachmentID, &blrSkinReader);
		bcs.pSkinReader = &blrSkinReader;

		CButeListReader blrRenderStyleReader;
		g_pAttachButeMgr->GetAttachmentRenderStyles(pAttach->nAttachmentID, &blrRenderStyleReader);
		bcs.pRenderStyleReader = &blrRenderStyleReader;

		bcs.fInitialAlpha = 1.0f;
		bcs.fFinalAlpha = 1.0f;
		bcs.nType = OT_MODEL;
		bcs.fLifeTime = 1000000.0f;
		bcs.bLoop = LTTRUE;
		bcs.nMenuLayer = m_CharacterSFX[nTeam].GetMenuLayer();

		CBaseScaleFX *pSFX = &m_aAttachment[m_nNumAttachments].sfx;

		if (!pSFX->Init(&bcs)) return;
		
		pSFX->CreateObject(g_pLTClient);
		if (!pSFX->GetObject()) return;

		HOBJECT hChar = m_CharacterSFX[nTeam].GetObject();
		if (!hChar) return;
		if (g_pModelLT->GetSocket(hChar, pAttach->szSocket, m_aAttachment[m_nNumAttachments].socket) != LT_OK)
			return;

		g_pInterfaceMgr->AddInterfaceSFX(pSFX, IFX_ATTACH);
		m_nNumAttachments++;
	}
}
/*
void CBaseScreen::CreateAttachFX(INT_ATTACH *pAttach)
{
	if (m_nNumAttachments < MAX_INT_ATTACHMENTS)
	{

		BSCREATESTRUCT bcs;
	    LTVector vPos, vTemp, vScale(1.0f,1.0f,1.0f);
	    LTRotation rRot = s_rRot;

		char szModel[128];
//		char szSkin[128];

		g_pAttachButeMgr->GetAttachmentModel(pAttach->nAttachmentID,szModel,sizeof(szModel));

//		str = g_pAttachButeMgr->GetAttachmentSkin(pAttach->nAttachmentID);
//		strncpy(szSkin, (char*)(LPCSTR)str, 128);

		VEC_SET(vScale,1.0f,1.0f,1.0f);
		VEC_MULSCALAR(vScale, vScale, pAttach->fScale);

		VEC_COPY(bcs.vInitialScale, vScale);
		VEC_COPY(bcs.vFinalScale, vScale);
		VEC_SET(bcs.vInitialColor, 1.0f, 1.0f, 1.0f);
		VEC_SET(bcs.vFinalColor, 1.0f, 1.0f, 1.0f);
		bcs.bUseUserColors = LTTRUE;

		bcs.pFilename = szModel;
//		bcs.pSkin[0] = szSkin;
		bcs.dwFlags = FLAG_VISIBLE | FLAG_FOGDISABLE;// | FLAG_NOLIGHT;

//		g_pAttachButeMgr->GetAttachmentProperties(pAttach->nAttachmentID);

		CButeListReader blrSkinReader;
		g_pAttachButeMgr->GetAttachmentSkins(pAttach->nAttachmentID, &blrSkinReader);
		bcs.pSkinReader = &blrSkinReader;

		CButeListReader blrRenderStyleReader;
		g_pAttachButeMgr->GetAttachmentRenderStyles(pAttach->nAttachmentID, &blrRenderStyleReader);
		bcs.pRenderStyleReader = &blrRenderStyleReader;

		bcs.fInitialAlpha = 1.0f;
		bcs.fFinalAlpha = 1.0f;
		bcs.nType = OT_MODEL;
		bcs.fLifeTime = 1000000.0f;
		bcs.bLoop = LTTRUE;
		bcs.nMenuLayer = m_CharSFX.GetMenuLayer();

		CBaseScaleFX *pSFX = &m_aAttachment[m_nNumAttachments].sfx;

		if (!pSFX->Init(&bcs)) return;
		
		pSFX->CreateObject(g_pLTClient);
		if (!pSFX->GetObject()) return;

		HOBJECT hChar = m_CharSFX.GetObject();
		if (!hChar) return;
		if (g_pModelLT->GetSocket(hChar, pAttach->szSocket, m_aAttachment[m_nNumAttachments].socket) != LT_OK)
			return;

		g_pInterfaceMgr->AddInterfaceSFX(pSFX, IFX_ATTACH);
		m_nNumAttachments++;
	}
}
*/

void CBaseScreen::ScreenDimsChanged()
{
	m_fLastScale = g_pInterfaceResMgr->GetXRatio();
	unsigned int i;
	for ( i = 0; i < m_controlArray.size(); i++ )
	{
		m_controlArray[i]->SetScale(g_pInterfaceResMgr->GetXRatio());
	}
}

void CBaseScreen::SetPolyRenderState()
{
	g_pDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
	g_pDrawPrim->SetZBufferMode(DRAWPRIM_NOZ); 
	g_pDrawPrim->SetClipMode(DRAWPRIM_NOCLIP);
	g_pDrawPrim->SetFillMode(DRAWPRIM_FILL);
	g_pDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
	g_pDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
		
}

void CBaseScreen::InitPoly(LTPoly_GT4* pPoly, LTIntPt pos, HTEXTURE hTex)
{
	_ASSERT(pPoly && hTex);
	if (!pPoly || !hTex)
		return;
	ScalePoly(pPoly,pos,hTex);
	SetupQuadUVs(*pPoly, hTex, 0.0f,0.0f,1.0f,1.0f);
	g_pDrawPrim->SetRGBA(pPoly,argbWhite);

}

void CBaseScreen::ScalePoly(LTPoly_GT4* pPoly, LTIntPt pos, HTEXTURE hTex)
{
	_ASSERT(pPoly && hTex);
	if (!pPoly || !hTex)
		return;
	uint32 w,h;
	g_pTexInterface->GetTextureDims(hTex,w,h);
	float x = (float)pos.x * g_pInterfaceResMgr->GetXRatio();
	float y = (float)pos.y * g_pInterfaceResMgr->GetXRatio();
	float fw = (float)w * g_pInterfaceResMgr->GetXRatio();
	float fh = (float)h * g_pInterfaceResMgr->GetXRatio();

	g_pDrawPrim->SetXYWH(pPoly,x,y,fw,fh);

}



void HandleEditKey(int key)
{
	switch (key)
	{
	case VK_LEFT:
		{
			vSFXPos.x -= fEditDelta;
		} break;
	case VK_RIGHT:
		{
			vSFXPos.x += fEditDelta;
		} break;
	case VK_UP:
		{
			vSFXPos.y += fEditDelta;
		} break;
	case VK_DOWN:
		{
			vSFXPos.y -= fEditDelta;
		} break;
	case VK_A:
		{
			vSFXScale.x -= fEditDelta / 10.0f;
		} break;
	case VK_D:
		{
			vSFXScale.x += fEditDelta / 10.0f;
		} break;
	case VK_W:
		{
			vSFXScale.y -= fEditDelta / 10.0f;
		} break;
	case VK_S:
		{
			vSFXScale.y += fEditDelta / 10.0f;
		} break;
	case VK_PRIOR:
		{
			fEditDelta *= 10.0f;
			g_pLTClient->CPrint("Edit scale = %0.3f",fEditDelta);
		}break;
	case VK_NEXT:
		{
			fEditDelta /= 10.0f;
			g_pLTClient->CPrint("Edit scale = %0.3f",fEditDelta);
		}break;
	case VK_RETURN:
		{
			bEditSFXMode = false;
			g_pLTClient->CPrint("Exiting SFX edit mode.");
			g_pLTClient->CPrint("	SFX Pos = <%0.3f, %0.3f, %0.3f>", vSFXPos.x,vSFXPos.y,vSFXPos.z);
			g_pLTClient->CPrint("	SFX Scale = <%0.3f, %0.3f, %0.3f>", vSFXScale.x,vSFXScale.y,vSFXScale.z);
		} break;
	}

	HOBJECT hObj = pEditSFX->GetObject();
	g_pLTClient->SetObjectPos(hObj,&vSFXPos);
	g_pLTClient->SetObjectScale(hObj,&vSFXScale);

}

// NOVICE
void CBaseScreen::AddTextToList( CSAD3DText* pText )
{
	m_TextList.push_back( pText );
}

BOOL CBaseScreen::ClearTextList()
{
	if ( FALSE == m_TextList.empty() )
	{
		for ( uint32 n = 0; n < m_TextList.size(); n++ )
		{
			SAFE_DELETE( m_TextList[n] );
		}

		m_TextList.clear();

		return TRUE;
	}

	return FALSE;
}

void CBaseScreen::SetRenderState()
{
	g_pDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
	g_pDrawPrim->SetZBufferMode(DRAWPRIM_NOZ); 
	g_pDrawPrim->SetClipMode(DRAWPRIM_NOCLIP);
	g_pDrawPrim->SetFillMode(DRAWPRIM_FILL);
	g_pDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
	g_pDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	g_pDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
}