// ----------------------------------------------------------------------- //
//
// MODULE  : TeamDeathMatchMissionMgr.h
//
// PURPOSE : Definition of class to handle team deathmatch missions
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __TEAMDEATHMATCHMISSIONMGR_H__
#define __TEAMDEATHMATCHMISSIONMGR_H__

//
// Includes...
//

	#include "DeathMatchMissionMgr.h"


class CTeamDeathMatchMissionMgr : public CDeathMatchMissionMgr
{
	public:

		CTeamDeathMatchMissionMgr();
		~CTeamDeathMatchMissionMgr();

	// Game type overrides...
	public:

		// Initializes the object.  Overrides should call up.
		virtual bool	Init( );

		// Called every frame.  Overrides should call up.
		virtual void	Update();
		
	// Game type overrides...
	protected:

		// Init game.
		virtual bool	StartGame();

		// Does loading of next level.
		virtual bool	FinishExitLevel( );

		// Called when the clients enter the world
		virtual void	LevelStarted();

		//handle updating multiplayer options while in game
		virtual bool	HandleMultiplayerOptions( HCLIENT hSender, ILTMessage_Read& msg );

		// Added By Bestline ==>
		// Time : 2004-06-11 오후 4:41:10
		// 멀티 플레이의 게임 종료 구현
		virtual bool	EndGame( uint8 nWhy );
		// Added By Bestline <==
};

#endif // __TEAMDEATHMATCHMISSIONMGR_H__
