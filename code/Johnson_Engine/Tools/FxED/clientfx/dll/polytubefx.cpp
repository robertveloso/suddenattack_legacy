//------------------------------------------------------------------
//
//   MODULE  : POLYTUBEFX.CPP
//
//   PURPOSE : Implements class CPolyTubeFX
//
//   CREATED : On 12/3/98 At 6:34:44 PM
//
//------------------------------------------------------------------

// Includes....

#include "stdafx.h"
#include "PolyTubeFX.h"
#include "ClientFX.h"

//------------------------------------------------------------------
//
//   FUNCTION : CPolyTubeFX()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CPolyTubeFX::CPolyTubeFX()
{
	m_tmAddPtInterval   = 0.1f;
	m_nMaxTrailLength   = 40;
	m_tmSectionLifespan = 2.0f;
	m_hTexture			= NULL;
	m_dwWidth			= 1;
	m_dwHeight			= 1;
	m_uOffset			= 0.0f;
	m_uAdd				= 0.0f;
	m_fTrailWidth		= 20.0f;
	m_bLoadFailed		= false;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CPolyTubeFX
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CPolyTubeFX::~CPolyTubeFX()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	if (m_hTexture)
	{
//		m_pClientDE->GetTexInterface()->ReleaseTextureHandle(m_hTexture);
	}
	m_hObject = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CPolyTubeFX
//
//------------------------------------------------------------------

bool CPolyTubeFX::Init(ILTClient *pClientDE, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	ObjectCreateStruct ocs;
	INIT_OBJECTCREATESTRUCT(ocs);

	LTVector vSave = pBaseData->m_vPos;

	// Perform base class initialisation

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
			if (sPath)
			{
				strcpy(m_sPath, sPath);
			}
		} 
		else if (!stricmp(pNode->m_Data.m_sName, "TrailWidth"))
		{
			m_fTrailWidth = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "UAdd"))
		{
			m_uAdd = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "SectionLifespan"))
		{
			m_tmSectionLifespan = pNode->m_Data.m_data.m_fVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "TrailLen"))
		{
			m_nMaxTrailLength = pNode->m_Data.m_data.m_nVal;
		}
		else if (!stricmp(pNode->m_Data.m_sName, "SectionInterval"))
		{
			m_tmAddPtInterval = pNode->m_Data.m_data.m_fVal;
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

	// Make sure we don't exceed the total length of the polytrail....

	LTVector3f vStart, vEnd;

	vStart = vSave;
	m_pClientDE->Physics()->GetPosition(m_hParent, &vEnd);

	float fLen = (vStart - vEnd).Mag();

	if (fLen > 256.0f)
	{
		// This is too big of a distance, start polytrail at the right spot...

		vSave = m_vPos;
	}
	
	// Add an initial point

	PT_TRAIL_SECTION ts;
	ts.m_vPos    = (vSave + m_vMotionOffset + m_vRandOffset);
	ts.m_tmStart = m_pClientDE->GetTime();
	ts.m_uVal = 0.0f;
	m_collPathPts.AddTail(ts);

	// Go for an initial update

//	CBaseFX::Update(m_pClientDE->GetTime());

	// Add another one

	m_collPathPts.AddTail(ts);
		
	m_tmLastUpdate = m_pClientDE->GetTime();

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CPolyTubeFX
//
//------------------------------------------------------------------

void CPolyTubeFX::Term()
{
	if (m_hObject) m_pClientDE->DeleteObject(m_hObject);
	m_hObject = NULL;

	delete this;
}

//------------------------------------------------------------------
//
//   FUNCTION : Update()
//
//   PURPOSE  : Updates class CPolyTubeFX
//
//------------------------------------------------------------------

bool CPolyTubeFX::Update(float tmCur)
{
	if ((!m_hTexture) && (!m_bLoadFailed))
	{
		m_pClientDE->GetTexInterface()->FindTextureFromName(m_hTexture, m_sPath);

		if (m_hTexture)
		{
			// Retrieve texture dims
			
			m_pClientDE->GetTexInterface()->GetTextureDims(m_hTexture, m_dwWidth, m_dwHeight);
		}
		else
		{
			m_bLoadFailed = true;
		}
	}

	// Base class update first
	
	if (!CBaseFX::Update(tmCur)) return false;

	if ((m_collPathPts.GetSize() < 2) && (m_dwState == FS_SHUTDOWN))
	{
		m_collPathPts.RemoveAll();
		return true;
	}
	
	float tmAddPtInterval = m_tmAddPtInterval;

	if (g_fDetail)
	{
		tmAddPtInterval *= ((1.0f / g_fDetail) * 2.0f);
	}
	
	if ((!(m_dwState & FS_SHUTDOWN)) && 
	    (m_collPathPts.GetSize() < m_nMaxTrailLength) && 
		((m_pClientDE->GetTime() - m_tmLastAddPt > m_tmAddPtInterval) || (m_collPathPts.GetSize() == 1)))
	{
		LTVector vNew = m_vPos + m_vMotionOffset + m_vRandOffset;
		
		// Only add the new point if it's not the same as the last one....

		// Add a new trail section

		PT_TRAIL_SECTION ts;
		ts.m_vPos    = vNew;
		ts.m_tmStart = tmCur;
		
		// Compute u coordinate

		if (m_collPathPts.GetSize())
		{
			LTVector vPrev = m_collPathPts.GetTail()->m_Data.m_vPos;
			float fUPrev = m_collPathPts.GetTail()->m_Data.m_uVal;
			
			float fWidth = (float)m_dwWidth;
			float fScalar = fWidth / m_fTrailWidth;

			ts.m_uVal = fUPrev + ((((vNew - vPrev).Mag()) / fWidth) * fScalar);
		}
		else
		{
			ts.m_uVal = 0.0f;
		}

		m_collPathPts.AddTail(ts);
		
		m_tmLastAddPt = tmCur;
	}

	// Render the tube....

	if (m_collPathPts.GetSize() < 2) return true;

	CLinkListNode<PT_TRAIL_SECTION> *pNode = m_collPathPts.GetHead();

	// Fudge the last point to be the current one...

	m_collPathPts.GetTail()->m_Data.m_vPos = (m_vPos + m_vMotionOffset + m_vRandOffset);

	// Transform the path

	LTMatrix mCam = GetCamTransform(m_pClientDE, m_hCamera);

	while (pNode)
	{
		MatVMul(&pNode->m_Data.m_vTran, &mCam, &pNode->m_Data.m_vPos);	
		
		pNode = pNode->m_pNext;
	}

	// Do some precalculations

	pNode = m_collPathPts.GetHead();

	float fCurU = 0.0f;
	
	while (pNode)
	{
		LTVector vBisector;
		vBisector.z = 0.0f;

		// Compute the midpoint vectors

		if (pNode == m_collPathPts.GetHead())
		{
			LTVector vStart = pNode->m_Data.m_vTran;
			LTVector vEnd   = pNode->m_pNext->m_Data.m_vTran;
			
			vBisector.x = vEnd.y - vStart.y;
			vBisector.y = -(vEnd.x - vStart.x);
		}
		else if (pNode == m_collPathPts.GetTail())
		{
			LTVector vEnd   = pNode->m_Data.m_vTran;
			LTVector vStart = pNode->m_pPrev->m_Data.m_vTran;
			
			vBisector.x = vEnd.y - vStart.y;
			vBisector.y = -(vEnd.x - vStart.x);
		}
		else
		{
			LTVector vPrev  = pNode->m_pPrev->m_Data.m_vTran;
			LTVector vStart = pNode->m_Data.m_vTran;
			LTVector vEnd   = pNode->m_pNext->m_Data.m_vTran;

			float x1 = vEnd.y - vStart.y;
			float y1 = -(vEnd.x - vStart.x);

			float x2 = vStart.y - vPrev.y;
			float y2 = -(vStart.x - vPrev.x);
			
			vBisector.x = (x1 + x2) / 2.0f;
			vBisector.y = (y1 + y2) / 2.0f;
		}
		
		vBisector.Norm(m_fTrailWidth);
		pNode->m_Data.m_vBisector = vBisector;
		
		// Fudge the last point

		if (pNode != m_collPathPts.GetTail())
		{
			// Setup the colour
			
			float r, g, b, a;			
			CalcColour(pNode->m_Data.m_tmStart, tmCur, m_tmSectionLifespan, &r, &g, &b, &a);			

			int ir = (int)(r * 255.0f);
			int ig = (int)(g * 255.0f);
			int ib = (int)(b * 255.0f);
			int ia = (int)(a * 255.0f);

			if (ir < 0) ir = 0;
			if (ig < 0) ig = 0;
			if (ib < 0) ib = 0;
			if (ia < 0) ia = 0;

			if (ir > 255) ir = 255;
			if (ig > 255) ig = 255;
			if (ib > 255) ib = 255;
			if (ia > 255) ia = 255;

			pNode->m_Data.m_red = ir;
			pNode->m_Data.m_green = ig;
			pNode->m_Data.m_blue = ib;
			pNode->m_Data.m_alpha = ia;
		}
		else if (m_collColourKeys.GetSize())
		{
			pNode->m_Data.m_red   = (uint8)m_collColourKeys.GetHead()->m_Data.m_red;
			pNode->m_Data.m_green = (uint8)m_collColourKeys.GetHead()->m_Data.m_green;
			pNode->m_Data.m_blue  = (uint8)m_collColourKeys.GetHead()->m_Data.m_blue;
			pNode->m_Data.m_alpha = (uint8)m_collColourKeys.GetHead()->m_Data.m_alpha;
		}

		pNode = pNode->m_pNext;
	}

	pNode = m_collPathPts.GetHead();

	g_nTris = 0;
	g_nVerts = 0;

	LT_POLYGT3 *pTri = g_pTris;
	LTVector *pVerts = g_pVerts;

	pNode = m_collPathPts.GetHead();

	// Delete any dead nodes

	while (pNode->m_pNext)
	{
		CLinkListNode<PT_TRAIL_SECTION> *pDelNode= NULL;

		if (m_pClientDE->GetTime() >= pNode->m_Data.m_tmStart + m_tmSectionLifespan)
		{
			pDelNode = pNode;
		}
		
		pNode = pNode->m_pNext;

		if (pDelNode) m_collPathPts.Remove(pDelNode);
	}

	pNode = m_collPathPts.GetHead();

	ILTDrawPrim *pDrawPrimLT;
	pDrawPrimLT = m_pClientDE->GetDrawPrim();

	pDrawPrimLT->SetTexture(m_hTexture);

	LTVector vTmp;
	LTMatrix mInvCam = mCam;
	mInvCam.MakeInverseTransform();

	while (pNode->m_pNext)
	{
		LTVector vStart = pNode->m_Data.m_vTran;
		LTVector vEnd   = pNode->m_pNext->m_Data.m_vTran;
	
		LTVector vBisector1 = pNode->m_Data.m_vBisector;
		LTVector vBisector2 = pNode->m_pNext->m_Data.m_vBisector;

		*pVerts ++ = vStart + vBisector1;
		*pVerts ++ = vEnd + vBisector2;
		*pVerts ++ = vEnd - vBisector2;
		*pVerts ++ = vStart - vBisector1;

		uint8 r1 = pNode->m_Data.m_red;
		uint8 g1 = pNode->m_Data.m_green;
		uint8 b1 = pNode->m_Data.m_blue;
		uint8 a1 = pNode->m_Data.m_alpha;
		float u1 = pNode->m_Data.m_uVal + m_uOffset;

		uint8 r2 = pNode->m_pNext->m_Data.m_red;
		uint8 g2 = pNode->m_pNext->m_Data.m_green;
		uint8 b2 = pNode->m_pNext->m_Data.m_blue;
		uint8 a2 = pNode->m_pNext->m_Data.m_alpha;
		float u2 = pNode->m_pNext->m_Data.m_uVal + m_uOffset;		

		SetupVert(pTri, 0, g_pVerts[g_nVerts].x, g_pVerts[g_nVerts].y, g_pVerts[g_nVerts].z, r1, g1, b1, a1, u1, 0.0f);
		SetupVert(pTri, 1, g_pVerts[g_nVerts + 1].x, g_pVerts[g_nVerts + 1].y, g_pVerts[g_nVerts + 1].z, r2, g2, b2, a2, u2, 0.0f);
		SetupVert(pTri, 2, g_pVerts[g_nVerts + 2].x, g_pVerts[g_nVerts + 2].y, g_pVerts[g_nVerts + 2].z, r2, g2, b2, a2, u2, 1.0f);

		pTri ++;
		g_nTris ++;

		SetupVert(pTri, 0, g_pVerts[g_nVerts].x, g_pVerts[g_nVerts].y, g_pVerts[g_nVerts].z, r1, g1, b1, a1, u1, 0.0f);
		SetupVert(pTri, 1, g_pVerts[g_nVerts + 2].x, g_pVerts[g_nVerts + 2].y, g_pVerts[g_nVerts + 2].z, r2, g2, b2, a2, u2, 1.0f);
		SetupVert(pTri, 2, g_pVerts[g_nVerts + 3].x, g_pVerts[g_nVerts + 3].y, g_pVerts[g_nVerts + 3].z, r1, g1, b1, a1, u1, 1.0f);

		pTri ++;
		g_nTris ++;

		g_nVerts += 4;

		pNode = pNode->m_pNext;
	}

	// Increment the offset

	m_uOffset += (tmCur - m_tmLastUpdate) * m_uAdd;

	// Draw the polylist

	if (g_bAppFocus)
	{
		pDrawPrimLT->SetTransformType(DRAWPRIM_TRANSFORM_CAMERA);
		pDrawPrimLT->SetZBufferMode(DRAWPRIM_ZRO);
		pDrawPrimLT->DrawPrim(g_pTris, g_nTris);
		pDrawPrimLT->SetZBufferMode(DRAWPRIM_ZRW);
	}

	m_tmLastUpdate = tmCur;

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : OnRendererShutdown()
//
//   PURPOSE  : Handles app focus issues.....
//
//------------------------------------------------------------------

void CPolyTubeFX::OnRendererShutdown()
{
	if (m_hTexture)
	{
//		m_pClientDE->GetTexInterface()->ReleaseTextureHandle(m_hTexture);
		m_hTexture = NULL;
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : fxGetPolyTubeProps()
//
//   PURPOSE  : Returns a list of properties for this FX
//
//------------------------------------------------------------------

void fxGetPolyTubeProps(CFastList<FX_PROP> *pList)
{
	FX_PROP fxProp;
	
	// Add the base props

	AddBaseProps(pList);

	// Add the class props

	fxProp.Path("Texture", "dtx|...");
	pList->AddTail(fxProp);

	fxProp.Float("TrailWidth", 20.0f);
	pList->AddTail(fxProp);
	
	fxProp.Int("TrailLen", 50);
	pList->AddTail(fxProp);

	fxProp.Float("SectionLifespan", 1.0f);
	pList->AddTail(fxProp);

	fxProp.Float("UAdd", 0.0f);
	pList->AddTail(fxProp);

	fxProp.Float("SectionInterval", 0.1f);
	pList->AddTail(fxProp);
}
