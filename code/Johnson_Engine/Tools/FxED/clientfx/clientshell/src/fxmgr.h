//------------------------------------------------------------------
//
//   MODULE  : FXMGR.H
//
//   PURPOSE : Defines class CFxMgr
//
//   CREATED : On 10/5/98 At 6:58:51 PM
//
//------------------------------------------------------------------

#ifndef __FXMGR__H_
	#define __FXMGR__H_

	// Includes....

	#include "iltclient.h"
	#include "basefx.h"
	#include "linklist.h"

	// Forwards....

	class CFxMgr;
	struct CLIENTFX_INSTANCE;

	// Typedefs....

	typedef int (*FX_GETNUM)();
	typedef FX_REF (*FX_GETREF)(int);

	// Structures

	struct FX_KEY
	{
		FX_REF							   *m_pFxRef;
		uint32								m_dwID;
		float								m_tmStart;
		float								m_tmEnd;
		uint32								m_dwKeyRepeat;
		CLinkList<FX_PROP>					m_collProps;

		bool								m_bLinked;
		uint32								m_dwLinkedID;
		char								m_sLinkedNodeName[32];
		bool								m_bContinualLoop;
	};
	
	struct FX_GROUP
	{
		char								m_sName[128];
		CLinkList<FX_KEY *>					m_collFx;
		float								m_tmTotalTime;
	};

	struct FX_LINK
	{
		uint32								m_dwID;
		CBaseFX							   *m_pFX;
		FX_KEY							   *m_pRef;
	};


	struct CLIENTFX_CREATESTRUCT
	{
											CLIENTFX_CREATESTRUCT()
											{
												memset(this, 0, sizeof(CLIENTFX_CREATESTRUCT));
											}

											CLIENTFX_CREATESTRUCT(char *sName, uint32 dwFlags, LTVector vPos)
											{
												memset(this, 0, sizeof(CLIENTFX_CREATESTRUCT));

												strcpy(m_sName, sName);
												m_dwFlags    = dwFlags;
												m_vPos       = vPos;
											}

											CLIENTFX_CREATESTRUCT(char *sName, uint32 dwFlags, HOBJECT hParent)
											{
												memset(this, 0, sizeof(CLIENTFX_CREATESTRUCT));

												strcpy(m_sName, sName);
												m_dwFlags    = dwFlags;
												m_hParent	 = hParent;
											}

		char								m_sName[32];
		uint32								m_dwFlags;
		LTVector							m_vPos;

		HOBJECT								m_hParent;
		HOBJECT								m_hServerObject;
		char							    m_sParentNode[32];
		HOBJECT								m_hTarget;

		bool								m_bUseTargetData;
		DVector								m_vTargetPos;
		DVector								m_vTargetNorm;
	};

	// Classes....

	class CFxMgr
	{
		public :

			// Constuctor

											CFxMgr();

			// Destructor
							
											~CFxMgr();

			// Member Functions

			bool							Init(CClientDE *pClientDE);
			void							Term();

			bool							PreUpdateAllActiveFX(HOBJECT hCamera);
			bool							UpdateAllActiveFX(bool bAppHasFocus, HOBJECT hCamera);

			bool							LoadFxGroups(const char *sDir, const char *sName);
			bool							LoadFxDll(char *sName);

			CLIENTFX_INSTANCE*				CreateClientFX(const CLIENTFX_CREATESTRUCT &fxInit);
			void							SetGroupParent(CLIENTFX_INSTANCE *pInstance, HOBJECT hParent);

			void							OnSpecialEffectNotify(uint8 nId, HOBJECT hObject, HMESSAGEREAD hMessage);
			bool							OnObjectRemove(HOBJECT hObject);
			void							OnRendererShutdown();

			void							ShutdownAllFX();
			void							ShutdownClientFX(CLIENTFX_INSTANCE* pFxGroup);
		
			uint32							GetUniqueID();

			// Accessors

			CLinkList<FX_REF>*				GetFxList() { return &m_collFX; }
			int								GetFxListLength() { return m_collFX.GetSize(); }

			void							SetMultiplay(bool bMultiplay) { if (m_pfnSetMultiplay) m_pfnSetMultiplay(bMultiplay); }

			static ILTClient*				GetClientDE();

		private :

			// Member Functions

			CBaseFX*						CreateFX(char *sName, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps);

			void							FreezeInstance(CLIENTFX_INSTANCE *pInst);
			void							UnfreezeInstance(CLIENTFX_INSTANCE *pInst, HOBJECT hNewParent);

			FX_REF*							FindFX(char *sName);
			FX_GROUP*						FindGroupFX(char *sName);
			bool							ExistFX(CLIENTFX_INSTANCE *pFxGroup);

			// Member Variables

			float							m_tmUpdateDelta;
			float							m_tmLastUpdate;

			CClientDE					   *m_pClientDE;
			static ILTClient			   *s_pClientDE;
			CLinkList<FX_REF>				m_collFX;

			CLinkList<FX_GROUP *>			m_collGroupFX;
			CLinkList<CLIENTFX_INSTANCE *>	m_collActiveGroupFX;
			CLinkList<CLIENTFX_INSTANCE *>  m_collInactiveGroupFX;
			FX_SETMULTIPLAYFUNC				m_pfnSetMultiplay;
			FX_DETAILFUNC					m_pfnDetailFunc;
			FX_SETPLAYERFUNC				m_pfnSetPlayer;
			FX_SETPARAMS					m_pfnSetParams;

			FX_PARAMS						m_params;
	};


	struct CLIENTFX_INSTANCE
	{
		// Member Functions

											CLIENTFX_INSTANCE()
											{
												m_sNode[0]		   = 0;
												m_hAlternateParent = NULL;
											}

											~CLIENTFX_INSTANCE()
											{
												if (m_hAlternateParent)
												{
													CFxMgr::GetClientDE()->DeleteObject(m_hAlternateParent);
												}
											}

		bool								ExistFX(CBaseFX *pFX)
											{
												CLinkListNode<FX_LINK> *pNode = m_collActiveFX.GetHead();

												while (pNode)
												{
													if (pNode->m_Data.m_pFX == pFX) return true;
													
													pNode = pNode->m_pNext;
												}

												return NULL;
											}

		// Member Variables

		FX_GROUP						   *m_pData;
		CLinkList<FX_KEY *>					m_collInactiveFX;
		CLinkList<FX_LINK>					m_collActiveFX;
		float								m_tmCreate;
		DVector								m_vPos;
		uint32								m_dwID;
		HOBJECT								m_hParent;
		HOBJECT								m_hTarget;
		bool								m_bLoop;
		bool								m_bSmoothShutdown;
		bool								m_bShutdown;
		char								m_sNode[32];

		bool								m_bHasServerObject;
		HOBJECT								m_hServerObject;
		uint32								m_dwServerID;

		bool								m_bUseTargetData;
		DVector								m_vTargetPos;
		DVector								m_vTargetNorm;

		HOBJECT								m_hAlternateParent;
	};

#endif