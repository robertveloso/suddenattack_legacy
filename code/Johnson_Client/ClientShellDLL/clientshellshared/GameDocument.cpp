#include "StdAfx.h"
#include "GameDocument.h"
#include "ClientWeaponBase.h"
#include "ClientWeaponMgr.h"
#include "..\\SA\\SAHUDMgr.h"


CGameDocument::CGameDocument()
{
	m_bLowViolence		= LTFALSE;
	m_nRoomIndexBuffer	= -1;
	m_nChannelIndexBuffer= 0;
	m_bFirst			= LTTRUE;
	m_bCanBeCaptain		= LTFALSE;
	m_bHost				= LTFALSE;

	m_nTeam				= 255;
	m_nUniqueIndex		= -1;
	m_nFireCryptKey		= 0;

	memset( m_szLastSendNick, 0, MAX_NICK_LEN );
	memset( m_szLastRecvNick, 0, MAX_NICK_LEN );

	// ±¤½Ä
	memset(m_nPlayerModelId, 255, sizeof(uint8)*2);
	
	memset(m_nEquipList, 255, sizeof(m_nEquipList) );
		
	m_bSetRanking	= LTFALSE;
}

CGameDocument::~CGameDocument()
{
}

LTBOOL CGameDocument::IsFirst()
{
	return m_bFirst;
}

LTBOOL CGameDocument::SetFirst( LTBOOL bFirst )
{
	m_bFirst = bFirst;
	return LTTRUE;
}

LTBOOL CGameDocument::CanBeHost()
{
	return m_bCanBeCaptain;
}

LTBOOL CGameDocument::SetCanBeHost( LTBOOL bICan )
{
	m_bCanBeCaptain = bICan;
	return LTTRUE;
}

LTBOOL CGameDocument::IsHost()
{
	return m_bHost;
}

LTBOOL CGameDocument::SetHost( LTBOOL bHost )
{
	m_bHost	= bHost;
	return LTTRUE;
}

LTBOOL CGameDocument::SetID( const char* lpszID )
{
	if( !lpszID ) return LTFALSE;
	strcpy( m_szID, lpszID );
	return LTTRUE;
}

const char* CGameDocument::GetID()
{
	return m_szID;
}

LTBOOL CGameDocument::SetNick( const char* lpszNick )
{
	if( !lpszNick ) return LTFALSE;
	strcpy( m_szNick, lpszNick );
	return LTTRUE;
}

const char* CGameDocument::GetNick()
{
	return m_szNick;
}

LTBOOL CGameDocument::SetSearchUser( const char* lpszNick )
{
	if( !lpszNick ) return LTFALSE;
	strncpy( m_szSearchUserNick, lpszNick, MAX_NICK_LEN-1 );
	return LTTRUE;
}

const char* CGameDocument::GetSearchUser()
{
	return m_szSearchUserNick;
}

LTBOOL CGameDocument::SetUserInfoNick( const char* lpszNick )
{
	if( !lpszNick ) return LTFALSE;
	strncpy( m_szUserInfoNick, lpszNick, MAX_NICK_LEN-1 );
	return LTTRUE;
}

const char* CGameDocument::GetUserInfoNick()
{
	return m_szUserInfoNick;
}

LTBOOL CGameDocument::SetUserInfoGrade( unsigned int nGrade )
{
	m_nUserInfoGrade = nGrade;
	return LTTRUE;
}

unsigned int CGameDocument::GetUserInfoGrade()
{
	return m_nUserInfoGrade;
}

LTBOOL CGameDocument::SetGrade( unsigned int nGrade )
{
	m_nGrade = nGrade;
	return LTTRUE;
}

unsigned int CGameDocument::GetGrade()
{
	return m_nGrade;
}

LTBOOL CGameDocument::SetExp( __int64 nExp )
{
	m_nExp = nExp;
	if( m_nExp < 0 ) m_nExp = 0;
	return LTTRUE;
}

__int64 CGameDocument::GetExp()
{
	return m_nExp;
}

LTBOOL CGameDocument::SetKill( unsigned int nKill )
{
	m_nKill = nKill;
	return LTTRUE;
}

unsigned int CGameDocument::GetKill()
{
	return m_nKill;
}

LTBOOL CGameDocument::SetDeath( unsigned int nDeath )
{
	m_nDeath = nDeath;
	return LTTRUE;
}

unsigned int CGameDocument::GetDeath()
{
	return m_nDeath;
}

LTBOOL CGameDocument::SetMoney( __int64 nMoney )
{
	m_nMoney = nMoney;
	if( m_nMoney < 0 ) m_nMoney = 0;
	return LTTRUE;
}

__int64 CGameDocument::GetMoney()
{
	return m_nMoney;
}

LTBOOL CGameDocument::SetTeam( int nTeam )
{
	m_nTeam	= nTeam;
	return LTTRUE;
}

int CGameDocument::GetTeam()
{
	return m_nTeam;
}

LTBOOL CGameDocument::SetWinCount( uint32 nWin )
{
	m_nWin = nWin;
	return LTTRUE;
}

unsigned int CGameDocument::GetWinCount()
{
	return m_nWin;
}

LTBOOL CGameDocument::SetLoseCount( uint32 nLose )
{
	m_nLose = nLose;
	return LTTRUE;
}

unsigned int CGameDocument::GetLoseCount()
{
	return m_nLose;
}

LTBOOL CGameDocument::SetDrawCount( uint32 nDraw )
{
	m_nDraw = nDraw;
	return LTTRUE;
}

unsigned int CGameDocument::GetDrawCount()
{
	return m_nDraw;
}

LTBOOL CGameDocument::SetChannelIndex( int nIndex )
{
	m_nChannelIndexBuffer	= nIndex;
	return LTTRUE;
}

int CGameDocument::GetChannelIndex()
{
	return m_nChannelIndexBuffer;
}

LTBOOL CGameDocument::SetRoomIndex( int nIndex )
{
	m_nRoomIndexBuffer	= nIndex;
	return LTTRUE;
}

int CGameDocument::GetRoomIndex()
{
	return m_nRoomIndexBuffer;
}

LTBOOL CGameDocument::SetTitle( const char* lpszTitle )
{
	if( !lpszTitle ) return LTFALSE;
	memset( m_szTitleBuffer, 0, MAX_R_TITLE_LEN );
	strncpy( m_szTitleBuffer, lpszTitle, MAX_R_TITLE_LEN-1 );
	
	return LTTRUE;
}

const char* CGameDocument::GetTitle()
{
	return m_szTitleBuffer;
}

LTBOOL CGameDocument::SetCaptain( LTBOOL bCaptain )
{
	m_bCaptain = bCaptain;
	return LTTRUE;
}

LTBOOL CGameDocument::IsCaptain()
{
	return m_bCaptain;
}

LTBOOL CGameDocument::SetPassword( const char* lpszPassword )
{
	if( !lpszPassword ) return LTFALSE;
	strcpy( m_szPasswordBuffer, lpszPassword );

	return LTTRUE;
}

const char* CGameDocument::GetPassword()
{
	return m_szPasswordBuffer;
}

LTBOOL CGameDocument::SetLastSendNick( const char* lpszNick )
{
	if( LTNULL == lpszNick || strlen(lpszNick) == 0 ) return LTFALSE;
	strcpy( m_szLastSendNick, lpszNick );
	return LTTRUE;
}

const char* CGameDocument::GetLastSendNick()
{
	return m_szLastSendNick;
}

LTBOOL CGameDocument::SetLastRecvNick( const char* lpszNick )
{
	if( LTNULL == lpszNick || strlen(lpszNick) == 0 ) return LTFALSE;
	strcpy( m_szLastRecvNick, lpszNick );
	return LTTRUE;
}

const char* CGameDocument::GetLastRecvNick()
{
	return m_szLastRecvNick;
}

void CGameDocument::SetCurrentMission( int nIndex )
{
	m_nCurrentMissionIndex	= nIndex;
}

int CGameDocument::GetCurrentMission()
{
	return m_nCurrentMissionIndex;
}


void CGameDocument::SetMandateHost( LTBOOL bMandateHost )
{
	m_bMandateHost = bMandateHost;
}

LTBOOL CGameDocument::IsMandateHost()
{
	return m_bMandateHost;
}

void CGameDocument::SetRejoinHost( LTBOOL bRejoinHost )
{
	m_bRejoinHost = bRejoinHost;
}

LTBOOL CGameDocument::IsRejoinHost()
{
	return m_bRejoinHost;
}

LTBOOL CGameDocument::BackupCurrentState()
{
	HOBJECT hObj = g_pPlayerMgr->GetServerObject();
	if( !hObj ) return LTFALSE;

	LTVector	vTemp;
	int nSelWeaponID = WMGR_INVALID_ID;
	CClientWeaponMgr* pWpnMgr;
	IClientWeaponBase* pWeaponBase;
	WEAPON const* pWeapon;

	// Mission
	BackupMissionInfo();

	// Pos	
	g_pLTClient->GetObjectPos( hObj, &vTemp );
	BackupPlayerPos( vTemp );

	// Rot
	vTemp.x = g_pPlayerMgr->GetPitch();
	vTemp.y = g_pPlayerMgr->GetYaw();
	vTemp.z = g_pPlayerMgr->GetRoll();
	BackupPlayerPYR( vTemp );

	//HP, AP
	BackupPlayerHPAP( g_pPlayerStats->GetHealth(),
								g_pPlayerStats->GetArmor() );
	
	//Time
	BackupWorldTime( g_pHUDTimer->GetTime() );

	if( g_pPlayerStats->GetHealth() <= 0 )
	{
		BackupPlayerRespawnTime( g_pPlayerMgr->GetRespawnTime()-g_pLTClient->GetTime() );
		return LTTRUE;
	}

	switch( g_pGameClientShell->GetGameType() )
	{
	case eGameTypeSurvival:
		BackupAttachState( g_pPlayerMgr->IsAttached() );
		break;
	case eGameTypeTeamDeathmatch:
	default:
		BackupAttachState( LTFALSE );
		break;
	}

	//Weapon
	pWpnMgr = g_pPlayerMgr->GetClientWeaponMgr();
	if( !pWpnMgr ) return LTFALSE;

	pWeaponBase = pWpnMgr->GetCurrentClientWeapon();
	if( pWeaponBase ) nSelWeaponID = pWeaponBase->GetWeaponId();

	int* pnWpnList = g_pPlayerStats->GetOrderedWeaponId();
	if( !pnWpnList ) return LTFALSE;

	for( int i=0 ; i<5 ; ++i ) //MAX_WEAPON_CLASS
	{
		pWeaponBase = pWpnMgr->GetWeaponFromID(	pnWpnList[i] );
		if( pWeaponBase )
		{
			pWeapon = g_pWeaponMgr->GetWeapon( pWeaponBase->GetWeaponId() );
			if( pWeapon )
			{
				BackupPlayerWeapon( i,
									pWeaponBase->GetWeaponId(), pWeapon->nDefaultAmmoId,
									g_pPlayerStats->GetAmmoCount(pWeapon->nDefaultAmmoId), pWeaponBase->GetAmmoInClip(),
									(pWeaponBase->GetWeaponId()==nSelWeaponID) );
			}
			else
			{
				BackupPlayerWeapon( i,
									WMGR_INVALID_ID, WMGR_INVALID_ID,
									0, 0, 
									LTFALSE );
			}
		}
		else
		{
			BackupPlayerWeapon( i,
							    WMGR_INVALID_ID, WMGR_INVALID_ID,
								0, 0, 
								LTFALSE );
		}
	}

	return LTTRUE;	
}

void CGameDocument::BackupMissionInfo()
{
	switch( g_pGameClientShell->GetGameType() )
	{
	case eGameTypeSurvival:
		m_bMissionInfo = true;
		m_fTimeBombTime	= g_pHUDTimeBombTimer->GetTime();
		m_vTimeBombPos = g_pPlayerMgr->GetTimeBombPos();
		m_rTimeBombRot.Init();
		break;

	case eGameTypeTeamDeathmatch:
	default:
		m_bMissionInfo = false;		
		break;
	}
}

void CGameDocument::BackupPlayerPos( LTVector& vPos )
{
	m_BackupLocalInfo.vPos = vPos;
}

void CGameDocument::BackupPlayerPYR( LTVector& vPYR )
{
	m_BackupLocalInfo.vPYR	= vPYR;
}

void CGameDocument::BackupPlayerHPAP( uint8 nHP, uint8 nAP )
{
	m_BackupLocalInfo.nHP = nHP;
	m_BackupLocalInfo.nAP = nAP;
}

void CGameDocument::BackupPlayerWeapon( uint32 nClass, uint32 nWeaponID, uint32 nAmmoID,
									    uint32 nAmmoCount, uint32 nAmmoInClip,
										LTBOOL bSeleted )
{
	if( nClass < 5 ) //MAX_WEAPON_CLASS
	{
		m_BackupLocalInfo.WeaponData[nClass].bSelected		= bSeleted;
		m_BackupLocalInfo.WeaponData[nClass].nWeaponID		= nWeaponID;
		m_BackupLocalInfo.WeaponData[nClass].nAmmoID		= nAmmoID;
		m_BackupLocalInfo.WeaponData[nClass].nAmmoCount		= nAmmoCount;
		m_BackupLocalInfo.WeaponData[nClass].nAmmoInClip	= nAmmoInClip;
	}
}

void CGameDocument::BackupWorldTime( float fTime )
{
	m_BackupLocalInfo.fWorldTime	= LTMAX( 0.0f, fTime );
}

void CGameDocument::BackupAttachState( LTBOOL bAttached )
{
	m_BackupLocalInfo.bAttached = bAttached;
}

void CGameDocument::BackupPlayerRespawnTime( float fTime )
{
	m_BackupLocalInfo.fRespawnTime	= LTMAX( 0.0f, fTime );
}

LTVector& CGameDocument::GetTimeBombPos()
{
	return m_vTimeBombPos;
}

LTRotation& CGameDocument::GetTimeBombRot()
{
	return m_rTimeBombRot;
}

float CGameDocument::GetTimeBombTime()
{
	return m_fTimeBombTime;
}

uint8 CGameDocument::GetBackupSelectedWeaponID()
{
	for( int i=0 ; i<5 ; ++i ) //MAX_WEAPON_CLASS
	{
		if( m_BackupLocalInfo.WeaponData[i].bSelected ) return m_BackupLocalInfo.WeaponData[i].nWeaponID;
	}

	return WMGR_INVALID_ID;
}

LTVector& CGameDocument::GetBackupPlayerPos()
{
	return m_BackupLocalInfo.vPos;
}

LTVector& CGameDocument::GetBackupPlayerPYR()
{
	return m_BackupLocalInfo.vPYR;
}

uint8 CGameDocument::GetBackupPlayerHP()
{
	return m_BackupLocalInfo.nHP;
}

uint8 CGameDocument::GetBackupPlayerAP()
{
	return m_BackupLocalInfo.nAP;
}

T_BACKUP_WEAPONptr CGameDocument::GetBackupPlayerWeapon( uint32 nClass )
{
	if( nClass < 4 )
	{
		return &m_BackupLocalInfo.WeaponData[nClass];
	}

	return LTNULL;
}

float CGameDocument::GetBackupWorldTime()
{
	return m_BackupLocalInfo.fWorldTime;
}

LTBOOL CGameDocument::GetBackupAttachState()
{
	return m_BackupLocalInfo.bAttached;
}

float CGameDocument::GetBackupPlayerRespawnTime()
{
	return m_BackupLocalInfo.fRespawnTime;
}

void CGameDocument::SetWorldName( const char* lpszWorldName )
{
	if( lpszWorldName )
	{
		strcpy( m_szWorldName, lpszWorldName );
		strcat( m_szWorldName, ".dat" );
	}
	else
	{
		strcpy( m_szWorldName, "" );
	}
}

const char* CGameDocument::GetWorldName()
{
	return (const char*)m_szWorldName;
}

void CGameDocument::SetLowViolence( LTBOOL bEnable )
{
	m_bLowViolence = bEnable;
}

LTBOOL CGameDocument::IsLowViolence()
{
	return m_bLowViolence;
}

void CGameDocument::SetUniqueIndex( int nIndex )
{
	m_nUniqueIndex = nIndex;

	m_nFireCryptKey = m_nUniqueIndex % 132;
}

int CGameDocument::GetUniqueIndex()
{
	return m_nUniqueIndex;
}

void CGameDocument::SetCreateRoomParam( int nMapIndex, int nGameType, int nMaxUsers, int nTimeLimit, int nKillLimit, int nRoundLimit )
{
	m_CreateRoomParam.nMapIndex	= nMapIndex;
	m_CreateRoomParam.nGameType	= nGameType;
	m_CreateRoomParam.nMaxUsers = nMaxUsers;
	m_CreateRoomParam.nTimeLimit = nTimeLimit;
	m_CreateRoomParam.nKillLimit = nKillLimit;
	m_CreateRoomParam.nRoundLimit = nRoundLimit;
}

T_CREATEROOM_PARAMptr CGameDocument::GetCreateRoomParam()
{
	return &m_CreateRoomParam;
}

// ±¤½Ä
//----------------------------------------------------------------
void CGameDocument::SetUserModelId( uint8 nModelId, int nTeam )
{
	if( nTeam < TEAM_A || nTeam > TEAM_B ) return;

	if( nModelId >= g_pModelButeMgr->GetNumModels() )
	{
		m_nPlayerModelId[nTeam] = nTeam;
	}
	else if( nTeam == TEAM_A && (nModelId%2) == 1 )
	{
		m_nPlayerModelId[nTeam] = nTeam;
	}
	else if( nTeam == TEAM_B && (nModelId%2) == 0 )
	{
		m_nPlayerModelId[nTeam] = nTeam;
	}
	else
	{
		m_nPlayerModelId[nTeam] = nModelId;
	}
}

uint8 CGameDocument::GetUserModelId( int nTeam )
{
	if( nTeam < TEAM_A || nTeam > TEAM_B ) return 255;

	return m_nPlayerModelId[nTeam];
}

void CGameDocument::SetRoomOption( T_ROOM_OPTIONptr pRoomOption )
{
	m_RoomOption.bFriendlyFire	= pRoomOption->bFriendlyFire;
	m_RoomOption.bInstrusion	= pRoomOption->bInstrusion;
	m_RoomOption.bTeamBalance	= pRoomOption->bTeamBalance;
	m_RoomOption.nGameType		= pRoomOption->nGameType;
	m_RoomOption.nKillLimit		= pRoomOption->nKillLimit;
	m_RoomOption.nMapIndex		= pRoomOption->nMapIndex;
	m_RoomOption.nMaxUsers		= pRoomOption->nMaxUsers;
	m_RoomOption.nRoundLimit	= pRoomOption->nRoundLimit;
	m_RoomOption.nTimeLimit		= pRoomOption->nTimeLimit;
	m_RoomOption.bFreeSpectator	= pRoomOption->bFreeSpectator;
}

T_ROOM_OPTIONptr CGameDocument::GetRoomOption()
{
	return &m_RoomOption;
}

void CGameDocument::SetChannelName( const char* lpszName )
{
	if( lpszName )
	{
		strcpy( m_szChannelName, lpszName );
	}
	else
	{
		m_szChannelName[0] = LTNULL;
	}
}

const char* CGameDocument::GetChannelName()
{
	return m_szChannelName;
}

void CGameDocument::BeginClientHWInfo()
{
	memset( m_szClientHWInfo, 0, MAX_HW_INFO_LEN );
	m_nHWInfoPos = 0;
}

void CGameDocument::AddClientHWInfo( const char* lpszInfo )
{
	if( LTNULL == lpszInfo ) return;
	if( strlen(lpszInfo)+m_nHWInfoPos >= MAX_HW_INFO_LEN ) return;
	
	strcat( m_szClientHWInfo, lpszInfo );

	m_nHWInfoPos += strlen(lpszInfo);
}

void CGameDocument::EndClientHWInfo()
{
	m_szClientHWInfo[MAX_HW_INFO_LEN-2] = '|';
	m_szClientHWInfo[MAX_HW_INFO_LEN-1] = LTNULL;
}

const char* CGameDocument::GetClientHWInfo()
{
	return m_szClientHWInfo;
}

void CGameDocument::SetRanking( const char* lpszInfo )
{
	if( lpszInfo )
	{
		strcpy( m_szRankingInfo, lpszInfo );
		m_bSetRanking = LTTRUE;
	}
}

void CGameDocument::SetEquipItem( uint8 nItemIndex, uint8 nArrayIndex )
{
	if( nArrayIndex < 0 || nArrayIndex >= MAX_EQUIP_ITEM ) return;

	m_nEquipList[nArrayIndex] = nItemIndex;
}

LTBOOL CGameDocument::IsAllItemSet()
{
	for( int i=0 ; i<6 /* Character 2  Weapon 4 */ ; ++i )
	{
		if( m_nEquipList[i] == 255 ) return LTFALSE;
	}

	return LTTRUE;
}

uint8 CGameDocument::GetEquipCharacter_A()
{
	return m_nEquipList[ITEM_IDX_CHAR_A];
}

uint8 CGameDocument::GetEquipCharacter_B()
{
	return m_nEquipList[ITEM_IDX_CHAR_B];
}

uint8 CGameDocument::GetEquipWeapon( int nClass )
{
	return m_nEquipList[nClass+1];
}

LTBOOL CGameDocument::IsWaitEquipList()
{
	return m_bWaitEquipList;
}

void CGameDocument::SetWaitEquipList( LTBOOL bWait )
{
	m_bWaitEquipList = bWait;
}

uint8 CGameDocument::EncryptFireValue( uint8 nVal )
{
	return ((nVal+m_nFireCryptKey)^m_nFireCryptKey);
}