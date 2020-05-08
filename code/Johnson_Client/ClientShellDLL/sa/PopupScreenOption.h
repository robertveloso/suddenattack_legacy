#ifndef _POPUP_SCREEN_OPTION_H_
#define _POPUP_SCREEN_OPTION_H_

#include "ScreenBase.h"
#include "..\\..\\Libs\\LTGUIMgr\\ltguiwindow.h"
#include "../ClientShellShared/HanText.h"
#include "PopupScreenBase.h"
#include "LTGUIMgr.h"
#include "PopupOption_Ex.h"
#include "BaseScreen.h"

class CPopScreenOption : public CPopScreenBase	
{
public :
	CPopScreenOption();
	virtual ~CPopScreenOption();

	VOID	Init( CLTGUICommandHandler* pCommandHandler );
	VOID	Render();
	VOID	OnFocus(LTBOOL bFocus);

	virtual LTBOOL OnMouseMove(int x, int y);
	virtual LTBOOL OnLButtonUp(int x, int y);
	virtual LTBOOL OnLButtonDown(int x, int y);
	virtual LTBOOL OnWheelUp(int x, int y);
	virtual LTBOOL OnWheelDown(int x, int y);

	VOID	ApplyOptionSetting();  // Profile Setting
	VOID	ReturnOptionValue(uint8 flags = NULL);

	// 인덱스에 해당한 '컨트롤 키' 리턴 
	const char* GetCommandItem(int iIndex);
	
	LTBOOL   HandleKeyDown(int key, int rep);

	CLTGUIEditBox* GetSndMusicEditBox(){ return m_pEB_SndMusic; }

	
	// HUD에서도 Sensitivity를 조절하기 위하여 Public으로 뺌
	// Sensi Package 
	CLTGUIButton*			m_pBtnSensiLow;
	CLTGUIButton*			m_pBtnSensiHigh;
	CDragSupportButton*		m_pDragSupSensi;
	CLTGUIDragButton*		m_pScrollSensi;
	CLTGUIEditBox*			m_pEBSensi; //EditBox
	CLTGUIFrame*			m_pFmGauge_Sensi;

protected:
	LTBOOL	m_bMouseHere;
    uint32  OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
	VOID	OptionFrameSetting(uint8 iSelect);
	VOID    OptionLowLevel(uint32 dwCommand );
	LTBOOL	OnEnter();
	LTBOOL	OnEscape();
// Resolution Setting -----------------------------------
	CSaListResolution*		m_pResolutionList;
	Resolution_Item			m_sTempResolution[15];
	Resolution_Item			m_sRealResolution[15];
	uint8	m_iTotalIndex;
	VOID	ResolutionValueSetting();
	LTBOOL	InitResolutionSelect();
	RMode	GetCurrentResolMode(int iIndex);
	LTBOOL	SortSameResolution(RMode pCurrentMode);
	VOID	SortResolution();	// 해상도 정렬 
	VOID	ApplyResolutionSetting();  // 해상도 
	
	// Check Button
	CLTGUIButton*			m_pCkBtnResol[6];
	CLTGUIButton*			m_pCkBtnResolBit[2];
	CLTGUIButton*			m_pCkBtnSound_OnOff[4];

	LTBOOL					m_bExistResol[6];
	CLTGUIButton*			m_pFmSoundDisable[2];
	VOID	ResetSoundCheckButton();
	VOID	ResetResolCheckButton(int nSelect = -1);
	VOID	SetResolCkBtnVisible(LTBOOL bVisible = LTTRUE);
	

//-------------------------------------------------------
// Control Setting ---------------------
	CSaListControlText*		m_pControlList;	
	LTFLOAT					m_fInputPauseTimeLeft;
	DeviceInput				m_pInputArray[CTRL_MAXLINE];
	DeviceInput				m_pInputArray_KB[CTRL_MAXLINE];
	uint32					m_iArraySize_KB;
	LTBOOL					m_bWaitingForKey, m_bKeyBoardEvent;
	CBindingData*			m_pSaveBindData;
	VOID	CtrlTextSetting_All();
	VOID	InitControlList();
	VOID	UpdateControlList(BOOL bOnce = FALSE);
	LTBOOL	Test_SameText(int iIndex);
	VOID	ControlRender();
	VOID	ControlChange();
	VOID	SetControlText(int iIndex, BOOL bOnce = FALSE );
	//  선택된 인덱스에 해당한 값 세팅 
	VOID	SetSelectCtrlListSetting(int iIndex, char* szCommand, char* szAction = LTNULL);
	
	LTBOOL  CheckMouseWheel (DeviceInput* pInput, int iIndex);

	//LTBOOL  HandleKeyDown(int key, int rep);
    LTBOOL  KeyRemappable (DeviceInput* pInput);
	VOID    Bind(int nCommand, uint16 diCode, char *lpszControlName, uint32 deviceType);
    VOID    UnBind( uint32 nDeviceObjectId, char const* pszControlName, uint32 deviceType);
	LTBOOL	SetCurrentSelection (DeviceInput* pInput, int iIndex);

	// Performance & Etc
	VOID	InitPerforAndEtcCheckButton();
	VOID	ApplyPerforAndEtc();
	LTBOOL  SetPerforCkBtnVisible(LTBOOL bVisible = LTTRUE);
	LTBOOL	SetEtcCkBtnVisible(LTBOOL bVisible = LTTRUE);

//-------------------------------------- 

// 크로스헤어 선택탭-----------------------------------
	CLTGUIButton*			m_pCkBtnCrossHair[4];
	CLTGUIFrame*			m_pFmCrossHair;
	uint8					m_nCrossHair_CurrentIndex;
	VOID	ResetCrossHair_CkButton(int nSelect = -1);
	VOID	SetCrossHair_CkBtnVisible(LTBOOL bVisible = LTTRUE);

//-----------------------------------------------------

	CUserProfile*			m_pProfile;
	uint16					m_iGamma;    // 감마값 

	// Button
	CLTGUIButton*			m_pBtnInit;
	CLTGUIButton*			m_pBtnOK;
	CLTGUIButton*			m_pBtnCancel;
	// TabButton
	CLTGUIButton*			m_pTabBtnDisplay;
	CLTGUIButton*			m_pTabBtnCtrl;
	CLTGUIButton*			m_pTabBtnPerformance;
	CLTGUIButton*			m_pTabBtnEtc;
	// Effect_Check
	CLTGUIButton*			m_pCkBtnPerformance[10][2];
	CLTGUIButton*			m_pCkBtnEtc[10][2];
	LTBOOL					m_bPerforTextVisible;
	LTBOOL					m_bEtcTextVisible;
	CSAD3DText*				m_pPerformanceText;
	CSAD3DText*				m_pEtcText;
	int						m_iPerformanceTotalCnt;
	int						m_iEtcTotalCnt;

	// Drag & Scroll
	CLTGUIButton*			m_pBtnResolUp;
	CLTGUIButton*			m_pBtnResolDown;
	CLTGUIButton*			m_pBtnCtrlUp;
	CLTGUIButton*			m_pBtnCtrlDown;
	CLTGUIDragButton*		m_pScrollResol;
	CLTGUIDragButton*		m_pScrollCtrl;
	
	// Gamma Package
	CLTGUIButton*			m_pBtnGammaLow;
	CLTGUIButton*			m_pBtnGammaHigh;
	CDragSupportButton*		m_pDragSupGamma;
	CLTGUIDragButton*		m_pScrollGamma;
	CLTGUIEditBox*			m_pEBGamma; 
	CLTGUIFrame*			m_pFmGauge_Gamma;
	// Sound_Music Package
	CLTGUIButton*			m_pBtn_SndMusicLow;
	CLTGUIButton*			m_pBtn_SndMusicHigh;
	CDragSupportButton*		m_pDragSupSndMusic;
	CLTGUIDragButton*		m_pScroll_SndMusic;
	CLTGUIEditBox*			m_pEB_SndMusic; 
	CLTGUIFrame*			m_pFmGauge_SndMusic;
	// Sound_Effect Package
	CLTGUIButton*			m_pBtn_SndEffectLow;
	CLTGUIButton*			m_pBtn_SndEffectHigh;
	CDragSupportButton*		m_pDragSupSndEffect;
	CLTGUIDragButton*		m_pScroll_SndEffect;
	CLTGUIEditBox*			m_pEB_SndEffect; 
	CLTGUIFrame*			m_pFmGauge_SndEffect;


	// Frame
	CLTGUIFrame*			m_pFmControl;
	CLTGUIFrame*			m_pFmPerformance;
	CLTGUIFrame*			m_pFmEtc;
	
	FLOAT					m_fDragRatio;
	
	//[MURSUM]
	LTRect					m_rcResol[6];
};

extern CPopScreenOption* g_pPopupOption;

#endif // _POPUP_SCREEN_OPTION_H_