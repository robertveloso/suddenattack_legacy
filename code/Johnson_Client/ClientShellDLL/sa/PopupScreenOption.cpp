#include "stdafx.h"
#include "PopupScreenOption.h"
#include "dinput.h"
#include "ScreenCommands.h"
#include "ClientRes.h"
#include "GameClientShell.h"
#include "VarTrack.h"
#include "ScreenMain.h"
#include "SAHUDMgr.h"

//------------------------------------------- (B) Novice (minjin) [03/12/2005]
#include "GameText.h"
#include "GameMessage.h"
//------------------------------------------- (E) Novice (minjin) [03/12/2005]

extern CGameClientShell* g_pGameClientShell;

CPopScreenOption* g_pPopupOption = LTNULL;

#define OFFSET_BASEPOS 2
#define OFFSET_POS	   4
#define OPT_GAUGE_H	   16
#define OPT_INPUT_PAUSE_TIME_LEFT 0.3f

enum OPTION
{
	OPCMD_DISPLAY = 1,
	OPCMD_PERFORMANCE,
	OPCMD_CTRL,
	OPCMD_ETC,
	OPCMD_INIT,
	OPCMD_OK,
	OPCMD_CANCEL,

	OPCMD_RESOLUTION_CHECK0,
	OPCMD_RESOLUTION_CHECK1,
	OPCMD_RESOLUTION_CHECK2,
	OPCMD_RESOLUTION_CHECK3,
	OPCMD_RESOLUTION_CHECK4,
	OPCMD_RESOLUTION_CHECK5,
	OPCMD_RESOLUTION_16BIT,
	OPCMD_RESOLUTION_32BIT,

	
	OPCMD_MUSIC_ON,
	OPCMD_MUSIC_OFF,
	OPCMD_SOUND_ON,
	OPCMD_SOUND_OFF,

	OPCMD_CROSSHAIR0,
	OPCMD_CROSSHAIR1,
	OPCMD_CROSSHAIR2,
	OPCMD_CROSSHAIR3,

	OPCMD_RESOLUTION,
	OPCMD_KEYSETTING,
	OPCMD_RESOLUTION_UP,
	OPCMD_RESOLUTION_DOWN,
	OPCMD_KEYSETTING_UP,
	OPCMD_KEYSETTING_DOWN,
	OPCMD_RESOLUTION_DRAG,
	OPCMD_KEYSETTING_DRAG,

	OPCMD_SENSI_LOW,
	OPCMD_SENSI_HIGH,
	OPCMD_SENSI_DRAGSUPPORT,
	OPCMD_SENSI_DRAG,
	OPCMD_EDITBOX_SENSI,
	OPCMD_GAMMA_LOW,
	OPCMD_GAMMA_HIGH,
	OPCMD_GAMMA_DRAGSUPPORT,
	OPCMD_GAMMA_DRAG,
	OPCMD_EDITBOX_GAMMA,
	OPCMD_MUSIC_LOW,
	OPCMD_MUSIC_HIGH,
	OPCMD_MUSIC_DRAGSUPPORT,
	OPCMD_MUSIC_DRAG,
	OPCMD_EDITBOX_MUSIC,
	OPCMD_SOUND_LOW,
	OPCMD_SOUND_HIGH,
	OPCMD_SOUND_DRAGSUPPORT,
	OPCMD_SOUND_DRAG,
	OPCMD_EDITBOX_SOUND,

	OPCMD_PERFORMANCE_CHECK0,
	OPCMD_PERFORMANCE_CHECK1,
	OPCMD_PERFORMANCE_CHECK2,
	OPCMD_PERFORMANCE_CHECK3,
	OPCMD_PERFORMANCE_CHECK4,
	OPCMD_PERFORMANCE_CHECK5,
	OPCMD_PERFORMANCE_CHECK6,
	OPCMD_PERFORMANCE_CHECK7,
	OPCMD_PERFORMANCE_CHECK8,
	OPCMD_PERFORMANCE_CHECK9,
	OPCMD_PERFORMANCE_CHECK10,
	OPCMD_PERFORMANCE_CHECK11,
	OPCMD_PERFORMANCE_CHECK12,
	OPCMD_PERFORMANCE_CHECK13,

	OPCMD_ETC_CHECK0,
	OPCMD_ETC_CHECK1,
	OPCMD_ETC_CHECK2,
	OPCMD_ETC_CHECK3,
	OPCMD_ETC_CHECK4,
	OPCMD_ETC_CHECK5,
	OPCMD_ETC_CHECK6,
	OPCMD_ETC_CHECK7,

	OPCMD_SAMPLE_TEST,
};
enum CONTROL
{
	SCREEN_CONTROLS_NUM_DEVICES = 3,
};

namespace
{
    uint32 devices[SCREEN_CONTROLS_NUM_DEVICES] =
	{
		DEVICETYPE_KEYBOARD,
		DEVICETYPE_MOUSE,
		DEVICETYPE_JOYSTICK
	};

	char strDeviceName[SCREEN_CONTROLS_NUM_DEVICES][256] =
	{
		"","",""
	};

	char strDeviceNiceName[SCREEN_CONTROLS_NUM_DEVICES][256] =
	{
		"","",""
	};

    LTBOOL   bEatMouseButtonUp = LTFALSE;

	LTBOOL g_bEsc = FALSE;
	LTBOOL g_bDel = FALSE;
}


VOID ShowMsg(char* szMessage, eMBType eType, MBCallBackFn pfnCallBack, void* pData )
{
	MBCreate mb;
	mb.eType	= eType;
	mb.pFn		= pfnCallBack;
	mb.pData	= pData;
	g_pInterfaceMgr->ShowMessageBox( (char*)szMessage, &mb);			
}

VOID ShowMsg(DWORD dwIdx, eMBType eType, MBCallBackFn pfnCallBack, void* pData )
{
	ShowMsg( NF_GetMsg( dwIdx ), eType, pfnCallBack, pData );			
}

VOID ApplyCallBack(LTBOOL bReturn, void *pData)
{
	CPopScreenOption *pThisScreen = (CPopScreenOption *)pData;
	if(bReturn)
	{
		pThisScreen->ApplyOptionSetting();  // Option Save
		g_pScreenMain->ShowListTab();  
	}
};
VOID OptionDefaultCallBack(LTBOOL bReturn, void *pData)
{
	CPopScreenOption *pThisScreen = (CPopScreenOption *)pData;
	if(bReturn)
	{
		pThisScreen->ReturnOptionValue(PROFILE_ALL);  // (TRUE)Default Setting
	}
};

CPopScreenOption::CPopScreenOption()
{
	// CheckButton
	m_pTabBtnDisplay			= LTNULL;
	m_pTabBtnPerformance		= LTNULL;
	m_pTabBtnCtrl				= LTNULL;
	
	for(int i=0; i<6; i++)
	{
		m_pCkBtnResol[i] = LTNULL;	
		m_bExistResol[i] = LTFALSE;
	}
	
	for(i=0; i<4; i++)
	{
		m_pCkBtnSound_OnOff[i] = LTNULL;
	}

	m_pFmSoundDisable[0] = LTNULL;
	m_pFmSoundDisable[1] = LTNULL;

	// 색품질 
	m_pCkBtnResolBit[0] = LTNULL;
	m_pCkBtnResolBit[1] = LTNULL;
	
	// Performance & Etc
	for(i=0; i<10; i++)
	{
		for(int j=0; j<2; j++)
		{	
			m_pCkBtnPerformance[i][j] = LTNULL;
			m_pCkBtnEtc[i][j]		  = LTNULL;
		}
	}
	m_pPerformanceText			= LTNULL;
	m_pEtcText					= LTNULL;
	m_iPerformanceTotalCnt		= 0;
	m_iEtcTotalCnt				= 0;
	m_bPerforTextVisible		= LTFALSE;
	m_bEtcTextVisible			= LTFALSE;
	m_nCrossHair_CurrentIndex	= 0;
	for(i=0; i<4; i++)
		m_pCkBtnCrossHair[i]	   = LTNULL;
	m_pFmCrossHair = LTNULL;
	
	
	// Button
	m_pBtnInit					= LTNULL;
	m_pBtnOK					= LTNULL;
	m_pBtnCancel				= LTNULL;
	
	// Frame
	m_pFmPerformance			= LTNULL;
	m_pFmControl				= LTNULL;
	
	// List
	m_bKeyBoardEvent			= FALSE;
	m_iArraySize_KB				= 1;
	m_fInputPauseTimeLeft		= 0.0f;
	
	// Drag & Scroll
	m_pBtnResolUp				= LTNULL;
	m_pBtnResolDown				= LTNULL;
	m_pBtnCtrlUp				= LTNULL;
	m_pBtnCtrlDown				= LTNULL;
	m_pScrollResol				= LTNULL;
	m_pScrollCtrl				= LTNULL;
	
	// Package_Sensi
	m_pBtnSensiLow				= LTNULL;
	m_pBtnSensiHigh				= LTNULL;
	m_pDragSupSensi				= LTNULL;
	m_pScrollSensi				= LTNULL;
	m_pEBSensi					= LTNULL;
	m_pFmGauge_Sensi			= LTNULL;
	// Package_Gamma
	m_pBtnGammaLow				= LTNULL;
	m_pBtnGammaHigh				= LTNULL;
	m_pDragSupGamma				= LTNULL;
	m_pScrollGamma				= LTNULL;
	m_pEBGamma					= LTNULL;
	m_pFmGauge_Gamma			= LTNULL;
	// Package_SndMusic
	m_pBtn_SndMusicLow			= LTNULL;
	m_pBtn_SndMusicHigh			= LTNULL;
	m_pDragSupSndMusic			= LTNULL;
	m_pScroll_SndMusic			= LTNULL;
	m_pEB_SndMusic				= LTNULL;
	m_pFmGauge_SndMusic			= LTNULL;
	// Package_SndEffect
	m_pBtn_SndEffectLow			= LTNULL;
	m_pBtn_SndEffectHigh		= LTNULL;
	m_pDragSupSndEffect			= LTNULL;
	m_pScroll_SndEffect			= LTNULL;
	m_pEB_SndEffect				= LTNULL;
	m_pFmGauge_SndEffect		= LTNULL;
	
	m_iTotalIndex				= 0;	// 16bit 와 32bit 위치를 뒤바꾸기 위해 
	
	m_pSaveBindData				= LTNULL;
	m_fDragRatio				= 0.f;

}

CPopScreenOption::~CPopScreenOption()
{
	SAFE_DELETE( m_pSaveBindData );
	SAFE_DELETE_ARRAY( m_pPerformanceText );
;	SAFE_DELETE_ARRAY( m_pEtcText );
}

VOID
CPopScreenOption::Init( CLTGUICommandHandler* pCommandHandler )
{
	// Profile Init
	m_pProfile = g_pProfileMgr->GetCurrentProfile();
	m_pProfile->SetDisplay();
	m_pProfile->SetMouse();
	m_pProfile->SetSound();

	for (int dev = 0; dev < 3; dev++)
	{
		g_pLTClient->GetDeviceName (devices[dev],strDeviceName[dev], sizeof(strDeviceName[dev]));
	}

	// BackGround
	char szStr[128] = {0, };
	char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)SCREEN_ID_OPTIONS);
	//char* pScreenTag = (char*)g_pInterfaceMgr->GetScreenMgr()->GetScreenName((eScreenID)SCREEN_ID_HOST);
	g_pLayoutMgr->GetString(pScreenTag, "BGTexture", szStr, 128 );
	HTEXTURE hFrameTex	  = g_pInterfaceResMgr->GetTexture(szStr);
	LTRect rcFrameTexRect = g_pLayoutMgr->GetRect(pScreenTag, "BGTextureRect");
	LTRect rcFrame		  = g_pLayoutMgr->GetRect( pScreenTag, "BGRect");
	float xRate = (float)rcFrame.right / (float)rcFrameTexRect.right;
	float yRate = (float)rcFrame.bottom / (float)rcFrameTexRect.bottom;
	Create(hFrameTex, rcFrame.right, rcFrame.bottom, xRate, yRate, LTTRUE);
	LTIntPt basePos		  = g_pLayoutMgr->GetPoint(pScreenTag, "BGBasePos");
	SetBasePos(basePos);
	Show(LTTRUE);

	// Frame
	m_pFmControl = new CLTGUIFrame;
	AddFrame(pScreenTag, m_pFmControl, "FmBasePos", "FmControlsTX");
	m_pFmPerformance   = new CLTGUIFrame;
	AddFrame(pScreenTag, m_pFmPerformance, "FmBasePos", "FmPerformanceTX");
	m_pFmEtc   = new CLTGUIFrame;
	AddFrame(pScreenTag, m_pFmEtc, "FmBasePos", "FmEtcTX");
	
	// TabButton
	m_pTabBtnDisplay = new CLTGUIButton;
	AddCheckButton(pScreenTag, m_pTabBtnDisplay, OPCMD_DISPLAY,"TabBtnDisplayRect", "TabBtnDisplayD", "TabBtnDisplayH");
	m_pTabBtnCtrl = new CLTGUIButton;
	AddCheckButton(pScreenTag, m_pTabBtnCtrl, OPCMD_CTRL, "TabBtnCtrlRect", "TabBtnCtrlD", "TabBtnCtrlH");
	m_pTabBtnPerformance  = new CLTGUIButton;
	AddCheckButton(pScreenTag, m_pTabBtnPerformance, OPCMD_PERFORMANCE, "TabBtnPerformanceRect", "TabBtnPerformanceD", "TabBtnPerformanceH");
	m_pTabBtnEtc  = new CLTGUIButton;
	AddCheckButton(pScreenTag, m_pTabBtnEtc, OPCMD_ETC, "TabBtnEtcRect", "TabBtnEtcD", "TabBtnEtcH");
	
	m_pTabBtnDisplay->SetClickSound( (int)IS_CLICK_FLAT );
	m_pTabBtnCtrl->SetClickSound( (int)IS_CLICK_FLAT );
	m_pTabBtnPerformance->SetClickSound( (int)IS_CLICK_FLAT );
	m_pTabBtnEtc->SetClickSound( (int)IS_CLICK_FLAT );

	// Basic Button
	m_pBtnOK 	 = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtnOK, OPCMD_OK, "BtnOK", "BtnOKH", "BtnOKD", "BtnOKC", "BtnOKRect");
	m_pBtnCancel = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtnCancel, OPCMD_CANCEL, "BtnCancel", "BtnCancelH", "BtnCancelD", "BtnCancelC", "BtnCancelRect");
	m_pBtnCancel->SetClickSound( (int)IS_CLICK_CANCEL );

	m_pBtnInit   = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtnInit, OPCMD_INIT, "BtnInit", "BtnInitH", "BtnInitD", "BtnInitC", "BtnInitRect");
	
	// 공통변수 Init
	char szTemp[128] = {LTNULL};
	char szTempOutput[128] = {LTNULL};
	char szTempD[128] = {LTNULL};
	char szTempH[128] = {LTNULL};
	int iCounter = 0;
	int iCommandNum = -1;
	
	
	// Performance
	iCommandNum = OPCMD_PERFORMANCE_CHECK0;
	m_iPerformanceTotalCnt = g_pLayoutMgr->GetInt(pScreenTag, "PerformanceTotalCount");
	LTIntPt nTextBasePos(83, 110);
	m_pPerformanceText = new CSAD3DText[m_iPerformanceTotalCnt];
	for(int i=0; i<m_iPerformanceTotalCnt; i++)
	{
		sprintf(szTemp, "PerformanceText%d", i);
		lstrcpy( szTempOutput, NF_GetMsg( (DWORD)g_pLayoutMgr->GetInt(pScreenTag, szTemp, 0) ) );
		
		m_pPerformanceText[i].CreateD3DText( 180, 25, false );
		m_pPerformanceText[i].SetTextColor( RGB(210,210,210), RGB(0,0,0) );
		m_pPerformanceText[i].SetTransparent(true);
		m_pPerformanceText[i].SetPosition( (float)nTextBasePos.x+190, (float)nTextBasePos.y+(i*30)+80 );
		m_pPerformanceText[i].DrawD3DText( szTempOutput, strlen(szTempOutput), DT_LEFT, D3D_TEXT_SHADOW, true );
		
		for(int j=0; j<2; j++)
		{	
			sprintf(szTemp, "CkBtnPerforRect%d", iCounter);
			m_pCkBtnPerformance[i][j] = new CLTGUIButton;
			sprintf(szTempD, "CkBtnPerforD%d", j);
			sprintf(szTempH, "CkBtnPerforH%d", j);
			AddCheckButton(pScreenTag, m_pCkBtnPerformance[i][j], iCommandNum+iCounter, szTemp, szTempD, szTempH);
			iCounter++;
		}
	}
	
	
	//[SPIKE] 임시
//	g_GameDoc.SetLowViolence(LTTRUE);
	//====================================

	// Etc
	iCounter	= 0;	
	iCommandNum = OPCMD_ETC_CHECK0;
	m_iEtcTotalCnt = g_pLayoutMgr->GetInt(pScreenTag, "EtcTotalCount");

	nTextBasePos.x = 50;
	nTextBasePos.y = 100;
	m_pEtcText = new CSAD3DText[m_iEtcTotalCnt];
	for(i=0; i<m_iEtcTotalCnt; i++)
	{
		sprintf(szTemp, "EtcText%d", i);
		DWORD dwMsgIndex = (DWORD)g_pLayoutMgr->GetInt(pScreenTag, szTemp, 0);
		// 미성년자는 혈흔을 '타격효과'로 변경 !!!!!!!!!!
		if(i == 3)
		{	
			if( g_GameDoc.IsLowViolence() )
			{
				dwMsgIndex = 316;
			}
		}
		//========================================================================

		lstrcpy( szTempOutput, NF_GetMsg( dwMsgIndex ) );
		
		m_pEtcText[i].CreateD3DText( 180, 25, false );
		m_pEtcText[i].SetTextColor( RGB(210,210,210), RGB(0,0,0) );
		m_pEtcText[i].SetTransparent(true);
		m_pEtcText[i].SetPosition( (float)nTextBasePos.x+190, (float)nTextBasePos.y+(i*29)+80 );
		m_pEtcText[i].DrawD3DText( szTempOutput, strlen(szTempOutput), DT_LEFT, D3D_TEXT_SHADOW, true );
		
		for(int j=0; j<2; j++)
		{	
			sprintf(szTemp, "CkBtnEtcRect%d", iCounter);
			m_pCkBtnEtc[i][j] = new CLTGUIButton;
			sprintf(szTempD, "CkBtnEtcD%d", j);
			sprintf(szTempH, "CkBtnEtcH%d", j);
			AddCheckButton(pScreenTag, m_pCkBtnEtc[i][j], iCommandNum+iCounter, szTemp, szTempD, szTempH);
			m_pCkBtnEtc[i][j]->Show(LTFALSE);
			m_pCkBtnEtc[i][j]->SetCheck(LTFALSE);
			iCounter++;
		}
	}

	// 15세는 일단 피효과라는 걸 빼자!!
	if(g_GameDoc.IsLowViolence())
	{
//		m_iEtcTotalCnt--;
//		m_pProfile->m_bGore = 0;
	}
	//===========================================


	// Performance & Etc 체크박스 초기화 
	InitPerforAndEtcCheckButton();


// ResolutionList / ControlList Setting -----------------------	
	
	// Check Button
	iCommandNum = OPCMD_RESOLUTION_CHECK0;
	for(i=0; i<6; i++)
	{
		sprintf(szTemp, "CkBtnResolRect%d", i);
		sprintf(szTempD, "CkBtnResolD%d", i);
		sprintf(szTempH, "CkBtnResolH%d", i);
		m_pCkBtnResol[i] = new CLTGUIButton;
		AddCheckButton(pScreenTag, m_pCkBtnResol[i], iCommandNum+i, szTemp, szTempD, szTempH);
	}

	m_pCkBtnSound_OnOff[0] = new CLTGUIButton;
	AddCheckButton(pScreenTag, m_pCkBtnSound_OnOff[0], OPCMD_MUSIC_OFF, "CkBtnMusicOffRect", "CkBtnSoundOffD", "CkBtnSoundOffH");
	m_pCkBtnSound_OnOff[1] = new CLTGUIButton;
	AddCheckButton(pScreenTag, m_pCkBtnSound_OnOff[1], OPCMD_MUSIC_ON, "CkBtnMusicOnRect", "CkBtnSoundOnD", "CkBtnSoundOnH");
	m_pCkBtnSound_OnOff[2] = new CLTGUIButton;
	AddCheckButton(pScreenTag, m_pCkBtnSound_OnOff[2], OPCMD_SOUND_OFF, "CkBtnSoundOffRect", "CkBtnSoundOffD", "CkBtnSoundOffH");
	m_pCkBtnSound_OnOff[3] = new CLTGUIButton;
	AddCheckButton(pScreenTag, m_pCkBtnSound_OnOff[3], OPCMD_SOUND_ON, "CkBtnSoundOnRect", "CkBtnSoundOnD", "CkBtnSoundOnH");
	
	// CrossHair Select
	iCommandNum = OPCMD_CROSSHAIR0;
	for(i=0; i<4; i++)
	{
		sprintf(szTemp, "CkBtnCrossHairRect%d", i);
		sprintf(szTempD, "CkBtnCrossHairD%d", i);
		sprintf(szTempH, "CkBtnCrossHairH%d", i);
		m_pCkBtnCrossHair[i] = new CLTGUIButton;
		AddCheckButton(pScreenTag, m_pCkBtnCrossHair[i], iCommandNum+i, szTemp, szTempD, szTempH);
	}
	// CrossHair Display
	m_pFmCrossHair = new CLTGUIFrame;
	AddFrame(pScreenTag, m_pFmCrossHair, "FmCrossHairDPPos", "FmCrossHairDP");
	//[MURSUM]
	m_pFmCrossHair->SetSize(64, 64);

	ResetCrossHair_CkButton(m_pProfile->m_nStyle);
	g_pCrosshair->SetStyle(m_pProfile->m_nStyle);

	m_pCkBtnResolBit[0] = new CLTGUIButton;
	AddCheckButton(pScreenTag, m_pCkBtnResolBit[0], OPCMD_RESOLUTION_16BIT, "CkBtnResol16BitRect", "CkBtn16BitD", "CkBtn16BitH");
	m_pCkBtnResolBit[1] = new CLTGUIButton;
	AddCheckButton(pScreenTag, m_pCkBtnResolBit[1], OPCMD_RESOLUTION_32BIT, "CkBtnResol32BitRect", "CkBtn32BitD", "CkBtn32BitH");

	ResolutionValueSetting();

	if(!InitResolutionSelect())
	{	
		// 검색해도 해상도가 안나오면 가장 기초적인 해상도 적용 !!!
		m_pProfile->m_nScreenWidth	= 800;
		m_pProfile->m_nScreenHeight = 600;
		m_pProfile->m_nScreenDepth	= 16;
		m_pProfile->ApplyDisplay();	// w
		InitResolutionSelect();
	}

	CtrlTextSetting_All();

	LTIntPt postest = this->GetBasePos();
	
	// Drag & Scroll
/*	if(m_iTotalIndex >= 7)
	{
		m_pBtnResolUp = new CLTGUIButton;
		AddButton(pScreenTag, m_pBtnResolUp, OPCMD_RESOLUTION_UP, "BtnScrollUp", "BtnScrollUpH", "BtnResolUpRect");
		m_pBtnResolDown = new CLTGUIButton;
		AddButton(pScreenTag, m_pBtnResolDown, OPCMD_RESOLUTION_DOWN, "BtnScrollDown", "BtnScrollDownH", "BtnResolDownRect");
		
		m_pScrollResol = new CLTGUIDragButton();
		AddDragButton(pScreenTag, m_pScrollResol, OPCMD_RESOLUTION_DRAG, "BtnScroll", "BtnScrollH", "BtnResolScrollRect");
		m_pScrollResol->SetLockX(LTTRUE);
		m_pScrollResol->SetLimitY( 109+postest.y, 154+postest.y );
	
	}*/
	m_pBtnCtrlUp = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtnCtrlUp, OPCMD_KEYSETTING_UP, "BtnScrollUp", "BtnScrollUpH", "BtnScrollUpD", "BtnScrollUpC", "BtnCtrlUpRect");
	m_pBtnCtrlDown = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtnCtrlDown, OPCMD_KEYSETTING_DOWN, "BtnScrollDown", "BtnScrollDownH", "BtnScrollDownD", "BtnScrollDownC", "BtnCtrlDownRect");	
	m_pScrollCtrl = new CLTGUIDragButton();
	AddDragButton(pScreenTag, m_pScrollCtrl, OPCMD_KEYSETTING_DRAG, "BtnScroll", "BtnScrollH", "BtnCtrlScrollRect" );
	m_pScrollCtrl->SetLockX(LTTRUE);
	m_pScrollCtrl->SetLimitY( 100+postest.y, 287+postest.y );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Package 	//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Package_Sensitivity /////////////////////////
	char szTempEditCount[128] = {LTNULL};
	LTIntPt ControlBasePos;
	const uint16 nEditboxBasePos = 366;
	//EditBox
	ControlBasePos.x = nEditboxBasePos;
	ControlBasePos.y = 364;
	m_pEBSensi	= new CLTGUIEditBox;
	m_pEBSensi->Create( 0, 0, 30, 24,
						RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
						D3D_TEXT_SHADOW, 
						LTFALSE,
						OPCMD_EDITBOX_SENSI, this );
	AddControl( m_pEBSensi, ControlBasePos );
	sprintf(szTempEditCount, "%d", m_pProfile->m_nSensitivity);
	m_pEBSensi->SetText(szTempEditCount);
	m_pEBSensi->SetNumericOnly(LTTRUE);
	// Button & Scroll
	m_pBtnSensiLow = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtnSensiLow, OPCMD_SENSI_LOW, "BtnLow", "BtnLowH", "BtnLowD", "BtnLowC", "BtnSensiLowRect");
	m_pBtnSensiHigh = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtnSensiHigh, OPCMD_SENSI_HIGH, "BtnHigh", "BtnHighH", "BtnHighD", "BtnHighC", "BtnSensiHighRect");
	
	m_pBtnSensiLow->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtnSensiHigh->SetClickSound( (int)IS_CLICK_NORMAL );
	
	m_pDragSupSensi	= new CDragSupportButton;
	AddButton(pScreenTag, m_pDragSupSensi, OPCMD_SENSI_DRAGSUPPORT, LTNULL, LTNULL, "BtnSensiScrollSupportRect");

	m_pScrollSensi = new CLTGUIDragButton();
	AddDragButton(pScreenTag, m_pScrollSensi, OPCMD_SENSI_DRAG, "BtnMiniScroll", "BtnMiniScrollH", "BtnSensiMiniScrollRect" );
	m_pScrollSensi->SetLockY(LTTRUE);
	m_pScrollSensi->SetLimitX( 71+postest.x, 285+postest.x );
	m_pScrollSensi->SetPosXRatio(((float)m_pEBSensi->GetInt())/100);

	m_pFmGauge_Sensi = new CLTGUIFrame;
	AddFrame(pScreenTag, m_pFmGauge_Sensi, "FmGaugeSensiPos", "FmGaugeTX");
	m_pFmGauge_Sensi->SetSize(((m_pScrollSensi->GetPos().x+OFFSET_POS)-m_pFmGauge_Sensi->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);

// Package_Gamma/////////////////////////
	//EditBox
	ControlBasePos.x = nEditboxBasePos;
	ControlBasePos.y = 236;
	m_pEBGamma	= new CLTGUIEditBox;
	m_pEBGamma->Create( 0, 0, 30, 24,
						RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
						D3D_TEXT_SHADOW, 
						LTFALSE,
						OPCMD_EDITBOX_GAMMA, this );
	AddControl( m_pEBGamma, ControlBasePos );
	if(m_pProfile->m_fGamma < 0.8f) m_pProfile->m_fGamma = 0.8f;
	if(m_pProfile->m_fGamma > 1.8f) m_pProfile->m_fGamma = 1.8f;
	sprintf(szTempEditCount, "%d", (int)((1.0f-(1.8f - m_pProfile->m_fGamma))*100));
	m_pEBGamma->SetText(szTempEditCount);
	m_pEBGamma->SetNumericOnly(LTTRUE);
	// Button & Scroll
	m_pBtnGammaLow = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtnGammaLow, OPCMD_GAMMA_LOW, "BtnDarkN", "BtnDarkH", "BtnDarkD", "BtnDarkC", "BtnGammaLowRect");
	m_pBtnGammaHigh = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtnGammaHigh, OPCMD_GAMMA_HIGH, "BtnBrightN", "BtnBrightH", "BtnBrightD", "BtnBrightC", "BtnGammaHighRect");

	m_pBtnGammaLow->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtnGammaHigh->SetClickSound( (int)IS_CLICK_NORMAL );
	
	m_pDragSupGamma	= new CDragSupportButton;
	AddButton(pScreenTag, m_pDragSupGamma, OPCMD_GAMMA_DRAGSUPPORT, LTNULL, LTNULL, "BtnGammaScrollSupportRect");

	m_pScrollGamma = new CLTGUIDragButton();
	AddDragButton(pScreenTag, m_pScrollGamma, OPCMD_GAMMA_DRAG, "BtnMiniScroll", "BtnMiniScrollH", "BtnGammaMiniScrollRect" );
	m_pScrollGamma->SetLockY(LTTRUE);
	m_pScrollGamma->SetLimitX( 71+postest.x, 285+postest.x );
	m_pScrollGamma->SetPosXRatio(((float)m_pEBGamma->GetInt())/100);

	m_pFmGauge_Gamma = new CLTGUIFrame;
	AddFrame(pScreenTag, m_pFmGauge_Gamma, "FmGaugeGammaPos", "FmGaugeTX");
	m_pFmGauge_Gamma->SetSize(((m_pScrollGamma->GetPos().x+OFFSET_POS)-m_pFmGauge_Gamma->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
// Package_Sound_Music/////////////////////////
	//EditBox
	ControlBasePos.x = nEditboxBasePos;
	ControlBasePos.y = 315;
	m_pEB_SndMusic	= new CLTGUIEditBox;
	m_pEB_SndMusic->Create( 0, 0, 30, 24,
						RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
						D3D_TEXT_SHADOW, 
						LTFALSE,
						OPCMD_EDITBOX_MUSIC, this );
	AddControl( m_pEB_SndMusic, ControlBasePos );
	sprintf(szTempEditCount, "%d", m_pProfile->m_nMusicVolume);
	m_pEB_SndMusic->SetText(szTempEditCount);
	m_pEB_SndMusic->SetNumericOnly(LTTRUE);
	// Button & Scroll
	m_pBtn_SndMusicLow = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtn_SndMusicLow, OPCMD_MUSIC_LOW, "BtnLow", "BtnLowH", "BtnLowD", "BtnLowC", "BtnMusicLowRect");
	m_pBtn_SndMusicHigh = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtn_SndMusicHigh, OPCMD_MUSIC_HIGH, "BtnHigh", "BtnHighH", "BtnHighD", "BtnHighC", "BtnMusicHighRect");	

	m_pBtn_SndMusicLow->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtn_SndMusicHigh->SetClickSound( (int)IS_CLICK_NORMAL );
	
	m_pDragSupSndMusic	= new CDragSupportButton;
	AddButton(pScreenTag, m_pDragSupSndMusic, OPCMD_MUSIC_DRAGSUPPORT, LTNULL, LTNULL, "BtnMusicScrollSupportRect");

	m_pScroll_SndMusic = new CLTGUIDragButton();
	AddDragButton(pScreenTag, m_pScroll_SndMusic, OPCMD_MUSIC_DRAG, "BtnMiniScroll", "BtnMiniScrollH", "BtnMusicMiniScrollRect" );
	m_pScroll_SndMusic->SetLockY(LTTRUE);
	m_pScroll_SndMusic->SetLimitX( 71+postest.x, 285+postest.x );
	m_pScroll_SndMusic->SetPosXRatio(((float)m_pEB_SndMusic->GetInt())/100);

	m_pFmGauge_SndMusic = new CLTGUIFrame;
	AddFrame(pScreenTag, m_pFmGauge_SndMusic, "FmGaugeMusicPos", "FmGaugeTX");
	m_pFmGauge_SndMusic->SetSize(((m_pScroll_SndMusic->GetPos().x+OFFSET_POS)-m_pFmGauge_SndMusic->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
// Package_Sound_Effect/////////////////////////
	//EditBox
	ControlBasePos.x = nEditboxBasePos;
	ControlBasePos.y = 364;
	m_pEB_SndEffect	= new CLTGUIEditBox;
	m_pEB_SndEffect->Create( 0, 0, 30, 24,
						RGB(255,255,255), RGB(0,0,0), 0xFFFFFFFF,
						D3D_TEXT_SHADOW, 
						LTFALSE,
						OPCMD_EDITBOX_SOUND, this );
	AddControl( m_pEB_SndEffect, ControlBasePos );
	sprintf(szTempEditCount, "%d", m_pProfile->m_nSoundVolume);
	m_pEB_SndEffect->SetText(szTempEditCount);
	m_pEB_SndEffect->SetNumericOnly(LTTRUE);
	// Button & Scroll
	m_pBtn_SndEffectLow = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtn_SndEffectLow, OPCMD_SOUND_LOW, "BtnLow", "BtnLowH", "BtnLowD", "BtnLowC", "BtnSndEffectLowRect");
	m_pBtn_SndEffectHigh = new CLTGUIButton;
	AddButton(pScreenTag, m_pBtn_SndEffectHigh, OPCMD_SOUND_HIGH, "BtnHigh", "BtnHighH", "BtnHighD", "BtnHighC", "BtnSndEffectHighRect");	

	m_pBtn_SndEffectLow->SetClickSound( (int)IS_CLICK_NORMAL );
	m_pBtn_SndEffectHigh->SetClickSound( (int)IS_CLICK_NORMAL );
	
	m_pDragSupSndEffect	= new CDragSupportButton;
	AddButton(pScreenTag, m_pDragSupSndEffect, OPCMD_SOUND_DRAGSUPPORT, LTNULL, LTNULL, "BtnSndEffectScrollSupportRect");
	
	m_pScroll_SndEffect = new CLTGUIDragButton();
	AddDragButton(pScreenTag, m_pScroll_SndEffect, OPCMD_SOUND_DRAG, "BtnMiniScroll", "BtnMiniScrollH", "BtnSndEffectMiniScrollRect" );
	m_pScroll_SndEffect->SetLockY(LTTRUE);
	m_pScroll_SndEffect->SetLimitX( 71+postest.x, 285+postest.x );
	m_pScroll_SndEffect->SetPosXRatio(((float)m_pEB_SndEffect->GetInt())/100);

	m_pFmGauge_SndEffect = new CLTGUIFrame;
	AddFrame(pScreenTag, m_pFmGauge_SndEffect, "FmGaugeSndEffectPos", "FmGaugeTX");
	m_pFmGauge_SndEffect->SetSize(((m_pScroll_SndEffect->GetPos().x+OFFSET_POS)-m_pFmGauge_SndEffect->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);


	
	m_pFmSoundDisable[0]   = new CLTGUIButton;
	AddButton(pScreenTag, m_pFmSoundDisable[0], OPCMD_SAMPLE_TEST,"BtnMusicDisableTX", "BtnMusicDisableTX", "BtnMusicDisableTX", "BtnMusicDisableTX", "BtnMusicDisablePos");
	m_pFmSoundDisable[1]   = new CLTGUIButton;
	AddButton(pScreenTag, m_pFmSoundDisable[1], OPCMD_SAMPLE_TEST,"BtnSoundDisableTX", "BtnSoundDisableTX", "BtnSoundDisableTX", "BtnSoundDisableTX", "BtnSoundDisablePos");
	
	ResetSoundCheckButton();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Package End	//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Extern 
	g_pPopupOption = this;
	
	SetParentScreen( (CScreenBase*)pCommandHandler );

	m_pProfile->Save(TRUE);
}

// Handle a keypress
LTBOOL 
CPopScreenOption::HandleKeyDown(int key, int rep)
{
    LTBOOL handled = LTFALSE;
	g_bEsc = FALSE;
	g_bDel = FALSE;
	
	if(m_pControlList->m_iIndex >= 0)
	{
		switch (key)
		{
		case VK_ESCAPE:
			g_bEsc = TRUE;
			m_pControlList->SetCommandText(m_pControlList->m_iIndex, m_pControlList->m_szSaveText);
			Test_SameText(m_pControlList->m_iListVectorIndex);
			m_pControlList->SetSelectIndex(-1);
			m_pControlList->m_bWaitingKey = FALSE;
			g_pLTClient->EndDeviceTrack();
			return LTFALSE;
			break;
		case VK_DELETE:
			g_bDel = TRUE;
			//														"[ 사용안함 ]"
			m_pControlList->SetCommandText(m_pControlList->m_iIndex, NF_GetMsg(SA_TEXT_OPTION_UNASSIGNED) );
			CBindingData* pUnBindData = m_pProfile->FindBinding(m_pControlList->m_iListVectorIndex);
			if( !pUnBindData ) return LTFALSE;

			for(int i=0; i<3; i++)
			{
				strcpy(pUnBindData->strRealName[i],"");
				strcpy(pUnBindData->strTriggerName[i],"");
				pUnBindData->nDeviceObjectId[i] = 0;
			}

			m_pControlList->SetSelectIndex(-1);
			m_pControlList->m_bWaitingKey = FALSE;
			g_pLTClient->EndDeviceTrack();
			return LTFALSE;
			break;
		}
		// Keyboard Input Setting 
		m_iArraySize_KB = 9;
		g_pLTClient->TrackDevice(m_pInputArray_KB, &m_iArraySize_KB);
		if(m_pInputArray_KB->m_InputValue)
			m_bKeyBoardEvent = TRUE;
		else
			m_bKeyBoardEvent = FALSE;
	}
	
	// Check to see if the base class is handling this key
	if (m_pControlList->m_bWaitingKey)
		return handled;
	if (!handled)
	{
		handled = CLTGUIWindow::HandleKeyDown(key, rep);
	}
	// Handled the key
	return handled;
}

LTBOOL   
CPopScreenOption::OnWheelUp(int x, int y)
{
	if(m_pControlList->IsVisible() && !m_pControlList->m_bWaitingKey)
	{
		m_fDragRatio -= 0.1f;
		if(m_fDragRatio < 0.0f) m_fDragRatio = 0.0f;
		m_pScrollCtrl->SetPosYRatio(m_fDragRatio);
		m_pControlList->ScrollFromRatio( m_pScrollCtrl->GetPosYRatio() );
	}
/*	else if(m_iTotalIndex >= 7)
	{
		if(m_pResolutionList->IsVisible())
		{
			m_fDragRatio -= 0.1f;
			if(m_fDragRatio < 0.0f) m_fDragRatio = 0.0f;
			m_pScrollResol->SetPosYRatio(m_fDragRatio);
			m_pResolutionList->ScrollFromRatio( m_pScrollResol->GetPosYRatio() );
		}
	}*/
	return LTTRUE;
}

LTBOOL   
CPopScreenOption::OnWheelDown(int x, int y)
{
	if(m_pControlList->IsVisible() && !m_pControlList->m_bWaitingKey)
	{
		m_fDragRatio += 0.1f;
		if(m_fDragRatio > 1.0f) m_fDragRatio = 1.0f;
		m_pScrollCtrl->SetPosYRatio(m_fDragRatio);
		m_pControlList->ScrollFromRatio( m_pScrollCtrl->GetPosYRatio() );
	}
/*	else if(m_iTotalIndex >= 7)
	{
		if(m_pResolutionList->IsVisible())
		{
			m_fDragRatio += 0.1f;
			if(m_fDragRatio > 1.0f) m_fDragRatio = 1.0f;
			m_pScrollResol->SetPosYRatio(m_fDragRatio);
			m_pResolutionList->ScrollFromRatio( m_pScrollResol->GetPosYRatio() );
		}
	}*/
   return LTTRUE;
}
LTBOOL 
CPopScreenOption::OnMouseMove(int x, int y)
{
	return CLTGUIWindow::OnMouseMove( x, y );
}
LTBOOL 
CPopScreenOption::OnLButtonUp(int x, int y)
{
	CLTGUIWindow::OnLButtonUp( x, y );  // PopWindow Button이 먼저 처리해야 됨 
	uint16 nControlIndex = 0;
	if (GetControlUnderPoint(x, y, &nControlIndex))
	{
		CLTGUICtrl* pCtrl = GetControl(nControlIndex);
	/*	if (pCtrl == m_pSlider_Gamma)
		{
			m_pProfile->m_fGamma = ;
			g_pGameClientShell->SetGammaRamp( m_pProfile->m_fGamma );
		}*/
	}
	return LTTRUE;
}

LTBOOL 
CPopScreenOption::OnLButtonDown(int x, int y)
{
	CLTGUIWindow::OnLButtonDown( x, y );  // PopWindow Button이 먼저 처리해야 됨 
	uint16 nControlIndex = 0, nTemp;
	char szTempEditCount[128] = {LTNULL};
	
	if (GetControlUnderPoint(x, y, &nControlIndex))
	{
		CLTGUICtrl* pCtrl = GetControl(nControlIndex);
		if (pCtrl == m_pDragSupSensi)
		{
			m_pScrollSensi->SetPosXRatio(m_pDragSupSensi->CalculateDragButtonRatio(x));
			nTemp = (uint16)((m_pScrollSensi->GetPosXRatio()+0.01f)*100);
			if(nTemp > 100) nTemp = 100;
			sprintf(szTempEditCount, "%d", nTemp);
			m_pEBSensi->SetText(szTempEditCount);
			m_pFmGauge_Sensi->SetSize(((m_pScrollSensi->GetPos().x+OFFSET_POS)-m_pFmGauge_Sensi->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		}
		else if(pCtrl == m_pDragSupGamma)
		{
			m_pScrollGamma->SetPosXRatio(m_pDragSupGamma->CalculateDragButtonRatio(x));
			nTemp = (uint16)((m_pScrollGamma->GetPosXRatio()+0.01f)*100);
			if(nTemp > 100) nTemp = 100;
			sprintf(szTempEditCount, "%d", nTemp);
			m_pEBGamma->SetText(szTempEditCount);
			m_pProfile->m_fGamma = 0.8f + ((float)m_pEBGamma->GetInt())/100;
			g_pGameClientShell->SetGammaRamp( m_pProfile->m_fGamma );
			m_pFmGauge_Gamma->SetSize(((m_pScrollGamma->GetPos().x+OFFSET_POS)-m_pFmGauge_Gamma->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		}
		else if(pCtrl == m_pDragSupSndMusic)
		{
			m_pScroll_SndMusic->SetPosXRatio(m_pDragSupSndMusic->CalculateDragButtonRatio(x));
			nTemp = (uint16)((m_pScroll_SndMusic->GetPosXRatio()+0.01f)*100);
			if(nTemp > 100) nTemp = 100;
			sprintf(szTempEditCount, "%d", nTemp);
			m_pEB_SndMusic->SetText(szTempEditCount);
			m_pFmGauge_SndMusic->SetSize(((m_pScroll_SndMusic->GetPos().x+OFFSET_POS)-m_pFmGauge_SndMusic->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
			// BGM
			m_pProfile->ReSetBGMVolumeRatio(((float)m_pEB_SndMusic->GetInt())/100);
		}
		else if(pCtrl == m_pDragSupSndEffect)
		{
			m_pScroll_SndEffect->SetPosXRatio(m_pDragSupSndEffect->CalculateDragButtonRatio(x));
			nTemp = (uint16)((m_pScroll_SndEffect->GetPosXRatio()+0.01f)*100);
			if(nTemp > 100) nTemp = 100;
			sprintf(szTempEditCount, "%d", nTemp);
			m_pEB_SndEffect->SetText(szTempEditCount);
			m_pFmGauge_SndEffect->SetSize(((m_pScroll_SndEffect->GetPos().x+OFFSET_POS)-m_pFmGauge_SndEffect->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		}
	}

	return LTTRUE;
}

LTBOOL CPopScreenOption::OnEnter()
{
	OnCommand( OPCMD_OK, LTNULL, LTNULL );
	return LTTRUE;
}

LTBOOL CPopScreenOption::OnEscape()
{
	if( LTTRUE /*[TODO] 키입력 대기상태가 아닐때만*/ )
	{
		OnCommand( OPCMD_CANCEL, LTNULL, LTNULL );		
	}

	return LTTRUE;
}

VOID 
CPopScreenOption::Render()
{
	if( IsVisible() == LTFALSE ) return;
	
	ControlRender();

	CLTGUIWindow::Render();

	if(m_bPerforTextVisible)
	{
		for(int i=0; i<m_iPerformanceTotalCnt; i++)
			m_pPerformanceText[i].Render();
	}
	else if(m_bEtcTextVisible)
	{
		for(int j=0; j<m_iEtcTotalCnt; j++)
		{
			m_pEtcText[j].Render();
		}
	}
}

uint32 CPopScreenOption::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{	
	char szTempEditCount[128] = {LTNULL};
	uint8 nTemp = 0;
	uint8 nIndex = 0;
	switch(dwCommand) {
	case OPCMD_DISPLAY:
		OptionFrameSetting(OPCMD_DISPLAY);
		break;
	case OPCMD_PERFORMANCE:
		OptionFrameSetting(OPCMD_PERFORMANCE);
		break;
	case OPCMD_CTRL:
		OptionFrameSetting(OPCMD_CTRL);
		break;
	case OPCMD_ETC:
		OptionFrameSetting(OPCMD_ETC);
		break;
	case OPCMD_INIT:
		//"모든 옵션값을 초기화 합니다. \n\n적용할까요?"
		ShowMsg( SA_TEXT_OPTION_RESET, LTMB_YESNO, OptionDefaultCallBack, this );
		break;
	case OPCMD_OK:
		if(!m_pControlList->m_bWaitingKey)
		{
			//"새로 설정한 모든 값이 적용됩니다. \n\n적용할까요?"
			ShowMsg( SA_TEXT_OPTION_APPLY, LTMB_YESNO, ApplyCallBack, this );
		}
		break;
	case OPCMD_CANCEL:
		ReturnOptionValue();
		g_pScreenMain->ShowListTab();   
		break;

	case OPCMD_RESOLUTION_CHECK0:
		ResetResolCheckButton(0);
		if(m_pCkBtnResolBit[0]->GetCheck())
			nIndex = 0;
		else nIndex = 6;
		m_pResolutionList->SetSelectIndex(nIndex);
		break;
	case OPCMD_RESOLUTION_CHECK1:
		ResetResolCheckButton(1);
		if(m_pCkBtnResolBit[0]->GetCheck())
			nIndex = 1;
		else nIndex = 7;
		m_pResolutionList->SetSelectIndex(nIndex);
		break;
	case OPCMD_RESOLUTION_CHECK2:
		ResetResolCheckButton(2);
		if(m_pCkBtnResolBit[0]->GetCheck())
			nIndex = 2;
		else nIndex = 8;
		m_pResolutionList->SetSelectIndex(nIndex);
		break;
	case OPCMD_RESOLUTION_CHECK3:
		ResetResolCheckButton(3);
		if(m_pCkBtnResolBit[0]->GetCheck())
			nIndex = 3;
		else nIndex = 9;
		m_pResolutionList->SetSelectIndex(nIndex);
		break;
	case OPCMD_RESOLUTION_CHECK4:
		ResetResolCheckButton(4);
		if(m_pCkBtnResolBit[0]->GetCheck())
			nIndex = 4;
		else nIndex = 10;
		m_pResolutionList->SetSelectIndex(nIndex);
		break;
	case OPCMD_RESOLUTION_CHECK5:
		ResetResolCheckButton(5);
		if(m_pCkBtnResolBit[0]->GetCheck())
			nIndex = 5;
		else nIndex = 11;
		m_pResolutionList->SetSelectIndex(nIndex);
		break;
	
	case OPCMD_RESOLUTION_16BIT:
		if(m_pResolutionList->GetSelectIndex() > 5)
		{
			nIndex = m_pResolutionList->GetSelectIndex() - 6;
			m_pResolutionList->SetSelectIndex(nIndex);
		}		
		m_pCkBtnResolBit[0]->SetCheck();
		m_pCkBtnResolBit[1]->SetCheck(LTFALSE);
		break;
	case OPCMD_RESOLUTION_32BIT:
		if(m_pResolutionList->GetSelectIndex() <= 5)
		{
			nIndex = m_pResolutionList->GetSelectIndex() + 6;
			m_pResolutionList->SetSelectIndex(nIndex);
		}
		m_pCkBtnResolBit[1]->SetCheck();
		m_pCkBtnResolBit[0]->SetCheck(LTFALSE);
		break;

	case OPCMD_MUSIC_OFF:
		if(!m_pCkBtnSound_OnOff[0]->GetCheck())
		{
			m_pCkBtnSound_OnOff[0]->SetCheck(LTTRUE);
			m_pCkBtnSound_OnOff[1]->SetCheck(LTFALSE);
			m_pFmSoundDisable[0]->Show(LTTRUE);
			m_pProfile->m_bMusicEnable = LTFALSE;
			g_pInterfaceMgr->KillBGM();
		}
		break;
	case OPCMD_MUSIC_ON:
		if( false == g_pGameClientShell->IsSoundInitted() )
		{
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_SND_DEVICE_NOT_FOUND, LTMB_OK, LTNULL, LTNULL, LTFALSE );
		}
		else if(!m_pCkBtnSound_OnOff[1]->GetCheck())
		{
			m_pCkBtnSound_OnOff[1]->SetCheck(LTTRUE);
			m_pCkBtnSound_OnOff[0]->SetCheck(LTFALSE);
			m_pFmSoundDisable[0]->Show(LTFALSE);
			m_pProfile->m_bMusicEnable = LTTRUE;

			//BGM
			g_pInterfaceMgr->PlayBGM( "SA_INTERFACE\\SND\\BG\\Screen.wav", LTTRUE );
			m_pProfile->ReSetBGMVolumeRatio(((float)m_pEB_SndMusic->GetInt())/100);
		}
		break;
	case OPCMD_SOUND_OFF:
		if(!m_pCkBtnSound_OnOff[2]->GetCheck())
		{
			m_pCkBtnSound_OnOff[2]->SetCheck(LTTRUE);
			m_pCkBtnSound_OnOff[3]->SetCheck(LTFALSE);
			m_pFmSoundDisable[1]->Show(LTTRUE);
			m_pProfile->m_bSoundEnable = LTFALSE;
		}
		break;
	case OPCMD_SOUND_ON:
		if( false == g_pGameClientShell->IsSoundInitted() )
		{
			g_pInterfaceMgr->ShowMessageBox( SA_TEXT_SND_DEVICE_NOT_FOUND, LTMB_OK, LTNULL, LTNULL, LTFALSE );
		}
		else if(!m_pCkBtnSound_OnOff[3]->GetCheck())
		{
			m_pCkBtnSound_OnOff[3]->SetCheck(LTTRUE);
			m_pCkBtnSound_OnOff[2]->SetCheck(LTFALSE);
			m_pFmSoundDisable[1]->Show(LTFALSE);
			m_pProfile->m_bSoundEnable = LTTRUE;
		}
		break;

		
	case OPCMD_CROSSHAIR0:
		ResetCrossHair_CkButton(0);
		break;
	case OPCMD_CROSSHAIR1:
		ResetCrossHair_CkButton(1);
		break;
	case OPCMD_CROSSHAIR2:
		ResetCrossHair_CkButton(2);
		break;
	case OPCMD_CROSSHAIR3:
		ResetCrossHair_CkButton(3);
		break;


	case OPCMD_RESOLUTION_UP:
/*		if(m_iTotalIndex >= 7)
		{
			if( m_pResolutionList->IsEnableScroll() )
			{
				m_pResolutionList->ScrollUp();
				m_pScrollResol->SetPosYRatio( m_pResolutionList->GetRatio() );
				m_fDragRatio = m_pScrollResol->GetPosYRatio()+0.01f;
			}
		}*/
		break;
	case OPCMD_RESOLUTION_DOWN:
/*		if(m_iTotalIndex >= 7)
		{
			if( m_pResolutionList->IsEnableScroll() )
			{
				m_pResolutionList->ScrollDown();
				m_pScrollResol->SetPosYRatio( m_pResolutionList->GetRatio() );
				m_fDragRatio = m_pScrollResol->GetPosYRatio()+0.01f;
			}
		}*/
		break;
	case OPCMD_RESOLUTION_DRAG:
/*		if(m_iTotalIndex >= 7)
		{
			if( dwParam1 == CMD_PARAM_MOUSEMOVE ) 
			{
				m_pResolutionList->ScrollFromRatio( m_pScrollResol->GetPosYRatio() );
				m_fDragRatio = m_pScrollResol->GetPosYRatio()+0.01f;
			}
		}*/
		break;
	case OPCMD_KEYSETTING_UP:
		if( m_pControlList->IsEnableScroll() && !m_pControlList->m_bWaitingKey)
		{
			m_pControlList->ScrollUp();
			m_pScrollCtrl->SetPosYRatio( m_pControlList->GetRatio() );
			m_fDragRatio = m_pScrollCtrl->GetPosYRatio()+0.01f;
		}
		break;
	case OPCMD_KEYSETTING_DOWN:
		if( m_pControlList->IsEnableScroll() && !m_pControlList->m_bWaitingKey)
		{
			m_pControlList->ScrollDown();
			m_pScrollCtrl->SetPosYRatio( m_pControlList->GetRatio() );
			m_fDragRatio = m_pScrollCtrl->GetPosYRatio()+0.01f;
		}
		break;
	case OPCMD_KEYSETTING_DRAG:
		if( dwParam1 == CMD_PARAM_MOUSEMOVE && !m_pControlList->m_bWaitingKey) 
		{
			m_pControlList->ScrollFromRatio( m_pScrollCtrl->GetPosYRatio()+0.01f );
			m_fDragRatio = m_pScrollCtrl->GetPosYRatio()+0.01f;
		}
		
		break;

	case OPCMD_SENSI_LOW:
		if(m_pEBSensi->GetInt() <= 0) return 0;
		sprintf(szTempEditCount, "%d", m_pEBSensi->GetInt()-1);
		m_pEBSensi->SetText(szTempEditCount);
		m_pScrollSensi->SetPosXRatio(((float)m_pEBSensi->GetInt())/100);
		m_pFmGauge_Sensi->SetSize(((m_pScrollSensi->GetPos().x+OFFSET_POS)-m_pFmGauge_Sensi->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		break;
	case OPCMD_SENSI_HIGH:
		if(m_pEBSensi->GetInt() >= 100) return 0;
		sprintf(szTempEditCount, "%d", m_pEBSensi->GetInt()+1);
		m_pEBSensi->SetText(szTempEditCount);
		m_pScrollSensi->SetPosXRatio(((float)m_pEBSensi->GetInt())/100);
		m_pFmGauge_Sensi->SetSize(((m_pScrollSensi->GetPos().x+OFFSET_POS)-m_pFmGauge_Sensi->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		break;
	case OPCMD_SENSI_DRAG:
		if( dwParam1 == CMD_PARAM_MOUSEMOVE) 
		{
			nTemp = (uint8)((m_pScrollSensi->GetPosXRatio()+0.01f)*100);
			if(nTemp > 100) nTemp = 100;
			sprintf(szTempEditCount, "%d", nTemp);
			m_pEBSensi->SetText(szTempEditCount);
			m_pFmGauge_Sensi->SetSize(((m_pScrollSensi->GetPos().x+OFFSET_POS)-m_pFmGauge_Sensi->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		}
		break;
	case OPCMD_EDITBOX_SENSI:
		if(m_pEBSensi->GetInt() > 100)
		{
			sprintf(szTempEditCount, "%d", 100);
			m_pEBSensi->SetText(szTempEditCount);
		}
		m_pScrollSensi->SetPosXRatio(((float)m_pEBSensi->GetInt())/100);
		m_pFmGauge_Sensi->SetSize(((m_pScrollSensi->GetPos().x+OFFSET_POS)-m_pFmGauge_Sensi->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		break;
		
	case OPCMD_GAMMA_LOW:
		if(m_pEBGamma->GetInt() <= 0) return 0;
		sprintf(szTempEditCount, "%d", m_pEBGamma->GetInt()-1);
		m_pEBGamma->SetText(szTempEditCount);
		m_pScrollGamma->SetPosXRatio(((float)m_pEBGamma->GetInt())/100);
		m_pProfile->m_fGamma = 0.8f + ((float)m_pEBGamma->GetInt())/100;
		g_pGameClientShell->SetGammaRamp( m_pProfile->m_fGamma );
		m_pFmGauge_Gamma->SetSize(((m_pScrollGamma->GetPos().x+OFFSET_POS)-m_pFmGauge_Gamma->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		break;
	case OPCMD_GAMMA_HIGH:
		if(m_pEBGamma->GetInt() >= 100) return 0;
		sprintf(szTempEditCount, "%d", m_pEBGamma->GetInt()+1);
		m_pEBGamma->SetText(szTempEditCount);
		m_pScrollGamma->SetPosXRatio(((float)m_pEBGamma->GetInt())/100);
		m_pProfile->m_fGamma = 0.8f + ((float)m_pEBGamma->GetInt())/100;
		g_pGameClientShell->SetGammaRamp( m_pProfile->m_fGamma );
		m_pFmGauge_Gamma->SetSize(((m_pScrollGamma->GetPos().x+OFFSET_POS)-m_pFmGauge_Gamma->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		break;
	case OPCMD_GAMMA_DRAG:
		if( dwParam1 == CMD_PARAM_MOUSEMOVE) 
		{
			nTemp = (uint8)((m_pScrollGamma->GetPosXRatio()+0.01f)*100);
			if(nTemp > 100) nTemp = 100;
			sprintf(szTempEditCount, "%d", nTemp);
			m_pEBGamma->SetText(szTempEditCount);
			m_pProfile->m_fGamma = 0.8f + ((float)m_pEBGamma->GetInt())/100;
			g_pGameClientShell->SetGammaRamp( m_pProfile->m_fGamma );
			m_pFmGauge_Gamma->SetSize(((m_pScrollGamma->GetPos().x+OFFSET_POS)-m_pFmGauge_Gamma->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		}
		break;
	case OPCMD_EDITBOX_GAMMA:
		if(m_pEBGamma->GetInt() > 100)
		{
			sprintf(szTempEditCount, "%d", 100);
			m_pEBGamma->SetText(szTempEditCount);
		}
		m_pScrollGamma->SetPosXRatio(((float)m_pEBGamma->GetInt())/100);
		m_pProfile->m_fGamma = 0.8f + ((float)m_pEBGamma->GetInt())/100;
		g_pGameClientShell->SetGammaRamp( m_pProfile->m_fGamma );
		m_pFmGauge_Gamma->SetSize(((m_pScrollGamma->GetPos().x+OFFSET_POS)-m_pFmGauge_Gamma->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		break;

	case OPCMD_MUSIC_LOW:
		if(m_pEB_SndMusic->GetInt() <= 0) return 0;
		sprintf(szTempEditCount, "%d", m_pEB_SndMusic->GetInt()-1);
		m_pEB_SndMusic->SetText(szTempEditCount);
		m_pScroll_SndMusic->SetPosXRatio(((float)m_pEB_SndMusic->GetInt())/100);
		m_pFmGauge_SndMusic->SetSize(((m_pScroll_SndMusic->GetPos().x+OFFSET_POS)-m_pFmGauge_SndMusic->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		
		// BGM
		m_pProfile->ReSetBGMVolumeRatio(((float)m_pEB_SndMusic->GetInt())/100);
		break;
	case OPCMD_MUSIC_HIGH:
		if(m_pEB_SndMusic->GetInt() >= 100) return 0;
		sprintf(szTempEditCount, "%d", m_pEB_SndMusic->GetInt()+1);
		m_pEB_SndMusic->SetText(szTempEditCount);
		m_pScroll_SndMusic->SetPosXRatio(((float)m_pEB_SndMusic->GetInt())/100);
		m_pFmGauge_SndMusic->SetSize(((m_pScroll_SndMusic->GetPos().x+OFFSET_POS)-m_pFmGauge_SndMusic->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);

		// BGM
		m_pProfile->ReSetBGMVolumeRatio(((float)m_pEB_SndMusic->GetInt())/100);
		break;
	case OPCMD_MUSIC_DRAG:
		if( dwParam1 == CMD_PARAM_MOUSEMOVE) 
		{
			nTemp = (uint8)((m_pScroll_SndMusic->GetPosXRatio()+0.01f)*100);
			if(nTemp > 100) nTemp = 100;
			sprintf(szTempEditCount, "%d", nTemp);
			m_pEB_SndMusic->SetText(szTempEditCount);
			m_pFmGauge_SndMusic->SetSize(((m_pScroll_SndMusic->GetPos().x+OFFSET_POS)-m_pFmGauge_SndMusic->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);

			// BGM
			m_pProfile->ReSetBGMVolumeRatio(((float)m_pEB_SndMusic->GetInt())/100);
		}
		break;
	case OPCMD_EDITBOX_MUSIC:
		if(m_pEB_SndMusic->GetInt() > 100)
		{
			sprintf(szTempEditCount, "%d", 100);
			m_pEB_SndMusic->SetText(szTempEditCount);
		}
		m_pScroll_SndMusic->SetPosXRatio(((float)m_pEB_SndMusic->GetInt())/100);
		m_pFmGauge_SndMusic->SetSize(((m_pScroll_SndMusic->GetPos().x+OFFSET_POS)-m_pFmGauge_SndMusic->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);

		// BGM
		m_pProfile->ReSetBGMVolumeRatio(((float)m_pEB_SndMusic->GetInt())/100);
		break;

	case OPCMD_SOUND_LOW:
		if(m_pEB_SndEffect->GetInt() <= 0) return 0;
		sprintf(szTempEditCount, "%d", m_pEB_SndEffect->GetInt()-1);
		m_pEB_SndEffect->SetText(szTempEditCount);
		m_pScroll_SndEffect->SetPosXRatio(((float)m_pEB_SndEffect->GetInt())/100);
		m_pFmGauge_SndEffect->SetSize(((m_pScroll_SndEffect->GetPos().x+OFFSET_POS)-m_pFmGauge_SndEffect->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		break;
	case OPCMD_SOUND_HIGH:
		if(m_pEB_SndEffect->GetInt() >= 100) return 0;
		sprintf(szTempEditCount, "%d", m_pEB_SndEffect->GetInt()+1);
		m_pEB_SndEffect->SetText(szTempEditCount);
		m_pScroll_SndEffect->SetPosXRatio(((float)m_pEB_SndEffect->GetInt())/100);
		m_pFmGauge_SndEffect->SetSize(((m_pScroll_SndEffect->GetPos().x+OFFSET_POS)-m_pFmGauge_SndEffect->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		break;
	case OPCMD_SOUND_DRAG:
		if( dwParam1 == CMD_PARAM_MOUSEMOVE) 
		{
			nTemp = (uint8)((m_pScroll_SndEffect->GetPosXRatio()+0.01f)*100);
			if(nTemp > 100) nTemp = 100;
			sprintf(szTempEditCount, "%d", nTemp);
			m_pEB_SndEffect->SetText(szTempEditCount);
			m_pFmGauge_SndEffect->SetSize(((m_pScroll_SndEffect->GetPos().x+OFFSET_POS)-m_pFmGauge_SndEffect->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		}
		break;
	case OPCMD_EDITBOX_SOUND:
		if(m_pEB_SndEffect->GetInt() > 100)
		{
			sprintf(szTempEditCount, "%d", 100);
			m_pEB_SndEffect->SetText(szTempEditCount);
		}
		m_pScroll_SndEffect->SetPosXRatio(((float)m_pEB_SndEffect->GetInt())/100);
		m_pFmGauge_SndEffect->SetSize(((m_pScroll_SndEffect->GetPos().x+OFFSET_POS)-m_pFmGauge_SndEffect->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
		break;

	case OPCMD_PERFORMANCE_CHECK0:
		m_pCkBtnPerformance[0][0]->SetCheck();
		m_pCkBtnPerformance[0][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_PERFORMANCE_CHECK1:
		m_pCkBtnPerformance[0][0]->SetCheck(LTFALSE);
		m_pCkBtnPerformance[0][1]->SetCheck();
		break;
	case OPCMD_PERFORMANCE_CHECK2:
		m_pCkBtnPerformance[1][0]->SetCheck();
		m_pCkBtnPerformance[1][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_PERFORMANCE_CHECK3:
		m_pCkBtnPerformance[1][0]->SetCheck(LTFALSE);
		m_pCkBtnPerformance[1][1]->SetCheck();
		break;
	case OPCMD_PERFORMANCE_CHECK4:
		m_pCkBtnPerformance[2][0]->SetCheck();
		m_pCkBtnPerformance[2][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_PERFORMANCE_CHECK5:
		m_pCkBtnPerformance[2][0]->SetCheck(LTFALSE);
		m_pCkBtnPerformance[2][1]->SetCheck();
		break;
	case OPCMD_PERFORMANCE_CHECK6:
		m_pCkBtnPerformance[3][0]->SetCheck();
		m_pCkBtnPerformance[3][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_PERFORMANCE_CHECK7:
		m_pCkBtnPerformance[3][0]->SetCheck(LTFALSE);
		m_pCkBtnPerformance[3][1]->SetCheck();
		break;
	case OPCMD_PERFORMANCE_CHECK8:
		m_pCkBtnPerformance[4][0]->SetCheck();
		m_pCkBtnPerformance[4][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_PERFORMANCE_CHECK9:
		m_pCkBtnPerformance[4][0]->SetCheck(LTFALSE);
		m_pCkBtnPerformance[4][1]->SetCheck();
		break;
	case OPCMD_PERFORMANCE_CHECK10:
		m_pCkBtnPerformance[5][0]->SetCheck();
		m_pCkBtnPerformance[5][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_PERFORMANCE_CHECK11:
		m_pCkBtnPerformance[5][0]->SetCheck(LTFALSE);
		m_pCkBtnPerformance[5][1]->SetCheck();
		break;
	case OPCMD_PERFORMANCE_CHECK12:
		m_pCkBtnPerformance[6][0]->SetCheck();
		m_pCkBtnPerformance[6][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_PERFORMANCE_CHECK13:
		m_pCkBtnPerformance[6][0]->SetCheck(LTFALSE);
		m_pCkBtnPerformance[6][1]->SetCheck();
		break;

	case OPCMD_ETC_CHECK0:
		m_pCkBtnEtc[0][0]->SetCheck();
		m_pCkBtnEtc[0][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_ETC_CHECK1:
		m_pCkBtnEtc[0][0]->SetCheck(LTFALSE);
		m_pCkBtnEtc[0][1]->SetCheck();
		break;
	case OPCMD_ETC_CHECK2:
		m_pCkBtnEtc[1][0]->SetCheck();
		m_pCkBtnEtc[1][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_ETC_CHECK3:
		m_pCkBtnEtc[1][0]->SetCheck(LTFALSE);
		m_pCkBtnEtc[1][1]->SetCheck();
		break;
	case OPCMD_ETC_CHECK4:
		m_pCkBtnEtc[2][0]->SetCheck();
		m_pCkBtnEtc[2][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_ETC_CHECK5:
		m_pCkBtnEtc[2][0]->SetCheck(LTFALSE);
		m_pCkBtnEtc[2][1]->SetCheck();
		break;
	case OPCMD_ETC_CHECK6:
		m_pCkBtnEtc[3][0]->SetCheck();
		m_pCkBtnEtc[3][1]->SetCheck(LTFALSE);
		break;
	case OPCMD_ETC_CHECK7:
		m_pCkBtnEtc[3][0]->SetCheck(LTFALSE);
		m_pCkBtnEtc[3][1]->SetCheck();
		break;
	default:
		break;
	}


	return 1;
}

VOID
CPopScreenOption::OnFocus(LTBOOL bFocus)
{
	if( bFocus )
	{
		// 컨트롤List 최상위로 위치 이동 
		m_pScrollCtrl->SetPosYRatio(0.0f);
		m_pControlList->ScrollFromRatio( m_pScrollCtrl->GetPosYRatio()+0.01f );
		m_fDragRatio = m_pScrollCtrl->GetPosYRatio()+0.01f;

		OptionFrameSetting(OPCMD_DISPLAY);
		
		
		// Performance & Etc 체크박스 초기화 
		InitPerforAndEtcCheckButton();
	}
	else
	{
		m_pEBSensi->OnLostFocus();
//		ReturnOptionValue(); // 원상태로...
	}	
}

VOID
CPopScreenOption::OptionFrameSetting(uint8 iSelect)
{
	// ChectButton
	m_pTabBtnDisplay->SetCheck(LTFALSE);
	m_pTabBtnCtrl->SetCheck(LTFALSE);
	m_pTabBtnPerformance->SetCheck(LTFALSE);
	m_pTabBtnEtc->SetCheck(LTFALSE);
	// Frame
	m_pFmPerformance->Show(LTFALSE);
	m_pFmControl->Show(LTFALSE);
	m_pFmEtc->Show(LTFALSE);

	// List
	m_pResolutionList->Show(LTFALSE);
	m_pControlList->Show(LTFALSE);
	
	// Drag & Scroll
/*	if(m_iTotalIndex >= 7)
	{
		m_pBtnResolUp->Show(LTFALSE);
		m_pBtnResolDown->Show(LTFALSE);
		m_pScrollResol->Show(LTFALSE);
	}*/
	m_pBtnCtrlUp->Show(LTFALSE);
	m_pBtnCtrlDown->Show(LTFALSE);		
	m_pScrollCtrl->Show(LTFALSE);
	
	// Package
	m_pBtnSensiLow->Show(LTFALSE);
	m_pBtnSensiHigh->Show(LTFALSE);
	m_pDragSupSensi->Show(LTFALSE);
	m_pScrollSensi->Show(LTFALSE);
	m_pEBSensi->Show(LTFALSE);
	m_pFmGauge_Sensi->Show(LTFALSE);

	m_pBtnGammaLow->Show(LTFALSE);
	m_pBtnGammaHigh->Show(LTFALSE);
	m_pDragSupGamma->Show(LTFALSE);
	m_pScrollGamma->Show(LTFALSE);
	m_pEBGamma->Show(LTFALSE);
	m_pFmGauge_Gamma->Show(LTFALSE);

	m_pBtn_SndMusicLow->Show(LTFALSE);
	m_pBtn_SndMusicHigh->Show(LTFALSE);
	m_pDragSupSndMusic->Show(LTFALSE);
	m_pScroll_SndMusic->Show(LTFALSE);
	m_pEB_SndMusic->Show(LTFALSE);
	m_pFmGauge_SndMusic->Show(LTFALSE);

	m_pBtn_SndEffectLow->Show(LTFALSE);
	m_pBtn_SndEffectHigh->Show(LTFALSE);
	m_pDragSupSndEffect->Show(LTFALSE);
	m_pScroll_SndEffect->Show(LTFALSE);
	m_pEB_SndEffect->Show(LTFALSE);
	m_pFmGauge_SndEffect->Show(LTFALSE);
	
	//Resolution Check Button
	SetResolCkBtnVisible(LTFALSE);

	// Performance & Etc
	m_bPerforTextVisible = LTFALSE;
	m_bEtcTextVisible	 = LTFALSE;
	SetPerforCkBtnVisible(LTFALSE);
	SetEtcCkBtnVisible(LTFALSE);
	SetCrossHair_CkBtnVisible(LTFALSE);
	
	switch(iSelect) {
	case OPCMD_DISPLAY:
		// ChectButton
		m_pTabBtnDisplay->SetCheck();
		SetResolCkBtnVisible();	
		
		// List
	//	m_pResolutionList->Show(LTTRUE);

		// Drag & Scroll
/*		if(m_iTotalIndex >= 7)
		{
			m_pBtnResolUp->Show(LTTRUE);
			m_pBtnResolDown->Show(LTTRUE);
			m_pScrollResol->Show(LTTRUE);
		}*/
		// Package
		m_pBtnGammaLow->Show(LTTRUE);
		m_pBtnGammaHigh->Show(LTTRUE);
		m_pDragSupGamma->Show(LTTRUE);
		m_pScrollGamma->Show(LTTRUE);
		m_pEBGamma->Show(LTTRUE);
		m_pFmGauge_Gamma->Show(LTTRUE);
		
		m_pBtn_SndMusicLow->Show(LTTRUE);
		m_pBtn_SndMusicHigh->Show(LTTRUE);
		m_pDragSupSndMusic->Show(LTTRUE);
		m_pScroll_SndMusic->Show(LTTRUE);
		m_pEB_SndMusic->Show(LTTRUE);
		m_pFmGauge_SndMusic->Show(LTTRUE);
		
		m_pBtn_SndEffectLow->Show(LTTRUE);
		m_pBtn_SndEffectHigh->Show(LTTRUE);
		m_pDragSupSndEffect->Show(LTTRUE);
		m_pScroll_SndEffect->Show(LTTRUE);
		m_pEB_SndEffect->Show(LTTRUE);
		m_pFmGauge_SndEffect->Show(LTTRUE);

		
		break;
	case OPCMD_PERFORMANCE:
		m_pTabBtnPerformance->SetCheck();
		m_pFmPerformance->Show(LTTRUE);
		m_bPerforTextVisible = SetPerforCkBtnVisible(LTTRUE);
		break;
		
	case OPCMD_CTRL:
		m_pTabBtnCtrl->SetCheck();
		m_pFmControl->Show(LTTRUE);

		m_pControlList->Show(LTTRUE);

		m_pBtnCtrlUp->Show(LTTRUE);
		m_pBtnCtrlDown->Show(LTTRUE);		
		m_pScrollCtrl->Show(LTTRUE);

		m_pBtnSensiLow->Show(LTTRUE);
		m_pBtnSensiHigh->Show(LTTRUE);
		m_pDragSupSensi->Show(LTTRUE);
		m_pScrollSensi->Show(LTTRUE);
		m_pEBSensi->Show(LTTRUE);
		m_pFmGauge_Sensi->Show(LTTRUE);

		break;

	case OPCMD_ETC:
		m_pTabBtnEtc->SetCheck();
		m_pFmEtc->Show(LTTRUE);
		
		m_bEtcTextVisible = SetEtcCkBtnVisible(LTTRUE);
		
		SetCrossHair_CkBtnVisible();

		
		break;
	default:
		break;
	}
}


// -------------------------------------------------------------------------
// 해상도 ------------------------------------------------------------------
/*
640 * 480 * 16
800 * 600 * 16
1024 * 768 * 16

640 * 480 * 32
800 * 600 * 32
1024 * 768 * 32
*/

VOID	
CPopScreenOption::ResolutionValueSetting()
{
	// Build the list of render modes
    RMode *pRenderModes = g_pLTClient->GetRenderModes();

	// Iterate through the list of render modes adding each one to the array
	RMode *pCurrentMode=pRenderModes;

	char szTemp[256] = { LTNULL }; 
	
	//[MURSUM]=============================================================
	while (pCurrentMode != LTNULL )
	//=====================================================================
	{
		//[MURSUM]=============================================================
		//if (pCurrentMode->m_Width >= 640 && pCurrentMode->m_Height >= 480 && pCurrentMode->m_BitDepth == 32)
	//	if( (pCurrentMode->m_Width >= 640 && pCurrentMode->m_Height >= 480) &&
	//		(pCurrentMode->m_Width <= 1280 && pCurrentMode->m_Height <= 1024) )
		//=====================================================================

		if( (pCurrentMode->m_Width >= 640 && pCurrentMode->m_Height >= 480) &&
			(pCurrentMode->m_Width <= 1600 && pCurrentMode->m_Height <= 1200) )
		{
			// disallow non-standard aspect ratios
			uint32 testWidth = (pCurrentMode->m_Height * 4 / 3);
			if ( pCurrentMode->m_Width != testWidth && !(pCurrentMode->m_Width == 1280 && pCurrentMode->m_Height == 1024) )
			{
				// Go to the next render mode
				pCurrentMode=pCurrentMode->m_pNext;
				continue;
			}
			//disallow any that aren't hardware TnL

			//[MURSUM]======================================================
			/*
			if(!pCurrentMode->m_bHWTnL)
				continue;
			*/
			if(!pCurrentMode->m_bHWTnL)
			{
				pCurrentMode=pCurrentMode->m_pNext;
				continue;
			}
			//==============================================================

			// sort
			if(!SortSameResolution(*pCurrentMode) || !m_iTotalIndex)
			{
				// 임시배열에 우선 저장
				m_sTempResolution[m_iTotalIndex].m_bHWTnL = pCurrentMode->m_bHWTnL;
				SAFE_STRCPY(m_sTempResolution[m_iTotalIndex].m_description, pCurrentMode->m_Description);
				SAFE_STRCPY(m_sTempResolution[m_iTotalIndex].m_internalName, pCurrentMode->m_InternalName);
				m_sTempResolution[m_iTotalIndex].m_dwWidth		= pCurrentMode->m_Width;
				m_sTempResolution[m_iTotalIndex].m_dwHeight		= pCurrentMode->m_Height;
				m_sTempResolution[m_iTotalIndex].m_dwBitDepth	= pCurrentMode->m_BitDepth;
			
				m_iTotalIndex++;
				if(m_iTotalIndex > 20) m_iTotalIndex = 20;
			}
		}
		// Go to the next render mode
		pCurrentMode=pCurrentMode->m_pNext;
	}
	
	// 해상도값이 들어있는 배열을 순서대로 정렬 
	SortResolution();



	m_pResolutionList	= new CSaListResolution();
	LTIntPt ResolPos;
	ResolPos.x = 205;
	ResolPos.y = 93;
	int iTempWidth = 344;
//	if(m_iTotalIndex >= 7) iTempWidth = 326;
	m_pResolutionList->Create(ResolPos.x, ResolPos.y, iTempWidth, 108, 18, OPCMD_RESOLUTION);
	AddControl( m_pResolutionList, ResolPos );

	// Add		
	for(int i=0; i<m_iTotalIndex; i++)
	{
		// Add the display resolutions for this renderer
		ResolutionItem_Ptr pResolution = new Resolution_Item;
		pResolution->m_bHWTnL = m_sRealResolution[i].m_bHWTnL;
		SAFE_STRCPY(pResolution->m_description, m_sRealResolution[i].m_description);
		SAFE_STRCPY(pResolution->m_internalName, m_sRealResolution[i].m_internalName);
		pResolution->m_dwIndex		= i;
		pResolution->m_dwWidth		= m_sRealResolution[i].m_dwWidth;
		pResolution->m_dwHeight		= m_sRealResolution[i].m_dwHeight;
		pResolution->m_dwBitDepth	= m_sRealResolution[i].m_dwBitDepth;
		
		m_pResolutionList->AddResolution(pResolution);	
	}
	
	
	// Free the linked list of render modes
    g_pLTClient->RelinquishRenderModes(pRenderModes);
}

VOID
CPopScreenOption::SortResolution()
{
	uint8 nIndex = 0;
	LTBOOL bResolutionBit[6][2] = {LTNULL};
	while(nIndex < m_iTotalIndex)
	{	
		int	  iTranspositionIndex = -1;	

		if(m_sTempResolution[nIndex].m_dwWidth == 640 && m_sTempResolution[nIndex].m_dwHeight == 480)
		{
			if(m_sTempResolution[nIndex].m_dwBitDepth < 20)
			{
				iTranspositionIndex = 0;
				bResolutionBit[0][0] = LTTRUE;
			}
			else
			{
				iTranspositionIndex = 6;
				bResolutionBit[0][1] = LTTRUE;
			}
			if(	bResolutionBit[0][0] && bResolutionBit[0][1] )
				m_bExistResol[0] = LTTRUE;
		}
		else if( m_sTempResolution[nIndex].m_dwWidth == 800 && m_sTempResolution[nIndex].m_dwHeight == 600)
		{
			if(m_sTempResolution[nIndex].m_dwBitDepth < 20)
			{
				iTranspositionIndex = 1;
				bResolutionBit[1][0] = LTTRUE;
			}
			else
			{
				iTranspositionIndex = 7;
				bResolutionBit[1][1] = LTTRUE;
			}
			if(	bResolutionBit[1][0] && bResolutionBit[1][1] )
				m_bExistResol[1] = LTTRUE;
		}
		else if( m_sTempResolution[nIndex].m_dwWidth == 1024 && m_sTempResolution[nIndex].m_dwHeight == 768)
		{
			if(m_sTempResolution[nIndex].m_dwBitDepth < 20)
			{
				iTranspositionIndex = 2;
				bResolutionBit[2][0] = LTTRUE;
			}
			else
			{
				iTranspositionIndex = 8;
				bResolutionBit[2][1] = LTTRUE;
			}
			if(	bResolutionBit[2][0] && bResolutionBit[2][1] )
				m_bExistResol[2] = LTTRUE;
		}
		else if( m_sTempResolution[nIndex].m_dwWidth == 1152 && m_sTempResolution[nIndex].m_dwHeight == 864)
		{
			if(m_sTempResolution[nIndex].m_dwBitDepth < 20)
			{
				iTranspositionIndex = 3;
				bResolutionBit[3][0] = LTTRUE;
			}
			else
			{
				iTranspositionIndex = 9;
				bResolutionBit[3][1] = LTTRUE;
			}
			if(	bResolutionBit[3][0] && bResolutionBit[3][1] )
				m_bExistResol[3] = LTTRUE;
		}
		else if( m_sTempResolution[nIndex].m_dwWidth == 1280 && m_sTempResolution[nIndex].m_dwHeight == 1024)
		{
			if(m_sTempResolution[nIndex].m_dwBitDepth < 20)
			{
				iTranspositionIndex = 4;
				bResolutionBit[4][0] = LTTRUE;
			}
			else
			{
				iTranspositionIndex = 10;
				bResolutionBit[4][1] = LTTRUE;
			}
			if(	bResolutionBit[4][0] && bResolutionBit[4][1] )
				m_bExistResol[4] = LTTRUE;
		}
		else if( m_sTempResolution[nIndex].m_dwWidth == 1600 && m_sTempResolution[nIndex].m_dwHeight == 1200)
		{
			if(m_sTempResolution[nIndex].m_dwBitDepth < 20)
			{
				iTranspositionIndex = 5;
				bResolutionBit[5][0] = LTTRUE;
			}
			else
			{
				iTranspositionIndex = 11;
				bResolutionBit[5][1] = LTTRUE;
			}
			if(	bResolutionBit[5][0] && bResolutionBit[5][1] )
				m_bExistResol[5] = LTTRUE;
		}
		
		m_sRealResolution[iTranspositionIndex].m_dwWidth		=	m_sTempResolution[nIndex].m_dwWidth;
		m_sRealResolution[iTranspositionIndex].m_dwHeight		=	m_sTempResolution[nIndex].m_dwHeight;
		m_sRealResolution[iTranspositionIndex].m_dwBitDepth		=	m_sTempResolution[nIndex].m_dwBitDepth;
		m_sRealResolution[iTranspositionIndex].m_bHWTnL			=	m_sTempResolution[nIndex].m_bHWTnL;
		SAFE_STRCPY(m_sRealResolution[iTranspositionIndex].m_internalName, m_sTempResolution[nIndex].m_internalName);
		SAFE_STRCPY(m_sRealResolution[iTranspositionIndex].m_description, m_sTempResolution[nIndex].m_description);
		
		nIndex++;
	}

	// Real TotalIndex
	m_iTotalIndex = nIndex;
}
//-----------------------------------------------------------------------------------
// Control List Setting -------------------------------------------------------------
//-----------------------------------------------------------------------------------
VOID	
CPopScreenOption::CtrlTextSetting_All()
{
	m_pControlList	= new CSaListControlText();
	m_pControlList->m_bWaitingKey		= FALSE;
	m_pControlList->m_iIndex			= -1;
	m_pControlList->m_bSelect			= FALSE;
	LTIntPt ControlBasePos;
	ControlBasePos.x = 31;
	ControlBasePos.y = 100;
	m_pControlList->Create(ControlBasePos.x, ControlBasePos.y, 353, 224, 20, OPCMD_KEYSETTING );
	AddControl( m_pControlList, ControlBasePos );
	
	m_pSaveBindData = new CBindingData();

	InitControlList();

	m_pProfile->SetBindings();
	UpdateControlList(TRUE);
	// 초기 PC부팅후 처음 시작할때 간혹 Command키에 쓰레기값이 들어가는 경우가 있어서 강제로 한번더 Update!!!
	m_pProfile->SetBindings();
	UpdateControlList(TRUE);
}

VOID
CPopScreenOption::InitControlList()
{	

	//////////////////////////////////////////////////////////////////////////
	// Option KOR Msg - Novice

	const int ciOptionCmdStrBase = 194;
	const int nOptionCmdNum		 = 35;
	char	szTemp[128];

	int nArrOptionCmdStrMapping[nOptionCmdNum][2] = 	{
												{ ciOptionCmdStrBase + 0,	1284 },
												{ ciOptionCmdStrBase + 1,	1285 },
												{ ciOptionCmdStrBase + 2,	1289 },
												{ ciOptionCmdStrBase + 3,	1290 },
												{ ciOptionCmdStrBase + 4,	1286 },
												{ ciOptionCmdStrBase + 5,	1287 },
												{ ciOptionCmdStrBase + 6,	1300 },
												{ ciOptionCmdStrBase + 7,	1301 },
												{ ciOptionCmdStrBase + 8,	1291 },
												{ ciOptionCmdStrBase + 9,	1297 },
												{ ciOptionCmdStrBase + 10,	1292 },
												{ ciOptionCmdStrBase + 11,	1312 },
												{ ciOptionCmdStrBase + 12,	1295 },
												{ ciOptionCmdStrBase + 13,	1293 },
												{ ciOptionCmdStrBase + 14,	1296 },
												{ ciOptionCmdStrBase + 15,	1294 },
												{ ciOptionCmdStrBase + 16,	1329 },
												{ ciOptionCmdStrBase + 17,	1323 },
												{ ciOptionCmdStrBase + 44,	1288 },
												{ ciOptionCmdStrBase + 45,	1309 },
												{ ciOptionCmdStrBase + 46,	1305 },
												{ ciOptionCmdStrBase + 47,	1331 },
												{ ciOptionCmdStrBase + 48,	1330 },
												{ ciOptionCmdStrBase + 49,	1327 },
												{ ciOptionCmdStrBase + 18,	1334 },
												{ ciOptionCmdStrBase + 42,	1299 },
												{ ciOptionCmdStrBase + 43,	1298 },
												{ ciOptionCmdStrBase + 19,	1307 },
												{ ciOptionCmdStrBase + 20,	1393 },
												{ ciOptionCmdStrBase + 21,	1333 },
												{ ciOptionCmdStrBase + 22,	1302 },
												{ ciOptionCmdStrBase + 23,	1328 },
												{ ciOptionCmdStrBase + 24,	1304 },
												{ ciOptionCmdStrBase + 25,	1311 },
												{ ciOptionCmdStrBase + 26,	1310 }
	};

	//////////////////////////////////////////////////////////////////////////
	

	for (int iIndex=0; iIndex < g_kNumCommands; iIndex++)
	{
		const CommandData *pData = GetCommandData(iIndex);
		if(!pData) continue;
		//check to see that this is a TO2 command
		if (pData->nCommandID < FIRST_TRON_COMMAND)
		{
			ControlTextItem_Ptr pControlData = new ControlText_Item(); 
			pControlData->m_iIndex = iIndex;
			
			//////////////////////////////////////////////////////////////////////////
			// NOVICE

			if ( iIndex < nOptionCmdNum )
			{
				if ( nArrOptionCmdStrMapping[iIndex][1] == pData->nStringID )
				{
					lstrcpy( szTemp, NF_GetMsg( nArrOptionCmdStrMapping[iIndex][0] ) );
				}
			}
			//////////////////////////////////////////////////////////////////////////
			pControlData->Ctrl_Action.CreateD3DText( 512, 15, false );
			pControlData->Ctrl_Action.SetTransparent(true);
			pControlData->Ctrl_Action.DrawD3DText( szTemp, strlen(szTemp) );

			pControlData->Ctrl_Command.CreateD3DText( 512, 15, false );
			pControlData->Ctrl_Command.SetTransparent(true);
			//"[ 사용안함 ]"
			pControlData->Ctrl_Command.DrawD3DText( NF_GetMsg(SA_TEXT_OPTION_UNASSIGNED), 
												   strlen(NF_GetMsg(SA_TEXT_OPTION_UNASSIGNED)) );											   

			m_pControlList->AddControlText(pControlData);
		}
	}
}

VOID
CPopScreenOption::UpdateControlList(BOOL bOnce)
{
	for (int iIndex=0; iIndex < g_kNumCommands; iIndex++)
	{
		SetControlText(iIndex, bOnce);
	}
}

LTBOOL
CPopScreenOption::Test_SameText(int iIndex)
{	
	char szTemp1[64] = {LTNULL};
	char szTemp2[64] = {LTNULL};
	strcpy(szTemp2, GetCommandItem(iIndex));

	for (int i=0; i < g_kNumCommands; i++)
	{
		if(i != iIndex)
		{
			strcpy(szTemp1, GetCommandItem(i));
			if(stricmp(szTemp1, szTemp2) == 0)
			{
				SetSelectCtrlListSetting(i, m_pControlList->m_szSaveText);
				
				CBindingData* pData = m_pProfile->FindBinding(i);
				if(pData != LTNULL)	 // 추가수정 필요 
				{
					int8 nIndex = 0;
					while(nIndex < 2) 
					{
						//  " " <== 이문자(빈공간)는 인정하지 않음.
						if( (strlen(m_pSaveBindData->strRealName[nIndex]) != LTNULL || strlen(m_pSaveBindData->strTriggerName[nIndex]) != LTNULL)
							&& (stricmp(m_pSaveBindData->strTriggerName[nIndex]," ") != 0 && stricmp(m_pSaveBindData->strRealName[nIndex]," ") != 0) )
						{
							strcpy(pData->strRealName[nIndex], m_pSaveBindData->strRealName[nIndex]);
							strcpy(pData->strTriggerName[nIndex], m_pSaveBindData->strTriggerName[nIndex]);
							if(nIndex == 0)
							{
								strcpy(pData->strRealName[1], "");
								strcpy(pData->strTriggerName[1], "");
							}
							else
							{
								strcpy(pData->strRealName[0], "");
								strcpy(pData->strTriggerName[0], "");
							}
						}
						nIndex++;
					}
					
				}
				return LTTRUE;
			}
		}
	}
	return LTFALSE;
}

VOID
CPopScreenOption::SetControlText(int iIndex, BOOL bOnce )
{	
	CBindingData* pData = m_pProfile->FindBinding(iIndex);
	if(!pData) return;
	char strControls[256] = "";

	if (pData) 
	{
		if(!m_bKeyBoardEvent && (strlen(pData->strTriggerName[1]) != 0))// && strlen(pData->strTriggerName[0]) == 0)
		{
			if (stricmp("#U",pData->strRealName[1]) == 0)
				strcpy(strControls,LoadTempString(IDS_WHEEL_UP));
			else if (stricmp("#D",pData->strRealName[1]) == 0)
				strcpy(strControls,LoadTempString(IDS_WHEEL_DOWN));
			else if (stricmp("##3",pData->strRealName[1]) == 0)
				strcpy(strControls,LoadTempString(IDS_MOUSE_LEFTBUTTON));
			else if (stricmp("##4",pData->strRealName[1]) == 0)
				strcpy(strControls,LoadTempString(IDS_MOUSE_RIGHTBUTTON));
			else if (stricmp("##5",pData->strRealName[1]) == 0)
				strcpy(strControls,LoadTempString(IDS_MOUSE_MIDDLEBUTTON));
			else
			{
			}
			strcpy(pData->strTriggerName[0], "");
			strcpy(pData->strRealName[0], "");
		}
		else//if(strlen(pData->strTriggerName[0]) != 0 )
		{ 
			SAFE_STRCPY(strControls,pData->strTriggerName[0]);
			strcpy(pData->strTriggerName[1], "");
			strcpy(pData->strRealName[1], "");
		}
	}

	char szTmp[64];
	// If the key is unassigned, then just set the text to the unassigned message
	if (strlen(strControls) == 0 )
	{		
		//"[사용 안함]"
		strcpy( szTmp, NF_GetMsg(SA_TEXT_OPTION_UNASSIGNED) );
		SetSelectCtrlListSetting(iIndex, szTmp);
	}
	else
	{
		if(!bOnce)
		{
			m_pControlList->SetSelectIndex(-1);  // ListSelect 초기화 
			m_pControlList->m_bWaitingKey = FALSE;	
			m_bKeyBoardEvent = FALSE; 	
			m_fInputPauseTimeLeft = OPT_INPUT_PAUSE_TIME_LEFT;
			memset( m_pInputArray_KB, 0, CTRL_MAXLINE );
			g_pLTClient->EndDeviceTrack();
						
			// Set the text in the control
			SetSelectCtrlListSetting(iIndex, strControls);
			Test_SameText(iIndex);  // SameText 검사

			g_pInterfaceMgr->RequestInterfaceSound( (InterfaceSound)IS_CLICK_NORMAL );	
		}
		else
		{
			// Set the text in the control
			SetSelectCtrlListSetting(iIndex, strControls);
		}
	}
}




// Unbinds an action
VOID
CPopScreenOption::UnBind( uint32 nObjectId, char const* pszControlName, uint32 deviceType)
{
	if( !nObjectId && !pszControlName )
	{
		ASSERT( !"CScreenConfigure::UnBind: Invalid inputs." );
		return;
	}

	int dev = 0;
	while (dev < 3 && devices[dev] != deviceType)
		++dev;

	char szTmp[16] ="";
	for (int i = 0; i < g_kNumCommands; i++)
	{
		CBindingData *pData = m_pProfile->FindBinding(i);
		if(pData != LTNULL)
		{
			if( nObjectId )
			{
				if( pData->nDeviceObjectId[dev] != nObjectId )
				{
					continue;
				}
			}
			else
			{
				if( stricmp( pData->strRealName[dev], pszControlName ))
					continue;
			}
			
			strcpy(pData->strRealName[dev],"");
			strcpy(pData->strTriggerName[dev],"");
			pData->nDeviceObjectId[dev] = 0;
		}
		else
			int a = i;

	}
}
// Binds a key to a action
VOID
CPopScreenOption::Bind(int nCommand, uint16 diCode, char *lpszControlName, uint32 deviceType)
{
	_ASSERT(lpszControlName || diCode);

	int dev = 0;
	while (dev < 3 && devices[dev] != deviceType)
		++dev;


	CBindingData *pData = m_pProfile->FindBinding(nCommand);
	if (pData)
	{
		if (diCode && deviceType != DEVICETYPE_JOYSTICK)
		{
			sprintf(pData->strRealName[dev],"##%d",diCode);
			SAFE_STRCPY(pData->strTriggerName[dev],lpszControlName)
		}
		else
		{
			SAFE_STRCPY(pData->strRealName[dev],lpszControlName)
			SAFE_STRCPY(pData->strTriggerName[dev],lpszControlName)
		}
	}
}

// 원래는 키보드버튼 처리 함수 
LTBOOL 
CPopScreenOption::KeyRemappable (DeviceInput* pInput)
{	
	if (!g_pLTClient) return LTFALSE;

	uint16 nDIK = pInput->m_ControlCode;
	if (nDIK == DIK_ESCAPE)
        return LTFALSE;
	if (nDIK == DIK_PAUSE)
        return LTFALSE;
	if (nDIK >= DIK_F1 && nDIK <= DIK_F10)
        return LTFALSE;
	if (nDIK >= DIK_F11 && nDIK <= DIK_F15)
        return LTFALSE;

    DeviceBinding* pBindings = g_pLTClient->GetDeviceBindings (DEVICETYPE_KEYBOARD);
    if (!pBindings) return LTTRUE;

	// see if this input object is already bound to something...

	DeviceBinding* ptr = pBindings;
	while (ptr)
	{
		if( ptr->m_nObjectId == pInput->m_nObjectId )
		{
			// see if this binding is in the list of mappable ones
			GameAction* pAction = ptr->pActionHead;
			while (pAction)
			{
                LTBOOL bFound = LTFALSE;
				for (int i = 0; i < g_kNumCommands; i++)
				{
					const CommandData *pData = GetCommandData(i);
					if (pData->nCommandID == pAction->nActionCode)
					{
                        bFound = LTTRUE;
						break;
					}
				}

				if (!bFound)
				{
					// this key is already bound to something but is not remappable
                    g_pLTClient->FreeDeviceBindings (pBindings);
					return LTFALSE;
				}

				pAction = pAction->pNext;
			}

			// binding must already exist in the folders...
			break;
		}
		ptr = ptr->pNext;
	}

	// either the binding exists in the folders or this key is not currently bound...therefore it's remappable

    g_pLTClient->FreeDeviceBindings (pBindings);

	return LTTRUE;
}

VOID	
CPopScreenOption::ControlRender()
{
	if( m_pControlList->m_bWaitingKey ) m_fInputPauseTimeLeft -= g_pGameClientShell->GetFrameTime();
	
	if(m_pControlList->m_bWaitingKey && 0.0f > m_fInputPauseTimeLeft)
	{
		m_fInputPauseTimeLeft = OPT_INPUT_PAUSE_TIME_LEFT;   // 딜레이 시간 

		uint32 nArraySize = 9;
        
		if(m_bKeyBoardEvent && !g_bEsc && !g_bDel)  // 키보드 이벤트 
		{
			memcpy(m_pInputArray, m_pInputArray_KB, sizeof(m_pInputArray_KB));
			nArraySize = m_iArraySize_KB;
		}
		else 
			g_pLTClient->TrackDevice (m_pInputArray, &nArraySize);
		
		for (uint32 i = 0; i < nArraySize; i++)
		{
			// 값이 있으나 길이가 20이 넘지 않게(쓰레기값이 들어간 경우 무효)
			if (m_pInputArray[i].m_InputValue && strlen(m_pInputArray[i].m_ControlName) < 20) 
			{
				SetCurrentSelection(&m_pInputArray[i], m_pControlList->m_iListVectorIndex);
			}
		}
	}
}
LTBOOL 
CPopScreenOption::SetCurrentSelection (DeviceInput* pInput, int iListIndex)
{		
	CBindingData* pTempData = m_pProfile->FindBinding(iListIndex);
	if(pTempData)
		memcpy(m_pSaveBindData, pTempData, sizeof(CBindingData));
	
	// 마우스 휠~
	if (pInput->m_DeviceType == DEVICETYPE_MOUSE && pInput->m_ControlType == CONTROLTYPE_ZAXIS)
	{
		m_pControlList->m_bWaitingKey = FALSE;
		return CheckMouseWheel(pInput, iListIndex);
	}

	if (pInput->m_ControlType != CONTROLTYPE_BUTTON &&
		pInput->m_ControlType != CONTROLTYPE_KEY)
        return LTFALSE;

	// see if this key is bound to something not in the keyboard configuration folder...
	if (KeyRemappable(pInput))
	{
		uint16 diCode = pInput->m_ControlCode;
		UnBind(diCode, pInput->m_ControlName, pInput->m_DeviceType);
		Bind(iListIndex, diCode, pInput->m_ControlName, pInput->m_DeviceType);
	}
	else // 마우스버튼 3개	if(pInput->m_DeviceType != DEVICETYPE_KEYBOARD) 
	{
		uint16 diCode = pInput->m_ControlCode;
		UnBind(diCode, pInput->m_ControlName, pInput->m_DeviceType);
		Bind(iListIndex, diCode, pInput->m_ControlName, pInput->m_DeviceType);
	}	

	SetControlText(iListIndex);

    return LTTRUE;
}

LTBOOL 
CPopScreenOption::CheckMouseWheel (DeviceInput* pInput, int iIndex)
{
    if (!g_pLTClient) return LTFALSE;

	if (pInput->m_DeviceType != DEVICETYPE_MOUSE || pInput->m_ControlType != CONTROLTYPE_ZAXIS)
		return LTFALSE;

	LTBOOL bWheelUp = ((int)pInput->m_InputValue > 0);
	
	char szCommand[64];
	uint16 diCode = pInput->m_ControlCode;
	
	if (bWheelUp)
		strcpy(szCommand, "#U");
	else
		strcpy(szCommand, "#D");

	UnBind(0, szCommand, pInput->m_DeviceType);
	Bind(iIndex, 0, szCommand, pInput->m_DeviceType);

	SetControlText(iIndex);

    return LTTRUE;
}

LTBOOL
CPopScreenOption::InitResolutionSelect()
{
	int iIndex = 0;
	RMode currentMode;
	g_pLTClient->GetRenderMode(&currentMode);
	currentMode.m_Width    = m_pProfile->m_nScreenWidth;
	currentMode.m_Height   = m_pProfile->m_nScreenHeight;
	currentMode.m_BitDepth = m_pProfile->m_nScreenDepth;

	while(iIndex < m_iTotalIndex )
	{
		if (IsRendererEqual(&currentMode, &GetCurrentResolMode(iIndex)) )
		{
			m_pResolutionList->SetSelectIndex(iIndex);

			if(currentMode.m_BitDepth < 20) 
			{
				m_pCkBtnResolBit[0]->SetCheck();
				m_pCkBtnResolBit[1]->SetCheck(LTFALSE);
			}
			else
			{
				m_pCkBtnResolBit[1]->SetCheck();
				m_pCkBtnResolBit[0]->SetCheck(LTFALSE);
				uint nCount = 0;
				for(int i=0; i<6; i++)
				{
					if(!m_bExistResol[i]) nCount++;
				}
				iIndex -= (6 - nCount);
			}
			
			ResetResolCheckButton(iIndex);
			return LTTRUE;
		}
		iIndex++;
	}
	return LTFALSE;
}
VOID
CPopScreenOption::ResetResolCheckButton(int nSelect)
{
	for(int i=0; i<6; i++)
	{
		if(m_bExistResol[i])
		{
			if(i == nSelect)
				m_pCkBtnResol[i]->SetCheck();
			else m_pCkBtnResol[i]->SetCheck(LTFALSE);
		}
	}
}


VOID
CPopScreenOption::ResetSoundCheckButton()
{
	if(!GetConsoleInt("MusicEnable", 1))
	{
		m_pCkBtnSound_OnOff[0]->SetCheck(LTTRUE);
		m_pCkBtnSound_OnOff[1]->SetCheck(LTFALSE);
		m_pFmSoundDisable[0]->Show(LTTRUE);
		g_pInterfaceMgr->KillBGM();
	}
	else
	{
		m_pCkBtnSound_OnOff[0]->SetCheck(LTFALSE);
		m_pCkBtnSound_OnOff[1]->SetCheck(LTTRUE);
		m_pFmSoundDisable[0]->Show(LTFALSE);
		//BGM
		if( LTFALSE == g_pInterfaceMgr->IsPlayBGM() )
		{
			g_pInterfaceMgr->PlayBGM( "SA_INTERFACE\\SND\\BG\\Screen.wav" );
			m_pProfile->ReSetBGMVolumeRatio(((float)m_pEB_SndMusic->GetInt())/100);
		}
	}

	if(!GetConsoleInt("SoundEnable", 1))
	{
		m_pCkBtnSound_OnOff[2]->SetCheck(LTTRUE);
		m_pCkBtnSound_OnOff[3]->SetCheck(LTFALSE);
		m_pFmSoundDisable[1]->Show(LTTRUE);
	}
	else
	{
		m_pCkBtnSound_OnOff[2]->SetCheck(LTFALSE);
		m_pCkBtnSound_OnOff[3]->SetCheck(LTTRUE);
		m_pFmSoundDisable[1]->Show(LTFALSE);
	}
}

VOID
CPopScreenOption::ResetCrossHair_CkButton(int nSelect)
{
	for(int i=0; i<4; i++)
	{
		if(i == nSelect)
		{
			m_pCkBtnCrossHair[i]->SetCheck();
			m_nCrossHair_CurrentIndex = nSelect;
		}
		else m_pCkBtnCrossHair[i]->SetCheck(LTFALSE);
	}

	// 선택한 CrossHair 이미지 
	char *szTag = "HUDCrosshair";
	char szAtt[32], szTempStyle[64]={LTNULL};
	sprintf(szAtt, "Crosshair%d", nSelect);  // 스타일에 맞게 주소 세팅 
	strcat(szAtt, "0");
	char szTexStyle[256];
	g_pLayoutMgr->GetString(szTag,szAtt,szTexStyle,sizeof(szTexStyle));
	m_pFmCrossHair->SetFrame(g_pInterfaceResMgr->GetTexture(szTexStyle));
}

VOID
CPopScreenOption::SetCrossHair_CkBtnVisible(LTBOOL vVisible)
{
	for(int i=0; i<4; i++)
		m_pCkBtnCrossHair[i]->Show(vVisible);
	
	m_pFmCrossHair->Show(vVisible);
}

VOID
CPopScreenOption::InitPerforAndEtcCheckButton()
{
	if(!m_pProfile) return;
	// Performance
	m_pCkBtnPerformance[0][0]->SetCheck(m_pProfile->m_bModelShadow);
	m_pCkBtnPerformance[0][1]->SetCheck(!m_pProfile->m_bModelShadow);

	m_pCkBtnPerformance[1][0]->SetCheck(m_pProfile->m_bWeather);
	m_pCkBtnPerformance[1][1]->SetCheck(!m_pProfile->m_bWeather);

	m_pCkBtnPerformance[2][0]->SetCheck(m_pProfile->m_bTracer);
	m_pCkBtnPerformance[2][1]->SetCheck(!m_pProfile->m_bTracer);

	m_pCkBtnPerformance[3][0]->SetCheck(m_pProfile->m_bShellCasing);
	m_pCkBtnPerformance[3][1]->SetCheck(!m_pProfile->m_bShellCasing);

	m_pCkBtnPerformance[4][0]->SetCheck(m_pProfile->m_bVSyncOnFlip);
	m_pCkBtnPerformance[4][1]->SetCheck(!m_pProfile->m_bVSyncOnFlip);

	m_pCkBtnPerformance[5][0]->SetCheck(m_pProfile->m_bParticle);
	m_pCkBtnPerformance[5][1]->SetCheck(!m_pProfile->m_bParticle);

	// Johnson Li 2007-07-24 청唐뚤壇栗都
	//m_pCkBtnPerformance[6][0]->SetCheck(m_pProfile->m_bDynaLight);
	//m_pCkBtnPerformance[6][1]->SetCheck(!m_pProfile->m_bDynaLight);
	
	// 15세는 일단 피효과라는 걸 빼자!!
	if(g_GameDoc.IsLowViolence())
	{
//		m_pProfile->m_bGore = 0;
	}
	//===========================================
	// Etc
	m_pProfile->m_bNameRadar = GetConsoleInt("NameRadar", 1);
	m_pCkBtnEtc[0][0]->SetCheck(m_pProfile->m_bNameRadar);
	m_pCkBtnEtc[0][1]->SetCheck(!m_pProfile->m_bNameRadar);
	m_pProfile->m_bRejectWhisper = GetConsoleInt("RejectWhisper", 1);
	m_pCkBtnEtc[1][0]->SetCheck(m_pProfile->m_bRejectWhisper);
	m_pCkBtnEtc[1][1]->SetCheck(!m_pProfile->m_bRejectWhisper);
	m_pProfile->m_bRejectInvite = GetConsoleInt("RejectInvite", 1);
	m_pCkBtnEtc[2][0]->SetCheck(m_pProfile->m_bRejectInvite);
	m_pCkBtnEtc[2][1]->SetCheck(!m_pProfile->m_bRejectInvite);

	m_pCkBtnEtc[3][0]->SetCheck(m_pProfile->m_bGore);
	m_pCkBtnEtc[3][1]->SetCheck(!m_pProfile->m_bGore);
}
VOID
CPopScreenOption::ApplyPerforAndEtc()
{
	// Performance
	m_pProfile->m_bModelShadow	= m_pCkBtnPerformance[0][0]->GetCheck() ? LTTRUE : LTFALSE;
	m_pProfile->m_bWeather		= m_pCkBtnPerformance[1][0]->GetCheck() ? LTTRUE : LTFALSE;
	m_pProfile->m_bTracer		= m_pCkBtnPerformance[2][0]->GetCheck() ? LTTRUE : LTFALSE;
	m_pProfile->m_bShellCasing	= m_pCkBtnPerformance[3][0]->GetCheck() ? LTTRUE : LTFALSE;
	m_pProfile->m_bVSyncOnFlip	= m_pCkBtnPerformance[4][0]->GetCheck() ? LTTRUE : LTFALSE;
	m_pProfile->m_bParticle		= m_pCkBtnPerformance[5][0]->GetCheck() ? LTTRUE : LTFALSE;
	m_pProfile->m_bDynaLight	= m_pCkBtnPerformance[6][0]->GetCheck() ? LTTRUE : LTFALSE;
	
	// Etc
	m_pProfile->m_bNameRadar	 = m_pCkBtnEtc[0][0]->GetCheck() ? LTTRUE : LTFALSE;
	m_pProfile->m_bRejectWhisper = m_pCkBtnEtc[1][0]->GetCheck() ? LTTRUE : LTFALSE;
	m_pProfile->m_bRejectInvite	 = m_pCkBtnEtc[2][0]->GetCheck() ? LTTRUE : LTFALSE;
	m_pProfile->m_bGore		 	 = m_pCkBtnEtc[3][0]->GetCheck() ? LTTRUE : LTFALSE;

	m_pProfile->m_nStyle		= m_nCrossHair_CurrentIndex;
	g_pCrosshair->SetStyle(m_nCrossHair_CurrentIndex);
}
VOID
CPopScreenOption::SetResolCkBtnVisible(LTBOOL bVisible)
{
	for(int i=0; i<6; i++)
	{
		if(m_bExistResol[i]) // 지원되는 해상도만 Show~!!!
		{
			m_pCkBtnResol[i]->Show(bVisible);
		}
		else m_pCkBtnResol[i]->Show(LTFALSE);
		
		if(i<2)	// 16 & 32 Bit
		{
			m_pCkBtnResolBit[i]->Show(bVisible);
		}
		if(i<4)	// 사운드 
		{
			m_pCkBtnSound_OnOff[i]->Show(bVisible);
		}		
	}

	if(!bVisible)
	{
		m_pFmSoundDisable[0]->Show(LTFALSE);
		m_pFmSoundDisable[1]->Show(LTFALSE);
	}
	else
	{
		if(m_pCkBtnSound_OnOff[0]->GetCheck())
		{
			m_pFmSoundDisable[0]->Show(LTTRUE);
		}
		if(m_pCkBtnSound_OnOff[2]->GetCheck())
		{
			m_pFmSoundDisable[1]->Show(LTTRUE);
		}
	}
}

LTBOOL
CPopScreenOption::SetPerforCkBtnVisible(LTBOOL bVisible)
{
	for(int i=0; i<m_iPerformanceTotalCnt; i++)
	{
		for(int j=0; j<2; j++)
		{	
			m_pCkBtnPerformance[i][j]->Show(bVisible);
		}
	}
	return bVisible;
}
LTBOOL
CPopScreenOption::SetEtcCkBtnVisible(LTBOOL bVisible)
{
	for(int i=0; i<m_iEtcTotalCnt; i++)
	{
		for(int j=0; j<2; j++)
		{	
			m_pCkBtnEtc[i][j]->Show(bVisible);
		}
	}
	return bVisible;
}

LTBOOL
CPopScreenOption::SortSameResolution(RMode pCurrentMode)
{
	int iIndex = 0;
	
	while(iIndex < m_iTotalIndex)
	{
		RMode mode;
		mode.m_Width		=	m_sTempResolution[iIndex].m_dwWidth;
		mode.m_Height		=	m_sTempResolution[iIndex].m_dwHeight;
		mode.m_BitDepth		=	m_sTempResolution[iIndex].m_dwBitDepth;
		mode.m_bHWTnL		=	m_sTempResolution[iIndex].m_bHWTnL;
		SAFE_STRCPY(mode.m_InternalName, m_sTempResolution[iIndex].m_internalName);
		SAFE_STRCPY(mode.m_Description, m_sTempResolution[iIndex].m_description);

		mode.m_pNext		=	LTNULL;
		if (IsRendererEqual(&pCurrentMode, &mode) )
		{
			return LTTRUE;
		}
		iIndex++;
	}
	return LTFALSE;
}

RMode 
CPopScreenOption::GetCurrentResolMode(int iIndex)
{
	ResolutionItem_Ptr	pResolution;
	RMode mode;

	pResolution = m_pResolutionList->GetResolutionItem(iIndex);
	
	if( pResolution )
	{	
		mode.m_Width		=	pResolution->m_dwWidth;
		mode.m_Height		=	pResolution->m_dwHeight;
		mode.m_BitDepth		=	pResolution->m_dwBitDepth;
		mode.m_bHWTnL		=	pResolution->m_bHWTnL;
		SAFE_STRCPY(mode.m_InternalName, pResolution->m_internalName);
		SAFE_STRCPY(mode.m_Description, pResolution->m_description);
		mode.m_pNext		=	LTNULL;
	}
	else
	{
		//[MURSUM] Default..
		mode.m_Width		=	USABLE_WIDTH_I;
		mode.m_Height		=	USABLE_HEIGHT_I;
		mode.m_BitDepth		=	USABLE_BPP_I;
		mode.m_bHWTnL		=	true;
		SAFE_STRCPY(mode.m_InternalName, "none");
		SAFE_STRCPY(mode.m_Description, "none");
		mode.m_pNext		=	LTNULL;
	}
	
	return mode;
}


VOID 
CPopScreenOption::SetSelectCtrlListSetting(int iIndex, char* szCommand, char* szAction)
{
	ControlTextItem_Ptr pTempCtrlText = LTNULL;
	if(pTempCtrlText = m_pControlList->GetSelectedKeySettingData(iIndex))
	{
		pTempCtrlText->Ctrl_Command.DrawD3DText( szCommand, strlen(szCommand) );
	}
}

const char*
CPopScreenOption::GetCommandItem(int iIndex)
{
	if(iIndex >= g_kNumCommands) return LTFALSE;

	ControlTextItem_Ptr pTempCtrlText = LTNULL;
	if(pTempCtrlText = m_pControlList->GetSelectedKeySettingData(iIndex))
		return pTempCtrlText->Ctrl_Command.GetText();
	else
		return LTFALSE;
}


// Save
VOID
CPopScreenOption::ApplyOptionSetting()
{
	// Keyboard Setting
	m_pProfile->ApplyBindings();
	
	// Mouse Setting
	m_pProfile->m_nSensitivity = m_pEBSensi->GetInt();
	m_pProfile->ApplyMouse();

	// Sound Setting
	m_pProfile->m_nMusicVolume = m_pEB_SndMusic->GetInt();  // BGM
	m_pProfile->m_nSoundVolume = m_pEB_SndEffect->GetInt();  // Effect
	m_pProfile->ApplySound();

	// Performance & Etc
	/** TODO ROBERT - EDITED based on: https://forum.ragezone.com/f841/release-fix-option-save-resolution-1090930/
	* ApplyPerforAndEtc();
	* this->ApplyResolutionSetting();	// Gamma 포함 
	** replaced with the following 3 lines.
	**/
	m_pProfile->ApplyResolution();
	this->ApplyResolutionSetting();
	m_pProfile->ApplyDisplay();

	m_pProfile->Save(TRUE);
}

VOID
CPopScreenOption::ApplyResolutionSetting()
{
// Display Value Setting
	RMode newMode=GetCurrentResolMode(-1);		// 현재 선택된 리스트 정보 
	m_pProfile->m_nScreenWidth	= newMode.m_Width;
	m_pProfile->m_nScreenHeight = newMode.m_Height;
	m_pProfile->m_nScreenDepth	= newMode.m_BitDepth;
	m_pProfile->ApplyDisplay();	// Gamma , Performance, Etc  포함 
}


// 원래 세팅값으로 돌아감  
VOID
CPopScreenOption::ReturnOptionValue(uint8 flags)
{	
	char szTempEditCount[128] = {LTNULL};
	float fGamma = 1.1f;   // Default Gamma
	// 아예 초기화 (gamma포함)
	if(flags)
	{
		m_pProfile->RestoreDefaults(flags);
		WriteConsoleFloat("GammaR", fGamma);
		WriteConsoleFloat("GammaG", fGamma);
		WriteConsoleFloat("GammaB", fGamma);
	}
	else
		fGamma = (GetConsoleFloat("GammaR",1.0f) + GetConsoleFloat("GammaG",1.0f) + GetConsoleFloat("GammaB",1.0f)) / 3.0f;

	m_pProfile->SetBindings();
	UpdateControlList(TRUE);
	
	m_pProfile->SetDisplay();
	
	if(!InitResolutionSelect())
	{	
		// 검색해도 해상도가 안나오면 가장 기초적인 해상도 적용 !!!
		m_pProfile->m_nScreenWidth	= 800;
		m_pProfile->m_nScreenHeight = 600;
		m_pProfile->m_nScreenDepth	= 16;
		m_pProfile->ApplyDisplay();	// w
		InitResolutionSelect();
	}
	
	sprintf(szTempEditCount, "%d", (int)((1.0f-(1.8f - fGamma))*100));
	m_pEBGamma->SetText(szTempEditCount);
	m_pScrollGamma->SetPosXRatio(((float)m_pEBGamma->GetInt())/100);
	m_pFmGauge_Gamma->SetSize(((m_pScrollGamma->GetPos().x+OFFSET_POS)-m_pFmGauge_Gamma->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
	g_pGameClientShell->SetGammaRamp( fGamma );
	
	m_pProfile->SetMouse();
	
	sprintf(szTempEditCount, "%d", m_pProfile->m_nSensitivity);
	m_pEBSensi->SetText(szTempEditCount);
	m_pScrollSensi->SetPosXRatio(((float)m_pEBSensi->GetInt())/100);
	m_pFmGauge_Sensi->SetSize(((m_pScrollSensi->GetPos().x+OFFSET_POS)-m_pFmGauge_Sensi->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);

	m_pProfile->SetSound();
	sprintf(szTempEditCount, "%d", m_pProfile->m_nMusicVolume);
	m_pEB_SndMusic->SetText(szTempEditCount);
	m_pScroll_SndMusic->SetPosXRatio(((float)m_pEB_SndMusic->GetInt())/100);
	m_pFmGauge_SndMusic->SetSize(((m_pScroll_SndMusic->GetPos().x+OFFSET_POS)-m_pFmGauge_SndMusic->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);
	sprintf(szTempEditCount, "%d", m_pProfile->m_nSoundVolume);
	m_pEB_SndEffect->SetText(szTempEditCount);
	m_pScroll_SndEffect->SetPosXRatio(((float)m_pEB_SndEffect->GetInt())/100);
	m_pFmGauge_SndEffect->SetSize(((m_pScroll_SndEffect->GetPos().x+OFFSET_POS)-m_pFmGauge_SndEffect->GetBasePos().x+OFFSET_BASEPOS), OPT_GAUGE_H);

	
	ResetSoundCheckButton();

	// Performance & Etc
	InitPerforAndEtcCheckButton();
	ResetCrossHair_CkButton(m_pProfile->m_nStyle);

	m_pProfile->Save(TRUE);
}
