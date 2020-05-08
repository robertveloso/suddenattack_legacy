//-----------------------------------------------------------------------------
// 파  일 : ButeFileDefines.h
//-----------------------------------------------------------------------------
// 목  적 : 각종 bute file 정의
// 날  자 : [2005, 1]
// 작성자 : [FPS 팀]
// 소  유 : (c) GameHi~
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 설명 : 상수정의
//-----------------------------------------------------------------------------
#ifndef __BUTE_FILE_DEFINES_H__
#define __BUTE_FILE_DEFINES_H__

// AI   
#define AI_DEFAULT_BUTES						"SA_SCRIPTS\\AIButes.txt"
#define AI_GOALS_DEFAULT_FILE					"SA_SCRIPTS\\AIGoals.txt"

// Animation
#define	MODELBMGR_MODEL_DEFAULT_ANIMATION		CString("SA_SCRIPTS\\Ani_STUD.txt")

// Attachment
#define	ATTACHMENTS_DEFAULT_FILE				"SA_SCRIPTS\\Attachments.txt"

// Command
#define CBMGR_DEFAULT_FILE						"SA_SCRIPTS\\Commands.txt"

// FX
#define DTMGR_DEFAULT_FILE						"SA_SCRIPTS\\DamageTypes.txt"
#define DEBRISMGR_DEFAULT_FILE					"SA_SCRIPTS\\Debris.txt"
#define FXBMGR_DEFAULT_FILE						"SA_SCRIPTS\\Fx.txt"
#define DMGFXMGR_DEFAULT_FILE					"SA_SCRIPTS\\DamageFX.txt"

// Gadget
#define	GTMGR_DEFAULT_FILE						"SA_SCRIPTS\\GadgetTargets.txt"

// GameShell
#define CLIENT_BUTES_DEFAULT_FILE				"SA_SCRIPTS\\ClientButes.txt"
#define SERVER_BUTES_DEFAULT_FILE				"SA_SCRIPTS\\ServerButes.txt"

// Interface
#define LO_DEFAULT_FILE							"SA_SCRIPTS\\Layout.txt"

// Item
#define KEY_DEFAULT_FILE						"SA_SCRIPTS\\KeyItems.txt"
#define SI_DEFAULT_FILE							"SA_SCRIPTS\\SearchItems.txt"
#define POPUP_DEFAULT_FILE						"SA_SCRIPTS\\PopupItems.txt"
#define INVENTORY_DEFAULT_FILE					"SA_SCRIPTS\\Inventory.txt"
#define INTELMGR_DEFAULT_FILE					"SA_SCRIPTS\\IntelItems.txt"

// Mission
#define MISSION_DEFAULT_FILE					"SA_SCRIPTS\\Mission.txt"
#define MISSION_COOP_FILE						"SA_SCRIPTS\\MissionCoop.txt" 
#define MISSION_DM_FILE							"SA_SCRIPTS\\MissionDM.txt"  // Death Mission.
#define MISSION_TDM_FILE						"SA_SCRIPTS\\MissionTDM.txt"  // TeamDeathMission.
#define MISSION_DD_FILE							"SA_SCRIPTS\\MissionDD.txt"  // DoomsDay Mission.


// Model
#define MBMGR_DEFAULT_FILE						"SA_SCRIPTS\\ModelButes.txt"
//#define	MODELBMGR_MODEL_DEFAULT_MODELFILE		CString("Li_Characters\\Models\\STUD\\STUD_Base.ltb")
#define	MODELBMGR_MODEL_DEFAULT_MODELFILE		CString("SA_Characters\\Models\\STUD\\STUD_Base.ltb")
#define	MODELBMGR_MODEL_DEFAULT_HANDSSKIN		CString("SA_Weapons\\Skins\\PV\\Hand.dtx")
// 광식
#define MODELBMGR_MODEL_DEFAULT_SLEEVESKIN		CString("SA_Weapons\\Skins\\PV\\Sleeve.dtx")
#define MODELBMGR_MODEL_DEFAULT_ARMSSKIN		CString("SA_Weapons\\Skins\\PV\\Arms.dtx")
#define MODELBMGR_MODEL_DEFAULT_SHOP_ICON		CString("SA_Interface\\model\\CharId0")

// ObjectTypes
#define ATMGR_DEFAULT_FILE						"SA_SCRIPTS\\ActivateTypes.txt"
#define	RTMGR_DEFAULT_FILE						"SA_SCRIPTS\\RadarTypes.txt"
#define TTMGR_DEFAULT_FILE						"SA_SCRIPTS\\TriggerTypes.txt"

// Prop
#define PTMGR_DEFAULT_FILE						"SA_SCRIPTS\\PropTypes.txt"

// Relation
#define RELATION_DATA_DEFAULT_FILE				"SA_SCRIPTS\\RelationData.txt"

// Skill
#define SMGR_DEFAULT_FILE						"SA_SCRIPTS\\Skills.txt"

// Sound 
#define SOUND_BUTES_DEFAULT_FILE				"SA_SCRIPTS\\SndButes.txt"
#define SFM_DEFAULT_FILE						"SA_SCRIPTS\\SndFilters.txt"
#define CSNDMGR_DEFAULT_FILE					"SA_SCRIPTS\\ClientSnd.txt"
#define MUSIC_DEFAULT_FILE						"SA_SCRIPTS\\Music.txt"
#define SSNDMGR_DEFAULT_FILE					"SA_SCRIPTS\\ServerSnd.txt"

// Surface
#define SRFMGR_DEFAULT_FILE						"SA_SCRIPTS\\Surface.txt"

// Weapon
#define WEAPON_DEFAULT_FILE						"SA_SCRIPTS\\Weapons.txt"
#define WEAPON_DEFAULT_MULTI_FILE				"SA_SCRIPTS\\weapons_MP.txt"

// Weightset
#define MODELBMGR_MODEL_DEFAULT_WEIGHTSET		CString("SA_SCRIPTS\\Ws_Default.txt")

#endif // __BUTE_FILE_DEFINES_H__