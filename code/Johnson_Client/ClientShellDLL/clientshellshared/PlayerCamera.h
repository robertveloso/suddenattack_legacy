// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerCamera.cpp
//
// PURPOSE : PlayerCamera - Implementation
//
// CREATED : 10/5/97
//
// ----------------------------------------------------------------------- //

#ifndef __PLAYER_CAMERA_H__
#define __PLAYER_CAMERA_H__

#include "iltclient.h"


class CPlayerCamera
{
	public:

		enum CameraState	{ SOUTH, SOUTHEAST, EAST, NORTHEAST, NORTH,
							  NORTHWEST, WEST, SOUTHWEST, MLOOK, HOLD };

		enum CameraMode		{ FIRSTPERSON, CHASE, GOINGFIRSTPERSON,
							  CIRCLING, SCRIPT, FIRSTDYING, DYING, DEAD };

		enum CameraPoint	{ AT_POSITION, IN_DIRECTION, AUTOMATIC };

		// Constructor
		CPlayerCamera();
		~CPlayerCamera();

        LTBOOL Init(ILTClient* pClientDE);

		// Update the position & orientation of the camera based on the target
        void CameraUpdate(float deltaTime);

		// Set the state/orientation of the camera
		void SetCameraState(CameraState eOrientation);
		CameraState GetCameraState() const { return m_eOrientation; }

        void StartCircle(float HeightOffset, float Radius,
                         float PointAtOffset, float Time);

		// Attach the camera to an object
		void AttachToObject(HLOCALOBJ hObj);


		// Get the object that the camera is attached to
		HLOCALOBJ GetAttachedObject()
		{
			return m_hTarget;
		}

		/*
		// Added By Bestline ==>
		// Time : 2004-07-12 오후 7:56:35
		void DetatchToggle();

		LTBOOL IsDetatched()
		{
			return m_bIsDetatched;
		}
		// Added By Bestline <==
		*/

		// Save the camera's current orientation
		void SaveState()
		{
			m_eSaveOrientation = m_eOrientation;
		}

		// Restore the camera's last saved orientation
		void RestoreState()
		{
			SetCameraState(m_eSaveOrientation);
		}

		void GoChaseMode()
		{
			m_eCameraMode = CHASE;
		}

		void SetCameraMode (CameraMode eCMType)
		{
			m_eCameraMode = eCMType ;
		}

		CameraMode GetCameraMode() const { return m_eCameraMode; }

		void GoFirstPerson();

        LTBOOL IsChaseView()     const { return (m_eCameraMode == CHASE); }
        LTBOOL IsFirstPerson()   const { return (m_eCameraMode == FIRSTPERSON); }
        const LTRotation &GetRotation() const { return m_rRotation; }
		
		void	SetRotation( LTRotation& rRot );

        const LTVector &GetPos()        const { return m_vPos; }
		// 광식 : Observer Mode 때문에..
		//-----------------------------------------------------
		void SetCameraPos( LTVector vPos )		{ m_vPos = vPos; }
		//-----------------------------------------------------

        void SetDistUp(float f)            { m_CameraDistUp = f; }
        void SetDistBack(float f)          { m_CameraDistBack = f; }
        void SetDistDiag(float f)          { m_CameraDistDiag = f; }
        void SetPointAtOffset(LTVector v)    { VEC_COPY(m_TargetPointAtOffset, v); }
        void SetChaseOffset(LTVector v)      { VEC_COPY(m_TargetChaseOffset, v); }
        void SetFirstPersonOffset(LTVector v) { VEC_COPY(m_TargetFirstPersonOffset, v); }

        void SetOptZ(float f)   { m_OptZ = f; }

		void CalcNonClipPos( LTVector & vPos, LTRotation & rRot );
		void CalcNoClipPos_WithoutObject( LTVector & vPos, LTRotation & rRot );

		//------------------------------------------------------------------------
		// 광식
		//------------------------------------------------------------------------
		void	AttachToObjectId(uint32 nId)	{ m_nTargetId = nId; }
		uint32	GetAttachedToObjectId()			{ return m_nTargetId; }
		ILTClient* GetClientDE()				{ return m_pClientDE; }
		//------------------------------------------------------------------------

		//[SPIKE]
		VOID	SetKillerPos(LTVector vPos, LTBOOL bExistKiller){ m_vKillerPos = vPos; m_bRotateCalcStart = bExistKiller; }

		//=================

	private : // Data members
		/*
		// Added By Bestline ==>
		// Time : 2004-07-12 오후 7:59:17
		LTBOOL			m_bIsDetatched;
		HLOCALOBJ		m_hSavedTarget;		
		// Added By Bestline <==
		*/
		
		// Target object for the camera to follow
		HLOCALOBJ		m_hTarget;
		// 광식 : Target Object 의 Client ID
		uint32			m_nTargetId;

        ILTClient*		m_pClientDE;

        LTRotation	m_rRotation;
        LTVector		m_vPos;
		
		//[SPIKE]
		LTVector		m_vKillerPos, m_vKillerDir;
		LTFLOAT			m_fKillerSeePitch, m_fKillerSeeYaw, m_fYawOffset;
		LTFLOAT			m_fFallBackDist, m_fFallBackOffset;
		LTBOOL			m_bRotateCalcStart, m_bYawRotComplate;
		//=====================================

        LTVector		m_vLastTargetPos;   // The last position of our target
        LTVector		m_vLastOptPos;      // Last optimal camera pos
        LTRotation		m_rLastTargetRot;

		CameraState		m_eOrientation;
		CameraState		m_eSaveOrientation;

		CameraMode		m_eCameraMode;
		CameraMode		m_eSaveCameraMode;

		// Should the camera point at a position, in a direction, or dynamically
		CameraPoint		m_ePointType;

		// Current optimal camera positions and orientation variables
        float			m_OptX, m_OptY, m_OptZ;

		// Optimal camera distance from the target
        float			m_CameraDistBack, m_CameraDistUp, m_CameraDistDiag;

		// Does the camera slide or not
        LTBOOL			m_bSlide;

		// Offset from the target's position.
		// If a wall is between the optimal camera position and the target, the camera
		// will move closer to the target's position added to this offset. (Chase view)
        LTVector		m_TargetChaseOffset;

		// Offset from the target's position...
		// The camera will point at this potision (Chase view)
        LTVector		m_TargetPointAtOffset;

		// How high from the center of the target should the camera be during a circle
        float			m_CircleHeightOffset;
		// What is the radius of the circle
        float			m_CircleRadius;
		// How long in seconds should it take for a circle to complete
        float			m_CircleTime;

        float			m_GoingFirstPersonStart;        // start time for going first person
        float			m_GoingFirstPersonTransition;   // transition time

        LTBOOL			m_bStartCircle;
        float			m_CircleStartTime;
        float			m_SaveAnglesY;
        LTBOOL			m_bRestoreFirstPerson;

        LTVector		m_TargetFirstPersonOffset;

		HOBJECT			m_hCollisionObject;

	// Internal functions..
	private:

		// Matrix apply function.
        LTVector Apply(const LTVector &right, const LTVector &up, const LTVector &forward, const LTVector &copy);

		// Point the camera at a position from a position
        void PointAtPosition(const LTVector &pos, const LTRotation &rot, const LTVector &pointFrom);

		// Point the camera in a direction
        void PointInDirection(const LTRotation &rRot)
		{
            m_rRotation = rRot;
		}

		// Dynamically point the camera based on the distance between the camera and the target
        void PointAutomatic(LTVector pos, LTVector angles, LTVector pointFrom)
		{
		}

		// Rotate the camera clockwise or counterclockwise around the target
        void RotateCameraState(LTBOOL bClockwise);

		// Move the camera to a position over a time period
        void MoveCameraToPosition(LTVector pos, float deltaTime);

		// Find the optimal camera position
        LTVector FindOptimalCameraPosition();

		//[MURSUM]========================================
		float	FindDyingCameraPosition( float fDist, float fHeightOffset );
		void	UpdateDying();
		LTVector	m_vDyingStart;
		LTVector	m_vDyingEnd;
		LTVector	m_vDyingDir;

		float		m_fDyingVel;
		float		m_fDyingDist;
		//================================================

        void PrintVector(LTVector v);

		void CircleAroundTarget();

		void SaveCameraMode()
		{
			m_eSaveCameraMode = m_eCameraMode;
		}

		void RestoreCameraMode()
		{
			if((m_eCameraMode == CIRCLING) && (m_eSaveCameraMode == FIRSTPERSON))
				GoFirstPerson();
			else
				m_eCameraMode = m_eSaveCameraMode;
		}

        void UpdateFirstPerson(LTVector vF, LTVector vPos, float fDeltaTime);

        LTBOOL VCompare(LTVector a, LTVector b);

		bool CreateCameraCollisionObject();

		//[SPIKE] 죽인넘 바라보기 계산 
		VOID	InitSeeKiller();
		VOID	CalcSeeKiller();
};


#endif // __PLAYER_CAMERA_H__














