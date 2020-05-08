#ifndef __EDITPATCH_H__
#define __EDITPATCH_H__


// Includes....
#include "worldnode.h"
#include "bezierpatch.h"
#include "texturedplane.h"

#define MAX_PATCH_RESOLUTION		256
#define DEFAULT_PATCH_RESOLUTION	8

//this defines how much s and t are stretched to generate u and v. This is to prevent
//the P and Q vectors from having to be so insanely large
#define PATCH_TEXTURE_SCALE			256

class CLTANode;
class CLTAFile;

class CEditPatch : public CWorldNode, public CTexturedPlane
{
	public:

								CEditPatch();
								CEditPatch( const CEditPatch& pCopyFrom );
		virtual					~CEditPatch();
		
		virtual void			Term();
		virtual CWorldNode*		AllocateSameKind()	{return new CEditPatch;}
		virtual void			DoCopy(CWorldNode *pOther);			


	// Loading/saving.
	public:

		BOOL					LoadLTA( CLTANode* pNode, CStringHolder *pStringHolder );
		void					SaveLTA( CLTAFile* pFile, uint32 level );

		virtual	void			Rotate(LTMatrix &mMatrix, LTVector &vCenter);

		//builds up a transformation matrix that includes the translation and
		//orientation of this patch
		LTMatrix				GetTransform();

		//gets the bounding box of the patch.
		void					GetBoundingBox(LTVector& vMin, LTVector& vMax);

		//gets the upper left corner of the patch
		CVector					GetUpperLeftCornerPos();

		//sets the position of this node
		void					SetPos(const LTVector& v);

		//sets the orientation of this node
		void					SetOr(const LTVector& v);

	//accessors
	public:

		CBezierPatch&			GetPatch()						{return m_Patch;}
		const CBezierPatch&		GetPatch() const				{return m_Patch;}

		uint32					GetXResolution() const			{return m_nXResolution;}
		uint32					GetYResolution() const			{return m_nYResolution;}

		void					SetXResolution(uint32 nVal);
		void					SetYResolution(uint32 nVal);

		uint16					GetPatchTextureWidth() const		{return m_nTextureWidth;}
		uint16					GetPatchTextureHeight() const		{return m_nTextureHeight;}

		void					SetPatchTextureWidth(uint16 nWid)	{m_nTextureWidth = nWid;}
		void					SetPatchTextureHeight(uint16 nHgt)	{m_nTextureHeight = nHgt;}

	//texture operations
	public:

		//retrieves the normal that should be used for various texture operations
		LTVector				GetTextureNormal();

		void					SetupBaseTextureSpace();

		void					SetTextureSpace(const LTVector& newO, const LTVector& newP, const LTVector& newQ);


	protected:

		//these are the texture dimensions for tesselation
		uint16					m_nTextureWidth;
		uint16					m_nTextureHeight;
		
		//for tesselation, this holds the number of times it is to be tesselated
		//on each side
		uint32					m_nXResolution;
		uint32					m_nYResolution;

		//the actual patch data
		CBezierPatch			m_Patch;
};


#endif  // __EDITPATCH_H__
