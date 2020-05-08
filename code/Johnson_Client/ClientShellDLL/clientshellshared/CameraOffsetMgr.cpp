// ----------------------------------------------------------------------- //
//
// MODULE  : CameraOffsetMgr.cpp
//
// PURPOSE : Camera offset mgr - Implementation
//
// CREATED : 8/23/99
//
// (c) 1999-2000 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "CameraOffsetMgr.h"
#include "VarTrack.h"
#include "GameClientShell.h"

#define		CAM_MAX_PITCH_OFFSET	15.0f
#define		CAM_MAX_YAW_OFFSET		15.0f
#define		CAM_MAX_ROLL_OFFSET		15.0f
#define		CAM_MAX_POS_X_OFFSET	200.0f
#define		CAM_MAX_POS_Y_OFFSET	200.0f
#define		CAM_MAX_POS_Z_OFFSET	200.0f

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CCameraOffsetMgr::CCameraOffsetMgr
//
//	PURPOSE:	Constructor
//
// --------------------------------------------------------------------------- //

CCameraOffsetMgr::CCameraOffsetMgr()
{
	m_vPitchYawRollDelta.Init();
	m_vPosDelta.Init();
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CCameraOffsetMgr::Init
//
//	PURPOSE:	Init
//
// --------------------------------------------------------------------------- //

LTBOOL CCameraOffsetMgr::Init()
{
	m_vPitchYawRollDelta.Init();
	m_vPosDelta.Init();

    return LTTRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CCameraOffsetMgr::Update
//
//	PURPOSE:	Update all our deltas
//
// --------------------------------------------------------------------------- //

void CCameraOffsetMgr::Update()
{
	// Reset offsets...

	m_vPitchYawRollDelta.Init();
	m_vPosDelta.Init();

    float fTimeDelta = g_pGameClientShell->GetFrameTime();

    int i;
    for (i=0; i < MAX_CAMERA_DELTAS; i++)
	{
		if (m_CameraDeltas[i].GetTotalDelta() > 0.0f)
		{
			m_CameraDeltas[i].Pitch.UpdateVar(fTimeDelta);
			m_vPitchYawRollDelta.x += m_CameraDeltas[i].Pitch.GetValue();

			m_CameraDeltas[i].Yaw.UpdateVar(fTimeDelta);
			m_vPitchYawRollDelta.y += m_CameraDeltas[i].Yaw.GetValue();

			m_CameraDeltas[i].Roll.UpdateVar(fTimeDelta);
			m_vPitchYawRollDelta.z += m_CameraDeltas[i].Roll.GetValue();

			m_CameraDeltas[i].PosX.UpdateVar(fTimeDelta);
			m_vPosDelta.x += m_CameraDeltas[i].PosX.GetValue();

			m_CameraDeltas[i].PosY.UpdateVar(fTimeDelta);
			m_vPosDelta.y += m_CameraDeltas[i].PosY.GetValue();

			m_CameraDeltas[i].PosZ.UpdateVar(fTimeDelta);
			m_vPosDelta.z += m_CameraDeltas[i].PosZ.GetValue();
		}
	}

	for (i=0; i < MAX_STATIC_CAMERA_DELTAS; i++)
	{
		if (m_StaticCameraDeltas[i].GetTotalDelta() > 0.0f)
		{
			m_StaticCameraDeltas[i].Pitch.UpdateVar(fTimeDelta);
			m_vPitchYawRollDelta.x += m_StaticCameraDeltas[i].Pitch.GetValue();

			m_StaticCameraDeltas[i].Yaw.UpdateVar(fTimeDelta);
			m_vPitchYawRollDelta.y += m_StaticCameraDeltas[i].Yaw.GetValue();

			m_StaticCameraDeltas[i].Roll.UpdateVar(fTimeDelta);
			m_vPitchYawRollDelta.z += m_StaticCameraDeltas[i].Roll.GetValue();

			m_StaticCameraDeltas[i].PosX.UpdateVar(fTimeDelta);
			m_vPosDelta.x += m_StaticCameraDeltas[i].PosX.GetValue();

			m_StaticCameraDeltas[i].PosY.UpdateVar(fTimeDelta);
			m_vPosDelta.y += m_StaticCameraDeltas[i].PosY.GetValue();

			m_StaticCameraDeltas[i].PosZ.UpdateVar(fTimeDelta);
			m_vPosDelta.z += m_StaticCameraDeltas[i].PosZ.GetValue();
		}
	}

	ValidateDeltas();
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CCameraOffsetMgr::ValidateDeltas
//
//	PURPOSE:	Make sure all the deltas are valid
//
// --------------------------------------------------------------------------- //

void CCameraOffsetMgr::ValidateDeltas()
{
	if (m_vPitchYawRollDelta.x > CAM_MAX_PITCH_OFFSET)
	{
		m_vPitchYawRollDelta.x = CAM_MAX_PITCH_OFFSET;
	}
	else if (m_vPitchYawRollDelta.x < -CAM_MAX_PITCH_OFFSET)
	{
		m_vPitchYawRollDelta.x = -CAM_MAX_PITCH_OFFSET;
	}

	if (m_vPitchYawRollDelta.y > CAM_MAX_YAW_OFFSET)
	{
		m_vPitchYawRollDelta.y = CAM_MAX_YAW_OFFSET;
	}
	else if (m_vPitchYawRollDelta.y < -CAM_MAX_YAW_OFFSET)
	{
		m_vPitchYawRollDelta.y = -CAM_MAX_YAW_OFFSET;
	}

	if (m_vPitchYawRollDelta.z > CAM_MAX_ROLL_OFFSET)
	{
		m_vPitchYawRollDelta.z = CAM_MAX_ROLL_OFFSET;
	}
	else if (m_vPitchYawRollDelta.z < -CAM_MAX_ROLL_OFFSET)
	{
		m_vPitchYawRollDelta.z = -CAM_MAX_ROLL_OFFSET;
	}

	if (m_vPosDelta.x > CAM_MAX_POS_X_OFFSET)
	{
		m_vPosDelta.x = CAM_MAX_POS_X_OFFSET;
	}
	else if (m_vPosDelta.x < -CAM_MAX_POS_X_OFFSET)
	{
		m_vPosDelta.x = -CAM_MAX_POS_X_OFFSET;
	}

	if (m_vPosDelta.y > CAM_MAX_POS_Y_OFFSET)
	{
		m_vPosDelta.y = CAM_MAX_POS_Y_OFFSET;
	}
	else if (m_vPosDelta.y < -CAM_MAX_POS_Y_OFFSET)
	{
		m_vPosDelta.y = -CAM_MAX_POS_Y_OFFSET;
	}

	if (m_vPosDelta.z > CAM_MAX_POS_Z_OFFSET)
	{
		m_vPosDelta.z = CAM_MAX_POS_Z_OFFSET;
	}
	else if (m_vPosDelta.z < -CAM_MAX_POS_Z_OFFSET)
	{
		m_vPosDelta.z = -CAM_MAX_POS_Z_OFFSET;
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CCameraOffsetMgr::AddDelta
//
//	PURPOSE:	Add a new delta
//
// --------------------------------------------------------------------------- //

void CCameraOffsetMgr::AddDelta(CameraDelta & delta)
{
	// Find an open slot for the new delta...

	float fMinTotalDelta = 100000.0f;
	int nSlot = 0;
	for (int i=0; i < MAX_CAMERA_DELTAS; i++)
	{
		float fTotalDelta = m_CameraDeltas[i].GetTotalDelta();

		if (fTotalDelta == 0.0f)
		{
			m_CameraDeltas[i] = delta;
			return;
		}
		else
		{
			if (fTotalDelta < fMinTotalDelta)
			{
				fMinTotalDelta = fTotalDelta;
				nSlot = i;
			}
		}
	}

	// Override the slot with the least amount of change...

	if (delta.GetTotalDelta() > fMinTotalDelta)
	{
		m_CameraDeltas[nSlot] = delta;
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CCameraOffsetMgr::SetStaticDelta
//
//	PURPOSE:	Set a static delta
//
// --------------------------------------------------------------------------- //

void CCameraOffsetMgr::SetStaticDelta(CameraDelta & delta, int nIndex)
{
	if (nIndex < 0 || nIndex >= MAX_STATIC_CAMERA_DELTAS) return;

	m_StaticCameraDeltas[nIndex] = delta;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CCameraOffsetMgr::SetStaticDelta
//
//	PURPOSE:	Set a static delta
//
// --------------------------------------------------------------------------- //

CameraDelta* CCameraOffsetMgr::GetStaticDelta(int nIndex)
{
	if (nIndex < 0 || nIndex >= MAX_STATIC_CAMERA_DELTAS) return LTNULL;

	return &(m_StaticCameraDeltas[nIndex]);
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CameraAdjustVar::UpdateVar
//
//	PURPOSE:	Update the variable value
//
// --------------------------------------------------------------------------- //

void CameraAdjustVar::UpdateVar(float fTimeDelta)
{
	if (fVar == 0.0f) return;

	// Figure out the direction we are going...

	if (m_fCurTime <= 0.0f)
	{
		if (fTime1 > 0.0f)
		{
			m_nDir = 1;
			m_fLastRealValue = 0.0f;
		}
		else if (fTime2 > 0.0f)
		{
			m_nDir = -1;
			m_fLastRealValue = fVar;
		}
		else
		{
			return;
		}
	}


	// Determine percent of time gone by...

	float fPercent = 0.0f;
	WaveType eType = eWave1;

	if (m_nDir == 1)
	{
		eType = eWave1;
		fPercent = m_fCurTime / fTime1;
		m_fRealValue = fVar * GetWaveFn(eType)(fPercent);
	}
	else
	{
		eType = eWave2;
		fPercent = m_fCurTime / fTime2;
		m_fRealValue = fVar - (fVar * GetWaveFn(eType)(fPercent));
	}


	// Set our variable value as an increment from the last value...

	if (bIncrement)
	{
		m_fValue = m_fRealValue - m_fLastRealValue;
		m_fLastRealValue = m_fRealValue;
	}
	else // Normal calculation...
	{
		m_fValue = m_fRealValue;
	}

    // g_pLTClient->CPrint("m_fValue = %.2f (in Deg = %.2f)", m_fValue, RAD2DEG(m_fValue));

	// Calculate new value...

	m_fCurTime += fTimeDelta;

	if (m_nDir == 1 && m_fCurTime > fTime1)
	{
		m_fCurTime = 0.0f;
		fTime1 = 0.0f;
	}
	else if (m_nDir == -1 && m_fCurTime > fTime2)
	{
		// We're done, so clear everything...

		Init();
	}
}
