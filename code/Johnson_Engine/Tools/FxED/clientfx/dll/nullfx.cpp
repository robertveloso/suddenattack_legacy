//------------------------------------------------------------------
//
//   MODULE  : NULLFX.CPP
//
//   PURPOSE : Implements class CNullFX
//
//   CREATED : On 12/3/98 At 6:34:44 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "NullFX.h"
#include "ClientFX.h"

//------------------------------------------------------------------
//
//   FUNCTION : CNullFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CNullFX::CNullFX()
{
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CNullFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CNullFX::~CNullFX()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CNullFX
//
//------------------------------------------------------------------

bool CNullFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	// Perform base class initialisation

	if (!CBaseFX::Init(pClientDE, pBaseData, pProps)) return false;

	LTVector3f vPos;
	m_pClientDE->Physics()->GetPosition(m_hParent, &vPos);

	LTVector vScale;
	vScale.Init(1.0f, 1.0f, 1.0f);

	ocs.m_ObjectType		= OT_NORMAL;
	ocs.m_Flags				= FLAG_NOLIGHT;
	ocs.m_Pos				= vPos;
	ocs.m_Scale				= vScale;

	m_hObject = m_pClientDE->CreateObject(&ocs);
		
	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CNullFX
//
//------------------------------------------------------------------

void CNullFX::Term()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CNullFX
//
//------------------------------------------------------------------

bool CNullFX::Update(float tmCur)
{
	// Base class update first
	
	if (!CBaseFX::Update(tmCur)) return false;

	// Success !!

	return true;
}


//------------------------------------------------------------------
//
//   FUNCTION : fxGetNullProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetNullProps(CFastList<FX_PROP> *pList)
{
	// Add the base props

	AddBaseProps(pList);
}