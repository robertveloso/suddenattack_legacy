//------------------------------------------------------------------
//
//   MODULE    : CLIENTFX.H
//
//   PURPOSE   : Defines class CClientFX
//
//   CREATED   : On 8/23/00 At 6:45:48 PM
//
//   COPYRIGHT : (C) 2000 LithTech Inc
//
//------------------------------------------------------------------


#ifndef __CLIENTFX_H_
	#define __CLIENTFX_H_

	//---------------------
	// Includes....
	//---------------------

	#include "ltbasetypes.h"
	#include "ltengineobjects.h"
	#include "linklist.h"

	//---------------------
	// Structure definitions....
	//---------------------

	struct LINK_STATUS
	{
		bool							m_bLinked;
		uint32							m_dwLinkedID;
		char							m_sLinkedNodeName[32];
	};

	struct FX_PROP
	{
		struct FX_CLRKEY
		{
			float						m_tmKey;
			uint32						m_dwCol;
		};

		// Enumerations

		enum eDataType
		{
			STRING,
			INTEGER,
			FLOAT,
			COMBO,
			VECTOR,
			VECTOR4,
			CLRKEY,
			PATH,
			ENUM
		};
													
		// Member Variables


		char							m_sName[128];
		eDataType						m_nType;
		union							FX_DATATYPE
		{
			char						m_sVal[128];
			int							m_nVal;
			float						m_fVal;
			float						m_fVec[3];
			float						m_fVec4[4];
			FX_CLRKEY					m_clrKey;
		}								m_data;
	};

	struct FX_REZ
	{
		int								m_nType;
		char							m_sRez[128];
	};

	struct FX_HEADER
	{
		char							m_sName[64];
		float							m_tmLen;
		CLinkList<FX_REZ>				m_collResources;
	};

	struct FX_LIST
	{
		CLinkList<FX_HEADER *>			m_collClientFX;
	};

	//---------------------
	// Class definitions....
	//---------------------

	class CClientFX : public BaseClass
	{
		public :
			
			// Constructor

										CClientFX();

			// Destructor

										~CClientFX();

		protected :

			// Member Functions

			uint32						EngineMessageFn(uint32 messageID, void *pData, float fData);
			uint32						ObjectMessageFn(HOBJECT hSender, uint32 messageID, HMESSAGEREAD hRead);

			virtual void				ReadProps(ObjectCreateStruct *pOcs);
			virtual void				OnSave(HMESSAGEWRITE hWrite, uint32 dwFlags);
			virtual void				OnLoad(HMESSAGEREAD hRead, uint32 dwFlags);
			void						CacheFiles();

			// Member Variables

			bool						m_bStartOn;
			char						m_sFxName[64];
			uint32						m_dwFxFlags;
			bool						m_bLoop;
	};
		
	
	//---------------------
	// Global functions....
	//---------------------

	void SetupClientFX(ILTServer *pServer);
	bool LoadClientFX(const char *sDir, const char *sFileName, const bool bMultiplayer);
	void RemoveClientFX();
	bool CacheClientFX(const char *sName);
	void SetClientFXMessage(BaseClass *pClass, char *sName, uint32 uFlags);

#endif