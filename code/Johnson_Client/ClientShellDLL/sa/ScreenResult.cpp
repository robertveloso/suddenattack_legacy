#include "stdafx.h"
#include "ScreenResult.h"
#include "ScreenMgr.h"
#include "CommonUtilities.h"
#include "clientmultiplayermgr.h"
#include "WinUtil.h"
#include "GameClientShell.h"

//------------------------------------------- (B) Novice (minjin) [03/12/2005]
#include "GameText.h"
#include "GameMessage.h"
//------------------------------------------- (E) Novice (minjin) [03/12/2005]

CScreenResult* g_pScreenResult = LTNULL;

#define RESULT_TEXTURE_SIZE			32.0f

CScreenResult::CScreenResult()
{
	g_pScreenResult		= this;
	m_pBtnOk			= LTNULL;
	m_pWinRoundRed		= LTNULL;
	m_pWinRoundBlue		= LTNULL;
	m_nWinTeam			= -1;
	m_fLastShownTime	= 0.0f;
	m_bFirstUpdate		= LTFALSE;
	m_bInitted			= LTFALSE;
}

CScreenResult::~CScreenResult()
{
}

LTBOOL CScreenResult::Build()
{
	LTFLOAT	fBackupXRatio = g_pInterfaceResMgr->GetXRatio();
	LTFLOAT	fBackupYRatio = g_pInterfaceResMgr->GetYRatio();
	g_pInterfaceResMgr->SetXRatio(1.0f);
	g_pInterfaceResMgr->SetYRatio(1.0f);

	CreateBackWindow(SCREEN_ID_RESULT);
	
	m_pBtnOk	= AddButton(CMD_RESULT_OK, "BtnOkRect", "BtnOk", "BtnOkH", "BtnOkD", "BtnOkC", LTTRUE );	
		
/*	g_pDrawPrim->SetXYWH( &m_PolyWinTeam[ETN_RED], 127.0f, 17.0f, 128.0f, 32.0f );
	g_pDrawPrim->SetUVWH( &m_PolyWinTeam[ETN_RED], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyWinTeam[ETN_RED], 0xFFFFFFFF );
	g_pDrawPrim->SetXYWH( &m_PolyWinTeam[ETN_BLUE], 127.0f, 292.0f, 128.0f, 32.0f );
	g_pDrawPrim->SetUVWH( &m_PolyWinTeam[ETN_BLUE], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyWinTeam[ETN_BLUE], 0xFFFFFFFF );
*/
	
	g_pDrawPrim->SetXYWH( &m_PolyWinTeamFrame[ETN_RED][0], 34.0f, 63.0f, 512.0f, 256.0f );
	g_pDrawPrim->SetUVWH( &m_PolyWinTeamFrame[ETN_RED][0], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyWinTeamFrame[ETN_RED][0], 0xFFFFFFFF );
	g_pDrawPrim->SetXYWH( &m_PolyWinTeamFrame[ETN_RED][1], 512.0f, 63.0f, 256.0f, 256.0f );
	g_pDrawPrim->SetUVWH( &m_PolyWinTeamFrame[ETN_RED][1], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyWinTeamFrame[ETN_RED][1], 0xFFFFFFFF );
	g_pDrawPrim->SetXYWH( &m_PolyWinTeamFrame[ETN_BLUE][0], 34.0f, 338.0f, 512.0f, 256.0f );
	g_pDrawPrim->SetUVWH( &m_PolyWinTeamFrame[ETN_BLUE][0], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyWinTeamFrame[ETN_BLUE][0], 0xFFFFFFFF );
	g_pDrawPrim->SetXYWH( &m_PolyWinTeamFrame[ETN_BLUE][1], 512.0f, 338.0f, 256.0f, 256.0f );
	g_pDrawPrim->SetUVWH( &m_PolyWinTeamFrame[ETN_BLUE][1], 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_PolyWinTeamFrame[ETN_BLUE][1], 0xFFFFFFFF );
	

	m_hClanMarkTx = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\Textures\\Result\\SpaceTx.dtx" );
	m_hMyInfoBar = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\Textures\\Result\\MyInfoBar.dtx" );
/*	m_hTxWinTeam[ETN_RED] = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\Textures\\Result\\WinStamp.dtx" );
	m_hTxWinTeam[ETN_BLUE] = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\Textures\\Result\\WinStamp.dtx" );
	m_hTxWinTeam[ETN_DRAW] = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\Textures\\Result\\DrawGame.dtx" );*/
	m_hTxWinTeamFrame[0] = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\Textures\\Result\\WinTeamFrame0.dtx" );
	m_hTxWinTeamFrame[1] = g_pInterfaceResMgr->GetTexture( "sa_interface\\menu\\Textures\\Result\\WinTeamFrame1.dtx" );		
	m_ptTeamFrameOffset[ETN_RED].x = 35;
	m_ptTeamFrameOffset[ETN_RED].y = 71;
	m_ptTeamFrameOffset[ETN_BLUE].x = 35;
	m_ptTeamFrameOffset[ETN_BLUE].y = 346;	
	m_nInfoBarOffset	= 7;
	m_nClanMarkOffset	= 9;
	m_nGradeOffset		= 48;
	m_nNameOffset		= 95;
	m_nKillOffset		= 276;
	m_nDeathOffset		= 312;
	m_nMissionSuccessCountOffset = 360;
	m_nPointOffset		= 439;
	m_nMoneyOffset		= 518;
	m_nETCOffset		= 587;
	m_ptWinCountOffset.x = 578;
	m_ptWinCountOffset.y = -48;	
	m_nPlayerScoreLineHeight = 27;
	
	Result_CreateD3DText();
	
	LTBOOL bResult = CScreenBase::Build();

	g_pInterfaceResMgr->SetXRatio(fBackupXRatio);
	g_pInterfaceResMgr->SetYRatio(fBackupYRatio);

	return bResult;
}

void CScreenResult::Term()
{
	SAFE_DELETE( m_pWinRoundRed  );
	SAFE_DELETE( m_pWinRoundBlue );

	for( int i = 0; i < MAX_PLAYERS_COUNT; i++ )
	{
		SAFE_DELETE( m_ResultPaint[i].pTextNick );
		SAFE_DELETE( m_ResultPaint[i].pTextKill );
		SAFE_DELETE( m_ResultPaint[i].pTextDeath );
		SAFE_DELETE( m_ResultPaint[i].pTextMissionSuccessCount );
		SAFE_DELETE( m_ResultPaint[i].pTextPoint );
		SAFE_DELETE( m_ResultPaint[i].pTextMoney );
	}
	
	CScreenBase::Term();
}

void CScreenResult::SetResult_User( const char* szNick, WORD nTeam, UINT nGrade,
									WORD nKill, WORD nDead,
									WORD nMissionSuccessCount,
									UINT nExp, UINT nMoney, int nMiscFlags )
{
#ifdef GAME_SHOW
	T_PLAYER_INFOptr pPI = g_pInterfaceMgr->GetPlayerInfoMgr()->GetPlayerByNick( szNick );
	if( pPI && pPI->bCaptain )
	{
		--m_nResultCount;
		if( m_nResultCount <= 0 )
		{
			Result_Sort();
			g_Network.SetRecvResult( LTTRUE );
		}
		return;
	}
#endif

	char szTemp[64] = {0, };
	m_ResultData[++m_nResultIndex].nGrade				= nGrade;
	m_ResultData[m_nResultIndex].nMoney					= nMoney;
	m_ResultData[m_nResultIndex].nTeam					= nTeam;
	m_ResultData[m_nResultIndex].nExp					= nExp;
	m_ResultData[m_nResultIndex].nDead					= nDead;
	m_ResultData[m_nResultIndex].nKill					= nKill;
	m_ResultData[m_nResultIndex].nMissionSuccessCount	= nMissionSuccessCount;
	m_ResultData[m_nResultIndex].nMiscFlags				= nMiscFlags;	//OR연산으로 필요한 아이콘을 가져옴.
	SAFE_STRCPY( m_ResultData[m_nResultIndex].szNick, szNick );

	if( strcmp( szNick, g_GameDoc.GetNick() ) == 0 )
	{
		g_GameDoc.SetExp( g_GameDoc.GetExp() + nExp );
		g_GameDoc.SetGrade( nGrade );
		g_GameDoc.SetMoney( g_GameDoc.GetMoney() + nMoney );
		g_GameDoc.SetKill( g_GameDoc.GetKill() + nKill );
		g_GameDoc.SetDeath( g_GameDoc.GetDeath() + nDead );
	}

	--m_nResultCount;
	if( m_nResultCount <= 0 )
	{
		Result_Sort();
		g_Network.SetRecvResult( LTTRUE );
	}
}

void CScreenResult::Result_Sort()
{
	int			i, j, nMax;
	sResultData	MaxData;

	for( i = 0 ; i < MAX_PLAYERS_COUNT - 1 ; ++i )
	{
		nMax = i;
		memcpy( &MaxData, &m_ResultData[i], sizeof(sResultData) );

		for ( j = i + 1 ; j < MAX_PLAYERS_COUNT ; ++j )
		{
			if ( MaxData.nTeam == m_ResultData[j].nTeam &&
				 MaxData.nExp <= m_ResultData[j].nExp )
			//if( MaxData.nKill <= m_ResultData[j].nKill )
			{
				nMax	= j;
				memcpy( &MaxData, &m_ResultData[j], sizeof(sResultData) );
			}
		}

		memcpy( &m_ResultData[nMax], &m_ResultData[i], sizeof(sResultData) );
		memcpy( &m_ResultData[i], &MaxData, sizeof(sResultData) );
	}
}

void CScreenResult::SetResult_Team( WORD nWinTeam, WORD nResultCount )
{	
	if( !m_bInitted ) 
	{
		ASSERT(FALSE);
		g_pLTClient->CPrint( "[ERROR] Recv Result Before Init" );
		Result_Init();
		SetRoundScore( 0, 0 );
	}

	m_nWinTeam		= nWinTeam;
	m_nResultCount	= nResultCount;

	if( nWinTeam == ::LogicObject::Player::Type::E_TEAM::ENUM::DRAW )
	{
		g_GameDoc.SetDrawCount( g_GameDoc.GetDrawCount() + 1 );
	}
	else if( nWinTeam == g_GameDoc.GetTeam() )
	{
		g_GameDoc.SetWinCount( g_GameDoc.GetWinCount() + 1 );
	}
	else
	{
		g_GameDoc.SetLoseCount( g_GameDoc.GetLoseCount() + 1 );
	}
}

void CScreenResult::SetRoundScore( WORD nWinRound_A, WORD nWinRound_B )
{
	m_nTotalData_Red  = nWinRound_A;
	m_nTotalData_Blue = nWinRound_B;
}

void CScreenResult::Result_Position()
{
	char szTemp[64] = {0, };

	int nPosX_Red	= m_ptTeamFrameOffset[ETN_RED].x;
	int nPosY_Red	= m_ptTeamFrameOffset[ETN_RED].y;
	int nPosX_Blue	= m_ptTeamFrameOffset[ETN_BLUE].x;
	int nPosY_Blue	= m_ptTeamFrameOffset[ETN_BLUE].y;
	
	COLORREF rgbWinTeam		= RGB( 250, 204, 133 );
	COLORREF rgbLoseTeam	= RGB( 152, 134, 117 );
	
	COLORREF rgbRedList		= ( ETN_RED  == m_nWinTeam ) ? rgbWinTeam : rgbLoseTeam;
	COLORREF rgbBlueList	= ( ETN_BLUE == m_nWinTeam ) ? rgbWinTeam : rgbLoseTeam;
	
	char szTotal[256] = {0, };
	if(g_pGameClientShell->GetGameType() == eGameTypeSurvival)  
	{
		wsprintf( szTotal, "%s: %dWin", NF_GetMsg(SA_TEXT_SCORE_TOTAL), m_nTotalData_Red );
		m_pWinRoundRed->DrawD3DText(  szTotal, strlen(szTotal), DT_CENTER  );
		wsprintf( szTotal, "%s: %dWin", NF_GetMsg(SA_TEXT_SCORE_TOTAL), m_nTotalData_Blue );
		m_pWinRoundBlue->DrawD3DText( szTotal, strlen(szTotal), DT_CENTER  );
	}
	else if(g_pGameClientShell->GetGameType() == eGameTypeDeathmatch)
	{
		wsprintf( szTotal, "%s: %dKill", NF_GetMsg(SA_TEXT_SCORE_TOTAL), m_nTotalData_Red );
		m_pWinRoundRed->DrawD3DText(  szTotal, strlen(szTotal), DT_CENTER  );
		wsprintf( szTotal, "%s: %dKill", NF_GetMsg(SA_TEXT_SCORE_TOTAL), m_nTotalData_Blue );
		m_pWinRoundBlue->DrawD3DText( szTotal, strlen(szTotal), DT_CENTER  );
	}
	
	for ( int i = 0; i < MAX_PLAYERS_COUNT; i++ )
	{
		if ( m_ResultData[i].nExp == 0xFFFFFFFF )
			break;

		int* nPosX = NULL;
		int* nPosY = NULL;
		COLORREF rgbList;

		switch ( m_ResultData[i].nTeam )
		{
		case ETN_RED:	nPosX = &nPosX_Red;		nPosY = &nPosY_Red;	  rgbList = rgbRedList;	break;
		case ETN_BLUE:	nPosX = &nPosX_Blue;	nPosY = &nPosY_Blue;  rgbList = rgbBlueList;	break;
		default:
			break;
		}

		m_ResultPaint[i].pTextNick->DrawD3DText(m_ResultData[i].szNick, strlen(m_ResultData[i].szNick), DT_LEFT, D3D_TEXT_NORMAL, true );
		m_ResultPaint[i].pTextNick->SetPosition( *nPosX + m_nNameOffset, *nPosY );
		m_ResultPaint[i].pTextNick->SetTextColor( rgbList );
		
		sprintf(szTemp, "%d /", m_ResultData[i].nKill);
		m_ResultPaint[i].pTextKill->DrawD3DText(szTemp, strlen(szTemp), DT_RIGHT );
		m_ResultPaint[i].pTextKill->SetPosition( *nPosX + m_nKillOffset, *nPosY );
		m_ResultPaint[i].pTextKill->SetTextColor( rgbList );

		sprintf( szTemp, "%d", m_ResultData[i].nDead );
		m_ResultPaint[i].pTextDeath->DrawD3DText( szTemp, strlen(szTemp), DT_LEFT );
		m_ResultPaint[i].pTextDeath->SetPosition( *nPosX + m_nDeathOffset, *nPosY );
		m_ResultPaint[i].pTextDeath->SetTextColor( rgbList );

		if( g_pGameClientShell->GetGameType() == eGameTypeTeamDeathmatch )
		{
			m_ResultPaint[i].pTextMissionSuccessCount->DrawD3DText( "", 0 );
		}
		else
		{
			sprintf( szTemp, "%d", m_ResultData[i].nMissionSuccessCount );
			m_ResultPaint[i].pTextMissionSuccessCount->DrawD3DText( szTemp, strlen(szTemp), DT_CENTER );
			m_ResultPaint[i].pTextMissionSuccessCount->SetPosition( *nPosX + m_nMissionSuccessCountOffset, *nPosY );
			m_ResultPaint[i].pTextMissionSuccessCount->SetTextColor( rgbList );			
		}
		
		sprintf( szTemp, "%d", m_ResultData[i].nExp );
		m_ResultPaint[i].pTextPoint->DrawD3DText( szTemp, strlen(szTemp), DT_CENTER );
		m_ResultPaint[i].pTextPoint->SetPosition( *nPosX + m_nPointOffset, *nPosY );
		m_ResultPaint[i].pTextPoint->SetTextColor( rgbList );
		
		sprintf( szTemp, "%d", m_ResultData[i].nMoney );
		m_ResultPaint[i].pTextMoney->DrawD3DText(szTemp, strlen(szTemp), DT_CENTER );
		m_ResultPaint[i].pTextMoney->SetPosition( *nPosX + m_nMoneyOffset, *nPosY );
		m_ResultPaint[i].pTextMoney->SetTextColor( rgbList );

		g_pDrawPrim->SetXYWH( &m_ResultPaint[i].PolyClanMark, (float)(*nPosX + m_nClanMarkOffset), (float)(*nPosY - 5), 30.0f, 30.0f );
		g_pDrawPrim->SetRGBA( &m_ResultPaint[i].PolyClanMark, 0xFFFFFFFF );
		SetupQuadUVs( m_ResultPaint[i].PolyClanMark, m_hClanMarkTx, 0.0f, 0.0f, 1.0f, 1.0f );
				
		m_ResultPaint[i].hGradeIcon = g_Global_Data.GetGradeTexture( m_ResultData[i].nGrade );
		g_pDrawPrim->SetXYWH( &m_ResultPaint[i].PolyGrade,
							  (float)(*nPosX + m_nGradeOffset),
							  (float)(*nPosY - 6.0f),
							  RESULT_TEXTURE_SIZE, RESULT_TEXTURE_SIZE );

		g_pDrawPrim->SetRGBA( &m_ResultPaint[i].PolyGrade, 0xFFFFFFFF );

		SetupQuadUVs( m_ResultPaint[i].PolyGrade,
					  m_ResultPaint[i].hGradeIcon,
					  0.0f, 0.0f, 1.0f, 1.0f );

		if ( 0 == strcmp( g_GameDoc.GetNick(), m_ResultData[i].szNick ) )
		{
			g_pDrawPrim->SetXYWH( &m_PolyMyInfoBar,
								  (float)(*nPosX),
								  (float)(*nPosY - m_nInfoBarOffset),
								  729.0f, 26.0f );

			g_pDrawPrim->SetRGBA( &m_PolyMyInfoBar, 0xFFFFFFFF );

			SetupQuadUVs( m_PolyMyInfoBar, m_hMyInfoBar,
						  0.0f, 0.0f, 1.0f, 1.0f );
		}

		*nPosY += m_nPlayerScoreLineHeight;
	}
}

void CScreenResult::Result_Render()
{
	int i = 0;
	if ( ETN_RED == m_nWinTeam || ETN_BLUE == m_nWinTeam )
	{
		for( i=0; i<2; i++)
		{
			g_pDrawPrim->SetTexture( m_hTxWinTeamFrame[i] );
			g_pDrawPrim->DrawPrim( &m_PolyWinTeamFrame[m_nWinTeam][i]);
		}
	}

	g_pDrawPrim->SetTexture( m_hMyInfoBar );
	g_pDrawPrim->DrawPrim( &m_PolyMyInfoBar );
	
	for ( i = 0; i < MAX_PLAYERS_COUNT; i++ )
	{
		if ( m_ResultData[i].nExp == 0xFFFFFFFF )
			break;

		m_ResultPaint[i].pTextNick->Render();
		m_ResultPaint[i].pTextKill->Render();
		m_ResultPaint[i].pTextDeath->Render();
		m_ResultPaint[i].pTextMissionSuccessCount->Render();
		m_ResultPaint[i].pTextPoint->Render();
		m_ResultPaint[i].pTextMoney->Render();

		if ( m_ResultPaint[i].hGradeIcon )
		{
			//g_pDrawPrim->SetTexture( m_hClanMarkTx );
			//g_pDrawPrim->DrawPrim( &m_ResultPaint[i].PolyClanMark );
			g_pDrawPrim->SetTexture( m_ResultPaint[i].hGradeIcon );
			g_pDrawPrim->DrawPrim( &m_ResultPaint[i].PolyGrade );
		}		
	}

	//[SPIKE] 승리횟수
	m_pWinRoundRed->Render();		
	m_pWinRoundBlue->Render();		
}

void CScreenResult::Result_Clear()
{
	m_pWinRoundRed->DrawD3DText( "", 0 );
	m_pWinRoundBlue->DrawD3DText( "", 0 );
	
	for( int i = 0; i < MAX_PLAYERS_COUNT; i++ )
	{
		m_ResultPaint[i].pTextNick->DrawD3DText( "", 0 );
		m_ResultPaint[i].pTextKill->DrawD3DText( "", 0 );
		m_ResultPaint[i].pTextDeath->DrawD3DText( "", 0 );
		m_ResultPaint[i].pTextMissionSuccessCount->DrawD3DText( "", 0 );
		m_ResultPaint[i].pTextPoint->DrawD3DText( "", 0 );
		m_ResultPaint[i].pTextMoney->DrawD3DText( "", 0 );
	}
}


void CScreenResult::Result_Init()
{
	m_nResultIndex	= -1;

	m_nTotalData_Red	= 0;
	m_nTotalData_Blue	= 0;
	
	for ( int i = 0; i < MAX_PLAYERS_COUNT; i++ )
	{
		m_ResultData[i].nExp = 0xFFFFFFFF;
	}

	m_bInitted = LTTRUE;
}

void CScreenResult::Result_CreateD3DText()
{
	for( int i = 0; i < MAX_PLAYERS_COUNT; i++ )
	{
		m_ResultPaint[i].pTextNick = new CSAD3DText();
		m_ResultPaint[i].pTextNick->CreateD3DText(180, m_nPlayerScoreLineHeight, true );
		m_ResultPaint[i].pTextNick->SetTransparent(true);
		
		m_ResultPaint[i].pTextKill = new CSAD3DText();
		m_ResultPaint[i].pTextKill->CreateD3DText(30, m_nPlayerScoreLineHeight, true );
		m_ResultPaint[i].pTextKill->SetTransparent(true);		

		m_ResultPaint[i].pTextDeath = new CSAD3DText();
		m_ResultPaint[i].pTextDeath->CreateD3DText(30, m_nPlayerScoreLineHeight, true );
		m_ResultPaint[i].pTextDeath->SetTransparent(true);
		
		m_ResultPaint[i].pTextMissionSuccessCount = new CSAD3DText();
		m_ResultPaint[i].pTextMissionSuccessCount->CreateD3DText(30, m_nPlayerScoreLineHeight, true );
		m_ResultPaint[i].pTextMissionSuccessCount->SetTransparent(true);

		m_ResultPaint[i].pTextPoint = new CSAD3DText();
		m_ResultPaint[i].pTextPoint->CreateD3DText(40, m_nPlayerScoreLineHeight, true );
		m_ResultPaint[i].pTextPoint->SetTransparent(true);
		
		m_ResultPaint[i].pTextMoney = new CSAD3DText();
		m_ResultPaint[i].pTextMoney->CreateD3DText(60, m_nPlayerScoreLineHeight, true );
		m_ResultPaint[i].pTextMoney->SetTransparent(true);
	}	

	//[SPIKE]
	m_pWinRoundRed	= new CSAD3DText;	
	m_pWinRoundRed->CreateD3DText(140, m_nPlayerScoreLineHeight, true );
	m_pWinRoundRed->SetTextColor(RGB(250, 204, 133));
	m_pWinRoundRed->SetTransparent(true);

	m_pWinRoundBlue	= new CSAD3DText;	
	m_pWinRoundBlue->CreateD3DText(140, m_nPlayerScoreLineHeight, true );
	m_pWinRoundBlue->SetTextColor(RGB(250, 204, 133));
	m_pWinRoundBlue->SetTransparent(true);
	
	//[SPIKE] 승리횟수
	LTIntPt pt;
	pt.x = m_ptTeamFrameOffset[ETN_RED].x + m_ptWinCountOffset.x;
	pt.y = m_ptTeamFrameOffset[ETN_RED].y + m_ptWinCountOffset.y;
	m_pWinRoundRed->SetPosition( pt );

	pt.x = m_ptTeamFrameOffset[ETN_BLUE].x + m_ptWinCountOffset.x;
	pt.y = m_ptTeamFrameOffset[ETN_BLUE].y + m_ptWinCountOffset.y;
	m_pWinRoundBlue->SetPosition( pt );
}
//--------------------------------------------------------------------------------------------

void CScreenResult::Result_WinTeam()
{
	//[MURSUM]==========================================
	//에러를 막기 위해서..

	switch( m_nWinTeam )
	{
	case ETN_RED:
		m_nWinTxIndex = ETN_RED;
		break;
		
	case ETN_BLUE:
		m_nWinTxIndex = ETN_BLUE;
		break;
		
	case 255:		// Draw Game
		m_nWinTxIndex = ETN_DRAW;
		break;
		
	default:
		m_nWinTxIndex = -1;
		break;
	}
}

void CScreenResult::OnFocus(LTBOOL bFocus)
{
	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();

	if (bFocus)
	{
		g_Network.SendPing();
		
//		m_fLastShownTime = g_pLTClient->GetTime();
		m_bFirstUpdate	= LTFALSE;

		//[MURSUM]===================================================
		g_Network.EnableNetwork(LTTRUE);
		g_pInterfaceMgr->CloseMessageBox(LTFALSE);
		//===========================================================
		Result_Position();
		Result_WinTeam();

		g_pLTClient->CPrint( "Switch To ScreenResult Complete %d", ::GetTickCount() );
	}
	else
	{
		//[MURSUM]===================================================
		g_Network.CS_GetRoomInfo();
		g_Network.EnableNetwork(LTFALSE);
		//===========================================================
		Result_Clear();

		m_bInitted = LTFALSE;
	}

	CScreenBase::OnFocus(bFocus);
}

LTBOOL CScreenResult::Render(HSURFACE hDestSurf)
{
	SetRenderState();
	
	if ( LTFALSE == m_bFirstUpdate )
	{
		m_bFirstUpdate = LTTRUE;
		m_fLastShownTime = g_pLTClient->GetTime();
	}

	if ( ( g_pLTClient->GetTime() - m_fLastShownTime ) > RESULT_DELTA_TIME )
	{
		this->SendCommand(CMD_RESULT_OK, 0, 0);
		return LTFALSE;
	}
		
	LTBOOL bTemp = CScreenBase::Render(hDestSurf);

	Result_Render();	

/*	if( m_nWinTxIndex >= 0 )
	{
		g_pDrawPrim->SetTexture( m_hTxWinTeam[m_nWinTxIndex] );
		g_pDrawPrim->DrawPrim( &m_PolyWinTeam[m_nWinTxIndex] );
	}
*/
	return bTemp;
}

uint32 CScreenResult::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch( dwCommand )
	{
	case CMD_RESULT_OK :
		{
			ResultOKProc();
		}break;

	default :
		return CScreenBase::OnCommand(dwCommand,dwParam1,dwParam2);
	}

	return 1;
}

LTBOOL CScreenResult::OnEnter()
{
	return ResultOKProc();
}

LTBOOL CScreenResult::OnEscape()
{
	return ResultOKProc();
}

LTBOOL CScreenResult::ResultOKProc()
{
	//g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_MAIN);
	//g_pInterfaceMgr->SetServerPatchNotice( "Test" );

	return g_pInterfaceMgr->SwitchToScreen(SCREEN_ID_SCREEN_ROOM);
}







































