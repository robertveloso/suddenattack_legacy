// ----------------------------------------------------------------------- //
//
// MODULE  : FlashLight.h
//
// PURPOSE : FlashLight class - Definition
//
// CREATED : 07/21/99
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __FLASH_LIGHT_H__
#define __FLASH_LIGHT_H__

#include "ltbasedefs.h"
#include "PolyLineFX.h"

class CFlashLight
{
	public :

		CFlashLight();
		virtual ~CFlashLight();

		void	Term();
		virtual void Update();

		virtual void Toggle()	{ (m_bOn ? TurnOff() : TurnOn());}
		virtual void TurnOn();
		virtual void TurnOff();

		virtual LTBOOL IsOn() const { return m_bOn; }

	protected :

		virtual void CreateLight();

		virtual void GetLightPositions(LTVector & vStartPos, LTVector & vEndPos, LTVector & vUOffset, LTVector & vROffset) = 0;

		virtual LTBOOL UpdateServer() { return LTTRUE; }

		LTBOOL   m_bOn;
		HOBJECT	 m_hLight;
};

class CFlashLightPlayer : public CFlashLight
{
	public :
		void Init();
		virtual	void Update();
		virtual	void TurnOn();	

	protected :
		void GetLightPositions(LTVector & vStartPos, LTVector & vEndPos, LTVector & vUOffset, LTVector & vROffset) {};
		float		m_fFLLightRadius;
		float		m_fFLLightColor;		
		float		m_fFLLightOffsetUp;
		float		m_fFLLightOffsetForward;

		float		m_fTurnOffTime;
		
		LTVector	m_vLightPos;
};

class CFlashLight3rdPerson : public CFlashLightPlayer
{
	public :

		CFlashLight3rdPerson();
		~CFlashLight3rdPerson();

		void Init(HOBJECT hObj);

		virtual	void Update();
		void TurnOn( LTVector* pvFlashPos, LTRotation* prRot );

	protected :
		void GetLightPositions(LTVector & vStartPos, LTVector & vEndPos, LTVector & vUOffset, LTVector & vROffset) {};
		virtual void CreateLight();
		
		HOBJECT			m_hObj;
};

#endif // __FLASH_LIGHT_H__