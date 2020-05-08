// ----------------------------------------------------------------------- //
//
// MODULE  : Bombable.h
//
// PURPOSE : The Bombable object
//
// CREATED : 10/25/01
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __BOMBABLE_H__
#define __BOMBABLE_H__

//
// Includes...
//

	#include "GadgetTarget.h"
	#include "Timer.h"

LINKTO_MODULE( Bombable );


//
// Classes...
//

//[SPIKE]
typedef struct SoundInfo
{
	HOBJECT		m_hSenderObject;
	HLTSOUND	m_hSISound;

public:
	SoundInfo()
	{
		m_hSenderObject = LTNULL;
		m_hSISound		= LTNULL;
	}
}*SoundInfo_Ptr;

typedef	std::vector<VOID*> VECTOR_SOUND_LIST;
typedef VECTOR_SOUND_LIST::iterator VECTOR_SOUND_LIST_ITR;

class Bombable : public GadgetTarget
{
	public : // Methods...

		Bombable();
		~Bombable();

		virtual LTVector GetInitPosTimeBomb(){ return m_vTimeBombInitPos;}
		virtual VOID SetInitPosTimeBomb(LTVector vInitPos){m_vTimeBombInitPos = vInitPos;}
		
		//[MURSUM]
		virtual void	ForcePlantedTimeBomb( float fTime );


	protected : // Methods...

		uint32	EngineMessageFn( uint32 messageID, void *pData, LTFLOAT fData );
		bool	OnTrigger(HOBJECT hSender, const CParsedMsg &cMsg);

		void	Update();

		LTBOOL	ReadProp( ObjectCreateStruct *pStruct );

		LTBOOL	OnSave( ILTMessage_Write *pMsg ); 
		LTBOOL	OnLoad( ILTMessage_Read *pMsg );
	
		void	SetupDisabledState( LTBOOL bDestroyed );
		void	HandleActivateMsg( HOBJECT hSender );
		void	HandleStoppedMsg( const CParsedMsg &cMsg, HOBJECT hSender = LTNULL );

		void	SetupDefusedState( bool bSend = true );
		void	SetupDetonatedState( );

		// Implementing classes will have this function called
		// when HOBJECT ref points to gets deleted.
		virtual void OnLinkBroken( LTObjRefNotifier *pRef, HOBJECT hObj );


		void	UpdateSFXMsg(uint8 nTargetType, bool bSwitch);

	protected : // Members...

		float		m_fDefuseTime;
		float		m_fDetonateTime;
		HSTRING		m_hstrDetonateCmd;
		HSTRING		m_hstrDefusedCmd;
		HSTRING		m_hstrDefusingSnd;
		HSTRING		m_hstrDefusedSnd;
		LTObjRefNotifier	m_hTimeBombModel;
		LTObjRefNotifier	m_hBombPickup;

		CTimer		m_Timer;
		
		bool		m_bStartPlanted;
		bool		m_bCanDefuse;
		bool		m_bDefuseOnce;

		//[SPIKe]
		LTVector m_vTimeBombInitPos;

		VECTOR_SOUND_LIST	m_SoundItemList;
};


class CBombablePlugin : public CGadgetTargetPlugin 
{
	public:

		virtual LTRESULT PreHook_EditStringList(
			const char* szRezPath,
			const char* szPropName,
			char** aszStrings,
			uint32* pcStrings,
			const uint32 cMaxStrings,
			const uint32 cMaxStringLength);
};

#endif // __TIME_BOMB_H__