// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerMgr.cpp
//
// PURPOSE : Implementation of class used to manage the client player
//
// (c) 2001-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#include "stdafx.h"
#include "GameClientShell.h"
#include "PlayerMgr.h"
#include "AttachButeMgr.h"
#include "BodyFX.h"
#include "CMoveMgr.h"
#include "CameraFX.h"
#include "CameraOffsetMgr.h"
#include "ClientUtilities.h"
#include "ClientWeaponMgr.h"
#include "FlashLight.h"
#include "GadgetDisabler.h"
#include "HeadBobMgr.h"
#include "LeanMgr.h"
#include "MsgIDs.h"
#include "PlayerCamera.h"
#include "PlayerShared.h"
#include "Searcher.h"
#include "VarTrack.h"
#include "VehicleMgr.h"
#include "VisionModeMgr.h"
#include "VolumeBrushFX.h"
#include "TargetMgr.h"
#include "MissionMgr.h"
#include "ClientMultiplayerMgr.h"
#include "ClientButeMgr.h"
#include "DynamicOccluderVolumeFX.h"
#include "TriggerFX.h"
#include "PlayerLureFX.h"
#include "CharacterFX.h"
#include "PolyGridFX.h"
#include "PlayerViewAttachmentMgr.h"
#include "LTEulerAngles.h"
#include "DoomsDayPieceFX.h"
#include "GameText.h"
#include "GameMessage.h"
// 광식
//-------------------------------
#include "iltclient.h"
//-------------------------------

#include "../SA/SAHUDMgr.h"
#include "ClientWeapon.h"

CPlayerMgr* g_pPlayerMgr = NULL;

#define MAX_SHAKE_AMOUNT		10.0f

#define FOVX_HALFZOOMED			28.0f
#define FOVX_ZOOMED				20.0f
#define FOVX_ZOOMED1			7.0f
#define FOVX_ZOOMED2			2.0f
//#define ZOOM_TIME				0.5f
#define ZOOM_TIME				1.0f
#define LOD_ZOOMADJUST			-5.0f
#define DEFAULT_LOD_OFFSET				0.0f

#define FIRST_ZOOM_BREATH		1.5f
#define SECOND_ZOOM_BREATH		10.0f

#define MAX_AIR_LEVEL						100.0f
#define AIR_LOSS_DAMAGE						10.0f
#define AIR_LOSS_DAMAGE_TIME				3.0f
#define FULL_AIR_LOSS_TIME					20.0f
#define FULL_AIR_REGEN_TIME					7.0f

namespace
{
	bool		g_bCinChangedNumModelShadows = false;
	int			g_nCinSaveNumModelShadows = 0;
	const		g_kMaxNumberOfCinShadows = 10;
	uint8		s_nLastCamType = CT_FULLSCREEN;
	float       s_fDeadTimer    = 0.0f;
	float       s_fDeathDelay   = 0.0f;
	LTVector	g_vSVModelColor; // model color modifier for spy vision
	int			g_nSaveSpyVisionShadows = 0;
	// 광식
	uint8		g_SelectID = 255;
	LTBOOL		g_bIsAttachFirst = LTFALSE;

	HOBJECT		staticObject = LTNULL;
}

LTVector g_vPlayerCameraOffset = g_kvPlayerCameraOffset;

extern bool	g_bScreenShotMode;
LTBOOL g_bHalfZoomMode = LTFALSE; //[SPIKE]
LTFLOAT g_fPVModelFOVStep = 0.0f; //[SPIKE]
LTFLOAT g_fLeft = 1;
LTBOOL	g_bUp   = LTTRUE;

void SVModelHook(ModelHookData *pData, void *pUser);


VarTrack			g_CV_CSendRate;		// The SendRate console variable.
VarTrack			g_vtPlayerRotate;	// The PlayerRotate console variable
VarTrack			g_vtCamZoom1MaxDist;
VarTrack			g_vtCamZoom2MaxDist;
VarTrack			g_vtUseCamRecoil;
VarTrack			g_vtCamRecoilRecover;
VarTrack			g_vtBaseCamRecoilPitch;
VarTrack			g_vtMaxCamRecoilPitch;
VarTrack			g_vtBaseCamRecoilYaw;
VarTrack			g_vtMaxCamRecoilYaw;
VarTrack			g_vtFireJitterDecayTime;
VarTrack			g_vtFireJitterMaxPitchDelta;
VarTrack			g_vtFOVXNormal;
VarTrack			g_vtFOVYNormal;
VarTrack			g_vtFOVYMaxUW;
VarTrack			g_vtFOVYMinUW;
VarTrack			g_vtUWFOVRate;
VarTrack			g_vtRespawnWaitTime;
VarTrack			g_vtMultiplayerRespawnWaitTime;
VarTrack			g_vtDoomsdayRespawnWaitTime;
VarTrack			g_vtCamRotInterpTime;
VarTrack			g_vtScreenFadeInTime;
VarTrack			g_vtScreenFadeOutTime;
VarTrack			g_vtChaseCamPitchAdjust;
VarTrack			g_vtChaseCamOffset;
VarTrack			g_vtChaseCamDistUp;
VarTrack			g_vtChaseCamDistBack;
VarTrack			g_vtNormalTurnRate;
VarTrack			g_vtFastTurnRate;
VarTrack			g_vtLookUpRate;
VarTrack			g_vtCameraSwayXFreq;
VarTrack			g_vtCameraSwayYFreq;
VarTrack			g_vtCameraSwayXSpeed;
VarTrack			g_vtCameraSwayYSpeed;
VarTrack			g_vtCameraSwayDuckMult;
VarTrack			g_vtModelGlowTime;
VarTrack			g_vtModelGlowMin;
VarTrack			g_vtModelGlowMax;
VarTrack			g_vtActivateOverride;
VarTrack			g_vtCamDamage;
VarTrack			g_vtCamDamagePitch;
VarTrack			g_vtCamDamageRoll;
VarTrack			g_vtCamDamageTime1;
VarTrack			g_vtCamDamageTime2;
VarTrack			g_vtCamDamageMinPitchVal;
VarTrack			g_vtCamDamageMaxPitchVal;
VarTrack			g_vtCamDamageMinRollVal;
VarTrack			g_vtCamDamageMaxRollVal;
VarTrack			g_vtAlwaysHUD;
VarTrack			g_vtDamageFadeRate;
VarTrack			g_vtAdaptiveMouse;
VarTrack			g_vtAdaptiveMouseMaxOffset;
VarTrack			g_vtShowSoundFilterInfo;
VarTrack			g_vtMultiplayerDeathCamMoveTime;
VarTrack			g_vtMultiAttachDeathCamMaxTime;
VarTrack			g_vtAttachedCamInterpolationRate;

extern VarTrack		g_vtUseSoundFilters;

// --------------------------------------------------------------------------- //
// Constructor & Destructor
// --------------------------------------------------------------------------- //
CPlayerMgr::CPlayerMgr()
	:  m_bServerAccurateRotation( false )
	, m_bSendCameraOffsetToServer( false )
{
	m_pHeadBobMgr = debug_new( CHeadBobMgr );            ASSERT( 0 != m_pHeadBobMgr );
	m_pCameraOffsetMgr = debug_new( CCameraOffsetMgr );  ASSERT( 0 != m_pCameraOffsetMgr );
	m_pFlashLight = debug_new( CFlashLightPlayer );      ASSERT( 0 != m_pFlashLight );
	m_pGadgetDisabler = debug_new( CGadgetDisabler );    ASSERT( 0 != m_pGadgetDisabler );
	m_pSearcher = debug_new( CSearcher );                ASSERT( 0 != m_pSearcher );
	m_pMoveMgr = debug_new( CMoveMgr );                  ASSERT( 0 != m_pMoveMgr );
	m_pVisionModeMgr = debug_new( CVisionModeMgr );      ASSERT( 0 != m_pVisionModeMgr );
	m_pAttachButeMgr = debug_new( CAttachButeMgr );      ASSERT( 0 != m_pAttachButeMgr );
	m_pLeanMgr = debug_new( CLeanMgr );                  ASSERT( 0 != m_pLeanMgr );
	m_pClientWeaponMgr = debug_new( CClientWeaponMgr );  ASSERT( 0 != m_pClientWeaponMgr );
	m_pPlayerCamera = debug_new( CPlayerCamera );        ASSERT( 0 != m_pPlayerCamera );
	m_pPVAttachmentMgr = debug_new( CPlayerViewAttachmentMgr ); ASSERT( 0 != m_pPVAttachmentMgr );

//	InitTargetMgr();

	m_hCamera                   = NULL;
	m_bStrafing                 = LTFALSE;
	m_bHoldingMouseLook         = LTFALSE;
	m_fYawBackup                = 0.0f;
	m_fPitchBackup              = 0.0f;
	m_bRestoreOrientation       = LTFALSE;
	m_bAllowPlayerMovement      = LTTRUE;
	m_bLastAllowPlayerMovement  = LTTRUE;
	m_bWasUsingExternalCamera   = LTFALSE;
	m_bUsingExternalCamera      = LTFALSE;
	m_bCamIsListener            = LTFALSE;
	m_bCamera  = LTFALSE;
	m_bStartedPlaying			= LTFALSE;
	m_vSVLightScale.Init(1, 1, 1);

	m_vShakeAmount.Init();
	m_bSpectatorMode        = LTFALSE;
	m_bInvisibleMode		= LTFALSE;

	m_fEarliestRespawnTime	= 0.0f;
	
	m_rRotation.Init();

	m_fPitch			= 0.0f;
	m_fYaw				= 0.0f;
	m_fRoll				= 0.0f;

	m_fPlayerPitch		= 0.0f;
	m_fPlayerYaw		= 0.0f;
	m_fPlayerRoll		= 0.0f;

	m_fModelAttachPitch	= 0.0f;
	m_fModelAttachYaw	= 0.0f;
	m_fModelAttachRoll	= 0.0f;

	m_fFireJitterPitch	= 0.0f;
	m_fFireJitterYaw	= 0.0f;

	m_dwPlayerFlags			= 0;
	m_ePlayerState			= PS_UNKNOWN;

	m_nZoomView				= 0;
	m_bZooming              = LTFALSE;
	m_bZoomingIn            = LTFALSE;
	m_fSaveLODScale			= DEFAULT_LOD_OFFSET;

	m_eCurContainerCode		= CC_NO_CONTAINER;
	m_nSoundFilterId		= 0;
	m_nGlobalSoundFilterId	= 0;
	m_bInSafetyNet			= false;
	// 광식
	m_bInTimeBombArea		= false;

	m_fContainerStartTime	= -1.0f;
	m_fFovXFXDir			= 1.0f;

	m_vCurModelGlow.Init(127.0f, 127.0f, 127.0f);
	m_vMaxModelGlow.Init(255.0f, 255.0f, 255.0f);
	m_vMinModelGlow.Init(50.0f, 50.0f, 50.f);
	m_fModelGlowCycleTime	= 0.0f;
	m_bModelGlowCycleUp     = LTTRUE;

	m_bCameraPosInited      = LTFALSE;

	m_bPlayerUpdated		= LTFALSE;

	m_bCameraAttachedToHead	= false;
	m_bLerpAttachedCamera	= false;

	m_nPlayerInfoChangeFlags	= 0;
	m_fPlayerInfoLastSendTime	= 0.0f;

	m_bUseWorldFog          = LTTRUE;
	m_hContainerSound       = NULL;

	m_bStartedDuckingDown   = LTFALSE;
	m_bStartedDuckingUp     = LTFALSE;
	m_fCamDuck				= 0.0f;
	m_fDuckDownV			= -75.0f;
	m_fDuckUpV				= 75.0f;
	m_fMaxDuckDistance		= -40.0f;
	m_fStartDuckTime		= 0.0f;

	m_nCarryingObject		= CFX_CARRY_NONE;
	m_bCanDropCarriedObject	= false;

 	m_fDistanceIndicatorPercent = -1.0f;
	m_hDistanceIndicatorIcon	= LTNULL;

	m_nPreGadgetWeapon		= WMGR_INVALID_ID;
	m_bChangingToGadget = false;
	m_bSwitchingWeapons = false;

	m_fMultiplayerDeathCamMoveTimer = 0.0f;
	m_fMultiAttachDeathCamTimer = 0.0f;

	// 광식
	m_bAttachedCamera		= false;

	//SPIKE
	m_fZoomIn_Increase		 = 50.0f;
	m_fZoomIn2_Increase		 = 20.0f;
	m_nTempZoomLevel		 = 0;
	m_bTimeBombSelected		 = LTFALSE;
	m_bTargetingBomb		 = LTFALSE;
	m_bPlantedBomb			 = LTFALSE;	
	m_bTimeBombPlanting_Level1 = LTFALSE;
	m_bTimeBombPlanting_Level2 = LTFALSE;

	m_fAirLevel				 = MAX_AIR_LEVEL;
	m_fLastAirLossDamageTime = 0.0f;	

	m_fFireRecoilAccel		 = 0.0f;
	m_fCurFireRecoilMaxPitch = 0.0f;
	m_nSpectatorViewIndex	 = MID_ATTACH_FIRST;
	m_bFreeSpectatorMode	 = LTTRUE;

	m_hThirdAttach			 = LTNULL;

	m_bUpdateTargetMgr		 = true;
}

CPlayerMgr::~CPlayerMgr()
{
	if ( m_pHeadBobMgr )
	{
		debug_delete( m_pHeadBobMgr );
		m_pHeadBobMgr = 0;
	}

	if ( m_pCameraOffsetMgr )
	{
		debug_delete( m_pCameraOffsetMgr );
		m_pCameraOffsetMgr = 0;
	}

	if ( m_pFlashLight )
	{
		debug_delete( m_pFlashLight );
		m_pFlashLight = 0;
	}

	if ( m_pGadgetDisabler )
	{
		debug_delete( m_pGadgetDisabler );
		m_pGadgetDisabler = 0;
	}

	if ( m_pSearcher )
	{
		debug_delete( m_pSearcher );
		m_pSearcher = 0;
	}

	if ( m_pMoveMgr )
	{
		debug_delete( m_pMoveMgr );
		m_pMoveMgr = 0;
	}

	if ( m_pVisionModeMgr )
	{
		debug_delete( m_pVisionModeMgr );
		m_pVisionModeMgr = 0;
	}

	if ( m_pAttachButeMgr )
	{
		debug_delete( m_pAttachButeMgr );
		m_pAttachButeMgr = 0;
	}

	if ( m_pLeanMgr )
	{
		debug_delete( m_pLeanMgr );
		m_pLeanMgr = 0;
	}

	if ( m_pClientWeaponMgr )
	{
		debug_delete( m_pClientWeaponMgr );
		m_pClientWeaponMgr = 0;
	}

	if ( m_pPlayerCamera )
	{
		debug_delete( m_pPlayerCamera );
		m_pPlayerCamera = 0;
	}

	if( m_pPVAttachmentMgr )
	{
		debug_delete( m_pPVAttachmentMgr );
		m_pPVAttachmentMgr = LTNULL;
	}

	g_pPlayerMgr = NULL;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::Init
//
//	PURPOSE:	Init the mgr
//
// ----------------------------------------------------------------------- //
LTBOOL CPlayerMgr::Init()
{
	g_pPlayerMgr = this;

	InitTargetMgr();

	m_fSendRate  = 1.0f / GetConsoleFloat( "UpdateRate", DEFAULT_CSENDRATE );
	
	g_CV_CSendRate.Init(g_pLTClient, "CSendRate", NULL, DEFAULT_CSENDRATE);
	g_vtPlayerRotate.Init(g_pLTClient, "PlayerRotate", NULL, 1.0f);
	g_vtCamZoom1MaxDist.Init(g_pLTClient, "CamZoom1MaxDist", NULL, 600.0f);
	g_vtCamZoom2MaxDist.Init(g_pLTClient, "CamZoom2MaxDist", NULL, 1000.0f);
//-------------------------------------------------------------------------
// [bestline] 2004.04.19 카메라 화각 조정
//	g_vtFOVXNormal.Init(g_pLTClient, "FovX", NULL, 90.0f); // 원본
//	g_vtFOVYNormal.Init(g_pLTClient, "FovY", NULL, 78.0f); // 원본

	//[MURSUM] Test...
	g_vtFOVXNormal.Init(g_pLTClient, "FovX", NULL, 85.0f);
	g_vtFOVYNormal.Init(g_pLTClient, "FovY", NULL, 80.0f);
//-------------------------------------------------------------------------
	g_vtFOVYMaxUW.Init(g_pLTClient, "FOVYUWMax", NULL, 77.0f);
	g_vtFOVYMinUW.Init(g_pLTClient, "FOVYUWMin", NULL, 73.0f);
			
	g_vtUWFOVRate.Init(g_pLTClient, "FOVUWRate", NULL, 1.0f);
	
	g_vtUseCamRecoil.Init(g_pLTClient, "CamRecoil", NULL, 0.0f);
	g_vtCamRecoilRecover.Init(g_pLTClient, "CamRecoilRecover", NULL, 0.3f);
	g_vtBaseCamRecoilPitch.Init(g_pLTClient, "CamRecoilBasePitch", NULL, 5.0f);
	g_vtMaxCamRecoilPitch.Init(g_pLTClient, "CamRecoilMaxPitch", NULL, 75.0f);
	g_vtBaseCamRecoilYaw.Init(g_pLTClient, "CamRecoilBaseYaw", NULL, 3.0f);
	g_vtMaxCamRecoilYaw.Init(g_pLTClient, "CamRecoilMaxYaw", NULL, 35.0f);
	g_vtCamRotInterpTime.Init(g_pLTClient, "CamRotInterpTime", NULL, 0.15f);
	g_vtRespawnWaitTime.Init(g_pLTClient, "RespawnWaitTime", NULL, 10.0f);
	g_vtMultiplayerRespawnWaitTime.Init(g_pLTClient, "RespawnMultiWaitTime", NULL, 10.0f);
	g_vtDoomsdayRespawnWaitTime.Init(g_pLTClient, "RespawnDoomsdayWaitTime", NULL, 15.0f);
	g_vtScreenFadeInTime.Init(g_pLTClient, "ScreenFadeInTime", NULL, 3.0f);
	g_vtScreenFadeOutTime.Init(g_pLTClient, "ScreenFadeOutTime", NULL, 5.0f);
	g_vtChaseCamOffset.Init(g_pLTClient, "ChaseCamOffset", NULL, 50.0f);
	g_vtChaseCamPitchAdjust.Init(g_pLTClient, "ChaseCamPitchAdjust", NULL, 0.0f);
	g_vtChaseCamDistUp.Init(g_pLTClient, "ChaseCamDistUp", NULL, 10.0f);
	g_vtChaseCamDistBack.Init(g_pLTClient, "ChaseCamDistBack", NULL, 100.0f);
	g_vtFastTurnRate.Init(g_pLTClient, "FastTurnRate", NULL, 2.3f);
	g_vtNormalTurnRate.Init(g_pLTClient, "NormalTurnRate", NULL, 1.5f);
	g_vtLookUpRate.Init(g_pLTClient, "LookUpRate", NULL, 2.5f);
	g_vtCameraSwayXFreq.Init(g_pLTClient, "CameraSwayXFreq", NULL, 13.0f);
	g_vtCameraSwayYFreq.Init(g_pLTClient, "CameraSwayYFreq", NULL, 5.0f);
	g_vtCameraSwayXSpeed.Init(g_pLTClient, "CameraSwayXSpeed", NULL, 12.0f);
	g_vtCameraSwayYSpeed.Init(g_pLTClient, "CameraSwayYSpeed", NULL, 1.5f);
	g_vtCameraSwayDuckMult.Init(g_pLTClient, "CameraSwayCrouchMultiplier", NULL, 0.5f);
	g_vtModelGlowTime.Init(g_pLTClient, "ModelGlowTime", NULL, 1.5f);
	g_vtModelGlowMin.Init(g_pLTClient, "ModelGlowMin", NULL, -25.0f);
	g_vtModelGlowMax.Init(g_pLTClient, "ModelGlowMax", NULL, 75.0f);
    g_vtActivateOverride.Init(g_pLTClient, "ActivateOverride", " ", 0.0f);
    g_vtCamDamage.Init(g_pLTClient, "CamDamage", NULL, 1.0f);
    g_vtCamDamagePitch.Init(g_pLTClient, "CamDamagePitch", NULL, 1.0f);
    g_vtCamDamageRoll.Init(g_pLTClient, "CamDamageRoll", NULL, 1.0f);

    //g_vtCamDamageTime1.Init(g_pLTClient, "CamDamageTime1", NULL, 0.05f);
    //g_vtCamDamageTime2.Init(g_pLTClient, "CamDamageTime2", NULL, 0.5f);
    //g_vtCamDamageMinPitchVal.Init(g_pLTClient, "CamDamageMinPitchVal", NULL, 2.5f);
    //g_vtCamDamageMaxPitchVal.Init(g_pLTClient, "CamDamageMaxPitchVal", NULL, 5.0f);

	g_vtCamDamageTime1.Init(g_pLTClient, "CamDamageTime1", NULL, 0.0f);
    g_vtCamDamageTime2.Init(g_pLTClient, "CamDamageTime2", NULL, 0.05f);
	g_vtCamDamageMinPitchVal.Init(g_pLTClient, "CamDamageMinPitchVal", NULL, 5.0f);
    g_vtCamDamageMaxPitchVal.Init(g_pLTClient, "CamDamageMaxPitchVal", NULL, 5.0f);


    g_vtCamDamageMinRollVal.Init(g_pLTClient, "CamDamageMinRollVal", NULL, 2.5f);
    g_vtCamDamageMaxRollVal.Init(g_pLTClient, "CamDamageMaxRollVal", NULL, 5.0f);    
	g_vtAlwaysHUD.Init(g_pLTClient, "AlwaysHUD", NULL, 0.0f);
	g_vtDamageFadeRate.Init(g_pLTClient, "DamageFadeRate", NULL, 0.5f);
	g_vtAdaptiveMouse.Init(g_pLTClient, "AdaptiveMouse", NULL, 0.0f);
	g_vtAdaptiveMouseMaxOffset.Init(g_pLTClient, "AdaptiveMouseMaxOffset", NULL, 0.1f);
	g_vtShowSoundFilterInfo.Init(g_pLTClient, "SoundFilterInfo", NULL, 0.0f);
	g_vtMultiplayerDeathCamMoveTime.Init( g_pLTClient, "MultiplayerDeathCamMoveTime", NULL, 0.5f );
	g_vtMultiAttachDeathCamMaxTime.Init( g_pLTClient, "MultiplayerDeathAttachCamMaxTime", NULL, 5.0f );
	g_vtAttachedCamInterpolationRate.Init( g_pLTClient, "AttachedCamIntrepolationRate", NULL, 0.1f );

	// Default these to use values specified in the weapon...(just here for
	// tweaking values...)

	g_vtFireJitterDecayTime.Init(g_pLTClient, "FireJitterDecayTime", NULL, -1.0f);
	g_vtFireJitterMaxPitchDelta.Init(g_pLTClient, "FireJitterMaxPitchDelta", NULL, -1.0f);

	g_vSVModelColor = g_pLayoutMgr->GetSpyVisionModelColor();
	m_vSVLightScale = g_pLayoutMgr->GetSpyVisionLightScale();

	// Create the camera...

	ObjectCreateStruct theStruct;
	INIT_OBJECTCREATESTRUCT(theStruct);

	theStruct.m_ObjectType = OT_CAMERA;

	m_hCamera = g_pLTClient->CreateObject(&theStruct);
	_ASSERT(m_hCamera);

	LTVector	vCamPos;
	g_pLTClient->GetObjectPos( m_hCamera, &vCamPos );

	ResetCamera();

	m_pHeadBobMgr->Init();
	m_pCameraOffsetMgr->Init();
	
	//m_pGadgetDisabler no init
	//m_pSearcher
	m_pMoveMgr->Init();
	//m_pVisionModeMgr no init
	m_pAttachButeMgr->Init();
	m_pLeanMgr->Init();
	m_pClientWeaponMgr->Init();
	m_pPVAttachmentMgr->Init();

	s_fDeathDelay = g_pLayoutMgr->GetDeathDelay();

	// Player camera (non-1st person) stuff...

	if (m_pPlayerCamera->Init(g_pLTClient))
	{
		InitPlayerCamera();
		m_pPlayerCamera->GoFirstPerson();
	}
	else
	{
		g_pGameClientShell->CSPrint ("Could not init player camera!");
	}

	ClearDamageSectors();

	m_bSpyVision = false;

	// Added By Bestline ==>
	// Time : 2004-07-13 오전 11:49:43
	m_bTestCamera = false;
	// Added By Bestline <==

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::Term
//
//	PURPOSE:	Term the mgr
//
// ----------------------------------------------------------------------- //
void CPlayerMgr::Term()
{
	// terminate the weapon mgr
	m_pClientWeaponMgr->Term();

	m_pPVAttachmentMgr->Term();

	g_pPlayerMgr = NULL;

	if (m_hCamera)
	{
        g_pLTClient->RemoveObject(m_hCamera);
        m_hCamera = NULL;
	}

	if( m_pTargetMgr )
	{
		debug_delete( m_pTargetMgr );
		m_pTargetMgr = LTNULL;
	}

}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CInterfaceMgr::Save
//
//	PURPOSE:	Save the interface info
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::Save(ILTMessage_Write *pMsg, SaveDataState eSaveDataState)
{
	// save the class information
	m_pMoveMgr->Save(pMsg, eSaveDataState);
	m_pClientWeaponMgr->Save(pMsg);

	pMsg->Writebool(m_bSpectatorMode != LTFALSE);
	pMsg->WriteLTRotation(m_rRotation);
	pMsg->Writebool(m_bLastSent3rdPerson != LTFALSE);
	pMsg->Writebool(m_bAllowPlayerMovement != LTFALSE);
	pMsg->Writebool(m_bLastAllowPlayerMovement != LTFALSE);
	pMsg->Writebool(m_bWasUsingExternalCamera != LTFALSE);
	pMsg->Writebool(m_bUsingExternalCamera != LTFALSE);
	pMsg->Writeuint8(m_ePlayerState);
	pMsg->Writeuint8(m_nSoundFilterId);
	pMsg->Writeuint8(m_nGlobalSoundFilterId);

	pMsg->Writebool(m_pFlashLight->IsOn() != LTFALSE);

	pMsg->Writefloat(m_fPitch);
	pMsg->Writefloat(m_fYaw);
	pMsg->Writefloat(m_fRoll);

	pMsg->Writefloat(m_fPlayerPitch);
	pMsg->Writefloat(m_fPlayerYaw);
	pMsg->Writefloat(m_fPlayerRoll);

	pMsg->Writefloat(m_fModelAttachPitch);
	pMsg->Writefloat(m_fModelAttachYaw);
	pMsg->Writefloat(m_fModelAttachRoll);

	m_fPitchBackup = m_fPitch;
	m_fYawBackup = m_fYaw;

    pMsg->Writefloat(m_fPitchBackup);
    pMsg->Writefloat(m_fYawBackup);

    pMsg->Writefloat(m_fFireJitterPitch);
    pMsg->Writefloat(m_fFireJitterYaw);
    pMsg->Writefloat(m_fContainerStartTime);
    pMsg->Writefloat(m_fFovXFXDir);
    pMsg->Writefloat(m_fSaveLODScale);

	
	pMsg->Writeuint8( m_nCarryingObject );
	pMsg->Writebool( m_bCanDropCarriedObject );

	pMsg->Writebool( m_bCameraAttachedToHead );

	pMsg->Writebool( m_pMoveMgr->DuckLock() ? true : false );

	pMsg->Writebool( m_pMoveMgr->IsDucking() ? true : false );

	pMsg->Writefloat( m_fMultiplayerDeathCamMoveTimer );
	pMsg->Writefloat( m_fMultiAttachDeathCamTimer );

	pMsg->Writebool( m_bLerpAttachedCamera );
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::Load
//
//	PURPOSE:	Load the interface info
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::Load(ILTMessage_Read *pMsg, SaveDataState eLoadDataState)
{
	// load the contained member classes
	m_pMoveMgr->Load(pMsg, eLoadDataState);
	m_pClientWeaponMgr->Load(pMsg);

	m_bSpectatorMode            = pMsg->Readbool() ? LTTRUE : LTFALSE;
	m_rRotation                 = pMsg->ReadLTRotation();
	m_bLastSent3rdPerson        = pMsg->Readbool() ? LTTRUE : LTFALSE;
	m_bAllowPlayerMovement      = pMsg->Readbool() ? LTTRUE : LTFALSE;
	m_bLastAllowPlayerMovement  = pMsg->Readbool() ? LTTRUE : LTFALSE;
	m_bWasUsingExternalCamera   = pMsg->Readbool() ? LTTRUE : LTFALSE;
	m_bUsingExternalCamera      = pMsg->Readbool() ? LTTRUE : LTFALSE;
	m_ePlayerState              = (PlayerState) pMsg->Readuint8();
	m_nSoundFilterId            = pMsg->Readuint8();
	m_nGlobalSoundFilterId      = pMsg->Readuint8();

	if (pMsg->Readbool())
	{
		m_pFlashLight->TurnOn();
	}

	m_fPitch                    = pMsg->Readfloat();
	m_fYaw                      = pMsg->Readfloat();
	m_fRoll                     = pMsg->Readfloat();

	m_fPlayerPitch              = pMsg->Readfloat();
	m_fPlayerYaw                = pMsg->Readfloat();
	m_fPlayerRoll               = pMsg->Readfloat();

	m_fModelAttachPitch         = pMsg->Readfloat();
	m_fModelAttachYaw           = pMsg->Readfloat();
	m_fModelAttachRoll          = pMsg->Readfloat();

	m_fPitchBackup              = pMsg->Readfloat();
	m_fYawBackup                = pMsg->Readfloat();

    m_fFireJitterPitch          = pMsg->Readfloat();
    m_fFireJitterYaw	        = pMsg->Readfloat();
    m_fContainerStartTime       = pMsg->Readfloat();
    m_fFovXFXDir                = pMsg->Readfloat();
    m_fSaveLODScale             = pMsg->Readfloat();

	uint8 nCarryingObject = 0;
	if( g_pVersionMgr->GetCurrentSaveVersion( ) < CVersionMgr::kSaveVersion__1_3 )
	{
		bool bCarryingObject	= pMsg->Readbool();
		if (bCarryingObject)
			nCarryingObject = CFX_CARRY_BODY;
		else
			nCarryingObject = CFX_CARRY_NONE;
	}
	else
	{
		nCarryingObject		= pMsg->Readuint8();
	}
	SetCarryingObject( nCarryingObject, false );

	m_bCanDropCarriedObject		= pMsg->Readbool();

	m_bCameraAttachedToHead		= pMsg->Readbool();

	m_pMoveMgr->SetDuckLock( pMsg->Readbool() ? LTTRUE : LTFALSE );

	m_pMoveMgr->SetDucking( pMsg->Readbool() ? LTTRUE : LTFALSE );

	m_fMultiplayerDeathCamMoveTimer = pMsg->Readfloat();
	m_fMultiAttachDeathCamTimer = pMsg->Readfloat();


	if( g_pVersionMgr->GetCurrentSaveVersion( ) > CVersionMgr::kSaveVersion__1_2 )
	{
		m_bLerpAttachedCamera = pMsg->Readbool();
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::OnEnterWorld()
//
//	PURPOSE:	Handle entering world
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::OnEnterWorld()
{
	m_vShakeAmount.Init();
	m_ePlayerState            = PS_UNKNOWN;

	m_bPlayerUpdated          = LTFALSE;
	m_nZoomView               = 0;
	m_bZooming                = LTFALSE;
	m_bZoomingIn              = LTFALSE;

	m_eCurContainerCode       = CC_NO_CONTAINER;
	m_bInSafetyNet			  = false;

	// 광식
	m_bInTimeBombArea		  = false;

	m_bCameraAttachedToHead	= false;
	m_bLerpAttachedCamera	= false;

	m_bUpdateTargetMgr		 = true;

	m_pHeadBobMgr->OnEnterWorld();

	SetExternalCamera(LTFALSE);

	if( g_pInterfaceMgr->GetGameState() != GS_MANDATEHOST &&
		g_pInterfaceMgr->GetGameState() != GS_REJOINHOST )
	{
		m_bCameraPosInited        = LTFALSE;
	}

	m_nPlayerInfoChangeFlags |= CLIENTUPDATE_PLAYERROT | CLIENTUPDATE_ALLOWINPUT;

	m_pMoveMgr->OnEnterWorld();

	m_nCarryingObject		= CFX_CARRY_NONE;
	m_bCanDropCarriedObject	= false;

	m_fDistanceIndicatorPercent = -1.0f;
	g_pHUDMgr->QueueUpdate( kHUDDistance );

	m_pClientWeaponMgr->OnEnterWorld();

	//[MURSUM]
	m_bFirstRespawn			= true;

	m_bFreeSpectatorMode = g_GameDoc.GetRoomOption()->bFreeSpectator;
#ifdef GAME_SHOW
	if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
	{
		m_bFreeSpectatorMode = LTTRUE;
	}	
#endif
	g_pNickList->ScreenDimChanged();

	m_pFlashLight->Init();
	m_pFlashLight->TurnOff();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::OnExitWorld()
//
//	PURPOSE:	Handle leaving world
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::OnExitWorld()
{
	m_bStartedPlaying = LTFALSE;

	ClearPlayerModes();

	m_pPlayerCamera->AttachToObject(NULL);   // Detatch camera

	m_pFlashLight->TurnOff();
	m_pFlashLight->Term();

	m_pMoveMgr->OnExitWorld();
	
	if (m_hContainerSound)
	{
		g_pLTClient->SoundMgr()->KillSound(m_hContainerSound);
		m_hContainerSound = NULL;
	}

	m_pClientWeaponMgr->OnExitWorld();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::OnMessage()
//
//	PURPOSE:	Handle client messages
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerMgr::OnMessage(uint8 messageID, ILTMessage_Read *pMsg)
{
	switch(messageID)
	{
		case MID_SHAKE_SCREEN:				HandleMsgShakeScreen			(pMsg);	break;
		case MID_PLAYER_STATE_CHANGE:		HandleMsgPlayerStateChange		(pMsg);	break;
		case MID_CLIENT_PLAYER_UPDATE:		HandleMsgClientPlayerUpdate		(pMsg);	break;
		case MID_PLAYER_DAMAGE:				HandleMsgPlayerDamage			(pMsg);	break;
		case MID_PLAYER_ORIENTATION:		HandleMsgPlayerOrientation		(pMsg);	break;
		case MID_CHANGE_WORLDPROPERTIES:	HandleMsgChangeWorldProperties	(pMsg);	break;
		case MID_GADGETTARGET:				HandleMsgGadgetTarget			(pMsg); break;
		case MID_SEARCH:					HandleMsgSearch					(pMsg); break;
		case MID_ADD_PUSHER:				HandleMsgAddPusher				(pMsg); break;
		case MID_OBJECTIVES_DATA:			HandleMsgObjectivesData			(pMsg); break;
		case MID_WEAPON_RELOAD:				HandleMsgWeaponReload			(pMsg); break;	// 광식 추가
		case MID_PLAYER_ATTACH:				HandleMsgAttach					(pMsg); break;	// 광식 추가
		case MID_PROJECTILE_FX:				HandleProjectileFX				(pMsg); break;
		case MID_PRELOAD_WEAPON:			HandlePreloadWeapon				(pMsg); break;
		default:							return LTFALSE;	break;
	}
	
	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::OnKeyDown(int key, int rep)
//
//	PURPOSE:	Handle key down notification
//				Try to avoid using OnKeyDown and OnKeyUp as they
//				are not portable functions
//
// ----------------------------------------------------------------------- //
//static unsigned long staticfrequency = 19050;
//static HLTSOUND hTestSound = LTNULL;
LTBOOL CPlayerMgr::OnKeyDown(int key, int rep)
{
	if (!IsPlayerInWorld()) return LTFALSE;
	if (IsPlayerDead())
	{
		// See if we can respawn.  For SP, this just makes us wait
		// a little bit before we can go to the mission failure screen.
		if (g_pLTClient->GetTime() > m_fEarliestRespawnTime)
		{
			// Death is a mission failure in sp.
			if (!IsMultiplayerGame())
			{
				g_pMissionMgr->HandleMissionFailed();
				return LTTRUE;
			}
		}
	}

	// test test
	//--------------------------------------------------------------------------
	/*
	if( key == VK_END )
	{

		if(!hTestSound)
		{
			hTestSound = g_pClientSoundMgr->PlaySoundLocal( "sa_weapons\\snd\\flyby2.wav",
														SOUNDPRIORITY_MISC_HIGH, PLAYSOUND_LOOP, 100, 
														1.0f, WEAPONS_SOUND_CLASS );		
		}
		
		if( hTestSound )
		{
			if( LT_OK == g_pLTClient->SoundMgr()->SetFrequency(hTestSound, staticfrequency) )
			{				
				staticfrequency += 200;
			}
			else
			{
				g_pLTClient->CPrint("!!!! Error");
			}
		}
		
	}
	*/
	//--------------------------------------------------------------------------

	return LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::OnKeyUp(int key)
//
//	PURPOSE:	Handle key up notification
//
// ----------------------------------------------------------------------- //
LTBOOL CPlayerMgr::OnKeyUp(int key)
{
#ifndef _FINAL
	// 광식 : Model Test
	//--------------------------------------------------------
	// test test
	//--------------------------------------------------------------------------
	/*
	if( key == VK_END )
	{
		if( hTestSound )
		{
			//g_pLTClient->SoundMgr()->KillSound(hTestSound);
			g_pLTClient->SoundMgr()->KillSoundLoop(hTestSound);
            hTestSound = NULL;
			staticfrequency = 19050;
		}
	}
	*/
	//--------------------------------------------------------------------------

	if( key == VK_NEXT )
	{
		g_pGameClientShell->CloneCharacter();
	}
	//--------------------------------------------------------
	if ( key == VK_HOME )
	{
		m_bTestCamera = false;
		static BOOL	bMode = TRUE;

		if( bMode == TRUE )
		{
			bMode = FALSE;

			ShowPlayer(LTTRUE);

			m_pClientWeaponMgr->HideWeapons();
			m_pPlayerCamera->GoChaseMode();
			m_pPlayerCamera->CameraUpdate(0.0f);
		}else
		{
			bMode = TRUE;

			ShowPlayer(LTFALSE);

			m_pClientWeaponMgr->ShowWeapons();
			m_pPlayerCamera->GoFirstPerson();
			m_pPlayerCamera->CameraUpdate(0.0f);
		}
	}

	if ( key == VK_INSERT )
	{	
		if( m_bTestCamera == false )
		{
			ShowPlayer(LTTRUE);

			m_pClientWeaponMgr->HideWeapons();
			m_pPlayerCamera->GoChaseMode();
			m_pPlayerCamera->CameraUpdate(0.0f);

			m_bTestCamera = true;
		}
		else
		{		
			ShowPlayer(LTFALSE);

			m_pClientWeaponMgr->ShowWeapons();
			m_pPlayerCamera->GoFirstPerson();
			m_pPlayerCamera->CameraUpdate(0.0f);

			m_bTestCamera = false;
		}
	}
#endif

//[SPIKE] 수정 ==================================================================	
	if( key == VK_SPACE && IsPlayerDead() && IsRevivePlayerGameType() )
	{
		if( LTTRUE == g_pHUDPlayerState->IsAttachBarVisible() )
		{			
			if(!m_bFreeSpectatorMode)
			{
				if(m_nSpectatorViewIndex != MID_ATTACH_FIRST)
				{
					return DoFirstViewMode();
				}
			}
			else
			{
				m_nSpectatorViewIndex++;
				if(m_nSpectatorViewIndex > MID_DETACH_OBSERVER)
				{
					m_nSpectatorViewIndex = MID_ATTACH_FIRST;
				}
				
				if(m_nSpectatorViewIndex == MID_ATTACH_FIRST)
				{
					m_bAttachedCamera = true;
				}
				else
				{
					m_bAttachedCamera = false;
				}
				
				LTRESULT ltResult;
				CAutoMessage cMsg;
				cMsg.Writeuint8( MID_PLAYER_ATTACH );
				cMsg.Writeuint8( m_nSpectatorViewIndex );
				ltResult = g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
				ASSERT( LT_OK == ltResult );
				
				return LTTRUE;
				
			}		
		}
	}
	/*
		// 이쪽에서는 Message만 날리고 요청 하는 코드만 넣자.
		if( key == VK_SPACE )
		{
			// Spectator Mode 때문에 수정
			//if( m_ePlayerState == PS_DEAD && IsRevivePlayerGameType() )
			if( IsPlayerDead() && IsRevivePlayerGameType() )
			{
				m_bAttachedCamera = true;
				
				LTRESULT ltResult;
				CAutoMessage cMsg;
				cMsg.Writeuint8( MID_PLAYER_ATTACH );
				cMsg.Writeuint8( MID_ATTACH_FIRST );
				ltResult = g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
				ASSERT( LT_OK == ltResult );
	
				return LTTRUE;
			}
		}
	
		if( key == VK_DELETE )
		{
			// Spectator Mode 때문에 수정
			//if( m_ePlayerState == PS_DEAD && IsRevivePlayerGameType() )
			if( IsPlayerDead() && IsRevivePlayerGameType() )
			{
				m_bAttachedCamera = false;
	
				LTRESULT ltResult;
				CAutoMessage cMsg;
				cMsg.Writeuint8( MID_PLAYER_ATTACH );
				cMsg.Writeuint8( MID_ATTACH_THIRD );
				ltResult = g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
				ASSERT( LT_OK == ltResult );
			}
			return LTTRUE;
		}
	
		// Observer 추가
		if( key == VK_END )
		{
			if( IsPlayerDead() && IsRevivePlayerGameType() )
			{
				LTRESULT ltResult;
				CAutoMessage cMsg;
				cMsg.Writeuint8( MID_PLAYER_ATTACH );
				cMsg.Writeuint8( MID_DETACH_OBSERVER );
				ltResult = g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
				ASSERT( LT_OK == ltResult );
			}
			return LTTRUE;
		}*/
	
//[SPIKE] 수정 ==================================================================
	



	/*
	// 무기 버리기..
	if ( m_ePlayerState == PS_ALIVE &&
		 g_pLTClient->IsCommandOn(COMMAND_ID_ACTIVATE) &&
		 GetCurrentClientWeapon()->GetState() != W_SELECT &&
		 GetCurrentClientWeapon()->GetState() != W_DESELECT )
	{
		// 이부분 약간 ~
		uint8 nWeaponClass = g_pWeaponMgr->GetWeaponClass(GetCurrentClientWeapon()->GetWeaponId());

		if( nWeaponClass == 3 || nWeaponClass == 4 || nWeaponClass == 5 ) return LTTRUE;
		
		LTRotation rRot;
		LTVector vPos;
		LTVector vForward;
		HOBJECT hCamera = GetCamera();
		g_pLTClient->GetObjectRotation( hCamera, &rRot );
		g_pLTClient->GetObjectPos( hCamera, &vPos );
		vForward = rRot.Forward();
		CAutoMessage cMsg;
		cMsg.Writeuint8( MID_PROJECTILE );
		cMsg.Writeuint8( MPROJ_WEAPON_THROW );
		cMsg.Writeuint8( GetCurrentClientWeapon()->GetWeaponId() );
		cMsg.WriteLTVector( vForward );
		cMsg.WriteLTVector( vPos );
		cMsg.Writeuint8( nWeaponClass - 1 );
		g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );

		g_pPlayerStats->ThrowWeapon(GetCurrentClientWeapon()->GetWeaponId(), nWeaponClass);

		g_pWpnChooser->RemoveWeapon( nWeaponClass - 1 );

		// 다음 무기로 Change
		uint8 nNextWeapon = GetClientWeaponMgr()->GetNextWeaponId( GetCurrentClientWeapon()->GetWeaponId(), 0);
		WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(nNextWeapon);
		if( pWeapon )
		{
			ChangeWeapon( nNextWeapon, pWeapon->nDefaultAmmoId );
			m_pClientWeaponMgr->ClearLastWeapon();
		}
	}
	*/
	
	return LTFALSE;
}

//[SPIKE]
LTBOOL 
CPlayerMgr::DoFirstViewMode()
{
	m_nSpectatorViewIndex = MID_ATTACH_FIRST;
	m_bAttachedCamera = true;
		
	LTRESULT ltResult;
	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_PLAYER_ATTACH );
	cMsg.Writeuint8( m_nSpectatorViewIndex );
	ltResult = g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
	ASSERT( LT_OK == ltResult );

	return LTTRUE;
}


// 광식
void CPlayerMgr::OnLButtonDown(int x, int y)
{
	if( IsPlayerDead() && IsRevivePlayerGameType() &&
		LTTRUE == g_pHUDPlayerState->IsAttachBarVisible() )
	{
		LTRESULT ltResult;
		CAutoMessage cMsg;
		cMsg.Writeuint8( MID_PLAYER_ATTACH );
		cMsg.Writeuint8( m_nSpectatorViewIndex );		
		ltResult = g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
	}
}

void CPlayerMgr::OnRButtonDown(int x, int y)
{
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::PreUpdate()
//
//	PURPOSE:	Handle client pre-updates
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::PreUpdate()
{
	// Conditions in which we don't want to clear the screen

	if ((m_ePlayerState == PS_UNKNOWN && g_pGameClientShell->IsWorldLoaded()))
	{
		return;
	}

	// See if we're using an external camera now - if so, clear the screen
	// immediately, and add to the clearscreen count
	if (m_bUsingExternalCamera && !m_bWasUsingExternalCamera)
	{
        m_bWasUsingExternalCamera = LTTRUE;
		g_pInterfaceMgr->AddToClearScreenCount();
	}
	else if (m_bWasUsingExternalCamera && !m_bUsingExternalCamera)
	{
        m_bWasUsingExternalCamera = LTFALSE;
		g_pInterfaceMgr->AddToClearScreenCount();
	}

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::Update()
//
//	PURPOSE:	Handle client updates
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::Update()
{

	// Update models (powerups) glowing...

	UpdateModelGlow();

	UpdateDamage();
	
	if( g_vtUseSoundFilters.GetFloat() )
	{
		ILTClientSoundMgr *pSoundMgr = (ILTClientSoundMgr *)g_pLTClient->SoundMgr();
		if( pSoundMgr )
		{
			if( GetMoveMgr()->IsHeadInLiquid() )
			{
				pSoundMgr->SetReverbFilter(true);
			}
			else
			{
				pSoundMgr->SetReverbFilter(false);
			}
		}
	}

	if (IsPlayerDead())
	{
	// Added By Bestline ==>
	// Time : 2004-07-16 오후 9:12:17
	// 원 코드처럼 activate키나 fire키를 눌렀을때만 respawn
	// 하지 않고 시간이 되면 자동으로 respawn 하기위해 수정함.
		if (IsMultiplayerGame() && !IsRevivePlayerGameType() )
		{					
            if (g_pLTClient->GetTime() > m_fEarliestRespawnTime)
			{
				// Allow respawn after a certain amount of time. This way the Player 
				// can remain in 'limbo' for as long as they want and wait to be revived.

				HandleRespawn();				
			}
		}
	// Added By Bestline <==

		if (!IsMultiplayerGame() && !g_pGameClientShell->IsGamePaused())
		{
			if (s_fDeadTimer < s_fDeathDelay)
			{
				s_fDeadTimer += g_pGameClientShell->GetFrameTime();
			}
			else
			{
				 LTBOOL bHandleMissionFailed = LTTRUE;

				if (g_pInterfaceMgr->FadingScreen() && g_pInterfaceMgr->FadingScreenIn())
				{
					bHandleMissionFailed = g_pInterfaceMgr->ScreenFadeDone();
				}

				if (bHandleMissionFailed)
				{ 
					g_pMissionMgr->HandleMissionFailed();
				}
			}
		}		
	}


	if (g_pLTClient->IsCommandOn(COMMAND_ID_ACTIVATE))
	{
		// 광식 : 떨어져 있는 무기에 Targeting 되었을 때 Action Key를 누르면 줍도록..
		//----------------------------------------------------------------
		/*
		if( GetTargetMgr()->IsTargetInRange() )
		{
			uint32 ObjType = 0;
			HOBJECT hTargetObj = GetTargetMgr()->GetTargetObject();
			g_pCommonLT->GetObjectType(hTargetObj, &ObjType);
			uint32 dwUserFlags = 0;
			g_pCommonLT->GetObjectFlags(hTargetObj, OFT_User, dwUserFlags);

			if( (ObjType & OT_MODEL) && (dwUserFlags & USRFLG_CAN_ACTIVATE) )
			{				
				CActivationData data = GetTargetMgr()->GetActivationData();
				data.m_nType = MID_ACTIVATE_WEAPON;				
				CAutoMessage cMsg;
				cMsg.Writeuint8(MID_PLAYER_ACTIVATE);
				data.Write(cMsg);
				g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);
				return;
			}
		}
		else
		{
			LTRotation rRot		= GetPlayerCamera()->GetRotation();
			LTVector vForward	= rRot.Forward();
			CAutoMessage cMsg;
			cMsg.Writeuint8( MID_PROJECTILE );
			cMsg.Writeuint8( MPROJ_WEAPON_THROW );
			cMsg.Writeuint8( GetCurrentClientWeapon()->GetWeaponId() );
			cMsg.WriteLTVector( vForward );
			g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );			
		}
		*/
		//----------------------------------------------------------------

		//if the activate key is down,
		//  and we're in range
		//	and we have not started to auto-switch to a gadget i.e (m_nPreGadgetWeapon == WMGR_INVALID_ID)
		//  and we are not in the process of switching weapons 
		//then try to switch to a gadget

		/*
		if (GetTargetMgr()->IsTargetInRange() && (m_nPreGadgetWeapon == WMGR_INVALID_ID) && !m_bSwitchingWeapons)
		{
			DamageType eDT = GetTargetMgr()->RequiredGadgetDamageType();
			UseGadget(eDT);
		}
		*/
	}
	else
	{
		//if the activate key is not down, 
		//	and we have started to auto-switch to a gadget i.e (m_nPreGadgetWeapon != WMGR_INVALID_ID)
		//  and we are not in the process of switching weapons 
		//then switch back to the old weapon
		if ((m_nPreGadgetWeapon != WMGR_INVALID_ID) && !m_bSwitchingWeapons)
		{
			m_bChangingToGadget = false;
			ChangeWeapon(m_nPreGadgetWeapon);
		}
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::FirstUpdate
//
//	PURPOSE:	Handle first update (each level)
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::FirstUpdate()
{
	// Force the player camera to update...

	UpdatePlayerCamera();

#ifdef USE_EAX20_HARDWARE_FILTERS

	m_nGlobalSoundFilterId = 0;

	char aGlobalSoundFilter[256] = {0};
	char* pGlobalSoundFilter = NULL;
	if (g_pLTClient->GetSConValueString("GlobalSoundFilter", aGlobalSoundFilter, sizeof(aGlobalSoundFilter)) == LT_OK)
	{
		pGlobalSoundFilter = aGlobalSoundFilter;
	}

	if (pGlobalSoundFilter && pGlobalSoundFilter[0])
	{
		SOUNDFILTER* pFilter = g_pSoundFilterMgr->GetFilter(pGlobalSoundFilter);
		if (pFilter)
		{
			m_nGlobalSoundFilterId = pFilter->nId;

			// set the global filter, if appropriate
			if (g_vtUseSoundFilters.GetFloat())
			{
				bool bFilterOK = true;
				ILTClientSoundMgr *pSoundMgr = (ILTClientSoundMgr *)g_pLTClient->SoundMgr();

				if ( !g_pSoundFilterMgr->IsUnFiltered( pFilter ) )
				{
			   		if ( pSoundMgr->SetSoundFilter( pFilter->szFilterName ) == LT_OK )
					{
						for (int i=0; i < pFilter->nNumVars; i++)
						{
							if ( pSoundMgr->SetSoundFilterParam(pFilter->szVars[i], pFilter->fValues[i]) != LT_OK )
								bFilterOK = false;
						}
					}
					else
					{
						bFilterOK = false;
					}
				}
				else
				{
					// Not filtered.
					bFilterOK = false;
				}

				pSoundMgr->EnableSoundFilter( bFilterOK );

#ifndef _FINAL
				if (g_vtShowSoundFilterInfo.GetFloat())
				{
					g_pLTClient->CPrint("Entering (Global) sound filter '%s' (%s)", 
						pFilter->szName, (bFilterOK ? "Enabled" : "Disabled"));

					// Display detailed filter info if necessary...
					if (g_vtShowSoundFilterInfo.GetFloat() > 1)
					{
						g_pLTClient->CPrint("  FilterName: '%s'", pFilter->szFilterName);
						for (int i=0; i < pFilter->nNumVars; i++)
						{
							g_pLTClient->CPrint("  '%s' = '%f'", pFilter->szVars[i], pFilter->fValues[i]);
						}
					}
				}
#endif // _FINAL

			}

		}
	}

#endif  // USE_EAX20_HARDWARE_FILTERS

	// Force us to re-evaluate what container we're in.  We call
	// UpdateContainers() first to make sure any container changes
	// have been accounted for, then we clear the container code
	// and force an update (this is done for underwater situations like
	// dying underwater and respawning, and also for picking up intelligence
	// items underwater)...
	UpdateContainers();
	ClearCurContainerCode();
	UpdateContainers();

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::FirstPlayingUpdate()
//
//	PURPOSE:	Handle when the player first starts playing
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::FirstPlayingUpdate()
{
	// Set Initial cheats...
	if (g_pCheatMgr)
	{
        uint8 nNumCheats = g_pClientButeMgr->GetNumCheatAttributes();
		char strCheat[64];

        for (uint8 i=0; i < nNumCheats; i++)
		{
			strCheat[0] = '\0';
			g_pClientButeMgr->GetCheat(i, strCheat, ARRAY_LEN(strCheat));
			if (strCheat[0])
			{
				g_pCheatMgr->Check(strCheat);
			}
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdatePlaying()
//
//	PURPOSE:	Handle updating playing (normal) game state
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdatePlaying()
{
	// First time UpdatePlaying is called...

	if (!m_bStartedPlaying)
	{	 
        m_bStartedPlaying = LTTRUE;
		FirstPlayingUpdate();
	}

	// Update player movement...
	
	m_pMoveMgr->Update();


	// Update our camera offset mgr...

	m_pCameraOffsetMgr->Update();


	// Update Player...

	UpdatePlayer();


	// Keep track of what the player is doing...

	UpdatePlayerFlags();


	// Update head-bob/head-cant camera offsets...

	m_pHeadBobMgr->Update();


	// Update duck camera offset...

	UpdateDuck();


	// Update the camera's position...

	UpdateCamera();


	// See if we are tracking distance to an object..

	UpdateDistanceIndicator();


	// Update player leaning...

	m_pLeanMgr->Update();

	//updates dependant on camera position and rotation should be handled after this point

	// Update the targetting info...

	//[MURSUM] 부하가 많이 걸리는 작업. 한프레임식 건너자~!
	if( m_bUpdateTargetMgr )
	{
		m_pTargetMgr->Update();
		m_bUpdateTargetMgr = false;
	}
	else
	{
		m_bUpdateTargetMgr = true;
	}

	// Update the Gadget Disabler...

	m_pGadgetDisabler->Update();

	// Update the Searcher...

	m_pSearcher->Update();

	// Update the vision modes...

	m_pVisionModeMgr->Update();

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::PostUpdate()
//
//	PURPOSE:	Handle post updates - after the scene is rendered
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::PostUpdate()
{
	// Update container effects...
	if(m_bStartedPlaying)
	{
		UpdateContainers();
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateDuck()
//
//	PURPOSE:	Update ducking camera offset
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdateDuck()
{
	// Can't duck when free movement...
	// 광식
	// [05.02.11] : ControlFlags Test
    if (!g_pPlayerMgr->GetMoveMgr()->IsBodyInLiquid() && 
		(g_pPlayerMgr->GetMoveMgr()->IsBodyOnLadder() ||
		/*g_pPlayerMgr->IsPlayerDead() ||*/
        IsFreeMovement(m_eCurContainerCode)) ) 
	{
		// Reset ducking parameters...
		m_fCamDuck = 0.0f;
        m_bStartedDuckingDown = LTFALSE;
		return;
	}

    float fTime = g_pLTClient->GetTime();

	float fBodyMult = 1.0f;

	if ( (m_dwPlayerFlags & BC_CFLG_DUCK) || m_bCameraDip)
	{
        m_bStartedDuckingUp = LTFALSE;

		// Get the difference in  crouch height...
		
		float fCrouchDelta = g_pMoveMgr->GetCrouchHeightDifference();
		
		// If dipping don't use the crouch distance...

		if( m_bCameraDip )
			fCrouchDelta = 0.0f;

		// See if the duck just started...

		if (!m_bStartedDuckingDown)
		{
            m_bStartedDuckingDown = LTTRUE;
			m_fStartDuckTime = fTime - g_pLTClient->GetFrameTime();
		}

		m_fCamDuck += m_fDuckDownV * (fTime - m_fStartDuckTime);

		if (m_fCamDuck < (m_fMaxDuckDistance - fCrouchDelta) )
		{
			m_bCameraDip = false;
			m_fCamDuck = m_fMaxDuckDistance - fCrouchDelta;
		}

	}
	else if (m_fCamDuck < 0.0) // Raise up
	{
        m_bStartedDuckingDown = LTFALSE;

		if (!m_bStartedDuckingUp)
		{
			m_fStartDuckTime = fTime - g_pLTClient->GetFrameTime();
            m_bStartedDuckingUp = LTTRUE;
		}

		if( IsCarryingHeavyObject() )
			fBodyMult = 0.01f;

		m_fCamDuck += m_fDuckUpV * (fTime - m_fStartDuckTime) * fBodyMult;

		if (m_fCamDuck > 0.0f)
		{
			m_fCamDuck = 0.0f;
		}
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::HandleMsgShakeScreen()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HandleMsgShakeScreen (ILTMessage_Read *pMsg)
{
	LTVector vAmount = pMsg->ReadLTVector();
	ShakeScreen(vAmount);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::ShakeScreen()
//
//	PURPOSE:	Shanke, rattle, and roll
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::ShakeScreen(LTVector vShake)
{
	// Add...

	VEC_ADD(m_vShakeAmount, m_vShakeAmount, vShake);

	if (m_vShakeAmount.x > MAX_SHAKE_AMOUNT) m_vShakeAmount.x = MAX_SHAKE_AMOUNT;
	if (m_vShakeAmount.y > MAX_SHAKE_AMOUNT) m_vShakeAmount.y = MAX_SHAKE_AMOUNT;
	if (m_vShakeAmount.z > MAX_SHAKE_AMOUNT) m_vShakeAmount.z = MAX_SHAKE_AMOUNT;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::HandleMsgClientPlayerUpdate()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HandleMsgClientPlayerUpdate (ILTMessage_Read *pMsg)
{
    //uint16 changeFlags = pMsg->Readuint16();
	uint8 changeFlags = pMsg->Readuint8();

	m_pMoveMgr->OnPhysicsUpdate(changeFlags, pMsg);

	if (changeFlags & PSTATE_INTERFACE)
	{
		uint8 nFlags	= pMsg->Readuint8();
		float fHideTime	= pMsg->Readfloat(); 

		LTBOOL bHiding		= !!(nFlags & PSTATE_INT_HIDING);
		LTBOOL bHidden		= !!(nFlags & PSTATE_INT_HIDDEN);
		LTBOOL bCantHide	= !!(nFlags & PSTATE_INT_CANTHIDE);

		g_pPlayerStats->UpdateHiding( bHiding, bHidden, bCantHide, fHideTime );

		SetCanDropCarriedObject( !!(nFlags & PSTATE_INT_CAN_DROP) );

// Don't allow respawn meter to go 100% immediately, since
// there is no punishment for dying.
/*
		if( IsPlayerDead( ) && IsRevivePlayerGameType( ))
		{
			// If we can't be revived, then allow respawn immediately.
			bool bCanBeRevived = !!( nFlags & PSTATE_INT_CANREVIVE );
			if( !bCanBeRevived )
			{
				SetRespawnTime( g_pLTClient->GetTime( ));
			}
		}
*/
	}


	m_bPlayerUpdated = LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::HandleMsgPlayerDamage()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //


void CPlayerMgr::HandleMsgPlayerDamage (ILTMessage_Read *pMsg)
{
	static const float kfSectorSize = MATH_PI * 2.0f / (float)kNumDamageSectors;
	if (!pMsg || g_bScreenShotMode) return;

	// 광식
	uint8 damageSector = pMsg->Readuint8();
    DamageType eType = (DamageType) pMsg->Readuint8();
	LTBOOL bTookHealth = (LTBOOL) pMsg->Readbool();
	
	if( IsPlayerDead() ) return;
	
 	float fPercent = 0.5f;
	
	m_fDamage[damageSector] += fPercent;

	if (m_fDamage[damageSector] > 1.0f)
		m_fDamage[damageSector] = 1.0f;
	
/*******************************
/****** Damage Sector Map ******
	            3
		     4     2

           5         1

		  6           0

		   7         11

			 8     10
			    9
*******************************/

	
	// Do some camera FX for taking armor and taking health...
	// Tie this into the DamageSector at some point??

	DamageFlags dmgFlag = DamageTypeToFlag( eType );

	if( !bTookHealth )
	{
		// Play the taking health fx for the DamageFX associated with the damage type...

		DAMAGEFX *pDamageFX = g_pDamageFXMgr->GetFirstDamageFX();
		while( pDamageFX )
		{
			// Test the damage flags against the DamageFX...

			if( dmgFlag & pDamageFX->m_nDamageFlag || pDamageFX->m_vtTestFX.GetFloat() > 0.0f )
			{
				CLIENTFX_CREATESTRUCT fxInit( pDamageFX->m_szTakingHealthFXName, FXFLAG_REALLYCLOSE, LTVector(0,0,0) ); 
				g_pClientFXMgr->CreateClientFX( LTNULL, fxInit, LTTRUE );
			}
			
			pDamageFX = g_pDamageFXMgr->GetNextDamageFX();
		}
	}
	else
	{
		// Play the taking armor fx for the DamageFX associated with the damage type...

		DAMAGEFX *pDamageFX = g_pDamageFXMgr->GetFirstDamageFX();
		while( pDamageFX )
		{
			// Test the damage flags against the DamageFX...

			if( dmgFlag & pDamageFX->m_nDamageFlag || pDamageFX->m_vtTestFX.GetFloat() > 0.0f )
			{
				CLIENTFX_CREATESTRUCT fxInit( pDamageFX->m_szTakingArmorFXName, FXFLAG_REALLYCLOSE, LTVector(0,0,0) ); 
				g_pClientFXMgr->CreateClientFX( LTNULL, fxInit, LTTRUE );
			}
			
			pDamageFX = g_pDamageFXMgr->GetNextDamageFX();
		}
	}

	// Tilt the camera based on the direction the damage came from...
	
	if (kNumDamageSectors != damageSector && IsJarCameraType(eType) && 
		m_pPlayerCamera->IsFirstPerson() && g_vtCamDamage.GetFloat() > 0.0f)
	{
		CameraDelta delta;

		if (g_vtCamDamagePitch.GetFloat() > 0.0f)
		{
			//float fPitchAngle = g_vtCamDamageMinPitchVal.GetFloat() + ((g_vtCamDamageMaxPitchVal.GetFloat() - g_vtCamDamageMinPitchVal.GetFloat()) * fPercent);
	
			if (damageSector >= 0 && damageSector <= 6)
			{
				delta.Pitch.fTime1	= g_vtCamDamageTime1.GetFloat();
				delta.Pitch.fTime2	= g_vtCamDamageTime2.GetFloat();
				delta.Pitch.eWave1	= Wave_SlowOff;
				delta.Pitch.eWave2	= Wave_SlowOff;
				delta.Pitch.fVar	= DEG2RAD(5);				
			}
			else
			{
				delta.Pitch.fTime1	= g_vtCamDamageTime1.GetFloat();
				delta.Pitch.fTime2	= g_vtCamDamageTime2.GetFloat();
				delta.Pitch.eWave1	= Wave_SlowOff;
				delta.Pitch.eWave2	= Wave_SlowOff;
				delta.Pitch.fVar	= -DEG2RAD(-5);				
			}

			if (damageSector >= 4 && damageSector <= 9)
			{
				delta.Yaw.fTime1	= g_vtCamDamageTime1.GetFloat();
				delta.Yaw.fTime2	= g_vtCamDamageTime2.GetFloat();
				delta.Yaw.eWave1	= Wave_SlowOff;
				delta.Yaw.eWave2	= Wave_SlowOff;
				delta.Yaw.fVar		= DEG2RAD(5);
			}
			else
			{
				delta.Yaw.fTime1	= g_vtCamDamageTime1.GetFloat();
				delta.Yaw.fTime2	= g_vtCamDamageTime2.GetFloat();
				delta.Yaw.eWave1	= Wave_SlowOff;
				delta.Yaw.eWave2	= Wave_SlowOff;
				delta.Yaw.fVar		= DEG2RAD(-5);
			}
		}

		/*
		if (g_vtCamDamageRoll.GetFloat() > 0.0f)
		{
			float fRollAngle = g_vtCamDamageMinRollVal.GetFloat() + ((g_vtCamDamageMaxRollVal.GetFloat() - g_vtCamDamageMinRollVal.GetFloat()) * fPercent);

			if (damageSector > 4 && damageSector < 8)
			{
				delta.Roll.fTime1	= g_vtCamDamageTime1.GetFloat();
				delta.Roll.fTime2	= g_vtCamDamageTime2.GetFloat();
				delta.Roll.eWave1	= Wave_SlowOff;
				delta.Roll.eWave2	= Wave_SlowOff;
				delta.Roll.fVar		= DEG2RAD(fRollAngle);
			}
			else if (damageSector < 2 || damageSector  == 11)
			{
				delta.Roll.fTime1	= g_vtCamDamageTime1.GetFloat();
				delta.Roll.fTime2	= g_vtCamDamageTime2.GetFloat();
				delta.Roll.eWave1	= Wave_SlowOff;
				delta.Roll.eWave2	= Wave_SlowOff;
				delta.Roll.fVar		= -DEG2RAD(fRollAngle);
			}
		}
		*/

		m_pCameraOffsetMgr->AddDelta(delta);
	}


	float fSlow = SlowMovementDuration(eType);
	
	if (fSlow > 0.0f)
	{
		m_pMoveMgr->AddDamagePenalty(fSlow);
	}

	PlayDamageSound( eType );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::Handle()
//
//	PURPOSE:	This handles the message sent by the world properties
//				indicating that they have changed and that the player needs
//				to sync to the console variables on the server
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HandleMsgChangeWorldProperties (ILTMessage_Read *pMsg)
{
	g_pGameClientShell->ResetDynamicWorldProperties(m_bUseWorldFog);
	
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::Handle()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HandleMsgPlayerOrientation (ILTMessage_Read *pMsg)
{
	// Set our pitch, yaw, and roll according to the players...

    uint8		nID			= pMsg->Readuint8();

	switch( nID )
	{
		case MID_ORIENTATION_ALL :
		{
			LTVector vVec	= pMsg->ReadLTVector();	
			m_fPitch	= vVec.x;
			m_fYaw		= vVec.y;
			m_fRoll		= 0.0f;
		}
		break;

		case MID_ORIENTATION_YAW :
		{
			m_fPitch	= 0.0f;
			m_fYaw		= pMsg->Readfloat();
			m_fRoll		= 0.0f;
		}
		break;

		default : break;
	};	

	float		fTimeLeft	= pMsg->Readfloat();
	g_pHUDTimer->SetTime( fTimeLeft, false );

	m_fPlayerPitch	= m_fPitch;
	m_fPlayerYaw	= m_fYaw;
	m_fPlayerRoll	= m_fRoll;	
	m_fYawBackup	= m_fYaw;
	m_fPitchBackup	= m_fPitch;

	SetAttachState(MID_DETACH_DISPLAY);
	m_pPlayerCamera->AttachToObject(g_pMoveMgr->GetObject());

	LTRotation rRot( 0.0f, m_fYaw, 0.0f );
	m_pPlayerCamera->SetRotation( rRot );
	m_pCameraOffsetMgr->Init();
	

	//g_pCameraBloodFX->Hide();
	g_pHUDFlashBangFX->InitVar();
	g_pScores->Show(false);
	g_pHUDFlashBangFX->SetVisible( false );
	
	m_nAttachState = MID_DETACH_DISPLAY;

	//[SPIKE] 산소 
	m_fAirLevel				= MAX_AIR_LEVEL;
	g_pPlayerStats->UpdateAir((uint8)m_fAirLevel);
	// 총기 Perturb가속도 초기화 
	m_fFireRecoilAccel		= 0.0f;
	// 줌 초기화 
	ReleaseZoom();
	//================================================
		
	// 광식 : 무기를 원 위치 시키고, 다른 Player에 Attach 시키면서 ShowPlayer(LTFALSE) 한 것을
	//		  다시 TRUE로 만드는 작업
	//----------------------------------------------------------------------------------------
	
	if( LTFALSE == g_GameDoc.IsMandateHost() &&
		LTFALSE == g_GameDoc.IsRejoinHost() )
	{
		g_pPlayerStats->RestartWeapon();
	}
		
	if( m_bFirstRespawn )
	{
		m_bFirstRespawn = false;
	}
	else if( eGameTypeSurvival == g_pGameClientShell->GetGameType() )
	{
#ifdef BIG_FAT_MODE_TEST
		if( g_GameDoc.IsBigHeadMode() )
		{
			g_pClientSoundMgr->PlaySoundLocal( "SA_INTERFACE\\SND\\FatRadio\\start_move.wav",
				SOUNDPRIORITY_MISC_HIGH, 0, 90 );
		}
		else
		{
			g_pClientSoundMgr->PlaySoundLocal( "SA_INTERFACE\\SND\\Radio\\start_move.wav",
				SOUNDPRIORITY_MISC_HIGH, 0, 90 );
		}
#else
		g_pClientSoundMgr->PlaySoundLocal( "SA_INTERFACE\\SND\\Radio\\start_move.wav",
				SOUNDPRIORITY_MISC_HIGH, 0, 90 );
#endif
	}
	
#ifdef GAME_SHOW
	if( g_GameDoc.IsHost() && g_GameDoc.IsCaptain() )
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8( MID_PLAYER_ATTACH );
		cMsg.Writeuint8( MID_DETACH_OBSERVER );
		g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
	}
#endif
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CPlayerMgr::HandleMsgGadgetTarget
//
//  PURPOSE:	Just pass the message on the the disabler object
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HandleMsgGadgetTarget( ILTMessage_Read *pMsg )
{
	m_pGadgetDisabler->OnGadgetTargetMessage( pMsg );
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CPlayerMgr::HandleMsgSearch
//
//  PURPOSE:	Just pass the message on the the searcher object
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HandleMsgSearch( ILTMessage_Read *pMsg )
{
	m_pSearcher->OnSearchMessage( pMsg );
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CPlayerMgr::HandleMsgAddPusher
//
//  PURPOSE:	Add a pusher to the MoveMgr.
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HandleMsgAddPusher( ILTMessage_Read *pMsg )
{
    LTVector vPos		= pMsg->ReadCompLTVector();
	float fRadius		= pMsg->Readfloat();
	float fStartDelay = pMsg->Readfloat();
	float fDuration	= pMsg->Readfloat();
	float fStrength	= pMsg->Readfloat();

	g_pPlayerMgr->GetMoveMgr()->AddPusher( vPos, fRadius, fStartDelay, fDuration, fStrength );
}

// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CPlayerMgr::HandleMsgObjectivesData
//
//  PURPOSE:	Set our objectives obtained from the server...
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HandleMsgObjectivesData( ILTMessage_Read *pMsg )
{
	g_pPlayerStats->OnObjectivesDataMessage( pMsg );
}

void CPlayerMgr::HandleMsgWeaponReload( ILTMessage_Read* pMsg )
{
	IClientWeaponBase *pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	if ( pClientWeapon )
	{
		uint8	nAmmoId		= pMsg->Readuint8();
		uint16	nAmmoCount	= pMsg->Readuint16();
		if( nAmmoCount == 0 ) nAmmoCount = -1;

		pClientWeapon->ReloadClip( true, nAmmoCount, false, false );
	}
}
// 광식
void CPlayerMgr::HandleMsgAttach( ILTMessage_Read* pMsg )
{
	// 3 인칭과 1 인칭 무기 관련 Data는 여기서 처리...
	uint8 nAttachState = pMsg->Readuint8();
	HLOCALOBJ hObj	   = (HLOCALOBJ)pMsg->ReadObject();
	
	if( !hObj ) return;

	switch( nAttachState )
	{
	case MID_ATTACH_FIRST :
		{
			//if( m_pPlayerCamera->GetAttachedObject() == hObj &&
			//if( staticObject == hObj &&
			//	m_nAttachState == MID_ATTACH_FIRST ) break;

			if( m_pPlayerCamera->GetAttachedObject() != m_pMoveMgr->GetObject() &&
				m_pPlayerCamera->GetAttachedObject() != LTNULL )
			{
				// 과거에 붙어 있던 녀석의 3 인칭 모델을 다시 살린다.
				//----------------------------------------------------------------------
				uint32 dwPlayerFlags;
				HLOCALOBJ hOldPlayerObj = m_pPlayerCamera->GetAttachedObject();
				g_pCommonLT->GetObjectFlags(hOldPlayerObj, OFT_Flags, dwPlayerFlags);
				if( dwPlayerFlags & FLAG_RAYHIT )	// RAYHIT 로 Player 가 죽은 것을 판단??
				{
					ShowPlayer(LTTRUE);
					HideShowAttachments(hObj);
				}
				//----------------------------------------------------------------------
			}
			// 새로운 녀석에게 1 인칭 Attach
			//----------------------------------------------------------------------
			// [05.03.04]	// Attach 된 녀석의 FLAG_SOLID flag를 빼려고...
			CSFXMgr* tempSFX = g_pGameClientShell->GetSFXMgr();
			tempSFX->PlayerDeathAttach(hObj);
			//----------------------------------------------------------------------

			m_pPlayerCamera->AttachToObject( hObj );
				
			m_pClientWeaponMgr->OnPlayerAlive();	// Client Weapon 을 다시 살린다
			m_pClientWeaponMgr->ShowWeapons();
			m_pPlayerCamera->GoFirstPerson();
			m_pPlayerCamera->CameraUpdate(0.0f);

			ShowPlayer(LTFALSE);					// 3 인칭 모델을 화면에서 지운다.
			HideShowAttachments(hObj);

			// 광식
			// [05.02.11] : ControlFlags Test
			g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderText);	// HUD는 Text 관련 된 것들만 그린다.
			//g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderFull);

			if( m_pClientWeaponMgr )
			{
				int WeaponId	= pMsg->Readuint32();
				int AmmoId		= pMsg->Readuint32();
				int AmmoCount	= pMsg->Readuint32();

				//------------------------------------
				int AmmoInClip	= pMsg->Readuint32();
				//------------------------------------
				
				g_pPlayerStats->UpdateAmmo(WeaponId, AmmoId, AmmoCount);
				g_pPlayerStats->UpdatePlayerWeapon(WeaponId, AmmoId);
				
				m_pClientWeaponMgr->ChangeWeapon(WeaponId, AmmoId, AmmoCount, true );
				m_pClientWeaponMgr->SetDeathAmmoInClip( WeaponId, AmmoInClip );
				
				IClientWeaponBase* pCurWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
				if( pCurWeapon )
				{
					((CClientWeapon*)pCurWeapon)->SetFirstAttachState();
				}
			}
			//----------------------------------------------------------------------

			m_nAttachState = MID_ATTACH_FIRST;

			g_pMoveMgr->SetSpectatorMode(LTFALSE);

			//[MURSUM]			
			T_PLAYER_INFOptr pPlayerInfo = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByHandle(hObj);
			if( pPlayerInfo )
			{
				g_pHUDPlayerState->AttachPlayer( pPlayerInfo->szNickName, nAttachState );				
			}

		}break;
		
	case MID_ATTACH_THIRD :
		{
			if( m_pPlayerCamera->GetAttachedObject() == hObj && 
				m_nAttachState == MID_ATTACH_THIRD ) break;

			//----------------------------------------------------------------------
			// [05.03.04]	// Attach 된 녀석의 FLAG_SOLID flag를 빼려고...
			CSFXMgr* tempSFX = g_pGameClientShell->GetSFXMgr();
			tempSFX->PlayerDeathAttach(hObj);
			//----------------------------------------------------------------------
			

			//m_pPlayerCamera->AttachToObject( hObj );	// 3 인칭 모델에 Attach
			m_pPlayerCamera->AttachToObject( LTNULL );	// 3 인칭 모델에 Attach

			staticObject = hObj;

			m_hThirdAttach = hObj;
			
			m_pClientWeaponMgr->OnPlayerDead();		// Client Weapon 을 다시 죽인다.
			m_pClientWeaponMgr->HideWeapons();
			m_pPlayerCamera->GoChaseMode();
			m_pPlayerCamera->CameraUpdate(0.0f);

			m_nAttachState = MID_ATTACH_THIRD;

			ShowPlayer(LTTRUE);
			HideShowAttachments(hObj);
			
			g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderText);

			g_pMoveMgr->SetSpectatorMode(LTFALSE);
			//-----------------------------------------------------------------
			HOBJECT hLocalObj = g_pLTClient->GetClientObject();
			CSpecialFXList* pList = tempSFX->GetFXList(SFX_CHARACTER_ID);
			if( pList )
			{
				for( int i = 0; i < pList->GetSize(); i++ )
				{
					if( (*pList)[i] )
					{
						CCharacterFX* pChar = (CCharacterFX*)(*pList)[i];
						if( hLocalObj == pChar->GetServerObj() ) continue;
						if( pChar->IsRadarPlayerDead() == false )
						{
							g_pCommonLT->SetObjectFlags(pChar->GetServerObj(), OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);
							HideShowAttachmentsAfterDeath(pChar->GetServerObj());
						}
					}
				}
			}
			//-----------------------------------------------------------------

			//[MURSUM]			
			T_PLAYER_INFOptr pPlayerInfo = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByHandle(hObj);
			if( pPlayerInfo )
			{
				g_pHUDPlayerState->AttachPlayer( pPlayerInfo->szNickName, nAttachState );
			}
		}break;

	case MID_DETACH_DISPLAY :
		{
			/*
			m_pPlayerCamera->SetCameraMode (CPlayerCamera::FIRSTDYING);
			g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderDead);
			m_pClientWeaponMgr->OnPlayerDead();
			m_pPlayerCamera->AttachToObject(LTNULL);

			g_pMoveMgr->SetSpectatorMode(LTTRUE);

			m_nAttachState = MID_DETACH_DISPLAY;
			*/
			//if( m_pPlayerCamera->GetAttachedObject() != LTNULL )
			//{
			//	ShowPlayer(LTTRUE);
			//}
			
			m_pPlayerCamera->GoFirstPerson();
			g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderDead);
			m_pClientWeaponMgr->OnPlayerDead();
			m_pPlayerCamera->AttachToObject(g_pMoveMgr->GetObject());
			
			m_nAttachState = MID_DETACH_OBSERVER;			

			//ShowPlayer(LTFALSE);
			//HideShowAttachments(g_pLTClient->GetClientObject());
			
			g_pMoveMgr->SetSpectatorMode(LTTRUE);

		}break;
		
	case MID_DETACH_OBSERVER :
		{
			if( m_pPlayerCamera->GetAttachedObject() != LTNULL )
			{
				ShowPlayer(LTTRUE);
				//HideShowAttachments(hObj);
			}
			
			m_pPlayerCamera->GoFirstPerson();
			g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderDead);
			m_pClientWeaponMgr->OnPlayerDead();
			m_pPlayerCamera->AttachToObject(g_pMoveMgr->GetObject());
			
			if( pMsg->Readbool() ) // 카메라 위치를 움직일지 결정
			{
				LTVector vPos;
				g_pLTClient->GetObjectPos(hObj, &vPos);
				g_pMoveMgr->SetMoverPos(vPos);

				LTRotation rRot;
				g_pLTClient->GetObjectRotation(hObj, &rRot);
				m_pPlayerCamera->SetRotation(rRot);
			}			
			
			m_nAttachState = MID_DETACH_OBSERVER;

			ShowPlayer(LTFALSE);
			HideShowAttachments(g_pLTClient->GetClientObject());
			
			g_pMoveMgr->SetSpectatorMode(LTTRUE);

			//-----------------------------------------------------------------
			HOBJECT hLocalObj = g_pLTClient->GetClientObject();
			CSFXMgr* tempSFX = g_pGameClientShell->GetSFXMgr();
			if( tempSFX )
			{
				CSpecialFXList* pList = tempSFX->GetFXList(SFX_CHARACTER_ID);
				if( pList )
				{
					for( int i = 0; i < pList->GetSize(); i++ )
					{
						if( (*pList)[i] )
						{
							CCharacterFX* pChar = (CCharacterFX*)(*pList)[i];
							if( hLocalObj == pChar->GetServerObj() ) continue;

							if( pChar->IsRadarPlayerDead() == false )
							{
								g_pCommonLT->SetObjectFlags(pChar->GetServerObj(), OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);
								HideShowAttachmentsAfterDeath(pChar->GetServerObj());
							}
						}
					}
				}
			}			
			//-----------------------------------------------------------------

			//[MURSUM]			
			T_PLAYER_INFOptr pPlayerInfo = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByHandle(hObj);
			if( pPlayerInfo )
			{
				g_pHUDPlayerState->AttachPlayer( LTNULL, nAttachState );
			}
		}break;
	}

	//[MURSUM]===================
	//g_pCameraBloodFX->Hide();
	//===========================
}

void CPlayerMgr::HandleProjectileFX( ILTMessage_Read* pMsg )
{
	uint8		nAmmoID		= pMsg->Readuint8();
	uint8		nID			= pMsg->Readuint8();
	LTVector	vFirePos	= pMsg->ReadCompPos();
	HOBJECT		hObj		= g_pInterfaceMgr->GetPlayerInfoMgr()->GetHandleByID(nID);
	g_pGameClientShell->GetSFXMgr()->CreateProjectileFX( hObj, vFirePos, nAmmoID );

	/*
	WEAPON const* pWeapon	= g_pWeaponMgr->GetWeapon(nAmmoID);
	ObjectCreateStruct cOCS;
	cOCS.Clear();				
	SAFE_STRCPY( cOCS.m_Filename, pWeapon->szHHModel );
	pWeapon->blrHHSkins.CopyList( 0, cOCS.m_SkinNames[0], MAX_CS_FILENAME_LEN +1 );	
	g_pCommonLT->SetObjectFilenames( hObj, &cOCS );
	*/
}

void CPlayerMgr::DetachProcess()
{
	if( IsPlayerDead() )
	{
		LTRESULT ltResult;
		CAutoMessage cMsg;
		cMsg.Writeuint8( MID_PLAYER_ATTACH );
		cMsg.Writeuint8( MID_DETACH_DISPLAY );
		ltResult = g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
		ASSERT( LT_OK == ltResult );
	}
}
// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateCamera()
//
//	PURPOSE:	Update the camera position/rotation
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdateCamera()
{
	//handle updating the camera, as long as we aren't paused
	if(!g_pGameClientShell->IsGamePaused() || !m_bCameraPosInited)
	{
		// Update the sway...

		if (IsZoomed())
		{
			// SPIKE  Scope 상하좌우로 흔들림
			//UpdateCameraSway();

			//[MURSUM] 숨소리
			if( g_pLTClient->GetTime() > m_fZoomInBreathTime )
			{
				m_fZoomInBreathTime = g_pLTClient->GetTime() + SECOND_ZOOM_BREATH;
#ifdef BIG_FAT_MODE_TEST
				if( LTFALSE == g_GameDoc.IsBigHeadMode() )
				{
					g_pClientSoundMgr->PlaySoundLocal( "SA_CHARACTERS\\SND\\breath.wav", SOUNDPRIORITY_MISC_MEDIUM, 0, 90 );
				}
				else
				{
					g_pClientSoundMgr->PlaySoundLocal( "SA_CHARACTERS\\SND\\FatEtc\\breath.wav", SOUNDPRIORITY_MISC_MEDIUM, 0, 90 );
				}
#else
				g_pClientSoundMgr->PlaySoundLocal( "SA_CHARACTERS\\SND\\breath.wav", SOUNDPRIORITY_MISC_MEDIUM, 0, 90 );
#endif
			}
		}

		// Update the camera's position and rotation..

		UpdateAlternativeCamera();


		// Update the player camera...

		UpdatePlayerCamera();
		
		/*
		// 광식
		if( IsPlayerDead() && 
			//g_pGameClientShell->GetGameType() == eGameTypeSurvival &&
			IsRevivePlayerGameType() &&
			m_pPlayerCamera->GetAttachedObject() != m_pMoveMgr->GetObject() )
		{
			if( m_pPlayerCamera->IsFirstPerson() )
			{
				LTVector vPos = m_pPlayerCamera->GetPos();
				m_rRotation = m_pPlayerCamera->GetRotation();
				m_pHeadBobMgr->AdjustCameraPos(vPos);
				vPos.y	+= m_fCamDuck;
				vPos.y	+= 5.0f;
				vPos	+= m_pCameraOffsetMgr->GetPosDelta();			
				g_pLTClient->SetObjectPos(m_hCamera, &vPos);
				g_pLTClient->SetObjectPos(g_pMoveMgr->GetObject(), &vPos);

				g_pLTClient->GetObjectRotation(m_pPlayerCamera->GetAttachedObject(), &m_rRotation);
				LTRotation pitch = LTRotation(LTVector(1.0f, 0.0f, 0.0f), m_fPitch);

				m_rRotation = m_rRotation * pitch;

				g_pLTClient->SetObjectRotation(m_hCamera, &m_rRotation);
				g_pLTClient->SetObjectRotation(g_pMoveMgr->GetObject(), &m_rRotation);

				m_bCameraPosInited = LTTRUE;
			}
			else
			{
				LTVector vPos = m_pPlayerCamera->GetPos();
				m_rRotation = m_pPlayerCamera->GetRotation();
				g_pLTClient->SetObjectPos(m_hCamera, &vPos);
				g_pLTClient->SetObjectRotation(m_hCamera, &m_rRotation);
				m_bCameraPosInited = LTTRUE;				
			}
			
		}
		else
		{
			if (!m_bUsingExternalCamera)
			{
				if( IsMultiplayerGame() )
				{
					UpdateMultiplayerCameraPosition();
				}
				else if( g_pInterfaceMgr->AllowCameraMovement() )
				{
					UpdateCameraPosition();
				}

			}

			if (g_pInterfaceMgr->AllowCameraRotation() )
			{
				// [RP] 11/14/02 - Don't calculate the rotation if we're not supposed to be allowing
				//		mouse inputs.  But we do want to update the rotation incase the camera is
				//		attached to the head of the player.

				if( g_pDamageFXMgr->AllowInput() )
				{
					CalculateCameraRotation();
				}

				UpdateCameraRotation();
			}
		}
		*/
		// 광식
		// [05.02.11] : ControlFlags Test				
		//======================================================================================
		if (!m_bUsingExternalCamera)
		{
			if( IsMultiplayerGame() )
			{
				UpdateMultiplayerCameraPosition();
			}
			else if( g_pInterfaceMgr->AllowCameraMovement() )
			{
				UpdateCameraPosition();
			}

		}		

		if (g_pInterfaceMgr->AllowCameraRotation() )
		{
			// [RP] 11/14/02 - Don't calculate the rotation if we're not supposed to be allowing
			//		mouse inputs.  But we do want to update the rotation incase the camera is
			//		attached to the head of the player.

			if( g_pDamageFXMgr->AllowInput() )
			{
				// 광식 : 1 인칭으로 Attach 되었을 때는 Mouse Input 을 받지 않는다.
				if( IsPlayerDead() &&
					IsRevivePlayerGameType() &&
					m_nAttachState == MID_ATTACH_FIRST )
				{
				}
				else
				{
					CalculateCameraRotation();
				}

				//CalculateCameraRotation();
			}

			UpdateCameraRotation();
		}
		//======================================================================================

		if (m_bUsingExternalCamera)
		{
			HandleZoomChange(LTTRUE);
		}

		// Update zoom if applicable...

		if (m_bZooming)
		{
			UpdateCameraZoom();  
		}
		else
		{
			m_fZoomIn_Increase	= 50.0f;
			m_fZoomIn2_Increase = 20.0f;
		}

		// Update any camera displacement.
		UpdateCameraDisplacement();

		// Make sure the player gets updated

 		if (IsMultiplayerGame() || g_pInterfaceMgr->AllowCameraMovement())
		{
			UpdatePlayerInfo(true);
		}
	}
	// Remember to continue updating the server if we're in multiplayer
	else if (IsMultiplayerGame() && g_pGameClientShell->IsGamePaused())
	{
		UpdatePlayerInfo(false);
	}
 
// SPIKE 스나이퍼 Reload시 Zoom 강제Release & 볼트액션 스나이퍼일 경우 //////////////////////////////////////////////////
	IClientWeaponBase *pClientWeapon = GetCurrentClientWeapon();
	if( !pClientWeapon ) return;
	WeaponState pCurrentState =  pClientWeapon->GetState();
	if (IsZoomed())
	{
		if(pClientWeapon->GetZoomMode() == VOLTSNIPER_ZOOMOUT)  // 줌아웃 모드 
		{
			m_nTempZoomLevel = m_nZoomView;
			m_nZoomView = 0;
			m_bZooming   = LTTRUE;
			m_bZoomingIn = LTFALSE;
			g_pInterfaceMgr->BeginZoom(LTFALSE);
			HandleZoomChange();
			
			pClientWeapon->SetZoomMode(VOLTSNIPER_ZOOMFREE);
		}
	}
	else 
	{
		if(pCurrentState != W_RELOADING && pClientWeapon->GetZoomMode() == VOLTSNIPER_ZOOMIN)  // 줌인 모드
		{
			if(m_nTempZoomLevel > 0)
			{
				BeginZoom();
				m_nZoomView  = m_nTempZoomLevel;
				m_nTempZoomLevel = 0;
				m_bZooming   = LTTRUE;
				m_bZoomingIn = LTTRUE;
				if(m_nZoomView == 1)
				{
					m_fZoomIn_Increase	= 50.0f;
					m_fZoomIn2_Increase = 20.0f;
				}
				else
				{
					m_fZoomIn_Increase  = 50.0f;
					m_fZoomIn2_Increase = 50.0f;
				}
				
				g_pInterfaceMgr->BeginZoom(LTTRUE);
				HandleZoomChange();
			}
		}
	}
	if( pCurrentState == W_RELOADING && g_bHalfZoomMode )
	{
		ReleaseZoom();
	}
	
	// TimeBomb 설치할 준비가 되있는가?   Firing + Change Bomb + Not PlantedBomb 
	SetTimeBombPlanting_Level2( (pCurrentState == W_FIRING && GetSelectTimeBomb() && !GetPlantedTimeBomb())? LTTRUE : LTFALSE );
	SetTimeBombPlanting_Level1( (pCurrentState == W_FIRING && GetSelectTimeBomb())? LTTRUE : LTFALSE );
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

VOID	
CPlayerMgr::ReleaseZoom()
{
	m_nTempZoomLevel = 0;
	g_pInterfaceMgr->BeginZoom(LTFALSE);
	HandleZoomChange(LTTRUE);
	IClientWeaponBase *pClientWeapon = GetCurrentClientWeapon();
	if( !pClientWeapon ) return;
	WriteConsoleFloat("PVModelFOV", pClientWeapon->GetWeapon()->m_fPVModelFOVY);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateCameraDisplacement
//
//	PURPOSE:	Update any camera displacement.
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateCameraDisplacement()
{
	LTVector vDisplacement( 0.0f, 0.0f, 0.0f );
	LTVector vAdd;

	vAdd.Init( );
	UpdateCameraShake( vAdd );
	vDisplacement += vAdd;
	vAdd.Init( );
	UpdateVehicleCamera( vAdd );
	vDisplacement += vAdd;

	if (!g_pInterfaceMgr->AllowCameraMovement())
		return;

	// Check if no significant movement occurred.
	if( vDisplacement.LengthSquared( ) < 0.01f )
		return;

	LTVector vPos;
	g_pLTClient->GetObjectPos(m_hCamera, &vPos);
	vPos += vDisplacement;
	g_pLTClient->SetObjectPos(m_hCamera, &vPos);

	//TODO: move this code into the weapon class
	HLOCALOBJ hWeapon = 0;
	IClientWeaponBase *pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	if ( pClientWeapon )
	{
		hWeapon = pClientWeapon->GetHandle();
	}
	if (!hWeapon) return;

	g_pLTClient->GetObjectPos(hWeapon, &vPos);
	vDisplacement += LTVector(0.95f, 0.95f, 0.95f);
	vPos += vDisplacement;
	g_pLTClient->SetObjectPos(hWeapon, &vPos);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateCameraShake
//
//	PURPOSE:	Update the camera's shake
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateCameraShake( LTVector& vDisplacement )
{
	// Decay...

	float fDecayAmount = 2.0f * g_pGameClientShell->GetFrameTime();

	m_vShakeAmount.x -= fDecayAmount;
	m_vShakeAmount.y -= fDecayAmount;
	m_vShakeAmount.z -= fDecayAmount;

	if (m_vShakeAmount.x < 0.0f) m_vShakeAmount.x = 0.0f;
	if (m_vShakeAmount.y < 0.0f) m_vShakeAmount.y = 0.0f;
	if (m_vShakeAmount.z < 0.0f) m_vShakeAmount.z = 0.0f;


	if (m_vShakeAmount.x <= 0.0f && m_vShakeAmount.y <= 0.0f && m_vShakeAmount.z <= 0.0f) return;


	// Apply...

	float faddX = GetRandom(-1.0f, 1.0f) * m_vShakeAmount.x * 3.0f;
	float faddY = GetRandom(-1.0f, 1.0f) * m_vShakeAmount.y * 3.0f;
	float faddZ = GetRandom(-1.0f, 1.0f) * m_vShakeAmount.z * 3.0f;

	LTVector vAdd;
	vDisplacement.Init(faddX, faddY, faddZ);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateVehicleCamera
//
//	PURPOSE:	Update the vehicle's camera displacement
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateVehicleCamera( LTVector& vDisplacement )
{
	// Check if we're not in vehicle mode.
	if( !m_pMoveMgr->GetVehicleMgr()->IsVehiclePhysics())
		return;

	m_pMoveMgr->GetVehicleMgr()->CalculateVehicleCameraDisplacment( vDisplacement );
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateCameraSway
//
//	PURPOSE:	Update the camera's sway
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateCameraSway()
{
	// Apply...
    float swayAmount = g_pGameClientShell->GetFrameTime() / 1000.0f;

    float tm  = g_pLTClient->GetTime()/10.0f;

	// Adjust if ducking...
	float fMult = (m_dwPlayerFlags & BC_CFLG_DUCK) ? g_vtCameraSwayDuckMult.GetFloat() : 1.0f;

	//Adjust for damage
	if (g_pDamageFXMgr->IsFOVAffected())
		fMult *= 5.0f;

    float faddP = fMult * g_vtCameraSwayYSpeed.GetFloat() * (float)sin(tm*g_vtCameraSwayYFreq.GetFloat()) * swayAmount;
    float faddY = fMult * g_vtCameraSwayXSpeed.GetFloat() * (float)sin(tm*g_vtCameraSwayXFreq.GetFloat()) * swayAmount;

	m_fPitch += faddP;
	m_fYaw	 += faddY;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::SetExternalCamera()
//
//	PURPOSE:	Turn on/off external camera mode
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::SetExternalCamera(LTBOOL bExternal)
{
	if (bExternal && m_pPlayerCamera->IsFirstPerson())
	{
		g_pDamageFXMgr->Clear();
		
		m_pClientWeaponMgr->DisableWeapons();
		
		ShowPlayer(LTTRUE);
		m_pPlayerCamera->GoChaseMode();
		m_pPlayerCamera->CameraUpdate(0.0f);

		g_pInterfaceMgr->EnableCrosshair(LTFALSE); // Disable cross hair in 3rd person...
	}
	else if (!bExternal && !m_pPlayerCamera->IsFirstPerson()) // Go Internal
	{
		m_pClientWeaponMgr->EnableWeapons();

		// We don't want to accidently fire off a round or two comming out of a cinematic...
		
		IClientWeaponBase *pWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
		if( pWeapon )
		{
			pWeapon->ClearFiring();
		}

		ShowPlayer(LTFALSE);
		m_pPlayerCamera->GoFirstPerson();
		m_pPlayerCamera->CameraUpdate(0.0f);

		g_pInterfaceMgr->EnableCrosshair(LTTRUE);
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateAlternativeCamera()
//
//	PURPOSE:	Update the camera using an alternative camera
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerMgr::UpdateAlternativeCamera()
{
	m_bLastAllowPlayerMovement = m_bAllowPlayerMovement;
    m_bAllowPlayerMovement     = LTTRUE;

    HOBJECT hObj = NULL;

	// See if we should use an alternative camera position...

	CSpecialFXList* pCameraList = g_pGameClientShell->GetSFXMgr()->GetCameraList();
	if (pCameraList)
	{
		int nNum = pCameraList->GetSize();

		for (int i=0; i < nNum; i++)
		{
			CCameraFX* pCamFX = (CCameraFX*)(*pCameraList)[i];
			if (!pCamFX) continue;

			hObj = pCamFX->GetServerObj();

			if (hObj)
			{
                uint32 dwUsrFlags;
                g_pCommonLT->GetObjectFlags(hObj, OFT_User, dwUsrFlags);

				if (dwUsrFlags & USRFLG_CAMERA_LIVE)
				{
                    g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderText);

                    SetExternalCamera(LTTRUE);

					m_bAllowPlayerMovement = pCamFX->AllowPlayerMovement();

                    LTVector vPos;
					g_pLTClient->GetObjectPos(hObj, &vPos);
					g_pLTClient->SetObjectPos(m_hCamera, &vPos);
                    m_bCameraPosInited = LTTRUE;

                    LTRotation rRot;
					g_pLTClient->GetObjectRotation(hObj, &rRot);
					g_pLTClient->SetObjectRotation(m_hCamera, &rRot);

					// Always set the camera as the listener, so ambient sounds are
					// heard from the camera position.
					m_bCamIsListener = LTTRUE;

					((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->SetListener(LTFALSE, &vPos, &rRot, (m_bCamIsListener ? true : false));

					// Initialize the cinematic camera

					s_nLastCamType = pCamFX->GetType();

					pCamFX->UpdateFOV();

					float	fFovX = pCamFX->GetFovX();
					float	fFovY = pCamFX->GetFovY();

					TurnOnAlternativeCamera(s_nLastCamType, fFovX, fFovY);

                    return LTTRUE;
				}
			}
		}
	}


	// Okay, we're no longer using an external camera...

	if (m_bUsingExternalCamera)
	{
		TurnOffAlternativeCamera(s_nLastCamType);
	}


    return LTFALSE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::TurnOnAlternativeCamera()
//
//	PURPOSE:	Set up using an alternative camera
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::TurnOnAlternativeCamera(uint8 nCamType, float fFovX, float fFovY)
{
	g_pInterfaceMgr->SetLetterBox((nCamType == CT_CINEMATIC));

	SetCameraFOV( DEG2RAD(fFovX), DEG2RAD(fFovY) ); 

	if (!m_bUsingExternalCamera)
	{
		//g_pLTClient->CPrint("TURNING ALTERNATIVE CAMERA: ON");

		// Make sure we clear whatever was on the screen before
		// we switch to this camera...
		HandleZoomChange( LTTRUE );

		g_pInterfaceMgr->ClosePopup();

		m_pClientWeaponMgr->DisableWeapons();

		// Make sure we're using the highest model lods
        g_pLTClient->RunConsoleString("+ModelLODOffset -10");
	}

	m_bUsingExternalCamera = LTTRUE;

	// All the number of model shadows in cinematics to be much higher

	if(!g_bCinChangedNumModelShadows)
	{
		g_nCinSaveNumModelShadows = GetConsoleInt("ModelShadow_Proj_MaxShadowsPerFrame", 0);
		WriteConsoleInt("ModelShadow_Proj_MaxShadowsPerFrame", g_kMaxNumberOfCinShadows);

		g_bCinChangedNumModelShadows = true;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::TurnOffAlternativeCamera()
//
//	PURPOSE:	Turn off the alternative camera mode
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::TurnOffAlternativeCamera(uint8 nCamType)
{
    //g_pLTClient->CPrint("TURNING ALTERNATIVE CAMERA: OFF");

	g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderFull);
    m_bUsingExternalCamera = LTFALSE;

	// Set the listener back to the client...

    ((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->SetListener(LTTRUE, NULL, NULL, LTTRUE);

	// Force 1st person...

    SetExternalCamera(LTFALSE);

	// enable the weapons
	m_pClientWeaponMgr->EnableWeapons();

	// turn off the letter box
	g_pInterfaceMgr->SetLetterBox(LTFALSE);

	// Reset the normal FOV

	SetCameraFOV(DEG2RAD(g_vtFOVXNormal.GetFloat()), DEG2RAD(g_vtFOVYNormal.GetFloat()));

	// Make sure we're using the normal model lod...
    g_pLTClient->RunConsoleString("+ModelLODOffset 0");

	// Set number of shadows back to whatever they were set to before...

	if(g_bCinChangedNumModelShadows)
	{
		WriteConsoleInt("ModelShadow_Proj_MaxShadowsPerFrame", g_nCinSaveNumModelShadows);
		g_bCinChangedNumModelShadows = false;
	}
}


// ----------------------------------------------------------------------- //
//
//	FUNCTION:	CPlayerMgr::GetCurrentClientWeapon()
//
//	PURPOSE:	Return a pointer to the player's bucurrent weapon
//
// ----------------------------------------------------------------------- //

IClientWeaponBase* CPlayerMgr::GetCurrentClientWeapon() const
{
	return m_pClientWeaponMgr->GetCurrentClientWeapon();
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::InCameraGadgetRange
//
//	PURPOSE:	See if the given object is in the camera gadget's range
//
// --------------------------------------------------------------------------- //

LTBOOL CPlayerMgr::InCameraGadgetRange(HOBJECT hObj)
{
	if (!hObj || !m_hCamera) return LTFALSE;

	IClientWeaponBase *pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	if ( pClientWeapon && !m_pClientWeaponMgr->WeaponsEnabled() )
	{
		return LTFALSE;
	}

	LTVector vCamPos, vObjPos;
	g_pLTClient->GetObjectPos(m_hCamera, &vCamPos);
	g_pLTClient->GetObjectPos(hObj, &vObjPos);

	LTVector vDist = vCamPos - vObjPos;
	float fDist = vDist.Length();

	// g_pLTClient->CPrint("fDist = %.2f, Zoom View = %d", fDist, m_nZoomView);

	if (fDist < g_vtCamZoom1MaxDist.GetFloat())
	{
		return LTTRUE;
	}
	else if (fDist < g_vtCamZoom2MaxDist.GetFloat())
	{
		if (m_nZoomView > 0) return LTTRUE;
	}
	else
	{
		if (m_nZoomView > 1) return LTTRUE;
	}
	// Not zoomed in enough...
	return LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateWeaponModel()
//
//	PURPOSE:	Update the weapon model
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdateWeaponModel()
{
	// 광식
	HLOCALOBJ hPlayerObj;
	if( IsPlayerDead() && m_pPlayerCamera->GetAttachedObject() != m_pMoveMgr->GetObject() )
	{
		hPlayerObj = m_pPlayerCamera->GetAttachedObject();
	}
	else
	{
		hPlayerObj = g_pLTClient->GetClientObject();
	}

	//HLOCALOBJ hPlayerObj = g_pLTClient->GetClientObject();
	if (!hPlayerObj) return;

	// 광식
	// [05.02.16] : 스나이퍼의 Zoom In 
	//---------------------------------------------
	if( (m_dwPlayerFlags & BC_CFLG_ZOOM_IN) != 0 && 
		(g_pMoveMgr->GetLastControlFlags() & BC_CFLG_ZOOM_IN) == 0 )
	{
		ClientWeaponZoomIn();
	}
	//---------------------------------------------

	// Decay weapon recoil...

	DecayWeaponRecoil();


	// If possible, get these values from the camera, because it
	// is more up-to-date...

	LTRotation rRot;
	LTVector vPos(0, 0, 0);

	// Weapon model pos/rot is relative to camera now...
	g_pLTClient->GetObjectPos(m_hCamera, &vPos);
	g_pLTClient->GetObjectRotation(m_hCamera, &rRot);

	if (!m_pPlayerCamera->IsFirstPerson() || m_bUsingExternalCamera)
	{
		// Use the gun's flash orientation...

		GetAttachmentSocketTransform(hPlayerObj, "Flash", vPos, rRot);
	}


	// If we aren't dead, and we aren't in the middle of changing weapons,
	// let us fire.

	FireType eFireType = FT_NORMAL_FIRE;
	bool bFire = false;


	// only fire if:
	//    fire key down (alt firing is disabled) AND
	//    player is not choosing ammo AND
	//    player is not choosing a weapon AND
	//    player is not dead AND
	//    player is not in spectator mode
	if ( ( m_dwPlayerFlags & BC_CFLG_FIRING ) &&
	     ( !g_pInterfaceMgr->IsChoosingAmmo() ) &&
	     ( !g_pInterfaceMgr->IsChoosingWeapon() ) &&
	     /*( !IsPlayerDead() ) &&*///광식 : 죽었을 때도 fire 처리 하려고 지웠음
	     ( !m_bSpectatorMode ) &&
		 ( g_pInterfaceMgr->GetGameState() == GS_PLAYING ) )
	{
		
		//[SPIKE] ===========
		g_pSurfaceMgr->SetSpecialDamageMode(LTFALSE);
		//===================
		bFire = true;
		
		if(g_pPlayerStats->GetHalf_Scope() != WMGR_INVALID_ID && g_bHalfZoomMode)
		{
			eFireType = FT_SPECIAL_FIRE;
			g_pSurfaceMgr->SetSpecialDamageMode(LTTRUE);
		}
	}
	
	//[SPIKE] Melee 무기 Special_Fire ======================
	IClientWeaponBase *pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	if( pClientWeapon && pClientWeapon->GetWeapon() && g_pSurfaceMgr )
	{
		if( !(m_dwPlayerFlags & BC_CFLG_FIRING) && (m_dwPlayerFlags & BC_CFLG_ZOOM_IN) && pClientWeapon->GetWeapon()->nAniType == 2 )
		{
			//[SPIKE] ===========
			g_pSurfaceMgr->SetSpecialDamageMode(LTTRUE);
			//===================
			eFireType = FT_SPECIAL_FIRE;
			bFire = true;
		}
	}
	//======================================================

	// Update the model position and state...
	WeaponState eWeaponState = m_pClientWeaponMgr->Update( rRot, vPos, bFire, eFireType );

	// Do fire camera jitter...

	if (FiredWeapon(eWeaponState) && m_pPlayerCamera->IsFirstPerson())
	{
		StartWeaponRecoil();
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::StartWeaponRecoil()
//
//	PURPOSE:	Start the weapon recoiling...
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::StartWeaponRecoil()
{
	float fMaxPitch = g_vtFireJitterMaxPitchDelta.GetFloat();
	float fMaxPitchInter = fMaxPitch ;

	if (g_vtUseCamRecoil.GetFloat() > 0.0f)
	{
		// Move view up a bit...

		float fPCorrect = 1.0f - (m_fFireJitterPitch / DEG2RAD(g_vtMaxCamRecoilPitch.GetFloat()));
		float fPVal = GetRandom(0.5f,2.0f) * DEG2RAD(g_vtBaseCamRecoilPitch.GetFloat()) * fPCorrect;
		m_fFireJitterPitch += fPVal;
		m_fPitch -= fPVal;

		// Move view left/right a bit...

		float fYawDiff = (float)fabs(m_fFireJitterYaw);
		float fYCorrect = 1.0f - (fYawDiff / DEG2RAD(g_vtMaxCamRecoilYaw.GetFloat()));
		float fYVal = GetRandom(-2.0f,2.0f) * DEG2RAD(g_vtBaseCamRecoilYaw.GetFloat()) * fYCorrect;
		m_fFireJitterYaw += fYVal;
		m_fYaw -= fYVal;
	}
	else
	{
		// Move view up a bit...(based on the current weapon/ammo type)

		WEAPON const *pWeaponData = 0;
		AMMO const *pAmmoData	= 0;
		IClientWeaponBase *pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
		if ( pClientWeapon )
		{
			pWeaponData = pClientWeapon->GetWeapon();
			pAmmoData = pClientWeapon->GetAmmo();
		}

		if (pWeaponData && pAmmoData)
		{
			if (fMaxPitch < 0.0f)
			{	
				//[SPIKE] 가속도로 증가
				LTFLOAT fCurFireRecoilPitch = 0.0f; // 기본 Perturb
				LTFLOAT fRandomOffset = pWeaponData->fFireRecoilPitch / 2.0f;

				m_fFireRecoilAccel += 1.0f;
				uint8 nCurAccel = 0;
				if(m_fFireRecoilAccel > 10.0f) m_fFireRecoilAccel = 10.0f;
				while(m_fFireRecoilAccel > (LTFLOAT)nCurAccel)
				{
					nCurAccel++;
					fCurFireRecoilPitch += GetRandom((pWeaponData->fFireRecoilPitch - fRandomOffset), (pWeaponData->fFireRecoilPitch + fRandomOffset));
				}
				m_fCurFireRecoilMaxPitch = fCurFireRecoilPitch;

				fMaxPitch = fCurFireRecoilPitch * pAmmoData->fFireRecoilMult ;
				fMaxPitchInter = pWeaponData->fFireRecoilPitchMax * pAmmoData->fFireRecoilMult ;
				//======================================
			}
		}

		//Dawn
		//float fVal = (float)DEG2RAD(fMaxPitch) - m_fFireJitterPitch;
		//m_fFireJitterPitch += fVal;
		//m_fPitch -= fVal;
		
		float fVal = (float)DEG2RAD(fMaxPitch);

		float fValInter = (float)DEG2RAD(fMaxPitchInter);
		if (m_fFireJitterPitch <= fValInter)
		{
			m_fFireJitterPitch += fVal;
			m_fPitch -= fVal;
			g_bUp	  = LTTRUE;
		}
		else
		{
			//[SPIKE] 난사에 정점에서..
			fVal = DEG2RAD(1.0f);
			if(g_bUp)
			{
				m_fFireJitterPitch += fVal;
				m_fPitch -= fVal;
				g_bUp = LTFALSE;
			}
			else
			{
				m_fFireJitterPitch -= fVal;
				m_fPitch += fVal;
				g_bUp = LTTRUE;
			}
			
			pClientWeapon->SetRandomFiringOn(LTTRUE);
			float fYawDiff = (float)fabs(m_fFireJitterYaw);
			
			if( fabs(m_fFireJitterYaw) > pWeaponData->fFireCamRecoilYawMax)
			{
				g_fLeft *= -1;
			}
			float fYVal = g_fLeft * GetRandom(0.5f,2.5f) * (DEG2RAD(pWeaponData->fFireCamRecoilYaw));

			m_fFireJitterYaw += fYVal;
			m_fYaw -= fYVal;

		}
	}

	if (fMaxPitch > 0.0f)
	{
		// Shake the screen if it isn't shaking...

		if (m_vShakeAmount.x < 0.1f && m_vShakeAmount.y < 0.1f && m_vShakeAmount.z < 0.1f)
		{
			LTVector vShake(0.1f, 0.1f, 0.1f);
			ShakeScreen(vShake);
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::DecayWeaponRecoil()
//
//	PURPOSE:	Decay the weapon's recoil...
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::DecayWeaponRecoil()
{
	// Decay firing jitter if necessary...

	if (g_vtUseCamRecoil.GetFloat() > 0.0f)
	{
		if (m_fFireJitterPitch > 0.0f)
		{
			float fCorrect = 1.0f + m_fFireJitterPitch / DEG2RAD(g_vtMaxCamRecoilPitch.GetFloat());
			float fVal = (g_pGameClientShell->GetFrameTime() * g_vtCamRecoilRecover.GetFloat()) * fCorrect;

			if (m_fFireJitterPitch < fVal)
			{
				fVal = m_fFireJitterPitch;
			}

			m_fFireJitterPitch -= fVal;
			m_fPitch += fVal;
		}

		float fYawDiff = (float)fabs(m_fFireJitterYaw);
		if (fYawDiff > 0.0f)
		{
			float fCorrect = 1.0f + fYawDiff / DEG2RAD(g_vtMaxCamRecoilYaw.GetFloat());
			float fVal = (g_pGameClientShell->GetFrameTime() * g_vtCamRecoilRecover.GetFloat()) * fCorrect;

			if (fYawDiff < fVal)
			{
				fVal = fYawDiff;
			}
			if (m_fFireJitterYaw < 0.0f)
				fVal *= -1.0f;

			m_fFireJitterYaw -= fVal;
			m_fYaw += fVal;
		}
	}
	else
	{
		if (m_fFireJitterPitch > 0.0f)
		{
			float fVal = m_fFireJitterPitch;

			// get the weapon and ammo data
			WEAPON const *pWeaponData = 0;
			AMMO const *pAmmoData	= 0;
			IClientWeaponBase *pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
			if ( pClientWeapon )
			{
				pWeaponData = pClientWeapon->GetWeapon();
				pAmmoData = pClientWeapon->GetAmmo();
			}
			
			if( !pWeaponData || !pAmmoData ) return;

			float fMaxPitch  = g_vtFireJitterMaxPitchDelta.GetFloat();
			float fTotalTime = g_vtFireJitterDecayTime.GetFloat();

			if (pWeaponData && pAmmoData)
			{
				if (fMaxPitch < 0.0f)
				{
				//	fMaxPitch = pWeaponData->fFireRecoilPitch * pAmmoData->fFireRecoilMult;
					fMaxPitch = m_fCurFireRecoilMaxPitch;//[SPIKE]
				}
				
				if (fTotalTime < 0.0f)
				{
					fTotalTime = pWeaponData->fFireRecoilDecay;
				}
			}

			if (fTotalTime > 0.01)
			{
				fVal = /*g_pGameClientShell->GetFrameTime() * */(DEG2RAD(fMaxPitch) / fTotalTime);
				//[SPIKE]********************************** 프레임 동기화 
				LTFLOAT fSyncFrame = g_pGameClientShell->GetFrameTime() * 70.0f;
				fVal *= fSyncFrame;	
				//**********************************
			}

			if (m_fFireJitterPitch - fVal < 0.0f)
			{
				fVal = m_fFireJitterPitch;
			}
			
			if(!pClientWeapon->GetRandomFiringOn() || pClientWeapon->GetState() == W_RELOADING)
			{
				m_fFireJitterPitch -= fVal;
				m_fPitch += fVal;
			}

			//[SPIKE] 본위치로 복귀  Yaw
			LTFLOAT fTemp0, fTemp1, fTemp2, fTemp3;
			const LTFLOAT MARN = 10000.0f;
			fTemp0 = m_fFireJitterPitch * MARN;
			fTemp1 = fVal * MARN;
			fTemp2  = fTemp0 / fTemp1;
			// Dawn
		/*	float fYawDiff = (float)fabs(m_fFireJitterYaw);
			float fYCorrect = 1.0f + (fYawDiff / DEG2RAD(pWeaponData->fFireCamRecoilYawMax));
			float fYVal = g_pGameClientShell->GetFrameTime() * (DEG2RAD(pWeaponData->fFireCamRecoilYaw) / fYCorrect);
		*/
			if(fabs(m_fFireJitterYaw) > 0.0f && !pClientWeapon->GetRandomFiringOn() || pClientWeapon->GetState() == W_RELOADING)
			{
				float fYawDiff = (float)fabs(m_fFireJitterYaw);
				fTemp0 = fYawDiff * MARN;
				fTemp3 = (fTemp0 / fTemp2) / (MARN/50);
				
				float fYVal = DEG2RAD(fTemp3);
				
				if (fYawDiff < fYVal)
					fYVal = fYawDiff ;
				if (m_fFireJitterYaw < 0.0f)
					fYVal *= -1.0f ;
				
				m_fFireJitterYaw -= fYVal;
				m_fYaw += fYVal;
			}
			//======================
		}
		else
		{
			if(GetRandom(0,1) > 0)
			{
				g_fLeft *= -1;
			}
			m_fFireJitterYaw   = 0.0f;
			m_fFireRecoilAccel = 0.0f;
		}
		
		//[SPIKE] 가속도 감소
		if(m_fFireRecoilAccel > 0) 
		{
			m_fFireRecoilAccel -= 0.07f;
			if(m_fFireRecoilAccel < 0.0f) 
				m_fFireRecoilAccel = 0.0f;
		}
		//=============================================
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::SetSpectatorMode()
//
//	PURPOSE:	Turn spectator mode on/off
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::SetSpectatorMode(LTBOOL bOn)
{
	m_bSpectatorMode = bOn;
	
	// Don't show stats in spectator mode...
	// Unless "alwaysHUD" console variable is set.

// Edited By IN'T ==>
// Time : 2004-05-04 오후 12:13:20
// 	if (bOn && ( g_vtAlwaysHUD.GetFloat() > 0.0f ) )
//	if (bOn && !( g_vtAlwaysHUD.GetFloat() > 0.0f ) )
	if (bOn)
// Edited By IN'T <==
	{
		g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderNone);
	}
	else
	{
		g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderFull);
	}

	if (m_pPlayerCamera->IsFirstPerson()) 
	{
 		m_pMoveMgr->SetSpectatorMode(bOn);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::ChangeWeapon()
//
//	PURPOSE:	Change the weapon model
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::ChangeWeapon(uint8 nWeaponId, 
							  uint8 nAmmoId,/*= WMGR_INVALID_ID*/
							  int dwAmmo/*= -1*/,
							  bool bForceSetting,
							  bool bForceLastSetting)
{
	g_pWpnChooser->SelectFromID( nWeaponId );
	m_pClientWeaponMgr->ChangeWeapon( nWeaponId, nAmmoId, dwAmmo, bForceSetting, bForceLastSetting );
	m_nTempZoomLevel = 0;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::HandleWeaponChanged()
//
//	PURPOSE:	Handle updating necessary player data after a weapon
//				change occurs...(NOTE: this should only be called from the
//				CClientWeaponMgr::ChangeWeapon() after a successful 
//				weapon change)
//
// ----------------------------------------------------------------------- //

	// jrg 9/2/02 - this will get called twice most of the time when switching weapons:
	//			once when the weapon switch is started
	//			and again when it completes (if a spwitching animation was needed)
	//		bImmediateSwitch will be true on the second call (or the first if the switch is immediate)
	//		(I'm using the repeated call to track whether we are in mid switch)

void CPlayerMgr::HandleWeaponChanged(uint8 nWeaponId, uint8 nAmmoId, bool bImmediateSwitch)
{
	// Turn off zooming...
	//HandleZoomChange( LTTRUE );

	// 광식 
	if( bImmediateSwitch && !IsPlayerDead() )
	{
		// Tell the server to change weapons...
		LTRESULT ltResult;
		CAutoMessage cMsg;
		IClientWeaponBase* pClientWeapon = GetCurrentClientWeapon( );
		if( !pClientWeapon ) return;

		cMsg.Writeuint8( MID_WEAPON_CHANGE );
		cMsg.Writeuint8( nWeaponId );
		cMsg.Writeuint8( nAmmoId );	

		// 광식 : ServerShell이 알고 있는 총알 개수와 틀려지는것 방지
		//------------------------------------------------------------------------
		uint8 nAmmoCount	= (uint8)g_pPlayerStats->GetAmmoCount(pClientWeapon->GetAmmoId());
		uint8 nClipInAmmo	= (uint8)pClientWeapon->GetAmmoInClip();

		cMsg.Writeuint8(nAmmoCount);
		cMsg.Writeuint8(nClipInAmmo);
		//------------------------------------------------------------------------

		ltResult = g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );
		ASSERT( LT_OK == ltResult );

		uint32 nID;
		g_pLTClient->GetLocalClientID(&nID);
		g_Network.CS_WeaponChange( (uint16)nID,
								   nWeaponId,
								   g_pPlayerStats->GetAmmoCount(pClientWeapon->GetAmmoId()),
								   pClientWeapon->GetAmmoInClip() );

		g_pGameClientShell->HandleWeaponChange( (uint16)nID,
												nWeaponId,
												g_pPlayerStats->GetAmmoCount(pClientWeapon->GetAmmoId()),
												pClientWeapon->GetAmmoInClip() );
	}
	
	// this isn't strictly necessary, the hide should have been
	// called when we entered the chase view
	if ( m_pPlayerCamera->IsChaseView() )
	{
		m_pClientWeaponMgr->HideWeapons();
	}

	//shutdown the weapon or ammo choosers...
	g_pInterfaceMgr->CloseChoosers();

	//when the swicth is complete...
	if (bImmediateSwitch)
	{
		//if we are changing to a gadget, we're done switching
		if (m_bChangingToGadget)
		{
			m_bChangingToGadget = false;
		}
		else
		{
			//we are not changing to a gadget, so forget about any gadget related stuff...
			if (m_nPreGadgetWeapon != WMGR_INVALID_ID )
			{
				m_nPreGadgetWeapon = WMGR_INVALID_ID;
			}
		}  
	}

	m_bSwitchingWeapons = !bImmediateSwitch;
}

void CPlayerMgr::HandlePreloadWeapon( ILTMessage_Read* pMsg )
{
	PreLoadingWeapon();
}

//[SPIKE]
LTBOOL CPlayerMgr::PreLoadingWeapon()
{
	// SPIKE 소유무기 강제로딩 =================================
	if( g_pPlayerStats )
	{	
		uint8 nTotalCount = 4; // 로딩할 Total Count
		if( eGameTypeSurvival == g_pGameClientShell->GetGameType() && g_GameDoc.GetTeam() == TEAM_A )
			nTotalCount = 5;
		
		int	iOrderedWeaponIndex[5] = {LTNULL};
		
		for(int i = 0; i<nTotalCount; i++)
		{
			iOrderedWeaponIndex[i] = *(g_pPlayerStats->GetOrderedWeaponId()+i);
			if(iOrderedWeaponIndex[i] != -1)  // 소유한 무기 갯수 
			{
				ChangeWeapon(iOrderedWeaponIndex[i], WMGR_INVALID_ID, -1, LTFALSE);
			}			
		}
		
		if( g_GameDoc.IsMandateHost() || g_GameDoc.IsRejoinHost() )
		{
			ChangeWeapon(g_GameDoc.GetBackupSelectedWeaponID(), WMGR_INVALID_ID, -1, LTFALSE, LTTRUE);
		}
		else
		{
			if(iOrderedWeaponIndex[1] == -1)  // 칼 기본 
				ChangeWeapon(iOrderedWeaponIndex[2], WMGR_INVALID_ID, -1, LTFALSE, LTTRUE);
			else
				ChangeWeapon(iOrderedWeaponIndex[1], WMGR_INVALID_ID, -1, LTFALSE, LTTRUE);
		}
		return LTTRUE;
	}
	return LTFALSE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::ToggleHolster()
//
//	PURPOSE:	Toggle the holster state of the weapon.
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::ToggleHolster( bool bPlayDeselect )
{
	// toggle the holster
	m_pClientWeaponMgr->ToggleHolster( bPlayDeselect );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::LastWeapon()
//
//	PURPOSE:	Swap to our previously used weapon.
//
// ----------------------------------------------------------------------- //
void CPlayerMgr::LastWeapon()
{
	// toggle the last weapon
	m_pClientWeaponMgr->LastWeapon();
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CPlayerMgr::BeginSpyVision
//
//  PURPOSE:	setup spy vision mode
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::BeginSpyVision()
{
/* KLS - 6/22/02 - Spy vision has been cut

  // Turn on the glow effect (and set the necessary values)...
	g_pLTClient->RunConsoleString("ScreenGlowEnable 1");

	// Turn off shadows (if they're on)...
	g_nSaveSpyVisionShadows = GetConsoleInt("MaxModelShadows", 0);
	WriteConsoleInt("MaxModelShadows", 0);

    g_pLTClient->SetModelHook((ModelHookFn)SVModelHook, g_pGameClientShell);
	m_bSpyVision = true;

*/
}


// ----------------------------------------------------------------------- //
//
//  ROUTINE:	CPlayerMgr::EndSpyVision
//
//  PURPOSE:	end spy vision mode
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::EndSpyVision()
{
/* KLS - 6/22/02 - Spy vision has been cut

	// Turn off the glow effect...
	g_pLTClient->RunConsoleString("ScreenGlowEnable 0");

	// Turn on shadows (if they were on)...NOTE: this assumes the user won't change
	// the state of the shadows while in spy vision...
	WriteConsoleInt("MaxModelShadows", g_nSaveSpyVisionShadows);

    g_pLTClient->SetModelHook((ModelHookFn)DefaultModelHook, g_pGameClientShell);
	m_bSpyVision = false;

*/
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::BeginZoom()
//
//	PURPOSE:	prepare for zooming
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::BeginZoom()
{
	uint8 nScopeId = g_pPlayerStats->GetScope();

	m_bCamera = LTFALSE;
	uint8 nAmmoId = g_pPlayerStats->GetCurrentAmmo();
	if (nAmmoId != WMGR_INVALID_ID)
	{
		AMMO const *pAmmo = g_pWeaponMgr->GetAmmo(nAmmoId);
		if (pAmmo)
		{
			m_bCamera = (pAmmo->eInstDamageType == DT_GADGET_CAMERA);
		}
	}


	g_pInterfaceMgr->BeginScope(m_bSpyVision, m_bCamera);

	m_pClientWeaponMgr->HideWeapons();
	
	m_fZoomInBreathTime = g_pLTClient->GetTime() + FIRST_ZOOM_BREATH;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::EndZoom()
//
//	PURPOSE:	done zooming
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::EndZoom()
{
	g_pInterfaceMgr->EndScope();
	m_bCamera = LTFALSE;

	// show the weapon
	m_pClientWeaponMgr->ShowWeapons();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::HandleZoomChange()
//
//	PURPOSE:	Handle a potential zoom change
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HandleZoomChange(LTBOOL bReset)
{
	// Reset to normal FOV...

	if (bReset)
	{
	//	if (m_nZoomView == 0) return;

		m_nZoomView  = 0;
		m_bZooming   = LTFALSE;
		m_bZoomingIn = LTFALSE;
		g_bHalfZoomMode = LTFALSE;
		g_fPVModelFOVStep = 0.0f;
		m_fZoomIn_Increase	= 50.0f;
		m_fZoomIn2_Increase = 20.0f;
		g_pInterfaceMgr->EndZoom();
		EndZoom();

		SetCameraFOV(DEG2RAD(g_vtFOVXNormal.GetFloat()), DEG2RAD(g_vtFOVYNormal.GetFloat()));

		char strConsole[40];
		sprintf(strConsole, "+ModelLODOffset %f", m_fSaveLODScale);
		g_pLTClient->RunConsoleString(strConsole);
	}


    uint8 nScopeId = g_pPlayerStats->GetScope();
	if (nScopeId == WMGR_INVALID_ID) return;

	MOD const *pMod = g_pWeaponMgr->GetMod(nScopeId);
	if (!pMod) return;

	// Play zoom in/out sounds...

	if (m_bZoomingIn)
	{
		if (pMod->szZoomInSound[0])
		{
			g_pClientSoundMgr->PlaySoundLocal(pMod->szZoomInSound, SOUNDPRIORITY_MISC_MEDIUM, 0, 90);
		}
	}
	else
	{
		if (pMod->szZoomOutSound[0])
		{
			g_pClientSoundMgr->PlaySoundLocal(pMod->szZoomOutSound, SOUNDPRIORITY_MISC_MEDIUM, 0, 90);
		}
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::SetCameraFOV
//
//	PURPOSE:	Set the camera's FOV
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::SetCameraFOV(float fFovX, float fFovY)
{
	if (!m_hCamera) return;

    g_pLTClient->SetCameraFOV(m_hCamera, fFovX, fFovY);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateCameraZoom
//
//	PURPOSE:	Update the camera's field of view
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateCameraZoom()
{
	char strConsole[30];

    uint32 dwWidth = 640, dwHeight = 480;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &dwWidth, &dwHeight);

    float fovX, fovY;
    g_pLTClient->GetCameraFOV(m_hCamera, &fovX, &fovY);

	IClientWeaponBase* pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	if( !pClientWeapon ) return;

    float fOldFovX = fovX;

	if (!fovX)
	{
		fovX = DEG2RAD(g_vtFOVXNormal.GetFloat());
	}

    m_bZooming = LTTRUE;


    float fFovXZoomed, fZoomDist;

	if (m_bZoomingIn)
	{
		if (m_nZoomView == 1)
		{
			LTFLOAT fTempFovX = 0.0f;
			// 광식  : 죽어서 Attach 되었을 때 처리
			if( IsPlayerDead() && IsRevivePlayerGameType() )
			{
				uint8 nHalfScopeId = WMGR_INVALID_ID;
				int nPriority	= -1;
				int m = pClientWeapon->GetWeapon()->nNumModIds;
				for( int i = 0; i < m; i++)
				{
					MOD const *pMod = g_pWeaponMgr->GetMod(pClientWeapon->GetWeapon()->aModIds[i]);
					if( pMod && pMod->eType == HALF_SCOPE )
					{
						if (pMod->nPriority > nPriority)
						{
							nPriority = pMod->nPriority;
							nHalfScopeId = pClientWeapon->GetWeapon()->aModIds[i];
						}
					}
				}

				if( nHalfScopeId != WMGR_INVALID_ID )
					fTempFovX = FOVX_HALFZOOMED;
				else fTempFovX = FOVX_ZOOMED;
			}
			else
			{
				if(g_pPlayerStats->GetHalf_Scope() != WMGR_INVALID_ID)
					fTempFovX = FOVX_HALFZOOMED;
				else fTempFovX = FOVX_ZOOMED;
			}
			/*
			//[SPIKE] HalfZoom과 SniperZoom과 거리 구분 
			LTFLOAT fTempFovX = 0.0f;
			if(g_pPlayerStats->GetHalf_Scope() != WMGR_INVALID_ID)
				fTempFovX = FOVX_HALFZOOMED;
			else fTempFovX = FOVX_ZOOMED;
			*/

			if(g_pPlayerStats->GetHalf_Scope() != WMGR_INVALID_ID)
			{
				m_fZoomIn_Increase -= 5.0f;
				LTFLOAT fTempFovY = pClientWeapon->GetWeapon()->m_fPVModelFOVY;
				g_fPVModelFOVStep += 5.0f;
				WriteConsoleFloat("PVModelFOV", fTempFovY - g_fPVModelFOVStep);
			}
			else
			{
				m_fZoomIn_Increase -= 7.0f;
			}
			

			if(m_fZoomIn_Increase < fTempFovX)
			{
				m_fZoomIn_Increase = fTempFovX;
				m_bZooming = LTFALSE;
				pClientWeapon->SetZoomMode(VOLTSNIPER_ZOOMFREE);
			}
			fFovXZoomed	= DEG2RAD(m_fZoomIn_Increase);
			//fFovXZoomed	= DEG2RAD(FOVX_ZOOMED);
			////////////////////////////////////////////////////////////////
			fZoomDist	= DEG2RAD(g_vtFOVXNormal.GetFloat()) - fFovXZoomed;
		}
		else if (m_nZoomView == 2)
		{
			//[SPIKE]  // Volt액션 스나이퍼일 경우 
			if(pClientWeapon->GetZoomMode() == VOLTSNIPER_ZOOMIN)
				m_fZoomIn2_Increase -= 9.0f;
			else	
				m_fZoomIn2_Increase -= 3.0f;

			if(m_fZoomIn2_Increase < FOVX_ZOOMED1)
			{
				m_fZoomIn2_Increase = FOVX_ZOOMED1;
				m_bZooming = LTFALSE;
				pClientWeapon->SetZoomMode(VOLTSNIPER_ZOOMFREE);
			}
			fFovXZoomed	= DEG2RAD(m_fZoomIn2_Increase);
			//fFovXZoomed	= DEG2RAD(FOVX_ZOOMED1);
			////////////////////////////////////////////////////////////////
			
			// KLS - Always zoom all the way in...
			fZoomDist	= DEG2RAD(g_vtFOVXNormal.GetFloat()) - fFovXZoomed;
			// Remove above code and uncomment line below to do old 
			// step zoom behavoir
			//fZoomDist	= DEG2RAD(FOVX_ZOOMED) - fFovXZoomed;
			
		}
		else if (m_nZoomView == 3)
		{
			fFovXZoomed	= DEG2RAD(FOVX_ZOOMED2);
			// KLS - Always zoom all the way in...
			fZoomDist	= DEG2RAD(g_vtFOVXNormal.GetFloat()) - fFovXZoomed;
			// Remove above code and uncomment line below to do old 
			// step zoom behavoir
			//fZoomDist	= DEG2RAD(FOVX_ZOOMED1) - fFovXZoomed;
		}
	}
	else
	{
		if (m_nZoomView == 0)
		{
			fFovXZoomed	= DEG2RAD(g_vtFOVXNormal.GetFloat());
			fZoomDist	= DEG2RAD(g_vtFOVXNormal.GetFloat()) - DEG2RAD(FOVX_ZOOMED);
		}
		else if (m_nZoomView == 1)
		{
			fFovXZoomed	= DEG2RAD(FOVX_ZOOMED);
			fZoomDist	= fFovXZoomed - DEG2RAD(FOVX_ZOOMED1);
		}
		else if (m_nZoomView == 2)
		{
			fFovXZoomed	= DEG2RAD(FOVX_ZOOMED1);
			fZoomDist	= fFovXZoomed - DEG2RAD(FOVX_ZOOMED2);
		}
	}

    float fZoomVel = fZoomDist / ZOOM_TIME;

    // Edited By Bestline ==>
	// Time : 2004-07-16 오후 5:36:36
	// float fZoomAmount = fZoomVel * g_pGameClientShell->GetFrameTime();
	// Edited By Bestline <==

	float fZoomAmount = fZoomVel;
	// Zoom camera in or out...

	if (m_bZoomingIn)
	{
		if (fovX > fFovXZoomed)
		{
			// Zoom camera in...

			fovX -= fZoomAmount;
		}

		if (fovX <= fFovXZoomed)
		{
			fovX = fFovXZoomed;
        //    m_bZooming = LTFALSE;
			g_pInterfaceMgr->EndZoom();
		}
	}
	else  // Zoom camera out...
	{
		if (fovX < fFovXZoomed)
		{
			// Zoom camera out...

			fovX += fZoomAmount;
		}

		if (fovX >= fFovXZoomed)
		{
			fovX = fFovXZoomed;
            m_bZooming = LTFALSE;
			g_pInterfaceMgr->EndZoom();
			if (m_nZoomView == 0)
			{
				EndZoom();
			}
		}
	}

	if (fOldFovX != fovX)
	{
		fovY = (fovX * DEG2RAD(g_vtFOVYNormal.GetFloat())) / DEG2RAD(g_vtFOVXNormal.GetFloat());

		SetCameraFOV(fovX, fovY);

		// Update the lod adjustment for models...
		float fZoomAmount = (DEG2RAD(g_vtFOVXNormal.GetFloat()) - fovX) / (DEG2RAD(g_vtFOVXNormal.GetFloat()) - DEG2RAD(FOVX_ZOOMED2));
		float fNewLODOffset = m_fSaveLODScale + (LOD_ZOOMADJUST * fZoomAmount);

		sprintf(strConsole, "+ModelLODOffset %f", fNewLODOffset);
        g_pLTClient->RunConsoleString(strConsole);

        //g_pLTClient->CPrint("Current FOV (%f, %f)", fovX, fovY);
        //g_pLTClient->CPrint("Current Zoom LODOffset: %f", fNewLODOffset);
	}

}






// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::OnCommandOn()
//
//	PURPOSE:	Handle client commands
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerMgr::OnCommandOn(int command)
{
	// Make sure we're in the world...

	if (!IsPlayerInWorld()) return LTFALSE; 

	// See if the vehiclemgr would like to trap this
 	if(m_pMoveMgr->GetVehicleMgr()->OnCommandOn(command))
	{
		return LTTRUE;
	}

	// Check for weapon change...

	if (g_pWeaponMgr->GetFirstWeaponCommandId() <= command &&
		command <= g_pWeaponMgr->GetLastWeaponCommandId())
	{
		ChangeWeapon( g_pWeaponMgr->GetWeaponId( command ) );
		return LTTRUE;
	}

	// Take appropriate action
	switch (command)
	{
		//폭탄 해체
		case COMMAND_ID_LEAN_RIGHT :
		{
			if (GetTargetMgr()->IsTargetInRange())
			{
				DamageType eDT = GetTargetMgr()->RequiredGadgetDamageType();
				if (eDT == DT_INVALID || GetGadgetDisabler()->DisableOnActivate())
				{
					if (g_pDamageFXMgr->AllowWeapons( ))
						DoActivate();

					//g_pClientSoundMgr->PlayInterfaceSound("SA_Weapons\\SND\\TimeBomb\\disarm.wav");
				}
			}
		}
		break;

		case COMMAND_ID_FIRING :
		{
			if (IsPlayerDead())
			{
				if (IsMultiplayerGame())
				{
                    if (g_pLTClient->GetTime() > m_fEarliestRespawnTime)
					{
						// Allow respawn after a certain amount of time. This way the Player 
						// can remain in 'limbo' for as long as they want and wait to be revived.

						HandleRespawn();
					}
				}
			}
		}
		break;

		case COMMAND_ID_RELOAD :
		{
			IClientWeaponBase *pClientWeapon = g_pPlayerMgr->GetCurrentClientWeapon();
			if ( pClientWeapon )
			{
				// Reload the clip and let the server know...
				pClientWeapon->ReloadClip( true, -1, false, true );
			}
		}
		break;

#ifndef _TO2DEMO
		case COMMAND_ID_NEXT_VISMODE :
		{
			m_pVisionModeMgr->NextMode();
		}
		break;
#endif
		/*
		case COMMAND_ID_ZOOM_IN :
		{
			if ( !m_pClientWeaponMgr->WeaponsEnabled() ) break;

			uint8 nScopeId = g_pPlayerStats->GetScope();
			//[SPIKE]Half_Scope
			uint8 nHalfScopeId = g_pPlayerStats->GetHalf_Scope();
			if (nScopeId == WMGR_INVALID_ID && nHalfScopeId == WMGR_INVALID_ID) break;
			
			MOD const * pMod;
			if(nScopeId != WMGR_INVALID_ID)
				pMod = g_pWeaponMgr->GetMod(nScopeId);
			else
				pMod = g_pWeaponMgr->GetMod(nHalfScopeId);
			if (!pMod) break;

			int nZoomLevel	  = pMod->nZoomLevel;
			int nMaxZoomLevel = pMod->nMaxZoomLevel;
			
			WeaponState pCurrentState =  GetCurrentClientWeapon()->GetState();
			if(pCurrentState != W_FIRING && pCurrentState != W_RELOADING)
			{
				if(!m_bZooming && nZoomLevel > 0)
				{
					if (m_nZoomView == 0 && nScopeId != WMGR_INVALID_ID)
						BeginZoom();		// Scope 생성
					else
					{
						if(g_bHalfZoomMode)
						{
							LTFLOAT fTempFovY = GetCurrentClientWeapon()->GetWeapon()->m_fPVModelFOVY;
							WriteConsoleFloat("PVModelFOV", fTempFovY - 15.0f);
						}
					}
					m_nZoomView += nZoomLevel;
					m_bZooming   = LTTRUE;
					m_bZoomingIn = LTTRUE;
					g_pInterfaceMgr->BeginZoom(LTTRUE);
					HandleZoomChange();
					g_bHalfZoomMode = LTTRUE;  // Reload시 한번만 초기화 하기 위해 
					if (m_nZoomView > nMaxZoomLevel)
					{
						m_nZoomView = 0;
						m_bZooming   = LTTRUE;
						m_bZoomingIn = LTFALSE;
						m_fZoomIn_Increase	= 50.0f;
						m_fZoomIn2_Increase = 20.0f;
						g_pInterfaceMgr->BeginZoom(LTFALSE);
						HandleZoomChange();
						g_bHalfZoomMode = LTFALSE;

						if(nScopeId == WMGR_INVALID_ID)
							WriteConsoleFloat("PVModelFOV", GetCurrentClientWeapon()->GetWeapon()->m_fPVModelFOVY);
					}
				}
			}
		}
		break;
		*/

		case COMMAND_ID_TURNAROUND :
		{
			m_fYaw += MATH_PI;
		}
		break;

		case COMMAND_ID_RUNLOCK :
		{
			if (!m_pMoveMgr->GetVehicleMgr()->IsVehiclePhysics())
			{
				m_pMoveMgr->SetRunLock(!m_pMoveMgr->RunLock());
			}
		}
		break;

		case COMMAND_ID_JUMP : 
		case COMMAND_ID_DUCK : 
		{
			m_pMoveMgr->SetDuckLock(LTFALSE); 
		}
		break;

		default :
			return LTFALSE;
		break;
	}
	return LTTRUE;

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::OnCommandOff()
//
//	PURPOSE:	Handle command off notification
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerMgr::OnCommandOff(int command)
{
	// See if the vehiclemgr would like to trap this
	if(m_pMoveMgr->GetVehicleMgr()->OnCommandOff(command))
	{
		return LTTRUE;
	}

	return LTTRUE;

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::HandleMsgPlayerStateChange()
//
//	PURPOSE:	
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HandleMsgPlayerStateChange (ILTMessage_Read *pMsg)
{
	m_ePlayerState = (PlayerState) pMsg->Readuint8();

	switch (m_ePlayerState)
	{
		case PS_GHOST:
			m_pPlayerCamera->SetCameraMode( CPlayerCamera::CameraMode::DEAD );
			m_nSpectatorViewIndex	 = MID_ATTACH_FIRST;
			break;

		case PS_DYING:		
		{
			// Clear out any damagefx...
			// Do this first since we may be unattaching the camera from the head when clearing out a 
			// damage fx.
			
			//[SPIKE] 
			uint8 nKiller = pMsg->Readuint8();
			
			if(nKiller != 255)
			{
				T_PLAYER_INFOptr pKiller = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByID(nKiller);
				if( pKiller )
				{
					SetKillerObject(pKiller->hObject, (pKiller->nServerIndex != g_GameDoc.GetUniqueIndex()));
				}	
			}
			//================================	

			g_pDamageFXMgr->Clear();
			
			//AttachCameraToHead(LTTRUE);
			m_pPlayerCamera->SetCameraMode (CPlayerCamera::FIRSTDYING) ;
			
			//g_pCameraBloodFX->Show(false);
			g_pHUDFlashBangFX->SetVisible(false);
			
			if( IsMultiplayerGame() )
			{
				m_fMultiplayerDeathCamMoveTimer = 0.0f;
				m_fMultiAttachDeathCamTimer = 0.0f;

				switch (g_pGameClientShell->GetGameType())
				{
				case eGameTypeDeathmatch:
				case eGameTypeTeamDeathmatch:
					{
						m_fSendRespawnTime	= 0.0f;
						m_fEarliestRespawnTime = g_pLTClient->GetTime() + g_vtMultiplayerRespawnWaitTime.GetFloat();
					}
					break;
				}

				// 광식
				// [05.02.11] : ControlFlags Test
				//g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderDead);
				g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderText);
				g_pHUDMgr->QueueUpdate(kHUDRespawn);
			}
			else
			{
				m_fSendRespawnTime	= 0.0f;
				m_fEarliestRespawnTime = g_pLTClient->GetTime() + g_vtRespawnWaitTime.GetFloat();

				g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderNone);
			}

			if( g_pMoveMgr->GetVehicleMgr()->GetPhysicsModel() == PPM_LURE )
			{
				// Get the playerlurefx.
				PlayerLureFX* pPlayerLureFX = PlayerLureFX::GetPlayerLureFX( g_pMoveMgr->GetVehicleMgr()->GetPlayerLureId() );
				if( pPlayerLureFX )
				{
					char const* pszDeathFX = pPlayerLureFX->GetDeathFX( );
					if( !pszDeathFX && pszDeathFX[0] != '\0' )
					{
						LTVector vPos;
						g_pLTClient->GetObjectPos( g_pMoveMgr->GetObject(), &vPos );

						CLIENTFX_CREATESTRUCT fxInit( pszDeathFX, 0, vPos );
						g_pClientFXMgr->CreateClientFX( LTNULL, fxInit, LTTRUE );
					}
				}
			}

			ClearPlayerModes();

			m_pClientWeaponMgr->OnPlayerDead();

			g_pInterfaceMgr->AddToClearScreenCount();

			if (!IsMultiplayerGame( ))
			{
				g_pGameClientShell->CSPrint(LoadTempString(IDS_YOUWEREKILLED));
				g_pInterfaceMgr->StartScreenFadeOut(g_vtScreenFadeOutTime.GetFloat());
			}

			//[MURSUM]
			LTVector vZero(0.0f,0.0f,0.0f);
			g_pCommonLT->SetObjectFlags( g_pMoveMgr->GetObject(), OFT_Flags, 0, FLAG_SOLID );
			g_pPhysicsLT->SetAcceleration( g_pMoveMgr->GetObject(), &vZero );
			g_pPhysicsLT->SetVelocity( g_pMoveMgr->GetObject(), &vZero );
		}
		break;

		case PS_ALIVE:
		{
			// 광식
			m_nAttachState = MID_DETACH_DISPLAY;

//			g_pScores->Show(false);
			g_pHUDMgr->QueueUpdate(kHUDRespawn);
			g_pDamageFXMgr->Clear();					// Remove all the damage sfx
			ClearDamageSectors();

			//g_pInterfaceMgr->ForceScreenFadeIn(g_vtScreenFadeInTime.GetFloat());

			AttachCameraToHead(LTFALSE);

			g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderFull);

			SetExternalCamera(LTFALSE);

			m_pClientWeaponMgr->OnPlayerAlive();

			if( IsMultiplayerGame() )
			{
				AllowPlayerMovement( LTTRUE );
			}
			
			// Since we are alive, disassociate any body from us...

			CSpecialFXList* pList = g_pGameClientShell->GetSFXMgr()->GetFXList(SFX_BODY_ID);
			if( !pList ) return;
			
			int nNumBodies = pList->GetSize();

			uint32 dwId;
			g_pLTClient->GetLocalClientID( &dwId );

			for( int i = 0; i < nNumBodies; ++i )
			{
				if ((*pList)[i])
				{
					CBodyFX* pBody = (CBodyFX*)(*pList)[i];

					if( pBody->GetClientId() == dwId )
					{
						pBody->RemoveClientAssociation( );
					}
				}
			}

			g_pCommonLT->SetObjectFlags( g_pMoveMgr->GetObject(), OFT_Flags, FLAG_SOLID, FLAG_SOLID );
		}
		break;

		case PS_DEAD:
		{
			g_pDamageFXMgr->Clear();					// Remove all the damage sfx
			ClearDamageSectors();
			s_fDeadTimer = 0.0f;
			m_nSpectatorViewIndex = MID_ATTACH_FIRST;
		}
		break;
		
		case PS_MANDATE_DEAD:
		{
			switch (g_pGameClientShell->GetGameType())
			{
			case eGameTypeSurvival:	
				//[MURSUM] Del Nick 
				/*
				{
					CSFXMgr* psfxMgr = g_pGameClientShell->GetSFXMgr();
					if( psfxMgr ) psfxMgr->SetCharFX_PlayerNick();
				}
				*/
				break;
			case eGameTypeTeamDeathmatch:
				m_fSendRespawnTime	= 0.0f;
				m_fEarliestRespawnTime = g_pLTClient->GetTime() + g_GameDoc.GetBackupPlayerRespawnTime();
				break;
			}

			ClearPlayerModes();

			m_pClientWeaponMgr->OnPlayerDead();
			g_pInterfaceMgr->AddToClearScreenCount();
			g_pCommonLT->SetObjectFlags( g_pMoveMgr->GetObject(), OFT_Flags, 0, FLAG_SOLID );

			g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderText);
			g_pHUDMgr->QueueUpdate(kHUDRespawn);

			/*
			g_pPhysicsLT->MoveObject( GetMoveMgr()->GetServerObject(),
									  &g_GameDoc.GetBackupPlayerPos(),
									  MOVEOBJECT_TELEPORT);
									  */
		}
		break;

		// 광식 : PS_GHOST 를 Round 방식에서 난입했을 때 사용
/*
		case PS_GHOST:
		{
			AttachCameraToHead(LTFALSE);
			m_pClientWeaponMgr->HideWeapons();

			g_pInterfaceMgr->SetHUDRenderLevel(kHUDRenderNone);

			g_pInterfaceMgr->ForceScreenFadeIn(g_vtScreenFadeInTime.GetFloat());

//			SetExternalCamera(LTTRUE);

			m_pClientWeaponMgr->DisableWeapons();
		}
		break;
*/
		default : break;
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::HandleRespawn
//
//	PURPOSE:	Handle player respawn
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::HandleRespawn()
{
	if( !IsPlayerDead() || m_fSendRespawnTime > 0.0f )
	{
		m_fSendRespawnTime	-= g_pLTClient->GetFrameTime();
		return;
	}

	m_fSendRespawnTime	= 0.5f;

	// 광식 : Respawn 시 시작할 때 선택한 총으로 바꾸기
	g_pPlayerStats->RestartWeapon();
	//===========================================================================
		
	SendEmptyServerMsg(MID_PLAYER_RESPAWN);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::PreRender()
//
//	PURPOSE:	Sets up the client for rendering
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerMgr::PreRender()
{
	if (!m_bCameraPosInited) return LTFALSE;

		// Make sure the rendered player object is right where it should be.

	UpdateServerPlayerModel();

	// Make sure we process attachments before updating the weapon model
	// and special fx...(some fx are based on attachment positions/rotations)

	g_pLTClient->ProcessAttachments(g_pLTClient->GetClientObject());

	// Make sure the weapon is updated before we render the camera...

	UpdateWeaponModel();

	// Make sure the move-mgr models are updated before we render...

	m_pMoveMgr->UpdateModels();

	// Update the flash light...

	m_pFlashLight->Update();

	return LTTRUE;
}




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdatePlayerCamera()
//
//	PURPOSE:	Update the player camera
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerMgr::UpdatePlayerCamera()
{
	// Make sure our player camera is attached...

	if (m_pPlayerCamera->GetAttachedObject() != m_pMoveMgr->GetObject())
	{
		// 광식 : spectator Mode에서 다른 사람에게 camera 붙이기
		//============================================================
		if( m_pPlayerCamera->IsChaseView() )
		{			
		}
		else
		{
			if( IsPlayerDead() )
			{
				if( m_pPlayerCamera->GetAttachedObject() == LTNULL )
				{
					int ttt = 0;
				}
				m_pPlayerCamera->AttachToObject(m_pPlayerCamera->GetAttachedObject());
			}
			else
			{
				m_pPlayerCamera->AttachToObject(m_pMoveMgr->GetObject());
			}			
		}
		//============================================================

		//m_pPlayerCamera->AttachToObject(m_pMoveMgr->GetObject());
	}

	if (m_pPlayerCamera->IsChaseView())
	{
		// Init the camera so they can be adjusted via the console...

		InitPlayerCamera();

		Update3rdPersonInfo();
	}


	// Update our camera position based on the player camera...

	m_pPlayerCamera->CameraUpdate(g_pGameClientShell->GetFrameTime());

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::InitPlayerCamera()
//
//	PURPOSE:	Update the player camera
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::InitPlayerCamera()
{
    LTVector vOffset(0.0f, 0.0f, 0.0f);
	vOffset.y = g_vtChaseCamOffset.GetFloat();

	m_pPlayerCamera->SetDistUp(g_vtChaseCamDistUp.GetFloat());
	m_pPlayerCamera->SetDistBack(g_vtChaseCamDistBack.GetFloat());
	m_pPlayerCamera->SetPointAtOffset(vOffset);
	m_pPlayerCamera->SetChaseOffset(vOffset);
	m_pPlayerCamera->SetCameraState(CPlayerCamera::SOUTH);

	// Determine the first person offset...

	vOffset = g_vPlayerCameraOffset;

	CCharacterFX* pChar = m_pMoveMgr->GetCharacterFX();
	if (pChar)
	{
		vOffset = GetPlayerHeadOffset( );
	}

	m_pPlayerCamera->SetFirstPersonOffset(vOffset);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateCameraPosition()
//
//	PURPOSE:	Update the camera position
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdateCameraPosition()
{
    LTVector vPos = m_pPlayerCamera->GetPos();

	if (m_pPlayerCamera->IsFirstPerson())
	{
		// Special case of camera being attached to the player's head
		// (i.e., death)...
		if (m_bCameraAttachedToHead)
		{
			LTVector	vHeadPos;	
			LTRotation	rHeadRot;

			HLOCALOBJ hPlayer = g_pLTClient->GetClientObject();

			// Make sure we never go backwards in our animation...

			uint32 dwCurTime = 0;
			static uint32 dwPrevTime = 0;
			
			g_pModelLT->GetCurAnimTime( hPlayer, MAIN_TRACKER, dwCurTime );

			if( (dwCurTime > 0) && (dwCurTime < dwPrevTime) )
			{
				uint32 dwFrameTime = (uint32)(10000.0f * g_pGameClientShell->GetFrameTime());
				g_pModelLT->SetCurAnimTime( hPlayer, MAIN_TRACKER, dwPrevTime + dwFrameTime );
			}
			dwPrevTime = dwCurTime;
			
			GetPlayerHeadPosRot(vHeadPos, rHeadRot);
			vPos = vHeadPos;

			if( m_bLerpAttachedCamera )
			{
				m_rRotation.Slerp( m_rRotation, rHeadRot, g_vtAttachedCamInterpolationRate.GetFloat( 0.1f ) );
			}
			else
			{
				m_rRotation = rHeadRot;
			}
			
			//save this rotation into the yaw pitch and roll as well so that when we
			//come out of the attached mode, our view won't pop
			EulerAngles EA = Eul_FromQuat( m_rRotation, EulOrdYXZr );
			
			m_fModelAttachYaw		= EA.x;
			m_fModelAttachPitch		= EA.y;
			m_fModelAttachRoll		= EA.z;
		}
		else
		{
			//normal updating of the head position
			m_pHeadBobMgr->AdjustCameraPos(vPos);

			if( m_nAttachState != MID_DETACH_OBSERVER )
			{
				vPos.y	+= m_fCamDuck + 5.0f;	
			}
			vPos	+= m_pCameraOffsetMgr->GetPosDelta();			
		}
		
		g_pLTClient->SetObjectPos(m_hCamera, &vPos);
		
	}
	else
	{
        m_rRotation = m_pPlayerCamera->GetRotation();
		if( m_bTestCamera == false )
			g_pLTClient->SetObjectPos(m_hCamera, &vPos);		
	}

    m_bCameraPosInited = LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateMultiplayerCameraPosition()
//
//	PURPOSE:	Update the camera position while in a multiplayer game...
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdateMultiplayerCameraPosition()
{
	// If the player is alive don't do anything special.  Just treat like single player....
	
	//if( IsPlayerDead() )
	// 광식
	// [05.02.11] : ControlFlags Test
	if( IsPlayerDead() && g_pGameClientShell->GetGameType() != eGameTypeSurvival )
	{
		HOBJECT hBody = NULL;

		// We actually want to use the body prop, so...

		CSpecialFXList* pList = g_pGameClientShell->GetSFXMgr()->GetFXList(SFX_BODY_ID);
		if (!pList) return;

		int nNumBodies = pList->GetSize();

		uint32 dwId;
		g_pLTClient->GetLocalClientID(&dwId);

		float fNewestBodyTime = -1.0f;

		for (int i=0; i < nNumBodies; i++)
		{
			if ((*pList)[i])
			{
				CBodyFX* pBody = (CBodyFX*)(*pList)[i];

				if( (pBody->GetClientId() == dwId) && (pBody->GetTimeCreated() > fNewestBodyTime) )
				{
					// Keep the newest body belonging to this client...

					fNewestBodyTime = pBody->GetTimeCreated();
					hBody = pBody->GetServerObj();
				}
			}
		}


		if( hBody )
		{
			bool bTimesUp = (m_fMultiAttachDeathCamTimer > g_vtMultiAttachDeathCamMaxTime.GetFloat());

			// Increment our attach camera timer...Always increment this even if we're
			// not attached since we may become unattached at some point...

			m_fMultiAttachDeathCamTimer += g_pLTClient->GetFrameTime();

			// Check the body to see if the death animation has finished playing....

			uint32 dwFlags;
			g_pModelLT->GetPlaybackState( hBody, MAIN_TRACKER, dwFlags );
			if( (dwFlags & MS_PLAYDONE) || bTimesUp )
			{
				// Dawn
				/*
				// Detach from the head and float above the body...

				LTVector vFinalCamPos;
				LTRotation rAniCamRot;
				LTVector vAniCamPos;

				// Get the position of where the camera left off in the animation...
				
				GetPlayerHeadPosRot( vAniCamPos, rAniCamRot );

				// Offset the camera position from the position of the body...

				g_pLTClient->GetObjectPos( hBody, &vFinalCamPos);
				vFinalCamPos += GetPlayerHeadOffset( );

				// Detach the camera and then get the rotation from the animation rotation...

				AttachCameraToHead( LTFALSE );
				LTRotation rRot( m_fPitch, m_fYaw, m_fRoll );

				m_fMultiplayerDeathCamMoveTimer += g_pLTClient->GetFrameTime();

				float fMoveDelta = g_vtMultiplayerDeathCamMoveTime.GetFloat( 3.0f );
				float fPercent = (fMoveDelta > 0.0f ? m_fMultiplayerDeathCamMoveTimer / fMoveDelta : 0.0f);
				float fT = Clamp( fPercent, 0.0f, 1.0f );
				
				LTVector vLerped;
				VEC_LERP( vLerped, vAniCamPos, vFinalCamPos, fT );

				// Don't clip the camera into the world...
				
				m_pPlayerCamera->CalcNonClipPos( vLerped, rRot );

				g_pLTClient->SetObjectPos( m_hCamera, &vLerped );
			    m_bCameraPosInited = LTTRUE;
				*/

				return;
			}
			else
			{
				// Sometimes the death animation is multiple animations, so we'll make
				// sure to keep the camera attached if an animation is playing...

				AttachCameraToHead(LTTRUE);
			}
		}
	}

	// The player is alive, has no body, or the body is still animating.  Just use the normal position...

	UpdateCameraPosition();
	return;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::CalculateCameraRotation()
//
//	PURPOSE:	Calculate the new camera rotation
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::CalculateCameraRotation()
{
	CGameSettings* pSettings = g_pInterfaceMgr->GetSettings();
	if (!pSettings) return;

	LTBOOL bIsVehicle = m_pMoveMgr->GetVehicleMgr()->IsVehiclePhysics();

    float fVal = 1.0f + (float)(3 * m_nZoomView);

	// Get axis offsets...

	float offsets[3];
    g_pLTClient->GetAxisOffsets(offsets);

	if (m_bRestoreOrientation)
	{
		offsets[0] = offsets[1] = offsets[2] = 0.0f;
        m_bRestoreOrientation = LTFALSE;
	}

	if (m_bStrafing)
	{
		// Clear yaw and pitch offsets if we're using mouse strafing...
		offsets[0] = offsets[1] = 0.0f;
	}

    float fYawDelta    = offsets[0] / fVal;
    float fPitchDelta  = offsets[1] / fVal;

	if (g_vtAdaptiveMouse.GetFloat() && fYawDelta != 0.0f)
	{
		float fMaxOff = g_vtAdaptiveMouseMaxOffset.GetFloat();
		float fAbsYawDelta = (float)fabs(fYawDelta);
		bool bNegative = (fYawDelta < 0.0f);

		//g_pLTClient->CPrint("Initial fYawDelta = %.6f", fYawDelta);

		if (fAbsYawDelta <= fMaxOff)
		{
			fVal = fAbsYawDelta / fMaxOff;
			fYawDelta = (WaveFn_SlowOn(fVal) * fMaxOff);
			fYawDelta = bNegative ? -fYawDelta : fYawDelta;

			//g_pLTClient->CPrint("Adjusted fYawDelta = %.6f", fYawDelta);
		}
	}

	m_fYaw += fYawDelta;

	// [kml] 12/26/00 Check varying degrees of strafe and look.
	if(!(m_dwPlayerFlags & BC_CFLG_STRAFE))
	{
		if(m_dwPlayerFlags & BC_CFLG_LEFT)
		{
			m_fYaw -= g_pGameClientShell->GetFrameTime() * ((m_dwPlayerFlags & BC_CFLG_RUN) ? g_vtFastTurnRate.GetFloat() : g_vtNormalTurnRate.GetFloat());
		}
		if(m_dwPlayerFlags & BC_CFLG_RIGHT)
		{
			m_fYaw += g_pGameClientShell->GetFrameTime() * ((m_dwPlayerFlags & BC_CFLG_RUN) ? g_vtFastTurnRate.GetFloat() : g_vtNormalTurnRate.GetFloat());
		}
	}

	if (pSettings->MouseLook() || (m_dwPlayerFlags & BC_CFLG_LOOKUP) || (m_dwPlayerFlags & BC_CFLG_LOOKDOWN)
		|| m_bHoldingMouseLook)
    {
        if (pSettings->MouseLook() || m_bHoldingMouseLook)
		{
			if (pSettings->MouseInvertY())
			{
				m_fPitch -= fPitchDelta;
			}
			else
			{
				m_fPitch += fPitchDelta;
			}
		}

		if(m_dwPlayerFlags & BC_CFLG_LOOKUP)
		{
			m_fPitch -= g_pGameClientShell->GetFrameTime() * g_vtLookUpRate.GetFloat();
		}

		if(m_dwPlayerFlags & BC_CFLG_LOOKDOWN)
		{
			m_fPitch += g_pGameClientShell->GetFrameTime() * g_vtLookUpRate.GetFloat();
		}
		// Don't allow much movement up/down if 3rd person...

		if (!m_pPlayerCamera->IsFirstPerson())
		{
            float fMinY = DEG2RAD(45.0f) - 0.1f;

			if (m_fPitch < -fMinY) m_fPitch = -fMinY;
			if (m_fPitch > fMinY)  m_fPitch = fMinY;
		}
	}
	else if (m_fPitch != 0.0f && GetConsoleInt("AutoCenter",0))
	{
        float fPitchDelta = (g_pGameClientShell->GetFrameTime() * g_vtLookUpRate.GetFloat());
		if (m_fPitch > 0.0f) m_fPitch -= Min(fPitchDelta, m_fPitch);
		if (m_fPitch < 0.0f) m_fPitch += Min(fPitchDelta, -m_fPitch);
	}

    float fMinY = MATH_HALFPI - 0.1f;

	if (m_fPitch < -fMinY) m_fPitch = -fMinY;
	if (m_fPitch > fMinY)  m_fPitch = fMinY;


	// Set camera and player variables...

	// Only use mouse values for yaw if the player isn't on a vehicle...

	if (bIsVehicle)
	{
		// Can't look up/down on vehicles...

		LTVector vPlayerPYR(m_fPlayerPitch, m_fPlayerYaw, m_fPlayerRoll);
		LTVector vPYR(m_fPitch, m_fYaw, m_fRoll);

		m_pMoveMgr->GetVehicleMgr()->CalculateVehicleRotation(vPlayerPYR, vPYR, fYawDelta);

		m_fPlayerPitch	= vPlayerPYR.x;
		m_fPlayerYaw	= vPlayerPYR.y;
		m_fPlayerRoll	= vPlayerPYR.z;

		m_fPitch		= vPYR.x;
		m_fYaw			= vPYR.y;
		m_fRoll			= vPYR.z;
	}
	else  // Not vehicle...
	{
		m_fPlayerPitch	= 0.0f;
		m_fPlayerYaw	= m_fYaw;
		m_fPlayerRoll	= m_fRoll;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateCameraRotation()
//
//	PURPOSE:	Set the new camera rotation
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerMgr::UpdateCameraRotation()
{
    HLOCALOBJ hPlayerObj = g_pLTClient->GetClientObject();
    if (!hPlayerObj) return LTFALSE;

	// Update camera orientation vars...

    LTVector vPitchYawRollDelta = m_pCameraOffsetMgr->GetPitchYawRollDelta();

	if (m_bUsingExternalCamera)
	{
		// Just calculate the correct player rotation...

		m_rRotation = LTRotation(m_fPitch, m_fYaw, m_fRoll);
	}
	else if (m_pPlayerCamera->IsFirstPerson())
	{
		/*
		if( IsPlayerDead() && m_nAttachState == MID_DETACH_OBSERVER )
		{
			LTVector vPos;
			
			float fPitch = m_fPitch + vPitchYawRollDelta.x;
			float fYaw   = m_fYaw	+ vPitchYawRollDelta.y;
			float fRoll  = m_fRoll  + vPitchYawRollDelta.z;

			m_rRotation = LTRotation(fPitch, fYaw, fRoll);

			g_pLTClient->GetObjectPos(m_hCamera, &vPos);

			m_pPlayerCamera->CalcNonClipPos(vPos, m_rRotation);				
			g_pLTClient->SetObjectPos(m_hCamera, &vPos);
			g_pLTClient->SetObjectRotation(m_hCamera, &m_rRotation);
		}
		else
		{
			if (!m_bCameraAttachedToHead)		
			{
				float fPitch = m_fPitch + vPitchYawRollDelta.x;
				float fYaw   = m_fYaw	+ vPitchYawRollDelta.y;
				float fRoll  = m_fRoll  + vPitchYawRollDelta.z;

				m_rRotation = LTRotation(fPitch, fYaw, fRoll);
			}			

			g_pLTClient->SetObjectRotation(m_hCamera, &m_rRotation);
		}
		*/
		if (!m_bCameraAttachedToHead)		
		{
			float fPitch = m_fPitch + vPitchYawRollDelta.x;
			float fYaw   = m_fYaw	+ vPitchYawRollDelta.y;
			float fRoll  = m_fRoll  + vPitchYawRollDelta.z;
	
			m_rRotation = LTRotation(fPitch, fYaw, fRoll);
		}			

		g_pLTClient->SetObjectRotation(m_hCamera, &m_rRotation);
	}
	else
	{
		// 3인칭으로 Attach 되었을 때 마우스에 움직임에 따라 Yaw 값이 먹도록 수정
		// Pitch 는 삭제함.
		static float fYawDelta;
		static float fPitchDelta;
		if( m_nAttachState == MID_ATTACH_THIRD )
		{
			// 3 인칭에서 마우스 움직이며 Yaw 값 변화 되도록 만들기
			m_rRotation	= m_pPlayerCamera->GetRotation();

			LTVector vPos;
			//g_pLTClient->GetObjectPos( m_pPlayerCamera->GetAttachedObject(), &vPos);
			g_pLTClient->GetObjectPos( staticObject, &vPos);
			LTVector vPosOffset(0.0f, 50.0f, 0.0f);
			LTVector vResultPos;

			float offsets[3];
			g_pLTClient->GetAxisOffsets(offsets);

			fYawDelta    += offsets[0];
			//fPitchDelta  += offsets[1];

			//LTRotation rPitch;
			LTRotation rYaw;

			//rPitch.Init();
			rYaw.Init();

			//rPitch.Rotate( LTVector(1,0,0), fPitchDelta );
			rYaw.Rotate( LTVector(0,1,0), fYawDelta );

			m_rRotation = m_rRotation * rYaw;
			//m_rRotation = m_rRotation * rPitch;

			vResultPos = vPos + vPosOffset + (m_rRotation.Right() * 0.0f) +
								(m_rRotation.Up()    * 10.0f) +
								(m_rRotation.Forward() * -100.0f);
			
			//if( m_pPlayerCamera->GetAttachedObject() != LTNULL )
			if( staticObject != LTNULL )
			{
				LTVector vTemp;
				LTVector     dir;
				float      distToOptimal;
				ClientIntersectQuery iQuery;
				ClientIntersectInfo  iInfo;

				vTemp = (vPos + vPosOffset) - vResultPos;
				distToOptimal = vTemp.Length();
				dir = vResultPos - (vPos + vPosOffset);
				dir.Normalize();

				iQuery.m_From = vPos + vPosOffset;
				iQuery.m_To = vResultPos;
				if (m_pPlayerCamera->GetClientDE()->IntersectSegment(&iQuery, &iInfo))
				{
					vTemp = iInfo.m_Point - (vPos + vPosOffset);
					if(vTemp.Length() < distToOptimal)
					{
						vResultPos = iInfo.m_Point + iInfo.m_Plane.m_Normal;
					}
				}
				m_pPlayerCamera->CalcNonClipPos(vResultPos, m_rRotation);
				g_pLTClient->SetObjectPos(m_hCamera, &vResultPos);
				g_pLTClient->SetObjectRotation(m_hCamera, &m_rRotation);
			}
		}
		else
		{
			//[MURSUM] 죽을 때 카메라 뒤로 빼기
			if( m_bTestCamera == false )
			{
				m_rRotation	= m_pPlayerCamera->GetRotation();
				
				if( m_pPlayerCamera->GetCameraMode() == CPlayerCamera::DEAD )
				{	
					float offsets[3];
					g_pLTClient->GetAxisOffsets(offsets);

					fYawDelta    += offsets[0];
					fPitchDelta  += offsets[1];

					LTRotation rPitch;
					LTRotation rYaw;

					rPitch.Init();
					rYaw.Init();

					rPitch.Rotate( LTVector(1,0,0), fPitchDelta );
					rYaw.Rotate( LTVector(0,1,0), fYawDelta );

					m_rRotation = m_rRotation * rYaw;
					m_rRotation = m_rRotation * rPitch;
				}
				else
				{
					fYawDelta    = 0.0f;
					fPitchDelta  = 0.0f;
				}

				g_pLTClient->SetObjectRotation(m_hCamera, &m_rRotation);
			}
		}
	}

	return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::Update3rdPersonInfo
//
//	PURPOSE:	Update the 3rd person cross hair / camera info
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::Update3rdPersonInfo()
{
    HLOCALOBJ hPlayerObj = g_pLTClient->GetClientObject();
	if (!hPlayerObj || IsPlayerDead()) return;

    HOBJECT hFilterList[] = {hPlayerObj, m_pMoveMgr->GetObject(), NULL};


	ClientIntersectInfo info;
	ClientIntersectQuery query;
    LTVector vPlayerPos, vForward;

	g_pLTClient->GetObjectPos(hPlayerObj, &vPlayerPos);

    float fCrosshairDist = -1.0f;
    float fCameraOptZ = g_vtChaseCamDistBack.GetFloat();

	// Figure out crosshair distance...
	IClientWeaponBase* pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	if (g_pInterfaceMgr->IsCrosshairOn() && pClientWeapon)
	{
		WEAPON const *pWeapon = pClientWeapon->GetWeapon();
		if (!pWeapon) return;

		fCrosshairDist = (float) pWeapon->nRange;

		vForward = m_rRotation.Forward();

		// Determine where the cross hair should be...

		LTVector vStart, vEnd, vPos;
		VEC_COPY(vStart, vPlayerPos);
		VEC_MULSCALAR(vEnd, vForward, fCrosshairDist);
		VEC_ADD(vEnd, vEnd, vStart);

		VEC_COPY(query.m_From, vStart);
		VEC_COPY(query.m_To, vEnd);

		query.m_Flags = INTERSECT_OBJECTS | IGNORE_NONSOLID;
		query.m_FilterFn = ObjListFilterFn;
		query.m_pUserData = hFilterList;

		if (g_pLTClient->IntersectSegment (&query, &info))
		{
			VEC_COPY(vPos, info.m_Point);
		}
		else
		{
			VEC_COPY(vPos, vEnd);
		}

		LTVector vTemp;
		VEC_SUB(vTemp, vPos, vStart);

		fCrosshairDist = VEC_MAG(vTemp);
	}


	// Figure out optinal camera distance...

	LTRotation rRot;
	g_pLTClient->GetObjectRotation(hPlayerObj, &rRot);
	vForward = rRot.Forward();

	// Determine how far behind the player the camera can go...

	LTVector vEnd;
	vEnd = vForward * -fCameraOptZ + vPlayerPos;

	query.m_From = vPlayerPos;
	query.m_To = vEnd;

	query.m_Flags = INTERSECT_OBJECTS | IGNORE_NONSOLID;
	query.m_FilterFn = ObjListFilterFn;
	query.m_pUserData = hFilterList;

	if (g_pLTClient->IntersectSegment (&query, &info))
	{
		LTVector vTemp;
		VEC_SUB(vTemp, info.m_Point, vPlayerPos);
		float fDist = VEC_MAG(vTemp);

		fCameraOptZ = fDist < fCameraOptZ ? -(fDist - 5.0f) : -fCameraOptZ;
	}
	else
	{
		fCameraOptZ = -fCameraOptZ;
	}


	m_pPlayerCamera->SetOptZ(fCameraOptZ);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdatePlayerFlags
//
//	PURPOSE:	Update our copy of the movement flags
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdatePlayerFlags()
{
	// Update flags...

	m_dwPlayerFlags = m_pMoveMgr->GetControlFlags();

    if (g_pLTClient->IsCommandOn(COMMAND_ID_LOOKUP))
	{
		m_dwPlayerFlags |= BC_CFLG_LOOKUP;
	}

    if (g_pLTClient->IsCommandOn(COMMAND_ID_LOOKDOWN))
	{
		m_dwPlayerFlags |= BC_CFLG_LOOKDOWN;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdatePlayerInfo()
//
//	PURPOSE:	Tell the player about the new camera stuff
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdatePlayerInfo(bool bPlaying)
{
	// [NOVICE] 죽었을때는 서버쪽으로 업데이트 메세지 안보냄
	if ( TRUE == IsPlayerDead() )
	{
		return;
	}

	if (m_bAllowPlayerMovement != m_bLastAllowPlayerMovement)
	{
		g_pGameClientShell->SetInputState(!!m_bAllowPlayerMovement);
	}

	if (m_pPlayerCamera->IsChaseView() != m_bLastSent3rdPerson)
	{
		m_nPlayerInfoChangeFlags |= CLIENTUPDATE_3RDPERSON;
		m_bLastSent3rdPerson = m_pPlayerCamera->IsChaseView();

		if (m_pPlayerCamera->IsChaseView())
		{
			m_nPlayerInfoChangeFlags |= CLIENTUPDATE_3RDPERVAL;
		}
	}

	if (m_bAllowPlayerMovement != m_bLastAllowPlayerMovement)
	{
		m_nPlayerInfoChangeFlags |= CLIENTUPDATE_ALLOWINPUT;
	}

	// Always send CLIENTUPDATE_ALLOWINPUT changes guaranteed...

	if (m_nPlayerInfoChangeFlags & CLIENTUPDATE_ALLOWINPUT)
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8(MID_PLAYER_UPDATE);
		cMsg.Writeuint16(CLIENTUPDATE_ALLOWINPUT);
		cMsg.Writeuint8((uint8)m_bAllowPlayerMovement);
		g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);
		m_nPlayerInfoChangeFlags &= ~CLIENTUPDATE_ALLOWINPUT;
	}

    float fCurTime   = g_pLTClient->GetTime();

#ifndef _FINAL
	m_fSendRate  = 1.0f / GetConsoleFloat( "UpdateRate", DEFAULT_CSENDRATE );
#endif
	float fSendDelta = (fCurTime - m_fPlayerInfoLastSendTime);

	if ( m_pMoveMgr->IsInWorld() && fSendDelta > m_fSendRate )
	{
		CAutoMessage cMsg;

		cMsg.Writeuint8(MID_PLAYER_UPDATE);

		if (g_vtPlayerRotate.GetFloat(1.0) > 0.0)
		{
			if( PPM_SNOWMOBILE == m_pMoveMgr->GetVehicleMgr()->GetPhysicsModel() )
			{
				m_nPlayerInfoChangeFlags |= CLIENTUPDATE_FULLPLAYERROT;
				
				m_nPlayerInfoChangeFlags &= ~CLIENTUPDATE_ACCURATEPLAYERROT;
				m_nPlayerInfoChangeFlags &= ~CLIENTUPDATE_PLAYERROT;
			}
			else if( m_bServerAccurateRotation )
			{
				m_nPlayerInfoChangeFlags |= CLIENTUPDATE_ACCURATEPLAYERROT;
				
				m_nPlayerInfoChangeFlags &= ~CLIENTUPDATE_PLAYERROT;
				m_nPlayerInfoChangeFlags &= ~CLIENTUPDATE_FULLPLAYERROT;
			}
			else
			{
				m_nPlayerInfoChangeFlags |= CLIENTUPDATE_PLAYERROT;
				
				m_nPlayerInfoChangeFlags &= ~CLIENTUPDATE_ACCURATEPLAYERROT;
				m_nPlayerInfoChangeFlags &= ~CLIENTUPDATE_FULLPLAYERROT;
			}
		}

		/*
		if ( m_bSendCameraOffsetToServer )
		{
			m_nPlayerInfoChangeFlags |= CLIENTUPDATE_CAMERAOFFSET;
		}
		*/

        cMsg.Writeuint16(m_nPlayerInfoChangeFlags);

		if (m_nPlayerInfoChangeFlags & CLIENTUPDATE_PLAYERROT)
		{
			// Set the player's rotation (don't allow model to rotate up/down).

            LTRotation rPlayerRot(m_fPlayerPitch, m_fPlayerYaw, m_fPlayerRoll);
            cMsg.Writeuint8(CompressRotationByte(&rPlayerRot));
		}
		else if ( m_nPlayerInfoChangeFlags & CLIENTUPDATE_ACCURATEPLAYERROT )
		{

            LTRotation rPlayerRot( m_fPlayerPitch, m_fPlayerYaw, m_fPlayerRoll );
            cMsg.Writeuint16( CompressRotationShort( &rPlayerRot ) );
		}
		else if( m_nPlayerInfoChangeFlags & CLIENTUPDATE_FULLPLAYERROT )
		{
			LTRotation rPlayerRot( m_fPlayerPitch, m_fPlayerYaw, m_fPlayerRoll);
			cMsg.WriteCompLTRotation( rPlayerRot );
		}

		if ( m_nPlayerInfoChangeFlags & ( CLIENTUPDATE_PLAYERROT | CLIENTUPDATE_ACCURATEPLAYERROT ) )
		{
			//
			// pitch
			//

			// and write it to the message.
			cMsg.Writeuint8( CompressAngleToByte( m_fPitch ) );
		}

		// write the camera offset
		if ( m_nPlayerInfoChangeFlags & CLIENTUPDATE_CAMERAOFFSET )
		{
			LTRESULT ltResult;

			// Get the fire position from the camera
			LTVector vCameraPosition;

			// get the camera
			HOBJECT hCamera = g_pPlayerMgr->GetCamera();

			if ( g_pPlayerMgr->IsFirstPerson() &&
			     ( !g_pPlayerMgr->IsUsingExternalCamera() ) && 
			     ( 0 != hCamera ) )
			{
				// we're in 1st person and not using an external camera,

				// get the camera's position
				ltResult = g_pLTClient->GetObjectPos( hCamera, &vCameraPosition );
				ASSERT( LT_OK == ltResult );
			}
			else
			{
				HMODELNODE hPlayerHeadNode;
				LTransform transHeadNode;

				// external camera, figure out where the model's head
				// is and use that.  It won't be as accurate but it is
				// better than nothing.

				// get the head node
				ltResult = g_pModelLT->GetNode( g_pLTClient->GetClientObject(),
				                                // Edited By Bestline ==>
												// Time : 2004-07-08 오후 6:41:26
												// "Head_node", Edited By Bestline <==
												"Bip01 Head",
				                                hPlayerHeadNode );
				ASSERT( LT_OK == ltResult );

				// get the node's transform
				ltResult =
					g_pModelLT->GetNodeTransform( g_pLTClient->GetClientObject(),
					                              hPlayerHeadNode,
					                              transHeadNode,
					                              true );
				ASSERT( LT_OK == ltResult );

				// fake the camera position
				ltResult =
					g_pLTClient->GetTransformLT()->GetPos( transHeadNode,
					                                       vCameraPosition );
				ASSERT( LT_OK == ltResult );
			}

			// get the player's position
			LTVector vPlayerPosition;
			ltResult = g_pLTClient->GetObjectPos( g_pMoveMgr->GetObject(),
			                                      &vPlayerPosition );
			ASSERT( LT_OK == ltResult );

			// get the camera offset
			LTVector vOffset = vCameraPosition - vPlayerPosition;

			TVector3< short > vCompressedOffset;

			// compress the offset
			bool result = CompressOffset( &vCompressedOffset, vOffset, 100 );
			ASSERT( true == result );

			// write the offset to the message
			cMsg.Writeuint16( vCompressedOffset.x );

			cMsg.Writeuint16( vCompressedOffset.y );

			cMsg.Writeuint16( vCompressedOffset.z );

		}

		// Write the control flags

		if (bPlaying)
			cMsg.Writeuint32(g_pMoveMgr->GetControlFlags());
		else
			cMsg.Writeuint32(g_pMoveMgr->GetControlFlags() & BC_CFLG_DUCK);

		// Write position info...

		m_pMoveMgr->WritePositionInfo(cMsg);
		
		g_pLTClient->SendToServer(cMsg.Read(), 0);


		m_fPlayerInfoLastSendTime = fCurTime;
		m_nPlayerInfoChangeFlags  = 0;
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::IsFirstPerson()
//
//	PURPOSE:	See if we are in first person mode
//
// --------------------------------------------------------------------------- //

LTBOOL CPlayerMgr::IsFirstPerson()
{
	return m_pPlayerCamera->IsFirstPerson();
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::IsPlayerInWorld()
//
//	PURPOSE:	See if the player is in the world
//
// --------------------------------------------------------------------------- //

LTBOOL CPlayerMgr::IsPlayerInWorld()
{
    HLOCALOBJ hPlayerObj = g_pLTClient->GetClientObject();

    if (!g_pGameClientShell->IsServerLoaded( ) || !g_pGameClientShell->IsWorldLoaded() || m_ePlayerState == PS_UNKNOWN || !hPlayerObj) return LTFALSE;

    return LTTRUE;
}

void CPlayerMgr::GetCameraRotation(LTRotation &rRot)
{
	rRot = LTRotation(m_fPitch, m_fYaw, m_fRoll);
}

void CPlayerMgr::GetPlayerRotation(LTRotation &rRot)
{
	rRot = LTRotation(m_fPlayerPitch, m_fPlayerYaw, m_fPlayerRoll);
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	UpdateModelGlow
//
//	PURPOSE:	Update the current model glow color
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateModelGlow()
{
    float fColor      = 0.0f;
	float fMin		  = g_vtModelGlowMin.GetFloat();
	float fMax		  = g_vtModelGlowMax.GetFloat();
    float fColorRange = fMax - fMin;
	float fTimeRange  = g_vtModelGlowTime.GetFloat();

	if (m_bModelGlowCycleUp)
	{
		if (m_fModelGlowCycleTime < fTimeRange)
		{
			fColor = fMin + (fColorRange * (m_fModelGlowCycleTime / fTimeRange));
			m_vCurModelGlow.Init(fColor, fColor, fColor);
		}
		else
		{
			m_fModelGlowCycleTime = 0.0f;
			m_vCurModelGlow.Init(fMax, fMax, fMax);
			m_bModelGlowCycleUp = LTFALSE;
			return;
		}
	}
	else
	{
		if (m_fModelGlowCycleTime < fTimeRange)
		{
			fColor = fMax - (fColorRange * (m_fModelGlowCycleTime / fTimeRange));
			m_vCurModelGlow.Init(fColor, fColor, fColor);
		}
		else
		{
			m_fModelGlowCycleTime = 0.0f;
			m_vCurModelGlow.Init(fMin, fMin, fMin);
            m_bModelGlowCycleUp = LTTRUE;
			return;
		}
	}

	if (!g_pGameClientShell->IsServerPaused())
	{
		m_fModelGlowCycleTime += g_pGameClientShell->GetFrameTime();
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::ClearCurContainerCode
//
//	PURPOSE:	Clear our current container info.
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::ClearCurContainerCode()
{
	//make sure we clear out any tinting that the container may have been doing
	g_pGameClientShell->GetLightScaleMgr()->ClearLightScale(CLightScaleMgr::eLightScaleEnvironment);

	m_eCurContainerCode = CC_NO_CONTAINER;
	m_nSoundFilterId = 0;
}




// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::HandleWeaponDisable
//
//	PURPOSE:	Handle the weapon being disabled...
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::HandleWeaponDisable(LTBOOL bDisabled)
{
	if (bDisabled)
	{
		ClearPlayerModes(LTTRUE);
	}
	else
	{
		// Force us to re-evaluate what container we're in.  We call
		// UpdateContainers() first to make sure any container changes
		// have been accounted for, then we clear the container code
		// and force an update (this is done for underwater situations like
		// dying underwater and respawning, and also for picking up intelligence
		// items underwater)...

		if (IsPlayerInWorld())
		{
			UpdateContainers();
			ClearCurContainerCode();
			UpdateContainers(); 
		}

		m_pClientWeaponMgr->ShowWeapons();
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::ClearPlayerModes
//
//	PURPOSE:	Clear any special modes the player is in
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::ClearPlayerModes(LTBOOL bWeaponOnly)
{
	//[SPIKE]
	ReleaseZoom();
	//HandleZoomChange( LTTRUE );
	//=================================
	EndZoom();

	if (!bWeaponOnly)
	{
		EndSpyVision();
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::RestorePlayerModes
//
//	PURPOSE:	Restore any special modes the player was in 
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::RestorePlayerModes()
{
	if (m_bSpyVision)
	{
		BeginSpyVision();
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::AttachCameraToHead
//
//	PURPOSE:	Attach the camera to a socket in the player's head
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::AttachCameraToHead( bool bAttach, bool bInterpolate )
{
	// We don't want to reset our orientations if we are currently doing what was requested...

	if( m_bCameraAttachedToHead == bAttach )
		return;

	//set the new value
	m_bCameraAttachedToHead	= bAttach;
	m_bLerpAttachedCamera	= bInterpolate;

	//if we are enabling, then cache our values
	if(bAttach)
	{
		m_fModelAttachPitch = m_fPitch;
		m_fModelAttachYaw	= m_fYaw;
		m_fModelAttachRoll	= m_fRoll;
	}
	else
	{
		//we are disabling, make our animation orientation our actual orientation
	
	//[SPIKE] 죽을때 마지막으로 보고 있는위치 저장하는곳( 주석풀면 지정한 리스폰위치데로 안됨!!!)
	//	m_fPitch	= m_fModelAttachPitch;
	//	m_fYaw		= m_fModelAttachYaw;
	//	m_fRoll		= m_fModelAttachRoll;
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::GetPlayerHeadPos
//
//	PURPOSE:	Get the player's head position
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::GetPlayerHeadPosRot(LTVector & vPos, LTRotation & rRot)
{
	HMODELSOCKET hSocket = INVALID_MODEL_SOCKET;
	HOBJECT hBody = NULL; // g_pLTClient->GetClientObject();

	// We actually want to use the body prop, so...

	CSpecialFXList* pList = g_pGameClientShell->GetSFXMgr()->GetFXList(SFX_BODY_ID);
	if (!pList) return;
	
	int nNumBodies = pList->GetSize();

    uint32 dwId;
    g_pLTClient->GetLocalClientID(&dwId);

	float fNewestBodyTime = -1.0f;

	for (int i=0; i < nNumBodies; i++)
	{
		if ((*pList)[i])
		{
			CBodyFX* pBody = (CBodyFX*)(*pList)[i];

			if( (pBody->GetClientId() == dwId) && (pBody->GetTimeCreated() > fNewestBodyTime) )
			{
				// Keep the newest body belonging to this client...

				fNewestBodyTime = pBody->GetTimeCreated();
				hBody = pBody->GetServerObj();
			}
		}
	}

	if( !hBody )
	{
		// If we are alive then we don't have a body and we should use the client object...

		hBody = g_pLTClient->GetClientObject();
	}


	if (hBody)
	{
		g_pModelLT->ApplyAnimations( hBody );
			
		if (g_pModelLT->GetSocket(hBody, "Eyes", hSocket) == LT_OK)		
		{
			LTransform transform;
            if (g_pModelLT->GetSocketTransform(hBody, hSocket, transform, LTTRUE) == LT_OK)
			{
				vPos = transform.m_Pos;
				rRot = transform.m_Rot;
				m_pPlayerCamera->CalcNonClipPos(vPos, rRot);
			}
		}
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::DoActivate
//
//	PURPOSE:	Tell the server to do Activate
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::DoActivate()
{
	char const* pActivateOverride = g_vtActivateOverride.GetStr();
	if (pActivateOverride && pActivateOverride[0] != ' ')
	{
        g_pLTClient->RunConsoleString(( char* )pActivateOverride);
		return;
	}

	CActivationData data = GetTargetMgr()->GetActivationData();

	//don't revive if we are too close
	if (data.m_nType == MID_ACTIVATE_REVIVE && !GetTargetMgr()->CanActivateTarget() )
		return;

	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_PLAYER_ACTIVATE);
	data.Write(cMsg);
    g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::Teleport
//
//	PURPOSE:	Tell the server to teleport to the specified point
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::Teleport(const LTVector & vPos)
{
#ifndef _FINAL // Don't allow player teleporing in the final version...

    LTVector vPitchYawRoll(m_fPlayerPitch, m_fPlayerYaw, m_fPlayerRoll);

	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_PLAYER_TELEPORT);
	cMsg.WriteLTVector(vPos);
	cMsg.WriteLTVector(vPitchYawRoll);
    g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);

#endif // _FINAL
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateServerPlayerModel()
//
//	PURPOSE:	Puts the server's player model where our invisible one is
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdateServerPlayerModel()
{
	HOBJECT hClientObj, hRealObj;

    if (!(hClientObj = g_pLTClient->GetClientObject())) return;

	if (!(hRealObj = m_pMoveMgr->GetObject())) return;

	// 광식 : 3 인칭으로 Attach 되었을 때 Player를 Attach 된 위치로 이동.
	//--------------------------------------------------------------------------------------
	if( IsPlayerDead() && IsRevivePlayerGameType() && GetAttachState() == MID_ATTACH_THIRD )
	{
		if( GetThirdAttach() )
		{
			CPlayerInfoMgr* pMgr = g_pInterfaceMgr->GetPlayerInfoMgr();
			if( pMgr )
			{
				LTVector vPos;
				LTRotation rRot;
				uint32 nPlayer = pMgr->GetNumPlayerInGame();
				for( uint32 i = 0; i < nPlayer; i++ )
				{
					T_PLAYER_INFOptr pInfo = pMgr->GetPlayerByIndex(i);
					if( pInfo->hObject == GetThirdAttach() )
					{
						g_pLTClient->GetObjectPos(GetThirdAttach(), &vPos);
						g_pLTClient->SetObjectPos(g_pMoveMgr->GetObject(), &vPos);
						HOBJECT hLocalObj = g_pLTClient->GetClientObject();
						g_pLTClient->SetObjectPos(hLocalObj, &vPos);

						g_pLTClient->GetObjectRotation(GetThirdAttach(), &rRot);
						g_pLTClient->SetObjectRotation(g_pMoveMgr->GetObject(), &rRot);
						return;
					}
				}
			}
		}
	}
	//--------------------------------------------------------------------------------------

    LTVector myPos;
	g_pLTClient->GetObjectPos(hRealObj, &myPos);
	g_pLTClient->SetObjectPos(hClientObj, &myPos);

	if (g_vtPlayerRotate.GetFloat(1.0) > 0.0)
	{
	    LTRotation myRot(m_fPlayerPitch, m_fPlayerYaw, m_fPlayerRoll);
		g_pLTClient->SetObjectRotation(hClientObj, &myRot);
	}
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateContainers
//
//	PURPOSE:	Update anything associated with being in a container
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateContainers()
{
    LTVector vCamPos;
	g_pLTClient->GetObjectPos(m_hCamera, &vCamPos);

    LTVector vScale(1.0f, 1.0f, 1.0f), vLightAdd(0.0f, 0.0f, 0.0f);

    char* pCurSound      = NULL;
	uint8 nSoundFilterId = 0;
    uint32 dwUserFlags   = USRFLG_VISIBLE;
    m_bUseWorldFog       = LTTRUE;

	// We'll update this below...

	m_bInSafetyNet = false;

	m_bInTimeBombArea = false;

	// [KLS 4/16/02] Find container objects that we care about...

	HLOCALOBJ hContainerObj = LTNULL;
	ContainerCode eCode = CC_NO_CONTAINER;

	HLOCALOBJ objList[MAX_OVERLAPPING_CONTAINERS];
	uint32 nContainerFlags = (CC_ALL_FLAG & ~CC_PLAYER_IGNORE_FLAGS);
    uint32 dwNum = ::GetPointContainers(vCamPos, objList, MAX_OVERLAPPING_CONTAINERS, nContainerFlags);

	for (uint32 i=0; i < dwNum; i++)
	{
		uint16 code;
		if (g_pLTClient->GetContainerCode(objList[i], &code))
		{
			ContainerCode eTempCode = (ContainerCode)code;

			// Ignore dynamic occluder volumes...
			if (CC_DYNAMIC_OCCLUDER_VOLUME != eTempCode)
			{
				// [KEF 9/03/02] Allow sound filter volumes to be overridden if needed

				// [KLS 9/07/02] Modified so we look at all the containers on the list,
				// and save off saftey nets separately...

				if (CC_SAFTEY_NET == eTempCode)
				{
					// Don't count this as a normal container as it has one specific
					// purpose ONLY (protect the player)...
					m_bInSafetyNet = true;
				}
				else if ( CC_TIMEBOMB_AREA == eTempCode )  // 광식
				{
					m_bInTimeBombArea = true;
				}
				else if (CC_NO_CONTAINER == eCode || CC_FILTER == eCode)
				{
					hContainerObj = objList[i];
					eCode = eTempCode;
				}
			}
		}
	}


	// Update Dynamic occluders...

	UpdateDynamicOccluders(objList, dwNum);


	if (hContainerObj)
	{
		// Check for weather volume brush first (weather volume brushes
		// should never overlap normal volume brushes)

		CVolumeBrushFX* pFX = dynamic_cast<CVolumeBrushFX*>(g_pGameClientShell->GetSFXMgr()->FindSpecialFX(SFX_WEATHER_ID, hContainerObj));
		if (!pFX)
		{
			pFX = dynamic_cast<CVolumeBrushFX*>(g_pGameClientShell->GetSFXMgr()->FindSpecialFX(SFX_VOLUMEBRUSH_ID, hContainerObj));
		}

		pFX = UpdateVolumeBrushFX(pFX, eCode);

		// See if we have entered/left a container...

        float fTime = g_pLTClient->GetTime();
		
		if (m_eCurContainerCode != eCode)
		{
			m_fContainerStartTime = fTime;

			if (pFX)
			{
				// Set the sound filter override...

				nSoundFilterId = pFX->GetSoundFilterId();

				// See if this container has fog associated with it..

                LTBOOL bFog = pFX->IsFogEnable();

				if (bFog)
				{
                    m_bUseWorldFog = LTFALSE;

					char buf[30];
					sprintf(buf, "FogEnable %d", (int)bFog);
                    g_pLTClient->RunConsoleString(buf);

					sprintf(buf, "FogNearZ %d", (int)pFX->GetFogNearZ());
                    g_pLTClient->RunConsoleString(buf);

					sprintf(buf, "FogFarZ %d", (int)pFX->GetFogFarZ());
                    g_pLTClient->RunConsoleString(buf);

                    LTVector vFogColor = pFX->GetFogColor();

					sprintf(buf, "FogR %d", (int)vFogColor.x);
                    g_pLTClient->RunConsoleString(buf);

					sprintf(buf, "FogG %d", (int)vFogColor.y);
                    g_pLTClient->RunConsoleString(buf);

					sprintf(buf, "FogB %d", (int)vFogColor.z);
                    g_pLTClient->RunConsoleString(buf);
				}

				// Get the tint color...

				vScale = pFX->GetTintColor();
				vScale /= 255.0f;

				vLightAdd = pFX->GetLightAdd();
				vLightAdd /= 255.0f;
			}
		}

		switch (eCode)
		{
			case CC_WATER:
			case CC_CORROSIVE_FLUID:
			case CC_FREEZING_WATER:
			{
				// 광식
				//pCurSound = "Chars\\Snd\\Player\\unwater.wav";
				//pCurSound = "SA_CHARACTERS\\SND\\unwater.wav";
			}
			break;

			case CC_ENDLESS_FALL:
			{
                float fFallTime = 1.0f;

				if (fTime > m_fContainerStartTime + fFallTime)
				{
					vScale.Init(0.0f, 0.0f, 0.0f);
				}
				else
				{
                    float fScaleStart = .3f;
                    float fTimeLeft = (m_fContainerStartTime + fFallTime) - fTime;
                    float fScalePercent = fTimeLeft/fFallTime;
                    float fScale = fScaleStart * fScalePercent;

					vScale.Init(fScale, fScale, fScale);
				}
			}
			break;

			default : break;
		}

	} // if (hContainerObj)



	// See if we have entered/left a container...

	if (m_eCurContainerCode != eCode)
	{
		// Adjust world properties as necessary...
		g_pGameClientShell->ResetDynamicWorldProperties(m_bUseWorldFog);

		g_pGameClientShell->GetLightScaleMgr()->ClearLightScale(CLightScaleMgr::eLightScaleEnvironment);

		if (vScale.x != 1.0f || vScale.y != 1.0f || vScale.z != 1.0f)
		{
			g_pGameClientShell->GetLightScaleMgr()->SetLightScale(vScale, CLightScaleMgr::eLightScaleEnvironment);
		}

		// See if we are coming out of water...

		if (IsLiquid(m_eCurContainerCode) && !IsLiquid(eCode))
		{
            UpdateUnderWaterFX(LTFALSE);
		}

		m_eCurContainerCode = eCode;

		UpdateSoundFilters(nSoundFilterId);

		if (m_hContainerSound)
		{
            g_pLTClient->SoundMgr()->KillSound(m_hContainerSound);
            m_hContainerSound = NULL;
		}

		if (pCurSound)
		{
            uint32 dwFlags = PLAYSOUND_CLIENT | PLAYSOUND_LOOP | PLAYSOUND_GETHANDLE;
			m_hContainerSound = g_pClientSoundMgr->PlaySoundLocal(pCurSound, SOUNDPRIORITY_PLAYER_MEDIUM, dwFlags);
		}

		g_pGameClientShell->GetScreenTintMgr()->Set(TINT_CONTAINER,&vLightAdd);
	}


	// See if we are under water (under any liquid)...
	
	//[SPIKE]===============
	if(!IsPlayerDead() && !g_pMissionMgr->IsRoundOver() && !g_pMissionMgr->IsGameOver())
	{
		if (IsLiquid(m_eCurContainerCode))
		{
			UpdateUnderWaterFX();
			
			UpdateAirLevel();
		}
		else
		{
			//회복 
			LTFLOAT fDeltaTime = g_pLTClient->GetFrameTime();
			if(m_fAirLevel < MAX_AIR_LEVEL)
			{
				LTFLOAT fFullAirRegenTime = FULL_AIR_REGEN_TIME;
				if (fFullAirRegenTime <= 0.0f) return;
				
				LTFLOAT fDeltaAirRegen = (MAX_AIR_LEVEL/fFullAirRegenTime);
				m_fAirLevel += fDeltaTime*fDeltaAirRegen;
				
				g_pPlayerStats->UpdateAir((uint8)m_fAirLevel);
			}
			
			UpdateBreathingFX();
		}
	}
	else
	{
		if(m_fAirLevel < MAX_AIR_LEVEL)
		{
			m_fAirLevel	= MAX_AIR_LEVEL;
			g_pPlayerStats->UpdateAir((uint8)m_fAirLevel);
		}
	}
	//======================================
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateVolumeBrushFX
//
//	PURPOSE:	Deterimine if we're really in a VolumeBrush that has
//				a polygrid surface (i.e., water)
//
//	NOTES:		Passed in eCode may be updated
//
// --------------------------------------------------------------------------- //

CVolumeBrushFX* CPlayerMgr::UpdateVolumeBrushFX(CVolumeBrushFX* pFX, 
												ContainerCode & eCode)
{
	if (!pFX) return LTNULL;

	// Get the PolyGridFX associated with the VolumeBrush...

	// Get a list of all the poly grids...

	CSpecialFXList* pList = g_pGameClientShell->GetSFXMgr()->GetFXList( SFX_POLYGRID_ID );
	if( pList )
	{
		int nNumPGrids = pList->GetSize();
		HOBJECT hObj = pFX->GetServerObj();

		// Try and find a polygrid that is the surface of our container...
		
		for( int i = 0; i < nNumPGrids; ++i )
		{
			if( (*pList)[i] )
			{
				CPolyGridFX* pPGrid= (CPolyGridFX*)(*pList)[i];

				if( pPGrid->GetVolumeBrush() == hObj )
				{
					float fDisplacement;
					LTVector vIntersection;

					LTRotation rRot;
					g_pLTClient->GetObjectRotation( m_hCamera, &rRot );

					LTVector vCamPos;
					g_pLTClient->GetObjectPos(m_hCamera, &vCamPos);

					// Develop a position out and down from the camera to test from...

					LTVector vPos = vCamPos + (rRot.Forward() * 5.0f);
					vPos.y -= 2.0f;
					
					// See if we interected the polygrid...

					if( pPGrid->GetOrientedIntersectionHeight( vPos, LTVector(0,1,0), fDisplacement, vIntersection ))
					{
						vIntersection.y += fDisplacement;

						// If we are above it clear our container and fx...
						
						if( vPos.y > vIntersection.y )
						{
							eCode = CC_NO_CONTAINER;
							pFX = LTNULL;
						}
					}

					break;
				}
			}
		}
	}

	return pFX;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateSoundFilters
//
//	PURPOSE:	Update sound filters
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateSoundFilters(uint8 nSoundFilterId)
{
#ifndef USE_EAX20_HARDWARE_FILTERS
	return;
#endif // USE_EAX20_HARDWARE_FILTERS

	if (g_vtUseSoundFilters.GetFloat())
	{
		if ( m_nSoundFilterId != nSoundFilterId )
		{
			m_nSoundFilterId = nSoundFilterId;

			bool bFilterOK = true;

			ILTClientSoundMgr *pSoundMgr = (ILTClientSoundMgr *)g_pLTClient->SoundMgr();
			SOUNDFILTER* pFilter = g_pSoundFilterMgr->GetFilter( nSoundFilterId );

			// tell the sound engine about new filter, if a dynamic filter, 
			// use the global sound filter
			bool bUsingDynamic = false;
			if ( g_pSoundFilterMgr->IsDynamic( pFilter ) )
			{
				bUsingDynamic = true;
				m_nSoundFilterId = m_nGlobalSoundFilterId;
			   	pFilter = g_pSoundFilterMgr->GetFilter( m_nGlobalSoundFilterId );
			}

			if ( !g_pSoundFilterMgr->IsUnFiltered( pFilter ) )
			{
				if ( pSoundMgr->SetSoundFilter( pFilter->szFilterName ) == LT_OK )
				{
					for (int i=0; i < pFilter->nNumVars; i++)
					{
						if ( pSoundMgr->SetSoundFilterParam(pFilter->szVars[i], pFilter->fValues[i]) != LT_OK )
							bFilterOK = false;
					}
				}
				else
				{
					bFilterOK = false;
				}
			}
			else
			{
				bFilterOK = false;
			}

			pSoundMgr->EnableSoundFilter( bFilterOK );

#ifndef _FINAL
			if (g_vtShowSoundFilterInfo.GetFloat())
			{
				g_pLTClient->CPrint("Entering %s sound filter '%s' (%s)", 
					((bUsingDynamic && m_eCurContainerCode == CC_NO_CONTAINER)? "(Global)" : ""), pFilter->szName, 
					(bFilterOK ? "Enabled" : "Disabled"));

				// Display detailed filter info if necessary...
				if (g_vtShowSoundFilterInfo.GetFloat() > 1 && pFilter)
				{
					g_pLTClient->CPrint("  FilterName: '%s'", pFilter->szFilterName);
					for (int i=0; i < pFilter->nNumVars; i++)
					{
						g_pLTClient->CPrint("  '%s' = '%f'", pFilter->szVars[i], pFilter->fValues[i]);
					}
				}
			}
#endif // _FINAL
		
		}
	}

}




// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateUnderWaterFX
//
//	PURPOSE:	Update under water fx
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateUnderWaterFX(LTBOOL bUpdate)
{
	if (m_nZoomView) return;

    uint32 dwWidth = 640, dwHeight = 480;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &dwWidth, &dwHeight);

	if (dwWidth < 0 || dwHeight < 0) return;


	// Initialize to default fov x and y...

    float fFovX = g_vtFOVXNormal.GetFloat();
    float fFovY = g_vtFOVYNormal.GetFloat();

	//see if we are supposed to modify the FOV. Note that we can't modify it while any damage
	//effects are active since they can modify the FOV as well which will cause cumulative issues
	//and ultimately lead to a fairly messed up camera state until it is restored
	if (bUpdate && (g_pDamageFXMgr->GetFirstActiveFX() == NULL))
	{
        g_pLTClient->GetCameraFOV(m_hCamera, &fFovX, &fFovY);

		fFovX = RAD2DEG(fFovX);
		fFovY = RAD2DEG(fFovY);

        float fSpeed = g_vtUWFOVRate.GetFloat() * g_pGameClientShell->GetFrameTime();

		if (m_fFovXFXDir > 0)
		{
			fFovX -= fSpeed;
			fFovY += fSpeed;

			if (fFovY > g_vtFOVYMaxUW.GetFloat())
			{
				fFovY = g_vtFOVYMaxUW.GetFloat();
				m_fFovXFXDir = -m_fFovXFXDir;
			}
		}
		else
		{
			fFovX += fSpeed;
			fFovY -= fSpeed;

			if (fFovY < g_vtFOVYMinUW.GetFloat())
			{
				fFovY = g_vtFOVYMinUW.GetFloat();
				m_fFovXFXDir = -m_fFovXFXDir;
			}
		}
	}

	SetCameraFOV(DEG2RAD(fFovX), DEG2RAD(fFovY));
}

//[SPIKE]
VOID CPlayerMgr::UpdateAirLevel()
{
	LTFLOAT fDeltaTime = g_pLTClient->GetFrameTime();
	LTFLOAT fTimer	   = g_pLTClient->GetTime();

	// See if we are in a liquid...

	LTFLOAT fFullAirLostTime = FULL_AIR_LOSS_TIME;
    if (fFullAirLostTime <= 0.0f) return;

	LTFLOAT fDeltaAirLoss	 = (MAX_AIR_LEVEL/fFullAirLostTime);

	m_fAirLevel -= fDeltaTime*fDeltaAirLoss;
	if(m_fAirLevel < 0.0f) m_fAirLevel = 0.0f;

	g_pPlayerStats->UpdateAir((uint8)m_fAirLevel);

	if (m_fAirLevel == 0.0f && (fTimer > m_fLastAirLossDamageTime+AIR_LOSS_DAMAGE_TIME))
	{
		CAutoMessage cMsg;
		cMsg.Writeuint8(MID_PLAYER_CLIENTMSG);
		cMsg.Writeuint8(CP_DAMAGE);
		cMsg.Writeuint8(DT_CHOKE);
		cMsg.Writefloat(AIR_LOSS_DAMAGE);
		// Ground caused damage...
		cMsg.WriteLTVector(LTVector(0.0f, 0.0f, 0.0f));
		cMsg.Writeuint8(0);
		cMsg.WriteObject(g_pLTClient->GetClientObject());
		g_pLTClient->SendToServer(cMsg.Read(), MESSAGE_GUARANTEED);			

		LTVector vCamPos;
		LTRotation rCamRot;
		g_pLTClient->GetObjectPos(m_hCamera, &vCamPos);
		g_pLTClient->GetObjectRotation(m_hCamera, &rCamRot);
		CLIENTFX_CREATESTRUCT	fxCS("SA_WaterBall", LTNULL, vCamPos );
		fxCS.m_rRot	 = LTRotation( rCamRot.Forward(), rCamRot.Up() );
		g_pClientFXMgr->CreateClientFX(LTNULL, fxCS, LTTRUE );
		m_fLastAirLossDamageTime = fTimer;
	}
	else if(m_fAirLevel > 0.0f)
	{
		m_fLastAirLossDamageTime = fTimer;
	}
}

//---------------------------
VOID CPlayerMgr::SetKillerObject(HOBJECT hKiller, LTBOOL bExistKiller)
{
	
	LTVector vTemp;
	g_pLTClient->GetObjectPos(hKiller, &vTemp);
	GetPlayerCamera()->SetKillerPos( vTemp, bExistKiller );
	
}
//========================================================================================================================


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateBreathingFX
//
//	PURPOSE:	Update breathing fx
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateBreathingFX(LTBOOL bUpdate)
{
	//if (m_nZoomView) return;

}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateDynamicOccluders
//
//	PURPOSE:	Update dynamic occluders 
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::UpdateDynamicOccluders(HLOCALOBJ* pContainerArray, uint32 nNumContainers)
{
	CDynamicOccluderVolumeFX* enabledOccluderFX[50];
	CDynamicOccluderVolumeFX* disabledOccluderFX[50];
	uint32 nNumEnabledOccluderFX = 0;
	uint32 nNumDisabledOccluderFX = 0;

	// Find the occluder volumes fx that we wish to enable...

	for (uint32 i=0; i < nNumContainers; i++)
	{
		// We only care about "visible" containers...

		uint32 dwUserFlags = USRFLG_VISIBLE;
		g_pCommonLT->GetObjectFlags(pContainerArray[i], OFT_User, dwUserFlags);
	
		if (dwUserFlags & USRFLG_VISIBLE)
		{
			uint16 code;
			if (g_pLTClient->GetContainerCode(pContainerArray[i], &code))
			{
				ContainerCode eTempCode = (ContainerCode)code;

				if (CC_DYNAMIC_OCCLUDER_VOLUME == eTempCode)
				{
					enabledOccluderFX[nNumEnabledOccluderFX] = (CDynamicOccluderVolumeFX*)g_pGameClientShell->GetSFXMgr()->FindSpecialFX(SFX_DYNAMIC_OCCLUDER_ID, pContainerArray[i]);
					nNumEnabledOccluderFX++;
				}
			}
		}
	}

	// Find the occluder volume FX that we wish to disable...

	CSpecialFXList* pDynOccluderList = g_pGameClientShell->GetSFXMgr()->GetFXList(SFX_DYNAMIC_OCCLUDER_ID);
	if (pDynOccluderList)
	{
		uint32 nNum = pDynOccluderList->GetSize();

		for (uint32 i=0; i < nNum; i++)
		{
			CDynamicOccluderVolumeFX* pFX = (CDynamicOccluderVolumeFX*)(*pDynOccluderList)[i];
			if (!pFX) continue;

			// Check to see if this fx is on the enabled list...

			bool bEnabled = false;
			for (uint32 j=0; j < nNumEnabledOccluderFX; j++)
			{
				if (pFX == enabledOccluderFX[j])
				{
					bEnabled = true;
					break;
				}
			}

			// Add the fx to the disabled list...

			if (!bEnabled)
			{
				disabledOccluderFX[nNumDisabledOccluderFX] = pFX;
				nNumDisabledOccluderFX++;
			}
		}
	}


	// Disable the necessary occluder volumes...

	for (i=0; i < nNumDisabledOccluderFX; i++)
	{
		if (disabledOccluderFX[i])
		{
			(disabledOccluderFX[i])->Enable(false);
		}
	}

	// Enable the necessary occluder volumes...

	for (i=0; i < nNumEnabledOccluderFX; i++)
	{
		if (enabledOccluderFX[i])
		{
			(enabledOccluderFX[i])->Enable(true);
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::SetMouseInput()
//
//	PURPOSE:	Allows or disallows mouse input on the client
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::SetMouseInput( LTBOOL bAllowInput, LTBOOL bRestoreBackupAngles )
{
	if (bAllowInput)
	{
        m_bRestoreOrientation = LTTRUE;
		
		if( bRestoreBackupAngles )
		{
			m_fYaw   = m_fYawBackup;
			m_fPitch = m_fPitchBackup;
		}
	}
	else
	{
		m_fYawBackup = m_fYaw;
		m_fPitchBackup = m_fPitch;
	}
}

void CPlayerMgr::AllowPlayerMovement(LTBOOL bAllowPlayerMovement)
{
	m_bAllowPlayerMovement = bAllowPlayerMovement;

	SetMouseInput( bAllowPlayerMovement, LTFALSE );
}


void CPlayerMgr::ResetCamera()
{
	// Make sure the FOV is set correctly...

    uint32 dwWidth = 640;
    uint32 dwHeight = 480;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &dwWidth, &dwHeight);

    g_pLTClient->SetCameraRect(m_hCamera, LTTRUE, 0, 0, dwWidth, dwHeight);
	SetCameraFOV(DEG2RAD(g_vtFOVXNormal.GetFloat()), DEG2RAD(g_vtFOVYNormal.GetFloat()));
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::ShowPlayer()
//
//	PURPOSE:	Show/Hide the player object
//
// --------------------------------------------------------------------------- //

void CPlayerMgr::ShowPlayer(LTBOOL bShow)
{
	//광식
	HLOCALOBJ hPlayerObj;
	if( IsPlayerDead() && m_pPlayerCamera->GetAttachedObject() != m_pMoveMgr->GetObject() )
	{
		if( m_pPlayerCamera->GetAttachedObject() == LTNULL )
		{
			hPlayerObj = staticObject;
		}
		else
		{
			hPlayerObj = m_pPlayerCamera->GetAttachedObject();
		}		
	}
	else
	{
		hPlayerObj = g_pLTClient->GetClientObject();
	}
    //HLOCALOBJ hPlayerObj = g_pLTClient->GetClientObject();
	if (!hPlayerObj) return;

	g_pCommonLT->SetObjectFlags(hPlayerObj, OFT_Flags, (bShow ? FLAG_VISIBLE : 0), FLAG_VISIBLE);
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdatePlayer()
//
//	PURPOSE:	Update the player
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdatePlayer()
{
	HLOCALOBJ hPlayerObj;
	if( IsPlayerDead() && m_pPlayerCamera->GetAttachedObject() != m_pMoveMgr->GetObject() )
	{
		hPlayerObj = m_pPlayerCamera->GetAttachedObject();
		if( !hPlayerObj ) return;
	}
	else
	{
		hPlayerObj = g_pLTClient->GetClientObject();
		if (!hPlayerObj || IsPlayerDead()) return;
	}
    //HLOCALOBJ hPlayerObj = g_pLTClient->GetClientObject();
	//if (!hPlayerObj || IsPlayerDead()) return;


	// This is pretty much a complete kludge, but I can't really think of
	// a better way to handle this...Okay, since the server can update the
	// player's flags at any time (and override anything that we set), we'll
	// make sure that the player's flags are always what we want them to be :)

    uint32 dwPlayerFlags;
	g_pCommonLT->GetObjectFlags(hPlayerObj, OFT_Flags, dwPlayerFlags);
	if (m_pPlayerCamera->IsFirstPerson())
	{
		if (dwPlayerFlags & FLAG_VISIBLE)
		{
            ShowPlayer(LTFALSE);
		}
	}
	else  // Third person
	{
		if (!(dwPlayerFlags & FLAG_VISIBLE))
		{
			// 광식 : 죽었을 때 모델 안그리려고
			//------------------------------------
			if( !IsPlayerDead() )//&& g_bIsAttachFirst == LTTRUE )
				ShowPlayer(LTTRUE);
			//------------------------------------
            //ShowPlayer(LTTRUE);
		}
	}


	// Hide/Show our attachments...

	HideShowAttachments(hPlayerObj);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::HideShowAttachments()
//
//	PURPOSE:	Recursively hide/show attachments...
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::HideShowAttachments(HOBJECT hObj)
{
	if (!hObj) return;

	HLOCALOBJ attachList[20];
    uint32 dwListSize = 0;
    uint32 dwNumAttach = 0;

    g_pCommonLT->GetAttachments(hObj, attachList, 20, dwListSize, dwNumAttach);
	int nNum = dwNumAttach <= dwListSize ? dwNumAttach : dwListSize;

	for (int i=0; i < nNum; i++)
	{
        uint32 dwUsrFlags;
        g_pCommonLT->GetObjectFlags(attachList[i], OFT_User, dwUsrFlags);

        if (dwUsrFlags & USRFLG_ATTACH_HIDE1SHOW3)
		{
			if (m_pPlayerCamera->IsFirstPerson())
			{
				g_pCommonLT->SetObjectFlags(attachList[i], OFT_Flags, 0, FLAG_VISIBLE);
			}
			else
			{
				// 광식 : Observer Mode 때문에 수정
				bool test = (m_nAttachState == MID_DETACH_OBSERVER);
				if( IsPlayerDead() && IsRevivePlayerGameType() && m_nAttachState == MID_DETACH_OBSERVER )
				{
					g_pCommonLT->SetObjectFlags(attachList[i], OFT_Flags, 0, FLAG_VISIBLE);
				}
				else
				{
					g_pCommonLT->SetObjectFlags(attachList[i], OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);
				}
				//g_pCommonLT->SetObjectFlags(attachList[i], OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);
			}
		}
		else if (dwUsrFlags & USRFLG_ATTACH_HIDE1)
		{
			if (m_pPlayerCamera->IsFirstPerson())
			{
				g_pCommonLT->SetObjectFlags(attachList[i], OFT_Flags, 0, FLAG_VISIBLE);
			}
		}

		if (g_pVersionMgr->IsLowViolence() && dwUsrFlags & USRFLG_ATTACH_HIDEGORE)
		{
			g_pCommonLT->SetObjectFlags(attachList[i], OFT_Flags, 0, FLAG_VISIBLE);
		}

		// Hide/Show this attachment's attachments...
		HideShowAttachments(attachList[i]);
	}
}

void CPlayerMgr::HideShowAttachmentsAfterDeath(HOBJECT hObj)
{
	if (!hObj) return;

	HLOCALOBJ attachList[20];
    uint32 dwListSize = 0;
    uint32 dwNumAttach = 0;

    g_pCommonLT->GetAttachments(hObj, attachList, 20, dwListSize, dwNumAttach);
	int nNum = dwNumAttach <= dwListSize ? dwNumAttach : dwListSize;

	for (int i=0; i < nNum; i++)
	{
		g_pCommonLT->SetObjectFlags(attachList[i], OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);

		HideShowAttachments(attachList[i]);
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	SVModelHook
//
//	PURPOSE:	Special Rendering Code for SpyVision Powerup
//
// --------------------------------------------------------------------------- //
/*
void SVModelHook(ModelHookData *pData, void *pUser)
{

	CGameClientShell* pShell = (CGameClientShell*) pUser;
	if (!pShell) return;

    uint32 nUserFlags = 0;
    g_pCommonLT->GetObjectFlags(pData->m_hObject, OFT_User, nUserFlags);
	if (nUserFlags & USRFLG_SPY_VISION)
	{
		// Do the poor-man's glow if necessary...
		bool bCanDoGlow = true;  // NEED TO CALL API FUNCTION FOR THIS!!!!

		if (!bCanDoGlow)
		{
			pData->m_HookFlags &= ~MHF_USETEXTURE;
			pData->m_LightAdd = g_vSVModelColor;
		}

		// pData->m_ObjectColor = g_vSVModelColor;
		// pData->m_ObjectFlags |= FLAG_NOLIGHT;
	}
	else
	{
		DefaultModelHook(pData, pUser);
	}
}
*/

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::IsSearching()
//
//	PURPOSE:	Return whether or not the searcher object is searching
//
// ----------------------------------------------------------------------- //

bool CPlayerMgr::IsSearching()
{ 
	return m_pSearcher->IsSearching();
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::IsDisabling()
//
//	PURPOSE:	Return whether or not the player is currently disabling a gadget target
//
// ----------------------------------------------------------------------- //

bool CPlayerMgr::IsDisabling()
{ 
	return !!(m_pGadgetDisabler->IsDisabling());
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::SetCarryingObject()
//
//	PURPOSE:	Set the player to carry an object...
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::SetCarryingObject( uint8 nCarry, bool bUpdateCameraDip )
{	
	if( m_nCarryingObject != nCarry )
	{
		m_nCarryingObject = nCarry;

		if(nCarry == CFX_CARRY_BODY || nCarry == CFX_CARRY_DD_CORE)
		{
			if( bUpdateCameraDip )
				m_bCameraDip = true;

			m_pMoveMgr->SetDuckLock(LTFALSE);
			m_pClientWeaponMgr->DisableWeapons();
		}
		else
		{
			m_pClientWeaponMgr->EnableWeapons();
		}
	}

	g_pHUDMgr->QueueUpdate(kHUDCarry);
}

//pulled target mgr initialization into a function so that it maybe overridden by 
// a game specific TargetMgr
void CPlayerMgr::InitTargetMgr()
{
	m_pTargetMgr = debug_new( CTargetMgr );				 
	ASSERT( 0 != m_pTargetMgr );
}


void CPlayerMgr::ClearDamageSectors()
{
	memset(m_fDamage,0,sizeof(m_fDamage));
}

void CPlayerMgr::UpdateDamage()
{
	if (g_pGameClientShell->IsGamePaused()) return;

	float fDelta = g_pGameClientShell->GetFrameTime() * g_vtDamageFadeRate.GetFloat();
	for (int i = 0; i < kNumDamageSectors; i++)
	{
		if (m_fDamage[i] > fDelta)
			m_fDamage[i] -= fDelta;
		else
			m_fDamage[i] = 0.0f;
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UpdateDistanceIndicator()
//
//	PURPOSE:	If we are close to a distance indicator object update the huditem...
//
// ----------------------------------------------------------------------- //

void CPlayerMgr::UpdateDistanceIndicator()
{
	float fLastDistPercent = m_fDistanceIndicatorPercent;
	m_fDistanceIndicatorPercent = -1.0f;

	// Currently the only distance indicator objects are triggers...

	CSpecialFXList* pTriggerList = g_pGameClientShell->GetSFXMgr()->GetFXList( SFX_TRIGGER_ID );
	if( pTriggerList )
	{
		uint32 nNum = pTriggerList->GetNumItems();

		for( uint32 i = 0; i < nNum; ++i )
		{
			CTriggerFX* pFX = (CTriggerFX*)(*pTriggerList)[i];
			if( !pFX || !pFX->WithinIndicatorRadius() ) continue;

			// Just grab the first distance being tracked.  The distance indicators
			// shouldn't overlap.  If they do it's an LD issue or this needs to change :)

			m_fDistanceIndicatorPercent	= pFX->GetDistancePercentage();
			m_hDistanceIndicatorIcon	= pFX->GetIcon();
			break;
		}
	}

	if( (m_fDistanceIndicatorPercent != fLastDistPercent) && m_hDistanceIndicatorIcon )
		g_pHUDMgr->QueueUpdate( kHUDDistance );
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::CanCarryObject()
//
//	PURPOSE:	Check to see if we are able to carry an object or not....
//
// ----------------------------------------------------------------------- //

uint8 CPlayerMgr::CanCarryObject()
{
	if( !GetCarryingObject() && GetTargetMgr()->IsTargetInRange() && GetTargetMgr()->IsMoveTarget() && !m_pMoveMgr->IsDucking() )
	{				
		// Check if the target object is a body...

		CBodyFX* pBody = g_pGameClientShell->GetSFXMgr()->GetBodyFX(GetTargetMgr()->GetTargetObject());
		if( pBody )
		{
			// If the body can be revived then we can't carry it...
			
			if (pBody->CanBeRevived())
				return CFX_CARRY_NONE;
			else
				return CFX_CARRY_BODY;
		}
		CCharacterFX* pChar = g_pGameClientShell->GetSFXMgr()->GetCharacterFX(GetTargetMgr()->GetTargetObject());
		if( pChar )
		{
			return CFX_CARRY_BODY;
		}


		CDoomsdayPieceFX* const pFX = (CDoomsdayPieceFX*)g_pGameClientShell->GetSFXMgr()->FindSpecialFX(SFX_DOOMSDAYPIECE_ID, GetTargetMgr()->GetTargetObject());
		if( pFX )
		{
			switch (pFX->GetType())
			{
			case kDoomsDay_transmitter:
				return (CFX_CARRY_DD_TRAN);
				break;
			case kDoomsDay_battery:
				return (CFX_CARRY_DD_BATT);
				break;
			case kDoomsDay_Core:
				return (CFX_CARRY_DD_CORE);
				break;
			}
		}

	}

	return CFX_CARRY_NONE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerMgr::UseGadget()
//
//	PURPOSE:	Try to switch to a gadget that will use the specified damage type...
//
// ----------------------------------------------------------------------- //

bool CPlayerMgr::UseGadget(DamageType eDamageType)
{
	if( IsCarryingHeavyObject() )
		return false;

	// if it's an invalid type we are either not pointing at a gadget target, or it doesn't require a gadget
	if (eDamageType == DT_INVALID)
	{
		//if we had been using a weapon, switch back to it
		if (m_nPreGadgetWeapon != WMGR_INVALID_ID && m_pClientWeaponMgr->CanChangeToWeapon(m_nPreGadgetWeapon))
		{
			m_bChangingToGadget = false;
			ChangeWeapon(m_nPreGadgetWeapon);
			return true;
		}
		return false;
	}

	uint8 nId = GetGadgetFromDamageType(eDamageType);
	IClientWeaponBase *pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	uint8 nCurWeapon = WMGR_INVALID_ID;
	if ( pClientWeapon )
		nCurWeapon = pClientWeapon->GetWeaponId();

	//if we found a weapon to use, and we aren't already using it
	if (nId != WMGR_INVALID_ID && nCurWeapon != nId)
	{
		//found a match
		m_bChangingToGadget = true;
		ChangeWeapon(nId);

		//switching to a gadget, so remember what we had
		m_nPreGadgetWeapon = nCurWeapon;

		return true;
	}
	return false;

}



uint8 CPlayerMgr::GetGadgetFromDamageType(DamageType eDamageType)
{
	IClientWeaponBase *pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	uint8 nCurWeapon = WMGR_INVALID_ID;
	if ( pClientWeapon )
	{
		// Check if we have ammo, we may have just run out...
		if (pClientWeapon->HasAmmo())
		{
			nCurWeapon = pClientWeapon->GetWeaponId();
		}
		else
		{
			// Start with our next weapon that has ammo...

			nCurWeapon = m_pClientWeaponMgr->GetNextWeaponId(nCurWeapon, 0);
		}
	}
	
	uint8 nLoopCount=0;
	uint8 nTestWeapon = nCurWeapon;

	do
	{
		const WEAPON* pWpnData = g_pWeaponMgr->GetWeapon(nTestWeapon);
		if (pWpnData)
		{
			const AMMO* pAmmoData = g_pWeaponMgr->GetAmmo(pWpnData->nDefaultAmmoId);
			if (pAmmoData->eInstDamageType == eDamageType)
			{
				return nTestWeapon;
			}
		}
		nTestWeapon = m_pClientWeaponMgr->GetNextWeaponId(nTestWeapon, 0);
	} while ( ++nLoopCount<20 && nTestWeapon != nCurWeapon && nTestWeapon != WMGR_INVALID_ID);

	return WMGR_INVALID_ID;

}


bool CPlayerMgr::FireOnActivate()
{
	if (!GetTargetMgr()->IsTargetGadgetActivatable()) return false;
	if (GetGadgetDisabler()->DisableOnActivate()) return false;

	DamageType eDT = GetTargetMgr()->RequiredGadgetDamageType();

	IClientWeaponBase *pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	uint8 nCurWeapon = WMGR_INVALID_ID;
	if ( pClientWeapon )
		nCurWeapon = pClientWeapon->GetWeaponId();

	const WEAPON* pWpnData = g_pWeaponMgr->GetWeapon(nCurWeapon);
	if (pWpnData)
	{
		const AMMO* pAmmoData = g_pWeaponMgr->GetAmmo(pWpnData->nDefaultAmmoId);
		if (pAmmoData->eInstDamageType == eDT)
		{
			return true;
		}
	}

	return false;
}

bool CPlayerMgr::ClientWeaponZoomIn()
{
	if ( !m_pClientWeaponMgr->WeaponsEnabled() ) return false;

	IClientWeaponBase* pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	if( !pClientWeapon ) return false;

	uint8 nScopeId = WMGR_INVALID_ID;
	uint8 nHalfScopeId = WMGR_INVALID_ID;
	if( IsPlayerDead() && IsRevivePlayerGameType() )
	{
		int nPriority	= -1;
		int m = pClientWeapon->GetWeapon()->nNumModIds;
		for( int i = 0; i < m; i++)
		{
			MOD const *pMod = g_pWeaponMgr->GetMod(pClientWeapon->GetWeapon()->aModIds[i]);
			if (pMod && pMod->eType == SCOPE)
			{
				if (pMod->nPriority > nPriority)
				{
					nPriority = pMod->nPriority;
					nScopeId = pClientWeapon->GetWeapon()->aModIds[i];
				}
			}
			else if( pMod && pMod->eType == HALF_SCOPE )
			{
				if (pMod->nPriority > nPriority)
				{
					nPriority = pMod->nPriority;
					nHalfScopeId = pClientWeapon->GetWeapon()->aModIds[i];
				}
			}
		}
	}
	else
	{
		nScopeId = g_pPlayerStats->GetScope();
		//[SPIKE]Half_Scope
		nHalfScopeId = g_pPlayerStats->GetHalf_Scope();
	}
	//uint8 nScopeId = g_pPlayerStats->GetScope();
	//[SPIKE]Half_Scope
	//uint8 nHalfScopeId = g_pPlayerStats->GetHalf_Scope();
	if (nScopeId == WMGR_INVALID_ID && nHalfScopeId == WMGR_INVALID_ID) return false;
	
	MOD const * pMod;
	if(nScopeId != WMGR_INVALID_ID)
		pMod = g_pWeaponMgr->GetMod(nScopeId);
	else
		pMod = g_pWeaponMgr->GetMod(nHalfScopeId);
	if (!pMod) return false;

	int nZoomLevel	  = pMod->nZoomLevel;
	int nMaxZoomLevel = pMod->nMaxZoomLevel;
	
	WeaponState pCurrentState =  pClientWeapon->GetState();
	
	if( ((pCurrentState != W_FIRING && pCurrentState != W_RELOADING)) || pClientWeapon->IsCurPostFireState() &&
		((!IsPlayerDead() && !g_pMissionMgr->IsRoundOver() && !g_pMissionMgr->IsGameOver()) || m_nAttachState == MID_ATTACH_FIRST) )
	{
		if(!m_bZooming && nZoomLevel > 0)
		{
			if (m_nZoomView == 0 && nScopeId != WMGR_INVALID_ID)
			{
				BeginZoom();		// Scope 생성
			}
			
			m_nZoomView += nZoomLevel;
			m_bZooming   = LTTRUE;
			m_bZoomingIn = LTTRUE;
			g_pInterfaceMgr->BeginZoom(LTTRUE);
			HandleZoomChange();
			g_bHalfZoomMode = LTTRUE;  // Reload시 한번만 초기화 하기 위해 
			if (m_nZoomView > nMaxZoomLevel)
			{
				m_nZoomView = 0;
				m_bZooming   = LTTRUE;
				m_bZoomingIn = LTFALSE;
				m_fZoomIn_Increase	= 50.0f;
				m_fZoomIn2_Increase = 20.0f;
				g_pInterfaceMgr->BeginZoom(LTFALSE);
				HandleZoomChange();
				g_bHalfZoomMode = LTFALSE;
				g_fPVModelFOVStep = 0.0f;

				if(nScopeId == WMGR_INVALID_ID)
				{
					WriteConsoleFloat("PVModelFOV", pClientWeapon->GetWeapon()->m_fPVModelFOVY);
				}
			}
		}
	}

	return true;
}

LTBOOL CPlayerMgr::WeaponThrow()
{
	// 무기 버리기..
	//if ( m_ePlayerState == PS_ALIVE &&
	//	 GetCurrentClientWeapon()->GetState() != W_SELECT &&
	//	 GetCurrentClientWeapon()->GetState() != W_DESELECT )
	IClientWeaponBase* pClientWeapon = m_pClientWeaponMgr->GetCurrentClientWeapon();
	if( !pClientWeapon ) return LTFALSE;

	if( m_ePlayerState == PS_ALIVE )
	{
		// 이부분 약간 ~
		uint8 nWeaponClass = g_pWeaponMgr->GetWeaponClass(pClientWeapon->GetWeaponId());

		if( nWeaponClass == 3 || nWeaponClass == 4 || nWeaponClass == 5 ) return LTFALSE;
		
		LTRotation rRot;
		LTVector vPos;
		LTVector vForward;
		HOBJECT hCamera = GetCamera();
		g_pLTClient->GetObjectRotation( hCamera, &rRot );
		g_pLTClient->GetObjectPos( hCamera, &vPos );
		vForward = rRot.Forward();
		CAutoMessage cMsg;
		cMsg.Writeuint8( MID_PROJECTILE );
		cMsg.Writeuint8( MPROJ_WEAPON_THROW );
		cMsg.Writeuint8( pClientWeapon->GetWeaponId() );
		cMsg.WriteLTVector( vForward );
		cMsg.WriteLTVector( vPos );
		cMsg.Writeuint8( nWeaponClass - 1 );
		g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );

		g_pPlayerStats->ThrowWeapon(pClientWeapon->GetWeaponId(), nWeaponClass);

		g_pWpnChooser->RemoveWeapon( nWeaponClass - 1 );

		// 다음 무기로 Change
		uint8 nNextWeapon = GetClientWeaponMgr()->GetNextWeaponId( pClientWeapon->GetWeaponId(), 0);
		WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(nNextWeapon);
		if( pWeapon )
		{
			ChangeWeapon( nNextWeapon, pWeapon->nDefaultAmmoId );
			m_pClientWeaponMgr->ClearLastWeapon();
			g_pWpnChooser->ClearLastWeapon();
		}
	}
	return LTTRUE;
}

void CPlayerMgr::PlayDamageSound( DamageType eType )
{
	if( eType == DT_BULLET || eType == DT_MELEE )
	{
		char szSound[MAX_CS_FILENAME_LEN];

		//if( GetRandom(0,2) > 1 )
		{
#ifdef BIG_FAT_MODE_TEST
			if( LTFALSE == g_GameDoc.IsBigHeadMode() )
			{
				strcpy( szSound, "SA_CHARACTERS\\SND\\MPPainSounds\\" );
			}
			else
			{
				strcpy( szSound, "SA_CHARACTERS\\SND\\FatMPPainSounds\\" );
			}
#else
			strcpy( szSound, "SA_CHARACTERS\\SND\\MPPainSounds\\" );
#endif
			char* DamageSounds[] =  { "damage01.wav", "damage02.wav" };
			strcat(szSound, DamageSounds[GetRandom(0, 1)]);

			g_pClientSoundMgr->PlaySoundLocal(szSound, SOUNDPRIORITY_PLAYER_MEDIUM );
		}

		strcpy( szSound, "SA_WEAPONS\\SND\\Impact\\Human\\" );
		char* ImpactSound[] = { "Armor1.wav", "Armor2.wav" };
		strcat(szSound, ImpactSound[GetRandom(0, 1)]);

		g_pClientSoundMgr->PlaySoundLocal(szSound, SOUNDPRIORITY_PLAYER_HIGH, 0, 90 );
	}
	else if( eType == DT_CRUSH )
	{
		char szSound[MAX_CS_FILENAME_LEN];
#ifdef BIG_FAT_MODE_TEST
		if( LTFALSE == g_GameDoc.IsBigHeadMode() )
		{
			strcpy( szSound, "SA_CHARACTERS\\SND\\MPPainSounds\\" );
		}
		else
		{
			strcpy( szSound, "SA_CHARACTERS\\SND\\FatMPPainSounds\\" );
		}
#else
		strcpy( szSound, "SA_CHARACTERS\\SND\\MPPainSounds\\" );
#endif
		char* DamageSounds[] =  { "bonebreak01.wav", "bonebreak02.wav" };
		strcat(szSound, DamageSounds[GetRandom(0, 1)]);

		g_pClientSoundMgr->PlaySoundLocal(szSound, SOUNDPRIORITY_PLAYER_HIGH, 0, 90 );
	}
	else if( eType == DT_CHOKE )
	{
#ifdef BIG_FAT_MODE_TEST
		if( LTFALSE == g_GameDoc.IsBigHeadMode() )
		{
			g_pClientSoundMgr->PlaySoundLocal("SA_CHARACTERS\\SND\\MPPainSounds\\uwdamage.wav", SOUNDPRIORITY_PLAYER_HIGH, 0, 90 );
		}
		else
		{
			g_pClientSoundMgr->PlaySoundLocal("SA_CHARACTERS\\SND\\FatMPPainSounds\\uwdamage.wav", SOUNDPRIORITY_PLAYER_HIGH, 0, 90 );
		}
#else
		g_pClientSoundMgr->PlaySoundLocal("SA_CHARACTERS\\SND\\MPPainSounds\\uwdamage.wav", SOUNDPRIORITY_PLAYER_HIGH, 0, 90 );
#endif
	}
}

void CPlayerMgr::DisconnectFromServer()
{
	SetDetachCamera(true);
	GetPlayerCamera()->AttachToObject(LTNULL);
}

HOBJECT CPlayerMgr::GetServerObject()
{
	return m_pMoveMgr->GetServerObject();
}

LTBOOL CPlayerMgr::IsAttached()
{
	if( m_pMoveMgr->GetServerObject() == m_pPlayerCamera->GetAttachedObject() ) return LTFALSE;
	return LTTRUE;
}














