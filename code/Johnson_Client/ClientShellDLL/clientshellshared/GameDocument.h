#ifndef __MURSUM_GAME_DOCUMENT_H__
#define __MURSUM_GAME_DOCUMENT_H__


//////////////////////////////////////////////////////////////////////////
// 게임에서 다 쓰는 디파인은 여기다 하자		by minjin ( advised by MURSUM )

#ifdef GAME_SHOW
#define DEFAULT_CSENDRATE				15.0f//60.0f
#else
#define DEFAULT_CSENDRATE				15.0f
#endif
#define MAX_HW_INFO_LEN					600

//[SPIKE] MissionTextList
enum MissionTextList
{
	MT_PLANT_POSSIBLE_AREA		= 0,
	MT_DEFUSE_POSSIBLE_AREA		= 1,
	MT_TIMEBOMB_PLANTED			= 2,
	//	MT_DETONATED_TIMER,
	MT_DETONATED				= 3,
	MT_DEFUSED					= 4,
	
	MT_INVALIDATE = 100,
};

//////////////////////////////////////////////////////////////////////////
// SA Macros..

#ifdef _FINAL
#define ONLY_DEBUG(__statement)
#else
#define ONLY_DEBUG(__statement)		__statement;
#endif //_FINAL

#define SAFE_DELETE(__ptr)	\
	{\
		if ( __ptr ) \
		{\
			delete (__ptr);  (__ptr) = NULL;\
		}\
	}

#define SAFE_DELETE_ARRAY(__ptr)	\
	{\
		if ( __ptr ) \
		{\
			delete [] (__ptr);  (__ptr) = NULL;\
		}\
	}


#define GET_TEXTURE_FROM_LAYOUT_MNGR( __hTexture__, __ScriptTag__, __ScriptString__ )	\
	{\
		char szStr[1024] = {0,};	\
		g_pLayoutMgr->GetString( __ScriptTag__, __ScriptString__, szStr, sizeof(szStr) );	\
		__hTexture__ = g_pInterfaceResMgr->GetTexture(szStr);	\
	}

#define GET_RGBA_FROM_LAYOUT_MNGR( __hColorRef__, __ScriptTag__, __Script_String__ )	\
	{\
		LTRect rtRect;	\
		rtRect = g_pLayoutMgr->GetRect( __ScriptTag__, __Script_String__ );	\
		__hColorRef__ = SET_ARGB( rtRect.bottom, rtRect.left, rtRect.top, rtRect.right );	\
	}

//ItemIndex
#define INVALID_ITEM_CODE	255
#define ITEM_IDX_CHAR_A		0
#define ITEM_IDX_CHAR_B		1
#define ITEM_IDX_WPN_1		2
#define ITEM_IDX_WPN_2		3
#define ITEM_IDX_WPN_3		4
#define ITEM_IDX_WPN_4		5
#define ITEM_IDX_WPN_5		6
#define ITEM_IDX_WPN_6		7

#define MAX_PRICE_SET		4
#define MAX_EQUIP_ITEM		12
//////////////////////////////////////////////////////////////////////////

typedef struct _tagBACKUP_WEAPON
{
	LTBOOL	bSelected;
	uint32	nWeaponID;
	uint32	nAmmoID;
	uint32	nAmmoCount;
	uint32	nAmmoInClip;
} T_BACKUP_WEAPON, *T_BACKUP_WEAPONptr;

typedef struct _tagBACKUP_LOCAL_INFO
{
	LTVector		vPos;
	LTVector		vPYR;
	uint8			nHP;
	uint8			nAP;
	float			fWorldTime;
	float			fRespawnTime;	
	T_BACKUP_WEAPON	WeaponData[5]; //MAX_WEAPON_CLASS
	LTBOOL			bAttached;
} T_BACKUP_LOCAL_INFO, *T_BACKUP_LOCAL_INFOptr;

typedef struct _tagCREATEROOM_PARAM
{
	int nMapIndex;
	int nGameType;
	int	nMaxUsers;
	int nTimeLimit;
	int nKillLimit;
	int nRoundLimit;
} T_CREATEROOM_PARAM, *T_CREATEROOM_PARAMptr;

typedef struct _tagROOMOPTION
{
	int nGameType;
	int	nMapIndex;
	int nMaxUsers;
	int nTimeLimit;
	int nKillLimit;
	int nRoundLimit;
	LTBOOL bInstrusion;
	LTBOOL bTeamBalance;
	LTBOOL bFreeSpectator;
	bool bFriendlyFire;
} T_ROOM_OPTION, *T_ROOM_OPTIONptr;			// 방정보도 게임도큐먼트에서 가지고 있는다

class CGameDocument
{
public:
	CGameDocument();
	~CGameDocument();

	static CGameDocument& GetSingleton()
	{
		static CGameDocument __stMURSUM_CGameDocument__;
		return __stMURSUM_CGameDocument__;
	}

	LTBOOL			IsFirst();
	LTBOOL			SetFirst( LTBOOL bFirst );
	
	LTBOOL			CanBeHost();
	LTBOOL			SetCanBeHost( LTBOOL bICan );
	
	LTBOOL			SetID( const char* lpszID );
	const char*		GetID();

	LTBOOL			SetNick( const char* lpszNick );
	const char*		GetNick();

	LTBOOL			SetSearchUser( const char* lpszNick );
	const char*		GetSearchUser();

	LTBOOL			SetUserInfoNick( const char* lpszNick );
	const char*		GetUserInfoNick();

	LTBOOL			SetUserInfoGrade( unsigned int nGrade );
	unsigned int	GetUserInfoGrade();

	LTBOOL			SetGrade( unsigned int nGrade );
	unsigned int	GetGrade();
	
	LTBOOL			SetExp( __int64 nExp );
	__int64			GetExp();

	LTBOOL			SetKill( unsigned int nKill );
	unsigned int	GetKill();

	LTBOOL			SetDeath( unsigned int nDeath );
	unsigned int	GetDeath();

	LTBOOL			SetMoney( __int64 nMoney );
	__int64			GetMoney();

	LTBOOL			SetTeam( int nTeam );
	int				GetTeam();	
	
	LTBOOL			SetWinCount( uint32 nWin );
	unsigned int	GetWinCount();

	LTBOOL			SetLoseCount( uint32 nLose );
	unsigned int	GetLoseCount();

	LTBOOL			SetDrawCount( uint32 nDraw );
	unsigned int	GetDrawCount();

	LTBOOL			SetChannelIndex( int nIndex );
	int				GetChannelIndex();

	LTBOOL			SetRoomIndex( int nIndex );
	int				GetRoomIndex();
	
	LTBOOL			SetTitle( const char* lpszTitle );
	const char*		GetTitle();

	LTBOOL			SetCaptain( LTBOOL bCaptain );
	LTBOOL			IsCaptain();

	void			SetCreateRoomParam( int nMapIndex, int nGameType, int nMaxUsers, int nTimeLimit, int nKillLimit, int nRoundLimit );
	T_CREATEROOM_PARAMptr	GetCreateRoomParam();

	LTBOOL			SetPassword( const char* lpszPassword );
	const char*		GetPassword();
	
	LTBOOL			IsHost();
	LTBOOL			SetHost( LTBOOL bHost );

	LTBOOL			SetLastSendNick( const char* lpszNick );
	const char*		GetLastSendNick();

	LTBOOL			SetLastRecvNick( const char* lpszNick );
	const char*		GetLastRecvNick();
	
	// 광식 [05.03.18] : Model을 Team에 따라 구분하기.
	void			SetUserModelId( uint8 nModelId, int nTeam );
	uint8			GetUserModelId( int nTeam );
	
	void			SetCurrentMission( int nIndex );
	int				GetCurrentMission();

	void			SetMandateHost( LTBOOL bMandateHost );
	LTBOOL			IsMandateHost();
	void			SetRejoinHost( LTBOOL bRejoinHost );
	LTBOOL			IsRejoinHost();
	
	LTBOOL			BackupCurrentState();
	void			BackupMissionInfo();
	void			BackupPlayerPos( LTVector& vPos );
	void			BackupPlayerPYR( LTVector& vPYR );
	void			BackupPlayerHPAP( uint8 nHP, uint8 nAP );
	void			BackupPlayerWeapon( uint32 nClass,
										uint32 nWeaponID, uint32 nAmmoID,
										uint32 nAmmoCount, uint32 nAmmoInClip,
										LTBOOL bSelected );
	void			BackupPlayerRespawnTime( float fTime );
	void			BackupWorldTime( float fTime );
	void			BackupAttachState( LTBOOL bAttached );

	LTVector&		GetTimeBombPos();
	LTRotation&		GetTimeBombRot();
	float			GetTimeBombTime();
	LTVector&		GetBackupPlayerPos();
	LTVector&		GetBackupPlayerPYR();
	uint8			GetBackupPlayerHP();
	uint8			GetBackupPlayerAP();
	T_BACKUP_WEAPONptr	GetBackupPlayerWeapon( uint32 nClass );
	uint8			GetBackupSelectedWeaponID();
	float			GetBackupPlayerRespawnTime();
	float			GetBackupWorldTime();
	LTBOOL			GetBackupAttachState();

	void			SetWorldName( const char* lpszWorldName );
	const char*		GetWorldName();

	void			SetLowViolence( LTBOOL bEnable );
	LTBOOL			IsLowViolence();

	void			SetUniqueIndex( int nIndex );
	int				GetUniqueIndex();

	void			SetRoomOption( T_ROOM_OPTIONptr pRoomOption );
	T_ROOM_OPTIONptr GetRoomOption();

	void			SetChannelName( const char* lpszName );
	const char*		GetChannelName();

	void			BeginClientHWInfo();
	void			AddClientHWInfo( const char* lpszInfo );
	void			EndClientHWInfo();
	const char*		GetClientHWInfo();

	void			SetRanking( const char* lpszInfo );
	LTBOOL			IsRankingInfo() { return m_bSetRanking; }
	const char*		GetRanking() { return m_szRankingInfo; }

	void			SetEquipItem( uint8 nItemIndex, uint8 nArrayIndex );
	LTBOOL			IsAllItemSet();
	uint8			GetEquipCharacter_A();
	uint8			GetEquipCharacter_B();
	uint8			GetEquipWeapon( int nClass );//1~
	LTBOOL			IsWaitEquipList();
	void			SetWaitEquipList( LTBOOL bWait );

	uint8			EncryptFireValue( uint8 nVal );

#ifdef BIG_FAT_MODE_TEST
	void			SetBigHeadMode( LTBOOL bBig ) { m_bBigHeadMode = bBig; }
	LTBOOL			IsBigHeadMode() { return m_bBigHeadMode; }
#endif
	
protected:
	//Local Info
	LTBOOL			m_bFirst;
	LTBOOL			m_bCanBeCaptain;
	LTBOOL			m_bHost;
	int				m_nUniqueIndex;
	char			m_szID[MAX_ID_LEN+1];
	char			m_szNick[MAX_NICK_LEN];
	char			m_szSearchUserNick[MAX_NICK_LEN];
	char			m_szUserInfoNick[MAX_NICK_LEN];
	unsigned int	m_nUserInfoGrade;
	unsigned int	m_nGrade;
	__int64			m_nExp;
	unsigned int	m_nKill;
	unsigned int	m_nDeath;
	__int64			m_nMoney;
	int				m_nTeam;
	uint32			m_nWin;
	uint32			m_nLose;
	uint32			m_nDraw;
	
	int				m_nCurrentMissionIndex;	

	//channel
	int				m_nChannelIndexBuffer;
	char			m_szChannelName[21];

	//Temp RoomInfo
	int				m_nRoomIndexBuffer;
	char			m_szTitleBuffer[MAX_R_TITLE_LEN];
	char			m_szPasswordBuffer[MAX_PASS_LEN];
	LTBOOL			m_bCaptain;
	
	//Whisper
	char			m_szLastRecvNick[MAX_NICK_LEN];	//마지막으로 받은
	char			m_szLastSendNick[MAX_NICK_LEN];	//마지막으로 보낸

	// 광식 [05.03.18] : Model을 Team에 따라 구분하기.
	uint8			m_nPlayerModelId[2];
	
	LTBOOL				m_bMandateHost;
	LTBOOL				m_bRejoinHost;
	T_BACKUP_LOCAL_INFO	m_BackupLocalInfo;

	bool			m_bMissionInfo;
	LTVector		m_vTimeBombPos;
	LTRotation		m_rTimeBombRot;
	float			m_fTimeBombTime;

	char			m_szWorldName[MAX_CS_FILENAME_LEN];

	LTBOOL			m_bLowViolence;

	T_CREATEROOM_PARAM	m_CreateRoomParam;
	T_ROOM_OPTION		m_RoomOption;

	char				m_szClientHWInfo[MAX_HW_INFO_LEN];	
	int					m_nHWInfoPos;
	
	char				m_szRankingInfo[257];
	LTBOOL				m_bSetRanking;
	
	uint8				m_nEquipList[MAX_EQUIP_ITEM];
	LTBOOL				m_bWaitEquipList;
	
	uint8				m_nFireCryptKey;

#ifdef BIG_FAT_MODE_TEST
	LTBOOL				m_bBigHeadMode;
#endif
};

#define g_GameDoc				CGameDocument::GetSingleton()

#endif	//#ifndef __MURSUM_GAME_DOCUMENT_H__
