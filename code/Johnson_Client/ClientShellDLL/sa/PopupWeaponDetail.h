#ifndef _POPUP_WEAPON_DETAIL_H_
#define _POPUP_WEAPON_DETAIL_H_

#include "ScreenBase.h"
#include "../ClientShellShared/HanText.h"
#include "PopupScreenBase.h"

class CPopUpWeaponDetail : public CPopScreenBase
{
	public :
		CPopUpWeaponDetail();
		virtual ~CPopUpWeaponDetail();

		void	Init();
		void	Term();
		void	OnFocus(LTBOOL bFocus);
		void	CreateBackeFrame( eScreenID nScreen );

		void	Render();
		void	SetGauge(int nDamage, int nShoot, int nHit, int nReact, int nClip, int nSltAmmo);
		void	SetDescription( char* szDescription );

		//void	CreateWeaponDetailSFX( int nWeapon );
		//void	ClearWeaponDetailSFX();
		//void	UpdateDetailSFX();

		CBaseScaleFX		m_WeaponDetailSFX;
		CLTGUIFrame*		m_pFrmWeapon;
		CLTGUIFrame*		m_pFrmWeaponTi;

	protected :
		void	UpdateGuageAni();
		void	DrawPercent( CSAD3DText* pText, LTFLOAT fDenominator, LTFLOAT fNumerator );

		uint32	OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
		
		CLTGUIButton*		m_pBtnExit;
		CLTGUIFrame*		m_pFrmDamage;
		CLTGUIFrame*		m_pFrmShootingVel;
		CLTGUIFrame*		m_pFrmHitRate;
		CLTGUIFrame*		m_pFrmReactRate;
		CSAD3DText*			m_pWeaponDesc;
		CSAD3DText*			m_pShotsPerClip;

		LTFLOAT	m_fDamage;		LTFLOAT	m_fNowDamage;
		LTFLOAT	m_fShoot;		LTFLOAT	m_fNowShoot;
		LTFLOAT	m_fHit;			LTFLOAT	m_fNowHit;
		LTFLOAT	m_fReact;		LTFLOAT	m_fNowReact;
		LTFLOAT	m_fClip;		LTFLOAT	m_fNowClip;
		LTFLOAT	m_fSltAmmo;		LTFLOAT	m_fNowSltAmmo;
		
		CSAD3DText*			m_pDamage;
		CSAD3DText*			m_pShoot;
		CSAD3DText*			m_pHit;
		CSAD3DText*			m_pReact;
		
		LTBOOL		m_bFirstUpdate;
		int			m_nFinishAniFlags;
		HLTSOUND	m_hGaugeSound;
		

		static LTVector		s_vPos; 
		static LTVector		s_vU; 
		static LTVector		s_vR; 
		static LTVector		s_vF;
		static LTRotation	s_rRot;
};

extern CPopUpWeaponDetail* g_pPopupWeaponDetail;

#endif //_POPUP_WEAPON_DETAIL_H_