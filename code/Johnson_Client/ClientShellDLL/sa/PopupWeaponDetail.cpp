#include "stdafx.h"
#include "..\\clientshellshared\\GameText.h"
#include "..\\clientshellshared\\ClientSoundMgr.h"
#include "ScreenCommands.h"
#include "PopupWeaponDetail.h"

CPopUpWeaponDetail* g_pPopupWeaponDetail = LTNULL;

LTVector	CPopUpWeaponDetail::s_vPos; 
LTVector	CPopUpWeaponDetail::s_vU; 
LTVector	CPopUpWeaponDetail::s_vR; 
LTVector	CPopUpWeaponDetail::s_vF;
LTRotation	CPopUpWeaponDetail::s_rRot;

namespace
{
	enum eLocalCommands
	{
		CMD_TIP_EXIT	= CMD_CUSTOM+100,
	};
}

CPopUpWeaponDetail::CPopUpWeaponDetail()
{
	g_pPopupWeaponDetail =  this;

	//////////////////////////////////////////////////////////////////////////
	// Guage Animation Variables
	m_fDamage	= LTNULL;	m_fNowDamage	= LTNULL;
	m_fShoot	= LTNULL;	m_fNowShoot		= LTNULL;
	m_fHit		= LTNULL;	m_fNowHit		= LTNULL;
	m_fReact	= LTNULL;	m_fNowReact		= LTNULL;
	m_fClip		= LTNULL;	m_fNowClip		= LTNULL;
	m_fSltAmmo	= LTNULL;	m_fNowSltAmmo	= LTNULL;

	m_pDamage	= LTNULL;
	m_pShoot	= LTNULL;
	m_pHit		= LTNULL;
	m_pReact	= LTNULL;
	//////////////////////////////////////////////////////////////////////////

	m_pBtnExit			= LTNULL;

	m_pFrmDamage		= LTNULL;
	m_pFrmShootingVel	= LTNULL;
	m_pFrmHitRate		= LTNULL;
	m_pFrmReactRate		= LTNULL;

	m_pFrmWeapon		= LTNULL;
	m_pFrmWeaponTi		= LTNULL;
	
	m_pWeaponDesc		= LTNULL;	// NOVICE
	m_pShotsPerClip		= LTNULL;

	m_hGaugeSound		= LTNULL;
}

CPopUpWeaponDetail::~CPopUpWeaponDetail()
{
	Term();
}

void CPopUpWeaponDetail::Init()
{
	// Create PopUp Window
	//------------------------------------------------------------------------------------------------
	char szStr[128] = {0, };
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName( SCREEN_ID_INVENTORY );
	
	g_pLayoutMgr->GetString(pScreenTag, "TBGTexture", szStr, 128 );
	HTEXTURE hFrameTex	  = g_pInterfaceResMgr->GetTexture(szStr);
	LTRect rcFrameTexRect = g_pLayoutMgr->GetRect(pScreenTag, "TBGTextureRect");
	LTRect rcFrame		  = g_pLayoutMgr->GetRect( pScreenTag, "TBGRect");
	float xRate = (float)rcFrame.right / (float)rcFrameTexRect.right;
	float yRate = (float)rcFrame.bottom / (float)rcFrameTexRect.bottom;
	Create(hFrameTex, rcFrame.right, rcFrame.bottom, xRate, yRate, LTTRUE);	
	LTIntPt basePos		  = g_pLayoutMgr->GetPoint(pScreenTag, "TBGBasePos");
	SetBasePos(basePos);
	Show(LTFALSE);	
	//------------------------------------------------------------------------------------------------
	m_pBtnExit	= new CLTGUIButton;
	AddButton(pScreenTag, m_pBtnExit, CMD_TIP_EXIT, 
			  "TBtnExitN", "TBtnExitH", "TBtnExitD", "TBtnExitC",
			  "TBtnExitRect");
	m_pBtnExit->SetClickSound( (int)IS_CLICK_NORMAL );

	m_pFrmDamage		= new CLTGUIFrame();
	AddFrame(pScreenTag, m_pFrmDamage, "FrmDamagePt", "FrmGaugeTx");
	m_pFrmShootingVel	= new CLTGUIFrame();
	AddFrame(pScreenTag, m_pFrmShootingVel, "FrmShootPt", "FrmGaugeTx");
	m_pFrmHitRate		= new CLTGUIFrame();
	AddFrame(pScreenTag, m_pFrmHitRate, "FrmHitPt", "FrmGaugeTx");
	m_pFrmReactRate		= new CLTGUIFrame();
	AddFrame(pScreenTag, m_pFrmReactRate, "FrmReactPt", "FrmGaugeTx");

	m_pFrmWeapon	= AddFrameRect(pScreenTag, "FrmWeaponRect", "FrmWeaponTx", LTTRUE);
	m_pFrmWeaponTi	= AddFrameRect(pScreenTag, "FrmWeaponTiRect", "FrmWeaponTiTx", LTTRUE);

	m_pWeaponDesc = new CSAD3DText();
	m_pWeaponDesc->CreateD3DText( 290, 90, false );
	m_pWeaponDesc->SetTransparent( true );
	m_pWeaponDesc->SetTextColor( RGB(255,255,255) );
	m_pWeaponDesc->SetPosition( basePos.x + 13, basePos.y + 220 );
	m_pWeaponDesc->DrawD3DText( "", 0 );

	m_pShotsPerClip = new CSAD3DText();
	m_pShotsPerClip->CreateD3DText( 64, 16, false );
	m_pShotsPerClip->SetTransparent( true );
	m_pShotsPerClip->SetTextColor( RGB(255,255,255) );
	m_pShotsPerClip->SetPosition( basePos.x + 90, basePos.y + 200 );	
	m_pShotsPerClip->DrawD3DText( "", 0 );

	
	m_pDamage = new CSAD3DText();
	m_pDamage->CreateD3DText( 30, 10, false );
	m_pDamage->SetTransparent( true );
	m_pDamage->SetTextColor( RGB(255,255,255) );
	m_pDamage->SetPosition( basePos.x + 182, basePos.y + 143 );	
	m_pDamage->DrawD3DText( "", 0 );

	m_pShoot = new CSAD3DText();
	m_pShoot->CreateD3DText( 30, 10, false );
	m_pShoot->SetTransparent( true );
	m_pShoot->SetTextColor( RGB(255,255,255) );
	m_pShoot->SetPosition( basePos.x + 182, basePos.y + 157 );	
	m_pShoot->DrawD3DText( "", 0 );
	
	m_pHit = new CSAD3DText();
	m_pHit->CreateD3DText( 30, 10, false );
	m_pHit->SetTransparent( true );
	m_pHit->SetTextColor( RGB(255,255,255) );
	m_pHit->SetPosition( basePos.x + 182, basePos.y + 171 );	
	m_pHit->DrawD3DText( "", 0 );
	
	m_pReact = new CSAD3DText();
	m_pReact->CreateD3DText( 30, 10, false );
	m_pReact->SetTransparent( true );
	m_pReact->SetTextColor( RGB(255,255,255) );
	m_pReact->SetPosition( basePos.x + 182, basePos.y + 185 );	
	m_pReact->DrawD3DText( "", 0 );
	
}

void CPopUpWeaponDetail::Term()
{
	SAFE_DELETE( m_pWeaponDesc );
	SAFE_DELETE( m_pShotsPerClip );

	SAFE_DELETE( m_pDamage	);
	SAFE_DELETE( m_pShoot	);
	SAFE_DELETE( m_pHit		);
	SAFE_DELETE( m_pReact	);
}


void CPopUpWeaponDetail::SetGauge(int nDamage, int nShoot, int nHit, int nReact, int nClip, int nSltAmmo)
{
	m_fDamage		= (float)nDamage;
	m_fShoot		= (float)nShoot;
	m_fHit			= (float)nHit;
	m_fReact		= (float)nReact;
	m_fClip			= (float)nClip;
	m_fSltAmmo		= (float)nSltAmmo;

	m_fNowDamage	= 0;
	m_fNowShoot		= 0;
	m_fNowHit		= 0;
	m_fNowReact		= 0;

	char szTemp[64] = {0, };
	ZeroMemory( szTemp, sizeof(szTemp) );
	sprintf(szTemp, "%.0f / %.0f", m_fClip, m_fSltAmmo);
	m_pShotsPerClip->DrawD3DText(szTemp, sizeof(szTemp));
}

void CPopUpWeaponDetail::UpdateGuageAni()
{
	if( m_bFirstUpdate )
	{
		m_bFirstUpdate		= LTFALSE;
		m_nFinishAniFlags	= 0;
		if( LTNULL == m_hGaugeSound )
		{
			m_hGaugeSound = g_pClientSoundMgr->PlayInterfaceSound( "SA_INTERFACE\\SND\\GaugeLoop.wav", PLAYSOUND_LOOP );
		}
	}

	float fGuageDelta = 100 / 1.5f * g_pLTClient->GetFrameTime();
	
	if ( m_fDamage > m_fNowDamage )
	{
		m_fNowDamage += fGuageDelta; 
	}
	else
	{
		m_fNowDamage = m_fDamage;
		m_nFinishAniFlags |= 0x0000000F;
	}

	if ( m_fShoot > m_fNowShoot )
	{
		m_fNowShoot	+= fGuageDelta;
	}
	else
	{
		m_fNowShoot = m_fShoot;
		m_nFinishAniFlags |= 0x000000F0;
	}

	if ( m_fHit > m_fNowHit )
	{
		m_fNowHit		+= fGuageDelta;
	}
	else
	{
		m_fNowHit = m_fHit;
		m_nFinishAniFlags |= 0x00000F00;
	}

	if ( m_fReact > m_fNowReact )
	{
		m_fNowReact += fGuageDelta;
	}
	else
	{
		m_fNowReact = m_fReact;
		m_nFinishAniFlags |= 0x0000F000;
	}

	if ( m_fClip > m_fNowClip )
	{
		m_fNowClip += fGuageDelta;
	}
	else
	{
		m_fNowClip = m_fClip;
		m_nFinishAniFlags |= 0x000F0000;
	}
	
	if( m_nFinishAniFlags >= 0x000FFFFF )
	{
		if( m_hGaugeSound )
		{
			g_pLTClient->SoundMgr()->KillSound( m_hGaugeSound );
			m_hGaugeSound	= LTNULL;
		}
		// NOVICE
		// return;
	}

	uint16 nHeight = 16;
	uint16 nWidth = (uint16)( 128 * (m_fNowDamage*0.01f) );
	m_pFrmDamage->SetSize(nWidth, nHeight);
	nWidth = (uint16)( 128 * (m_fNowShoot*0.01f) );
	m_pFrmShootingVel->SetSize(nWidth, nHeight);
	nWidth = (uint16)( 128 * (m_fNowHit*0.01f) );
	m_pFrmHitRate->SetSize(nWidth, nHeight);
	nWidth = (uint16)( 128 * (m_fNowReact*0.01f) );
	m_pFrmReactRate->SetSize(nWidth, nHeight);
	
	int nPercent = 0;

	this->DrawPercent( m_pDamage,	m_fDamage,	m_fNowDamage	);
	this->DrawPercent( m_pShoot,	m_fShoot,	m_fNowShoot		);
	this->DrawPercent( m_pHit,		m_fHit,		m_fNowHit		);
	this->DrawPercent( m_pReact,	m_fReact,	m_fNowReact		);
}

void CPopUpWeaponDetail::DrawPercent( CSAD3DText* pText, LTFLOAT fDenominator, LTFLOAT fNumerator )
{
	char szTemp[16] = {LTNULL};

	if ( fNumerator > 100 )
	{
		fNumerator = 100;
	}
	
	ZeroMemory( szTemp, sizeof(szTemp) );
	sprintf(szTemp, "%3.0f%%", fNumerator );
	pText->DrawD3DText(szTemp, sizeof(szTemp));
}


void CPopUpWeaponDetail::SetDescription( char* szDescription )
{
	m_pWeaponDesc->DrawD3DText( szDescription,
								lstrlen( szDescription ),
								DT_LEFT,
								D3D_TEXT_OUTLINE );
}

void CPopUpWeaponDetail::OnFocus(LTBOOL bFocus)
{
	if( bFocus )
	{
		//CreateWeaponDetailSFX(0);
		m_bFirstUpdate	= LTTRUE;
	}
	else
	{
		//ClearWeaponDetailSFX();
		if( m_hGaugeSound )
		{
			g_pLTClient->SoundMgr()->KillSound( m_hGaugeSound );
			m_hGaugeSound	= LTNULL;
		}
	}
}

void CPopUpWeaponDetail::CreateBackeFrame( eScreenID nScreen )
{
}

void CPopUpWeaponDetail::Render()
{
	if( !IsVisible() ) return;

	//UpdateDetailSFX();
	UpdateGuageAni();

	CLTGUIWindow::Render();

	m_pWeaponDesc->Render();

	m_pShotsPerClip->Render();

	m_pDamage->Render();
	m_pShoot->Render();
	m_pHit->Render();
	m_pReact->Render();
}

uint32 CPopUpWeaponDetail::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
		case CMD_TIP_EXIT :
			//this->Show(LTFALSE);
			this->CtrlHide();

			//g_pLTClient->Common()->SetObjectFlags(m_WeaponDetailSFX.GetObject(), OFT_Flags, 0, FLAG_VISIBLE );
			break;
	}

	return 1;
}