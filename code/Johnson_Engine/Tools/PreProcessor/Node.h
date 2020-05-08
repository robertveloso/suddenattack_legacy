
#ifndef __NODE_H__
#define __NODE_H__


	#include "preplane.h"
	

	class CPreLeaf;
	class CPrePoly;


	// Special node indices.
	#define NODE_IN		((CNode*)-1)
	#define NODE_OUT	((CNode*)-2)


	// Node flags.
	#define NODEFLAG_CANTSEETHRUPORTAL	(1<<0)	// Leaves with this node in them can be seen
												// into but not out of.
	#define NODEFLAG_VISIBLE			(1<<1)	// Used while compressing PVS data.


	class CNode : public CGLLNode
	{
		public:

						CNode()
						{
							m_pPlane				= NULL;
							m_pPoly					= NULL;
							m_pLeaf					= NULL;
							m_pParentNode			= NULL;
							m_Sides[0] = m_Sides[1] = NULL;
							m_Flags					= 0;
						}

			void		Verify()
			{
				if(this == NODE_IN || this == NODE_OUT)
					return;
				ASSERT(m_Sides[BackSide] != NODE_IN);
				ASSERT(m_Sides[FrontSide] != NODE_OUT);
				m_Sides[BackSide]->Verify();
				m_Sides[FrontSide]->Verify();
			}

			CPrePlane*	GetPlane()	{ return m_pPlane; }

			PVector&	Normal()	{ ASSERT(GetPlane()); return GetPlane()->m_Normal; }
			PReal&		Dist()		{ ASSERT(GetPlane()); return GetPlane()->m_Dist; }

			void		ClearPolyPostRemove();
			void		ClearNodePostRemove();

			// Recurses and sets up parenthood.
			void		SetParenthood(CNode *pParent=NULL);

			// Used while saving.
			uint32		m_Index;
			uint16		m_Flags;	// NODEFLAG_.

			CPrePlane	*m_pPlane;
			CPrePoly	*m_pPoly;
			CPreLeaf	*m_pLeaf;

			CNode		*m_pParentNode;
			CNode		*m_Sides[2];

			uint8		m_bPostRemove;

	};

	typedef CGLinkedList<CNode*>	CNodeList;
	typedef CNode*					NODEREF;

	inline BOOL IsValidNode(NODEREF node) {return !!node && node!=NODE_IN && node!=NODE_OUT;}


#endif  // __NODE_H__

