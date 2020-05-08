//------------------------------------------------------------------
//
//   MODULE  : LIGHTNINGFX.CPP
//
//   PURPOSE : Implements class CLightningFX
//
//   CREATED : On 10/12/98 At 5:07:14 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "LightningFX.h"
#include "ClientFX.h"

LTVector		g_vTmpBuf[1024];
LTVector		g_vTran[1024];
LT_POLYGT3	g_fan[1024];

//------------------------------------------------------------------
//
//   FUNCTION : CLightningFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CLightningFX::CLightningFX()
{
	m_nType				  = LT_BALL;
	m_vTarget			  = LTVector(0.0f, 0.0f, 0.0f);
	m_fRadius			  = 50.0f;
	m_fWidth			  = 3.0f;
	m_fRandomness		  = 2.0f;

	m_dwBreaks			  = 10;
	m_tmLastEmission	  = 0.0f;
	m_tmLightningEmission = 0.5f;
	m_tmLightningLifespan = 0.5f;
	m_bLoadFailed		  = false;
	m_hTexture			  = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CLightningFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CLightningFX::~CLightningFX()
{
	if (m_pBreaks) delete [] m_pBreaks;
	if (m_pLightning) delete [] m_pLightning;
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);

	if (m_hTexture) 
	{
//		m_pClientDE->GetTexInterface()->ReleaseTextureHandle(m_hTexture);
		m_hTexture = NULL;
	}

	m_pBreaks = NULL;
	m_pLightning = NULL;
	m_hObject = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CLightningFX
//
//------------------------------------------------------------------

bool CLightningFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	// Perform base class initialisation first

	if (!CBaseFX::Init(pClientDE, pBaseData, pProps)) return false;

	// Initialise members

	CLinkListNode<FX_PROP> *pNode = pProps->GetHead();

	while (pNode)
	{
		if (!stricmp(pNode->m_Data.m_sName, "Texture"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);

			// Get the path name

			char *sExt  = strtok(sTmp, "|");
			char *sPath = strtok(NULL, "|");
			if (sPath) strcpy(m_sTextureName, sPath);
		}
		if (!stricmp(pNode->m_Data.m_sName, "Type"))
		{
			char sTmp[128];
			strcpy(sTmp, pNode->m_Data.m_data.m_sVal);
			char *sVal = strtok(sTmp, ",");
			int nVal = atoi(sVal);
			m_dwType = (uint32)nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "NumLightning"))
		{
			m_nLightning = pNode->m_Data.m_data.m_nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "NumBreaks"))
		{
			m_dwBreaks = (uint32)pNode->m_Data.m_data.m_nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "EmissionInterval"))
		{
			m_tmLightningEmission = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "LightningLifespan"))
		{
			m_tmLightningLifespan = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Width"))
		{
			m_fTrailWidth = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Radius"))
		{
			m_fRadius = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Perturb"))
		{
			m_fPerturb = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Flash"))
		{
			m_nFlash = pNode->m_Data.m_data.m_nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "Direction"))
		{
			m_vDir.x = pNode->m_Data.m_data.m_fVec[0];
			m_vDir.y = pNode->m_Data.m_data.m_fVec[1];
			m_vDir.z = pNode->m_Data.m_data.m_fVec[2];
		}
		
		pNode = pNode->m_pNext;
	}

	// Fill out the creation structure

	LTVector vScale;
	vScale.Init(1.0f, 1.0f, 1.0f);

	// Create dummy object

	ocs.m_ObjectType	= OT_NORMAL;
	ocs.m_Flags			= FLAG_UPDATEUNSEEN;
	ocs.m_Pos			= pBaseData->m_vPos;
	ocs.m_Scale			= vScale;

	m_hObject = m_pClientDE->CreateObject(&ocs);
	
	ocs.m_ObjectType	= OT_LINESYSTEM;
	ocs.m_Flags			= FLAG_UPDATEUNSEEN;
	ocs.m_Pos			= pBaseData->m_vPos;
	ocs.m_Scale			= vScale;

	// Create lightning array

	m_pLightning = new LIGHTNING [m_nLightning];

	// Create breaks array

	m_pBreaks = new LTVector [m_dwBreaks];

	// Create the trail linklist

	for (uint32 i = 0; i < m_dwBreaks; i ++)
	{
		TRAIL_SECTION ts;
		ts.m_red   = 255;
		ts.m_blue  = 255;
		ts.m_green = 255;
		ts.m_alpha = 128;

		m_collLightning.AddTail(ts);
	}

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CLightningFX
//
//------------------------------------------------------------------

void CLightningFX::Term()
{
	if (m_pBreaks) delete [] m_pBreaks;
	if (m_pLightning) delete [] m_pLightning;
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);

	m_pBreaks = NULL;
	m_pLightning = NULL;
	m_hObject = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates the lightning
//
//------------------------------------------------------------------

float g_uOffset = 0.0f;

bool CLightningFX::Update(float tmCur)
{
	if ((!m_hTexture) && (!m_bLoadFailed))
	{
		m_pClientDE->GetTexInterface()->FindTextureFromName(m_hTexture, m_sTextureName);

		if (!m_hTexture)
		{
			m_bLoadFailed = true;
		}
	}

	CBaseFX::Update(tmCur);

	// Create some new lightning

	if (tmCur > m_tmLastEmission + m_tmLightningEmission)
	{
		for (uint32 i = 0; i < m_nLightning; i ++)
		{
			if (!m_pLightning[i].m_bActive)
			{
				LTVector vTarget;
				LTVector vPos = m_vPos + m_vMotionOffset + m_vRandOffset;

				m_pLightning[i].m_tmLifespan = (m_tmLightningLifespan / 2.0f) + fxGetRandom(0.0f, m_tmLightningLifespan / 2.0f);
				m_pLightning[i].m_tmStart    = tmCur;

				switch (m_dwType)
				{
					case LT_SINGLE :
					{
						ClientIntersectQuery ciq;
						ClientIntersectInfo  cii;

						ciq.m_From = vPos;
						ciq.m_Direction = m_vDir;

						if (m_pClientDE->CastRay(&ciq, &cii))
						{
							// We got a strike !!!!

							vTarget					  = cii.m_Point;
							m_pLightning[i].m_bActive = true;
						}
					}
					break;

					case LT_MEGAZAP :
					{
						ClientIntersectQuery ciq;
						ClientIntersectInfo  cii;

						ciq.m_From = vPos;

						LTVector vEmit;
						vEmit.x = (float)(-1000 + (rand() % 2000));
						vEmit.y = (float)(-1000 + (rand() % 2000));
						vEmit.z = (float)(-1000 + (rand() % 2000));

						ciq.m_Direction = vEmit;

						if (m_pClientDE->CastRay(&ciq, &cii))
						{
							// We got a strike !!!!

							vTarget					  = cii.m_Point;
							m_pLightning[i].m_bActive = true;
						}
					}
					break;

					case LT_BALL :
					{
						vTarget = LTVector((float)-4096 + (rand() & 8191), (float)-4096 + (rand() & 8191), (float)-4096 + (rand() & 8191));
						vTarget.Norm(m_fRadius);
						vTarget += vPos;
						m_pLightning[i].m_bActive = true;
						
					}
					break;
				}

				m_pLightning[i].m_vNorm   = vTarget;
				m_pLightning[i].m_vTarget = vTarget;

				break;
			}
		}

		m_tmLastEmission = tmCur;
	}

	// Update the active lightning

	for (uint32 i = 0; i < m_nLightning; i ++)
	{
		if (tmCur >= m_pLightning[i].m_tmStart + m_pLightning[i].m_tmLifespan)
		{
			m_pLightning[i].m_bActive = false;
		}

		if (m_pLightning[i].m_bActive)
		{
			// Draw the lightning

			bool bDrawLightning = true;

			if (m_nFlash)
			{
				if (rand() % m_nFlash != 0) bDrawLightning = false;
			}

			if (bDrawLightning) DrawLightning(m_vPos + m_vMotionOffset + m_vRandOffset, 
											  m_pLightning[i].m_vTarget, 
											  m_pLightning[i].m_tmStart, 
											  m_pLightning[i].m_tmLifespan);
		}
	}


	g_uOffset -= 0.1f;

	// Success !!
	
	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetLightningProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetLightningProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;

	float fVec[3];
	fVec[0] = 0.0f;
	fVec[1] = -1.0f;
	fVec[2] = 0.0f;

	// Add the base props

	AddBaseProps(pList);

	// Add all the props to the list

	fxProp.Path("Texture", "dtx|...");
	pList->AddTail(fxProp);

	fxProp.Combo("Type", "0,Single,MegaZap,Ball");
	pList->AddTail(fxProp);

	fxProp.Int("NumLightning", 20);
	pList->AddTail(fxProp);

	fxProp.Int("NumBreaks", 10);
	pList->AddTail(fxProp);

	fxProp.Float("EmissionInterval", 1.0f);
	pList->AddTail(fxProp);

	fxProp.Float("LightningLifespan", 1.0f);
	pList->AddTail(fxProp);

	fxProp.Float("Radius", 50.0f);
	pList->AddTail(fxProp);

	fxProp.Float("Width", 5.0f);
	pList->AddTail(fxProp);

	fxProp.Float("Perturb", 2.0f);
	pList->AddTail(fxProp);

	fxProp.Int("Flash", 0);
	pList->AddTail(fxProp);

	fxProp.Vector("Direction", fVec);
	pList->AddTail(fxProp);
}

//------------------------------------------------------------------
//
//   FUNCTION : DrawLightning()
//
//   PURPOSE  : Draws some cool lightning
//
//------------------------------------------------------------------

void CLightningFX::DrawLightning(LTVector vFrom, LTVector vTo, float tmStart, float tmLifespan)
{
	LTVector vForward = vTo - vFrom;

	LTVector vUp = vForward.Cross(LTVector(1.0f, 0.0f, 0.0f));

	LTVector vRight = vForward.Cross(vUp);
	vRight.Norm();

	vUp = vForward.Cross(vRight);
	vUp.Norm();

	// Go thru the list and set it up correctly

	CLinkListNode<TRAIL_SECTION> *pNode = m_collLightning.GetHead();

	LTVector vSection = ((vTo - vFrom) / (float)m_dwBreaks);
	LTVector vCur = vFrom;

	while (pNode)
	{
		pNode->m_Data.m_uVal = 0.0f;

		pNode->m_Data.m_vPos = vCur;
		if (pNode->m_pNext && pNode->m_pPrev)
		{
			pNode->m_Data.m_vPos += (vRight * fxGetRandom(-m_fPerturb, m_fPerturb));
									(vUp * fxGetRandom(-m_fPerturb, m_fPerturb));
		}

		vCur += vSection;
		
		pNode = pNode->m_pNext;
	}

	// Render the trail

	if (!m_hTexture) return;

	float fr, fg, fb, fa;
	CalcColour(tmStart, m_pClientDE->GetTime(), tmLifespan, &fr, &fg, &fb, &fa);

	if (fr > 1.0f) fr = 1.0f;
	if (fg > 1.0f) fg = 1.0f;
	if (fb > 1.0f) fb = 1.0f;
	if (fa > 255.0f) fa = 255.0f;

	if (fr < 0.0f) fr = 0.0f;
	if (fg < 0.0f) fg = 0.0f;
	if (fb < 0.0f) fb = 0.0f;
	if (fa < 0.0f) fa = 0.0f;
	
	uint8 r = (uint8)(fr * 255.0f);
	uint8 g = (uint8)(fg * 255.0f);
	uint8 b = (uint8)(fb * 255.0f);
	uint8 a = (uint8)fa;
	a = 255 - a;

	ILTDrawPrim *pDrawPrimitive;
	pDrawPrimitive = m_pClientDE->GetDrawPrim();

	pDrawPrimitive->SetAlphaBlendMode(DRAWPRIM_BLEND_ADD);
	RenderPolyTrail(m_pClientDE, &m_collLightning, m_hCamera, m_fTrailWidth, r, g, b, a, m_hTexture, 0);//DPL_ADDALPHA);	
}

//------------------------------------------------------------------
//
//   FUNCTION : OnRendererShutdown()
//
//   PURPOSE  : Handles loss of app focus
//
//------------------------------------------------------------------

void CLightningFX::OnRendererShutdown()
{
	if (m_hTexture)
	{
//		m_pClientDE->GetTexInterface()->ReleaseTextureHandle(m_hTexture);
		m_hTexture = NULL;
	}
}
