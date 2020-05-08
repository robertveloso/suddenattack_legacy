#ifndef __BEZIERPATCH_H__
#define __BEZIERPATCH_H__

#include "ltbasedefs.h"

class CBezierPatch
{
public:

	//maximum dimensions for a patch. Cannot be larger than this on a side. Note:
	//if this changes, the function BernsteinBasis must be changed to allow
	//larger values
	enum	{	MAX_DIMENSION	= 8	};

	//constructors
	CBezierPatch();
	CBezierPatch(const CBezierPatch& rhs);
	CBezierPatch(uint32 nWidth, uint32 nHeight);

	~CBezierPatch();

	//creates a new patch based upon the given dimensions
	bool				Init(uint32 nWidth, uint32 nHeight);

	//gets the width of the patch
	uint32				GetWidth() const;

	//gets the height of the patch
	uint32				GetHeight() const;

	//determines the position of the control point
	const LTVector3f&	GetControlPoint(uint32 nX, uint32 nY) const;

	//sets the position of the control point
	void				SetControlPoint(uint32 nX, uint32 nY, const LTVector3f& vPos);

	//evalutates the patch for the specified position. Note that 
	// 0 <= fU, fV <= 1
	LTVector3f			Evaluate(float fU, float fV) const;

	//this will get the bounding box that encompasses the patch's control points. This
	//will return false if there are no points in the patch, and the points will be the
	//zero vector
	bool				GetBoundingBox(LTVector3f& vMin, LTVector3f& vMax) const;

	//this will get the half dims of the bounding box that encompasses the control points
	LTVector3f			GetDims() const;
	
	//operator equals
	const CBezierPatch&	operator=(const CBezierPatch& rhs);

	//calculates the bernstein basis value. nPt is the point out of nNumPts
	//Note: 0 <= nPt <= nNumPts, and 0 <= fVal <= 1
	static double		BernsteinBasis(uint32 nPt, uint32 nNumPts, double fVal);

private:

	//frees the object to an initial state
	void				Free();

	//dimensions
	uint32				m_nWidth;
	uint32				m_nHeight;

	//the array of control points
	LTVector3f*			m_pvControlPts;

};

#endif
