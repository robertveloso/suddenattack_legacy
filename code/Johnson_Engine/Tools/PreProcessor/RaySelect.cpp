
#include "bdefs.h"
#include "preworld.h"
#include "editpoly.h"
#include "editregion.h"
#include "pregeometry.h"
#include "geomroutines.h"



uint32 DoRaySelect(CPreMainWorld *pMainWorld, CEditRegion *pRegion)
{
	uint32			i, nSelections, iWorld;
	PVector			rayPos, rayDir, iPoint;
	CBaseEditObj	*pObj;
	LTVector		vUp, vRight, vForward;
	CRotationProp	*pRot;
	GPOS			pos;
	CPrePoly		*pPoly, *pMinPoly;
	PReal			t, minDist;
	CPreWorld		*pWorld;
	

	for(iWorld=0; iWorld < pMainWorld->m_WorldModels; iWorld++)
	{
		pWorld = pMainWorld->m_WorldModels[iWorld];

		nSelections = 0;
		for(i=0; i < pMainWorld->m_Objects; i++)
		{
			pObj = pMainWorld->m_Objects[i];

			if(strcmp(pObj->GetClassName(), "RaySelecter") == 0)
			{
				rayPos = pObj->GetPos();
				if(pRot = (CRotationProp*)pObj->m_PropList.GetProp("Rotation"))
				{
					gr_GetEulerVectors(pRot->GetEulerAngles(),
						vRight, vUp, vForward);
					rayDir = vForward;

					minDist = 1000000.0f;
					pMinPoly = NULL;

					// Find the closest poly intersection.
					for(pos=pWorld->m_Polies; pos; )
					{
						pPoly = pWorld->m_Polies.GetNext(pos);

						// N * (P + Rt) - D = 0
						// t = (D - NP) / NR

						t = (pPoly->Dist() - pPoly->Normal().Dot(rayPos)) / pPoly->Normal().Dot(rayDir);
						if(t > 0.1f && t < minDist)
						{
							iPoint = rayPos + rayDir * t;

							if(InsideConvex(pPoly, iPoint))
							{
								pMinPoly = pPoly;
							}
						}
					}

					if(pMinPoly)
					{
						pRegion->SelectNode(pMinPoly->m_pSurface->m_pOriginalPoly->m_pBrush);
						++nSelections;
					}
				}
			}
		}
	}

	return nSelections;
}








