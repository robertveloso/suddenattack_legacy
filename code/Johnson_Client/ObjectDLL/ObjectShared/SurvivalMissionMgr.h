#ifndef __SURVIVALMISSIONMGR_H__
#define __SURVIVALMISSIONMGR_H__

#include "ServerMissionMgr.h"

class Bombable;

class CSurvivalMissionMgr : public CServerMissionMgr
{
	public:

		CSurvivalMissionMgr();
		~CSurvivalMissionMgr();

	public:

		// Initializes the object.  Overrides should call up.
		virtual bool	Init( );

		// Terminates the object.  Overrides should call up.
		virtual void	Term();

		// Save/load state of missionmgr.  Overrides should call up.
		virtual bool	Save( ILTMessage_Write& msg, uint32 dwSaveFlags );
		virtual bool	Load( ILTMessage_Read& msg, uint32 dwSaveFlags );

		// Called every frame.  Overrides should call up.
		virtual void	Update( );

		virtual bool	CanRespawn( CPlayerObj const& player ) const { return false; }
		
		//[MURSUM]=============================================================
		virtual bool	RestartLevel();
		virtual bool	ForceEndGame( uint8 nWinTeamID, uint32 nPlayerID, HOBJECT hDamager = LTNULL );
		virtual void	OnClientEnterWorld( HCLIENT hClient );
		virtual void	SetRemainedTime( float fRemainedTime );
		//=====================================================================
		
		
		//[SPIKE] ==================================
		virtual VOID	SetTimeBomb_InitTimer(LTBOOL bInitTimer){ m_bInitTimer = bInitTimer; }
		virtual LTBOOL	GetTimeBomb_InitTimer(){ return m_bInitTimer; }		
		VOID			SetTimeBomb( LTBOOL bBombing, float	fTime, LTVector* vPos );
		LTBOOL			IsTimeBomb_Counting(){ return m_bTimeBomb_Counting; }
		// 현재 게임End(유/무)
		virtual VOID	SetEndRoundStats(LTBOOL bEndGame){ m_bEndRoundStats = bEndGame; }
		virtual LTBOOL	IsEndRound(){ return m_bEndRoundStats; }
		//============================================
	// Game type overrides.
	protected:

		LTBOOL			m_bSendAllKilled;
		float			m_fTimeBombTime;
		float			m_fTimeBombStart;
		LTVector		m_vTimeBombPos;

		//[SPIKE]
		LTBOOL			m_bInitTimer;
		LTBOOL			m_bTimeBomb_Counting;
		LTBOOL			m_bEndRoundStats;

		// Does loading of next level.
		virtual bool	FinishExitLevel( );

		virtual void	LevelStarted();

		// Init game.
		virtual bool	StartGame();

		// End game.
		virtual bool	EndGame( uint8 nWhy );

		//handle updating multiplayer options while in game
		virtual bool	HandleMultiplayerOptions( HCLIENT hSender, ILTMessage_Read& msg );
		
		virtual bool	HandleEndLevel( HCLIENT hSender, ILTMessage_Read& msg );
		
		virtual bool	HandleRestartLevel( HCLIENT hSender, ILTMessage_Read& msg );
};

#endif // __SURVIVALMISSIONMGR_H__
