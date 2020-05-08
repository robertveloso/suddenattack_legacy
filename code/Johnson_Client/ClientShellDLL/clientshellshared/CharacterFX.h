// ----------------------------------------------------------------------- //
//
// MODULE  : CharacterFX.h
//
// PURPOSE : Character special fx class - Definition
//
// CREATED : 8/24/98
//
// (c) 1998-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __CHARACTER_FX_H__
#define __CHARACTER_FX_H__

#include "SpecialFX.h"
#include "SurfaceMgr.h"
#include "NodeController.h"
#include "ModelButeMgr.h"
#include "iltmodel.h"
#include "SharedFXStructs.h"
#include "PolyLineFX.h"
#include "Timer.h"
#include "FlashLight.h"
#include "ClientTrackedNodeMgr.h"
#include "ClientTrackedNodeContext.h"
#include "ClientFXMgr.h"
#include "iltfontmanager.h"
#include "FXButeMgr.h"
#include "HitBox.h"

#define MAX_DINGS	5

struct NodeRadiusStruct
{
	NodeRadiusStruct()
	{
        hModel = LTNULL;
		eNode = eModelNodeInvalid;
	}

	~NodeRadiusStruct()
	{
		if (hModel)
		{
            g_pLTClient->RemoveObject(hModel);
		}
	}

	LTObjRef hModel;
	ModelNode eNode;
};

class CCharacterFX : public CSpecialFX
{
	public :

		CCharacterFX() : CSpecialFX()
		{
            m_pBubbles              = LTNULL;
            m_pCigarette	        = LTNULL;
            m_pZzz					= LTNULL;
			
			m_fNextBubbleTime		= -1.0f;
			m_fNextCigaretteTime	= -1.0f;
			m_fNextSmokepuffTime	= -1.0f;
			m_fNextZzzTime			= -1.0f;
			m_fNextHeartTime		= -1.0f;
						
			// ±¤½Ä : Model Test
			m_TestModelCount		= 0;

            m_bLeftFoot             = LTTRUE;
            m_eLastSurface			= ST_UNKNOWN;
			m_hDialogueSnd			= LTNULL;
			m_hVehicleSound			= LTNULL;
			m_bSubtitle				= LTFALSE;
			m_bOnVehicle			= LTFALSE;
			m_hWeaponLoopSound		= LTNULL;

			//[MURSUM]
			m_fPitch				= 0.0f;
			m_fNewPitch				= 0.0f;
			m_bPrediction			= false;
			
			// minjin
			m_fPacketUpdateRate		= GetConsoleFloat( "UpdateRate", DEFAULT_CSENDRATE );
			m_fLastPitch			= 0.0f;
			m_fPredictPitch			= 0.0f;
			
			m_szInfoString[0] = 0;
			m_szInfoString[kMaxInfoStringLength] = 0;

			m_hUpperTorsoNode		= INVALID_TRACKEDNODE;
			m_hLowerTorsoNode		= INVALID_TRACKEDNODE;
			m_hHeadNode				= INVALID_TRACKEDNODE;

			m_nLastDamageFlags		= 0;
			m_nInstantDamageFlags	= 0;
			
			m_bWasPlayingSpecialDamageAni = false;

			m_bDamageFxTrackingOverride = false;

			m_bPlayerDead			= false;

			m_p3rdPersonDamageFX	= LTNULL;
			m_nNum3rdPersonDamageFX	= 0;

			m_bPitchEnabled			= false;

			m_bLocalPlayer			= false;
			m_bUpdateNick			= false;
			m_eLastContainerCode	= CC_NO_CONTAINER;
		}

		~CCharacterFX();

		void DoFootStep()
		{
			// Alternate feet...
			m_bLeftFoot = !m_bLeftFoot;
            DoFootStepKey(m_hServerObject, LTTRUE);
		}

		void PlayMovementSound(LTVector vPos, SurfaceType eSurface,
			LTBOOL bLeftFoot, PlayerPhysicsModel eModel=PPM_NORMAL);

 		void ResetSoundBufferData() { m_NodeController.ResetSoundBufferData(); }
 
		virtual LTBOOL	Init(HLOCALOBJ hServObj, ILTMessage_Read *pMsg);
        virtual LTBOOL	Init(SFXCREATESTRUCT* psfxCreateStruct);
        virtual LTBOOL	CreateObject(ILTClient* pClientDE);
        virtual LTBOOL	Update();
		virtual void	OnModelKey(HLOCALOBJ hObj, ArgList *pArgs);
        virtual LTBOOL	OnServerMessage(ILTMessage_Read *pMsg);
		virtual void	OnObjectRotate( LTRotation *pNewRot );
		
		virtual void	ChangeModel(ModelId eModelId);
		// ±¤½Ä [05.03.18] : Head skin °ú body skin Ãß°¡
		virtual void	ChangeModel(ModelId eModelId, uint8 nHeadSkin, uint8 nBodySkin);

		//[MURSUM]
		void WantRemove(LTBOOL bRemove=LTTRUE);

		ModelId			GetModelId() const { return m_cs.eModelId; }
		ModelSkeleton	GetModelSkeleton() const { return m_cs.eModelSkeleton; }
		ModelType		GetModelType() const { return m_cs.eModelType; }
		uint8			GetPlayerID() const { return m_cs.nClientID; }

        HLTSOUND        PlayLipSyncSound(char* szSound, LTFLOAT fRadius, LTBOOL & bSubtitle, bool bSubtitlePriority, bool bGetHandle = true);
		
		LTBOOL			IsPlayingTaunt() { return m_NodeController.IsLipSynching(); }
		
		bool			IsBodyInLiquid() { return IsLiquid(m_eLastContainerCode); }

		void			SetLastContainerCode( ContainerCode eCode ) { m_eLastContainerCode = eCode; }

        SurfaceType     GetLastSurface() const { return m_eLastSurface; }

		virtual uint32 GetSFXID() { return SFX_CHARACTER_ID; }

		HOBJECT	GetHitBox() const { return m_HitBox.GetObject(); }
				
		bool	CanBeCarried() { return m_cs.bCanCarry; }
		bool	CanWake() { return m_cs.bCanWake; }

		const char * GetInfoString() const { return m_szInfoString; }

		// show or hide all client effects attached to this character model
		void ShowAttachClientFX();
		void HideAttachClientFX();
		
		virtual void Render(HOBJECT hCamera);

		bool	IsPlayerDead();
		bool	IsRadarPlayerDead();
		bool	IsUnconscious() { return !!(m_cs.nDamageFlags & DamageTypeToFlag( DT_SLEEPING )); }
		bool	Slipped() { return !!(m_cs.nDamageFlags & DamageTypeToFlag( DT_SLIPPERY ));	}

		static uint32 GetNumPlayersInGame() { return m_lstPlayersInGame.size(); }

		uint8	GetCarrying() { return m_cs.nCarrying; }
		
		virtual void EnablePitch( bool bEnable );

		//[MURSUM]
		void OnWeaponChanged( uint8 nWeaponId, int nAmmoId, int nAmmoCount, int nAmmoInClip );
		void	SetCurrentWeapon( HOBJECT hWeapon ) { m_hCurrentWeapon = hWeapon; }
		HOBJECT GetCurrentWeapon() { return m_hCurrentWeapon; }
		uint8 GetCurrentWeaponID() { return m_nCurrentWeaponID; }
		float GetModelPitch()	   { return m_fNewPitch; }
		void SetUpdateNick( bool bUpdate )
		{ 
			m_bUpdateNick = bUpdate;
			if( m_bLocalPlayer ) m_bUpdateNick = false;
		}
		void RecvModelInfo() { m_bRecvModelInfo = true; }
		bool IsRecvModelInfo() { return m_bRecvModelInfo; }

		void TurnOnFlashLight( LTVector* pvFlashPos, LTRotation* prRot );
		void TurnOffFlashLight();
		void GetNodePos( uint8 nNode, LTVector* pvPos );
		LTBOOL	GetNodeTransform(const char* const pszNodeName,LTransform &NodeTransform);
						
		// ±¤½Ä
		LTBOOL	AttachAfterDeath( HLOCALOBJ hServerObj );
			
		enum Constants
		{
			kMaxInfoStringLength = 1023
		};

		CHARCREATESTRUCT	m_cs;

#ifdef _DEBUG
		// ±¤½Ä : Test Model
		//------------------------------------------
		void CreateTestModelFX();
		void UpdateTestModelFX();
		void RemoveTestModelFX();

		void CreateNodeRadiusModels();
		void RemoveNodeRadiusModels();
		void UpdateNodeRadiusModels();
		LTVector GetNodeModelColor(ModelSkeleton eModelSkeleton, ModelNode eModelNode);
		//------------------------------------------
#endif

		LTRESULT HandlePitchMsg( float fPitch/*ILTMessage_Read *pMsg*/ );

	protected :
		// create or destroy all client effects attached to this character model
		void CreateAttachClientFX();
		void RemoveAttachClientFX();

        void CreateUnderwaterFX(const LTVector & vPos);
        void UpdateUnderwaterFX(LTVector & vPos);
		void RemoveUnderwaterFX();

		void CreateCigaretteFX();
		void UpdateCigaretteFX();
		void RemoveCigaretteFX();		

		void CreateZzzFX();
		void UpdateZzzFX();
		void RemoveZzzFX();

		//[MURSUM]
		void UpdateNickName();
		void UpdateBreathFX();		
		void UpdateDamageFX();
		void ShutdownDamageFX();

		void UpdateSounds();
		void HandleDialogueMsg(ILTMessage_Read *pMsg);
		void HandleWeaponSoundLoopMsg(ILTMessage_Read *pMsg);
//		void HandlePitchMsg( float fPitch/*ILTMessage_Read *pMsg*/ );
		void UpdatePitch( );
		void pd_UpdatePitch();	//[MURSUM]
		void KillWeaponLoopSound();

		void ResetPitchTracking();

        void    DoFootStepKey(HLOCALOBJ hObj, LTBOOL bForceSound=LTFALSE);
		void	CreateTrail(SurfaceType eType, IntersectInfo & iInfo);
		void	CreateFootprint(SurfaceType eType, IntersectInfo & iInfo);
		void    UpdateOnVehicle();
		void	UpdateMultiVehicleSounds();
		void	UpdateVehicleContour( LTRotation &rCharacterRot );

        void    CreateVehicleTrail(	CPolyLineFX* pTrail,
									LTVector vDir,
									LTVector vStartPoint,
									LTVector vEndPoint,
									LTBOOL bNewTrail );

		void	InitLocalPlayer();
		void	UpdateAttachments();
		void	KillLipSyncSound( bool bSendNotification );
		void	UpdateCarrying();
		
		// ±¤½Ä
		void	UpdateAttachmentsWeapon();
		
		void	PlayDeathSound();

		//[SPIKE]
		VOID	UpdateServerObjectCheckLiquid();


		CFlashLight3rdPerson	m_3rdPersonFlashlight;	// Only used if fx is seen from 3rd person

		CSpecialFX*			m_pBubbles;			// Bubbles fx
		CSpecialFX*			m_pCigarette;		// Cigarette for poodle
		CSpecialFX*			m_pZzz;				// Zzz for sleeping dudes
		
		CBaseScaleFX		m_CigaretteModel;
		BSCREATESTRUCT		m_scalecs;
		
		// ±¤½Ä : Model Test
		//----------------------------------------
		CBaseScaleFX		m_TestModel[100];
		BSCREATESTRUCT		m_TestModelScalecs;
		int					m_TestModelCount;

		CTList<NodeRadiusStruct*>	m_NodeRadiusList;
		//----------------------------------------

        LTFLOAT             m_fNextBubbleTime;
        LTFLOAT             m_fNextHeartTime;
        LTFLOAT             m_fNextCigaretteTime;
        LTFLOAT             m_fNextSmokepuffTime;
        LTFLOAT             m_fNextZzzTime;
		
        LTBOOL              m_bLeftFoot;
        SurfaceType         m_eLastSurface;
		
		//[SPIKE]
		ContainerCode		m_eLastContainerCode;
		LTVector			m_vLandPos;

		CPolyLineFX			m_VehicleTrail1;
		CPolyLineFX			m_VehicleTrail2;
		CNodeController		m_NodeController;	// Our node controller

		//the amount of time that has currently elapsed for this breath interval
		float				m_fBreathElapsedTime;
		//the amount of time that needs to elapse for another breath to be emitted
		float				m_fBreathEndTime;

		HLTSOUND			m_hDialogueSnd;
		uint8				m_nUniqueDialogueId;
		bool				m_bSubtitlePriority;

		HLTSOUND			m_hVehicleSound;
		LTBOOL				m_bSubtitle;
		LTBOOL				m_bOnVehicle;

		char				m_szInfoString[kMaxInfoStringLength + 1];
		
		HLTSOUND			m_hWeaponLoopSound;
		
		HTRACKEDNODE		m_hUpperTorsoNode;
		HTRACKEDNODE		m_hLowerTorsoNode;
		HTRACKEDNODE		m_hHeadNode;

		CClientTrackedNodeContext	m_TrackedNodeContext;

		CLIENTFX_LINK_NODE	m_AttachClientFX;

		CLIENTFX_LINK		*m_p3rdPersonDamageFX;
		uint32				m_nNum3rdPersonDamageFX;

		CLIENTFX_LINK		m_link3rdPersonInstFX;
				
		DamageFlags			m_nLastDamageFlags;
		DamageFlags			m_nInstantDamageFlags;
		
		bool				m_bWasPlayingSpecialDamageAni;
		bool				m_bDamageFxTrackingOverride;
		bool				m_bPlayerDead;
		bool				m_bUpdateAttachments;

		CHitBox				m_HitBox;

		bool				m_bPitchEnabled;
		
		typedef std::vector<CCharacterFX*> CharFXList;
		static CharFXList	m_lstPlayersInGame;
		

		//[MURSUM]
		float				m_fPitch;
		float				m_fNewPitch;
		bool				m_bPrediction;		
		bool				m_bLocalPlayer;
		bool				m_bUpdateNick;
		bool				m_bRecvModelInfo;
		
		// minjin
		LTFLOAT				m_fPacketUpdateRate;
		LTFLOAT				m_fLastPitch;
		LTFLOAT				m_fPredictPitch;

		uint8				m_nCurrentWeaponID;
		uint8				m_nCurrentAmmoID;
		uint16				m_nCurrentAmmoCount;

		HOBJECT				m_hCurrentWeapon;
};

#endif 
