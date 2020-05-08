// ----------------------------------------------------------------------- //
//
// MODULE  : MissionButeMgr.h
//
// PURPOSE : MissionButeMgr definition - Controls attributes of all Missions
//
// CREATED : 07/26/99
//
// ----------------------------------------------------------------------- //

#ifndef __MISSION_BUTE_MGR_H__
#define __MISSION_BUTE_MGR_H__

#include "GameButeMgr.h"
#include "ltbasetypes.h"
#include "TemplateList.h"
#include "butefiledefines.h"


#define MMGR_INVALID_ID			255	// Invalid id
#define MISSION_DEFAULT_ID		0	// First Level in Mission attribute file

#define MMGR_MAX_FILE_PATH			64
#define MMGR_MAX_LEVEL_FILE_PATH	64
#define MMGR_MAX_REWARD_NAME		64
#define MMGR_MAX_MISSION_LEVELS		10
#define MMGR_MAX_WEAPONS			30
#define MMGR_MAX_AMMO				10

#define MMGR_MISSION_TAG				"Mission"
#define MMGR_GENERAL_TAG				"General"


struct MISSION
{
	MISSION();
    virtual LTBOOL   Init(CButeMgr & buteMgr, char* aTagName);
    virtual LTBOOL   Save(CButeMgr & buteMgr, char* aTagName);

	int			nId;
	int			nNameId;
	std::string	sName;

	int			nObjectiveID_A;
	int			nObjectiveID_B;
	std::string	sObjective;	
	char		szObjectiveSndFileName[MMGR_MAX_FILE_PATH];

	char		szLayout[MMGR_MAX_FILE_PATH];

	char		szMap[MMGR_MAX_LEVEL_FILE_PATH];
#ifdef _CLIENTBUILD
	HTEXTURE	hScreenShot;
	HTEXTURE	hMiniMap;
#endif

	GameType	eGameType;
	int			nTimeLimit;
	int			nKillLimit;
	int			nRoundLimit;

	int			nShortGameTypeID;
	int			nGameTypeID;
	
	int			nNumDefaultWeapons;
	int			aDefaultWeapons[MMGR_MAX_WEAPONS];
	int			nNumDefaultAmmo;
	int			aDefaultAmmo[MMGR_MAX_AMMO];
	
	int			nWalkSpeed;
	int			nRunSpeed;
	int			nJumpSpeed;
	int			nSuperJumpSpeed;
	int			nSwimSpeed;
	int			nLadderSpeed;
	int			nGravity;

protected :
	int		BuildWeaponsList(char *pszStr, int* pArray, int nArrayLen);
	int		BuildGadgetsList(char *pszStr, int* pArray, int nArrayLen);
	int		BuildAmmoList(char *pszStr, int* pArray, int nArrayLen);
};



class CMissionButeMgr : public CGameButeMgr
{
	public :

		CMissionButeMgr();
		virtual ~CMissionButeMgr();

        virtual LTBOOL	Init(const char* szAttributeFile=MISSION_DEFAULT_FILE) = 0;
		virtual void	Term();

        void            Reload() { Term(); Init(); }

		int				GetNumMissions() const { return m_MissionList.size(); }
		MISSION*		GetMission(int nMissionId);

        bool			IsMissionLevel( char const* pWorldFile, int & nMissionId, int & nLevel);


	protected :
		virtual	MISSION*	CreateMission( ) { return debug_new(MISSION); }
		virtual void		DestroyMission( MISSION* pMission ) { debug_delete( pMission ); }

		typedef std::vector< MISSION* > MissionList;
		MissionList		m_MissionList;

		char			m_aTagName[30];
};

extern CMissionButeMgr *g_pMissionButeMgr;

#endif // __MISSION_BUTE_MGR_H__