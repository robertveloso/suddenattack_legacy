// ----------------------------------------------------------------------- //
//
// MODULE  : ClientWeapon.h
//
// PURPOSE : Generic client-side weapon 
//
// CREATED : 9/27/97 (was WeaponModel.h)
//
// (c) 1997-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef  _ClientWeapon_h_INCLUDED_
#define  _ClientWeapon_h_INCLUDED_

// stl vector
#include <vector>

#include "PVFXMgr.h"
#include "ClientFXMgr.h"
#include "MsgIDs.h"
#include "ClientWeaponBase.h"

// forward definitions to reduce header dependancies
struct CLIENTFX_LINK;
enum SurfaceType;

//
// Defines
//
#define WM_MAX_FIRE_ANIS            WMGR_MAX_WEAPONANI_FIRE
#define WM_MAX_ALTFIRE_ANIS         WM_MAX_FIRE_ANIS    
#define WM_MAX_IDLE_ANIS            WMGR_MAX_WEAPONANI_IDLE
#define WM_MAX_ALTIDLE_ANIS         WM_MAX_IDLE_ANIS
#define WM_MAX_PV_ATTACH_CLIENTFX   WMGR_MAX_PVCLIENTFX_ATTACHMENTS
#define MAX_ROUND	5

///
// class CClientWeapon
//
class CClientWeapon : public IClientWeaponBase
{
public:

	CClientWeapon();
	virtual       ~CClientWeapon();

	// handle messages
	virtual bool  OnMessage( uint8 messageID, ILTMessage_Read *pMsg ) { return true; }

	// callbacks from the animation system
	virtual bool  OnModelKey( HLOCALOBJ hObj, ArgList* pArgs );

	// enter/exit world functionality
	virtual void          OnEnterWorld();
	virtual void          OnExitWorld();

	// standard init/term functions
	virtual bool          Init( WEAPON const &pWeapon );
	virtual void          Term();

	// main update function
	virtual WeaponState   Update( bool bFire, FireType eFireType = FT_NORMAL_FIRE );

	// external interfaces to change the ammo
	virtual void          ChangeAmmoWithReload( uint8 nNewAmmoId, bool bForce= false );
	virtual void          ChangeAmmoImmediate( uint8 nNewAmmoId, int nAmmoAmount = -1, bool bForce= false );
	virtual void          ReloadClip( bool bPlayReload = true, int nNewAmmo=-1, bool bForce = false, bool bNotifyServer = false );
	virtual void          DecrementAmmo();

	// set the information about the camera that the weapon needs to know
	virtual void          SetCameraInfo( LTRotation const &rCamRot, LTVector const &vCamPos );

	// Update the weapon model filenames, particularly the skins.
	// This is for cases where the base player model changes and 
	// the playerview model needs to change too (i.e. sleeves
	// look different on new player model).
	virtual void          ResetWeaponFilenames();

	// to update the weapon bob
	virtual void          UpdateBob( LTFLOAT fWidth, LTFLOAT fHeight );

	// dynanic perturb
	virtual LTFLOAT       GetDynamicPerturb()  const { return (m_fFiringPerturb > m_fMovementPerturb ? m_fFiringPerturb : m_fMovementPerturb); }

	//[SPIKE]
	virtual VOID		  SetRandomFiringOn(LTBOOL bOn) {m_bRandomFiringOn = bOn;}
	virtual LTBOOL		  GetRandomFiringOn()  const {return m_bRandomFiringOn;}
	virtual VOID		  SetZoomMode(uint8 nZoomMode) {m_nZoomMode = nZoomMode;}
	virtual uint8		  GetZoomMode()  const {return m_nZoomMode;}
	virtual VOID		  SetCurPostFireState(LTBOOL bPostFire){m_bCurPostFiring = bPostFire; }
	virtual LTBOOL		  IsCurPostFireState() const {return m_bCurPostFiring;}
	

	// Do NOT call these functions directly.  If you want to
	// enable/disable or show/hide the weapons, call the functions
	// in CClientWeaponMgr.
	virtual void          SetVisible( bool bVis = true );
	virtual void          SetDisable( bool bDisable = true );
	
	// Do NOT call these directly.  They are used CClientWeaponMgr.
	virtual void          Select( bool bImmediate = false );
	virtual bool          Deselect( ClientWeaponCallBackFn cbFn, void *pData );
	virtual bool          Activate();   // Create the weapon's resources
	virtual void          Deactivate(); // Shut down the weapon (destroy resources)

	// weapon info
	virtual WEAPON const *GetWeapon()       const { return m_pWeapon; }
	virtual int           GetWeaponId()     const { return m_nWeaponId; }

	// ammo info
	virtual AMMO const   *GetAmmo()         const { return m_pAmmo; }
	virtual int           GetAmmoId()       const { return m_nAmmoId; }
	virtual int           GetAmmoInClip()   const;
	// 광식
	// [05.02.11]
	//---------------------------------------------------------------------------------
	virtual void		  SetAmmoInClip(int nAmmoCount);
	LTFLOAT				  GetCurAnimationRate();	// Animation이 진행 된 비율
	virtual void		  GetImpactSurface( HOBJECT hFiredObj, uint8 nWeaponId, LTVector const &vStartPos, LTVector &vEndPos, LTVector& vImpactNormal, SurfaceType &eType, HOBJECT &hHitObj );
	LTBOOL				  UpdateDoVectorValues(float fDampen, int fPerturb, LTFLOAT fThickness, LTVector vImpactPos, LTVector& vFrom, LTVector& vTo, LTVector& vPath);
	//---------------------------------------------------------------------------------
	void				  SetFirstAttachState();

	virtual bool          HasAmmo() const;
	virtual bool          CanUseAmmo( uint8 nAmmoId ) const;
	virtual bool          CanChangeToAmmo( uint8 nAmmoId ) const;
	virtual uint8         GetNextAvailableAmmo( uint8 nGivenAmmoId = WMGR_INVALID_ID );
	virtual bool          GetBestAvailableAmmoId( int *nAmmoId ) const;

	// state info
	virtual WeaponState   GetState()        const { return m_eState; }

	// misc functions
	virtual bool          IsMeleeWeapon() const { return ( m_pAmmo && ( DT_MELEE == m_pAmmo->eInstDamageType ) ); }
	virtual bool          IsFireButtonDown() const { return m_bFireKeyDownLastUpdate; }
	virtual bool          WeaponNeedsCrosshair() const { return true; }

	virtual void          GetShellEjectPos( LTVector *vOriginalPos );

	// used with the cheat codes to adjust the proper position of the weapon
	virtual LTVector      GetMuzzleOffset() const               
	{ 
		if (!m_pWeapon) return LTVector();
		return m_pWeapon->vMuzzlePos;		
	}

	virtual void          SetMuzzleOffset( LTVector const &v )  
	{
		if (!m_pWeapon) return;
		const_cast<WEAPON *>(m_pWeapon)->vMuzzlePos = v;
	}
	
	virtual LTVector      GetBreachOffset() const               
	{ 
		if (!m_pWeapon) return LTVector();
		return m_pWeapon->vBreachOffset;		
	}

	virtual void          SetBreachOffset( LTVector const &v )  
	{
		if (!m_pWeapon) return;
		const_cast<WEAPON *>(m_pWeapon)->vBreachOffset = v;
	}
	
	virtual LTVector      GetWeaponOffset() const               
	{ 
		if (!m_pWeapon) return LTVector();
		return m_pWeapon->vPos;		
	}
	
	virtual void          SetWeaponOffset( LTVector const &v )  
	{ 
		if (!m_pWeapon) return;
		const_cast<WEAPON *>(m_pWeapon)->vPos = v;
	}

	// This interface is only used for the camera shake.  This
	// is privledged information, and the code to shake the
	// camera should eventually be moved to this class.  Please
	// don't make use of this function, and if you do comment its
	// use here.
	virtual HLOCALOBJ     GetHandle() const { return m_hObject; }

	// This needs to be exposed for the PlayerStats.  Eventually
	// this should be moved so creation/deletion of the mods
	// are transparent to the outside system (not all games
	// need mods).
	virtual void          CreateMods();

	// load/save functionality
	virtual void          Load( ILTMessage_Read *pMsg );
	virtual void          Save( ILTMessage_Write *pMsg );

	virtual void		  ResetWeapon();

	virtual void		SetPaused( bool bPaused );
	
	virtual void		ClearFiring();

	virtual void		SetNodeLastHit( uint8 nHitNode );
	virtual void		SetObjectLastHit( HOBJECT hObj );
	virtual uint8		GetNodeLastHit();
	virtual HOBJECT		GetObjectLastHit();
	void				SetClientIDLastHit( uint8 nID );
	uint8				GetClientIDLastHit();

	void				InitFireDetectionValue();


protected:

	//
	//
	// Protected interfaces
	//
	//

	struct sImpactCollection
	{
		uint8		nClientId;
		uint8		nHitNode;			
	};

	sImpactCollection	m_Impact[MAX_ROUND];

	// Create the weapon model and 
	bool          CreateWeaponModel();
	void          RemoveWeaponModel();

	// general model create (used for weapon, mods, etc)
	HOBJECT       CreateModelObject( HOBJECT hOldObj, ObjectCreateStruct *createStruct );

	WeaponState   UpdateAmmoFromFire( bool bDecrementAmmo = true );
	virtual WeaponState   UpdateModelState( bool bFire );
	void          UpdateWeaponPosition( LTVector const &vOffset );  // offset is the additional offset

	// Dynamic perturb updating
	void          UpdateMovementPerturb();
	void          UpdateFiringPerturb(LTBOOL bFiredWeapon);

	// Update the weapon based on current state and
	// animation and determines what animation to play next.
	virtual void  UpdateFiring();

	// [KLS 5/8/02] - changed the name of this function from UpdateIdle *back* to 
	// UpdateNonFiring which is more accurate since the weapon may be reloading, 
	// selecting, deselecting, or idle...but definitiely not firing ;)
	virtual void  UpdateNonFiring(); 

	// initialize all the animations for this model
	virtual void  InitAnimations( bool bAllowSelectOverride = false );
	void          SetAmmoOverrideAnis( bool bAllowSelectOverride );

	// play model animations
	void          PlayAnimation( uint32 dwAni, bool bReset = true,
	                             float fRate = 1.0f,  bool bLooping = false);
	bool          PlaySelectAnimation();
	bool          PlayDeselectAnimation();
	bool          PlayReloadAnimation();
	bool          PlayIdleAnimation();
	virtual bool  PlayFireAnimation( bool bResetAni );

	// selet animation
	bool          IsSelectAni( uint32 dwAni ) const;
	uint32        GetSelectAni() const;

	// deselect animation
	bool          IsDeselectAni( uint32 dwAni ) const;
	uint32        GetDeselectAni() const;

	// reload animation
// Edited By Bestline ==>
// Time : 2004-07-01 오후 6:46:32
// 	bool          IsReloadAni( uint32 dwAni ) const;
//	uint32        GetReloadAni() const;
// Edited By Bestline <==

	// idle animation
	bool          IsIdleAni( uint32 dwAni ) const;
	uint32        GetIdleAni() const;
	uint32        GetSubtleIdleAni() const;
	LTFLOAT       GetNextIdleTime() const;

	// pre fire animation
	bool          IsPreFireAni( uint32 dwAni ) const;
	uint32        GetPreFireAni() const;

	// fire animation
	bool          IsFireAni( uint32 dwAni , bool bCheckNormalOnly = false) const;
	uint32        GetFireAni( FireType eFireType=FT_NORMAL_FIRE ) const;

	// post fire
	bool          IsPostFireAni( uint32 dwAni ) const;
	uint32        GetPostFireAni() const;

	// are we using alt fire animations
	bool          CanUseAltFireAnis() const;

	// get the model position/rotation info
	void          GetModelPos( LTVector *vPos ) const;
	void          GetModelRot( LTRotation *rRot ) const;

	// mod helpers
	void          UpdateMods();
	void          RemoveMods();
	void          SetVisibleMods( bool bVis = true );

	//TODO: pull these out of the base code
	// silencer mod
	void          CreateSilencer();
	void          UpdateSilencer();
	void          RemoveSilencer();

	//TODO: pull these out of the base code
	// scope mod
	void          CreateScope();
	void          UpdateScope();
	void          RemoveScope();

	// muzzle flash helper routines
	void          CreateMuzzleFlash();
	void          RemoveMuzzleFlash();
	void          StartMuzzleFlash();
	void          UpdateMuzzleFlash( WeaponState eState, LTVector const &vOffset );

	// player-view fx helpers
	void          CreatePVAttachClientFX();
	void          ShowPVAttachClientFX();
	void          HidePVAttachClientFX();
	void          RemovePVAttachClientFX();

	// weapon fire helpers
	virtual void  Fire( bool bFire );
	virtual void  HandleFireKeyDownTransition();  // up to down transition
	virtual void  HandleFireKeyUpTransition();    // down to up transition
	virtual void  HandleFireKeyDown() {}          // fire held continuously down
	virtual void  HandleFireKeyUp() {}            // fire key not pressed

	// send all relevant fire information to the server
	virtual void  SendFireMessage( bool bFire,
	                               LTFLOAT fPerturb,
	                               LTVector const &vFirePos,
	                               LTVector const &vDir,
								   LTVector const &vImpactPos,
	                               HOBJECT hObject );
	virtual void SendHitMessage();

	// add any extra firing information (primarily for derived classes)
	virtual void  AddExtraFireMessageInfo( bool bFire, ILTMessage_Write *pMsg );

	// return the "type" identifier (1st part of a fire message)
	// NOTE: currently the server handles VECTOR and PROJECTILE the same
	virtual uint8 GetFireMessageType() const { return MWEAPFIRE_VECTOR; }

	// get the last type of fire sound played
	uint8         GetLastSndFireType() const;

	// Get information to use to fire the weapon, such
	// as weapon position and the forward vector.
	bool          GetFireVectors( LTVector *vU, LTVector *vR, LTVector *vF,
	                              LTVector *vFirePos ) const;

	// set the model's state, do any special setup necessary
	// to enter particular states
	WeaponState   SetState( WeaponState eNewState );

	// clientside firing considerations
	void          ClientFire( LTVector const &vPath, LTVector const &vFirePos, LTVector *pObjectImpactPos, HOBJECT *pObjectImpact );
	void          DoProjectile( LTVector const &vPath, LTVector const &vFirePos );
	void          DoVector( LTVector const &vPath, LTVector const &vFirePos, LTVector *pObjectImpactPos, HOBJECT *pObjectImpact );
	// 광식
	//void          HandleVectorImpact( LTVector const &vPath, IntersectQuery *qInfo, IntersectInfo *iInfo, LTVector *pObjectImpactPos, HOBJECT *pObjectImpact );
	LTBOOL		  HandleVectorImpact( uint8 nPlayerId, uint8 nWeaponId, LTVector const &vPath, IntersectQuery *qInfo, IntersectInfo *iInfo, LTVector *pObjectImpactPos, HOBJECT *pObjectImpact );
	void          DoGadget( LTVector const &vPath, LTVector const &vFirePos );
	void          HandleGadgetImpact( HOBJECT hObj, LTVector vImpactPoint );

	// special cases
	void          DoSpecialFire();
	virtual void  DoSpecialEndFire();
	void          DoSpecialCreateModel();
	void          SpecialShowPieces( bool bShow = true, bool bForce = false );
	bool          SpecialOverrideFire();

	// 광식
	void          AddImpact( uint8 nPlayerId, HLOCALOBJ hObj, LTVector const &vImpactPoint,
	                         LTVector const &vNormal,  LTVector const &vPath,
	                         SurfaceType eType );
	/*
	// handle client side impact effects
	void          AddImpact( HLOCALOBJ hObj, LTVector const &vImpactPoint,
	                         LTVector const &vNormal,  LTVector const &vPath,
	                         SurfaceType eType );
							 */

	// generic routine for placing this weapon's
	// information in the create struct
	void          PopulateCreateStruct( ObjectCreateStruct *pOCS ) const;
	
	// reset data before creating the weapon's resources
	void          ResetData();

	// keyframe handlers
	virtual bool  HandleFireKey( HLOCALOBJ hObj, ArgList* pArgs );
	virtual bool  HandleFXKey( HLOCALOBJ hObj, ArgList* pArgs );

	// remove all keyframed FXEd effects that have expired
	bool          RemoveFinishedKeyframedFX();

	//
	//
	// Protected Data
	//
	//

	ClientWeaponCallBackFn  m_cbDeselect;
	void         *m_pcbData;

	HOBJECT       m_hObject;          // Handle of WeaponModel model

	HMODELSOCKET  m_hBreachSocket;    // Handle of breach socket

	// mod models
	HOBJECT       m_hSilencerModel;   // Handle of silencer mod model
	HOBJECT       m_hScopeModel;      // Handle of scope mod model

	// mod sockets
	HMODELSOCKET  m_hSilencerSocket;  // Handle of silencer mod socket
	HMODELSOCKET  m_hScopeSocket;     // Handle of scope mod socket

	// mod posessions
	bool          m_bHaveSilencer;    // Do we have a silencer mod
	bool          m_bHaveScope;       // Do we have a scope mod

	int           m_nWeaponId;
	int           m_nAmmoId;

	WEAPON const *m_pWeapon;
	AMMO const   *m_pAmmo;

	LTVector      m_vFlashPos;
	LTVector      m_vFlashOffset;
	LTFLOAT       m_fFlashStartTime;       // When did flash start

	LTFLOAT       m_fBobHeight;
	LTFLOAT       m_fBobWidth;

	LTFLOAT       m_fMovementPerturb;
	LTFLOAT		  m_fFiringPerturb;

	FireType      m_eLastFireType;    // How did we last fire
	bool          m_bCanSetLastFire;  // Can we set m_eLastFireType

	LTFLOAT       m_fNextIdleTime;
	bool          m_bFire;

	// local copy of ammo
	int           m_nAmmoInClip;
	int           m_nNewAmmoInClip;

	// What are we currently doing
	WeaponState   m_eState;

	HMODELANIM    m_nSelectAni;       // Select weapon
	HMODELANIM    m_nDeselectAni;     // Deselect weapon
	HMODELANIM    m_nReloadAni;       // Reload weapon

	HMODELANIM    m_nIdleAnis[WM_MAX_IDLE_ANIS];  // Idle animations
	HMODELANIM    m_nFireAnis[WM_MAX_FIRE_ANIS];  // Fire animations

	HMODELANIM    m_nAltSelectAni;        // Alt-Fire Select weapon
	HMODELANIM    m_nAltDeselectAni;      // Alt-Fire Deselect weapon (back to normal weapon)
	HMODELANIM    m_nAltDeselect2Ani;     // Alt-Fire Deselect weapon (to new weapon)
	HMODELANIM    m_nAltReloadAni;        // Alt-Fire Reload weapon

	HMODELANIM    m_nAltIdleAnis[ WM_MAX_ALTIDLE_ANIS ];    // Alt-Fire Idle animations
	HMODELANIM    m_nAltFireAnis[ WM_MAX_ALTFIRE_ANIS ];    // Alt-Fire animations

	HMODELANIM    m_nPreFireAni;          // Optional animation to play before the actual fire ani
	HMODELANIM    m_nPostFireAni;         // Optional animation to play after the actual fire ani

	// Added By kwangsik ==>
	// Time : 2004-07-30 오후 12:29:49
	HMODELANIM		m_nClipLastFireAni;
	HMODELANIM		m_nIdleNoAmmoAni;
	LTFLOAT			m_fNextFireEnableTime;
	// Added By kwangsik <==

	// Added By Bestline ==>
	// Time : 2004-07-01 오후 6:34:29
	// 리로드 타이밍
	uint32		  m_nReloadNum;
	bool		  m_bPostReloadAniCancle;
	
	HMODELANIM	  m_nPreReloadAni;          // Optional animation to play before the actual reload ani
	HMODELANIM	  m_nPostReloadAni;         // Optional animation to play after the actual relod ani

	HMODELANIM	  m_nReloadNoAmmoAni;

	uint32		  GetPreReloadAni() const;	
	uint32		  GetPostReloadAni() const;

	bool		  IsReloadAni( uint32 dwAni , bool bCheckNormalOnly = false) const;
	bool		  IsPreReloadAni( uint32 dwAni ) const;		
	bool		  IsPostReloadAni( uint32 dwAni ) const;
	
	uint32		  GetReloadAni() const;

	LTBOOL		  SendToServerReloadMessage(CPWeaponStatusTyps eReloadStatus);
	// Added By Bestline <==

	bool          m_bUsingAltFireAnis;        // Use the m_nAltXXX anis?
	bool          m_bFireKeyDownLastUpdate;   // Was the fire key down last update?
	
    LTFLOAT		  m_fLastPitch;	// Holds last player pitch value
    LTFLOAT		  m_fLastYaw;	// Holds last player yaw value

	uint16        m_wIgnoreFX;    // FX to ignore for current vector/projectile

	bool          m_bWeaponDeselected;    // Did we just deselect the weapon

	bool          m_bDisabled;    // Is the weapon disabled
	bool          m_bVisible;     // Is the weapon visible (should it be)

	bool		  m_bControllingFlashLight; // Does this weapon control the pv flash light?

	LTRotation    m_rCamRot;
	LTVector      m_vCamPos;

	bool          m_bFirstSelection; // Is this the first time we are selecting this weapon?

	// used to determine if we should make a tracer
	int           m_nTracerNumber;

	CLIENTFX_LINK m_PVAttachClientFX[ WM_MAX_PV_ATTACH_CLIENTFX ];  // FXEd attachments
	bool		  m_bPVAttachClientFXHidden[ WM_MAX_PV_ATTACH_CLIENTFX ]; // Was the FX visible when we tried to hide it?

	CLIENTFX_LINK m_MuzzleFlashFX;  // An instance of a FxED created FX 

	// auto switch weapons
	bool          m_bAutoSwitchEnabled;
	bool          m_bAutoSwitch;

	// keyframed ClientFX
	CLIENTFX_LINK_NODE	m_KeyframedClientFX;
	
	// Is the weapon supposed to be paused...
	bool			m_bPaused;

	//[SPIKE]
	LTBOOL			m_bRandomFiringOn;
	uint8			m_nZoomMode;  // 0:사용안함  1:줌인 2:줌아웃 
	LTBOOL			m_bResetAniToggle;
	LTBOOL			m_bSpecialAniToggle;
	LTBOOL			m_bGrenadeDeathFire;
	LTBOOL			m_bLastMainFired;
	LTBOOL			m_bKeyToggle, m_bLastKeyToggle;
	LTBOOL			m_bCurPostFiring;
	
	//[MURSUM]
	float			m_fLastPlantedBombMsg;
	float			m_fLastBombAreaMsg;
	uint8			m_nLastHitNode;
	HOBJECT			m_hLastHitObject;
	LTVector		m_vFirePos;
	uint8			m_nHitCount;
	uint8			m_nLastHitClientID;
	
#ifdef _DEBUG
	float			m_fLastFireTime;
	float			m_fFireCount;
	float			m_fAccumFireTime;
#endif

private:
	uint32			m_nCryptKey;
};


#endif //_ClientWeapon_h_INCLUDED_