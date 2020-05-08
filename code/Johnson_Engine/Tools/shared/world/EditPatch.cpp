#include "bdefs.h"
#include "EditPatch.h"
#include "editobjects.h"
#include "geomroutines.h"
#include "ltamgr.h"
#include "ltasaveutils.h"

CEditPatch::CEditPatch()
{
	m_nTextureWidth  = 0;
	m_nTextureHeight = 0;

	m_nXResolution = DEFAULT_PATCH_RESOLUTION;
	m_nYResolution = DEFAULT_PATCH_RESOLUTION;

	m_Type = Node_Patch;

	// Set the class name
	SetClassName("Patch");
	
}

CEditPatch::CEditPatch( const CEditPatch& pCopyFrom )
{
	m_nXResolution = DEFAULT_PATCH_RESOLUTION;
	m_nYResolution = DEFAULT_PATCH_RESOLUTION;

	m_Type = Node_Patch;
	DoCopy((CWorldNode*)&pCopyFrom);
}

CEditPatch::~CEditPatch()
{
}

void CEditPatch::Term()
{
}

void CEditPatch::DoCopy(CWorldNode *pOther)
{
	ASSERT(pOther);
	//make sure they are the same type
	ASSERT(pOther->GetType() == Node_Patch);

	//get a patch version
	CEditPatch* pPatch = (CEditPatch*)pOther;

	m_Patch = pPatch->GetPatch();

	//copy over the properties
	SetXResolution(pPatch->GetXResolution());
	SetYResolution(pPatch->GetYResolution());
	SetPatchTextureWidth(pPatch->GetPatchTextureWidth());
	SetPatchTextureHeight(pPatch->GetPatchTextureHeight());

	//let the base class have it's turn
	CWorldNode::DoCopy(pOther);

	//now let the texture stuff get copied
	CopyTextureAttributes(pPatch, NULL);
}			

void CEditPatch::SetupBaseTextureSpace()
{
	//have the base one set it up
	GetP().Init( 1.0f, 0.0f, 0.0f );
	GetQ().Init( 0.0f, 0.0f, 1.0f );
	GetO().Init( 0.0f, 0.0f, 0.0f );
}

//retrieves the normal that should be used for various texture operations
LTVector CEditPatch::GetTextureNormal()
{
	//return CTexturedPlane::GetTextureNormal();
	//should always be the Y axis since the texture is laid out along XZ plane
	return LTVector(0, 1, 0);
}


BOOL CEditPatch::LoadLTA( CLTANode* pNode, CStringHolder *pStringHolder )
{
	CLTANode* pChild;

	if(pChild = CLTAUtil::ShallowFindList(pNode, "xres"))
		SetXResolution(GetInt32(pChild->GetElement(1)));

	if(pChild = CLTAUtil::ShallowFindList(pNode, "yres"))
		SetYResolution(GetInt32(pChild->GetElement(1)));
	
	pChild = CLTAUtil::ShallowFindList(pNode, "xpts");
	uint32 nWidth = GetInt32(pChild->GetElement(1));

	pChild = CLTAUtil::ShallowFindList(pNode, "ypts");
	uint32 nHeight = GetInt32(pChild->GetElement(1));

	//setup the patch
	GetPatch().Init(nWidth, nHeight);

	CLTANode* pPtList = CLTAUtil::ShallowFindList(pNode, "pts");

	if(pPtList)
	{
		//there is a sub list
		pPtList = pPtList->GetElement(1);
	}

	if(pPtList && (pPtList->GetNumElements() >= nWidth * nHeight))
	{
		//now read in all the control points
		for(uint32 nY = 0; nY < nHeight; nY++)
		{
			for(uint32 nX = 0; nX < nWidth; nX++)
			{
				CLTANode* pPt = pPtList->GetElement(nY * nWidth + nX);
				GetPatch().SetControlPoint(nX, nY, LTVector3f(GetFloat(pPt->GetElement(1)), GetFloat(pPt->GetElement(2)), GetFloat(pPt->GetElement(3))));
			}
		}
	}

	//load in the texture information
	CLTANode* pTemp;
	if(pTemp = CLTAUtil::ShallowFindList(pNode, "textureinfo"))
		LoadTextureInfoLTA(pTemp, pStringHolder);

	if(pTemp = CLTAUtil::ShallowFindList(pNode, "texturewidth"))
		SetPatchTextureWidth((uint16)GetInt32(pTemp->GetElement(1)));

	if(pTemp = CLTAUtil::ShallowFindList(pNode, "textureheight"))
		SetPatchTextureHeight((uint16)GetInt32(pTemp->GetElement(1)));

#ifdef DIRECTEDITOR_BUILD
	//update our textures
	UpdateTextureID();
#endif

	return TRUE;
}

void CEditPatch::SaveLTA( CLTAFile* pFile, uint32 level )
{
	//sanity check
	ASSERT(pFile);

	//write out the texture information
	SaveTextureInfoLTA(pFile, level);
	
	//have to custom save the texture flags to prevent it conflicting with the
	//node flags
	PrependTabs(pFile, level);
	pFile->WriteStr("( textureflags " );
	SaveFlagsLTA(pFile, level + 1);
	PrependTabs(pFile, level);
	pFile->WriteStr(" ) " );

	SaveShadeLTA(pFile, level);
	SavePhysicsMaterialLTA(pFile, level);

	//save out the resolution
	PrependTabs(pFile, level);
	pFile->WriteStrF("( xres %d )", GetXResolution());
	PrependTabs(pFile, level);
	pFile->WriteStrF("( yres %d )", GetYResolution());

	//save out the patch dims
	PrependTabs(pFile, level);
	pFile->WriteStrF("( xpts %d )", GetPatch().GetWidth());
	PrependTabs(pFile, level);
	pFile->WriteStrF("( ypts %d )", GetPatch().GetHeight());

	//write out the texture dimensions
	PrependTabs(pFile, level);
	pFile->WriteStrF("( texturewidth %d )", GetPatchTextureWidth());
	PrependTabs(pFile, level);
	pFile->WriteStrF("( textureheight %d )", GetPatchTextureHeight());

	//now print out the points
	PrependTabs(pFile, level);
	pFile->WriteStr("( pts (");

	for(uint32 nY = 0; nY < GetPatch().GetHeight(); nY++)
	{
		for(uint32 nX = 0; nX < GetPatch().GetWidth(); nX++)
		{
			PrependTabs(pFile, level + 1);
			LTVector3f vPt = GetPatch().GetControlPoint(nX, nY);
			pFile->WriteStrF("( pt %f %f %f )", vPt.x, vPt.y, vPt.z);
		}
	}

	//close out the list
	PrependTabs(pFile, level);
	pFile->WriteStr(") )");
}

void CEditPatch::Rotate(LTMatrix &mMatrix, LTVector &vCenter)
{
	CWorldNode::Rotate(mMatrix, vCenter);
}

//builds up a transformation matrix that includes the translation and
//orientation of this patch
LTMatrix CEditPatch::GetTransform()
{
	LTMatrix mTrans, mRot;

	mTrans.Identity();
	mTrans.SetTranslation(GetPos());

	::gr_SetupMatrixEuler(GetOr(), mRot.m);

	return mTrans * mRot;
}

//gets the bounding box of the patch.
void CEditPatch::GetBoundingBox(LTVector& vMin, LTVector& vMax)
{
	LTVector3f vPatchMax, vPatchMin;
	GetPatch().GetBoundingBox(vPatchMin, vPatchMax);
	vMin = vPatchMin;
	vMax = vPatchMax;
}

/************************************************************************/
// Returns the upper left corner of an object/brush
CVector CEditPatch::GetUpperLeftCornerPos()
{
	//get the bounding box of the patch
	LTVector3f vMin, vMax;

	if(GetPatch().GetBoundingBox(vMin, vMax))
	{
		//return the upper left
		return LTVector(vMin.x, vMax.y, vMax.z);
	}
	else
	{
		//no points. Just return a dummy vector
		return LTVector(0, 0, 0);
	}
}

//sets the position of this node
void CEditPatch::SetPos(const LTVector& v)
{
	//translate all points by the delta
	LTVector vDelta = v - GetPos() ;

	for(uint32 nX = 0; nX < GetPatch().GetWidth(); nX++)
	{
		for(uint32 nY = 0; nY < GetPatch().GetHeight(); nY++)
		{
			GetPatch().SetControlPoint(nX, nY, GetPatch().GetControlPoint(nX, nY) + vDelta);
		}
	}

	CWorldNode::SetPos(v);
}

//sets the orientation of this node
void CEditPatch::SetOr(const LTVector& v)
{
	//transform all points back from the original orientation, then transform them to
	//the new orientation
	LTMatrix mOldOr, mNewOr;
	::gr_SetupMatrixEuler(GetOr(), mOldOr.m);
	::gr_SetupMatrixEuler(v, mNewOr.m);


	//transpose the old orientation making it the inverse
	MatTranspose(&mOldOr);

	//combine the matrices for performance
	LTMatrix mFinalTrans = mNewOr * mOldOr;

	//cache the position
	LTVector vPos = GetPos();

	for(uint32 nX = 0; nX < GetPatch().GetWidth(); nX++)
	{
		for(uint32 nY = 0; nY < GetPatch().GetHeight(); nY++)
		{
			GetPatch().SetControlPoint(nX, nY, mFinalTrans * (GetPatch().GetControlPoint(nX, nY) - vPos) + vPos);
		}
	}

	CWorldNode::SetOr(v);
}

void CEditPatch::SetXResolution(uint32 nVal)
{
	m_nXResolution = LTMAX(1, LTMIN(MAX_PATCH_RESOLUTION, nVal));
}

void CEditPatch::SetYResolution(uint32 nVal)
{
	m_nYResolution = LTMAX(1, LTMIN(MAX_PATCH_RESOLUTION, nVal));
}

void CEditPatch::SetTextureSpace(const LTVector& newO, const LTVector& newP, const LTVector& newQ)
{
	CTexturedPlane::SetTextureSpace(GetTextureNormal(), newO, newP, newQ);
}
