// ----------------------------------------------------------------------- //
//
// MODULE  : HUDRadio.h
//
// PURPOSE : Definition of CHUDRadio to display transmission messages
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_RADIO_H
#define __HUD_RADIO_H

#include "HUDItem.h"
#include "ClientServerShared.h"

#define MAX_SET_COUNT   3

#define RADIO_HIDE		-1
#define RADIO_FIRST		0
#define RADIO_SECOND	6
#define RADIO_THIRD		12


#define RID_FIRE_IN_THE_HOLE	18
//******************************************************************************************
//** HUD Radio dialog
//******************************************************************************************
class CHUDRadio : public CHUDItem
{
public:
	CHUDRadio();
	

    virtual LTBOOL      Init();
	virtual void		Term();

    virtual void        Render();
	virtual void		Update() {};
    
	void				ScreenDimChanged();

	//hide or show
	void	Show( int nFirstIndex=0 );
	bool	IsVisible() {return m_bVisible;}
	VOID	SetVisible(bool bVisible){m_bVisible = bVisible;}
	void	Choose(uint8 nChoice);
	const char*	GetRadioString( uint8 nIndex );

protected:
	int				m_nMaxTextLen[MAX_SET_COUNT];
	int				m_nFirstIndex;
	LTIntPt			m_BasePos;
	bool			m_bVisible;
	CSAD3DTextEx*	m_pRadioList;
	int				m_nMaxCount;
	int				m_nRadioCount;
	float			m_fPlayTauntSoundTime;
	
	//[SPIKE] 라디오메세지와 WeaponChange와 중첩되는 버그때문에 
	LTBOOL			m_bCurRadioKeyDown;
};



#endif