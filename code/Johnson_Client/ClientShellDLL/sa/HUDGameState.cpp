#include "stdafx.h"
#include "HUDMgr.h"
#include "HUDGameState.h"
#include "SoundFilterMgr.h"

#define MAX_GAME_STATE_TEXT_LEN	128
#define MAX_GAME_STATE_TEXT		8
static char	g_szGameStateArray[][32] =
{
	"FrameRate ",
	"P2P Host ",
	"Shadow(F1) ",
	"LightModels(F2) ",
	"Model LOD(F3) ",
	"Occluders(F4) ",
	"Sound(F5) ",
	"BGM(F6) ",
};

CHUDGameState::CHUDGameState()
{
	m_eLevel		= kHUDRenderNone;
	m_UpdateFlags	= kHUDAll;
	m_bVisible		= false;

	m_pNumber		= LTNULL;
	m_pText			= LTNULL;
	m_pDot			= LTNULL;
	m_pMinus		= LTNULL;
}
	

LTBOOL CHUDGameState::Init()
{
	int i, nPosY;
	char szTemp[256];

	m_pNumber = new CSAD3DText[10];
	for( i=0 ; i<10 ; ++i )
	{
		sprintf( szTemp, "%d", i );
		m_pNumber[i].CreateD3DText( 16, 16, true );
		m_pNumber[i].SetTransparent( true );
		m_pNumber[i].SetTextColor( RGB(246,193,98), RGB(0,0,0) );
		m_pNumber[i].DrawD3DText( szTemp, strlen(szTemp), DT_LEFT, D3D_TEXT_OUTLINE, false );
	}

	m_nBasePosX = 5;
	m_nBasePosY = 5;	
	nPosY = m_nBasePosY;

	m_pText = new CSAD3DText[MAX_GAME_STATE_TEXT];
	for( i=0 ; i<MAX_GAME_STATE_TEXT ; ++i )
	{
		m_pText[i].CreateD3DText( MAX_GAME_STATE_TEXT_LEN, 16, true );
		m_pText[i].SetTransparent( true );
		m_pText[i].SetTextColor( RGB(246,193,98), RGB(0,0,0) );
		m_pText[i].SetPosition( m_nBasePosX, nPosY );
		m_pText[i].DrawD3DText( g_szGameStateArray[i], strlen(g_szGameStateArray[i]), DT_RIGHT, D3D_TEXT_OUTLINE, false );

		nPosY += 20;
	}


	m_pDot = new CSAD3DText();
	m_pDot->CreateD3DText( 16, 16, true );
	m_pDot->SetTransparent( true );
	m_pDot->SetTextColor( RGB(246,193,98), RGB(0,0,0) );
	m_pDot->DrawD3DText( ".", strlen("."), DT_LEFT, D3D_TEXT_OUTLINE, false );

	m_pMinus = new CSAD3DText();
	m_pMinus->CreateD3DText( 16, 16, true );
	m_pMinus->SetTransparent( true );
	m_pMinus->SetTextColor( RGB(246,193,98), RGB(0,0,0) );
	m_pMinus->DrawD3DText( "-", strlen("-"), DT_LEFT, D3D_TEXT_OUTLINE, false );

	m_fLastFrameUpdate = g_pLTClient->GetTime();
	m_nAccumFrame = 0;
	m_nFrameRate = 0;

	this->ScreenDimChanged();
	return LTTRUE;
}

void CHUDGameState::Term()
{
	SAFE_DELETE_ARRAY( m_pNumber );	
	SAFE_DELETE_ARRAY( m_pText );
	SAFE_DELETE( m_pDot );
	SAFE_DELETE( m_pMinus );
}

void CHUDGameState::Update()
{
}

void CHUDGameState::Render()
{
	int i;

	if( g_pLTClient->GetTime()-m_fLastFrameUpdate > 1.0f )
	{
		m_fLastFrameUpdate = g_pLTClient->GetTime();
		m_nFrameRate = m_nAccumFrame;
		m_nAccumFrame = 0;
	}
	else
	{
		++m_nAccumFrame;
	}

	//[MURSUM] ShowFireInfo
	//DrawNumber( m_nSendCountPosX, m_nSendCountPosY, m_nSendCount );	
	
	if( !m_bVisible ) return;

	int nPosX = m_nBasePosX+MAX_GAME_STATE_TEXT_LEN;
	int nPosY = m_nBasePosY;
	float fVar;

	DrawNumber( nPosX, nPosY, m_nFrameRate );

	for( i=0 ; i<MAX_GAME_STATE_TEXT ; ++i )
	{
		m_pText[i].Render();
	}

	nPosY += 20;
	DrawNumber( nPosX, nPosY, (int)g_GameDoc.IsHost() );

	fVar = GetConsoleFloat( "ModelShadow_Proj_Enable", 1.0f );
	nPosY += 20;
	DrawNumber( nPosX, nPosY, (int)fVar );

	fVar = GetConsoleFloat( "LightModels", 1.0f );
	nPosY += 20;
	DrawNumber( nPosX, nPosY, (int)fVar );

	fVar = GetConsoleFloat( "ModelLODOffset", 0.0f );
	nPosY += 20;
	DrawNumber( nPosX, nPosY, (fVar==0.0f) ? 1 : 0 );

	fVar = GetConsoleFloat( "DebugRBNoOccluders", 1.0f );
	nPosY += 20;
	DrawNumber( nPosX, nPosY, (fVar>0.0f) ? 0 : 1 );

	fVar = GetConsoleFloat( "soundenable", 1.0f );
	nPosY += 20;
	DrawNumber( nPosX, nPosY, (int)fVar );

	fVar = GetConsoleFloat( "musicenable", 1.0f );
	nPosY += 20;
	DrawNumber( nPosX, nPosY, (int)fVar );
}

void CHUDGameState::Show( bool bVisible )
{
	m_bVisible = bVisible;
}

void CHUDGameState::ScreenDimChanged()
{
	//[MURSUM] ShowFireInfo
	m_nSendCountPosX = g_pInterfaceResMgr->GetScreenWidth()*0.5f;
	m_nSendCountPosY = g_pInterfaceResMgr->GetScreenHeight()*0.5 - 20.0f;
}

void CHUDGameState::DrawNumber( int nPosX, int nPosY, int nNumber )
{
	itoa( nNumber, m_szNumber, 10 );

	int nLen = strlen(m_szNumber);
	int n;

	for( int i=0 ; i<nLen ; ++i )
	{
		if( m_szNumber[i] == '-' )
		{
			m_pMinus->SetPosition( nPosX, nPosY );
			m_pMinus->Render();
			nPosX += 8;
		}
		else
		{
			n = m_szNumber[i]-48;
			m_pNumber[n].SetPosition( nPosX, nPosY );
			m_pNumber[n].Render();
			nPosX += 8;
		}
	}
}

void CHUDGameState::DrawNumber( int nPosX, int nPosY, float fNumber )
{
	sprintf( m_szNumber, "%f", fNumber );
	
	int nLen = strlen(m_szNumber);
	int n;

	for( int i=0 ; i<nLen ; ++i )
	{
		if( m_szNumber[i] == '-' )
		{
			m_pMinus->SetPosition( nPosX, nPosY );
			m_pMinus->Render();
			nPosX += 8;
		}
		else if( m_szNumber[i] == '.' )
		{
			m_pDot->SetPosition( nPosX, nPosY );
			m_pDot->Render();
			nPosX += 4;
		}
		else
		{
			n = m_szNumber[i]-48;
			m_pNumber[n].SetPosition( nPosX, nPosY );
			m_pNumber[n].Render();
			nPosX += 8;
		}
	}
}

void CHUDGameState::Set( int nKey )
{
	
	float fVar;
	switch( nKey )
	{
	case VK_F1:
		fVar = GetConsoleFloat( "ModelShadow_Proj_Enable", 1.0f );
		if( fVar > 0.0f )
		{
			WriteConsoleFloat( "ModelShadow_Proj_Enable", 0.0f );
		}
		else
		{
			WriteConsoleFloat( "ModelShadow_Proj_Enable", 1.0f );
		}
		break;

	case VK_F2:
		fVar = GetConsoleFloat( "LightModels", 1.0f );
		if( fVar > 0.0f )
		{
			WriteConsoleFloat( "LightModels", 0.0f );
		}
		else
		{
			WriteConsoleFloat( "LightModels", 1.0f );
		}
		break;

	case VK_F3:
		fVar = GetConsoleFloat( "ModelLODOffset", 0.0f );
		if( fVar == 0.0f )
		{
			WriteConsoleFloat( "ModelLODOffset", -4.0f );
		}
		else
		{
			WriteConsoleFloat( "ModelLODOffset", 0.0f );
		}
		break;

	case VK_F4:
		fVar = GetConsoleFloat( "DebugRBNoOccluders", 1.0f );
		if( fVar > 0.0f )
		{
			WriteConsoleFloat( "DebugRBNoOccluders", 0.0f );
		}
		else
		{
			WriteConsoleFloat( "DebugRBNoOccluders", 1.0f );
		}
		break;

	case VK_F5:
		fVar = GetConsoleFloat( "soundenable", 1.0f );
		if( fVar > 0.0f )
		{
			WriteConsoleFloat( "soundenable", 0.0f );
		}
		else
		{
			WriteConsoleFloat( "soundenable", 1.0f );
		}
		break;

	case VK_F6:
		fVar = GetConsoleFloat( "musicenable", 1.0f );
		if( fVar > 0.0f )
		{
			WriteConsoleFloat( "musicenable", 0.0f );
		}
		else
		{
			WriteConsoleFloat( "musicenable", 1.0f );
		}
		break;
		
	case VK_F7:
		{
			bool bFilterOK = true;

			ILTClientSoundMgr *pSoundMgr = (ILTClientSoundMgr *)g_pLTClient->SoundMgr();

			SOUNDFILTER* pFilter = g_pSoundFilterMgr->GetFilter( 5 );

			if ( !g_pSoundFilterMgr->IsUnFiltered( pFilter ) )
			{
				if ( pSoundMgr->SetSoundFilter( pFilter->szFilterName ) == LT_OK )
				{
					for (int i=0; i < pFilter->nNumVars; i++)
					{
						if ( pSoundMgr->SetSoundFilterParam(pFilter->szVars[i], pFilter->fValues[i]) != LT_OK )
							bFilterOK = false;
					}
				}
				else
				{
					bFilterOK = false;
				}
			}
			else
			{
				bFilterOK = false;
			}

			pSoundMgr->EnableSoundFilter( bFilterOK );
		}
		break;
	}
}

//[MURSUM] ShowFireInfo
void CHUDGameState::InitFireSendCount()
{
	m_nSendCount = 0;
}

//[MURSUM] ShowFireInfo
void CHUDGameState::AddFireSendCount()
{
	++m_nSendCount;
}