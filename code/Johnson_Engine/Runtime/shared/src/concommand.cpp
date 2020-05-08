#include "bdefs.h"

#include "console.h"
#include "concommand.h"
#include "dhashtable.h"
#include "ltpvalue.h"
#include "clientmgr.h"
#include "client_filemgr.h"

// ±¤½Ä
// autoexec.cfg ¸¦ ¾ø¾Ö°í hardcoding ÇØ ¹ö¸².
//---------------------------------------------
#define AUTOEXEC_d sizeof(g_autoexeccfg) / sizeof(char*)
char* g_autoexeccfg[] = 
{
	TEXT("numconsolelines 2"),
	TEXT("LodScale 2.000000"),
	TEXT("FarZ 10000"),
	TEXT("ModelShadow_proj_perspective 1"),
	TEXT("VS_SPRITES_NOZ 128"),
	TEXT("BindingIconSize 0"),
	TEXT("CardDesc HAL"),
	TEXT("NormalTurnRate 1.500000"),
	TEXT("ModelWarble 0"),
	TEXT("DetailTextureScale 1.0"),
	TEXT("MaxModelShadows 1"),
	TEXT("LightModelSprites 1"),
	TEXT("GammaB 1.100000"),
	TEXT("VS_PARTICLESYSTEMS 512"),
	TEXT("WeaponFXMaxImpactDist 5000"),
	TEXT("FogEnable 1.000000"),
	TEXT("EnableRetailLevels 1"),
	TEXT("ScreenFlash 0.000000"),
	TEXT("MouseScaleBase 0.00015"),
	TEXT("EnvPanSpeed 0.000500"),
	TEXT("DrawPolyGrids 1"),
	TEXT("ModelLOD 2.000000"),
	TEXT("optimizesurfaces 1"),
	TEXT("ModelShadow_proj_textureres 256"),
	TEXT("FXDetail 0"),
	TEXT("GammaG 1.100000"),
	TEXT("Difficulty 1"),
	TEXT("SnowDensityScale 1.000000"),
	TEXT("RunLock 1"),
	TEXT("BackBufferCount 1"),
	TEXT("windowed 0"),
	TEXT("WeaponSway 0.900000"),
	TEXT("VS_SPRITES 512"),
	TEXT("ModelShadowProj 0"),
	TEXT("LeanAngle 18"),
	TEXT("Tracers 1"),
	TEXT("VehicleContour 0"),
	TEXT("LightMap 1"),
	TEXT("MusicActive 0"),
	TEXT("SoundFilters 1"),
	TEXT("screendepth 16"),
	TEXT("AxisPitchDeadZone 0.024000"),
	TEXT("DetailLevel 0"),
	TEXT("GammaR 1.100000"),
	TEXT("VS_POLYGRIDS_TRANSLUCENT 64"),
	TEXT("MouseSensitivity 0.000000"),
	TEXT("CrosshairStyle 0"),
	TEXT("FastTurnRate 2.300000"),
	TEXT("Saturate 1"),
	TEXT("Sound16Bit 1"),
	TEXT("BitDepth 16"),
	TEXT("HardwareCursor 0"),
	TEXT("CrosshairColorR 0.874510"),
	TEXT("ScatterEnable 1"),
	TEXT("VehicleTurnRateScale 1.000000"),
	TEXT("VS_POLYGRIDS 64"),
	TEXT("speechsoundmultiplier 1.000000"),
	TEXT("fixtjunc 1"),
	TEXT("ModelShadowProjLOD 1.000000"),
	TEXT("screenwidth 800"),
	TEXT("CacheTexture 1"),
	TEXT("Trilinear 0"),
	TEXT("RenderDll d3d.ren"),
	TEXT("ModelShadow_Proj_MaxShadowsPerFrame 2"),
	TEXT("MaxWorldPoliesToDraw 50000"),
	TEXT("MasterPaletteMode 1.000000"),
	TEXT("CrosshairAlpha 159"),
	TEXT("VSyncOnFlip 0"),
	TEXT("SpecialFX 2.000000"),
	TEXT("modelshadow_proj_blurenable 0"),
	TEXT("MusicQuality 0"),
	TEXT("FixSparkleys 0"),
	TEXT("EnvironmentalDetail 0"),
	TEXT("modeldist1 150.000000"),
	TEXT("UpdateRateInitted 1"),
	TEXT("32bittextures 1"),
	TEXT("BulletHoles 300"),
	TEXT("modeldist2 250.000000"),
	TEXT("MuzzleLight 1"),
	TEXT("ObjectiveMessages 1"),
	TEXT("NumRuns 6699.000000"),
	TEXT("bandwidthtargetserver 262144.000000"),
	TEXT("DrawSky 1"),
	TEXT("VS_CANVASES 32"),
	TEXT("AxisLeftRightDeadZone 0.200000"),
	TEXT("modeldist3 350.000000"),
	TEXT("Detailtextures 0"),
	TEXT("Anisotropic 0"),
	TEXT("Savewidth 800"),
	TEXT("envbumpmapenable 0"),
	TEXT("UseJoystick 0"),
	TEXT("VS_CANVASES_TRANSLUCENT 128"),
	TEXT("LodOffset -200.000000"),
	TEXT("IgnoreTaunts 0"),
	TEXT("LeanOutTime 0.15"),
	TEXT("EnvMapEnable 1"),
	TEXT("EnvMapWorld 1"),
	TEXT("ModelShadow_proj_enable 1"),
	TEXT("musicvolume -2500"),
	TEXT("BindingIconAlpha 0.500000"),
	TEXT("ModelShadow_Proj_BackFaceCull 1"),
	TEXT("SnowEnable 1"),
	TEXT("multiplayer 1"),
	TEXT("VS_LINESYSTEMS 128"),
	TEXT("AutoWeaponSwitch 1"),
	TEXT("Forceclear 0"),
	TEXT("FastLight 0.000000"),
	TEXT("SoundVolume 50"),
	TEXT("inputrate 12.000000"),
	TEXT("PolyGrids 1.000000"),
	TEXT("VS_MODELS 512"),
	TEXT("FresnelPolyGrids 0"),
	TEXT("GroupOffset0 0.000000"),
	TEXT("DrawShadows 1"),
	TEXT("MouseLook 1"),
	TEXT("BumpMapPolyGrids 0"),
	TEXT("Subtitles 0"),
	TEXT("DrawWorld 1.0"),
	TEXT("FadeBodies 0"),
	TEXT("GroupOffset1 1"),
	TEXT("ModelShadow_proj_alpha 3.000000"),
	TEXT("SendBandwidth 10000000"),
	TEXT("mipmapbias 0"),
	TEXT("GroupOffset2 1"),
	TEXT("musictype ima"),
	TEXT("DrawPortals 1"),
	TEXT("CrosshairGreen 255"),
	TEXT("GroupOffset3 2"),
	TEXT("ModelShadow_proj_MaxProjDist 200.000000"),
	TEXT("PVWeapons 0.000000"),
	TEXT("ModelShadowAlpha 96"),
	TEXT("MouseScaleIncrement 0.00003"),
	TEXT("TextureDetail 2.000000"),
	TEXT("consolealpha 0.25"),
	TEXT("ClientFXDetailLevel 0"),
	TEXT("VS_WORLDMODELS_TRANSLUCENT 512"),
	TEXT("GroupOffset4 -2"),
	TEXT("MouseInvertY 0"),
	TEXT("pvmodelaspect 1.333333"),
	TEXT("GroupOffset5 -2"),
	TEXT("VS_LIGHTS 64"),
	TEXT("DrawSprites 1"),
	TEXT("bandwidthtargetclient 10240000.000000"),
	TEXT("GroupOffset6 -2"),
	TEXT("ModelShadowProjRes 128.000000"),
	TEXT("SaveHeight 600"),
	TEXT("ModelFullbrite 0"),
	TEXT("ModelLODOffset 0"),
	TEXT("CrosshairBlue 255"),
	TEXT("LeanCenterTime 0.2"),
	TEXT("DebugLevel 0"),
	TEXT("ShadowDetail 0"),
	TEXT("DynamicLightWorld 1"),
	TEXT("AxisForwardBackwardDeadZone 0.200000"),
	TEXT("CloudMapLight 1.000000"),
	TEXT("FixedAxisPitch 0.000000"),
	TEXT("PreCacheAssets 0"),
	TEXT("screenheight 600"),
	TEXT("VS_MODELS_TRANSLUCENT 256"),
	TEXT("TripleBuffer 0"),
	TEXT("HUDLayout 0"),
	TEXT("prediction 1"),
	TEXT("defaultsoundmultiplier 0.750000"),
	TEXT("performancelevel 3"),
	TEXT("DrawParticles 1"),
	TEXT("AxisYawDeadZone 0.030000"),
	TEXT("musicenable 1"),
	TEXT("FontWidthHeightRatio 0.500000"),
	TEXT("CrosshairColorB 1.000000"),
	TEXT("32BitLightMaps 1"),
	TEXT("weaponssoundmultiplier 1.000000"),
	TEXT("CrosshairRed 255"),
	TEXT("CrosshairDynamic 1"),
	TEXT("DynamicLight 1"),
	TEXT("ImpactFXLevel 2"),
	TEXT("soundenable 1"),
	TEXT("PerformanceConfig .DefaultHigh"),
	TEXT("ModelAdd 50 50 50"),
	TEXT("WarbleScale .95"),
	TEXT("LookUpRate 2.500000"),
	TEXT("Gore 1"),
	TEXT("pvmodelfov 68.000000"),
	TEXT("BindingNumbers 1"),
	TEXT("EnableWeatherFX 1"),
	TEXT("HeadBob 5.600000"),
	TEXT("DebrisFXLevel 0"),
	TEXT("VS_WORLDMODELS 512"),
	TEXT("PickupMessageDuration 5.000000"),
	TEXT("SaveBitdepth 16"),
	TEXT("ShellCasings 1"),
	TEXT("LookSpring 0.000000"),
	TEXT("MipmapDist 375.000000"),
	TEXT("ShadowZRange 17.000000"),
	TEXT("shadowlodoffset 100.000000"),
	TEXT("CrosshairColorG 0.874510"),
	TEXT("MessageDuration 0.800000"),
	TEXT("LightSaturate 0.5"),
	TEXT("ProfileName Player_0"),
	TEXT("WarbleSpeed 15"),
	TEXT("AddAction Forward 0"),
	TEXT("AddAction Backward 1"),
	TEXT("AddAction Activate 2"),
	TEXT("AddAction Reload 3"),
	TEXT("AddAction TurnAround 4"),
	TEXT("AddAction Duck 5"),
	TEXT("AddAction Jump 6"),
	TEXT("AddAction Speed 7"),
	TEXT("AddAction Fire 8"),
	TEXT("AddAction Strafe 9"),
	TEXT("AddAction Left 10"),
	TEXT("AddAction Right 11"),
	TEXT("AddAction RunLock 12"),
	TEXT("AddAction StrafeLeft 13"),
	TEXT("AddAction StrafeRight 14"),
	TEXT("AddAction LookUp 16"),
	TEXT("AddAction LookDown 17"),
	TEXT("AddAction LeanLeft 18"),
	TEXT("AddAction LeanRight 19"),
	TEXT("AddAction Weapon_1 20"),
	TEXT("AddAction Weapon_2 21"),
	TEXT("AddAction Weapon_3 22"),
	TEXT("AddAction Weapon_4 23"),
	TEXT("AddAction Weapon_5 24"),
	TEXT("AddAction Weapon_6 25"),
	TEXT("AddAction Weapon_7 26"),
	TEXT("AddAction Weapon_8 27"),
	TEXT("AddAction Weapon_9 28"),
	TEXT("AddAction Weapon_10 29"),
	TEXT("AddAction Weapon_11 30"),
	TEXT("AddAction Weapon_12 31"),
	TEXT("AddAction Weapon_13 32"),
	TEXT("AddAction Weapon_14 33"),
	TEXT("AddAction Weapon_15 34"),
	TEXT("AddAction Weapon_16 35"),
	TEXT("AddAction Weapon_17 36"),
	TEXT("AddAction Weapon_18 37"),
	TEXT("AddAction Weapon_19 38"),
	TEXT("AddAction Weapon_20 39"),
	TEXT("AddAction Weapon_21 40"),
	TEXT("AddAction Weapon_22 41"),
	TEXT("AddAction Weapon_23 42"),
	TEXT("AddAction Weapon_24 43"),
	TEXT("AddAction Weapon_25 44"),
	TEXT("AddAction Weapon_26 45"),
	TEXT("AddAction Weapon_27 46"),
	TEXT("AddAction Weapon_28 47"),
	TEXT("AddAction Weapon_29 48"),
	TEXT("AddAction Weapon_30 49"),
	TEXT("AddAction Weapon_31 50"),
	TEXT("AddAction Weapon_32 51"),
	TEXT("AddAction Weapon_33 52"),
	TEXT("AddAction Weapon_34 53"),
	TEXT("AddAction Weapon_35 54"),
	TEXT("AddAction Weapon_36 55"),
	TEXT("AddAction Weapon_37 56"),
	TEXT("AddAction Weapon_38 57"),
	TEXT("AddAction Weapon_39 58"),
	TEXT("AddAction Weapon_40 59"),
	TEXT("AddAction NextAmmo 60"),
	TEXT("AddAction ZoomIn 61"),
	TEXT("AddAction ZoomOut 62"),
	TEXT("AddAction QuickSave 64"),
	TEXT("AddAction QuickLoad 65"),
	TEXT("AddAction PrevWeapon 66"),
	TEXT("AddAction NextWeapon 67"),
	TEXT("AddAction Score 68"),
	TEXT("AddAction Mission 69"),
	TEXT("AddAction MouseAim 70"),
	TEXT("AddAction CursorTog 71"),
	TEXT("AddAction CenterView 72"),
	TEXT("AddAction HolsterWeapon 73"),
	TEXT("AddAction SendMessage 74"),
	TEXT("AddAction IncLayout 75"),
	TEXT("AddAction DecLayout 76"),
	TEXT("AddAction Inventory 77"),
	TEXT("AddAction NextWeapon_1 78"),
	TEXT("AddAction NextWeapon_2 79"),
	TEXT("AddAction NextWeapon_3 80"),
	TEXT("AddAction NextWeapon_4 81"),
	TEXT("AddAction NextWeapon_5 82"),
	TEXT("AddAction NextWeapon_6 83"),
	TEXT("AddAction Choose_1 84"),
	TEXT("AddAction Choose_2 85"),
	TEXT("AddAction Choose_3 86"),
	TEXT("AddAction Choose_4 87"),
	TEXT("AddAction Choose_5 88"),
	TEXT("AddAction Choose_6 89"),
	TEXT("AddAction DuckLock 92"),
	TEXT("AddAction LastWeapon 93"),
	TEXT("AddAction TeamMessage 94"),
	TEXT("AddAction CompassToggle 101"),
	TEXT("AddAction ViewKeys 102"),
	TEXT("AddAction ViewIntel 103"),
	TEXT("AddAction MoveBody 104"),
	TEXT("AddAction Flashlight 105"),
	TEXT("AddAction Radio 106"),
	TEXT("AddAction LeaveUnassigned 201"),
	TEXT("AddAction Axis3 -3"),
	TEXT("AddAction Axis2 -2"),
	TEXT("AddAction Axis1 -1"),
	TEXT("AddAction AxisYaw -10001"),
	TEXT("AddAction AxisPitch -10002"),
	TEXT("AddAction AxisLeftRight -10003"),
	TEXT("AddAction AxisForwardBackward -10004"),
	TEXT("enabledevice ##keyboard"),
	TEXT("rangebind ##keyboard ##28 0.000000 0.000000 SendMessage" ),
	TEXT("rangebind ##keyboard ##47 0.000000 0.000000 Radio" ),
	TEXT("rangebind ##keyboard ##15 0.000000 0.000000 Mission" ),
	TEXT("rangebind ##keyboard ##208 0.000000 0.000000 LookDown" ),
	TEXT("rangebind ##keyboard ##200 0.000000 0.000000 LookUp" ),
	TEXT("rangebind ##keyboard ##16 0.000000 0.000000 LastWeapon"), 
	TEXT("rangebind ##keyboard ##19 0.000000 0.000000 Reload" ),
	TEXT("rangebind ##keyboard ##34 0.000000 0.000000 Activate" ),
	TEXT("rangebind ##keyboard ##58 0.000000 0.000000 RunLock" ),
	TEXT("rangebind ##keyboard ##18 0.000000 0.000000 LeanRight" ),
	TEXT("rangebind ##keyboard ##14 0.000000 0.000000 TurnAround" ),
	TEXT("rangebind ##keyboard ##29 0.000000 0.000000 Duck" ),
	TEXT("rangebind ##keyboard ##57 0.000000 0.000000 Jump" ),
	TEXT("rangebind ##keyboard ##42 0.000000 0.000000 Speed" ),
	TEXT("rangebind ##keyboard ##205 0.000000 0.000000 Right" ),
	TEXT("rangebind ##keyboard ##203 0.000000 0.000000 Left" ),
	TEXT("rangebind ##keyboard ##32 0.000000 0.000000 StrafeRight" ),
	TEXT("rangebind ##keyboard ##30 0.000000 0.000000 StrafeLeft" ),
	TEXT("rangebind ##keyboard ##31 0.000000 0.000000 Backward" ),
	TEXT("rangebind ##keyboard ##17 0.000000 0.000000 Forward" ),
	TEXT("rangebind ##keyboard ##40 0.000000 0.000000 CompassToggle" ),
	TEXT("rangebind ##keyboard ##25 0.000000 0.000000 Score" ),
	TEXT("rangebind ##keyboard ##23 0.000000 0.000000 ViewIntel" ),
	TEXT("rangebind ##keyboard ##49 0.000000 0.000000 ViewKeys" ),
	TEXT("rangebind ##keyboard ##50 0.000000 0.000000 Inventory" ),
	TEXT("rangebind ##keyboard ##207 0.000000 0.000000 CenterView" ),
	TEXT("rangebind ##keyboard ##38 0.000000 0.000000 Flashlight" ),
	TEXT("rangebind ##keyboard ##35 0.000000 0.000000 HolsterWeapon"), 
	TEXT("rangebind ##keyboard ##33 0.000000 0.000000 NextAmmo" ),
	TEXT("rangebind ##keyboard ##56 0.000000 0.000000 Strafe" ),
	TEXT("rangebind ##keyboard ##7 0.000000 0.000000 NextWeapon_6 0.000000 0.000000 Choose_6" ),
	TEXT("rangebind ##keyboard ##6 0.000000 0.000000 NextWeapon_5 0.000000 0.000000 Choose_5" ),
	TEXT("rangebind ##keyboard ##5 0.000000 0.000000 NextWeapon_4 0.000000 0.000000 Choose_4" ),
	TEXT("rangebind ##keyboard ##4 0.000000 0.000000 NextWeapon_3 0.000000 0.000000 Choose_3" ),
	TEXT("rangebind ##keyboard ##3 0.000000 0.000000 NextWeapon_2 0.000000 0.000000 Choose_2" ),
	TEXT("rangebind ##keyboard ##2 0.000000 0.000000 NextWeapon_1 0.000000 0.000000 Choose_1" ),
	TEXT("rangebind ##keyboard ##13 0.000000 0.000000 DecLayout" ),
	TEXT("rangebind ##keyboard ##12 0.000000 0.000000 IncLayout" ),
	TEXT("rangebind ##keyboard ##67 0.000000 0.000000 QuickLoad" ),
	TEXT("rangebind ##keyboard ##64 0.000000 0.000000 QuickSave" ),
	TEXT("rangebind ##keyboard ##27 0.000000 0.000000 ZoomOut" ),
	TEXT("enabledevice ##mouse"),
	TEXT("rangebind ##mouse ##4 0.000000 0.000000 ZoomIn" ),
	TEXT("rangebind ##mouse ##3 0.000000 0.000000 Fire" ),
	TEXT("rangebind ##mouse ##y-axis 0.000000 0.000000 Axis2" ),
	TEXT("scale ##mouse ##y-axis 0.000400"	),
	TEXT("rangebind ##mouse ##x-axis 0.000000 0.000000 Axis1" ),
	TEXT("scale ##mouse ##x-axis 0.000400"	),
	
	TEXT("ModelAdd 0.000000 0.000000 0.000000"	),
	
#ifdef GAME_SHOW
	TEXT("LockSendUUF 0.1"),
	TEXT("LockFPS 0.014"	),
#else
	TEXT("LockSendUUF 0.1"	),
	TEXT("LockFPS 0.014"	),
#endif

	//[SPIKE] ÆÀ¿ø¾ÆÀÌµð_·¹ÀÌ´õ 
	TEXT("NameRadar 1"),

	TEXT("PVModelPitch 0.0"),
	TEXT("PVModelYaw 0.0"),
	TEXT("PVModelRoll 0.0"),

	TEXT("FreeSpectate 1"),
	TEXT("SmokeGrenadeDetail 1"),
	//=============================
	TEXT("RejectWhisper 0"),
	TEXT("RejectInvite 0"),
	
};
//---------------------------------------------

// ------------------------------------------------------------------ //
// Helpers..
// ------------------------------------------------------------------ //

inline char cc_Toupper(char theChar)
{
	if(theChar >= 'a' && theChar <= 'z')
		return 'A' + (theChar - 'a');
	else
		return theChar;
}

inline int cc_UpperStrcmp(const char *pStr1, const char *pStr2)
{
	for(;;)
	{
		if(cc_Toupper(*pStr1) != cc_Toupper(*pStr2))
			return 0;

		if(*pStr1 == 0)
			return 1;

		++pStr1;
		++pStr2;
	}
}

static char *cc_AddString(ConsoleState *pState, const char *pName)
{
	HHashElement *hElement;
	char *pString;
	int nStrLen;

	nStrLen = strlen( pName );

	// See if it's in the hash table.
	hElement = hs_FindElement( pState->m_StringHash, ( void * )pName, nStrLen + 1 );
	if( hElement )
	{
		pString = ( char * )hs_GetElementKey( hElement, LTNULL );
		return pString;
	}

	// Make a new one...
	hElement = hs_AddElement( pState->m_StringHash, ( void * )pName, nStrLen + 1 );
	if( !hElement )
		return LTNULL;

	pString = ( char * )hs_GetElementKey( hElement, LTNULL );

	return pString;
}


static LTCommandVar* cc_AddConsoleVar(ConsoleState *pState, const char *pName)
{
	LTCommandVar *pVar;
	HHashElement *hElement;
	int nStrLen;

	nStrLen = strlen( pName );

	// See if it's in the hash table.
	hElement = hs_FindElement( pState->m_VarHash, ( void * )pName, nStrLen + 1 );
	if( hElement )
	{
		return ( LTCommandVar * )hs_GetElementUserData( hElement );
	}

	// Make a new one...
	LT_MEM_TRACK_ALLOC(pVar = (LTCommandVar*)pState->Alloc(sizeof(LTCommandVar)), LT_MEM_TYPE_CONSOLE);

	if( !pVar )
		return LTNULL;
	memset(pVar, 0, sizeof(LTCommandVar));

	// Add to the var hash table...
	hElement = hs_AddElement( pState->m_VarHash, ( void * )pName, nStrLen + 1 );
	if( !hElement )
	{
		pState->Free( pVar );
		return LTNULL;
	}

	// Setup member variables...
	pVar->pVarName = ( char * )hs_GetElementKey( hElement, LTNULL );
	pVar->hElement = hElement;
	hs_SetElementUserData( hElement, ( void * )pVar );

	return pVar;
}

static LTEngineVar* cc_FindEngineVar(ConsoleState *pState, char *pName)
{
	int i;

	for(i=0; i < pState->m_nEngineVars; i++)
	{
		if(cc_UpperStrcmp(pState->m_pEngineVars[i].pVarName, pName))
			return &pState->m_pEngineVars[i];
	}

	return LTNULL;
}

static void cc_SetEngineVar(ConsoleState *pState, LTCommandVar *pVar)
{
	LTEngineVar	*pEngineVar = cc_FindEngineVar(pState, pVar->pVarName);

	if(pEngineVar)
	{
		if(pEngineVar->pCommandVarAddress)
			*pEngineVar->pCommandVarAddress = pVar;

		if(pEngineVar->pValueAddressFloat)
			*pEngineVar->pValueAddressFloat = pVar->floatVal;

		if(pEngineVar->pValueAddressLong)
			*pEngineVar->pValueAddressLong = (int32)pVar->floatVal;

		if(pEngineVar->pValueAddressString)
			*pEngineVar->pValueAddressString = pVar->pStringVal;
	}
}



// ------------------------------------------------------------------ //
// Main functions.
// ------------------------------------------------------------------ //

void cc_PrintStringColor(uint8 r, uint8 g, uint8 b, char *pMsg, ...)
{
	static const uint32 knBufferSize = 512;

	va_list marker;
	char msg[knBufferSize];

	va_start(marker, pMsg);
	LTVSNPrintF(msg, knBufferSize, pMsg, marker);
	va_end(marker);

	#ifdef DE_CLIENT_COMPILE
		con_PrintString(CONRGB(r,g,b), 0, msg);
	#endif
}

void cc_HandleCommand(ConsoleState *pState, const char *pCommand)
{
	cc_HandleCommand2(pState, pCommand, 0);
}


void cc_HandleCommand2(ConsoleState *pState, const char *pCommand, uint32 flags)
{
	cc_HandleCommand3(pState, pCommand, flags, 0);
}


void cc_HandleCommand3(ConsoleState *pState, const char *pCommand, uint32 flags, uint32 varFlags)
{
	int bNew, bChanged, bFound;
	LTCommandVar *pVar;
	LTLink *pCur;
	LTExtraCommandStruct *pExtraCommand;
	char tempNumString[50];
	int tempNum;
	char *pVarName, *pString;
	LTBOOL bForceNoSave;
	uint32 newFlags;
	ConParse parse;


	parse.Init(pCommand);
	while(parse.Parse())
	{
		if(parse.m_nArgs > 0)
		{
			// Forward the command to command structs.
			bFound = 0;

			if(!(flags & CC_NOCOMMANDS))
			{
				if(!bFound)
				{
					pCur = pState->m_ExtraCommands.m_pNext;
					while(pCur != &pState->m_ExtraCommands)
					{
						pExtraCommand = (LTExtraCommandStruct*)pCur->m_pData;

						if(cc_UpperStrcmp(pExtraCommand->pCmdName, parse.m_Args[0]))
						{
							pExtraCommand->fn(parse.m_nArgs-1, &parse.m_Args[1]);
							bFound = 1;
							break;
						}

						pCur = pCur->m_pNext;
					}
				}
			}

			// Check if it was a command...
			if( bFound )
				continue;

			// Treat it like a variable.
			if(parse.m_nArgs == 2 && !(flags & CC_NOVARS))
			{
				bChanged = 0;
				bNew = 0;

				newFlags = varFlags;
				pVarName = parse.m_Args[0];
				bForceNoSave = LTFALSE;
				if(pVarName[0] == '+')
				{
					newFlags |= VARFLAG_SAVE;
					++pVarName;
				}
				else if(pVarName[0] == '-')
				{
					bForceNoSave = LTTRUE;
					++pVarName;
				}


				// Add the variable if not already there.
				pVar = cc_FindConsoleVar(pState, pVarName);
				if(!pVar)
				{
					bNew = bChanged = 1;
					pVar = cc_AddConsoleVar(pState, pVarName);
				}

				pVar->m_VarFlags |= newFlags;
				if(bForceNoSave)
					pVar->m_VarFlags &= ~VARFLAG_SAVE;

				// Check the negation syntax.
				if(parse.m_Args[1][0] == '!' && parse.m_Args[1][1] == 0)
				{
					if(pVar->pStringVal)
					{
						if(isdigit(pVar->pStringVal[0]) || pVar->pStringVal[0] == '-')
						{
							tempNum = !((int)pVar->floatVal);
							LTSNPrintF(tempNumString, sizeof(tempNumString), "%d", tempNum);
							parse.m_Args[1] = tempNumString;
						}
					}
					else
					{
						parse.m_Args[1] = "1";
					}
				}

				// Check if it changed..
				if(pVar->pStringVal && strcmp(pVar->pStringVal, parse.m_Args[1])==0)
				{
				}
				else
				{
					if(strlen(parse.m_Args[1]) < VARBUF_LEN)
					{
						// Put it in its own buffer so we don't have to allocate or search.
						LTStrCpy(pVar->m_Buffer, parse.m_Args[1], sizeof(pVar->m_Buffer));
						pString = pVar->m_Buffer;
					}
					else
					{
						pString = cc_AddString(pState, parse.m_Args[1]);
					}

					pVar->pStringVal = pString;
					pVar->floatVal = (float)atof(parse.m_Args[1]);
					bChanged = 1;
				}

				cc_SetEngineVar(pState, pVar);

				// Call the appropriate callback.
				if(bChanged)
				{
					if(bNew)
					{
						if(pState->NewVar)
							pState->NewVar(pState, pVar);
					}
					else
					{
						if(pState->VarChange)
							pState->VarChange(pState, pVar);
					}
				}
			}
			else if(parse.m_nArgs == 1)
			{
				pVar = cc_FindConsoleVar(pState, parse.m_Args[0]);
				if(pVar)
					cc_PrintVarDescription(pState, pVar);
			}
		}
	}
}


// Sets a specific variable without having to parse it...
void cc_SetConsoleVariable(ConsoleState *pState, const char *pName, const char *pValue )
{
	int bNew, bChanged;
	LTCommandVar *pVar;
	char *pString;

	bChanged = 0;
	bNew = 0;

	// Add the variable if not already there.
	pVar = cc_FindConsoleVar(pState, pName);
	if(!pVar)
	{
		bNew = bChanged = 1;
		pVar = cc_AddConsoleVar(pState, pName);
	}

	// Try to add the string.  If it returns the same pointer var
	// already has, then the string didn't change.
	pString = cc_AddString(pState, pValue);
	if( pString != pVar->pStringVal )
	{
		pVar->pStringVal = pString;
		pVar->floatVal = (float)atof(pValue);
		bChanged = 1;
	}

	cc_SetEngineVar(pState, pVar);

	// Call the appropriate callback.
	if(bChanged)
	{
		if(bNew)
		{
			if(pState->NewVar)
				pState->NewVar(pState, pVar);
		}
		else
		{
			if(pState->VarChange)
				pState->VarChange(pState, pVar);
		}
	}
	// ±¤½Ä
	//----------------------------------------------------
#ifndef DE_SERVER_COMPILE	
	ConsoleState *pClientState = &g_pClientMgr->m_ServerConsoleMirror;	
	if( pClientState->m_VarHash == LTNULL )
	{
		pClientState->Alloc = dalloc;
		pClientState->Free = dfree;
		cc_InitState(pClientState);
	}
	
	// Add the variable if not already there.
	pVar = cc_FindConsoleVar(pClientState, pName);
	if(!pVar)
	{
		bNew = bChanged = 1;
		pVar = cc_AddConsoleVar(pClientState, pName);
	}

	// Try to add the string.  If it returns the same pointer var
	// already has, then the string didn't change.
	pString = cc_AddString(pClientState, pValue);
	if( pString != pVar->pStringVal )
	{
		pVar->pStringVal = pString;
		pVar->floatVal = (float)atof(pValue);
		bChanged = 1;
	}

	cc_SetEngineVar(pClientState, pVar);

	// Call the appropriate callback.
	if(bChanged)
	{
		if(bNew)
		{
			if(pClientState->NewVar)
				pClientState->NewVar(pClientState, pVar);
		}
		else
		{
			if(pClientState->VarChange)
				pClientState->VarChange(pClientState, pVar);
		}
	}	
#endif	// DE_SERVER_COMPILE	
	//----------------------------------------------------
}

// ±¤½Ä 
void cc_SetClientConsoleVariable(ConsoleState* pState, const char *pName, const char *pValue )
{
	if( pState->m_VarHash == LTNULL )
	{
		pState->Alloc = dalloc;
		pState->Free = dfree;
		cc_InitState(pState);
	}

	int bNew, bChanged;
	LTCommandVar *pVar;
	char *pString;

	bChanged = 0;
	bNew = 0;

	// Add the variable if not already there.
	pVar = cc_FindConsoleVar(pState, pName);
	if(!pVar)
	{
		bNew = bChanged = 1;
		pVar = cc_AddConsoleVar(pState, pName);
	}

	// Try to add the string.  If it returns the same pointer var
	// already has, then the string didn't change.
	pString = cc_AddString(pState, pValue);
	if( pString != pVar->pStringVal )
	{
		pVar->pStringVal = pString;
		pVar->floatVal = (float)atof(pValue);
		bChanged = 1;
	}

	cc_SetEngineVar(pState, pVar);

	// Call the appropriate callback.
	if(bChanged)
	{
		if(bNew)
		{
			if(pState->NewVar)
				pState->NewVar(pState, pVar);
		}
		else
		{
			if(pState->VarChange)
				pState->VarChange(pState, pVar);
		}
	}
}

void cc_InitState(ConsoleState *pState)
{
	int i;

	pState->m_StringHash = hs_CreateHashTable(500, HASH_STRING_NOCASE);
	pState->m_VarHash = hs_CreateHashTable(500, HASH_STRING_NOCASE);
	dl_TieOff(&pState->m_ExtraCommands);

	// Register the commands.
	for(i=0; i < pState->m_nCommandStructs; i++)
	{
		cc_AddCommand(pState, pState->m_pCommandStructs[i].pCmdName, pState->m_pCommandStructs[i].fn,
			pState->m_pCommandStructs[i].flags);
	}

	// Create the variables.
}

void cc_TermState(ConsoleState *pState)
{
	LTCommandVar *pVar;
	char *pString;
	HHashIterator *hIterator;
	HHashElement *hElement;
	LTLink *pCur, *pNext;
	int i;


	// Clear the console variable strings.
	for(i=0; i < pState->m_nEngineVars; i++)
	{
		if(pState->m_pEngineVars[i].pValueAddressString)
			*pState->m_pEngineVars[i].pValueAddressString = LTNULL;
	}

	// Remove the extra commands.
	pCur = pState->m_ExtraCommands.m_pNext;
	while(pCur && pCur != &pState->m_ExtraCommands)
	{
		pNext = pCur->m_pNext;
		cc_RemoveCommand(pState, (LTExtraCommandStruct*)pCur->m_pData);
		pCur = pNext;
	}

	// Delete the strings...
	hIterator = hs_GetFirstElement(pState->m_StringHash);
	while(hIterator)
	{
		hElement = hs_GetNextElement(hIterator);
		if( !hElement )
			continue;

		pString = ( char * )hs_GetElementUserData( hElement );
		pState->Free( pString );
	}
	hs_DestroyHashTable(pState->m_StringHash);

	// Delete the vars...
	hIterator = hs_GetFirstElement(pState->m_VarHash);
	while(hIterator)
	{
		hElement = hs_GetNextElement(hIterator);
		if( !hElement )
			continue;

		pVar = ( LTCommandVar * )hs_GetElementUserData( hElement );
		pState->Free( pVar );
	}
	hs_DestroyHashTable(pState->m_VarHash);

	memset(pState, 0, sizeof(ConsoleState));
}


LTExtraCommandStruct* cc_AddCommand(ConsoleState *pState,
	const char *pCmdName, LTCommandFn fn, uint32 flags)
{
	LTExtraCommandStruct *pCommand;

	LT_MEM_TRACK_ALLOC(pCommand = (LTExtraCommandStruct*)pState->Alloc(sizeof(LTExtraCommandStruct)), LT_MEM_TYPE_CONSOLE);
	if(!pCommand)
		return 0;

	pCommand->link.m_pData = pCommand;
	pCommand->fn = fn;
	pCommand->pCmdName = pCmdName;
	pCommand->flags = flags;
	dl_Insert(&pState->m_ExtraCommands, &pCommand->link);

	return pCommand;
}


void cc_RemoveCommand(ConsoleState *pState, LTExtraCommandStruct *pCommand)
{
	dl_Remove(&pCommand->link);
	pState->Free(pCommand);
}


LTExtraCommandStruct* cc_FindCommand(ConsoleState *pState, const char *pName)
{
	LTLink *pCur;
	LTExtraCommandStruct *pCommand;

	pCur = pState->m_ExtraCommands.m_pNext;
	while(pCur != &pState->m_ExtraCommands)
	{
		pCommand = (LTExtraCommandStruct*)pCur->m_pData;
		if(cc_UpperStrcmp(pCommand->pCmdName, pName))
			return pCommand;

		pCur = pCur->m_pNext;
	}

	return 0;
}


LTCommandVar* cc_FindConsoleVar(ConsoleState *pState, const char *pName)
{
	HHashElement *hElement;

	// See if it's in the hash table.
	hElement = hs_FindElement( pState->m_VarHash, ( void * )pName, strlen( pName ) + 1 );
	if( hElement )
	{
		return ( LTCommandVar * )hs_GetElementUserData( hElement );
	}

	return LTNULL;
}

// to get autoexec.ps2 to read from rez uncomment this next line
//#define __NO_SCEOPENS__
bool cc_RunConfigFile(ConsoleState *pState, const char *pFilename, uint32 flags, uint32 varFlags)
{
	// ±¤½Ä
	//--------------------------------------------------------------------
	if( strcmp( pFilename, "autoexec.cfg") == 0 )
	{
		for( int i = 0; i < AUTOEXEC_d; i++ )
		{
			cc_HandleCommand3(pState, g_autoexeccfg[i], flags, varFlags);
		}
		return true;
	}
	//--------------------------------------------------------------------

	FILE *fp;
	char line[500];

	fp = fopen(pFilename, "rt");
	if(!fp)
		return false;

	while(fgets(line, 500, fp))
	{
		cc_HandleCommand3(pState, line, flags, varFlags);
	}

	fclose(fp);
	return true;
}

bool cc_SaveConfigFile(ConsoleState *pState, const char *pFilename)
{
	// ±¤½Ä : Autoexec.cfg´Â save ÇÏÁö ¾Ê´Â´Ù.
	//----------------------------------------------------
	if( strcmp( pFilename, "autoexec.cfg") == 0 )
	{
		return true;
	}	
	//----------------------------------------------------

	FILE *fp;
	int i;
	LTCommandVar *pVar;
	HHashIterator *hIterator;
	HHashElement *hElement;

	fp = fopen(pFilename, "wt");
	if(!fp)
		return false;

	// Save the vars.
	hIterator = hs_GetFirstElement( pState->m_VarHash );
	while(hIterator)
	{
		hElement = hs_GetNextElement(hIterator);
		if( !hElement )
			continue;
		
		pVar = ( LTCommandVar * )hs_GetElementUserData( hElement );
		if(pVar->m_VarFlags & VARFLAG_SAVE)
		{
			fprintf(fp, "\"%s\" \"%s\"\n", pVar->pVarName, pVar->pStringVal);
		}
	}
	
	// Call the save functions.
	for(i=0; i < pState->m_nSaveFns; i++)
	{
		pState->m_SaveFns[i](fp);
	}
	
	fclose(fp);
	return true;
}


bool cc_SaveConfigFileFields(ConsoleState *pState, const char *pFilename, uint32 nNumValues, const char** pValues)
{
	FILE *fp;
	LTCommandVar *pVar;
	HHashIterator *hIterator;
	HHashElement *hElement;

	//validate some parameters
	if(!pValues)
		nNumValues = 0;

	if(nNumValues == 0)
		return false;

	fp = fopen(pFilename, "wt");
	if(!fp)
		return false;

	// Save the vars.
	hIterator = hs_GetFirstElement( pState->m_VarHash );
	while(hIterator)
	{
		hElement = hs_GetNextElement(hIterator);
		if( !hElement )
			continue;
		
		pVar = ( LTCommandVar * )hs_GetElementUserData( hElement );

		for(uint32 nCurrVal = 0; nCurrVal < nNumValues; nCurrVal++)
		{
			if(stricmp(pVar->pVarName, pValues[nCurrVal]) == 0)
			{
				//we have a match, write out this string
				fprintf(fp, "\"%s\" \"%s\"\n", pVar->pVarName, pVar->pStringVal);
			}
		}
	}
	
	fclose(fp);
	return true;
}

void cc_PrintVarDescription(ConsoleState *pState, LTCommandVar *pVar)
{
	if(pVar->pStringVal)
	{
		pState->ConsolePrint("%s = %s", pVar->pVarName, pVar->pStringVal);
	}
}
