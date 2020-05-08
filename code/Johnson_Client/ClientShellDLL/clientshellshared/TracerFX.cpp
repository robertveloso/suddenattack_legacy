// ----------------------------------------------------------------------- //
//
// MODULE  : TracerFX.cpp
//
// PURPOSE : Tracer special FX - Implementation
//
// CREATED : 1/21/98
//
// (c) 1998-2000 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "TracerFX.h"
#include "iltclient.h"
#include "ClientUtilities.h"
#include "SFXMsgIds.h"
#include "GameClientShell.h"
#include "VarTrack.h"
#include "CMoveMgr.h"

extern CGameClientShell* g_pGameClientShell;

VarTrack	g_cvarTracerSegments;
VarTrack	g_cvarTracerLifetime;
VarTrack	g_cvarTracerVelocity;


#define TRACER_SFX_MAX_DIST		100.0f
#define TRACER_SFX_CHECK_DIST	200.0f


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CTracerFX::Init
//
//	PURPOSE:	Init the tracer fx
//
// ----------------------------------------------------------------------- //

LTBOOL CTracerFX::Init(HLOCALOBJ hServObj, ILTMessage_Read *pMsg)
{
    if (!CSpecialFX::Init(hServObj, pMsg)) return LTFALSE;
    if (!pMsg) return LTFALSE;

	// Don't support server-side versions of this fx...

    return LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CTracerFX::Init
//
//	PURPOSE:	Init the tracer fx
//
// ----------------------------------------------------------------------- //

LTBOOL CTracerFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
	if (!CSpecialFX::Init(psfxCreateStruct)) return LTFALSE;

	TRCREATESTRUCT* pTR = (TRCREATESTRUCT*)psfxCreateStruct;
	m_cs = *pTR;

    if (!m_cs.pTracerFX) return LTFALSE;

	m_vDir  = m_cs.vEndPos - m_cs.vStartPos;
	m_fDist = m_vDir.Mag();
	m_vDir.Normalize();

	if (!g_cvarTracerSegments.IsInitted())
	{
        g_cvarTracerSegments.Init(g_pLTClient, "TracerSegments", NULL, 1.0f);
	}

	if (!g_cvarTracerLifetime.IsInitted())
	{
        g_cvarTracerLifetime.Init(g_pLTClient, "TracerLifetime", NULL, 1.f);
	}

	if (!g_cvarTracerVelocity.IsInitted()) 
	{
        g_cvarTracerVelocity.Init(g_pLTClient, "TracerVelocity", NULL, -1.0f);
	}

	if (g_cvarTracerVelocity.GetFloat() > 0.0f)
	{
		m_fVelocity = g_cvarTracerVelocity.GetFloat();
	}
	else
	{
		m_fVelocity = m_cs.pTracerFX->fVelocity;
	}


	// Calculate life time...

	if (m_fVelocity > 0.0f)
	{
		m_fLifetime = m_fDist / m_fVelocity;
	}
	else
	{
        return LTFALSE;
	}

	return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CTracerFX::Update
//
//	PURPOSE:	Update the tracer
//
// ----------------------------------------------------------------------- //

LTBOOL CTracerFX::Update()
{
	if (!m_pClientDE || !m_cs.pTracerFX) return LTFALSE;

    LTFLOAT fTime = m_pClientDE->GetTime();

	if (m_bFirstUpdate)
	{
		m_bFirstUpdate  = LTFALSE;
		m_fStartTime	= fTime;
		m_bPlaySound	= LTFALSE;

		PLFXCREATESTRUCT pls;

		m_vTracerPos				= m_cs.vStartPos;
		pls.pTexture				= m_cs.pTracerFX->szTexture[0] ? m_cs.pTracerFX->szTexture : LTNULL;
		pls.vStartPos				= m_cs.vStartPos;
		pls.vEndPos					= m_vTracerPos + (m_vDir * m_cs.pTracerFX->fLength);
		pls.vInnerColorStart		= m_cs.pTracerFX->vColor;
		pls.vInnerColorEnd			= m_cs.pTracerFX->vColor;
        pls.vOuterColorStart        = m_cs.pTracerFX->vColor;
        pls.vOuterColorEnd          = m_cs.pTracerFX->vColor;
		pls.fAlphaStart				= m_cs.pTracerFX->fInitialAlpha;
		pls.fAlphaEnd				= m_cs.pTracerFX->fFinalAlpha;
		pls.fMinWidth				= 0;
		pls.fMaxWidth				= m_cs.pTracerFX->fWidth;
		pls.fMinDistMult			= 1.0f;
		pls.fMaxDistMult			= 1.0f;
		//pls.fLifeTime				= g_cvarTracerLifetime.GetFloat();
		pls.fLifeTime				= m_fLifetime; 
		pls.fAlphaLifeTime			= pls.fLifeTime+1;
		pls.fPerturb				= 0.0f;
        pls.bAdditive               = LTTRUE;
        pls.bNoZ                    = LTTRUE;
		pls.nWidthStyle				= PLWS_CONSTANT;
		pls.nNumSegments			= (int)g_cvarTracerSegments.GetFloat();
		pls.bAlignUsingRot			= LTTRUE;

		if (!m_Tracer.Init(&pls) || !m_Tracer.CreateObject(m_pClientDE))
		{
			return LTFALSE;
		}

		m_bCheckSound = IsInRange();
	}
	else if (fTime > m_fStartTime + m_fLifetime)
	{
		return LTFALSE;
	}
	
	m_Tracer.Update();
	
	PolyLineList* pLines = m_Tracer.GetLines();
	if (pLines->GetLength() > 0)
	{
		PolyLine** pLine = pLines->GetItem(TLIT_LAST);
		if( pLine && *pLine )
		{
			m_vTracerPos += (m_vDir * g_pGameClientShell->GetFrameTime() * m_fVelocity);
			m_Tracer.MoveLine( (*pLine), m_vTracerPos, m_vTracerPos + (m_vDir * m_cs.pTracerFX->fLength) );

			if( LTTRUE == m_bCheckSound && LTFALSE == m_bPlaySound )
			{
				LTVector	vPlayerPos;
				LTVector	vPTDist;
				g_pLTClient->GetObjectPos( g_pMoveMgr->GetObject(), &vPlayerPos );
				vPTDist = vPlayerPos - m_vTracerPos;

				if( vPTDist.Length() < TRACER_SFX_MAX_DIST ) 
				{
					PlayFlySound( m_vTracerPos );
				}
			}
		}
	}
	else
	{
		return LTFALSE;
	}
	
    return LTTRUE;
}

void CTracerFX::PlayFlySound( LTVector& vTracerPos )
{
	static char	szSndName[MAX_PATH];
	sprintf( szSndName, "SA_WEAPONS/SND/FLYBY%d.WAV", GetRandom( 1, 4 ) );
	m_bPlaySound	= LTTRUE;
	g_pClientSoundMgr->PlaySoundFromPos( const_cast<LTVector&>( vTracerPos ),
										 szSndName,
										 3000.0f,
										 SOUNDPRIORITY_PLAYER_HIGH,
										 0,
										 SMGR_DEFAULT_VOLUME,
										 1.0f,
										 -1.0f,
										 WEAPONS_SOUND_CLASS );
}

//SPIKE
// 직선과 점의 거리계산
LTBOOL CTracerFX::IsInRange()
{
	LTVector	vA, vB, vC;
	LTVector	vPlayerPos;
	float		fA;

	g_pLTClient->GetObjectPos( g_pMoveMgr->GetObject(), &vPlayerPos );

	vA = vPlayerPos - m_cs.vStartPos;
	vB = m_cs.vEndPos - m_cs.vStartPos;
	vB.Normalize();

	fA = vA.Dot(vB);
	vB *= fA;
	vC = vA - vB;
	
	if( vC.Length() < TRACER_SFX_CHECK_DIST )
	{
		return LTTRUE;
	}

	return LTFALSE;
}