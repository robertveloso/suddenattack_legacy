#include "stdafx.h"
#include "RadioFX.h"
#include "iltclient.h"
#include "ClientUtilities.h"
#include "ClientServerShared.h"

LTBOOL CRadioFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!CBaseParticleSystemFX::Init(psfxCreateStruct)) return LTFALSE;

	m_pTextureName	= "SFX\\Particle\\Smoke1.dtx";

	RDCREATESTRUCT* pRD = (RDCREATESTRUCT*)psfxCreateStruct;

	m_dwFlags = pRD->dwSystemFlags;

	m_vPos					= pRD->vPos;
	m_vColor1				= pRD->vColor1;
	m_vColor2				= pRD->vColor1;
	m_vMinDriftVel			= pRD->vMinDriftVel;
	m_vMaxDriftVel			= pRD->vMaxDriftVel;
	m_fVolumeRadius			= pRD->fVolumeRadius;
	m_fLifeTime				= pRD->fLifeTime;
	m_fRadius				= pRD->fRadius;
	m_fParticleCreateDelta	= pRD->fParticleCreateDelta;
	m_fMinParticleLife		= pRD->fMinParticleLife;
	m_fMaxParticleLife		= pRD->fMaxParticleLife;
	m_nNumParticles			= pRD->nNumParticles;
	m_bIgnoreWind			= pRD->bIgnoreWind;
	m_hstrTexture			= pRD->hstrTexture;

	//reset our elapsed time, and our emission time
	m_fElapsedTime			= 0.0f;
	m_fElapsedEmissionTime	= m_fParticleCreateDelta;

	m_fGravity		= 0.0f;

    return LTTRUE;
}

LTBOOL CRadioFX::Update()
{
	if (!m_hObject || !m_pClientDE ) return LTFALSE;

	if( g_pGameClientShell->IsServerPaused() )
	{
		g_pCommonLT->SetObjectFlags(m_hObject, OFT_Flags, FLAG_PAUSED, FLAG_PAUSED);
		return LTTRUE;
	}
	
	//make sure we aren't paused
	g_pCommonLT->SetObjectFlags(m_hObject, OFT_Flags, 0, FLAG_PAUSED);

	LTFLOAT fFrameTime = m_pClientDE->GetFrameTime();
	m_fElapsedTime += fFrameTime;
	if( m_fElapsedTime > m_fLifeTime )
	{
		Term();
	}	
	
	//return CBaseParticleSystemFX::Update();
	return LTTRUE;
}

LTBOOL CRadioFX::CreateObject(ILTClient* pClientDE)
{
	if (!pClientDE ) return LTFALSE;

	if (m_hstrTexture)
	{
		m_pTextureName = pClientDE->GetStringData(m_hstrTexture);
	}

    LTBOOL bRet = CBaseParticleSystemFX::CreateObject(pClientDE);


	// 한 번만 Add 하기 때문에 Create 할 때 Add 했음.
	LTVector vPos(0,0,0);
	LTVector vDriftVel(0,0,0);
	LTVector vColor(255,255,255);
	m_pClientDE->AddParticle(m_hObject, &vPos, &vDriftVel, &vColor, m_fLifeTime);

	return bRet;
}