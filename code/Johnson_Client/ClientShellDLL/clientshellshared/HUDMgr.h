// ----------------------------------------------------------------------- //
//
// MODULE  : HUDMgr.h
//
// PURPOSE : Definition of CHUDMgr class
//
// CREATED : 07/17/01
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __HUDMGR_H
#define __HUDMGR_H


#include "HUDItem.h"
#include "HUDChatInput.h"
#include "HUDScores.h"
#include "HUDRadar.h"
#include "HUDList.h"
#include "HUDHelp.h" //[yaiin]


enum eHUDUpdateFlag
{
	kHUDNone		= 0x00000000,
	kHUDFrame		= 0x00000001,
	kHUDHealth		= 0x00000002,
	kHUDArmor		= 0x00000004,
	kHUDDamage		= 0x00000008,
	kHUDAmmo		= 0x00000010,
	kHUDAir			= 0x00000020,	//TO2
	kHUDHiding		= 0x00000040,	//TO2
	kHUDWeapons		= 0x00000080,	//TRON
	kHUDPermissions	= 0x00000100,	//TRON
	kHUDVersion		= 0x00000200,	//TRON
	kHUDEnergy		= 0x00000400,	//TRON
	kHUDCarry		= 0x00000800,   //TO2
	kHUDChooser		= 0x00001000,	//TO2
	kHUDEnergyTrans = 0x00002000,	//TRON
	kHUDObjectives	= 0x00004000,	//both
	kHUDProcedurals = 0x00008000,	//TRON
	kHUDDistance	= 0x00010000,
	kHUDProgressBar	= 0x00020000,	//TO2
	kHUDDisplayMeter= 0x00040000,
	kHUDRespawn     = 0x00080000,
	kHUDScores		= 0x00100000,
	kHUDDoomsday	= 0x00200000,
	kHUDMissionText	= 0x00400000,
	kHUDAll			= 0xFFFFFFFF,
};


//******************************************************************************************
//** HUD Manager
//******************************************************************************************
class CHUDMgr
{
public:

	CHUDMgr();
	~CHUDMgr();

    virtual	LTBOOL		Init();
    virtual	void		Term();

    virtual	void        Render();
    virtual	void        Update();

	virtual	void		QueueUpdate(uint32 nUpdateFlag);
	virtual uint32		QueryUpdateFlags() {return m_UpdateFlags;}

	virtual	void		ScreenDimsChanged();

	virtual	void		NextLayout();
	virtual	void		PrevLayout();
	virtual	void		UpdateLayout();

	virtual	void		Show(LTBOOL bShow)	{m_bVisible = bShow;}
	virtual	LTBOOL		IsShown()			{return m_bVisible;}

	virtual void			SetRenderLevel(eHUDRenderLevel eLevel) {m_eLevel = eLevel;}
	virtual eHUDRenderLevel	GetRenderLevel() {return m_eLevel;}

	//[MURSUM]=======================================
	virtual void		WaitRestart();
	virtual void		SetWaitRestartFlag( bool bFlag ) { m_bWaitRestart = bFlag; }
	//===============================================


protected:
	void				SetRenderState();

	uint8		m_nCurrentLayout;
	LTBOOL		m_bVisible;

	uint32			m_UpdateFlags;
	eHUDRenderLevel m_eLevel;

	CHUDChatInput		m_ChatInput;
	CHUDChatMsg			m_ChatMsgs;
	CHUDHelpMsg			m_HelpMsg;
	//CHUDPickupMsg		m_PickupMsgs;
	CHUDKillMsg			m_KillMsgs;
	CHUDNickList		m_NickList;
	CHUDRadar			m_Radar;
	CHUDScores			m_Scores;
	CHUDHelp			m_HelpBoard; //[yaiin]
	
	//items
	typedef std::vector<CHUDItem *> ItemArray;
	ItemArray m_itemArray;			// Pointer to each screen
	
	float				m_fRestartTime;
	bool				m_bWaitRestart;

};

extern CHUDMgr*				g_pHUDMgr;
extern CHUDRadar*			g_pRadar;
extern CHUDScores*			g_pScores;
extern CHUDChatInput*		g_pChatInput;
extern CHUDChatMsg*			g_pChatMsgs;
extern CHUDHelpMsg*			g_pHelpMsgs; //마우스 감도 몇인지 확인해 주는 메시지
//extern CHUDPickupMsg*		g_pPickupMsgs;
extern CHUDKillMsg*			g_pKillMsgs;
extern CHUDNickList*		g_pNickList;
extern CHUDHelp*			g_pHelpBoard; //[yaiin] : CInterfaceMgr에서 쓰기 위함

#endif
