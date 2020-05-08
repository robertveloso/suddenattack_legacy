// ----------------------------------------------------------------------- //
//
// MODULE  : MissionButeMgr.cpp
//
// PURPOSE : MissionButeMgr implementation - Controls attributes of all
//			 missions
//
// CREATED : 07/26/99
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "MissionButeMgr.h"
#include "CommonUtilities.h"
#include "WeaponMgr.h"


#define MMGR_MISSION_NAMEID				"NameId"
#define MMGR_MISSION_OBJECTIVEIDS_A		"ObjectiveIds_A"
#define MMGR_MISSION_OBJECTIVEIDS_B		"ObjectiveIds_B"
#define MMGR_MISSION_OBJECTIVESOUND		"ObjectiveSoundFile"
#define MMGR_MISSION_LAYOUT				"LayoutInfo"
#define MMGR_MISSION_MAP				"Map"
#define MMGR_MISSION_SCREENSHOT			"ScreenShot"
#define MMGR_MISSION_MINIMAP			"MiniMap"
#define MMGR_MISSION_WEAPONS_DEFAULT	"DefaultWeapons"
#define MMGR_MISSION_AMMO_DEFAULT		"DefaultAmmo"
#define MMGR_MISSION_TYPE_DEFAULT		"DefaultGameType"
#define MMGR_MISSION_SHORT_TYPEID		"ShortGameTypeIds"
#define MMGR_MISSION_TYPEID				"GameTypeIds"

#define MMGR_MISSION_TIME_DEFAULT		"DefaultTimeLimit"
#define MMGR_MISSION_KILL_DEFAULT		"DefaultKillLimit"
#define MMGR_MISSION_ROUND_DEFAULT		"DefaultRoundLimit"

#define MMGR_MISSION_WALK_SPEED			"WalkSpeed"
#define MMGR_MISSION_RUN_SPEED			"RunSpeed"
#define MMGR_MISSION_JUMP_SPEED			"JumpSpeed"
#define MMGR_MISSION_SUPER_JUMP_SPEED	"SuperJumpSpeed"
#define MMGR_MISSION_SWIM_SPEED			"SwimSpeed"
#define MMGR_MISSION_LADDER_SPEED		"LadderSpeed"
#define MMGR_MISSION_GRAVITY			"Gravity"

#define MMGR_MISSION_LEVEL				"Level"
#define MMGR_MISSION_REWARD				"Reward"




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMissionButeMgr::CMissionButeMgr()
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

CMissionButeMgr::CMissionButeMgr()
{
	m_bInitted = LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMissionButeMgr::~CMissionButeMgr()
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

CMissionButeMgr::~CMissionButeMgr()
{
	g_pMissionButeMgr = NULL;

	Term();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMissionButeMgr::Term()
//
//	PURPOSE:	Clean up.
//
// ----------------------------------------------------------------------- //

void CMissionButeMgr::Term()
{
	MissionList::iterator it = m_MissionList.begin( );
	while( it != m_MissionList.end( ))
	{
		DestroyMission( *it );
		it++;
	}
	m_MissionList.clear();

	CGameButeMgr::Term( );

	m_bInitted = LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMissionButeMgr::GetMission
//
//	PURPOSE:	Get the specified mission
//
// ----------------------------------------------------------------------- //

MISSION* CMissionButeMgr::GetMission(int nMissionId)
{
    if (nMissionId < 0 || nMissionId > ( int )m_MissionList.size( )) return LTNULL;

	MissionList::iterator it = m_MissionList.begin( );
	for( ; it != m_MissionList.end( ); it++ )
	{
		MISSION* pMission = *it;
		if( !pMission )
			continue;

		if( pMission->nId == nMissionId )
			return pMission;
	}

    return NULL;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CMissionButeMgr::IsMissionLevel()
//
//	PURPOSE:	Determine if the passed in string is a level of one of the
//				missions.
//
//	RETURNS:	True if a mission level, else false.  Also nMissionId and
//				nLevel are filled in with the correct values.
//
// ----------------------------------------------------------------------- //

bool CMissionButeMgr::IsMissionLevel(char const* pWorldFile, int & nMissionId, int & nLevel)
{
	if (!pWorldFile) 
		return false;


	//strip path (if there is one) from world name
	char const* pTmp = strrchr(pWorldFile,'\\');
	if (pTmp)
	{
		pTmp++;
		pWorldFile = pTmp;
	}

	MissionList::iterator it = m_MissionList.begin( );
	for( ; it != m_MissionList.end( ); it++)
	{
		MISSION* pMission = *it;
		if( !pMission )
			continue;

		pTmp = strrchr(pMission->szMap,'\\');
		if (pTmp)
		{
			pTmp++;
		}
		

		if (stricmp( pTmp, pWorldFile ) == 0)
		{
			nMissionId = pMission->nId;
			nLevel = 0;
            return true;
		}
	}

    return false;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	MISSION::MISSION
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

MISSION::MISSION()
{
	nId					= -1;
	nNameId				= -1;
	
	szLayout[0]			= '\0';

#ifdef _CLIENTBUILD
	hScreenShot			= LTNULL;
	hMiniMap			= LTNULL;
#endif

	nNumDefaultWeapons	= 0;
	nNumDefaultAmmo		= 0;
	aDefaultWeapons[0]	= WMGR_INVALID_ID;
	aDefaultAmmo[0]		= WMGR_INVALID_ID;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	MISSION::Init
//
//	PURPOSE:	Build the mission struct
//
// ----------------------------------------------------------------------- //

LTBOOL MISSION::Init(CButeMgr & buteMgr, char* aTagName)
{
    if (!aTagName) return LTFALSE;

	char szStr[512] ="";
	char szStrMini[512] = {0, };

	nNameId			= buteMgr.GetInt(aTagName, MMGR_MISSION_NAMEID);
	//[TODO] nNameId -> sName
	
	nObjectiveID_A	= buteMgr.GetInt( aTagName, MMGR_MISSION_OBJECTIVEIDS_A );
	nObjectiveID_B	= buteMgr.GetInt( aTagName, MMGR_MISSION_OBJECTIVEIDS_B );
	//[TODO] nObjectiveID -> sObjective
	buteMgr.GetString(aTagName, MMGR_MISSION_OBJECTIVESOUND, szObjectiveSndFileName, ARRAY_LEN(szObjectiveSndFileName));

	buteMgr.GetString(aTagName, MMGR_MISSION_LAYOUT, szLayout, ARRAY_LEN(szLayout));

	buteMgr.GetString(aTagName, MMGR_MISSION_MAP, szMap, ARRAY_LEN(szMap));
	buteMgr.GetString(aTagName, MMGR_MISSION_SCREENSHOT, szStr, ARRAY_LEN(szStr));
	buteMgr.GetString(aTagName, MMGR_MISSION_MINIMAP, szStrMini);

#ifdef _CLIENTBUILD
	hScreenShot	= g_pInterfaceResMgr->GetTexture( szStr );
	hMiniMap	= g_pInterfaceResMgr->GetTexture( szStrMini );
#endif

	eGameType			= (GameType)buteMgr.GetInt(aTagName, MMGR_MISSION_TYPE_DEFAULT);
	nTimeLimit			= buteMgr.GetInt(aTagName, MMGR_MISSION_TIME_DEFAULT);
	nKillLimit			= buteMgr.GetInt(aTagName, MMGR_MISSION_KILL_DEFAULT);
	nRoundLimit			= buteMgr.GetInt(aTagName, MMGR_MISSION_ROUND_DEFAULT);

	nShortGameTypeID	= buteMgr.GetInt(aTagName, MMGR_MISSION_SHORT_TYPEID);
	nGameTypeID			= buteMgr.GetInt(aTagName, MMGR_MISSION_TYPEID);
	
	buteMgr.GetString(aTagName, MMGR_MISSION_WEAPONS_DEFAULT,szStr,sizeof(szStr));
	nNumDefaultWeapons = BuildWeaponsList(szStr, aDefaultWeapons, ARRAY_LEN(aDefaultWeapons));
	buteMgr.GetString(aTagName, MMGR_MISSION_AMMO_DEFAULT,szStr,sizeof(szStr));
	nNumDefaultAmmo = BuildAmmoList(szStr, aDefaultAmmo, ARRAY_LEN(aDefaultAmmo));

	nWalkSpeed			= buteMgr.GetInt(aTagName, MMGR_MISSION_WALK_SPEED);
	nRunSpeed			= buteMgr.GetInt(aTagName, MMGR_MISSION_RUN_SPEED);
	nJumpSpeed			= buteMgr.GetInt(aTagName, MMGR_MISSION_JUMP_SPEED);
	nSuperJumpSpeed		= buteMgr.GetInt(aTagName, MMGR_MISSION_SUPER_JUMP_SPEED);
	nSwimSpeed			= buteMgr.GetInt(aTagName, MMGR_MISSION_SWIM_SPEED);
	nLadderSpeed		= buteMgr.GetInt(aTagName, MMGR_MISSION_LADDER_SPEED);
	nGravity			= buteMgr.GetInt(aTagName, MMGR_MISSION_GRAVITY);

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	MISSION::Save
//
//	PURPOSE:	Save the mission struct
//
// ----------------------------------------------------------------------- //

LTBOOL MISSION::Save(CButeMgr & buteMgr, char* aTagName)
{
	/*
    if (!aTagName) return LTFALSE;

	char aAttName[100];


//***********************************************
//*jrg  NOTE:Only saving level info at this point
//***********************************************

	for (int n = 0; n < nNumLevels; n++)
	{
		sprintf(aAttName, "%s%d", MMGR_MISSION_LEVEL, n);
		buteMgr.SetString(aTagName, aAttName,aLevels[n].szLevel);
	}
	*/
    return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	MISSION::BuildWeaponsList()
//
//	PURPOSE:	Builds an array of ints from the string associated with
//					the given string;
//
//	RETURNS:	Number of weapons added to list
//
// ----------------------------------------------------------------------- //

int MISSION::BuildWeaponsList(char* pszStr, int* pArray, int nArrayLen)
{
	if (!pArray || !pszStr) return 0;

	int  nNumWeapons = 0;
	char buf[512] = "";

	while (*pszStr == ' ' || *pszStr == '\t')
		pszStr++;

	if (strlen(pszStr))
	{
		SAFE_STRCPY(buf, pszStr);
		char *pWpnName = strtok(buf,",");
		while (pWpnName && nNumWeapons < nArrayLen)
		{
			WEAPON const *pWeapon = g_pWeaponMgr->GetWeapon(pWpnName);
			if (pWeapon)
			{
				pArray[nNumWeapons] = pWeapon->nId;
				nNumWeapons++;
			}
			else
			{
#ifdef _CLIENTBUILD
                g_pLTClient->CPrint("Error in %s: Unknown weapon: %s", g_pMissionButeMgr->GetAttributeFile( ),pWpnName);
#else
                g_pLTServer->CPrint("Error in %s: Unknown weapon: %s", g_pMissionButeMgr->GetAttributeFile( ),pWpnName);
#endif
            }
			pWpnName = strtok(NULL,",");
		}
	}

	return nNumWeapons;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	MISSION::BuildAmmoList()
//
//	PURPOSE:	Builds an array of ints from the string associated with
//					the given string;
//
//	RETURNS:	Number of ammo added to list
//
// ----------------------------------------------------------------------- //

int MISSION::BuildAmmoList(char *pszStr, int* pArray, int nArrayLen)
{
	if (!pArray || !pszStr) return 0;

	int  nNumAmmo = 0;
	char buf[512] = "";

	while (*pszStr == ' ' || *pszStr == '\t')
		pszStr++;


	if (strlen(pszStr))
	{
		SAFE_STRCPY(buf, pszStr);
		char *pAmmoName = strtok(buf,",");
		while (pAmmoName && nNumAmmo < nArrayLen)
		{
			AMMO const *pAmmo = g_pWeaponMgr->GetAmmo(pAmmoName);
			if (pAmmo)
			{
				pArray[nNumAmmo] = pAmmo->nId;
				nNumAmmo++;
			}
			else
			{
#ifdef _CLIENTBUILD
                g_pLTClient->CPrint("Error in %s: Unknown ammo: %s", g_pMissionButeMgr->GetAttributeFile( ),pAmmoName);
#else
                g_pLTServer->CPrint("Error in %s: Unknown ammo: %s", g_pMissionButeMgr->GetAttributeFile( ),pAmmoName);
#endif
			}

			pAmmoName = strtok(NULL,",");
		}
	}

	return nNumAmmo;
}
