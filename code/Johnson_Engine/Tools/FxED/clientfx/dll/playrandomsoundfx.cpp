//------------------------------------------------------------------
//
//   MODULE  : PLAYRANDOMSOUNDFX.CPP
//
//   PURPOSE : Implements class CPlayRandomSoundFX
//
//   CREATED : On 12/15/98 At 5:06:01 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "PlayRandomSoundFX.h"
#include "ClientFX.h"
#include "stdio.h"

//------------------------------------------------------------------
//
//   FUNCTION : CPlayRandomSoundFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CPlayRandomSoundFX::CPlayRandomSoundFX()
{
	m_bLoop				 = false;
	m_hSound			 = NULL;
	m_bPlayLocal		 = false;
	m_nVolume			 = 100;
	m_nRand				 = 0;
	m_sMultiplaySound[0] = 0;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CPlayRandomSoundFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CPlayRandomSoundFX::~CPlayRandomSoundFX()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);

	if (m_hSound)
	{
		if (m_hSound) m_pClientDE->KillSound(m_hSound);
	}

	m_hObject = NULL;
	m_hSound  = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CPlayRandomSoundFX
//
//------------------------------------------------------------------

bool CPlayRandomSoundFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	// Perform base class initialisation

	if (!CBaseFX::Init(pClientDE, pBaseData, pProps)) return false;

	// Initialise members

	CLinkListNode<FX_PROP> *pNode = pProps->GetHead();

	while (pNode)
	{
		if (!stricmp(pNode->m_Data.m_sName, "Sound"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);

			// Get the path name

			char *sExt  = strtok(sTmp, "|");
			char *sPath = strtok(NULL, "|");
			if (sPath) strcpy(m_sSoundName, sPath);
		}
		else if (!stricmp(pNode->m_Data.m_sName, "MultiPlaySound"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);

			// Get the path name

			char *sExt  = strtok(sTmp, "|");
			char *sPath = strtok(NULL, "|");
			if (sPath) strcpy(m_sMultiplaySound, sPath);
			if (!stricmp(m_sMultiplaySound, "..."))
			{
				m_sMultiplaySound[0] = 0;
			}
		}
		else if (!stricmp(pNode->m_Data.m_sName, "NumRand"))
		{
			m_nRand = pNode->m_Data.m_data.m_nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "InnerRadius"))
		{
			m_fInnerRadius = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "OuterRadius"))
		{
			m_fOuterRadius = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Volume"))
		{
			m_nVolume = pNode->m_Data.m_data.m_nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Priority"))
		{
			m_nPriority = pNode->m_Data.m_data.m_nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Loop"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);
			char *sVal = strtok(sTmp, ",");
			int nVal = atoi(sVal);
			m_bLoop = (uint32)nVal ? true : false;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "PlayLocal"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);
			char *sVal = strtok(sTmp, ",");
			int nVal = atoi(sVal);
			m_bPlayLocal = (uint32)nVal ? true : false;
		}				
		pNode = pNode->m_pNext;
	}

	LTVector3f vPos;
	m_pClientDE->Physics()->GetPosition(m_hParent, &vPos);

	LTVector vScale;
	vScale.Init(1.0f, 1.0f, 1.0f);

	ocs.m_ObjectType		= OT_NORMAL;
	ocs.m_Flags				= FLAG_NOLIGHT;
	ocs.m_Pos				= vPos;
	ocs.m_Scale				= vScale;

	m_hObject = m_pClientDE->CreateObject(&ocs);

	// Play the sound

	PlaySoundInfo psi;
	memset(&psi, 0, sizeof(PlaySoundInfo));

	psi.m_dwFlags = PLAYSOUND_GETHANDLE |
				    PLAYSOUND_CTRL_VOL |
					PLAYSOUND_CLIENT;
	if ((m_bPlayLocal) && (!g_bMultiplay))
	{
		psi.m_dwFlags |= PLAYSOUND_LOCAL;
	}
	else
	{
		psi.m_dwFlags |= PLAYSOUND_3D;
	}
	if (m_bLoop) psi.m_dwFlags |= PLAYSOUND_LOOP;

	psi.m_nVolume = m_nVolume;

	char sTmp[256];
	
	// Strip of the extension and single number

	m_sSoundName[strlen(m_sSoundName) - 5] = 0;

	if (m_nRand)
	{
		if ((g_bMultiplay) && (m_sMultiplaySound[0]))
		{
			sprintf(sTmp, "%s%d.wav", m_sMultiplaySound, rand() % m_nRand);		
		}
		else
		{
			sprintf(sTmp, "%s%d.wav", m_sSoundName, rand() % m_nRand);
		}
	}
	else
	{
		if ((g_bMultiplay) && (m_sMultiplaySound[0]))
		{
			sprintf(sTmp, "%s.wav", m_sMultiplaySound);
		}
		else
		{
			sprintf(sTmp, "%s.wav", m_sSoundName);
		}
	}
	
	strcpy(psi.m_szSoundName, sTmp);
	psi.m_nPriority		= m_nPriority;
	psi.m_vPosition		= m_vPos;
	psi.m_fInnerRadius	= m_fInnerRadius;
	psi.m_fOuterRadius	= m_fOuterRadius;

	m_hSound = NULL;
	if (g_bAppFocus)
	{
		if (m_pClientDE->PlaySound(&psi) == LT_OK)
		{
			m_hSound = psi.m_hSound;
		}
	}
		
	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CPlayRandomSoundFX
//
//------------------------------------------------------------------

void CPlayRandomSoundFX::Term()
{
	m_pClientDE->DeleteObject(m_hObject);

	if (m_hSound)
	{
		if (m_hSound) m_pClientDE->KillSound(m_hSound);
	}

	m_hObject = NULL;
	m_hSound  = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CPlayRandomSoundFX
//
//------------------------------------------------------------------

bool CPlayRandomSoundFX::Update(float tmCur)
{
	// Base class update first
	
	if (!CBaseFX::Update(tmCur)) return false;

	if (m_dwState & FS_SHUTDOWN)
	{
		if (m_hSound)
		{
			m_pClientDE->KillSound(m_hSound);
			m_hSound = NULL;
		}

		return true;
	}

	if (m_hSound) 
	{
		LTVector3f vPos = m_vPos;
		m_pClientDE->SetSoundPosition(m_hSound, &vPos);
	}

	// Success !!

	return true;
}


//------------------------------------------------------------------
//
//   FUNCTION : fxGetPlayRandomSoundFXProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetPlayRandomSoundProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;

	// Add the base props

	AddBaseProps(pList);

	// Add all the props to the list

	fxProp.Path("Sound", "wav|...");
	pList->AddTail(fxProp);

	fxProp.Int("NumRand", 0);
	pList->AddTail(fxProp);

	fxProp.Combo("Loop", "0,No,Yes");
	pList->AddTail(fxProp);

	fxProp.Combo("PlayLocal", "0,No,Yes");
	pList->AddTail(fxProp);

	fxProp.Float("InnerRadius", 100.0f);
	pList->AddTail(fxProp);

	fxProp.Float("OuterRadius", 500.0f);
	pList->AddTail(fxProp);

	fxProp.Int("Volume", 100);
	pList->AddTail(fxProp);

	fxProp.Int("Priority", 0);
	pList->AddTail(fxProp);

	fxProp.Path("MultiPlaySound", "wav|...");
	pList->AddTail(fxProp);
}