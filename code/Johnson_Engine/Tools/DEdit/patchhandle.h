#ifndef __PATCHHANDLE_H__
#define __PATCHHANDLE_H__

#ifndef __BEZIERPATCH_H__
#	include "bezierpatch.h"
#endif

class CPatchHandle
{
public:

	CPatchHandle(CBezierPatch* pPatch = NULL, uint32 nX = 0, uint32 nY = 0) : 
			m_pPatch(pPatch), m_nPtX(nX), m_nPtY(nY), m_bSelected(false)	{}

	//gets the position of this handle
	LTVector GetPos() const
	{
		ASSERT(m_pPatch);
		return ((LTVector)m_pPatch->GetControlPoint(m_nPtX, m_nPtY));
	}

	//sets the position of this hanlde
	void SetPos(LTVector vPos)
	{
		ASSERT(m_pPatch);
		m_pPatch->SetControlPoint(m_nPtX, m_nPtY, vPos);
	}

	//determines if this handle is selected or not
	bool IsSelected() const			{return m_bSelected;}

	//sets this hanlde as selected or not
	void SetSelected(bool bSel)		{m_bSelected = bSel;}


private:

	//information to identify which point on which patch this is
	CBezierPatch*	m_pPatch;
	uint32			m_nPtX;
	uint32			m_nPtY;

	bool			m_bSelected;

};

#endif

