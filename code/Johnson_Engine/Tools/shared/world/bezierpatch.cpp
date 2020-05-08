#include "bezierpatch.h"

//constructors
CBezierPatch::CBezierPatch() :
	m_nWidth(0),
	m_nHeight(0),
	m_pvControlPts(NULL)
{
}

CBezierPatch::CBezierPatch(const CBezierPatch& rhs) :
	m_nWidth(0),
	m_nHeight(0),
	m_pvControlPts(NULL)
{
	*this = rhs;
}

CBezierPatch::CBezierPatch(uint32 nWidth, uint32 nHeight) :
	m_nWidth(0),
	m_nHeight(0),
	m_pvControlPts(NULL)
{
	Init(nWidth, nHeight);
}

CBezierPatch::~CBezierPatch()
{
	Free();
}

void CBezierPatch::Free()
{
	delete [] m_pvControlPts;
	m_pvControlPts = NULL;

	m_nWidth  = 0;
	m_nHeight = 0;
}

//operator equals
const CBezierPatch&	CBezierPatch::operator=(const CBezierPatch& rhs)
{
	//resize our patch
	if(Init(rhs.GetWidth(), rhs.GetHeight()))
	{
		//copy over the control points
		for(uint32 nX = 0; nX < GetWidth(); nX++)
		{
			for(uint32 nY = 0; nY < GetHeight(); nY++)
			{
				SetControlPoint(nX, nY, rhs.GetControlPoint(nX, nY));
			}
		}
	}

	return *this;
}

//creates a new patch based upon the given dimensions
bool CBezierPatch::Init(uint32 nWidth, uint32 nHeight)
{
	//clean up any old patches
	Free();

	//see if the sizes are valid
	if( (nWidth == 0) || (nHeight == 0) || 
		(nWidth > MAX_DIMENSION) || (nHeight > MAX_DIMENSION))
	{
		//invalid sizes
		return false;
	}

	//allocate memory
	m_pvControlPts = new LTVector3f[nWidth * nHeight];

	//see if the allocation worked
	if(m_pvControlPts == NULL)
	{
		return false;
	}

	//it worked, save the new size
	m_nWidth  = nWidth;
	m_nHeight = nHeight;
	
	return true;
}

//gets the width of the patch
uint32 CBezierPatch::GetWidth() const
{
	return m_nWidth;
}

//gets the height of the patch
uint32 CBezierPatch::GetHeight() const
{
	return m_nHeight;
}

//determines the position of the control point
const LTVector3f& CBezierPatch::GetControlPoint(uint32 nX, uint32 nY) const
{
	//sanity check
	ASSERT(nX < GetWidth());
	ASSERT(nY < GetHeight());

	return m_pvControlPts[nY * GetWidth() + nX];
}

//sets the position of the control point
void CBezierPatch::SetControlPoint(uint32 nX, uint32 nY, const LTVector3f& vPos)
{
	//sanity check
	ASSERT(nX < GetWidth());
	ASSERT(nY < GetHeight());

	m_pvControlPts[nY * GetWidth() + nX] = vPos;
}

//evalutates the patch for the specified position. Note that 
// 0 <= fU, fV <= 1
LTVector3f CBezierPatch::Evaluate(float fU, float fV) const
{
	//have double accuracy while computing the point
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;

	//the basis at the point
	double fBasisU, fBasisV, fBasis;

	for(uint32 nX = 0; nX < GetWidth(); nX++)
	{
		for(uint32 nY = 0; nY < GetHeight(); nY++)
		{
			fBasisU = BernsteinBasis(nX, GetWidth() - 1, fU);
			fBasisV = BernsteinBasis(nY, GetHeight() - 1, fV); 
			fBasis = fBasisU * fBasisV;
		
			x += GetControlPoint(nX, nY).x * fBasis;
			y += GetControlPoint(nX, nY).y * fBasis;
			z += GetControlPoint(nX, nY).z * fBasis;
		}
	}

	//convert this into a vector to return
	LTVector3f rv((float)x, (float)y, (float)z);

	return rv;
}

//calculates the bernstein basis value. nPt is the point out of nNumPts
//Note: 0 <= nPt <= nNumPts, and 0 <= fVal <= 1
double CBezierPatch::BernsteinBasis(uint32 nPt, uint32 nNumPts, double fVal)
{
	//a list of factorials up until 8!
	static uint32 snFactorials[] = { 1, 1, 2, 6, 24, 120, 720, 5040, 40320 };

	//first calculate the nNumPts choose nPt
	
	//note that this can be done with integer math since it follows pascal's triangle,
	//which is made up entirely of integers. Also, if this goes over 8, this will cause
	//overflow in the integers.
	uint32 nChoose = snFactorials[nNumPts] / (snFactorials[nPt] * snFactorials[nNumPts - nPt]);

	//now we calculate the two final value

	return (double)nChoose * pow(fVal, nPt) * pow(1 - fVal, nNumPts - nPt);
}

//this will get the bounding box that encompasses the patch's control points. This
//will return false if there are no points in the patch, and the points will be the
//zero vector
bool CBezierPatch::GetBoundingBox(LTVector3f& vMin, LTVector3f& vMax) const
{
	//see if we have no points
	if(m_pvControlPts == NULL)
	{
		vMin.Init(0, 0, 0);
		vMax.Init(0, 0, 0);
		return false;
	}

	//we have points. Build the list
	
	//init to the first point
	vMin = m_pvControlPts[0];
	vMax = vMin;

	//now get the max and min
	for(uint32 nCurrPt = 1; nCurrPt < m_nWidth * m_nHeight; nCurrPt++)
	{
		VEC_MIN(vMin, vMin, m_pvControlPts[nCurrPt]);
		VEC_MAX(vMax, vMax, m_pvControlPts[nCurrPt]);
	}

	//success
	return true;
}

//this will get the half dims of the bounding box that encompasses the control points
LTVector3f CBezierPatch::GetDims() const
{
	//get the bounding box
	LTVector3f vMin, vMax;
	GetBoundingBox(vMin, vMax);

	//get the half dimensions
	return (vMax - vMin) / 2;
}

