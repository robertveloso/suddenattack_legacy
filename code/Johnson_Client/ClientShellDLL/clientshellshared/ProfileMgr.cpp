// ----------------------------------------------------------------------- //
//
// MODULE  : ProfileMgr.cpp
//
// PURPOSE : Manages player profiles
//
// (c) 2001-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"

#include "profileMgr.h"
#include "clientutilities.h"
#include "commandids.h"
#include "interfacemgr.h"
#include "winutil.h"
#include "ClientResShared.h"
#include "GameClientShell.h"
#include "GameSettings.h"
#include "MenuMgr.h"
#include "ClientMultiplayerMgr.h"
#include "VarTrack.h"
#include "CMoveMgr.h"
#include "MsgIds.h"
#include "VersionMgr.h"
#include "SystemInfo.h"

extern CGameClientShell* g_pGameClientShell;

#include <Direct.h>			// For _rmdir
#include "dinput.h"
#include <set>
#include <IO.H>

VarTrack	g_vtMouseMinSensitivity;
VarTrack	g_vtMouseMaxSensitivity;
VarTrack	g_vtMouseMinInputRate;
VarTrack	g_vtMouseMaxInputRate;


CProfileMgr* g_pProfileMgr = LTNULL;

namespace
{
    uint32 devices[3] =
	{
		DEVICETYPE_KEYBOARD,
		DEVICETYPE_MOUSE,
	/*	DEVICETYPE_JOYSTICK*/
	};

	char strDeviceName[3][256] =
	{
		"","",""
	};

	char szWheelUp[32] = "";
	char szWheelDown[32] = "";

	char szJoyLookUp[32] = "";
	char szJoyLookDown[32] = "";
	char szJoyForward[32] = "";
	char szJoyBackward[32] = "";
	char szJoyLeft[32] = "";
	char szJoyRight[32] = "";
	char szJoyStrafeLeft[32] = "";
	char szJoyStrafeRight[32] = "";


	char szDefProfileName[16] = "";
}

	char s_aTagName[30];
	char s_aAttName[30];

//================================================================================================================
//[SPIKE] 순서변경 및 쓰임새변경 
//================================================================================================================

CommandData g_CommandArray[] =
{
	{ IDS_CONTROL_FORWARD,			COMMAND_ID_FORWARD,			IDS_ACTIONSTRING_FORWARD,			COM_MOVE },
	{ IDS_CONTROL_BACKWARD,			COMMAND_ID_REVERSE,			IDS_ACTIONSTRING_BACKWARD,			COM_MOVE },
	{ IDS_CONTROL_STRAFELEFT,		COMMAND_ID_STRAFE_LEFT,		IDS_ACTIONSTRING_STRAFELEFT,		COM_MOVE },
	{ IDS_CONTROL_STRAFERIGHT,		COMMAND_ID_STRAFE_RIGHT,	IDS_ACTIONSTRING_STRAFERIGHT,		COM_MOVE },
	{ IDS_CONTROL_TURNLEFT,			COMMAND_ID_LEFT,			IDS_ACTIONSTRING_TURNLEFT,			COM_MOVE },
	{ IDS_CONTROL_TURNRIGHT,		COMMAND_ID_RIGHT,			IDS_ACTIONSTRING_TURNRIGHT,			COM_MOVE },
	{ IDS_CONTROL_LOOKUP,			COMMAND_ID_LOOKUP,			IDS_ACTIONSTRING_LOOKUP,			COM_VIEW },
	{ IDS_CONTROL_LOOKDOWN,			COMMAND_ID_LOOKDOWN,		IDS_ACTIONSTRING_LOOKDOWN,			COM_VIEW },
	{ IDS_CONTROL_TURNAROUND,		COMMAND_ID_TURNAROUND,		IDS_ACTIONSTRING_TURNAROUND,		COM_MOVE },
	
	
	{ IDS_CONTROL_DUCK,				COMMAND_ID_DUCK,			IDS_ACTIONSTRING_DUCK,				COM_MOVE },
	{ IDS_CONTROL_RUN,				COMMAND_ID_RUN,				IDS_ACTIONSTRING_RUN,				COM_MOVE },
	{ IDS_CONTROL_RUNLOCKTOGGLE,	COMMAND_ID_RUNLOCK,			IDS_ACTIONSTRING_RUNLOCKTOGGLE,		COM_MOVE },
	{ IDS_CONTROL_JUMP,				COMMAND_ID_JUMP,			IDS_ACTIONSTRING_JUMP,				COM_MOVE },

	
	{ IDS_CONTROL_FIRE,				COMMAND_ID_FIRING,			IDS_ACTIONSTRING_FIRE,				COM_INV },
	{ IDS_CONTROL_RELOAD,			COMMAND_ID_RELOAD,			IDS_ACTIONSTRING_RELOAD,			COM_INV },
	{ IDS_CONTROL_ACTIVATE,			COMMAND_ID_ACTIVATE,		IDS_ACTIONSTRING_ACTIVATE,			COM_INV },
	{ IDS_CONTROL_LEANRIGHT,		COMMAND_ID_LEAN_RIGHT,		IDS_ACTIONSTRING_LEANRIGHT,			COM_MOVE },
	{ IDS_CONTROL_ZOOM_IN,			COMMAND_ID_ZOOM_IN,			IDS_ACTIONSTRING_ZOOMIN,			COM_VIEW },

	//무기변경 슬롯(1,2,3,4,5,6)
	{ IDS_CONTROL_STRAFE,		   COMMAND_ID_STRAFE,			IDS_ACTIONSTRING_STRAFE,			COM_INV },
	{ IDS_CONTROL_NEXT_AMMO,	   COMMAND_ID_NEXT_AMMO,		IDS_ACTIONSTRING_NEXTAMMO,			COM_INV },
	{ IDS_CONTROL_HOLSTERWEAPON,   COMMAND_ID_HOLSTER,			IDS_ACTIONSTRING_HOLSTERWEAPON,		COM_INV },
	{ IDS_CONTROL_MOVE_BODY,	   COMMAND_ID_MOVE_BODY,		IDS_ACTIONSTRING_MOVE_BODY,			COM_INV },
	{ IDS_CONTROL_CROUCHTOGGLE,	   COMMAND_ID_DUCKLOCK,			IDS_ACTIONSTRING_CROUCHTOGGLE,		COM_INV },
	{ IDS_CONTROL_COMPASS,		   COMMAND_ID_COMPASS,			IDS_ACTIONSTRING_COMPASS,			COM_INV },
	////////////////////////////////////	

	{ IDS_CONTROL_LASTWEAPON,		COMMAND_ID_LASTWEAPON,		IDS_ACTIONSTRING_LASTWEAPON,		COM_INV },
	{ IDS_CONTROL_PREVIOUSWEAPON,	COMMAND_ID_PREV_WEAPON,		IDS_ACTIONSTRING_PREVIOUSWEAPON,	COM_INV },
	{ IDS_CONTROL_NEXTWEAPON,		COMMAND_ID_NEXT_WEAPON,		IDS_ACTIONSTRING_NEXTWEAPON,		COM_INV },


	{ IDS_CONTROL_SAY,				COMMAND_ID_MESSAGE,			IDS_ACTIONSTRING_SAY,				COM_MISC },
	{ IDS_CONTROL_TEAM_SAY,			COMMAND_ID_TEAM_MESSAGE,	IDS_ACTIONSTRING_TEAM_SAY,			COM_MISC },
	{ IDS_CONTROL_RADIO,			COMMAND_ID_RADIO,			IDS_ACTIONSTRING_RADIO,				COM_MISC },
	{ IDS_CONTROL_CENTERVIEW,		COMMAND_ID_RADIO_2,			IDS_ACTIONSTRING_CENTERVIEW,		COM_VIEW },
	{ IDS_CONTROL_LEANLEFT,			COMMAND_ID_RADIO_3,			IDS_ACTIONSTRING_LEANLEFT,			COM_MOVE },

	
	{ IDS_CONTROL_MISSION,			COMMAND_ID_MISSION,			IDS_ACTIONSTRING_MISSION,			COM_MISC },
	{ IDS_CONTROL_INTEL,			COMMAND_ID_SENSITIVITY_LOW,	IDS_ACTIONSTRING_INTEL,				COM_MISC },
	{ IDS_CONTROL_KEYS,				COMMAND_ID_SENSITIVITY_HIGH,IDS_ACTIONSTRING_KEYS,				COM_MISC }
/*	
	{ IDS_CONTROL_FLASHLIGHT,		COMMAND_ID_FLASHLIGHT,		IDS_ACTIONSTRING_FLASHLIGHT,		COM_INV },
	{ IDS_CONTROL_INVENTORY,		COMMAND_ID_INVENTORY,		IDS_ACTIONSTRING_INVENTORY,			COM_MISC },


	// This control must always remain as the last one in the array
//	{ IDS_CONTROL_UNASSIGNED,		COMMAND_ID_UNASSIGNED,		IDS_ACTIONSTRING_UNASSIGNED,		COM_MISC }
*/

//===============================================================================================================
//===============================================================================================================

};




const int g_kNumCommands = sizeof(g_CommandArray) / sizeof(g_CommandArray[0]);

//Called to write out the display.cfg file which holds data for restoring the video
//mode the next time the game is run
void SaveDisplaySettings()
{
	//save out the display configuration
	const char* pszValsToSave[] = {	"ScreenWidth",
									"ScreenHeight",
									"BitDepth",
									"HardwareCursor",
									"VSyncOnFlip",
									"GammaR",
									"GammaG",
									"GammaB"	
								};
	uint32 nNumVals = sizeof(pszValsToSave) / sizeof(pszValsToSave[0]);
	g_pLTClient->WriteConfigFileFields("display.cfg", nNumVals, pszValsToSave);
}

//-------------------------------------------------------------------------------------------
// GetCommandName
//
// Retrieves the command name from a command number
// Arguments:
//		nCommand - command number
// Return:
//		String containing the name of the action
//-------------------------------------------------------------------------------------------

char* GetCommandName(int nCommand)
{
	static char buffer[128];

    uint32 nStringID = 0;

	for (int i=0; i < g_kNumCommands; i++)
	{
		if (g_CommandArray[i].nCommandID == nCommand)
		{
			nStringID = g_CommandArray[i].nActionStringID;
			break;
		}
	}

	if (nStringID)
	{
		LoadString(nStringID,buffer,sizeof(buffer));
	}
	else
	{
		SAFE_STRCPY(buffer, "Error in CommandName()!");
	}

	return buffer;
}

const CommandData* GetCommandData(int nIndex)
{
	if (nIndex < 0 || nIndex >= g_kNumCommands)
		return LTNULL;
	return &g_CommandArray[nIndex];
}

// HKey   : HKEY_LOCAL_MACHINE
// Subkey : "Software\\Gamehi\\Sa"
// Value  : DoesAutoSetting
BOOL Reg_ReadInt( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValue, LPUINT piResult )
{
	HKEY	key;
	DWORD	dwDisp;
	DWORD	Size;
	
	if( RegCreateKeyEx( hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL,
		&key, &dwDisp )	!= ERROR_SUCCESS ) 
	{
		return FALSE;
	}

	Size = sizeof( LONG );

	if( RegQueryValueEx( key, lpValue, 0, NULL, (LPBYTE)piResult, &Size ) != ERROR_SUCCESS )
	{
		return FALSE;
	}

	RegCloseKey( key );
	return TRUE;
}

BOOL Reg_WriteInt( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValue, UINT nData )
{
	HKEY	key;
	DWORD	dwDisp;

	if( RegCreateKeyEx( hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL,
		&key, &dwDisp )	!= ERROR_SUCCESS )
	{
		return FALSE;
	}

	if( RegSetValueEx( key, lpValue, 0, REG_DWORD, (LPBYTE)&nData, sizeof(UINT) ) != ERROR_SUCCESS )
	{
		return FALSE;
	}

	RegCloseKey( key );
	return TRUE;
}


CUserProfile::CUserProfile()
{
	m_sName			= "";		
	m_bInitted		= LTFALSE;
	m_pCryptKey		= LTNULL;

	//controls
    m_bInvertY = LTFALSE;
    m_bMouseLook = LTFALSE;
	m_nInputRate = 0;
	m_nSensitivity = 0;

	m_bAutoCenter = LTFALSE;
	m_nNormalTurn = 15;
	m_nFastTurn = 23;
	m_nLookUp = 25;

	m_nVehicleTurn	= 100;

	m_bUseJoystick = LTFALSE;
	memset(m_nAxis,0,sizeof(m_nAxis));

	//game options
	m_nDifficulty = 1;
    m_nSubtitles  = 0;
	m_bAlwaysRun = LTTRUE;
	m_nLayout = 0;
	m_nHeadBob = 0;
	m_nWeaponSway = 0;
	m_nMsgDur = 0;
	m_bAutoWeaponSwitch = LTFALSE;
	m_bLoadScreenTips = LTFALSE;
	m_bVehicleContour = LTFALSE;

	m_bCrosshair = LTTRUE;
	m_CrosshairR = 0x1F;
	m_CrosshairG = 0xFF;
	m_CrosshairB = 0xFF;
	m_CrosshairA = 0xBF;
	m_nStyle = 0;
    m_bDynamic = 1;


	//sound
	m_nSoundVolume=SOUND_DEFAULT_VOL;
	m_nMusicVolume=MUSIC_DEFAULT_VOL;
	m_bSoundEnable = LTTRUE;
	m_bSoundEnable = LTTRUE;
    m_bSoundQuality=LTFALSE;
    m_bMusicQuality=LTFALSE;


	//display
	m_bHardwareCursor = LTTRUE;
	m_nScreenWidth = 0;
	m_nScreenHeight = 0;
	m_nScreenDepth = 0;

	//performance
	memset(&m_sPerformance,0,sizeof(m_sPerformance));

// SPIKE Add Option Value /////////////////////////////////////////////
	m_dwVideoMem	= LTNULL;
	m_bVSyncOnFlip	= LTFALSE;
	m_bModelShadow	= LTFALSE;
	m_bPolyGrid		= LTFALSE;
	m_bSprite		= LTFALSE;
	m_bTracer		= LTFALSE;
	m_bParticle		= LTFALSE;
	m_bCacheTexture = LTFALSE;
	m_bWeather		= LTFALSE;
	m_bShellCasing	= LTFALSE;
    m_bGore			= LTFALSE;
	m_bNameRadar	= LTFALSE;
	m_bDynaLight	= LTFALSE;
	m_bRejectWhisper = LTFALSE;
	m_bRejectInvite	 = LTFALSE;

	//multi player
// Added By IN'T ==>
// Time : 2004-05-06 오후 6:21:22
	m_nDMPlayerSkin = 0;
// Added By IN'T <==
	m_nCPPlayerModel = 0;
	m_nDMPlayerModel = 0;
	memset(m_nPlayerSkills,0,sizeof(m_nPlayerSkills));
	m_nBandwidthClient = 3;
	m_nBandwidthClientCustom = g_BandwidthClient[m_nBandwidthClient];
	m_nClientPort = DEFAULT_PORT;
	m_nDifficulty = 1;

	m_nSaveVersion = 0;
}

static bool CreatePlayerGuid( char* pszPlayerGuid, int nSize )
{
	// Check inputs.
	if( !pszPlayerGuid )
	{
		ASSERT( false );
		return false;
	}

	// Clear out the result.
	pszPlayerGuid[0] = 0;

	// Create the guid.
	GUID guid;
	if( !SUCCEEDED( CoCreateGuid( &guid )))
		return false;

	// Write the guid into the string.
	_snprintf( pszPlayerGuid, nSize, "%X-%X-%X-%X%X-%X%X%X%X%X%X", guid.Data1, guid.Data2, 
		guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], 
		guid.Data4[5], guid.Data4[6], guid.Data4[7] );
	pszPlayerGuid[nSize-1] = 0;

	if( !pszPlayerGuid[0] )
		return false;

	return true;
}

LTBOOL CUserProfile::Init(const std::string& profileName, LTBOOL bCreate)
{
	std::string fn = GetProfileFile( profileName.c_str( ));
	std::string dfn = PROFILE_DIR;
	dfn += "default.prf";

	g_vtMouseMinSensitivity.Init(g_pLTClient, "MouseSensitivityMin", NULL, 0.0);
	g_vtMouseMaxSensitivity.Init(g_pLTClient, "MouseSensitivityMax", NULL, 100.0);
	g_vtMouseMinInputRate.Init(g_pLTClient, "MouseInputRateMin", NULL, 0.0);
	g_vtMouseMaxInputRate.Init(g_pLTClient, "MouseInputRateMax", NULL, 60.0);

	LTBOOL bRet = LTFALSE;

	AutoSetting();   // 자동 사양조절 옵션

	m_buteMgr.Term();

	if (m_pCryptKey)
	{
		bRet = m_buteMgr.Parse(fn.c_str(), m_pCryptKey);
	}
	else
	{
		bRet = m_buteMgr.Parse(fn.c_str());
	}

	// Handle case where we had a previous profile file.
	if (bRet)
	{
		m_sName = profileName;
	}
	// Handle brand new profile.
	else
	{
		//clear out any old data
		m_buteMgr.Term();

		// No previous profile and not told to create one.
		if (!bCreate)
			return LTFALSE;

		m_sName = profileName;

		if (m_pCryptKey)
		{
			bRet = m_buteMgr.Parse(dfn.c_str(), m_pCryptKey);
		}
		else
		{
			
			bRet = m_buteMgr.Parse(dfn.c_str());
		}

		if (!bRet) 
			return LTFALSE;
	}
	
	// Since we changed our profile, we'll need to re-init the saveloadmgr, which
	// uses the profile name.
	
	Load();

	if (m_buteMgr.Exist("Settings"))
	{
		LoadPerformance(false);//defer applying now, because ApplyPerformance() will do it below
	}
	else
	{
		g_pPerformanceMgr->GetPerformanceOptions(&m_sPerformance);
	}

	ApplyPerformance(false);


	m_bInitted = LTTRUE;

	return LTTRUE;

}

void CUserProfile::Load()
{
	m_nSaveVersion = m_buteMgr.GetInt( "Version", "VersionNumber", 0 );

	LoadControls();
	LoadMultiplayer();
	LoadGameOptions();
	LoadSound(true);
}

void CUserProfile::LoadControls()
{

	char temp[256];
	memset(m_bindings,0,sizeof(m_bindings));
	m_nNumBindings = 0;


	strcpy(s_aTagName,"Bindings");
	for (int c = 0; c < g_kNumCommands; c++)
	{

		int n = m_nNumBindings;
		m_nNumBindings++;

		m_bindings[n].nStringID = g_CommandArray[c].nStringID;
		m_bindings[n].nAction = g_CommandArray[c].nActionStringID;

		for( int nDev = 0; nDev < 2/*NUM_DEVICES*/; nDev++ ) //[SPIKE] 조이스틱은 사용을 안함으로 for문을 dev < 2 로 변경!!! 
		{
			m_bindings[n].nDeviceObjectId[nDev] = 0;
		}

		strcpy(m_bindings[n].strTriggerName[0],"");

		if (g_CommandArray[c].nCommandID < 0)
		{
			strcpy(m_bindings[n].strRealName[0]," ");
		}
		else
		{
			LoadString(m_bindings[n].nAction,s_aAttName,sizeof(s_aAttName));
			m_buteMgr.GetString(s_aTagName, s_aAttName,temp,sizeof(temp));

			char *pTok = strtok(temp,"@");
			if (pTok)
				strcpy(m_bindings[n].strRealName[0],pTok);

			pTok = strtok(LTNULL,"@");
			if (pTok)
				strcpy(m_bindings[n].strRealName[1],pTok);

			pTok = strtok(LTNULL,"@");
			if (pTok)
				strcpy(m_bindings[n].strRealName[2],pTok);
		}

	}

	strcpy(s_aTagName,"Controls");
    m_bInvertY = (LTBOOL)m_buteMgr.GetInt(s_aTagName,"InvertMouse",0);
    m_bMouseLook = (LTBOOL)m_buteMgr.GetInt(s_aTagName,"MouseLook",1);
	m_nInputRate = m_buteMgr.GetInt(s_aTagName,"InputRate",60);
	m_nSensitivity = m_buteMgr.GetInt(s_aTagName,"Sensitivity",5);

	m_nNormalTurn	= m_buteMgr.GetInt(s_aTagName,"NormalTurn",15);
	m_nFastTurn		= m_buteMgr.GetInt(s_aTagName,"FastTurn",23);
	m_nLookUp		= m_buteMgr.GetInt(s_aTagName,"LookUp",25);
	m_bAutoCenter	= (LTBOOL)m_buteMgr.GetInt(s_aTagName,"AutoCenter",0);

	m_nVehicleTurn = m_buteMgr.GetInt(s_aTagName,"VehicleTurnRate",100);

	m_bUseJoystick = (LTBOOL)m_buteMgr.GetInt(s_aTagName,"UseJoystick",0);

	for (int a = 0; a < g_pProfileMgr->GetNumAxis(); a++)
	{
		CDeviceAxisData *pAxisData = g_pProfileMgr->GetAxisData(a);
		uint8 nDef = 0;

		if (pAxisData->m_nType == CONTROLTYPE_YAXIS || pAxisData->m_nType == CONTROLTYPE_RYAXIS)
		{
			nDef = 2;
		}

		char szAxis[8] = "";
		sprintf(szAxis,"Axis%d",a);
		m_nAxis[a] = (uint8)m_buteMgr.GetInt(s_aTagName,szAxis,nDef);
		
	}

	for (a = 0; a < g_pProfileMgr->GetNumPOV(); a++)
	{
		CDevicePOVData *pPOVData = g_pProfileMgr->GetPOVData(a);
		char szPOV[8] = "";
		sprintf(szPOV,"POV%d",a);
		m_nPOV[a] = (uint8)m_buteMgr.GetInt(s_aTagName,szPOV,0);
		
	}

	ApplyControls();
	ApplyBindings();
	ApplyMouse();
	ApplyKeyboard();
//	ApplyJoystick();

}

void CUserProfile::LoadMultiplayer()
{
	strcpy(s_aTagName,"Multiplayer");

	char szStr[64];

	//m_buteMgr.GetString(s_aTagName, "PlayerName",m_sName.c_str(),szStr,sizeof(szStr));
	//m_sPlayerName = szStr;
	m_sPlayerName = g_GameDoc.GetID();

	m_buteMgr.GetString( s_aTagName, "Guid", "", szStr, sizeof( szStr ));
	// If they didn't have a guid, create one.
	if( !m_buteMgr.Success( ))
	{
		CreatePlayerGuid( szStr, ARRAY_LEN( szStr ));
	}
	m_sPlayerGuid = szStr;

	m_nCPPlayerModel = (uint8)m_buteMgr.GetInt(s_aTagName,"PlayerModel",0);
	m_nDMPlayerModel = (uint8)m_buteMgr.GetInt(s_aTagName,"DMPlayerModel",0);

// Added By IN'T ==>
// Time : 2004-05-06 오후 6:22:08
	// 0 or 1
	m_nDMPlayerSkin = (uint8)m_buteMgr.GetInt(s_aTagName,"DMPlayerSkin",0);
// Added By IN'T <==


	//see if the MP skills settings are valid
	sprintf(s_aAttName,"PlayerSkills0");
	int nTest = m_buteMgr.GetInt(s_aTagName,s_aAttName,-1);

	
	if (nTest >= 0)
	{
		//if valid, read 'em in
		for (uint8 i = 0; i < kNumSkills; i++)
		{
			eSkill skl = (eSkill)i;
			if (g_pSkillsButeMgr->IsAvailable(skl) )
			{
				sprintf(s_aAttName,"PlayerSkills%d",i);
				int nDef = (int)g_pSkillsButeMgr->GetDefaultMultiplayerLevel(skl);
				m_nPlayerSkills[i] = (uint8)m_buteMgr.GetInt(s_aTagName,s_aAttName,nDef);
			}
			else
				m_nPlayerSkills[i] = 0;
		}
	}
	else
	{
		//if the first was not valid, get 'em from the SkillsButeMgr
		for (uint8 i = 0; i < kNumSkills; i++)
		{
			eSkill skl = (eSkill)i;
			if (g_pSkillsButeMgr->IsAvailable(skl) )
			{
				m_nPlayerSkills[i] = (uint8)g_pSkillsButeMgr->GetDefaultMultiplayerLevel(skl);
			}
			else
				m_nPlayerSkills[i] = 0;
		}
	}

	m_nBandwidthClient = (uint8)m_buteMgr.GetInt(s_aTagName,"BandwidthClient",3);
	m_nBandwidthClientCustom = (uint16)m_buteMgr.GetInt(s_aTagName,"BandwidthClientCustom",g_BandwidthClient[3]);
	m_nClientPort = (uint16)m_buteMgr.GetInt(s_aTagName,"ClientPort",DEFAULT_PORT);

	m_bMPRadar = m_buteMgr.GetBool( s_aTagName, "MPRadar", true );

	// Set the default properties on the session name and password.
	m_ServerGameOptions.m_sPassword = LoadTempString( IDS_PASSWORD_DEFAULT );
	m_ServerGameOptions.m_sScmdPassword = LoadTempString( IDS_PASSWORD_DEFAULT );

	// Load the options from the bute file.
	if( !m_ServerGameOptions.LoadFromBute( m_buteMgr ))
	{
		ASSERT( !"CUserProfile::LoadMultiplayer: Could not read multiplayer info from profile." );
		return;
	}

	// Make sure the name of the profile is correct.
	m_ServerGameOptions.m_sProfileName = m_sName;

	ApplyMultiplayer(false);
}


void CUserProfile::LoadGameOptions()
{
	strcpy(s_aTagName,"Game");

	m_nDifficulty = m_buteMgr.GetInt(s_aTagName,"Difficulty",2);
    m_nSubtitles  = (uint8)m_buteMgr.GetInt(s_aTagName,"Subtitles",0);
	m_bAlwaysRun = (LTBOOL)m_buteMgr.GetInt(s_aTagName,"AlwaysRun",1);
	m_nLayout = m_buteMgr.GetInt(s_aTagName,"HUDLayout",0);
	m_nHeadBob = m_buteMgr.GetInt(s_aTagName,"HeadBob",0);
	m_nWeaponSway = m_buteMgr.GetInt(s_aTagName,"WeaponSway",0);
	m_nMsgDur = m_buteMgr.GetInt(s_aTagName,"MessageDur",0);
	m_bAutoWeaponSwitch = (LTBOOL)m_buteMgr.GetInt(s_aTagName,"AutoWeaponSwitch",1);
	m_bLoadScreenTips = (LTBOOL)m_buteMgr.GetInt(s_aTagName,"LoadScreenTips",1);
	m_bVehicleContour = (LTBOOL)m_buteMgr.GetInt(s_aTagName,"VehicleContour",1);

	m_bCrosshair = (LTBOOL)m_buteMgr.GetInt(s_aTagName,"UseCrosshair",1);
	m_CrosshairR = (uint32)m_buteMgr.GetInt(s_aTagName,"CrosshairRed",0x1F);
	m_CrosshairG = (uint32)m_buteMgr.GetInt(s_aTagName,"CrosshairGreen",0xFF);
	m_CrosshairB = (uint32)m_buteMgr.GetInt(s_aTagName,"CrosshairBlue",0xFF);
	m_CrosshairA = (uint8)m_buteMgr.GetInt(s_aTagName,"CrosshairAlpha",0xBF);
	m_nStyle = m_buteMgr.GetInt(s_aTagName,"CrosshairStyle",0);
    m_bDynamic = m_buteMgr.GetInt(s_aTagName,"CrosshairDynamic",1);
	
	m_bSPRadar = m_buteMgr.GetBool( s_aTagName, "SPRadar", true );
	
	// 추가 옵션 
	strcpy(s_aTagName,"AutoOptionSetting");
	m_nScreenWidth	= m_buteMgr.GetInt(s_aTagName,"ScreenWidth",800);
	m_nScreenHeight	= m_buteMgr.GetInt(s_aTagName,"ScreenHeight",600);
	m_nScreenDepth	= m_buteMgr.GetInt(s_aTagName,"ScreenBitdepth",32);
	m_bVSyncOnFlip	= m_buteMgr.GetInt(s_aTagName,"VsyncOnFlip",0);
	m_bModelShadow	= m_buteMgr.GetInt(s_aTagName,"ModelShadow",0);
	m_bPolyGrid		= m_buteMgr.GetInt(s_aTagName,"PolyGrid",0);
	m_bSprite		= m_buteMgr.GetInt(s_aTagName,"Sprite",0);
	m_bTracer		= m_buteMgr.GetInt(s_aTagName,"Tracer",0);
	m_bParticle		= m_buteMgr.GetInt(s_aTagName,"SmokeDetail",0);
	m_bCacheTexture	= m_buteMgr.GetInt(s_aTagName,"CacheTexture",0);
	m_bWeather		= m_buteMgr.GetInt(s_aTagName,"Weather",0);
	m_bShellCasing	= m_buteMgr.GetInt(s_aTagName,"ShellCasing",0);
	m_bGore			= m_buteMgr.GetInt(s_aTagName,"Blood",0);
	m_bNameRadar	= m_buteMgr.GetInt(s_aTagName,"NameRadar",0);
	m_bDynaLight	= m_buteMgr.GetInt(s_aTagName,"DynaLight",0);
	m_bRejectWhisper	= m_buteMgr.GetInt(s_aTagName,"RejectWhisper",0);
	m_bRejectInvite		= m_buteMgr.GetInt(s_aTagName,"RejectInvite",0);

	ApplyGameOptions();
	ApplyCrosshair();

}

void CUserProfile::LoadSound(bool bApply)
{		
	strcpy(s_aTagName,"Sound");

	m_nSoundVolume = m_buteMgr.GetInt(s_aTagName,"soundvolume",SOUND_DEFAULT_VOL);
	m_nMusicVolume = m_buteMgr.GetInt(s_aTagName,"musicvolume",MUSIC_DEFAULT_VOL);
	m_bSoundEnable = m_buteMgr.GetInt(s_aTagName,"SoundEnable",1);
	m_bMusicEnable = m_buteMgr.GetInt(s_aTagName,"MusicEnable",1);

	m_fWeaponsSoundMultiplier = m_buteMgr.GetFloat( s_aTagName, "weaponssoundmultiplier", WEAPONS_DEFAULT_MULTIPLIER );
	m_fSpeechSoundMultiplier = m_buteMgr.GetFloat( s_aTagName, "speechsoundmultiplier", SPEECH_DEFAULT_MULTIPLIER );
	m_fDefaultSoundMultiplier = m_buteMgr.GetFloat( s_aTagName, "defaultsoundmultiplier", DEFAULT_DEFAULT_MULTIPLIER );

	m_bSoundQuality = (LTBOOL)m_buteMgr.GetInt(s_aTagName,"sound16bit",0);
	m_bMusicQuality = (LTBOOL)m_buteMgr.GetInt(s_aTagName,"MusicQuality",0);

	uint32 dwAdvancedOptions = g_pInterfaceMgr->GetAdvancedOptions();
	if (!(dwAdvancedOptions & AO_MUSIC))
		m_nMusicVolume = MUSIC_MIN_VOL;
	
	
	if( false == g_pGameClientShell->IsSoundInitted() )
	{
		m_bSoundEnable = LTFALSE;
		m_bMusicEnable = LTFALSE;
	}

	if (bApply)
		ApplySound();

}

void CUserProfile::LoadPerformance(bool bApply)
{

	g_pPerformanceMgr->LoadPerformanceOptions(m_buteMgr,&m_sPerformance);

	if (bApply)
		ApplyPerformance(false);

}


void CUserProfile::Save(bool bForceClose /*= false*/)
{
	m_nSaveVersion = g_pVersionMgr->GetSaveVersion( );
	m_buteMgr.SetInt( "Version","VersionNumber", m_nSaveVersion );

	SaveControls();
	SaveMultiplayer();
	SaveGameOptions();
	SaveSound();
	SavePerformance();

	std::string fn = GetProfileFile( m_sName.c_str( ));
	m_buteMgr.Save(fn.c_str());


	if (bForceClose)
	{
		std::string name = m_sName.c_str( );
		Term();
		Init(name,false);
	}
}

void CUserProfile::SaveControls()
{
	//************************ save bindings
	strcpy(s_aTagName,"Bindings");
	for (int c = 0; c < g_kNumCommands; c++)
	{
		if ( g_CommandArray[c].nCommandID < 0 )
			continue;

		CBindingData *pData = FindBinding(c);
		if (pData)
		{
			LoadString(pData->nAction,s_aAttName,sizeof(s_aAttName));
			
			char szStr[128] = "";
			for (int d = 0; d < 3; d++)
			{
				if (pData->strRealName[d] && pData->strRealName[d][0])
				{
					strcat(szStr,pData->strRealName[d]);
				}
				else
				{
					strcat(szStr," ");
				}
				if (d < 2)
					strcat(szStr,"@");
			}
	
			m_buteMgr.SetString(s_aTagName, s_aAttName, szStr);

		}
	}

	//************************ save controls
	strcpy(s_aTagName,"Controls");

	//save mouse
	m_buteMgr.SetInt(s_aTagName,"InvertMouse",(int)m_bInvertY);
	m_buteMgr.SetInt(s_aTagName,"MouseLook",(int)m_bMouseLook);
    m_buteMgr.SetInt(s_aTagName,"InputRate",m_nInputRate);
    m_buteMgr.SetInt(s_aTagName,"Sensitivity",m_nSensitivity);
	m_buteMgr.SetInt(s_aTagName,"VehicleTurnRate",m_nVehicleTurn);


	m_buteMgr.SetInt(s_aTagName,"NormalTurn",m_nNormalTurn);
	m_buteMgr.SetInt(s_aTagName,"FastTurn",m_nFastTurn);
	m_buteMgr.SetInt(s_aTagName,"LookUp",m_nLookUp);
	m_buteMgr.SetInt(s_aTagName,"AutoCenter",(int)m_bAutoCenter);

	m_buteMgr.SetInt(s_aTagName,"UseJoystick",(int)m_bUseJoystick);

	for (int a = 0; a < g_pProfileMgr->GetNumAxis(); a++)
	{
		char szAxis[8] = "";
		sprintf(szAxis,"Axis%d",a);
		m_buteMgr.SetInt(s_aTagName,szAxis,(int)m_nAxis[a]);
		
	}

}


void CUserProfile::SaveMultiplayer()
{
	strcpy(s_aTagName,"Multiplayer");
	m_buteMgr.SetString(s_aTagName, "PlayerName",m_sPlayerName.c_str());
	m_buteMgr.SetString( s_aTagName, "Guid", m_sPlayerGuid.c_str( ));
	m_buteMgr.SetInt(s_aTagName,"PlayerModel",(int)m_nCPPlayerModel);
	m_buteMgr.SetInt(s_aTagName,"DMPlayerModel",(int)m_nDMPlayerModel);
// Added By IN'T ==>
// Time : 2004-05-06 오후 6:22:44
	m_buteMgr.SetInt(s_aTagName,"DMPlayerSkin",(int)m_nDMPlayerSkin);
// Added By IN'T <==

	for (uint8 i = 0; i < kNumSkills; i++)
	{
		sprintf(s_aAttName,"PlayerSkills%d",i);
		m_buteMgr.SetInt(s_aTagName,s_aAttName,(int)m_nPlayerSkills[i]);
	}

	m_buteMgr.SetInt(s_aTagName,"BandwidthClient",(int)m_nBandwidthClient);
	m_buteMgr.SetInt(s_aTagName,"BandwidthClientCustom",(int)m_nBandwidthClientCustom);
	m_buteMgr.SetInt(s_aTagName,"ClientPort",(int)m_nClientPort);

	m_buteMgr.SetBool( s_aTagName, "MPRadar", m_bMPRadar );

	m_ServerGameOptions.SaveToBute( m_buteMgr );
}

void CUserProfile::SaveGameOptions()
{
	strcpy(s_aTagName,"Game");
	m_buteMgr.SetInt(s_aTagName,"Difficulty",m_nDifficulty);
    m_buteMgr.SetInt(s_aTagName,"Subtitles",m_nSubtitles);
    m_buteMgr.SetInt(s_aTagName,"Gore",(int)m_bGore);
	m_buteMgr.SetInt(s_aTagName,"AlwaysRun",(int)m_bAlwaysRun);
	m_buteMgr.SetInt(s_aTagName,"HUDLayout",m_nLayout);
	m_buteMgr.SetInt(s_aTagName,"HeadBob",m_nHeadBob);
	m_buteMgr.SetInt(s_aTagName,"WeaponSway",m_nWeaponSway);
	m_buteMgr.SetInt(s_aTagName,"MessageDur",m_nMsgDur);
	m_buteMgr.SetInt(s_aTagName,"AutoWeaponSwitch",(int)m_bAutoWeaponSwitch);
	m_buteMgr.SetInt(s_aTagName,"LoadScreenTips",(int)m_bLoadScreenTips);
	m_buteMgr.SetInt(s_aTagName,"VehicleContour",(int)m_bVehicleContour);

	
	m_buteMgr.SetInt(s_aTagName,"UseCrosshair",m_bCrosshair);
	m_buteMgr.SetInt(s_aTagName,"CrosshairRed",(int)m_CrosshairR);
	m_buteMgr.SetInt(s_aTagName,"CrosshairGreen",(int)m_CrosshairG);
	m_buteMgr.SetInt(s_aTagName,"CrosshairBlue",(int)m_CrosshairB);
	m_buteMgr.SetInt(s_aTagName,"CrosshairAlpha",(int)m_CrosshairA);
	m_buteMgr.SetInt(s_aTagName,"CrosshairStyle",m_nStyle);
    m_buteMgr.SetInt(s_aTagName,"CrosshairDynamic",m_bDynamic);

	m_buteMgr.SetBool( s_aTagName, "SPRadar", m_bSPRadar );

	// 추가 옵션 
	strcpy(s_aTagName,"AutoOptionSetting");
	m_buteMgr.SetInt(s_aTagName,"ScreenWidth",m_nScreenWidth);
	m_buteMgr.SetInt(s_aTagName,"ScreenHeight",m_nScreenHeight);
    m_buteMgr.SetInt(s_aTagName,"ScreenBitdepth",m_nScreenDepth);
	m_buteMgr.SetInt(s_aTagName,"VsyncOnFlip",m_bVSyncOnFlip);
	m_buteMgr.SetInt(s_aTagName,"ModelShadow",m_bModelShadow);
	m_buteMgr.SetInt(s_aTagName,"PolyGrid",m_bPolyGrid);
	m_buteMgr.SetInt(s_aTagName,"Sprite",m_bSprite);
	m_buteMgr.SetInt(s_aTagName,"Tracer",m_bTracer);
	m_buteMgr.SetInt(s_aTagName,"SmokeDetail",m_bParticle);
	m_buteMgr.SetInt(s_aTagName,"CacheTexture",m_bCacheTexture);
	m_buteMgr.SetInt(s_aTagName,"Weather",m_bWeather);
	m_buteMgr.SetInt(s_aTagName,"ShellCasing",m_bShellCasing);
	m_buteMgr.SetInt(s_aTagName,"Blood",m_bGore);
	m_buteMgr.SetInt(s_aTagName,"NameRadar",m_bNameRadar);
	m_buteMgr.SetInt(s_aTagName,"DynaLight",m_bDynaLight);
	m_buteMgr.SetInt(s_aTagName,"RejectWhisper",m_bRejectWhisper);
	m_buteMgr.SetInt(s_aTagName,"RejectInvite",m_bRejectInvite);
	
}


void CUserProfile::SaveSound()
{
	strcpy(s_aTagName,"Sound");

	m_buteMgr.SetInt(s_aTagName,"soundvolume",m_nSoundVolume);
	m_buteMgr.SetInt(s_aTagName,"musicvolume",m_nMusicVolume);
	m_buteMgr.SetInt(s_aTagName,"SoundEnable",m_bSoundEnable);
	m_buteMgr.SetInt(s_aTagName,"MusicEnable",m_bMusicEnable);
	m_buteMgr.SetFloat(s_aTagName,"defaultsoundmultiplier",m_fDefaultSoundMultiplier);
	m_buteMgr.SetFloat(s_aTagName,"weaponssoundmultiplier", m_fWeaponsSoundMultiplier);
	m_buteMgr.SetFloat(s_aTagName,"speechsoundmultiplier", m_fSpeechSoundMultiplier);
	
	m_buteMgr.SetInt(s_aTagName,"sound16bit",(int)m_bSoundQuality);
	m_buteMgr.SetInt(s_aTagName,"MusicQuality",(int)m_bMusicQuality);
}

void CUserProfile::SavePerformance()
{
	g_pPerformanceMgr->SavePerformanceOptions(m_buteMgr,&m_sPerformance);
}

void CUserProfile::Term()
{
	m_buteMgr.Term();
	m_bInitted = LTFALSE;
}

CBindingData* CUserProfile::FindBinding(int commandIndex)
{
	int n = 0;
	while (n < m_nNumBindings && m_bindings[n].nAction != g_CommandArray[commandIndex].nActionStringID)
		n++;

	if (n >= m_nNumBindings)
		return LTNULL;
	return &m_bindings[n];
}

char const* CUserProfile::GetDeviceName( uint32 nDeviceIndex )
{
	if( nDeviceIndex > 2 )
		return NULL;

	return strDeviceName[nDeviceIndex];
}


//**************************************************************************
//functions to read settings from the profile and write them to the console
//**************************************************************************

	//take bindings from profile and apply them to the game
void CUserProfile::ApplyBindings()
{
	g_pProfileMgr->ClearBindings();

	for (int d = 0; d < 2/*3*/; d++) //[SPIKE] 조이스틱은 사용을 안함으로 for문을 dev < 2 로 변경!!! 
	{
		CBindingData *pWheelUp = LTNULL;
		CBindingData *pWheelDown = LTNULL;
		for (int c = 0; c < g_kNumCommands; c++)
		{
			CBindingData *pData = FindBinding(c);
			if (pData)
			{

				if (devices[d] == DEVICETYPE_MOUSE && stricmp(pData->strRealName[d],"#U") == 0)
				{
					pWheelUp = pData;
				}
				else if (devices[d] == DEVICETYPE_MOUSE && stricmp(pData->strRealName[d],"#D") == 0)
				{
					pWheelDown = pData;
				}
				else if (pData->strRealName[d][0] && pData->strRealName[d][0] != ' ')
				{
					char tempStr[64];
					LoadString(pData->nAction,tempStr,sizeof(tempStr));

					// Set the binding
					g_pLTClient->AddBinding(strDeviceName[d], pData->strRealName[d],tempStr,0.0f,0.0f);

				}
			}


		}
		if (devices[d] == DEVICETYPE_MOUSE)
		{
			char tempStr[512] = "";
			if (pWheelUp || pWheelDown)
			{
				// Set the mouse wheel binding
				char upStr[64] = "";
				if (pWheelUp)
				{
					LoadString(pWheelUp->nAction,upStr,sizeof(upStr));
				}
				char downStr[64] = "";
				if (pWheelDown)
				{
					LoadString(pWheelDown->nAction,downStr,sizeof(downStr));
				}

				g_pLTClient->AddBinding(strDeviceName[d],"##z-axis",upStr,0.10f,255.0f);
				g_pLTClient->AddBinding(strDeviceName[d],"##z-axis",downStr,-0.10f,-255.0f);
			}

		}
				
	}
}


void CUserProfile::ApplyMouse()
{
	WriteConsoleInt("MouseInvertY",m_bInvertY);
	WriteConsoleInt("MouseLook",m_bMouseLook);
	WriteConsoleFloat("MouseSensitivity",(float)m_nSensitivity);
	WriteConsoleFloat("InputRate",(float)m_nInputRate);

	float fTemp = (float)m_nVehicleTurn / 100.0f;
	WriteConsoleFloat("VehicleTurnRateScale",fTemp);

	CGameSettings* pSettings = g_pInterfaceMgr->GetSettings();
	pSettings->ImplementMouseSensitivity();
}

void CUserProfile::ApplyKeyboard()
{
	if (!m_bMouseLook)
		WriteConsoleInt("AutoCenter",(int)m_bAutoCenter);

	float fTemp = (float)m_nNormalTurn / 10.0f;
	WriteConsoleFloat("NormalTurnRate",fTemp);

	fTemp = (float)m_nFastTurn / 10.0f;
	WriteConsoleFloat("FastTurnRate",fTemp);

	fTemp = (float)m_nLookUp / 10.0f;
	WriteConsoleFloat("LookUpRate",fTemp);

	fTemp = (float)m_nVehicleTurn / 100.0f;
	WriteConsoleFloat("VehicleTurnRateScale",fTemp);
}

void CUserProfile::ApplyMultiplayer(bool bLAN)
{
	// Use max on LAN games
	float fMinBandwidth = (bLAN) ? k_fMaxBandwidth : 0.0f;

	//set mp console vars here
	if (m_nBandwidthClient < eBandwidth_Custom)
		WriteConsoleFloat("BandwidthTargetClient",LTCLAMP(((float)g_BandwidthClient[m_nBandwidthClient] * 1024.0f), fMinBandwidth, k_fMaxBandwidth) );
	else
		WriteConsoleFloat("BandwidthTargetClient",LTCLAMP(((float)m_nBandwidthClientCustom * 1024.0f), fMinBandwidth, k_fMaxBandwidth) );

}

void CUserProfile::ApplyGameOptions()
{
	WriteConsoleInt("Gore", m_bGore);
	WriteConsoleInt("Subtitles",m_nSubtitles);
	WriteConsoleInt("HUDLayout",m_nLayout);
	WriteConsoleInt("Difficulty",m_nDifficulty);

	GameDifficulty eDiff = GD_NORMAL;
	if (IsDifficultyGameType())
	{
		eDiff = (GameDifficulty)m_nDifficulty;
	}
	g_pGameClientShell->SetDifficulty(eDiff);

	

	WriteConsoleFloat("HeadBob",((LTFLOAT)m_nHeadBob / 10.0f));
	WriteConsoleFloat("WeaponSway",((LTFLOAT)m_nWeaponSway / 10.0f));
	WriteConsoleFloat("MessageDuration",((LTFLOAT)m_nMsgDur / 10.0f));

	WriteConsoleInt("AutoWeaponSwitch",(int)m_bAutoWeaponSwitch);
	WriteConsoleInt("VehicleContour",(int)m_bVehicleContour);

	g_pMoveMgr->SetRunLock(m_bAlwaysRun);

	g_pGameClientShell->UpdateGoreSettings();

//[SPIKE]==========================================================
// 자동 옵션으로 적용될 콘솔 변수 
	WriteConsoleInt("LightMap", 1);
	WriteConsoleInt("SoundFilters", m_bVSyncOnFlip);
	WriteConsoleInt("ModelShadow_Proj_Enable", m_bModelShadow);
	WriteConsoleInt("DrawPolyGrids", m_bPolyGrid);
	WriteConsoleInt("Trilinear", 0);
	WriteConsoleInt("DrawSprites", m_bSprite);
	WriteConsoleInt("Tracers", m_bTracer);

	WriteConsoleInt("DrawParticles", 1/* m_bParticle*/);
	WriteConsoleInt("SmokeGrenadeDetail", m_bParticle);

	WriteConsoleInt("CacheTexture", m_bCacheTexture);
	
//	WriteConsoleInt("Weather", m_bWeather);
	WriteConsoleInt("Gore", m_bGore);
	WriteConsoleInt("ShellCasings", m_bShellCasing);
	WriteConsoleInt("NameRadar", m_bNameRadar);
	WriteConsoleInt("DynamicLightWorld", m_bDynaLight);
	WriteConsoleInt("RejectWhisper", m_bRejectWhisper);
	WriteConsoleInt("RejectInvite", m_bRejectInvite);
//[SPIKE]==========================================================
}

void CUserProfile::ApplyCrosshair()
{
    WriteConsoleInt("CrosshairRed",m_CrosshairR );
    WriteConsoleInt("CrosshairGreen",m_CrosshairG );
    WriteConsoleInt("CrosshairBlue",m_CrosshairB );
    WriteConsoleInt("CrosshairAlpha",m_CrosshairA );

    WriteConsoleInt("CrosshairStyle", m_nStyle);
	WriteConsoleInt("CrosshairDynamic", m_bDynamic );
	g_pInterfaceMgr->EnableCrosshair(m_bCrosshair);
}

void CUserProfile::ApplySound()
{
	WriteConsoleInt("soundvolume",m_nSoundVolume);
	WriteConsoleInt("soundenable",m_bSoundEnable);
	WriteConsoleInt("musicenable",m_bMusicEnable);
	WriteConsoleFloat( "weaponssoundmultiplier", m_fWeaponsSoundMultiplier );
	WriteConsoleFloat( "defaultsoundmultiplier", m_fDefaultSoundMultiplier );
	WriteConsoleFloat( "speechsoundmultiplier", m_fSpeechSoundMultiplier );
	ImplementSoundVolume();

	LTBOOL bOldSoundQuality = GetConsoleInt("sound16bit",0);
	WriteConsoleInt("sound16bit",(int)m_bSoundQuality);
	if (m_bSoundQuality != bOldSoundQuality)
	{
		g_pGameClientShell->InitSound();
	}

	// Set the music quality...

	WriteConsoleInt("MusicQuality",(int)m_bMusicQuality);
	g_pGameClientShell->GetMusic()->SetQuality(!!m_bMusicQuality);	

	//if we haven't gotten to the menus yet, don't start the music yet...
	if (g_pInterfaceMgr->HasEnteredScreenState())
	{
		uint32 dwAdvancedOptions = g_pInterfaceMgr->GetAdvancedOptions();
		LTBOOL bMusicOn = (m_nMusicVolume > MUSIC_MIN_VOL) && (dwAdvancedOptions & AO_MUSIC) && GetConsoleInt("MusicActive",0);
		WriteConsoleInt("MusicActive",(int)bMusicOn);
		if (bMusicOn != g_pGameClientShell->GetMusic()->IsPlaying())
		{
			if (bMusicOn)
			{
				// [KLS 8/11/02] SetupMusic will handle turning on the
				// music and making sure it plays the appropriate music.
				g_pInterfaceMgr->SetupMusic();
			}
			else 
			{
				g_pGameClientShell->GetMusic()->Term();

			}
		}
	}
	
	ReSetBGMVolumeRatio(((float)m_nMusicVolume)/100);

	ImplementMusicVolume();
}


//[SPIKE] BGM 볼륨ReSet
VOID		
CUserProfile::ReSetBGMVolumeRatio(LTFLOAT fVolumeRatio)
{
	ILTClientSoundMgr* pSndMgr;
	pSndMgr = (ILTClientSoundMgr*)g_pLTClient->SoundMgr();
	pSndMgr->SetSoundClassMultiplier( BGM_SOUND_CLASSES, fVolumeRatio );
	pSndMgr->UpdateVolumeSettings();
	///////////////////////////////////////
}

void CUserProfile::ImplementSoundVolume()
{
	((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->SetVolume(m_nSoundVolume);

	// set up the sound volume classes for the game
	((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->SetSoundClassMultiplier( WEAPONS_SOUND_CLASS, m_fWeaponsSoundMultiplier );
	((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->SetSoundClassMultiplier( SPEECH_SOUND_CLASS, m_fSpeechSoundMultiplier, false );
	((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->SetSoundClassMultiplier( DEFAULT_SOUND_CLASS, m_fDefaultSoundMultiplier );

	// now have the sound manager update any playing sounds
	((ILTClientSoundMgr*)g_pLTClient->SoundMgr())->UpdateVolumeSettings();
}


void CUserProfile::ImplementMusicVolume()
{
	WriteConsoleInt("musicvolume",m_nMusicVolume);
	g_pGameClientShell->GetMusic()->SetMenuVolume(m_nMusicVolume);
}


void CUserProfile::ApplyDisplay()
{
	WriteConsoleInt("HardwareCursor",m_bHardwareCursor);

	WriteConsoleFloat("GammaR",m_fGamma);
	WriteConsoleFloat("GammaG",m_fGamma);
	WriteConsoleFloat("GammaB",m_fGamma);

	//make sure to save out these changed settings so that they will be applied next
	//time we run
	SaveDisplaySettings();

// [SPIKE]==========================================================
// 자동 옵션으로 적용될 콘솔 변수 
	WriteConsoleInt("LightMap", 1);
	WriteConsoleInt("SoundFilters", m_bVSyncOnFlip);
	WriteConsoleInt("ModelShadow_Proj_Enable", m_bModelShadow);
	WriteConsoleInt("DrawPolyGrids", m_bPolyGrid);
	WriteConsoleInt("Trilinear", 0);
	WriteConsoleInt("DrawSprites", m_bSprite);
	WriteConsoleInt("Tracers", m_bTracer);
	WriteConsoleInt("DrawParticles", 1 /*m_bParticle*/);
	WriteConsoleInt("SmokeGrenadeDetail", m_bParticle);
	WriteConsoleInt("CacheTexture", m_bCacheTexture);

//	WriteConsoleInt("Weather", m_bWeather);
	WriteConsoleInt("Gore", m_bGore);
	WriteConsoleInt("ShellCasings", m_bShellCasing);
	WriteConsoleInt("NameRadar", m_bNameRadar);
	WriteConsoleInt("DynamicLightWorld", m_bDynaLight);
	WriteConsoleInt("RejectWhisper", m_bRejectWhisper);
	WriteConsoleInt("RejectInvite", m_bRejectInvite);
}
// Resolution Apply
void CUserProfile::ApplyResolution()
{
	RMode currentMode;
	if (g_pGameClientShell->IsRendererInitted() && g_pLTClient->GetRenderMode(&currentMode) == LT_OK)
	{
		// Build the list of render modes
		RMode *pRenderModes=g_pLTClient->GetRenderModes();
		RMode *pMode=pRenderModes;
		LTBOOL bFound = LTFALSE;
		while (pMode != LTNULL && !bFound)
		{
			bFound = (	pMode->m_Width == m_nScreenWidth &&
						pMode->m_Height == m_nScreenHeight &&
						pMode->m_BitDepth == m_nScreenDepth &&
						pMode->m_bHWTnL == currentMode.m_bHWTnL);
			if (!bFound)
				pMode = pMode->m_pNext;
		}


		if (pMode && !IsRendererEqual(pMode,&currentMode))
		{
			//switch renderers
			g_Network.SendPing();
			g_pLTClient->CPrint( "[CHANGE_MODE] 1. ApplyResolution: %d", GetTickCount() );
			
			g_pLTClient->SetRenderMode(pMode);

			g_Network.SendPing();
			g_pLTClient->CPrint( "[CHANGE_MODE] 2. ApplyResolution: %d", GetTickCount() );

			g_pInterfaceMgr->ScreenDimsChanged();

			//update the game camera to have the new dimensions
			g_pLTClient->SetCameraRect(g_pGameClientShell->GetPlayerMgr()->GetCamera(), LTTRUE, 0, 0, pMode->m_Width, pMode->m_Height);

//			if (bRestart)
//				g_pLTClient->CPrint("CUserProfile::ApplyDisplay() : cancel restart because of mode change");
//			bRestart = false;
		}

		// Free the linked list of render modes
		g_pLTClient->RelinquishRenderModes(pRenderModes);
	}
	//make sure to save out these changed settings so that they will be applied next
	//time we run
//	SaveDisplaySettings();
}



void CUserProfile::SetPerformanceCfg(char *pszCfg)
{
	if (!pszCfg || !pszCfg[0]) return;
	
	g_pPerformanceMgr->SetPerformanceCfg(pszCfg);
	g_pPerformanceMgr->GetPerformanceOptions(&m_sPerformance);
	SavePerformance();
	ApplyPerformance(false);
}

void CUserProfile::ApplyPerformance(bool bForceDisplay)
{
	//[MURSUM]==============================
	// 게임 시작시 모드 변경을 위해...
	return;
	//======================================
	g_pPerformanceMgr->SetPerformanceOptions(&m_sPerformance);

	//other settings affected by performance
	//if we are going to force the display resolution to match out overall performance level,
	// we should ignore the current resolution when determining what our level is.
	uint8 nOverall = g_pPerformanceMgr->GetPerformanceCfg(bForceDisplay);
	SetSound();
	SetDisplay();

	g_pLTClient->CPrint("CUserProfile::ApplyPerformance(%d) : nOverall = %d",bForceDisplay,nOverall);
	switch (nOverall)
	{
	case 0:
		if (m_nScreenWidth != 640)
		{
			m_nScreenWidth  = 640;
			m_nScreenHeight = 480;
			ApplyDisplay();
		}
		break;
	case 1:
		if (m_nScreenWidth != 800)
		{
			m_nScreenWidth  = 800;
			m_nScreenHeight = 600;
			ApplyDisplay();
		}

		break;

	case 2:
		if (m_nScreenWidth < 1024)
		{
			m_nScreenWidth  = 1024;
			m_nScreenHeight = 768;
			ApplyDisplay();
		}
		break;

	}
	ApplySound();

	if (g_pLTClient->IsConnected())
		SendPerformanceMsg();
}


void CUserProfile::SendPerformanceMsg()
{
	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_PERFORMANCE );
	cMsg.Writeuint8( m_sPerformance.nSettings[kPerform_EnvironmentalDetail] );
	cMsg.Writebool( (m_sPerformance.nSettings[kPerform_PreCacheAssets] > 0) );
    g_pLTClient->SendToServer( cMsg.Read(), MESSAGE_GUARANTEED );

}


void CUserProfile::ApplyControls()
{

	WriteConsoleInt("UseJoystick",(int)m_bUseJoystick);
}

void CUserProfile::ApplyJoystick()
{
	uint32 devType = DEVICETYPE_UNKNOWN;
	if (g_pGameClientShell->HasGamepad())
		devType = DEVICETYPE_GAMEPAD;
	else if (g_pGameClientShell->HasJoystick())
		devType = DEVICETYPE_JOYSTICK;

	//clear all joystick actions bound to an axis
    DeviceBinding* pBindings = g_pLTClient->GetDeviceBindings (devType);
	//step through each binding
	DeviceBinding* ptr = pBindings;
	while (ptr)
	{
		uint32 contType = g_pProfileMgr->GetControlType(devType, ptr->m_nObjectId );
		if (contType != CONTROLTYPE_BUTTON && contType != CONTROLTYPE_KEY)							 
		{
			g_pLTClient->ClearBinding(ptr->strDeviceName, ptr->strRealName);
		}
		ptr = ptr->pNext;
	}

    g_pLTClient->FreeDeviceBindings (pBindings);

	if (!m_bUseJoystick)
	{
		WriteConsoleInt("UseJoystick",0);
		return;
	}
	
	WriteConsoleInt("UseJoystick",1);

	char strJoystick[128];
	memset(strJoystick, 0, 128);
	LTRESULT result = g_pLTClient->GetDeviceName(devType, strJoystick, 127);
	if (result != LT_OK) return;

	for (int a = 0; a < g_pProfileMgr->GetNumAxis(); a++)
	{
		CDeviceAxisData *pAxisData = g_pProfileMgr->GetAxisData(a);
		if (!pAxisData || m_nAxis[a] == eJoystick_None) continue;

		// figure out the intermediate range information
		float fRange = pAxisData->m_fRangeHigh - pAxisData->m_fRangeLow;
		float fActiveRange = fRange * 0.45f;

		char *pszHigh = NULL;
		char *pszLow = NULL;

		switch (pAxisData->m_nType)
		{
			case CONTROLTYPE_XAXIS:
			case CONTROLTYPE_RXAXIS:
			case CONTROLTYPE_ZAXIS:
				if (m_nAxis[a] == eJoystick_Look)
				{
					pszHigh = szJoyRight;
					pszLow = szJoyLeft;
				}
				else if (m_nAxis[a] == eJoystick_Move)
				{
					pszHigh = szJoyStrafeRight;
					pszLow = szJoyStrafeLeft;
				}
				break;
			case CONTROLTYPE_YAXIS:
			case CONTROLTYPE_RYAXIS:
			case CONTROLTYPE_RZAXIS:
				if (m_nAxis[a] == eJoystick_Look)
				{
					pszHigh = szJoyLookDown;
					pszLow = szJoyLookUp;
				}
				else if (m_nAxis[a] == eJoystick_Move)
				{
					pszHigh = szJoyBackward;
					pszLow = szJoyForward;
				}
				else if (m_nAxis[a] == eJoystick_LookInvert)
				{
					pszHigh = szJoyLookUp;
					pszLow = szJoyLookDown;
				}
				break;
		}
		if (pszHigh && pszLow)
		{
			g_pLTClient->AddBinding(strJoystick, pAxisData->m_sName,pszLow,pAxisData->m_fRangeLow,pAxisData->m_fRangeLow+fActiveRange);
			g_pLTClient->AddBinding(strJoystick, pAxisData->m_sName,pszHigh,pAxisData->m_fRangeHigh-fActiveRange,pAxisData->m_fRangeHigh);
		}
	}

	for (a = 0; a < g_pProfileMgr->GetNumPOV(); a++)
	{
		CDevicePOVData *pPOVData = g_pProfileMgr->GetPOVData(a);
		if (!pPOVData || m_nPOV[a] == eJoystick_None) continue;

		char *pszUp = NULL;
		char *pszLeft = NULL;
		char *pszDown = NULL;
		char *pszRight = NULL;

		if (m_nPOV[a] == eJoystick_Look)
		{
			pszUp = szJoyLookUp;
			pszLeft = szJoyLeft;
			pszDown = szJoyLookDown;
			pszRight = szJoyRight;
			
		}
		else if (m_nPOV[a] == eJoystick_Move)
		{
			pszUp = szJoyForward;
			pszLeft = szJoyStrafeLeft;
			pszDown = szJoyBackward;
			pszRight = szJoyStrafeRight;
			
		}

		if (pszUp)
		{
			g_pLTClient->AddBinding(strJoystick, pPOVData->m_sName,pszUp,0.0f,4500.0f);
			g_pLTClient->AddBinding(strJoystick, pPOVData->m_sName,pszRight,4500.0f,13500.0f);
			g_pLTClient->AddBinding(strJoystick, pPOVData->m_sName,pszDown,13500.0f,22500.0f);
			g_pLTClient->AddBinding(strJoystick, pPOVData->m_sName,pszLeft,22500.0f,31500.0f);
			g_pLTClient->AddBinding(strJoystick, pPOVData->m_sName,pszUp,31500.0f,36000.0f);

		}
	}

}


//**************************************************************************
//functions to read settings from the console and write them to the profile
//**************************************************************************

//read bindings from the game and save them in the profile
void CUserProfile::SetBindings()
{
	uint16 nTestCount = 0;
 	for (int dev = 0; dev < 2/*3*/; ++dev) //[SPIKE] 조이스틱은 사용을 안함으로 for문을 dev < 2 로 변경!!! 
	{
		for (int i = 0; i < m_nNumBindings; i++)
		{
			SAFE_STRCPY (m_bindings[i].strRealName[dev], "");
			SAFE_STRCPY (m_bindings[i].strTriggerName[dev], "");
			m_bindings[i].nCode = 0;
			m_bindings[i].nDeviceObjectId[dev] = 0;
		}
        uint32 devType = devices[dev];
        DeviceBinding* pBindings = g_pLTClient->GetDeviceBindings (devType);
		if (!pBindings)
		{
			continue;
		}
		DeviceBinding* ptr = pBindings;
		while (ptr)
		{
			GameAction* pAction = ptr->pActionHead;
			while (pAction)
			{
				int com = 0;
				while (com < g_kNumCommands && (pAction->nActionCode != g_CommandArray[com].nCommandID) )
					com++;

				CBindingData *pData = FindBinding(com);
				if (pData)
				{
					uint32 contType = g_pProfileMgr->GetControlType(devType, ptr->m_nObjectId );

					pData->nDeviceObjectId[dev] = ptr->m_nObjectId;

					if (contType == CONTROLTYPE_BUTTON || contType == CONTROLTYPE_KEY)							 
					{
						SAFE_STRCPY (pData->strTriggerName[dev], ptr->strTriggerName);
						SAFE_STRCPY (pData->strRealName[dev], ptr->strRealName);
					}
					else if (devType == DEVICETYPE_MOUSE && contType == CONTROLTYPE_ZAXIS)
					{
						if (pAction->nRangeHigh > 0)
						{
							strcpy(pData->strRealName[dev],"#U");
							strcpy(pData->strTriggerName[dev],szWheelUp);
						}
						else if (pAction->nRangeHigh < 0)
						{
							strcpy(pData->strRealName[dev],"#D");
							strcpy(pData->strTriggerName[dev],szWheelDown);
						}
					}
				}
	
				pAction = pAction->pNext;
			}

			ptr = ptr->pNext;
		}
        g_pLTClient->FreeDeviceBindings (pBindings);
	}


}

void CUserProfile::SetMouse()
{
	int nMin, nMax;

	m_bMouseLook = GetConsoleInt("MouseLook",1);
	m_bInvertY = GetConsoleInt("MouseInvertY",0);

	m_nInputRate = GetConsoleInt("InputRate",20);
	
	nMin = int(g_vtMouseMinInputRate.GetFloat());
	nMax = int(g_vtMouseMaxInputRate.GetFloat());

	if (m_nInputRate < nMin) m_nInputRate = nMin;
	if (m_nInputRate > nMax) m_nInputRate = nMax;

	m_nSensitivity = GetConsoleInt("MouseSensitivity",5);

	nMin = int(g_vtMouseMinSensitivity.GetFloat());
	nMax = int(g_vtMouseMaxSensitivity.GetFloat());

//	m_nSensitivity = 0.1f;
	if (m_nSensitivity < nMin) m_nSensitivity = nMin;
	if (m_nSensitivity > nMax) m_nSensitivity = nMax;

	float fTemp = GetConsoleFloat("VehicleTurnRateScale",1.0f);
	m_nVehicleTurn = (int)(100.0f * fTemp);
}

void CUserProfile::SetKeyboard()
{
	//keyboard settings
	if (m_bMouseLook)
	{
		m_bAutoCenter = LTFALSE;
	}
	else
		m_bAutoCenter = (LTBOOL)GetConsoleInt("AutoCenter",0);

	float fTemp = GetConsoleFloat("NormalTurnRate",1.5f);
	m_nNormalTurn = (int)(10.0f * fTemp);

	fTemp = GetConsoleFloat("FastTurnRate",2.3f);
	m_nFastTurn = (int)(10.0f * fTemp);

	fTemp = GetConsoleFloat("LookUpRate",2.5f);
	m_nLookUp = (int)(10.0f * fTemp);

}

void CUserProfile::SetControls()
{
	if (g_pProfileMgr->GetNumAxis() < 1 && g_pProfileMgr->GetNumPOV() < 1)
	{
		m_bUseJoystick = 0;
		return;
	}
	m_bUseJoystick = (LTBOOL)m_buteMgr.GetInt("Controls","UseJoystick",0);
}

void CUserProfile::SetJoystick()
{
}


void CUserProfile::SetMultiplayer()
{

}

void CUserProfile::SetGameOptions()
{
	m_bGore = (LTBOOL)GetConsoleInt("Gore",0);
	m_nDifficulty = g_pGameClientShell->GetDifficulty();
	m_nSubtitles = GetConsoleInt("Subtitles",0);

	m_bAlwaysRun = (LTBOOL)g_pMoveMgr->RunLock();

	m_nLayout = GetConsoleInt("HUDLayout",0);

	m_nHeadBob = (int)(10.0f * GetConsoleFloat("HeadBob",1.0f));
	m_nWeaponSway = (int)(10.0f * GetConsoleFloat("WeaponSway",1.0f));
	m_nMsgDur = (int)(10.0f * GetConsoleFloat("MessageDuration",1.0f));

	m_bAutoWeaponSwitch = (LTBOOL)GetConsoleInt("AutoWeaponSwitch",1);
	m_bVehicleContour = (LTBOOL)GetConsoleInt("VehicleContour",1);
}


void CUserProfile::SetCrosshair()
{
	m_bCrosshair = g_pInterfaceMgr->IsCrosshairEnabled();

	m_CrosshairR = GetConsoleInt("CrosshairRed",0x1F);
	m_CrosshairG = GetConsoleInt("CrosshairGreen",0xFF);
	m_CrosshairB = GetConsoleInt("CrosshairBlue",0xFF);
	m_CrosshairA = GetConsoleInt("CrosshairAlpha",0xBF);

	m_nStyle = GetConsoleInt("CrosshairStyle",0);
	m_bDynamic = GetConsoleInt("CrosshairDynamic",1);
}

void CUserProfile::SetSound()
{
	m_nSoundVolume = GetConsoleInt("soundvolume",SOUND_DEFAULT_VOL);
	m_nMusicVolume = GetConsoleInt("musicvolume",MUSIC_DEFAULT_VOL);

	m_bSoundEnable = GetConsoleInt("soundenable",1);
	m_bMusicEnable = GetConsoleInt("musicenable",1);

	m_fWeaponsSoundMultiplier = GetConsoleFloat("weaponssoundmultiplier",WEAPONS_DEFAULT_MULTIPLIER);
	m_fSpeechSoundMultiplier = GetConsoleFloat("speechsoundmultiplier",SPEECH_DEFAULT_MULTIPLIER);
	m_fDefaultSoundMultiplier = GetConsoleFloat("defaultsoundmultiplier", DEFAULT_DEFAULT_MULTIPLIER);

	m_bSoundQuality = (LTBOOL)GetConsoleInt("sound16bit",0);
	m_bMusicQuality = (LTBOOL)GetConsoleInt("MusicQuality",0);

//[SPIKE] BGM을 안켜도 profile에 있는 값 세팅 ///////////////
//	if (!GetConsoleInt("MusicEnable",0) || !GetConsoleInt("MusicActive",0))
//		m_nMusicVolume = MUSIC_MIN_VOL;
///////////////////////////////////////////////////////////
	//TODO: deal with m_bMusicQuality
}

void CUserProfile::SetDisplay()
{
	m_bHardwareCursor = (GetConsoleInt("HardwareCursor",0) > 0 && GetConsoleInt("DisableHardwareCursor",0) == 0);

	//get the average
	m_fGamma = (GetConsoleFloat("GammaR",1.0f) + GetConsoleFloat("GammaG",1.0f) + GetConsoleFloat("GammaB",1.0f)) / 3.0f;
	
	// The current render mode
	RMode currentMode;
	g_pLTClient->GetRenderMode(&currentMode);
}

void CUserProfile::SetPerformance()
{
	g_pPerformanceMgr->GetPerformanceOptions(&m_sPerformance);
	m_bMusicQuality = (LTBOOL)GetConsoleInt("MusicQuality",0);
}

int8
CUserProfile::GetHardWareGrade()
{
	uint8 nGrade = 0;
	CSystemInfo si;

	m_szVGAName[0] = LTNULL;
//VGA NAME =====================================
	si.GetVgaInfo( m_szVGAName, "DISPLAY" );
	m_buteMgr.SetString(s_aTagName, "VGAFullName",m_szVGAName);
	//////////////////////
	/// 그래픽카드사 계열검색 및 스크립트에 세팅 
	char szVGA_CompanyName[64] = {0,};
	strupr(m_szVGAName);  // 검색할 문자열 대문자로 변경 
	GetVGACompanyName(m_szVGAName, szVGA_CompanyName);
	m_buteMgr.SetString(s_aTagName, "VGACompanyName",szVGA_CompanyName);
	//////////////////////	

//VGA MEMORY =================================== 
	m_dwVideoMem = g_pGameClientShell->GetGlobalClientMgr()->GetVideoMemorySize();
	uint32 iVideoMem = m_dwVideoMem / 1000000;  // M

//CPU===========================================
	LTFLOAT fCpuSpeed = si.GetCPUEx();	
	
	
	return nGrade = GetGrade(iVideoMem, fCpuSpeed, szVGA_CompanyName);
}
VOID 
CUserProfile::GetVGACompanyName(const char* szVGAName, char* szVGA_CompanyName)
{
	if(strstr(szVGAName, "ATI "))  // 이름에 공백을 포함해야 중복을 피할 수 있슴.
		strcpy(szVGA_CompanyName, "ATI");
	else if(strstr(szVGAName, "NVIDIA "))
		strcpy(szVGA_CompanyName, "NVIDIA");
	else
		strcpy(szVGA_CompanyName, "OTHERS");
}
int8
CUserProfile::GetGrade(uint32 iVideoMem, LTFLOAT fCpuSpeed, char* szVGA_CompanyName)
{
	uint8 nGrade = 0;
	if(stricmp(szVGA_CompanyName, "OTHERS") == 0)
	{
		if(iVideoMem > 50)   // 편차를 가만해야 함.
			return	nGrade = 1;
		else if(iVideoMem > 25)
			return	nGrade = 2;
		else 
			return	nGrade = 3;
	}
	else  // ATI, NVIDIA 
	{
		if(iVideoMem > 100)   // 편차를 가만해야 함.
		{
			if(fCpuSpeed > 2.0f)
				return nGrade = 1;
			else if(fCpuSpeed > 1.0f)
				return nGrade = 1;
			else if(fCpuSpeed > 0.5f)
				return nGrade = 2;
			else 
				return	nGrade = 2;
		}
		else if(iVideoMem > 50)
		{
			if(fCpuSpeed > 2.0f)
				return nGrade = 1;
			else if(fCpuSpeed > 1.0f)
				return nGrade = 2;
			else if(fCpuSpeed > 0.5f)
				return nGrade = 3;
			else 
				return	nGrade = 3;
		}
		else if(iVideoMem > 25)
		{
			if(fCpuSpeed > 2.0f)
				return nGrade = 4;
			else if(fCpuSpeed > 1.0f)
				return nGrade = 4;
			else if(fCpuSpeed > 0.5f)
				return nGrade = 5;
			else 
				return	nGrade = 5;
		}
		else 
		{
			if(fCpuSpeed > 2.0f)
				return nGrade = 4;
			else if(fCpuSpeed > 1.0f)
				return nGrade = 5;
			else if(fCpuSpeed > 0.5f)
				return nGrade = 5;
			else 
				return	nGrade = 5;
		}
	}
	return nGrade;
}

VOID
CUserProfile::SetChangeOptionSetting()
{
	m_buteMgr.Term();
	
	std::string Profile = PROFILE_DIR;
	Profile += "Player.txt";
	LTBOOL bRet = LTFALSE;
	bRet = m_buteMgr.Parse(Profile.c_str());
	if(!bRet)return;
	
	m_bVSyncOnFlip	 = GetConsoleInt("SoundFilters", 0);				// 사운드 필터링으로 사용
	m_bModelShadow   = GetConsoleInt("ModelShadow_Proj_Enable", 1);
	m_bPolyGrid		 = GetConsoleInt("DrawPolyGrids", 1);
	m_bSprite		 = GetConsoleInt("DrawSprites", 1);
	m_bTracer		 = GetConsoleInt("Tracers", 0);

//	m_bParticle		 = GetConsoleInt("DrawParticles", 1);
	m_bParticle		 = GetConsoleInt("SmokeGrenadeDetail", 1);

	m_bCacheTexture	 = GetConsoleInt("CacheTexture", 1);

//	m_bWeather		 = GetConsoleInt("Weather", 1);
	m_bShellCasing	 = GetConsoleInt("ShellCasings", 1);
	m_bGore			 = GetConsoleInt("Gore", 1);
	m_bNameRadar	 = GetConsoleInt("NameRadar", 1);
	m_bDynaLight	 = GetConsoleInt("DynaLight", 1);
	m_bRejectWhisper = GetConsoleInt("RejectWhisper", 1);
	m_bRejectInvite	 = GetConsoleInt("RejectInvite", 1);
	
	strcpy(s_aTagName, "AutoOptionSetting");
	m_buteMgr.SetInt(s_aTagName, "ScreenWidth", m_nScreenWidth);
	m_buteMgr.SetInt(s_aTagName, "ScreenHeight", m_nScreenHeight);
	m_buteMgr.SetInt(s_aTagName, "ScreenBitdepth", m_nScreenDepth);
	m_buteMgr.SetInt(s_aTagName, "VsyncOnFlip", m_bVSyncOnFlip);
	m_buteMgr.SetInt(s_aTagName, "ModelShadow", m_bModelShadow);
	m_buteMgr.SetInt(s_aTagName, "PolyGrid", m_bPolyGrid);
	m_buteMgr.SetInt(s_aTagName, "Sprite", m_bSprite);
	m_buteMgr.SetInt(s_aTagName, "Tracer", m_bTracer);
	m_buteMgr.SetInt(s_aTagName, "SmokeDetail", m_bParticle);
	m_buteMgr.SetInt(s_aTagName, "CacheTexture", m_bCacheTexture);

	m_buteMgr.SetInt(s_aTagName, "Weather", m_bWeather);
	m_buteMgr.SetInt(s_aTagName, "ShellCasing", m_bShellCasing);
	m_buteMgr.SetInt(s_aTagName, "Blood", m_bGore);

	m_buteMgr.SetInt(s_aTagName, "NameRadar", m_bNameRadar);
	m_buteMgr.SetInt(s_aTagName, "DynaLight", m_bDynaLight);
	m_buteMgr.SetInt(s_aTagName, "RejectWhisper", m_bRejectWhisper);
	m_buteMgr.SetInt(s_aTagName, "RejectInvite", m_bRejectInvite);
	
	m_buteMgr.Save(Profile.c_str());   // Profile 최종 저장 	
}
VOID			
CUserProfile::AutoSetting()
{
	m_buteMgr.Term();
	
	std::string AutoSettingData = PROFILE_DIR;
	AutoSettingData += "AutoSettingData.txt";

	LTBOOL bRet = LTFALSE;
	bRet = m_buteMgr.Parse(AutoSettingData.c_str());
	if(!bRet) return;

	strcpy(s_aTagName,"Basic");

	LTBOOL bDoesSetting = LTFALSE; 
	
	char	szSubKey[MAX_PATH] = {0,};

	HCONSOLEVAR	hService	= LTNULL;
	hService	= g_pLTClient->GetConsoleVar("Service");
	if( hService )
	{		
		const char* lpszService	= g_pLTClient->GetVarValueString( hService );
		if( lpszService )	//넷마블
		{
			if( stricmp( lpszService, "cj_internet" ) == 0 )
			{
				strcpy( szSubKey, "Software\\Netmarble\\NetmarbleSuddenAttack" );
			}
		}
	}

	if( strlen( szSubKey ) <= 0 )
	{
		strcpy( szSubKey, "Software\\Gamehi\\Sa" );
	}
	
// 레지스트리를 이용해 최초의 AutoSetting을 했는지 검사 ====================================================================
	if(!Reg_ReadInt(HKEY_LOCAL_MACHINE, szSubKey, "DoesAutoSetting", &bDoesSetting))
	{
		if(Reg_WriteInt(HKEY_LOCAL_MACHINE, szSubKey, "DoesAutoSetting", LTTRUE))
		{
			bDoesSetting = LTFALSE;
		}
	}
	else
	{
		if(!bDoesSetting)
		{
			Reg_WriteInt(HKEY_LOCAL_MACHINE, szSubKey, "DoesAutoSetting", LTTRUE);
		}
	}
// ===================================================================================================

	if(bDoesSetting)   // 
		return;
	else   // Default와 Profile Setting 
	{
		m_buteMgr.SetInt(s_aTagName, "CurrentGrade", GetHardWareGrade());
		m_buteMgr.SetBool(s_aTagName, "DoSetting", LTTRUE);  // 다시는 하지 않기 위해 
		m_buteMgr.Save(AutoSettingData.c_str());

		char szVGAName[64] = {LTNULL};
		strcpy(szVGAName, m_buteMgr.GetString(s_aTagName, "VGACompanyName", 0));
		int8 nGrade = m_buteMgr.GetInt(s_aTagName, "CurrentGrade", 0);	
		
		sprintf(s_aTagName, "%s%d", szVGAName, nGrade);
		m_nScreenWidth	 = m_buteMgr.GetInt(s_aTagName, "ScreenWidth",800);
		m_nScreenHeight	 = m_buteMgr.GetInt(s_aTagName, "ScreenHeight",600);
		m_nScreenDepth	 = m_buteMgr.GetInt(s_aTagName, "ScreenBitdepth",32);
		m_bVSyncOnFlip	 = m_buteMgr.GetInt(s_aTagName, "VsyncOnFlip", 0);
		m_bModelShadow   = m_buteMgr.GetInt(s_aTagName, "ModelShadow", 0);
		m_bPolyGrid		 = m_buteMgr.GetInt(s_aTagName, "PolyGrid", 0);
		m_bSprite		 = m_buteMgr.GetInt(s_aTagName, "Sprite", 0);
		m_bTracer		 = m_buteMgr.GetInt(s_aTagName, "Tracer", 0);
		m_bParticle		 = m_buteMgr.GetInt(s_aTagName, "SmokeDetail", 0);
		m_bCacheTexture	 = m_buteMgr.GetInt(s_aTagName, "CacheTexture", 0);

		m_bWeather		 = m_buteMgr.GetInt(s_aTagName, "Weather", 0);
		m_bShellCasing	 = m_buteMgr.GetInt(s_aTagName, "ShellCasing", 0);
		m_bGore			 = m_buteMgr.GetInt(s_aTagName, "Blood", 0);
		m_bNameRadar	 = m_buteMgr.GetInt(s_aTagName, "NameRadar", 0);
		m_bDynaLight	 = m_buteMgr.GetInt(s_aTagName, "DynaLight", 0);
		m_bRejectWhisper = m_buteMgr.GetInt(s_aTagName, "RejectWhisper", 0);
		m_bRejectInvite	 = m_buteMgr.GetInt(s_aTagName, "RejectInvite", 0);
		
		m_buteMgr.Term();
		
		std::string Default = PROFILE_DIR;
		Default += "default.prf";
		std::string Profile = PROFILE_DIR;
		Profile += "Player.txt";
		
		bRet = m_buteMgr.Parse(Default.c_str());
		if(!bRet) return;

		strcpy(s_aTagName, "AutoOptionSetting");
		for(int8 i=0; i<2; i++)
		{
			m_buteMgr.SetInt(s_aTagName, "ScreenWidth", m_nScreenWidth);
			m_buteMgr.SetInt(s_aTagName, "ScreenHeight", m_nScreenHeight);
			m_buteMgr.SetInt(s_aTagName, "ScreenBitdepth", m_nScreenDepth);
			m_buteMgr.SetInt(s_aTagName, "VsyncOnFlip", m_bVSyncOnFlip);
			m_buteMgr.SetInt(s_aTagName, "ModelShadow", m_bModelShadow);
			m_buteMgr.SetInt(s_aTagName, "PolyGrid", m_bPolyGrid);
			m_buteMgr.SetInt(s_aTagName, "Sprite", m_bSprite);
			m_buteMgr.SetInt(s_aTagName, "Tracer", m_bTracer);
			m_buteMgr.SetInt(s_aTagName, "SmokeDetail", m_bParticle);
			m_buteMgr.SetInt(s_aTagName, "CacheTexture", m_bCacheTexture);

			m_buteMgr.SetInt(s_aTagName, "Weather", m_bWeather);
			m_buteMgr.SetInt(s_aTagName, "ShellCasing", m_bShellCasing);
			m_buteMgr.SetInt(s_aTagName, "Blood", m_bGore);
			m_buteMgr.SetInt(s_aTagName, "NameRadar", m_bNameRadar);
			m_buteMgr.SetInt(s_aTagName, "DynaLight", m_bDynaLight);
			m_buteMgr.SetInt(s_aTagName, "RejectWhisper", m_bRejectWhisper);
			m_buteMgr.SetInt(s_aTagName, "RejectInvite", m_bRejectInvite);
			
			if(i < 1)
			{
				m_buteMgr.Save(Default.c_str());   // Default 저장 
				m_buteMgr.Term();
				m_buteMgr.Parse(Profile.c_str());	// Profile 스크립트 Parse					
			}
			else
				m_buteMgr.Save(Profile.c_str());   // Profile 저장 
		}
	}
}

LTBOOL CProfileMgr::Init()
{

	ASSERT(g_kNumCommands <= MAX_NUM_COMMANDS);
	if (g_kNumCommands > MAX_NUM_COMMANDS) 
		return LTFALSE;


	LoadString(IDS_PLAYER,szDefProfileName,sizeof(szDefProfileName));



	g_pProfileMgr = this;

	SetDeviceData();

	std::string currentProfile;
	char szProfileName[128] = "";

	//get the profile name
	CRegMgr* pRegMgr = g_pVersionMgr->GetRegMgr();
	if (pRegMgr->IsValid())
	{
		unsigned int nBufSize = sizeof(szProfileName);
		pRegMgr->Get("ProfileName", szProfileName, nBufSize, "");
	}

	//are we supposed to set a particular performance level?
	char *pszPerf = GetConsoleTempString("SetPerformanceLevel","");
	//if we have a profile name try to init it
	if (strlen(szProfileName) > 0)
	{
		currentProfile = szProfileName;
		if (m_profile.Init(currentProfile, LTFALSE))
		{
			//if we've been told to restore, do so...
			if (GetConsoleInt("RestoreDefaults",0))
			{
				m_profile.RestoreDefaults(PROFILE_ALL);
				WriteConsoleInt("RestoreDefaults",0);
			}
			
		
			m_profile.SetPerformanceCfg(pszPerf);
			WriteConsoleString("SetPerformanceLevel","");


			//we also want to make sure to save the display configuration so that parameters passed
			//through the command line will be saved for the next run
			SaveDisplaySettings();
			return LTTRUE;
		}
	}

	//we either don't have a profile or it couldn't be initted, create one
	NewProfile(szDefProfileName);

	if (!m_profile.IsInitted()) 
		return LTFALSE;

	m_profile.SetPerformanceCfg(pszPerf);
	WriteConsoleString("SetPerformanceLevel","");


	//we also want to make sure to save the display configuration so that parameters passed
	//through the command line will be saved for the next run
	SaveDisplaySettings();

	return LTTRUE;
}

void CProfileMgr::Term()
{
	if (m_profile.IsInitted())
	{
		m_profile.SetBindings();

		m_profile.Save();
		m_profile.Term();
	}

	g_pProfileMgr = LTNULL;
}

void CProfileMgr::GetProfileList(StringSet& profileList)
{
	struct _finddata_t file;
	long hFile;

	std::string directory = GetProfileFile( "*" );

	// find first file
	if((hFile = _findfirst(directory.c_str(), &file)) != -1L)
	{
		do
		{
			char *pName = strtok(file.name,".");
			profileList.insert(pName);
		}
		while(_findnext(hFile, &file) == 0);
	}
	_findclose(hFile);
}

void CProfileMgr::ClearBindings()
{
	//step through each device
	for (int i = 0; i < 2/*3*/; ++i) //[SPIKE] 조이스틱은 사용을 안함으로 for문을 dev < 2 로 변경!!! 
	{
        DeviceBinding* pBindings = g_pLTClient->GetDeviceBindings (devices[i]);
		if (!pBindings)
		{
			continue;
		}

		//step through each binding
		DeviceBinding* ptr = pBindings;
		while (ptr)
		{
			//step through each action on that binding checking to see if we should clear it
			GameAction* pAction = ptr->pActionHead;
			while (pAction)
			{
				int com = 0;
				while (com < g_kNumCommands && pAction->nActionCode != g_CommandArray[com].nCommandID)
					com++;
				//if it is a bindable command, clear it
				if (com < g_kNumCommands)
				{
					uint32 contType = GetControlType(devices[i], ptr->m_nObjectId );
					if (contType == CONTROLTYPE_BUTTON || contType == CONTROLTYPE_KEY)							 
					{
						g_pLTClient->ClearBinding(ptr->strDeviceName, ptr->strRealName);
					}
					else if (devices[i] == DEVICETYPE_MOUSE && contType == CONTROLTYPE_ZAXIS)
					{
						g_pLTClient->ClearBinding(ptr->strDeviceName, ptr->strRealName);
					}

				}
				pAction = pAction->pNext;
			}

			ptr = ptr->pNext;
		}

        g_pLTClient->FreeDeviceBindings (pBindings);
	}
}



void CProfileMgr::NewProfile(const std::string& profileName)
{
	if (profileName.length() == 0)
		return;
	if (m_profile.IsInitted())
	{
		m_profile.Save();
		m_profile.Term();
	}

	// Make sure we're disconnected from server.
	if(g_pLTClient->IsConnected())
	{
		g_pInterfaceMgr->SetIntentionalDisconnect( true );
		g_pClientMultiplayerMgr->ForceDisconnect();
	}
	
	if (!m_profile.Init(profileName, LTTRUE)) return;

	m_profile.Save();
	

	CRegMgr* pRegMgr = g_pVersionMgr->GetRegMgr();
	if (pRegMgr->IsValid())
	{
		pRegMgr->Set("ProfileName", m_profile.m_sName.c_str());
	}
	else
	{
		_ASSERT("!RegMgr not valid.");
	}

}

void CProfileMgr::DeleteProfile(const std::string& profileName)
{
	LTBOOL bNeedNew = LTFALSE;
	if (m_profile.IsInitted() && m_profile.m_sName.compare(profileName) == 0)
	{
		m_profile.Term();
		bNeedNew = LTTRUE;
	}
		
	std::string fn = GetProfileFile( profileName.c_str( ));
	remove(fn.c_str());

	if (bNeedNew)
	{
		StringSet	profileList;
		GetProfileList(profileList);

		StringSet::iterator iter = profileList.begin();
		if (iter == profileList.end())
		{
			//empty list
			NewProfile(szDefProfileName);
		}
		else
			NewProfile(*iter);


	}

}
void CProfileMgr::RenameProfile(const std::string& oldName,const std::string& newName)
{
	/*
	if (newName.length() == 0)
		return;
	LTBOOL bIsCurrent = (m_profile.IsInitted() && m_profile.m_sName.compare(oldName) == 0);

	if (bIsCurrent && m_profile.IsInitted())
	{
		m_profile.Save();
		m_profile.Term();
	}
		
	std::string ofn = GetProfileFile( oldName.c_str( ));
	std::string nfn = GetProfileFile( newName.c_str( ));
	rename(ofn.c_str(),nfn.c_str());

	char oldDir[128];
	char newDir[128];
	sprintf(oldDir,"%s\\%s",SAVE_DIR,oldName.c_str());
	sprintf(newDir,"%s\\%s",SAVE_DIR,newName.c_str());
	rename(oldDir,newDir);

	if (bIsCurrent)
	{
		m_profile.Init(newName,LTFALSE);

		CRegMgr* pRegMgr = g_pVersionMgr->GetRegMgr();
		if (pRegMgr->IsValid())
		{
			pRegMgr->Set("ProfileName", m_profile.m_sName.c_str());
		}
		else
		{
			_ASSERT("!RegMgr not valid.");
		}


	}
	*/
}


LTBOOL CUserProfile::RestoreDefaults(uint8 nFlags)
{
	std::string dfn = PROFILE_DIR;
	dfn += "default.prf";

	LTBOOL bRet = LTFALSE;

	if (m_bInitted)
		m_buteMgr.Term();

	if (m_pCryptKey)
	{
		bRet = m_buteMgr.Parse(dfn.c_str(), m_pCryptKey);
	}
	else
	{
		bRet = m_buteMgr.Parse(dfn.c_str());
	}

	if (!bRet) return LTFALSE;

	if (nFlags & PROFILE_CONTROLS)
		LoadControls();
	if (nFlags & PROFILE_MULTI)
		LoadMultiplayer();
	if (nFlags & PROFILE_GAME)
		LoadGameOptions();
	if (nFlags & PROFILE_SOUND)
		LoadSound(true);

	m_bInitted =  LTTRUE;

	return LTTRUE;

}

uint32 CProfileMgr::GetControlType(uint32 deviceType, uint32 nObjectId )
{
	DeviceObject* pList = g_pLTClient->GetDeviceObjects(deviceType);
	DeviceObject* pObj = pList;
    uint32 type = CONTROLTYPE_UNKNOWN;
	bool bFound = false;
	while (pObj != NULL && !bFound)
	{
		bFound = ( nObjectId == pObj->m_nObjectId );
		if (bFound)
			type = pObj->m_ObjectType;
		pObj = pObj->m_pNext;
	}

	if( pList )
	{
		g_pLTClient->FreeDeviceObjects( pList );
		pList = NULL;
	}

	return type;
}


void CProfileMgr::SetDeviceData()
{
	//[SPIKE] 조이스틱은 사용을 안함!!! 
/*	if (g_pGameClientShell->HasGamepad())
	{
		devices[2] = DEVICETYPE_GAMEPAD;
		g_pGameClientShell->EnableGamepad();
	}
	else if (g_pGameClientShell->HasJoystick())
	{
		devices[2] = DEVICETYPE_JOYSTICK;
		g_pGameClientShell->EnableJoystick();
	}
	else
		devices[2] = DEVICETYPE_UNKNOWN;
*/
	for (int dev = 0; dev < 2/*3*/; dev++) //[SPIKE] 조이스틱은 사용을 안함으로 for문을 dev < 2 로 변경!!! 
	{
		g_pLTClient->GetDeviceName (devices[dev],strDeviceName[dev], sizeof(strDeviceName[dev]));
	}

	LoadString(IDS_WHEEL_UP,szWheelUp,sizeof(szWheelUp));
	LoadString(IDS_WHEEL_DOWN,szWheelDown,sizeof(szWheelDown));
/*
	LoadString(IDS_ACTIONSTRING_LOOKUP,szJoyLookUp,sizeof(szJoyLookUp));
	LoadString(IDS_ACTIONSTRING_LOOKDOWN,szJoyLookDown,sizeof(szJoyLookDown));
	LoadString(IDS_ACTIONSTRING_FORWARD,szJoyForward,sizeof(szJoyForward));
	LoadString(IDS_ACTIONSTRING_BACKWARD,szJoyBackward,sizeof(szJoyBackward));
	LoadString(IDS_ACTIONSTRING_TURNLEFT,szJoyLeft,sizeof(szJoyLeft));
	LoadString(IDS_ACTIONSTRING_TURNRIGHT,szJoyRight,sizeof(szJoyRight));
	LoadString(IDS_ACTIONSTRING_STRAFELEFT,szJoyStrafeLeft,sizeof(szJoyStrafeLeft));
	LoadString(IDS_ACTIONSTRING_STRAFERIGHT,szJoyStrafeRight,sizeof(szJoyStrafeRight));


	m_nNumDeviceAxis = 0;
	m_nNumDevicePOV = 0;

	

	DeviceObject* pObjects = g_pLTClient->GetDeviceObjects(devices[2]);
	DeviceObject* pObj = pObjects;
    LTBOOL bFoundIt = LTFALSE;

	// loop through all joystick objects and store the axis ones with our devicename the m_aDeviceData array
	while ((pObj != NULL) && (m_nNumDeviceAxis < kMaxDeviceAxis))
	{
		if ((pObj->m_ObjectName != NULL) && (pObj->m_DeviceName != NULL))
		{
			if	(	(pObj->m_DeviceType == DEVICETYPE_JOYSTICK || pObj->m_DeviceType == DEVICETYPE_GAMEPAD) &&
					(stricmp(pObj->m_DeviceName, strDeviceName[2]) == 0) &&
					(	(pObj->m_ObjectType == CONTROLTYPE_XAXIS) ||
						(pObj->m_ObjectType == CONTROLTYPE_YAXIS) ||
						(pObj->m_ObjectType == CONTROLTYPE_ZAXIS) ||
						(pObj->m_ObjectType == CONTROLTYPE_RXAXIS) ||
						(pObj->m_ObjectType == CONTROLTYPE_RYAXIS) ||
						(pObj->m_ObjectType == CONTROLTYPE_RZAXIS) 
					)
				)
			{
				m_aDeviceAxisData[m_nNumDeviceAxis].Init(pObj->m_ObjectName, pObj->m_ObjectType, pObj->m_RangeLow, pObj->m_RangeHigh);
				m_nNumDeviceAxis++;
			}

		}
		if ((pObj->m_ObjectName != NULL) && (pObj->m_DeviceName != NULL))
		{
			if	(	(pObj->m_DeviceType == DEVICETYPE_JOYSTICK || pObj->m_DeviceType == DEVICETYPE_GAMEPAD) &&
					(stricmp(pObj->m_DeviceName, strDeviceName[2]) == 0) &&
					(pObj->m_ObjectType == CONTROLTYPE_POV)
				)
			{
				m_aDevicePOVData[m_nNumDevicePOV].Init(pObj->m_ObjectName);
				m_nNumDevicePOV++;
			}

		}
		pObj = pObj->m_pNext;
	}

	// free the device objects
	if (pObjects != NULL) g_pLTClient->FreeDeviceObjects (pObjects);
*/
}

CDeviceAxisData* CProfileMgr::GetAxisData(int ndx)
{
	if (ndx < 0 || ndx >= m_nNumDeviceAxis) 
		return LTNULL;
	return &m_aDeviceAxisData[ndx];
}

CDevicePOVData* CProfileMgr::GetPOVData(int ndx)
{
	if (ndx < 0 || ndx >= m_nNumDevicePOV) 
		return LTNULL;
	return &m_aDevicePOVData[ndx];
}

LTBOOL IsRendererEqual(RMode *pRenderer1, RMode *pRenderer2)
{
	_ASSERT(pRenderer1);
	_ASSERT(pRenderer2);
	if (!pRenderer1 || !pRenderer2) return LTFALSE;

	if (pRenderer1->m_Width != pRenderer2->m_Width)
	{
        return LTFALSE;
	}

	if (pRenderer1->m_Height != pRenderer2->m_Height)
	{
        return LTFALSE;
	}

	if (pRenderer1->m_BitDepth != pRenderer2->m_BitDepth)
	{
        return LTFALSE;
	}

    return LTTRUE;
}

// Gets the default session name.
bool GetDefaultSessionName( GameType eGameType, char* pszSessionName, uint32 nSessionNameLen )
{
	uint32 nStringId = 0;

	switch( eGameType )
	{
		case eGameTypeCooperative:
			nStringId = IDS_HOST_NAME_COOP_DEFAULT;
			break;
		case eGameTypeDeathmatch:
			nStringId = IDS_HOST_NAME_DM_DEFAULT;
			break;
		case eGameTypeTeamDeathmatch:
		case eGameTypeSurvival :
			nStringId = IDS_HOST_NAME_TDM_DEFAULT;
			break;
		case eGameTypeDoomsDay:
			nStringId = IDS_HOST_NAME_DD_DEFAULT;
			break;		
		default:
			return false;
			break;
	}

	strncpy( pszSessionName, LoadTempString( nStringId ), nSessionNameLen );
	pszSessionName[ nSessionNameLen - 1 ] = '\0';

	return true;
}

