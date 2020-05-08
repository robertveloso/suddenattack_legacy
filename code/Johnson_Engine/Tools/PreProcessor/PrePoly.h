//------------------------------------------------------------------
//
//	FILE	  : PrePoly.h
//
//	PURPOSE	  : Defines the CPrePoly class, which is used 
//              in the preprocessing phase.
//
//	CREATED	  : 2nd May 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

#ifndef __PREPOLY_H__
#define __PREPOLY_H__

	// Defines....	
	#define CPrePolyPtrArray			CMoArray<CPrePoly*>
	
	
	// Includes....
	#include "bdefs.h"
	#include "preplane.h"
	#include "presurface.h"
	#include "prebasepoly.h"


	// Defines....
	class CPreWorld;
	class LMMWorld;

	
	#define PP_POSTREMOVE						(1<<0)	// Used in various places..
	#define PP_DONTPRESERVEORIGINAL				(1<<1)	// This poly was overlapping another poly
														// so you can't use m_pOriginalBrushPoly.
	#define PP_MAYBEDONTPRESERVEORIGINAL		(1<<2)	// Controls if PP_DONTPRESERVEORIGINAL can be set.
	#define PP_DONOTREMOVE						(1<<3)  // Make sure this polygon doesn't get removed

	class CPrePolyFragments;

	class CPrePoly : public CGLLNode
	{
		public:

			// Constructor
								CPrePoly();
			virtual				~CPrePoly();


		public:

			// Member functions
			BOOL				Init() { return TRUE; }
			void				Term();
			
			void CopySplitAttributes(const CPrePoly *pPoly)
			{
				m_pSurface				= pPoly->m_pSurface;
				m_pOriginalBrushPoly	= pPoly->m_pOriginalBrushPoly;
				m_PPFlags				= pPoly->m_PPFlags;
			}

			// These can be used to iterate both vertex lists in 1 loop.
			inline uint32		NumVertsAndTVerts() const {return NumVerts() + m_TVertices.GetSize();}
			
			inline CPolyVert&	GetVertOrTVert(uint32 index)
			{
				if(index >= NumVerts())
				{
					index -= NumVerts();
					ASSERT(index < m_TVertices.GetSize());
					return m_TVertices[index];
				}
				else
				{
					return Vert(index);
				}
			}

			inline const CPolyVert&	GetVertOrTVert(uint32 index) const { return (const_cast<CPrePoly*>(this))->GetVertOrTVert(index); }

			uint32&				VertOrTVertIndex(uint32 index)
			{
				if(index >= NumVerts())
				{
					index -= NumVerts();
					ASSERT(index < m_TVertices.GetSize());
					return m_TVertices[index].m_Index;
				}
				else
				{
					return Index(index);
				}
			}

			inline uint32&		VertOrTVertIndex(uint32 index) const { return (const_cast<CPrePoly*>(this))->VertOrTVertIndex(index); }

			// Sets up the poly lightmap origin.  If pPolyO is NULL, it uses m_PolyO.
			void				FindTextureOrigin(PVector *pPolyO, BOOL bAdjust, float lmGridSize);
			
			void				CalculateTextureSize(BOOL bCapSizes, 
													float maxTextureSize, float lmGridSize,
													uint16 &xPixels, uint16 &yPixels);

			void				RemoveCollinearVertices();
			BOOL				PostJoinFixup(PReal convexThreshold);

			// Make a copy of the poly.
			CPrePoly*			Clone() const;

			const char*&		TextureName(uint32 nTex)		{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_pTextureName; }
			const char*			TextureName(uint32 nTex) const	{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_pTextureName; }

			uint16				TextureWidth(uint32 nTex) const  { ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_TextureWidth; }
			uint16				TextureHeight(uint32 nTex) const { ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_TextureHeight; }
 
			PVector&			O()						{ ASSERT(GetSurface()); return GetSurface()->O; }
			const PVector&		O() const				{ ASSERT(GetSurface()); return GetSurface()->O; }
			PVector&			P()						{ ASSERT(GetSurface()); return GetSurface()->P; }
			const PVector&		P() const				{ ASSERT(GetSurface()); return GetSurface()->P; }
			PVector&			Q()						{ ASSERT(GetSurface()); return GetSurface()->Q; }
			const PVector&		Q()	const				{ ASSERT(GetSurface()); return GetSurface()->Q; }


			PVector&			InverseP()				{ ASSERT(GetSurface()); return GetSurface()->InverseP; }
			const PVector&		InverseP() const		{ ASSERT(GetSurface()); return GetSurface()->InverseP; }
			PVector&			InverseQ()				{ ASSERT(GetSurface()); return GetSurface()->InverseQ; }
			const PVector&		InverseQ() const		{ ASSERT(GetSurface()); return GetSurface()->InverseQ; }

			PVector&			TextureO(uint32 nTex)		{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_TextureO; }
			const PVector&		TextureO(uint32 nTex) const	{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_TextureO; }
			PVector&			TextureP(uint32 nTex)		{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_TextureP; }
			const PVector&		TextureP(uint32 nTex) const	{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_TextureP; }
			PVector&			TextureQ(uint32 nTex)		{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_TextureQ; }
			const PVector&		TextureQ(uint32 nTex) const	{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_TextureQ; }

			PVector&			InverseTextureP(uint32 nTex)		{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_InverseTextureP; }
			const PVector&		InverseTextureP(uint32 nTex) const	{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_InverseTextureP; }
			PVector&			InverseTextureQ(uint32 nTex)		{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_InverseTextureQ; }
			const PVector&		InverseTextureQ(uint32 nTex) const	{ ASSERT(GetSurface()); return GetSurface()->m_Texture[nTex].m_InverseTextureQ; }

			PVector&			Normal()				{ ASSERT(GetPlane()); return GetPlane()->m_Normal; }
			const PVector&		Normal() const			{ ASSERT(GetPlane()); return GetPlane()->m_Normal; }
			PReal&				Dist()					{ ASSERT(GetPlane()); return GetPlane()->m_Dist; }
			const PReal&		Dist() const			{ ASSERT(GetPlane()); return GetPlane()->m_Dist; }

			PVector&			PolyO()					{ return m_PolyO; }
			const PVector&		PolyO() const			{ return m_PolyO; }

			CPreSurface*		GetSurface()			{ return m_pSurface; }
			const CPreSurface*	GetSurface() const		{ return m_pSurface; }
			CPrePlane*			GetPlane()				{ ASSERT(GetSurface()); return GetSurface()->m_pPlane; }
			const CPrePlane*	GetPlane() const		{ ASSERT(GetSurface()); return GetSurface()->m_pPlane; }

			uint32&				Index(uint32 i)			{ ASSERT(i<NumVerts()); return Vert(i).m_Index; }
			const uint32&		Index(uint32 i)	const	{ ASSERT(i<NumVerts()); return Vert(i).m_Index; }

			uint32&				NextIndex(uint32 i)			{ return Index((i+1) % NumVerts());}
			const uint32&		NextIndex(uint32 i) const	{ return Index((i+1) % NumVerts());}

			inline uint32		GetSurfaceFlags() const	{ return GetSurface()->m_Flags;}

			// Set the PostRemove flag.
			inline BOOL			GetPostRemove() const	{ return !!(m_PPFlags & PP_POSTREMOVE);}
			inline void			SetPostRemove(BOOL bSet){ if(bSet) m_PPFlags |= PP_POSTREMOVE; else m_PPFlags &= ~PP_POSTREMOVE;}

			// Get/set PP_ flags.
			inline uint32		GetPPFlags() const		{ return m_PPFlags;}
			inline void			SetPPFlags(uint32 flags){ m_PPFlags = flags;}

			// Find the center of the polygon
			inline LTVector CalcCenter() const
			{
				LTVector vRV(0, 0, 0);

				for(uint32 nCurrVert = 0; nCurrVert < NumVerts(); nCurrVert++)
				{
					vRV += Pt(nCurrVert);
				}

				vRV /= (float)NumVerts();
				return vRV;
			}

			// Extents tracking 
			inline void CalcExtents(PReal fExpand = 0.0f) const
			{
				m_vExtentsMin = Pt(0);
				m_vExtentsMax = m_vExtentsMin;

				for (uint32 nVertLoop = 1; nVertLoop < NumVerts(); ++nVertLoop)
				{
					VEC_MIN(m_vExtentsMin, m_vExtentsMin, Pt(nVertLoop));
					VEC_MAX(m_vExtentsMax, m_vExtentsMax, Pt(nVertLoop));
				}

				m_vExtentsMin -= fExpand;
				m_vExtentsMax += fExpand;
			}

			inline bool OverlapExtents(const CPrePoly &pOtherPoly) const
			{
				return	(pOtherPoly.m_vExtentsMin.x < m_vExtentsMax.x) && (pOtherPoly.m_vExtentsMax.x > m_vExtentsMin.x) &&
						(pOtherPoly.m_vExtentsMin.y < m_vExtentsMax.y) && (pOtherPoly.m_vExtentsMax.y > m_vExtentsMin.y) &&
						(pOtherPoly.m_vExtentsMin.z < m_vExtentsMax.z) && (pOtherPoly.m_vExtentsMax.z > m_vExtentsMin.z);
			}

			const CPrePolyFragments *GetFragments() const { return m_pFragments; }
			CPrePolyFragments *GetFragments() { return m_pFragments; }


//DEdit only information for rendering the lightmaps on the polygons
#ifdef DIRECTEDITOR_BUILD
		public:
			void				*m_pUser;		// Its Direct3dTexture.
			void				*m_pUser2;		
#endif
		
		public:

			// Vertices with t-junctions fixed.
			CMoArray<CPolyVert>	m_TVertices;

			LMMWorld			*m_pWorld;		// Used by LightMapMaker.

			// The original poly this poly comes from.. after the BSP is generated,
			// all the polies that were split up in the BrushToWorld module are replaced
			// with their original polies.
			// If PP_DONTPRESERVEORIGINAL is set, this can't be used.
			CPrePoly			*m_pOriginalBrushPoly;

			CPrePoly			*m_pReplacement; // Used in BSP generation.

			// Used in the BSP generator's CPolyList.
			CMLLNode			m_PolyListNode;

			// Used for lightmapping .. tells the upper-left corner of the polygon in texture space.
			PVector				m_PolyO;

			// Extents tracking
			// Note : This is mutable because it's not really part of the polygon,
			// and everything that's going to be doing extents overlap checking might
			// require a different amount of overlap.  So it's more like processing-
			// specific state data associated with this specific poly.
			mutable PVector		m_vExtentsMin;
			mutable PVector		m_vExtentsMax;

		protected:
			
			// Combination of PP_ flags.
			uint32				m_PPFlags;


		public:
			
			// Width and height of lightmap data for this polygon.
			uint16				m_LMWidth, m_LMHeight;

			CPreSurface			*m_pSurface;
			
			// Which world it comes from.  Used by lighting and while saving files.
			uint32				m_WorldIndex;

			// This poly's index into the world's array.
			uint32				m_Index;

			CPrePolyFragments	*m_pFragments;

			// The ID for representing the "name" of this polygon
			uint32				m_nName;

			//this must come last
			BASEPOLY_MEMBER()

	};


	typedef CGLinkedList<CPrePoly*> CPrePolyList;
	typedef CMoArray<CPrePoly*> PrePolyArray;

#endif

