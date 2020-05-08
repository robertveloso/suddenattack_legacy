// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerCamera.cpp
//
// PURPOSE : PlayerCamera - Implementation
//
// CREATED : 10/5/97
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "PlayerCamera.h"
#include "iltclient.h"
#include <stdio.h>
#include "GameClientShell.h"
#include "VarTrack.h"
#include "VehicleMgr.h"
#include "CMoveMgr.h"
#include "../SA/SAHUDMgr.h"

VarTrack		g_vtCameraMoveUpTime;
VarTrack		g_vtCameraMoveUpMinVel;
VarTrack		g_vtCameraMaxYDifference;
VarTrack		g_vtCameraClipDistance;
VarTrack		g_vtCameraCollisionObjScale;
VarTrack		g_vtCameraCollisionUseObject;

//[MURSUM]================================================================
//경로수정
//#define CAMERA_COLLISION_MODEL			"models\\1x1_square.ltb"
#define CAMERA_COLLISION_MODEL			"global\\models\\1x1_square.ltb"
#define DYING_VEL	150.0f
//[SPIKE]
#define	FALLBACKDIST 250.0f
//========================================================================
#define DEFAULT_COLLISION_MODEL_SCALE	12.0f

// Camera's distance back from the player's position
#define DEFAULT_CAMERA_DIST_BACK	110
// Camera's distance up from the player's position
#define DEFAULT_CAMERA_DIST_UP		40
// Camera's distance back from the player's position but at a 45 degree angle
// (for example, if the camera is looking at the player from the southeast position)
#define DEFAULT_CAMERA_DIST_DIAG	106

// Camera's X offset from the player when it is in the MLOOK state
#define DEFAULT_CAMERA_DIST_MLOOK_X	0
// Camera's Y offset from the player when it is in the MLOOK state
#define DEFAULT_CAMERA_DIST_MLOOK_Y 10
// Camera's Z offset from the player when it is in the MLOOK state
#define DEFAULT_CAMERA_DIST_MLOOK_Z 29

extern CGameClientShell*	g_pGameClientShell;

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::CPlayerCamera()
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CPlayerCamera::CPlayerCamera()
{
    m_hTarget           = NULL;
    m_pClientDE         = NULL;

    m_rRotation.Init();
    m_rLastTargetRot.Init();
	m_vPos.Init();
	m_vLastTargetPos.Init();
	m_vLastOptPos.Init();

	m_eOrientation		= SOUTH;
	m_eSaveOrientation	= SOUTH;

	m_OptX				= 0.0f;
	m_OptY				= 0.0f;
	m_OptZ				= 0.0f;
	m_CircleStartTime	= 0.0f;
	m_SaveAnglesY		= 0.0f;

    m_bSlide            = LTFALSE;

	m_CameraDistBack	= DEFAULT_CAMERA_DIST_BACK;
	m_CameraDistUp		= DEFAULT_CAMERA_DIST_UP;
	m_CameraDistDiag	= DEFAULT_CAMERA_DIST_DIAG;

	m_ePointType			= AT_POSITION;
    m_bStartCircle          = LTFALSE;
	m_CircleHeightOffset	= 0.0f;
	m_CircleRadius			= 75.0f;
	m_CircleTime			= 3.0f;
    m_bRestoreFirstPerson   = LTFALSE;

	m_GoingFirstPersonStart	= 0.0f;
	m_GoingFirstPersonTransition = 1.5f;

	m_eCameraMode			= CHASE;
	m_eSaveCameraMode		= CHASE;

	m_TargetFirstPersonOffset.Init();
	m_TargetChaseOffset.Init();
	m_TargetPointAtOffset.Init();

	m_hCollisionObject	= LTNULL;

	// 광식
	m_nTargetId			= 0xFFFFFFFF;

	//[SPIKE]
	m_vKillerPos.Init();
	m_vKillerDir.Init();
	m_fKillerSeePitch		= 0.0f;
	m_fKillerSeeYaw			= 0.0f;
	m_fYawOffset			= 0.0f;
	
	m_bRotateCalcStart		= LTFALSE;
	m_bYawRotComplate		= LTFALSE;
	m_fFallBackDist			= 0.0f;
	m_fFallBackOffset		= 0.0f;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::~CPlayerCamera()
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CPlayerCamera::~CPlayerCamera()
{
	if( m_hCollisionObject )
	{
		g_pLTClient->RemoveObject( m_hCollisionObject );
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::Init()
//
//	PURPOSE:	Init the camera
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerCamera::Init(ILTClient* pClientDE)
{
    if (!pClientDE) return LTFALSE;
	m_pClientDE = pClientDE;

	g_vtCameraMoveUpTime.Init(pClientDE, "CameraMoveUpTime", NULL, 0.1f);
	g_vtCameraMoveUpMinVel.Init(pClientDE, "CameraMoveUpMinVel", NULL, 140.0f);
	g_vtCameraMaxYDifference.Init(pClientDE, "CameraMoveMaxYDiff", NULL, 30.0f);
	g_vtCameraClipDistance.Init(pClientDE, "CameraClipDist", NULL, 30.0f);
	g_vtCameraCollisionObjScale.Init(pClientDE, "CameraCollisionObjScale", NULL, DEFAULT_COLLISION_MODEL_SCALE );
	g_vtCameraCollisionUseObject.Init(pClientDE, "CameraCollisionUseObject", NULL, 1.0f );

	CreateCameraCollisionObject();

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::CameraUpdate()
//
//	PURPOSE:	Update the position & orientation of the camera based
//				on the target
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::CameraUpdate(float deltaTime)
{
	if (!m_pClientDE || !m_hTarget) return;
	
    LTRotation rRot;
    LTVector vOpt, vPos;

	g_pLTClient->GetObjectPos(m_hTarget, &vPos);
	g_pLTClient->GetObjectRotation(m_hTarget, &rRot);
	
	LTVector vF = rRot.Forward();
	LTVector vR = rRot.Right();

	if (m_hTarget)
	{
		if (m_eOrientation == HOLD)
			return;

		switch(m_eCameraMode)
		{
			case CHASE:
			{
			}
			break;

			case GOINGFIRSTPERSON:
			{
				vF.y = 0.0f;
				vPos += (vF * m_TargetFirstPersonOffset.x);
				vPos.y += m_TargetFirstPersonOffset.y;

				vOpt = vPos;
			}
			break;

			case CIRCLING:
			{
				CircleAroundTarget();
				return;
			}
			break;

			case FIRSTPERSON:
			{
				UpdateFirstPerson(vF, vPos, deltaTime);
				PointInDirection(rRot);
				return;
			}
			break;

			case SCRIPT:
				return;

			case FIRSTDYING:
				m_eCameraMode		= DYING;
				
				//[SPIKE] 날 죽이놈 쳐다보기 준비!
				if(m_bRotateCalcStart)
				{
					InitSeeKiller();
				}
				else // 자살 
				{
					m_fDyingDist = FindDyingCameraPosition( 400.0f, 0.0f );
					if( m_fDyingDist > 300.0f ) m_fDyingDist = 300.0f;
					
					if( m_fDyingDist < 1.0f )
					{
						m_eCameraMode	= CHASE;					
					}
					else
					{
						m_fDyingVel			= DYING_VEL;
						m_eCameraMode		= DYING;
						m_vDyingDir			= m_vDyingEnd - m_vDyingStart;
						m_vDyingDir.Normalize();					
					}
				}
				return;

			case DYING:
			case DEAD:
				UpdateDying();
				return;

			default:
				return;
		}

		// Find the optimal position for the camera
		if (m_eCameraMode == GOINGFIRSTPERSON)
		{
			MoveCameraToPosition(vOpt, deltaTime);
			PointAtPosition(vOpt, rRot, m_vPos);
			return;
		}
		else
		{
			vOpt = FindOptimalCameraPosition();
		}

		// Move the camera to the optimal position
		// (it will slide or not depending on the m_bSlide param)
		MoveCameraToPosition(vOpt, deltaTime);
		
		// Either point the camera at the player or MLOOK
		if(m_eOrientation == MLOOK)
		{
            m_rRotation = rRot;
		}
		else
		{
			switch(m_ePointType)
			{
				case IN_DIRECTION:
					PointInDirection(rRot);
				break;

				case AT_POSITION:
				default:
				{					
					LTVector vTemp;
					vTemp = vPos + m_TargetPointAtOffset;					
					PointAtPosition(vTemp, rRot, vOpt);
					// g_pLTClient->CPrint("Camera orientation : %0.4f, %0.4f, %0.4f", VEC_EXPAND(m_rRotation.Forward()));
				}
				break;
			}
		}
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::UpdateFirstPerson()
//
//	PURPOSE:	Update the position of the camera when in first person
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::UpdateFirstPerson(LTVector vF, LTVector vPos, float fDeltaTime)
{
	vF.y = 0.0f;
	vPos += (vF * m_TargetFirstPersonOffset.x);
	vPos.y += m_TargetFirstPersonOffset.y;

	//[SPIKE]임시 
/*	vR.y = 0.0f;
	vPos += (vR * 5.0f);
	vPos.y += m_TargetFirstPersonOffset.y;*/
	//=============================================


	// Modify the position so it's always based on the standing height...

	vPos.y += g_pMoveMgr->GetCurrentHeightDifference();

	// Make sure we don't move the camera up too fast (going up stairs)...

	if (vPos.y > (m_vPos.y + 1.0f))
	{
        float fNewY = m_vPos.y;
		CMoveMgr* pMoveMgr = g_pPlayerMgr->GetMoveMgr();
		if (!pMoveMgr) return;

        LTBOOL bFreeMovement = pMoveMgr->IsFreeMovement();

		if (!bFreeMovement)
		{
            float fMaxYDiff = g_vtCameraMaxYDifference.GetFloat();
            float fYDiff = (vPos.y - fNewY);

			// Force the camera to never be more than fMaxYDiff away from
			// its "real" position...

			if (fYDiff > fMaxYDiff)
			{
				fNewY  += (fYDiff - fMaxYDiff);
				fYDiff = fMaxYDiff;
			}

            float fVel = fYDiff / g_vtCameraMoveUpTime.GetFloat();
			fVel = fVel < g_vtCameraMoveUpMinVel.GetFloat() ? g_vtCameraMoveUpMinVel.GetFloat() : fVel;

			fNewY += (fDeltaTime * fVel);

			if (fabs(fNewY - vPos.y) > 1.0f)
			{
				vPos.y = fNewY > vPos.y ? vPos.y : fNewY;
			}
		}
	}

	m_vPos = vPos;	
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::Apply()
//
//	PURPOSE:	Matrix apply function
//
// ----------------------------------------------------------------------- //

LTVector CPlayerCamera::Apply(const LTVector &right, const LTVector &up, const LTVector &forward, const LTVector &copy)
{
    LTVector target;

	target.x = copy.x*right.x + copy.y*right.y + copy.z*right.z;
	target.y = copy.x*up.x + copy.y*up.y + copy.z*up.z;
	target.z = copy.x*forward.x + copy.y*forward.y + copy.z*forward.z;

	return target;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::PointAtPosition()
//
//	PURPOSE:	Point the camera at a position from a position
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::PointAtPosition(const LTVector &pos, const LTRotation &rRot, const LTVector &pointFrom)
{
	if (!m_pClientDE) return;

    LTVector vAngles;
	vAngles.Init();

	m_rRotation = rRot;

    float diffX = pos.x - m_vPos.x;
    float diffY = pos.z - m_vPos.z;
    vAngles.y = (float)atan2(diffX, diffY);

    LTVector     target, copy;

	copy = pos - pointFrom;

	target = Apply(m_rRotation.Right(), m_rRotation.Up(), m_rRotation.Forward(), copy);

	diffX = target.z;
	diffY = target.y;


    vAngles.x = (float)-atan2(diffY, diffX);

    LTRotation rOldRot;
    rOldRot = m_rRotation;

	m_rRotation = LTRotation(VEC_EXPAND(vAngles));

	// Make sure rotation is valid...

    if (m_rRotation.Up().y < 0) m_rRotation = rOldRot;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::SetCameraState()
//
//	PURPOSE:	Set the state/orientation of the camera
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::SetCameraState(CameraState eOrientation)
{
	switch(eOrientation)
	{
		case SOUTHEAST:
			m_OptX = m_CameraDistDiag;
			m_OptY = m_CameraDistUp;
			m_OptZ = -m_CameraDistDiag;
		break;

		case EAST:
			m_OptX = m_CameraDistBack;
			m_OptY = m_CameraDistUp;
			m_OptZ = 0;
		break;

		case NORTHEAST:
			m_OptX = m_CameraDistDiag;
			m_OptY = m_CameraDistUp;
			m_OptZ = m_CameraDistDiag;
		break;

		case NORTH:
			m_OptX = 0;
			m_OptY = m_CameraDistUp;
			m_OptZ = m_CameraDistBack;
		break;

		case NORTHWEST:
			m_OptX = -m_CameraDistDiag;
			m_OptY = m_CameraDistUp;
			m_OptZ = m_CameraDistDiag;
		break;

		case WEST:
			m_OptX = -m_CameraDistBack;
			m_OptY = m_CameraDistUp;
			m_OptZ = 0;
		break;

		case SOUTHWEST:
			m_OptX = -m_CameraDistDiag;
			m_OptY = m_CameraDistUp;
			m_OptZ = -m_CameraDistDiag;
		break;

		case SOUTH:
			m_OptX = 0;
			m_OptY = m_CameraDistUp;
			m_OptZ = -m_CameraDistBack;
		break;

		case MLOOK:
			m_OptX = DEFAULT_CAMERA_DIST_MLOOK_X;
			m_OptY = DEFAULT_CAMERA_DIST_MLOOK_Y;
			m_OptZ = DEFAULT_CAMERA_DIST_MLOOK_Z;
		break;

		default:
		break;
	}

	m_eOrientation = eOrientation;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::RotateCameraState()
//
//	PURPOSE:	Rotate the camera clockwise or counterclockwise around
//				the target
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::RotateCameraState(LTBOOL bClockwise)
{
	switch(m_eOrientation)
	{
		case SOUTHEAST:
			if(bClockwise)
				SetCameraState(SOUTH);
			else
				SetCameraState(EAST);
		break;

		case EAST:
			if(bClockwise)
				SetCameraState(SOUTHEAST);
			else
				SetCameraState(NORTHEAST);
		break;

		case NORTHEAST:
			if(bClockwise)
				SetCameraState(EAST);
			else
				SetCameraState(NORTH);
		break;

		case NORTH:
			if(bClockwise)
				SetCameraState(NORTHEAST);
			else
				SetCameraState(NORTHWEST);
		break;

		case NORTHWEST:
			if(bClockwise)
				SetCameraState(NORTH);
			else
				SetCameraState(WEST);
		break;

		case WEST:
			if(bClockwise)
				SetCameraState(NORTHWEST);
			else
				SetCameraState(SOUTHWEST);
		break;

		case SOUTHWEST:
			if(bClockwise)
				SetCameraState(WEST);
			else
				SetCameraState(SOUTH);
		break;

		case SOUTH:
			if(bClockwise)
				SetCameraState(SOUTHWEST);
			else
				SetCameraState(SOUTHEAST);
		break;

		default:
		break;

	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::MoveCameraToPosition()
//
//	PURPOSE:	Move the camera to a position over a time period
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::MoveCameraToPosition(LTVector pos, float deltaTime)
{
	if (!m_pClientDE || !m_hTarget) return;

	switch( m_eCameraMode )
	{
	case GOINGFIRSTPERSON:
		{
			m_eCameraMode = FIRSTPERSON;

			LTRotation rRot;
			g_pLTClient->GetObjectRotation(m_hTarget, &rRot);

			LTVector vF = rRot.Forward();

			LTVector vPos;
			g_pLTClient->GetObjectPos(m_hTarget, &vPos);

			vF.y = 0.0f;
			vPos += (vF * m_TargetFirstPersonOffset.x);
			vPos.y += m_TargetFirstPersonOffset.y;

			m_vPos = vPos;

			PointInDirection(rRot);
		}
		break;

	case DYING:
		m_vPos = pos;
		break;

	default:
		m_vPos = pos;
		break;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::FindOptimalCameraPosition()
//
//	PURPOSE:	Find the optimal camera position
//
// ----------------------------------------------------------------------- //

LTVector CPlayerCamera::FindOptimalCameraPosition()
{
	LTVector pos;
	VEC_INIT(pos);

	if (!m_pClientDE || !m_hTarget) return pos;

    LTVector     dir;
    float      distToOptimal;
    LTVector     TargetPlusOffset;

    LTVector vTargetPos;
	g_pLTClient->GetObjectPos(m_hTarget, &vTargetPos);
	
    LTRotation rRot;
	g_pLTClient->GetObjectRotation(m_hTarget, &rRot);

	if (vTargetPos.NearlyEquals(m_vLastTargetPos, 1.0f) && (rRot == m_rLastTargetRot))
	{
		return m_vLastOptPos;
	}
	else
	{
		m_vLastTargetPos = vTargetPos;
        m_rLastTargetRot = rRot;
	}

    LTVector vTemp;
	vTemp = vTargetPos + m_TargetChaseOffset;
	TargetPlusOffset = vTemp;

	//	pos = TargetPlusOffset + right*m_OptX + up*m_OptY + forward*m_OptZ;

	ClientIntersectQuery iQuery;
	ClientIntersectInfo  iInfo;

	pos = TargetPlusOffset + 
		(rRot.Right() * m_OptX) + 
		(rRot.Up() * m_OptY) + 
		(rRot.Forward() * m_OptZ);

	vTemp = TargetPlusOffset - pos;
	distToOptimal = vTemp.Length();

	dir = pos - TargetPlusOffset;
	dir.Normalize();

	iQuery.m_From = TargetPlusOffset;
	iQuery.m_To = pos;

	if (m_pClientDE->IntersectSegment(&iQuery, &iInfo))
	{
		vTemp = iInfo.m_Point - TargetPlusOffset;

		// If there was something in the way, move in front of that thing.
		if (vTemp.Length() < distToOptimal)
		{
			pos = iInfo.m_Point + iInfo.m_Plane.m_Normal;
		}
	}


	CalcNonClipPos(pos, rRot);

	m_vLastOptPos = pos;

	return pos;
}


//[MURSUM]===================================================
float CPlayerCamera::FindDyingCameraPosition( float fDist, float fHeightOffset )
{
	int			i;
	LTVector	vTemp;
	LTVector	vAxis;
	LTVector	vCameraPos;	//이동할 위치
	LTVector	vTargetPos;	//시체의 위치
	float		fDistBackup	= 0.0f;
	LTVector	vBackup;
	LTRotation	rRot;
	LTRotation	rRotBackup;

	vTargetPos		= m_vPos;
	vBackup			= vTargetPos;
	m_vDyingStart	= vTargetPos;
	rRot			= m_rRotation;
	rRotBackup		= m_rRotation;
	
	IntersectQuery	iQuery;
	IntersectInfo	iInfo;

	iQuery.m_From	= vTargetPos;
	iQuery.m_Flags	= CHECK_FROM_POINT_INSIDE_OBJECTS | INTERSECT_HPOLY | IGNORE_NONSOLID;

	vAxis.x	= 0.0f;
	vAxis.y	= 1.0f;
	vAxis.z	= 0.0f;

	for( i=0 ; i<20 ; ++i )
	{	
		//18도 간격으로 20번 검사
		rRot.Rotate( vAxis, DEG2RAD(18.0f) );
		iQuery.m_To	= vTargetPos - (rRot.Forward()*fDist);
		iQuery.m_To.y	+= fHeightOffset;

		if( m_pClientDE->IntersectSegment( &iQuery, &iInfo ) )
		{
			if( IsMainWorld( iInfo.m_hObject ) )
			{
				if (iInfo.m_hPoly != INVALID_HPOLY)
				{
					vTemp = iInfo.m_Point - vTargetPos;
					if( vTemp.Length() >= fDist )
					{
						// Find!
						m_vDyingEnd	= iInfo.m_Point;
						m_rRotation	= rRot;						
						return fDist;
					}
					else
					{
						// Backup!
						if( vTemp.Length() > fDistBackup )
						{
							fDistBackup	= vTemp.Length();							
							vBackup		= iInfo.m_Point;
							rRotBackup	= rRot;						
						}						
					}
				}
			}
		}
		else
		{
			// Find!
			m_vDyingEnd	= iQuery.m_To;
			m_rRotation	= rRot;
			return fDist;
		}
	}

	m_vDyingEnd	= vBackup;
	m_rRotation	= rRotBackup;

	return fDistBackup;
}

void CPlayerCamera::UpdateDying()
{
	if( DYING == m_eCameraMode )
	{		
		//[SPIKE]날 죽이놈 쳐다보기 계산!!
		if(m_bRotateCalcStart)
		{
			CalcSeeKiller();
		}
		else
		{
			LTVector vVel = (m_vDyingDir * g_pLTClient->GetFrameTime() * m_fDyingVel);
			m_vPos			+= vVel;
			
			LTVector vTemp = m_vPos-m_vDyingStart;
			
			if( vTemp.Length() >= m_fDyingDist || m_fDyingVel <= 0.0f )
			{
					m_eCameraMode		= DEAD;
					if( IsRevivePlayerGameType() ) g_pHUDPlayerState->ShowAttachBar();
			}
			else if( vTemp.Length() > (m_fDyingDist*0.3f) )
			{
				m_fDyingVel	-= g_pLTClient->GetFrameTime() * 100.0f;
			}
		}
	}
}


VOID 
CPlayerCamera::InitSeeKiller()
{
	LTVector vTempKillerPos = m_vKillerPos;
	LTVector vTempPos		= m_vPos;
	m_vKillerDir = vTempKillerPos - vTempPos;
	m_vKillerDir.y = 0.01f;
	LTVector vTempForward	= m_rRotation.Forward();
	vTempForward.y = 0.01f;
	vTempForward.Normalize();

	//Yaw
	m_vKillerDir.Normalize();
	m_fKillerSeeYaw = (LTFLOAT)acos(vTempForward.Dot(m_vKillerDir));
	LTVector vCross = vTempForward.Cross(m_vKillerDir);
	if(vCross.y > 0) m_fKillerSeeYaw *= -1.0f;

	m_bYawRotComplate	= LTFALSE;
	m_vKillerPos.Init();

	//뒤로 이동 초기화 
	IntersectQuery	iQuery;
	IntersectInfo	iInfo;
	LTVector		vTempDist;

	iQuery.m_From	= vTempPos;
	iQuery.m_Flags	= CHECK_FROM_POINT_INSIDE_OBJECTS | INTERSECT_HPOLY | IGNORE_NONSOLID;

	iQuery.m_To	= vTempPos - (m_vKillerDir * FALLBACKDIST);

	if( m_pClientDE->IntersectSegment( &iQuery, &iInfo ) )
	{
		vTempDist = iInfo.m_Point - vTempPos;
		m_fFallBackDist = vTempDist.Length() - 10.0f;		
	}
	else
	{
		m_fFallBackDist = FALLBACKDIST;
	}
}

VOID
CPlayerCamera::CalcSeeKiller()
{
	LTFLOAT	fTemp = (LTFLOAT)(fabs(m_fKillerSeeYaw) - fabs(m_fYawOffset));
	LTFLOAT fVel = g_pLTClient->GetFrameTime() * fTemp * 2.0f;
	if((fabs(m_fKillerSeeYaw) - fabs(m_fYawOffset)) < 0.01f) 
		m_fYawOffset = m_fKillerSeeYaw;
	
	if((fabs(m_fKillerSeeYaw) > fabs(m_fYawOffset)) && !m_bYawRotComplate)
	{
		if(m_fKillerSeeYaw < 0.0f) fVel *= -1.0f;
		m_rRotation.Rotate(m_rRotation.Up(), fVel);
		m_fYawOffset += fVel;
		
		// Camera FallBack
		if(m_fFallBackDist > m_fFallBackOffset)
		{
			LTFLOAT fFallBackCnt = (LTFLOAT)(fabs(m_fKillerSeeYaw) / fabs(fVel));
			LTFLOAT fFallBackVel = m_fFallBackDist / fFallBackCnt;
			m_vPos += -(m_vKillerDir) * (LTFLOAT)(fabs(fFallBackVel));	
			m_fFallBackOffset += fFallBackVel;
		}
	}
	else
	{
		m_bYawRotComplate = LTTRUE;
		m_fYawOffset	  = 0.0f;
	}
	
	if(m_bYawRotComplate)
	{
		m_fFallBackDist		= 0.0f;
		m_fFallBackOffset	= 0.0f;
		m_eCameraMode		= DEAD;
		m_bRotateCalcStart	= LTFALSE;
		if( IsRevivePlayerGameType() ) 
		{
			g_pHUDPlayerState->ShowAttachBar();
		
			//[SPIKE] 시점제안 Spectator일때는 바로 1인칭 Attach !!
			if(!g_GameDoc.GetRoomOption()->bFreeSpectator)
			{
				g_pPlayerMgr->DoFirstViewMode();
			}
		}
	}
}
//===========================================================


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::PrintVector()
//
//	PURPOSE:	Print it!
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::PrintVector(LTVector v)
{
	if (!m_pClientDE) return;

	char buf[50];
	sprintf(buf, "x = %f, y = %f, z = %f", v.x, v.y, v.z);
	m_pClientDE->CPrint(buf);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::StartCircle()
//
//	PURPOSE:	Start the circle...jerk!
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::StartCircle(float HeightOffset, float Radius, float PointAtOffset, float Time)
{
	if (!m_pClientDE || !m_hTarget) return;

	if(m_eCameraMode != CIRCLING)
	{
		m_CircleHeightOffset = HeightOffset;
		m_CircleRadius		 = Radius;
		m_CircleHeightOffset = PointAtOffset;
		m_CircleTime		 = Time;
        m_bStartCircle       = LTFALSE;

        LTVector vTargetPos, up, right, forward;
        LTRotation rRot;

		g_pLTClient->GetObjectRotation(m_hTarget, &rRot);
		g_pLTClient->GetObjectPos(m_hTarget, &vTargetPos);

		SaveState();

		SaveCameraMode();
		//GetClientShell().CPrint("Going circle");
		m_eCameraMode = CIRCLING;

		m_CircleStartTime = m_pClientDE->GetTime();


		m_vPos = vTargetPos + 
			((rRot.Up() * m_CircleHeightOffset) - 
			 (rRot.Forward() * m_CircleRadius));

		// PointAtPosition(m_Target.m_Pos+CreateVector(0,m_CircleHeightOffset,0), m_Target.m_Angles, m_Pos);
		LTVector vTemp = vTargetPos + LTVector(0.0f, m_CircleHeightOffset, 0.0f);

		PointAtPosition(vTemp, rRot, m_vPos);

		// m_SaveAnglesY = m_Angles.y;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::CircleAroundTarget()
//
//	PURPOSE:	Circle the target
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::CircleAroundTarget()
{
	if (!m_pClientDE || !m_hTarget) return;

    float diff = (m_pClientDE->GetTime() - m_CircleStartTime) / m_CircleTime;

	if(diff >= 1.0f)
	{
		RestoreState();
		RestoreCameraMode();
		//GetClientShell().CPrint("Done with circle");
		return;
	}

	//m_Angles.y = m_SaveAnglesY + (MATH_CIRCLE*diff);
	m_rRotation.Rotate(m_rRotation.Up(), MATH_CIRCLE*diff);

    LTVector vTargetPos;
    LTRotation rRot;
	g_pLTClient->GetObjectPos(m_hTarget, &vTargetPos);
	g_pLTClient->GetObjectRotation(m_hTarget, &rRot);

	m_vPos = vTargetPos + 
		(rRot.Up() * m_CircleHeightOffset) - 
		(rRot.Forward() * m_CircleRadius);

	LTVector vTemp = vTargetPos + LTVector(0.0f, m_CircleHeightOffset, 0.0f);

	PointAtPosition(vTemp, rRot, m_vPos);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::VCompare()
//
//	PURPOSE:	Compare two vectors
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerCamera::VCompare(LTVector a, LTVector b)
{
	if((fabs(a.x - b.x) > 5.0f) || (fabs(a.y - b.y) > 5.0f) || (fabs(a.z - b.z) > 5.0f))
        return LTFALSE;
	else
        return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::AttachToObject()
//
//	PURPOSE:	Attach camera to an object
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::AttachToObject(HLOCALOBJ hObj)
{
	if( m_hTarget == hObj ) return;

	m_hTarget = hObj;

	// Initialize our position to that of the object...

	if (m_hTarget && m_pClientDE)
	{
		g_pLTClient->GetObjectPos(m_hTarget, &m_vPos);

		// Assume for now first person...

        LTRotation rRot;
		g_pLTClient->GetObjectRotation(m_hTarget, &rRot);

        LTVector vF = rRot.Forward();
		vF.y = 0.0f;

		m_vPos += (vF * m_TargetFirstPersonOffset.x);
		m_vPos.y += m_TargetFirstPersonOffset.y;
	}
}

void CPlayerCamera::GoFirstPerson()
{
	if((m_eCameraMode != GOINGFIRSTPERSON) && (m_eCameraMode != FIRSTPERSON))
	{
        m_GoingFirstPersonStart = g_pLTClient->GetTime();
		m_eCameraMode = GOINGFIRSTPERSON;
        CameraUpdate(g_pGameClientShell->GetFrameTime());
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::CreateCameraCollisionObject()
//
//	PURPOSE:	Create an object to use for collision detection when calculating a no-clip pos...
//
// ----------------------------------------------------------------------- //

bool CPlayerCamera::CreateCameraCollisionObject()
{
	if( !g_pLTClient )
		return false;

	ObjectCreateStruct ocs;

	SAFE_STRCPY( ocs.m_Filename, CAMERA_COLLISION_MODEL );
	ocs.m_ObjectType	= OT_MODEL;
	ocs.m_Flags			=0;
	ocs.m_Flags2		= FLAG2_PLAYERCOLLIDE;
	ocs.m_Pos.Init();
	
	float fScale = g_vtCameraCollisionObjScale.GetFloat();
	ocs.m_Scale.Init( fScale, fScale, fScale );

	m_hCollisionObject = g_pLTClient->CreateObject( &ocs );
	if( !m_hCollisionObject )
		return false;

	g_pPhysicsLT->SetObjectDims( m_hCollisionObject, &LTVector( fScale, fScale, fScale ), 0 );

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::CalcNonClipPos()
//
//	PURPOSE:	Get a position that won't clip into the world or other objects...
//				Doesn't actullay move the camera to that position!
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::CalcNonClipPos( LTVector & vPos, LTRotation & rRot )
{
	LTVector vTemp, vU, vR, vF;
	vU = rRot.Up();
	vR = rRot.Right();
	vF = rRot.Forward();

	if( !m_hCollisionObject || (g_vtCameraCollisionUseObject.GetFloat() < 1.0f) )
	{
		CalcNoClipPos_WithoutObject( vPos, rRot );
		return;
	}

	LTVector vCamPos;
	g_pLTClient->GetObjectPos( g_pPlayerMgr->GetCamera(), &vCamPos );

	g_pLTClient->SetObjectPos( m_hCollisionObject, &vCamPos );
	g_pLTClient->SetObjectRotation( m_hCollisionObject, &rRot );

	if( g_vtCameraCollisionObjScale.GetFloat() != DEFAULT_COLLISION_MODEL_SCALE )
	{
		float fScale = g_vtCameraCollisionObjScale.GetFloat();
		LTVector vScale( fScale, fScale, fScale );

		g_pPhysicsLT->SetObjectDims( m_hCollisionObject, &vScale, 0 );
	}

	// 광식 : 죽었을 때는 flag를 setting 하지 않는다.
	//------------------------------------------------------
	if( g_pPlayerMgr->IsPlayerDead() )
		return;
	//------------------------------------------------------

	uint32 dwMoveObjFlags;
	HOBJECT hMoveObj = g_pMoveMgr->GetObject();

	g_pCommonLT->GetObjectFlags( hMoveObj, OFT_Flags, dwMoveObjFlags );
	
	// Set objects the camera is inside of to be non-solid...

	g_pCommonLT->SetObjectFlags( hMoveObj, OFT_Flags, 0, FLAG_SOLID );
	
	// Our collision object needs to be solid while moving...

	g_pCommonLT->SetObjectFlags( m_hCollisionObject, OFT_Flags,	FLAG_SOLID, FLAG_SOLID );
	
	// Do the collison checking and get the final pos...

	g_pPhysicsLT->MoveObject( m_hCollisionObject, &vPos, 0 );
	g_pLTClient->GetObjectPos( m_hCollisionObject, &vPos );
	
	// Set the objects to their previous solidity...

	g_pCommonLT->SetObjectFlags( hMoveObj, OFT_Flags, dwMoveObjFlags, FLAG_SOLID );
	g_pCommonLT->SetObjectFlags( m_hCollisionObject, OFT_Flags, 0, FLAG_SOLID );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerCamera::CalcNoClipPos_WithoutObject()
//
//	PURPOSE:	Just use ray casts to test for collision...
//
// ----------------------------------------------------------------------- //

void CPlayerCamera::CalcNoClipPos_WithoutObject( LTVector & vPos, LTRotation & rRot )
{
	LTVector vTemp, vU, vR, vF;
	vU = rRot.Up();
	vR = rRot.Right();
	vF = rRot.Forward();

	
	uint32 dwFlags = INTERSECT_OBJECTS | IGNORE_NONSOLID | INTERSECT_HPOLY;

	// Make sure we aren't clipping into walls...

    float fClipDistance = g_vtCameraClipDistance.GetFloat();

	// Check for walls to the right...

	vTemp = (vR * fClipDistance);
	vTemp += vPos;

	ClientIntersectQuery iQuery;
	ClientIntersectInfo  iInfo;

	iQuery.m_Flags = dwFlags;
	iQuery.m_From = vPos;
	iQuery.m_To   = vTemp;

	if (g_pLTClient->IntersectSegment(&iQuery, &iInfo))
	{
		vTemp = iInfo.m_Point - vPos;
        float fDist = (fClipDistance - vTemp.Length());

		vTemp = (vR * -fDist);
		vPos += vTemp;
	}
	else
	{
	 	// If we didn't adjust for a wall to the right, check walls to the left...

		vTemp = (vR * -fClipDistance);
		vTemp += vPos;

		iQuery.m_Flags = dwFlags;
		iQuery.m_From = vPos;
		iQuery.m_To = vTemp;

		if (g_pLTClient->IntersectSegment(&iQuery, &iInfo))
		{
			vTemp = iInfo.m_Point - vPos;
            float fDist = (fClipDistance - vTemp.Length());

			vTemp = (vR * fDist);
			vPos += vTemp;
		}
	}

	// Check for ceilings...

	vTemp = vU * fClipDistance;
	vTemp += vPos;

	iQuery.m_Flags = dwFlags;
	iQuery.m_From = vPos;
	iQuery.m_To = vTemp;

	if (g_pLTClient->IntersectSegment(&iQuery, &iInfo))
	{
		vTemp = iInfo.m_Point - vPos;
        float fDist = (fClipDistance - vTemp.Length());

		vTemp = vU * -fDist;
		vPos += vTemp;
	}
	else
	{
 		// If we didn't hit any ceilings, check for floors...

		vTemp = vU * -fClipDistance;
		vTemp += vPos;

		iQuery.m_Flags = dwFlags;
		iQuery.m_From = vPos;
		iQuery.m_To = vTemp;

		if (g_pLTClient->IntersectSegment(&iQuery, &iInfo))
		{
			vTemp = iInfo.m_Point - vPos;
            float fDist = (fClipDistance - vTemp.Length());

			vTemp = vU * fDist;
			vPos += vTemp;
		}
	}

	// Check infront of us...

	vTemp = vF * fClipDistance;
	vTemp += vPos;

	iQuery.m_Flags = dwFlags;
	iQuery.m_From = vPos;
	iQuery.m_To = vTemp;

	if( g_pLTClient->IntersectSegment( &iQuery, &iInfo ))
	{
		vTemp = iInfo.m_Point - vPos;
		float fDist = (fClipDistance - vTemp.Length());

		vTemp = vF * -fDist;
		vPos += vTemp;
	}
	else
	{
		vTemp = vF * -fClipDistance;
		vTemp += vPos;

		iQuery.m_Flags = dwFlags;
		iQuery.m_From = vPos;
		iQuery.m_To = vTemp;

		if( g_pLTClient->IntersectSegment( &iQuery, &iInfo ))
		{
			vTemp = iInfo.m_Point - vPos;
			float fDist = (fClipDistance - vTemp.Length());

			vTemp = vU * -fDist;
			vPos += vTemp;
		}
	}
}

void CPlayerCamera::SetRotation( LTRotation& rRot )
{
	m_rRotation = rRot;
	g_pLTClient->SetObjectRotation(m_hTarget, &m_rRotation);
}

/*
// Added By Bestline ==>
// Time : 2004-07-12 오후 7:57:47
// 카메라를 오부젝트에서 떨어져서 작동 하도록..
void CPlayerCamera::DetatchToggle()
{
	if ( !g_pPlayerMgr )
		return;

	// 카메라가 의존적인 작업을 하고 있을때는 attach/detatch되지 않도록 한다.	
	if ( (m_eCameraMode != CIRCLING) )
	{
		if ( (g_pPlayerMgr->GetPlayerState() == PS_ALIVE) && !g_pPlayerMgr->IsZoomed() )
		{
			if ( m_hTarget )
			{
				SaveCameraMode();
				AttachToObject(NULL);
				m_bIsDetatched = LTTRUE;
				g_pPlayerMgr->SetExternalCamera(LTTRUE);				
			}
			else
			{				
				AttachToObject(m_hSavedTarget);			
				m_bIsDetatched = LTFALSE;
				if ( m_eSaveCameraMode == FIRSTPERSON )
					g_pPlayerMgr->SetExternalCamera(LTFALSE);
			}
		}
	}
}
// Added By Bestline <==
*/