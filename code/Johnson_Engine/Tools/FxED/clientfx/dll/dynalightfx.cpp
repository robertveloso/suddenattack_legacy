//------------------------------------------------------------------
//
//   MODULE  : DYNALIGHTFX.CPP
//
//   PURPOSE : Implements class CDynaLightFX
//
//   CREATED : On 12/14/98 At 5:43:43 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "DynaLightFX.h"
#include "ClientFX.h"

//------------------------------------------------------------------
//
//   FUNCTION : CDynaLightFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CDynaLightFX::CDynaLightFX()
{
	m_bFlicker = false;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CDynaLightFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CDynaLightFX::~CDynaLightFX()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CDynaLightFX
//
//------------------------------------------------------------------

bool CDynaLightFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	// Perform base class initialisation

	if (!CBaseFX::Init(pClientDE, pBaseData, pProps)) return false;

	LTVector vScale;
	vScale.Init(100.0f, 100.0f, 100.0f);

	ocs.m_ObjectType		= OT_LIGHT;
	ocs.m_Flags				= 0;
	ocs.m_Pos				= pBaseData->m_vPos;
	ocs.m_Scale				= vScale;

	// Create the light

	m_hObject = m_pClientDE->CreateObject(&ocs);

	// Set the object colour

	m_pClientDE->SetLightColor(m_hObject, 1.0f, 0.0f, 1.0f);
	m_pClientDE->SetLightRadius(m_hObject, 100.0f);

	// We don't want the colour updated thankyou

	m_bUpdateColour = true;
	m_bUpdateScale  = true;

	// Initialise members

	CLinkListNode<FX_PROP> *pNode = pProps->GetHead();

	while (pNode)
	{		
		if (!stricmp(pNode->m_Data.m_sName, "Flicker"))
		{
			m_bFlicker = pNode->m_Data.m_data.m_nVal ? true : false;
		}

		pNode = pNode->m_pNext;
	}
		
	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CDynaLightFX
//
//------------------------------------------------------------------

void CDynaLightFX::Term()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CDynaLightFX
//
//------------------------------------------------------------------

bool CDynaLightFX::Update(float tmCur)
{
	// Base class update first
	
	if (!CBaseFX::Update(tmCur)) return false;

	if (m_dwState & FS_SHUTDOWN)
	{
		m_pClientDE->SetLightRadius(m_hObject, 0);
		
		return true;
	}

	// If we're flickering, change some of the attributes slightly

	if (m_bFlicker)
	{
		float fRand = 0.3f + fxGetRandom(0.0f, 0.19f);
		
		m_red   *= fRand;
		m_green *= fRand;
		m_blue  *= fRand;
	}
	
	// Set the new light colour

	m_pClientDE->SetLightColor(m_hObject, m_red, m_green, m_blue);

	// Set the new light scale

	m_pClientDE->SetLightRadius(m_hObject, m_scale);

	// Store this as the last time update

	m_tmLastUpdate = tmCur;

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetDynaLightFXProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetDynaLightProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;

	// Add the base props

	AddBaseProps(pList);

	fxProp.Int("Flicker", 0);
	pList->AddTail(fxProp);
}