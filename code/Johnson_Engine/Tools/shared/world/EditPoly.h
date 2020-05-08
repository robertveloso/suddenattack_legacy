//
//   (c) 1998-1999 Monolith Productions, Inc.  All Rights Reserved
//
// ---------------------------------------------------------------
//
//------------------------------------------------------------------
//
//	FILE	  : EditPoly.h
//
//	PURPOSE	  : Defines the CEditPoly class.
//
//	CREATED	  : October 5 1996
//
//
//------------------------------------------------------------------

#ifndef __EDITPOLY_H__
	#define __EDITPOLY_H__


	// Includes....
	#include "basepoly.h"
	#include "editregion.h"
	#include "texturedplane.h"


	// Defines....
	class CRegionView;
	class CPrePoly;
	class CLTANode;
	class CLTAFile;


	class CEditPoly : public CBasePoly
	{
		public:

			enum	{ NUM_TEXTURES	= 2 };

								CEditPoly();
								CEditPoly( CEditPoly *pCopyFrom );
								CEditPoly( CEditBrush *pBrush );
								~CEditPoly();
		
			void				Term();

			CTexturedPlane&			GetTexture(uint32 nTexture)			{ assert(nTexture < NUM_TEXTURES); return m_Textures[nTexture]; }
			const CTexturedPlane&	GetTexture(uint32 nTexture) const	{ assert(nTexture < NUM_TEXTURES); return m_Textures[nTexture]; }

		private:

			void				CommonConstructor( );

		// Loading/saving.
		public:

			BOOL				LoadEditPolyLTA( CLTANode* pNode, CStringHolder *pStringHolder );
			void				SaveEditPolyLTA( CLTAFile* pFile, uint32 level );

		// Functionality.
		public:

			void				CopyAttributes( CBasePoly *pOther, CStringHolder *pStringHolder=NULL );
			
			// Decrements all indices >= index.
			void				DecrementPoints( uint32 index );

			// Tells if and where the ray intersects the poly.
			BOOL				IntersectRay( CEditRay &ray, CReal &t, BOOL bBackface );

			BOOL				CopyEditPoly( CEditPoly *pPoly, BOOL bCopyIndices=TRUE, CStringHolder *pStringHolder=NULL );
			
			// Makes the given index the first one.
			// NOTE:  index is NOT an index into m_Indices, it's a VALUE in m_Indices.
			void				SetFirstVert( uint32 index );
			
			void				Flip();
			
			void				SetupBaseTextureSpace(uint32 nTex);

			void				SetTextureSpace(uint32 nTex, const LTVector& newO, const LTVector& newP, const LTVector& newQ);

			// sets up OPQs based on uv coordinates of the first 3 verts
			bool				SetUVTextureSpace(uint32 nTex, const float* coords, const int texWidth, const int texHeight );

			//determines if this polygon is concave or not
			bool				IsConcave();

			//determines if this polygon lies entirely within a single plane
			bool				IsCoplanar();

			//determines the surface area of the polygon
			CReal				GetSurfaceArea();

			//retrieves the normal that should be used for various texture operations
			LTVector			GetTextureNormal()			{return Normal();}

		public:

			// Bsp stuff.
			CEditPoly				*m_Sides[2];

		private:

			//the textures
			CTexturedPlane			m_Textures[NUM_TEXTURES];
			

#ifndef DIRECTEDITOR_BUILD
		//Preprocessor specific data
		public:

			// The CPrePoly that was built directly from this poly.
			CPrePoly				*m_pPrePoly;

			// User data pointer .. used by the preprocessor to point to its surface.
			void					*m_pUser1;

			// Used during preprocessing.
			WORD					m_iPlane;
#endif


	};


	typedef CMoArray<CEditPoly*> CEditPolyArray;


#endif  // __EDITPOLY_H__



