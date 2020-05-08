#include "stdafx.h"
#include "SurvivalMissionMgr.h"
#include "PlayerObj.h"
#include "ProjectileTypes.h"
#include "ServerSaveLoadMgr.h"
#include "MissionButeMgr.h"

CSurvivalMissionMgr::CSurvivalMissionMgr()
{
	//[SPIKE] TimeBomb관련 
	m_bInitTimer		 = LTFALSE;
	m_bTimeBomb_Counting = LTFALSE;
	m_bEndRoundStats	 = LTFALSE;	
}

CSurvivalMissionMgr::~CSurvivalMissionMgr()
{
	Term( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSurvivalMissionMgr::Init
//
//	PURPOSE:	Init the mgr
//
// ----------------------------------------------------------------------- //
bool CSurvivalMissionMgr::Init()
{
	if( !CServerMissionMgr::Init( ))
		return false;

	/*
	// 팀 추가 하기
	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );

	uint8 nNumTeams = sgo.GetSurvival().m_nNumTeams;

 	for( uint8 i = 0; i < nNumTeams+1 ; ++i )	// For Spectator
	{
		if (i==0)	// Spectator <-- 나중에 이녀석 빼야지.. 짜증...
		{
			CTeamMgr::Instance().AddTeam("Spectator", i);
		}
		else
		{
			CTeamMgr::Instance().AddTeam( sgo.GetSurvival().m_sTeamName[i-1].c_str(), i);
		}
	}
	*/
	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );
	uint8 nNumTeams = sgo.GetSurvival().m_nNumTeams;
	for( uint8 i = 0; i < nNumTeams; i++ )
	{
		CTeamMgr::Instance().AddTeam( sgo.GetSurvival().m_sTeamName[i].c_str(), i);
	}

	m_bSendAllKilled = LTFALSE;

	return !!CTeamMgr::Instance().GetNumTeams();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSurvivalMissionMgr::Term
//
//	PURPOSE:	Term the mgr
//
// ----------------------------------------------------------------------- //
void CSurvivalMissionMgr::Term()
{
	CServerMissionMgr::Term( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSurvivalMissionMgr::Save()
//
//	PURPOSE:	Saves state of misionmgr.
//
// ----------------------------------------------------------------------- //

bool CSurvivalMissionMgr::Save( ILTMessage_Write& msg, uint32 dwSaveFlags )
{
	if( !CServerMissionMgr::Save( msg, dwSaveFlags ))
		return false;

	return true;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSurvivalMissionMgr::Load()
//
//	PURPOSE:	Loads state of misionmgr.
//
// ----------------------------------------------------------------------- //

bool CSurvivalMissionMgr::Load( ILTMessage_Read& msg, uint32 dwSaveFlags )
{
	if( !CServerMissionMgr::Load( msg, dwSaveFlags ))
		return false;

	return true;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSurvivalMissionMgr::Update
//
//	PURPOSE:	Frame update.
//
// --------------------------------------------------------------------------- //

void CSurvivalMissionMgr::Update( )
{
	/*
	//check for end of level...

	//check score limit
	if ( m_ServerSettings.m_nScoreLimit > 0 && m_fStartTime >= 0.0f )
	{
		CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
		while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
		{
			CPlayerObj* pPlayerObj = *iter;
			if (pPlayerObj->GetPlayerScore()->GetScore() >= m_ServerSettings.m_nScoreLimit)
			{
				g_pLTServer->CPrint("ScoreLimit reached.");
				NextRound();
			}
			iter++;
		}
	}
	*/
	CServerMissionMgr::Update( );
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSurvivalMissionMgr::FinishExitLevel
//
//	PURPOSE:	Finish the exitlevel we started.
//
// --------------------------------------------------------------------------- //

bool CSurvivalMissionMgr::FinishExitLevel( )
{
	TRACE( "CSurvivalMissionMgr::FinishExitLevel:\n" );

	// No longer waiting for exitlevel messages back from client.
	m_bExitingLevel = false;

	// Clear out the playertracker.
	m_PlayerTracker.Term( );

	// Tell the players to handle an exit level.
	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pPlayerObj = *iter;
		pPlayerObj->HandleExit( true );
		iter++;
	}
	CHECK_PLAYEROBJ_END();

	// Load new level with no restoring of keepalives or save games.
	if( !g_pServerSaveLoadMgr->LoadNewLevel( m_sCurrentWorldName ))
		return false;

	// Unpause the server now that we're done switching levels.
	g_pGameServerShell->PauseGame( LTFALSE );	


	return true;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSurvivalMissionMgr::StartGame
//
//	PURPOSE:	Start the game.
//
// --------------------------------------------------------------------------- //

bool CSurvivalMissionMgr::StartGame()
{
	g_pLTServer->CPrint( "CSurvivalMissionMgr::StartGame()" );

	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );
	ServerMissionSettings sms = GetServerSettings();

	sms.m_bUseSkills = false;
	sms.m_bFriendlyFire = sgo.GetSurvival().m_bFriendlyFire;
	sms.m_nScoreLimit = sgo.GetSurvival().m_nScoreLimit;
	sms.m_nTimeLimit = sgo.GetSurvival().m_nTimeLimit;
	sms.m_nRounds = sgo.GetSurvival().m_nRounds;
	sms.m_nFragScore = sgo.GetSurvival().m_nFragScore;
	sms.m_nTagScore = sgo.GetSurvival().m_nTagScore;

	SetServerSettings(sms);

	if( !g_pMissionButeMgr->Init( MISSION_DEFAULT_FILE ))
		return false;

	if( !CServerMissionMgr::StartGame())
		return false;

	return true;
}

void CSurvivalMissionMgr::LevelStarted()
{
	CServerMissionMgr::LevelStarted( );

	// Check if this is a new level, or just another round.
	if( m_bNewMission )
	{
		CTeamMgr::Instance( ).NewLevel( );
	}
	else
	{
		CTeamMgr::Instance( ).NewRound( );
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CSurvivalMissionMgr::EndGame
//
//	PURPOSE:	End the game.
//
// --------------------------------------------------------------------------- //

bool CSurvivalMissionMgr::EndGame( uint8 nWhy )
{
	if( !CServerMissionMgr::EndGame( nWhy ))	return false;
	
	CAutoMessage cMsg;
	cMsg.Writeuint8( MID_END_GAME );
	cMsg.Writeuint8( nWhy );
	g_pLTServer->SendToClient( cMsg.Read(), g_pGameServerShell->GetHostPlayerHandle(), MESSAGE_GUARANTEED );
	return true;
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CServerMissionMgr::HandleMultiplayerOptions
//
//	PURPOSE:	Handle updating the host options
//
// --------------------------------------------------------------------------- //

bool CSurvivalMissionMgr::HandleMultiplayerOptions( HCLIENT hSender, ILTMessage_Read& msg )
{
	ServerGameOptions& sgo = g_pGameServerShell->GetServerGameOptions( );
	sgo.GetSurvival().Read(&msg);

	ServerMissionSettings sms = GetServerSettings();

	sms.m_bUseSkills = false;
	sms.m_bFriendlyFire = sgo.GetSurvival().m_bFriendlyFire;
	sms.m_nScoreLimit = sgo.GetSurvival().m_nScoreLimit;
	sms.m_nTimeLimit = sgo.GetSurvival().m_nTimeLimit;
	sms.m_nRounds = sgo.GetSurvival().m_nRounds;
	sms.m_nFragScore = sgo.GetSurvival().m_nFragScore;
	sms.m_nTagScore = sgo.GetSurvival().m_nTagScore;

	SetServerSettings(sms);

	return true;

}

//[MURSUM]=============================================================
bool CSurvivalMissionMgr::HandleEndLevel( HCLIENT hSender, ILTMessage_Read& msg )
{
	/*
	if( LTFALSE == g_pGameServerShell->IsPaused() )
	{
		g_pGameServerShell->PauseGame(LTTRUE);
	}
	*/

	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pPlayerObj = *iter;
		pPlayerObj->DetachAllPlayer();
		pPlayerObj->RestartWeaponClass();
		iter++;
	}
	CHECK_PLAYEROBJ_END();

	g_pLTServer->CPrint( "CSurvivalMissionMgr::HandleEndLevel()" );

	//[SPIKE] Detonated, Defused로 게임이 끝나면 굳이 초기화 하지 않기 위해 
	if(IsTimeBomb_Counting())  // 현재 TimeBomb이 설치되있는 상태이면... 
	{
		SetTimeBomb_InitTimer(LTTRUE);
	}
	SetEndRoundStats(LTTRUE); // 게임이 끝난 상태 
	g_pWeaponMgr->SetPlantedTimeBomb( LTFALSE );
	//===========================================================================
	return true;
}

bool CSurvivalMissionMgr::HandleRestartLevel( HCLIENT hSender, ILTMessage_Read& msg )
{
	bool bRestartLevel	= msg.Readbool();

	g_pLTServer->CPrint( "CSurvivalMissionMgr::HandleRestartLevel( %d )", bRestartLevel );
	
	//[SPIKE]
	if(IsTimeBomb_Counting())  // 현재 TimeBomb이 설치되있는 상태이면... 
	{
		SetTimeBomb_InitTimer(LTTRUE);
	}
	SetEndRoundStats(LTFALSE);
	g_pWeaponMgr->SetPlantedTimeBomb( LTFALSE );
	m_bSendAllKilled = LTFALSE;
	//===========================================
	
	if( bRestartLevel )
	{
		CGrenade** pGrenade = g_lstGrenades.GetItem(TLIT_FIRST);
		while( pGrenade )
		{
			if( (*pGrenade) ) (*pGrenade)->RemoveGrenade();
			pGrenade = g_lstGrenades.GetItem(TLIT_NEXT);
		}
		g_lstGrenades.Clear();

		CFlashBang** pFlashBang = g_lstFlashBang.GetItem(TLIT_FIRST);
		while( pFlashBang )
		{
			if( (*pFlashBang) ) (*pFlashBang)->RemoveFlashBang();
			pFlashBang = g_lstFlashBang.GetItem(TLIT_NEXT);
		}		
		g_lstFlashBang.Clear();
		
		CSmokeGrenade** pSmokeGrenade = g_lstSmokeGrenade.GetItem(TLIT_FIRST);
		while( pSmokeGrenade )
		{
			if( (*pSmokeGrenade) ) (*pSmokeGrenade)->RemoveSmokeGrenade();
			pSmokeGrenade = g_lstSmokeGrenade.GetItem(TLIT_NEXT);
		}		
		g_lstSmokeGrenade.Clear();

		return RestartLevel();
	}
	else
	{
		return EndGame( EG_NORMAL );
	}	

	return false;
}

void CSurvivalMissionMgr::OnClientEnterWorld( HCLIENT hClient )
{
	if( IsTimeBomb_Counting() )
	{
		float	fTime = m_fTimeBombTime-(g_pLTServer->GetTime()-m_fTimeBombStart);
		if( fTime > 0.0f )
		{
			CAutoMessage cMsg;
			cMsg.Writeuint8(MID_MISSION_INFO);
			cMsg.Writeuint8(MI_BOMB_LIMIT_TIME);
			cMsg.Writeuint8( (uint8)fTime );
			cMsg.WriteCompLTVector( m_vTimeBombPos );
			g_pLTServer->SendToClient( cMsg.Read(), hClient, MESSAGE_GUARANTEED );
		}
	}
}

void CSurvivalMissionMgr::SetTimeBomb( LTBOOL bBombing, float fTime, LTVector* pvPos )
{
	g_pLTServer->CPrint( "CSurvivalMissionMgr::SetTimeBomb( %d, %f )", bBombing, fTime );
	m_bTimeBomb_Counting	= bBombing;
	if( m_bTimeBomb_Counting )
	{
		m_fTimeBombTime			= fTime;
		m_fTimeBombStart		= g_pLTServer->GetTime();
		m_vTimeBombPos.x		= pvPos->x;
		m_vTimeBombPos.y		= pvPos->y;
		m_vTimeBombPos.z		= pvPos->z;
	}
	else
	{
		m_fTimeBombTime			= 0.0f;
		m_fTimeBombStart		= 0.0f;
		m_vTimeBombPos.x		= 0.0f;
		m_vTimeBombPos.y		= 0.0f;
		m_vTimeBombPos.z		= 0.0f;
	}
}

bool CSurvivalMissionMgr::RestartLevel()
{
	if( false == CServerMissionMgr::RestartLevel() )
	{
		return EndGame( EG_NORMAL );
	}

	g_pLTServer->CPrint( "CSurvivalMissionMgr::RestartLevel()" );
	SendEmptyClientMsg( MID_RESTART_LEVEL, NULL, MESSAGE_GUARANTEED );

	return true;
}

bool CSurvivalMissionMgr::ForceEndGame( uint8 nWinTeamID, uint32 nPlayerID, HOBJECT hDamager )
{
	if( nWinTeamID == INVALID_TEAM ) return false;

	g_pLTServer->CPrint( "CSurvivalMissionMgr::ForceEndGame( %d )", nWinTeamID );
	
	//[SPIKE]===========================================
	LTVector vDamagerPos, vPos, vDir;
	if(hDamager)
	{
		g_pLTServer->GetObjectPos(hDamager, &vDamagerPos);			
	}

	CHECK_PLAYEROBJ_BEGIN();
	CPlayerObj::PlayerObjList::const_iterator iter = CPlayerObj::GetPlayerObjList( ).begin( );
	while( iter != CPlayerObj::GetPlayerObjList( ).end( ))
	{
		CPlayerObj* pPlayerObj = *iter;
		DamageStruct damage;

		if( nWinTeamID != pPlayerObj->GetTeamID() )
		{
			g_pLTServer->GetObjectPos(pPlayerObj->m_hObject, &vPos);
			vDir = vPos - vDamagerPos;
			if(vDir.Length() < 3000.0f) //죽음
			{					
				damage.eType		= DT_EXPLODE;
				damage.fDamage		= 2000.0f;
				damage.hDamager		= pPlayerObj->m_hObject;
				vDir.Normalize();
				damage.vDir			= vDir;
				damage.nWeaponId	= WMGR_INVALID_ID-10;	//칼로자살 방지.
				damage.hDamager		= hDamager;
				
				LPBASECLASS pClass = g_pLTServer->HandleToObject(hDamager);
				damage.DoDamage(pClass, pPlayerObj->m_hObject);
			}
			else //무조건 죽음
			{
				pPlayerObj->GetDestructible()->SetForceDeath(LTTRUE);
			}
		}
		else
		{
			if(hDamager)
			{
				g_pLTServer->GetObjectPos(pPlayerObj->m_hObject, &vPos);
				vDir = vPos - vDamagerPos;
				
				if(vDir.Length() < 3000.0f) //죽음
				{					
					damage.eType		= DT_EXPLODE;
					damage.fDamage		= (3000.0f - vDir.Length())/5;
					
					//물에서는 데미지 상쇄!!
					ContainerCode eContainerCode = ::GetContainerCode(vPos);
					if(IsLiquid(eContainerCode))
					{
						damage.fDamage /= 3.0f;
					}

					damage.hDamager		= pPlayerObj->m_hObject;
					vDir.Normalize();
					damage.vDir			= vDir;
					damage.nWeaponId	= WMGR_INVALID_ID-10;	//칼로자살 방지.
					damage.hDamager		= hDamager;

					LPBASECLASS pClass = g_pLTServer->HandleToObject(hDamager);
					damage.DoDamage(pClass, pPlayerObj->m_hObject);
				}
			}			
		}
		iter++;
	}
	CHECK_PLAYEROBJ_END();
	
	SetTimeBomb( LTFALSE, 0.0f, LTNULL ); // TimeBomb 초기화 해제 

	CAutoMessage cMsg;
	cMsg.Writeuint8(MID_MISSION_INFO);
	cMsg.Writeuint8((nWinTeamID == TEAM_A) ? MI_BOMB_DETONATED : MI_BOMB_DEFUSED);
	cMsg.Writeuint32(nPlayerID);
	g_pLTServer->SendToClient( cMsg.Read(), g_pGameServerShell->GetHostPlayerHandle(), MESSAGE_GUARANTEED );

	return true;
	//==================================================
}

void CSurvivalMissionMgr::SetRemainedTime( float fRemainedTime )
{
	if( fRemainedTime <= 0.0f ) 
	{
		m_fStartTime = -1.0f;
	}
	else
	{
		m_fStartTime = g_pLTServer->GetTime() - (m_ServerSettings.m_nTimeLimit-fRemainedTime);
	}
}
//=====================================================================