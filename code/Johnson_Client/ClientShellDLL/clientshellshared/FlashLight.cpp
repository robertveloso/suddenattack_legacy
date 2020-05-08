 // ----------------------------------------------------------------------- //
//
// MODULE  : FlashLight.cpp
//
// PURPOSE : FlashLight class - Implementation
//
// CREATED : 07/21/99
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "FlashLight.h"
#include "GameClientShell.h"
#include "ClientUtilities.h"
#include "VarTrack.h"
#include "BaseScaleFX.h"
#include "MsgIDs.h"
#include "VehicleMgr.h"
#include "CMoveMgr.h"

VarTrack	g_cvarFLMinLightRadius;
VarTrack	g_cvarFLMaxLightRadius;
VarTrack	g_cvarFLMinLightColor;
VarTrack	g_cvarFLMaxLightColor;
VarTrack	g_cvarFLLightOffsetUp;
VarTrack	g_cvarFLLightOffsetRight;
VarTrack	g_cvarFLLightOffsetForward;

static bool NonSolidFilterFn(HOBJECT hTest, void *pUserData)
{
	if (ObjListFilterFn(hTest, pUserData))
	{
		// Ignore non-solid objects (even if ray-hit is true)...

		uint32 dwFlags;
		g_pCommonLT->GetObjectFlags(hTest, OFT_Flags, dwFlags);

		if (!(dwFlags & FLAG_SOLID))
		{
			return false;
		}
	}
    return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight::CFlashLight()
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CFlashLight::CFlashLight()
{
    m_bOn	 = LTFALSE;
    m_hLight = LTNULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight::~CFlashLight()
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CFlashLight::~CFlashLight()
{
	Term();
}

void CFlashLight::Term()
{
	if (m_hLight)
	{
        g_pLTClient->RemoveObject(m_hLight);
        m_hLight = LTNULL;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight::TurnOn()
//
//	PURPOSE:	Turn light on
//
// ----------------------------------------------------------------------- //

void CFlashLight::TurnOn()
{
	CreateLight();

	if( m_hLight && !m_bOn )
	{
        m_bOn = LTTRUE;
		g_pCommonLT->SetObjectFlags(m_hLight, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight::TurnOff()
//
//	PURPOSE:	Turn light off
//
// ----------------------------------------------------------------------- //

void CFlashLight::TurnOff()
{
	if( m_hLight && m_bOn )
	{
        m_bOn = LTFALSE;
		g_pCommonLT->SetObjectFlags(m_hLight, OFT_Flags, 0, FLAG_VISIBLE);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight::CreateLight()
//
//	PURPOSE:	Create the dynamic light
//
// ----------------------------------------------------------------------- //

void CFlashLight::CreateLight()
{
	if (m_hLight) return;

    g_cvarFLMinLightRadius.Init(g_pLTClient, "FLMinRadius", NULL, 100.0f);
    g_cvarFLMaxLightRadius.Init(g_pLTClient, "FLMaxtRadius", NULL, 150.0f);
    g_cvarFLMinLightColor.Init(g_pLTClient, "FLMinColor", NULL, 150.0f);
    g_cvarFLMaxLightColor.Init(g_pLTClient, "FLMaxColor", NULL, 150.0f);
    g_cvarFLLightOffsetUp.Init(g_pLTClient, "FLOffsetUp", NULL, -20.0f);
    g_cvarFLLightOffsetRight.Init(g_pLTClient, "FLOffsetRight", NULL, 0.0f);
    g_cvarFLLightOffsetForward.Init(g_pLTClient, "FLOffsetForward", NULL, 15.0f);

	HOBJECT hCamera = g_pPlayerMgr->GetCamera();
	if (!hCamera) return;

	ObjectCreateStruct createStruct;
	INIT_OBJECTCREATESTRUCT(createStruct);

	createStruct.m_ObjectType	= OT_LIGHT;
	createStruct.m_Flags		= FLAG_VISIBLE;		
	g_pLTClient->GetObjectPos(hCamera, &(createStruct.m_Pos));

    m_hLight = g_pLTClient->CreateObject(&createStruct);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight::Update()
//
//	PURPOSE:	Update the flash light
//
// ----------------------------------------------------------------------- //

void CFlashLight::Update()
{
	if (!m_bOn || !m_hLight) return;

	// Calculate light position...

    HLOCALOBJ hPlayerObj = g_pLTClient->GetClientObject();
	if (!hPlayerObj) return;

    HOBJECT hFilterList[] = {hPlayerObj, g_pPlayerMgr->GetMoveMgr()->GetObject(), LTNULL};

	IntersectQuery qInfo;
	IntersectInfo iInfo;

	LTVector vPos, vEndPos, vUOffset, vROffset;

	GetLightPositions(vPos, vEndPos, vUOffset, vROffset);

	qInfo.m_From = vPos;
	qInfo.m_To   = vEndPos;

	qInfo.m_Flags = INTERSECT_OBJECTS | IGNORE_NONSOLID;
	qInfo.m_FilterFn = NonSolidFilterFn;
	qInfo.m_pUserData = hFilterList;

    if (g_pLTClient->IntersectSegment(&qInfo, &iInfo))
	{
		vEndPos = iInfo.m_Point;
	}

	g_pLTClient->SetObjectPos(m_hLight, &vEndPos);

    LTVector vDir = vEndPos - vPos;
    LTFLOAT fDist = vDir.Length();
	vDir *= 1.0f / fDist;

    LTFLOAT fLightRadius = g_cvarFLMinLightRadius.GetFloat() +
		((g_cvarFLMaxLightRadius.GetFloat() - g_cvarFLMinLightRadius.GetFloat()) * fDist / g_cvarFLLightOffsetForward.GetFloat());

    g_pLTClient->SetLightRadius(m_hLight, fLightRadius);

    LTVector vColor; 
    vColor.y = vColor.z = vColor.x = GetRandom(g_cvarFLMinLightColor.GetFloat(), g_cvarFLMaxLightColor.GetFloat());

    LTVector vLightColor = vColor / 255.0f;

    g_pLTClient->SetLightColor(m_hLight, vLightColor.x, vLightColor.y, vLightColor.z);
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLightPlayer::Init()
//
//	PURPOSE:	Turn light on
//
// ----------------------------------------------------------------------- //

void CFlashLightPlayer::Init()
{
	if( GetConsoleFloat("DynamicLightWorld", 0.0f ) <= 0.0f ) return;
	
	CreateLight();
	g_pCommonLT->SetObjectFlags(m_hLight, OFT_Flags, 0, FLAG_VISIBLE);
	
	m_fFLLightRadius		= g_cvarFLMaxLightRadius.GetFloat();
	m_fFLLightColor			= g_cvarFLMaxLightColor.GetFloat() / 255.0f;
	m_fFLLightOffsetUp		= g_cvarFLLightOffsetUp.GetFloat();
	m_fFLLightOffsetForward	= g_cvarFLLightOffsetForward.GetFloat();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLightPlayer::TurnOn()
//
//	PURPOSE:	Turn light on
//
// ----------------------------------------------------------------------- //

void CFlashLightPlayer::TurnOn()
{
	if( !m_hLight ) return;
	
	if( !m_bOn )
	{
        m_bOn = LTTRUE;
		g_pCommonLT->SetObjectFlags(m_hLight, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);		
	}
	
	m_fTurnOffTime			= g_pLTClient->GetTime() + 0.04f;

	LTRotation	rRot;
	HOBJECT hCamera = g_pPlayerMgr->GetCamera();
	if (!hCamera) return;

	g_pLTClient->GetObjectRotation(hCamera, &rRot);
	g_pLTClient->GetObjectPos(hCamera, &m_vLightPos);

	m_vLightPos		+= rRot.Forward() * m_fFLLightOffsetForward;
	m_vLightPos.y	+= m_fFLLightOffsetUp;
	
	g_pLTClient->SetObjectPos(m_hLight, &m_vLightPos);
    g_pLTClient->SetLightRadius(m_hLight, m_fFLLightRadius);
    g_pLTClient->SetLightColor(m_hLight, m_fFLLightColor, m_fFLLightColor, m_fFLLightColor*0.7f);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLightPlayer::Update()
//
//	PURPOSE:	Update the flash light
//
// ----------------------------------------------------------------------- //

void CFlashLightPlayer::Update()
{
	if( m_bOn && m_fTurnOffTime <= g_pLTClient->GetTime() )
	{
		TurnOff();
		return;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight3rdPerson::CFlashLight3rdPerson()
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CFlashLight3rdPerson::CFlashLight3rdPerson()
{
	m_hObj = LTNULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight3rdPerson::~CFlashLight3rdPerson()
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CFlashLight3rdPerson::~CFlashLight3rdPerson()
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight3rdPerson::Init()
//
//	PURPOSE:	Initializes the flashlight
//
// ----------------------------------------------------------------------- //

void CFlashLight3rdPerson::Init(HOBJECT hObj)
{
	if( GetConsoleFloat("DynamicLightWorld", 0.0f ) <= 0.0f ) return;

	CreateLight();
	g_pCommonLT->SetObjectFlags(m_hLight, OFT_Flags, 0, FLAG_VISIBLE);

	m_hObj = hObj;

	
	m_fFLLightRadius		= g_cvarFLMaxLightRadius.GetFloat();
	m_fFLLightColor			= 80.0f / 255.0f;
	m_fFLLightOffsetUp		= -40.0f;
	m_fFLLightOffsetForward	= 10.0f;
}


void CFlashLight3rdPerson::CreateLight()
{
	CFlashLightPlayer::CreateLight();
	g_pCommonLT->SetObjectFlags( m_hLight, OFT_Flags2, FLAG2_DYNAMICDIRLIGHT, FLAG2_DYNAMICDIRLIGHT );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight3rdPerson::TurnOn()
//
//	PURPOSE:	Turn light on
//
// ----------------------------------------------------------------------- //

void CFlashLight3rdPerson::TurnOn( LTVector* pvFlashPos, LTRotation* prRot )
{
	if( !m_hLight ) return;
	
	if( !m_bOn )
	{
        m_bOn = LTTRUE;
		g_pCommonLT->SetObjectFlags(m_hLight, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);		
	}

	m_fTurnOffTime	= g_pLTClient->GetTime() + 0.04f;
	
	m_vLightPos.x	= pvFlashPos->x;
	m_vLightPos.y	= pvFlashPos->y;
	m_vLightPos.z	= pvFlashPos->z;	

	m_vLightPos		+= prRot->Forward()*m_fFLLightOffsetForward;
	m_vLightPos.y	+= m_fFLLightOffsetUp;

	g_pLTClient->SetObjectPos(m_hLight, &m_vLightPos);
    g_pLTClient->SetLightRadius(m_hLight, m_fFLLightRadius);
    g_pLTClient->SetLightColor(m_hLight, m_fFLLightColor, m_fFLLightColor, m_fFLLightColor*0.7f);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CFlashLight3rdPerson::Update()
//
//	PURPOSE:	Update the flash light
//
// ----------------------------------------------------------------------- //

void CFlashLight3rdPerson::Update()
{
	if( m_bOn && m_fTurnOffTime <= g_pLTClient->GetTime() )
	{
		TurnOff();
		return;
	}
}
