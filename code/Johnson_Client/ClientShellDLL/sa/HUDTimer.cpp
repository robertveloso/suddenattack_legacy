#include "stdafx.h"
#include "SAHUDMgr.h"
#include "GameClientShell.h"

uint16 g_nBeepSoundDelay = 0;
uint16 g_nBeepSoundCount = 0;
LTBOOL g_bBeepSound		 = LTTRUE;

CHUDTimer::CHUDTimer()
{
	m_fTime			= 0.0f;
	m_UpdateFlags	= kHUDFrame;	
	m_eLevel		= kHUDRenderGameOver;
	m_fBasePosX		= 0.0f;
	m_fBasePosY		= 130.0f;	
	m_bDraw		    = LTFALSE;
	m_bPause		= LTFALSE;

	m_hTimerSeprator	= LTNULL;
	m_bSeperatorBlink	= LTFALSE;
}

LTBOOL CHUDTimer::Init()
{
	int		i;
	char	szTex[256];

	m_hTimerBack	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/TIMER/TimerBackground.dtx" );
	SetupQuadUVs( m_hPolyTimerBack, m_hTimerBack, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_hPolyTimerBack, 0xFFFFFFFF );

	m_hTimerSeprator = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/TIMER/TimeSeperator.dtx" );
	SetupQuadUVs( m_hPolyTimerSeperator, m_hTimerSeprator, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_hPolyTimerSeperator, 0xFFFFFFFF );
	
	for( i = 0 ; i < 10 ; ++i )
	{
		sprintf( szTex, "SA_INTERFACE/HUD/TIMER/t-%d.DTX", i );
		m_hTimerNumber[i] = g_pInterfaceResMgr->GetTexture( szTex );
	}

	for( i = 0 ; i < TIMER_DIGIT ; ++i )
	{
		SetupQuadUVs( m_hPolyTimerNumber[i], m_hTimerNumber[0], 0.0f, 0.0f, 1.0f, 1.0f);
		g_pDrawPrim->SetRGBA( &m_hPolyTimerNumber[i], 0xFFFFFFFF );
	}

	this->ScreenDimChanged();
	return LTTRUE;
}

void CHUDTimer::SetTime( LTFLOAT fTime, bool bPause )
{
	 m_fTime = fTime+g_pLTClient->GetTime();
	 m_bPause = bPause;
}

float CHUDTimer::GetTime()
{
	float fTime = m_fTime - g_pLTClient->GetTime();
	return LTMAX( 0.0f, fTime );
}

void CHUDTimer::Term()
{
}

void CHUDTimer::Render()
{
	if ( !m_bDraw && m_fTime > 0.0f ) return;

	g_pDrawPrim->SetTexture( m_hTimerBack );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerBack );

	if ( m_bSeperatorBlink )
	{
		g_pDrawPrim->SetTexture( m_hTimerSeprator );
		g_pDrawPrim->DrawPrim( &m_hPolyTimerSeperator );
	}

#ifdef _DEBUG
	if( m_nMinute_10 < 0 || m_nMinute_10 > 9 ||
		m_nMinute_1 < 0 || m_nMinute_1 > 9 ||
		m_nSecond_10 < 0 || m_nSecond_10 > 9 ||
		m_nSecond_1 < 0 || m_nSecond_1 > 9 )
	{
		ASSERT( LTFALSE );
	}
#endif

	if( m_nMinute_10 < 0 || m_nMinute_10 > 9 ) m_nMinute_10 = 0;
	if( m_nMinute_1 < 0 || m_nMinute_1 > 9 ) m_nMinute_1 = 0;
	if( m_nSecond_10 < 0 || m_nSecond_10 > 9 ) m_nSecond_10 = 0;
	if( m_nSecond_1 < 0 || m_nSecond_1 > 9 ) m_nSecond_1 = 0;

	g_pDrawPrim->SetTexture( m_hTimerNumber[m_nMinute_10] );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerNumber[0] );

	g_pDrawPrim->SetTexture( m_hTimerNumber[m_nMinute_1] );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerNumber[1] );

	g_pDrawPrim->SetTexture( m_hTimerNumber[m_nSecond_10] );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerNumber[2] );

	g_pDrawPrim->SetTexture( m_hTimerNumber[m_nSecond_1] );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerNumber[3] );
}

void CHUDTimer::Update()
{
	//if ( !m_bDraw && m_fTime > 0.0f && m_bPause ) return;
	if( !m_bDraw || m_fTime <= 0.0f || m_bPause ) return;

	float fCountDownTime;
	fCountDownTime = m_fTime - g_pLTClient->GetTime();
	
	// Update/Draw the timer if there is anytime left...
	if( fCountDownTime <= 0.0f )
	{
		m_nMinute_10	= 0;
		m_nMinute_1		= 0;
		m_nSecond_10	= 0;
		m_nSecond_1		= 0;
	}
	else
	{
		// Draw the string to the surface...
		m_nMinute_10	= int(fCountDownTime) / 600;
		fCountDownTime	-= 600.0f*m_nMinute_10;

		m_nMinute_1		= int(fCountDownTime) / 60;
		fCountDownTime	-= 60.0f*m_nMinute_1;

		m_nSecond_10	= int(fCountDownTime) / 10;
		fCountDownTime	-= 10.0f*m_nSecond_10;

		m_nSecond_1		= int(fCountDownTime);
		fCountDownTime	-= float(m_nSecond_1);

		fCountDownTime	*= 100.0f;

		m_bSeperatorBlink = ( int(fCountDownTime) / 10 >= 5 ) ? LTTRUE : LTFALSE;
	}
}

void CHUDTimer::ScreenDimChanged()
{
	int i;

	LTFLOAT fXRate		= 0.0f;
	LTFLOAT fYRate		= 0.0f;
	LTFLOAT fXCenter	= 0.0f;

	fXRate = 128.0f;
	fYRate = 32.0f;

	g_pDrawPrim->SetXYWH( &m_hPolyTimerBack,
						   m_fBasePosX * g_pInterfaceResMgr->GetYRatio(),
						   m_fBasePosY * g_pInterfaceResMgr->GetYRatio(),
						   fXRate * g_pInterfaceResMgr->GetXRatio(),
						   fYRate * g_pInterfaceResMgr->GetYRatio() );

	fXCenter = (m_fBasePosX + fXRate * 0.5f);
	g_pDrawPrim->SetXYWH( &m_hPolyTimerSeperator,
						   (fXCenter - 2.0f) * g_pInterfaceResMgr->GetXRatio(),
						   m_fBasePosY * g_pInterfaceResMgr->GetYRatio(),
						   16.0f * g_pInterfaceResMgr->GetXRatio(),
						   32.0f * g_pInterfaceResMgr->GetYRatio() );

	float fX[TIMER_DIGIT] = { fXCenter - 37, fXCenter - 22, 
							  fXCenter + 6, fXCenter + 21 };

	float fY = m_fBasePosY+6.0f;
	float fW = 16.0f;
	float fH = 32.0f;

	fY *= g_pInterfaceResMgr->GetYRatio();
	fW *= g_pInterfaceResMgr->GetXRatio();
	fH *= g_pInterfaceResMgr->GetYRatio();

	for ( i = 0 ; i < TIMER_DIGIT ; ++i )
	{
		fX[i] *= g_pInterfaceResMgr->GetXRatio();
		g_pDrawPrim->SetXYWH( &m_hPolyTimerNumber[i], fX[i], fY, fW, fH);
	}
}






CHUDTimeBombTimer::CHUDTimeBombTimer()
: CHUDTimer()
{
	m_bDefused		= LTFALSE;
	m_fBasePosX		= (800.0f - 176.0f) * 0.5f;
	m_fBasePosY		= 35.0f; 
	m_eLevel		= kHUDRenderNone;
}

LTBOOL CHUDTimeBombTimer::Init()
{
	int		i;
	char	szTex[256];
	
	m_hTimerBack	= g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/TIMER/b_TimerBackground.dtx" );
	SetupQuadUVs( m_hPolyTimerBack, m_hTimerBack, 0.0f, 0.0f, 1.0f, 1.0f );
	g_pDrawPrim->SetRGBA( &m_hPolyTimerBack, 0xFFFFFFFF );
	
	m_hTimerSeprator = g_pInterfaceResMgr->GetTexture( "SA_INTERFACE/HUD/TIMER/TimeSeperator.dtx" );

	for ( i = 0; i < 2; i++ )
	{
		SetupQuadUVs( m_hPolyTimerSeperator[i], m_hTimerSeprator, 0.0f, 0.0f, 1.0f, 1.0f );
		g_pDrawPrim->SetRGBA( &m_hPolyTimerSeperator[i], 0xFFFFFFFF );
	}
	
	for( i = 0 ; i < 10 ; ++i )
	{
		sprintf( szTex, "SA_INTERFACE/HUD/TIMER/b-%d.DTX", i );
		m_hTimerNumber[i] = g_pInterfaceResMgr->GetTexture( szTex );
	}
	
	for( i = 0 ; i < BOMB_TIMER_DIGIT ; ++i )
	{
		SetupQuadUVs( m_hPolyTimerNumber[i], m_hTimerNumber[0], 0.0f, 0.0f, 1.0f, 1.0f);
		g_pDrawPrim->SetRGBA( &m_hPolyTimerNumber[i], 0xFFFFFFFF );
	}
	
	this->ScreenDimChanged();
	return LTTRUE;
}

void CHUDTimeBombTimer::InitTimer() 
{
	SetDetonateTimer(0.0f);		
	SetTimeBombDefused(LTFALSE);
	SetDraw(LTFALSE);
}

void CHUDTimeBombTimer::Term()
{
	CHUDTimer::Term();
}

void CHUDTimeBombTimer::Update()
{
	if( !m_bDraw || m_fTime <= 0.0f || m_bPause ) return;

	if( IsTimeBombDefused() ) return;

	float fCountDownTime;
	fCountDownTime = m_fTime - g_pLTClient->GetTime();
	
	if ( !g_bBeepSound )
	{
		g_nBeepSoundCount++;
		if(g_nBeepSoundCount >= g_nBeepSoundDelay)
			g_bBeepSound = LTTRUE;
	}
	else
	{					
		g_nBeepSoundCount = 0;

		if		(	fCountDownTime > 21		)	g_nBeepSoundDelay = 100;
		else if (	fCountDownTime > 11		)	g_nBeepSoundDelay = 50;
		else if (	fCountDownTime > 6		)	g_nBeepSoundDelay = 30;
		else	g_nBeepSoundDelay = 25;

		LTFLOAT m_fSoundRadius = 2500.0f;
		g_pClientSoundMgr->PlaySoundFromPos( g_pPlayerMgr->GetTimeBombPos(),
											 "SA_Weapons\\SND\\TimeBomb\\beep.wav",
											 m_fSoundRadius, SOUNDPRIORITY_MISC_LOW, LTNULL );

		g_bBeepSound = LTFALSE;
	}

	// Update/Draw the timer if there is anytime left...
	if ( fCountDownTime <= 0.0f )
	{
		m_nMinute_10	= 0;
		m_nMinute_1		= 0;
		m_nSecond_10	= 0;
		m_nSecond_1		= 0;
		m_nMillSec_10	= 0;
		m_nMillSec_1	= 0;
	}
	else
	{
		// Draw the string to the surface...
		m_nMinute_10	= int(fCountDownTime) / 600;
		fCountDownTime	-= 600.0f*m_nMinute_10;

		m_nMinute_1		= int(fCountDownTime) / 60;
		fCountDownTime	-= 60.0f*m_nMinute_1;

		m_nSecond_10	= int(fCountDownTime) / 10;
		fCountDownTime	-= 10.0f*m_nSecond_10;

		m_nSecond_1		= int(fCountDownTime);
		fCountDownTime	-= float(m_nSecond_1);

		fCountDownTime	*= 100.0f;

		m_bSeperatorBlink = ( int(fCountDownTime) / 10 >= 5 ) ? LTTRUE : LTFALSE;
		
		m_nMillSec_10	= int(fCountDownTime) / 10;
		fCountDownTime	-= 10.0f*m_nMillSec_10;

		m_nMillSec_1	= int(fCountDownTime);
	}
}

void CHUDTimeBombTimer::Render()
{
	if ( !m_bDraw && m_fTime > 0.0f ) return;

	g_pDrawPrim->SetTexture( m_hTimerBack );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerBack );

	if ( m_bSeperatorBlink )
	{
		g_pDrawPrim->SetTexture( m_hTimerSeprator );
		g_pDrawPrim->DrawPrim( &m_hPolyTimerSeperator[0] );
		g_pDrawPrim->SetTexture( m_hTimerSeprator );
		g_pDrawPrim->DrawPrim( &m_hPolyTimerSeperator[1] );
	}
	
	g_pDrawPrim->SetTexture( m_hTimerNumber[m_nMinute_10] );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerNumber[0] );

	g_pDrawPrim->SetTexture( m_hTimerNumber[m_nMinute_1] );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerNumber[1] );

	g_pDrawPrim->SetTexture( m_hTimerNumber[m_nSecond_10] );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerNumber[2] );

	g_pDrawPrim->SetTexture( m_hTimerNumber[m_nSecond_1] );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerNumber[3] );

	g_pDrawPrim->SetTexture( m_hTimerNumber[m_nMillSec_10] );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerNumber[4] );

	g_pDrawPrim->SetTexture( m_hTimerNumber[m_nMillSec_1] );
	g_pDrawPrim->DrawPrim( &m_hPolyTimerNumber[5] );
}

VOID
CHUDTimeBombTimer::SetDetonateTimer(LTFLOAT fDetonateTimer)
{
	SetTime(fDetonateTimer, LTFALSE);
	SetDraw( (fDetonateTimer>0.0f) ? LTTRUE : LTFALSE);
}

void CHUDTimeBombTimer::ScreenDimChanged()
{
	int i;

	LTFLOAT fXRate		= 0.0f;
	LTFLOAT fYRate		= 0.0f;
	
	fXRate = 256.0f * g_pInterfaceResMgr->GetXRatio();
	fYRate = 64.0f  * g_pInterfaceResMgr->GetYRatio();
	
	g_pDrawPrim->SetXYWH( &m_hPolyTimerBack,
						   m_fBasePosX * g_pInterfaceResMgr->GetYRatio(),
						   m_fBasePosY * g_pInterfaceResMgr->GetYRatio(),
						   fXRate, fYRate );

	LTFLOAT fBaseTimerDigitX = m_fBasePosX + 53.0f;

	float fX[6] = { fBaseTimerDigitX, fBaseTimerDigitX + 18.0f, fBaseTimerDigitX + 40.0f, 
					fBaseTimerDigitX + 58.0f, fBaseTimerDigitX + 80.0f, fBaseTimerDigitX + 98.0f };
	float fY = m_fBasePosY + 10.0f;
	float fW = 32.0f;
	float fH = 32.0f;

	g_pDrawPrim->SetXYWH( &m_hPolyTimerSeperator[0],
							(fX[2] - 5.0f) * g_pInterfaceResMgr->GetXRatio(),
							(fY - 3.0f) * g_pInterfaceResMgr->GetYRatio(),
							16.0f * g_pInterfaceResMgr->GetXRatio(),
							32.0f * g_pInterfaceResMgr->GetYRatio() );
	
	g_pDrawPrim->SetXYWH( &m_hPolyTimerSeperator[1],
							(fX[4] - 5.0f) * g_pInterfaceResMgr->GetXRatio(),
							(fY - 3.0f) * g_pInterfaceResMgr->GetYRatio(),
							16.0f * g_pInterfaceResMgr->GetXRatio(),
							32.0f * g_pInterfaceResMgr->GetYRatio() );
	

	fY *= g_pInterfaceResMgr->GetYRatio();
	fW *= g_pInterfaceResMgr->GetXRatio();
	fH *= g_pInterfaceResMgr->GetYRatio();

	for ( i = 0 ; i < BOMB_TIMER_DIGIT ; ++i )
	{
		fX[i] *= g_pInterfaceResMgr->GetXRatio();
		g_pDrawPrim->SetXYWH( &m_hPolyTimerNumber[i], fX[i], fY, fW, fH);
	}
}
