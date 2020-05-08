// ----------------------------------------------------------------------- //
//
// MODULE  : PlayerSoundFX.cpp
//
// PURPOSE : Player sound special FX - Implementation
//
// CREATED : 7/28/98 (was WeaponSoundFX)
//
// (c) 1998-2000 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "PlayerSoundFX.h"
#include "GameClientShell.h"
#include "iltclient.h"
#include "MsgIds.h"
// 광식
#include "ClientMultiplayerMgr.h"

extern CGameClientShell* g_pGameClientShell;

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerSoundFX::Init
//
//	PURPOSE:	Init the fx
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerSoundFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
    if (!CSpecialFX::Init(psfxCreateStruct)) return LTFALSE;

	PLAYERSOUNDCREATESTRUCT* pPSCS = (PLAYERSOUNDCREATESTRUCT*)psfxCreateStruct;

	m_vPos		= pPSCS->vPos;
	m_nClientId = pPSCS->nClientId;
	m_nType		= pPSCS->nType;
	m_nWeaponId	= pPSCS->nWeaponId;

    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerSoundFX::CreateObject
//
//	PURPOSE:	Create object associated with the CPlayerSoundFX
//
// ----------------------------------------------------------------------- //

LTBOOL CPlayerSoundFX::CreateObject(ILTClient *pClientDE)
{
    if (!CSpecialFX::CreateObject(pClientDE)) return LTFALSE;

    uint32 dwId;
    if (m_pClientDE->GetLocalClientID(&dwId) != LT_OK) return LTFALSE;

	// Don't play sounds for this client...

    if (int(dwId) == m_nClientId) return LTFALSE;

	PlayerSoundId eSndType = (PlayerSoundId)m_nType;

	if (::IsWeaponSound(eSndType))
	{
		WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(m_nWeaponId);
		if (!pWeapon) return LTFALSE;

		// 광식 [sound] : 너무 멀리 있는 녀석들은 아에 Sound Play 자체를 하지 않으려고...
		//---------------------------------------------------------------------------------
		HOBJECT hLocalObj = g_pLTClient->GetClientObject();
		if( hLocalObj )
		{
			LTVector vLocalPos;
			LTVector vResult;
			g_pLTClient->GetObjectPos( hLocalObj, &vLocalPos);
			vResult = vLocalPos - m_vPos ;
			float Mag = vResult.Mag();
			if( Mag <= pWeapon->nWeaponSoundRadius )
			{
				PlayWeaponSound(pWeapon, m_vPos, (PlayerSoundId)m_nType);
			}
		}
		//---------------------------------------------------------------------------------
		//PlayWeaponSound(pWeapon, m_vPos, (PlayerSoundId)m_nType);
	}
	else
	{
		switch (eSndType)
		{
			case PSI_JUMP :
			{
				// 광식 : sound 위치 변경
				//char* pSounds[] = { "Chars\\Snd\\jump1.wav", "Chars\\Snd\\jump2.wav" };
#ifdef BIG_FAT_MODE_TEST
				if( LTFALSE == g_GameDoc.IsBigHeadMode() )
				{
					char* pSounds[] = { "SA_CHARACTERS\\SND\\jump1.wav", "SA_CHARACTERS\\SND\\jump2.wav" };
					g_pClientSoundMgr->PlaySoundFromPos(m_vPos, pSounds[GetRandom(0,1)], 1000.0f,
														SOUNDPRIORITY_MISC_HIGH);
				}
				else
				{
					char* pSounds[] = { "SA_CHARACTERS\\SND\\FatEtc\\jump1.wav", "SA_CHARACTERS\\SND\\FatEtc\\jump2.wav" };
					g_pClientSoundMgr->PlaySoundFromPos(m_vPos, pSounds[GetRandom(0,1)], 1000.0f,
														SOUNDPRIORITY_MISC_HIGH);
				}
#else
				char* pSounds[] = { "SA_CHARACTERS\\SND\\jump1.wav", "SA_CHARACTERS\\SND\\jump2.wav" };
				g_pClientSoundMgr->PlaySoundFromPos(m_vPos, pSounds[GetRandom(0,1)], 1000.0f,
					SOUNDPRIORITY_MISC_HIGH);
#endif
			}
			break;

			case PSI_LAND :
			{
				// 광식 : sound 위치 변경
				//char* pSounds[] = { "Chars\\Snd\\player\\landing1.wav", "Chars\\Snd\\player\\landing2.wav" };
				char* pSounds[] = { "SA_CHARACTERS\\SND\\landing1.wav", "SA_CHARACTERS\\SND\\landing2.wav" };
				g_pClientSoundMgr->PlaySoundFromPos(m_vPos, pSounds[GetRandom(0,1)], 1000.0f,
					SOUNDPRIORITY_MISC_HIGH);
			}
			break;

			default : break;
		}
	}


    return LTFALSE;  // Delete me, I'm done :)
}