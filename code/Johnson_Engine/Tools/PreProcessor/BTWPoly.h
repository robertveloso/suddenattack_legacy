#ifndef __BTWPOLY_H__
#define __BTWPOLY_H__

#ifndef __PREBASEPOLY_H__
#	include "prebasepoly.h"
#endif

#ifndef __PRESURFACE_H__
#	include "presurface.h"
#endif

#ifndef __PREPLANE_H__
#	include "preplane.h"
#endif

	class CBTWPoly : public CGLLNode
	{
		public:

			 CBTWPoly() : m_pPrePoly(NULL), m_pSurface(NULL)	{}
			~CBTWPoly() {}

			void CopySplitAttributes(CBTWPoly *pPoly)
			{
				m_pSurface = pPoly->m_pSurface;
				m_pPrePoly = pPoly->m_pPrePoly;
			}

			CPreSurface*		GetSurface()	{return m_pSurface;}
			CPrePlane*			GetPlane()		{ASSERT(GetSurface()); return GetSurface()->m_pPlane;}
			PVector&			Normal()		{ASSERT(GetPlane()); return GetPlane()->m_Normal;}


			// The world poly that it came from.
			CPrePoly			*m_pPrePoly;

			CPreSurface			*m_pSurface;

			uint32				m_nName;

			//this must come last
			BASEPOLY_MEMBER()

	};


#endif // __BTWPOLY_H__

