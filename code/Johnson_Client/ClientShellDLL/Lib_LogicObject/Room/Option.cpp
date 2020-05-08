#include "StdAfx.h"

#include "./Option.h"

namespace LogicObject {		namespace Room {

void
Option::SetTeamBalance( bool i_bBalance )
{
	if( i_bBalance == true )
	{
		m_cOption |= E_BIT::BALANCE;
	}
	else
	{
		m_cOption &= ~ E_BIT::BALANCE;
	}
}

bool
Option::IsTeamBalance()
{
	if( ( m_cOption & E_BIT::BALANCE ) == 0x00 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

void
Option::SetRunnableInBattle( bool i_bJoin )
{
	if( i_bJoin == true )
	{
		m_cOption |= E_BIT::RUNBATTLE;
	}
	else
	{
		m_cOption &= ~ E_BIT::RUNBATTLE;
	}
}

bool
Option::CanRunnableInBattle() const
{
	if( ( m_cOption & E_BIT::RUNBATTLE ) == 0x00 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

void
Option::SetPassword( bool i_bPass )
{
	if( i_bPass == true )
	{
		m_cOption |= E_BIT::PASSWORD;
	}
	else
	{
		m_cOption &= ~ E_BIT::PASSWORD;
	}
}

bool
Option::IsPassword()
{
	if( ( m_cOption & E_BIT::PASSWORD ) == 0x00 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

void
Option::SetPlaying( bool i_bPlaying )
{
	if( i_bPlaying == true )
	{
		m_cOption |= E_BIT::PLAYING;
	}
	else
	{
		m_cOption &= ~ E_BIT::PLAYING;
	}
}

bool
Option::IsPlaying() const
{
	if( ( m_cOption & E_BIT::PLAYING ) == 0x00 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

void
Option::SetMode( E_MODE::ENUM i_eMode )
{
	m_cModeTeam &= 0xf0;
	m_cModeTeam |= ( 0xff & i_eMode );
}

Option::E_MODE::ENUM
Option::GetMode()
{
	E_MODE::ENUM eMode		= static_cast<E_MODE::ENUM>( m_cModeTeam & 0x0f );
	return eMode;
}

void
Option::SetTeamMax( E_TEAMMAX::ENUM i_eTeamMax )
{
	m_cModeTeam &= 0x0f;
	m_cModeTeam |= ( 0xff & ( i_eTeamMax << 4 ) );
}

void
Option::SetTeamMaxForValue( const unsigned int i_nTeamMax )
{
	unsigned int nEnumValue		= i_nTeamMax - 1;

	if( nEnumValue >= E_TEAMMAX::__MAX__ )
	{
		nEnumValue			= E_TEAMMAX::__MAX__ - 1;
	}
	SetTeamMax( static_cast<E_TEAMMAX::ENUM>( nEnumValue ) );
}

unsigned int
Option::GetTeamMax() const
{
	E_TEAMMAX::ENUM eMax	= static_cast<E_TEAMMAX::ENUM>( ( m_cModeTeam & 0xf0 ) >> 4 );
	
	return eMax + 1;
}

void
Option::SetTime( E_TIME::ENUM i_eTime )
{
	m_cTimeKill &= 0xf0;
	m_cTimeKill |= ( 0xff & i_eTime );
}

void
Option::SetTimeForValue( const unsigned int i_nTime )
{
	E_TIME::ENUM eTime;
	switch( i_nTime )
	{
	case E_TIME::T5 * 5 + 5 :
		eTime = E_TIME::T5;
		break;
	case E_TIME::T10 * 5 + 5 :
		eTime = E_TIME::T10;
		break;
	case E_TIME::T15 * 5 + 5 :
		eTime = E_TIME::T15;
		break;
	case E_TIME::T20 * 5 + 5 :
		eTime = E_TIME::T20;
		break;
	case E_TIME::T25 * 5 + 5 :
		eTime = E_TIME::T25;
		break;
	case E_TIME::T30 * 5 + 5 :
		eTime = E_TIME::T30;
		break;
	case E_TIME::T35 * 5 + 5 :
		eTime = E_TIME::T35;
		break;
//	case E_TIME::T40 * 5 + 5 :
	default:
		eTime = E_TIME::T40;
		break;
	}

	SetTime( eTime );
}

unsigned int
Option::GetTime() const
{
	E_TIME::ENUM eTime		= static_cast<E_TIME::ENUM>( m_cTimeKill & 0x0f );
	return( eTime * 5 + 5);
}

void
Option::SetKillLimit( E_KILL::ENUM i_eKillLimit )
{
	m_cTimeKill &= 0x0f;
	m_cTimeKill |= ( 0xff & ( i_eKillLimit << 4 ) );
}

void
Option::SetKillLimitForValue( const unsigned int i_nKillLimit )
{
	E_KILL::ENUM eKill;
	switch( i_nKillLimit )
	{
	case E_KILL::K20 * 20 + 20 :
		eKill = E_KILL::K20;
		break;
	case E_KILL::K40 * 20 + 20 :
		eKill = E_KILL::K40;
		break;
	case E_KILL::K60 * 20 + 20 :
		eKill = E_KILL::K60;
		break;
	case E_KILL::K80 * 20 + 20 :
		eKill = E_KILL::K80;
		break;
	case E_KILL::K100 * 20 + 20 :
		eKill = E_KILL::K100;
		break;
	case E_KILL::K120 * 20 + 20 :
		eKill = E_KILL::K120;
		break;
	case E_KILL::K140 * 20 + 20 :
		eKill = E_KILL::K140;
		break;
//	case E_KILL::K160 * 20 + 20 :
	default:
		eKill = E_KILL::K160;
		break;
	}

	SetKillLimit( eKill );
}

unsigned int
Option::GetKillLimit() const
{
	E_KILL::ENUM eKill		= static_cast<E_KILL::ENUM>( ( m_cTimeKill & 0xf0 ) >> 4 );

	return( eKill * 20 + 20 );
}

void
Option::SetRoundLimit( unsigned char i_byRound )
{
	m_byRound				= i_byRound;
}

unsigned char
Option::GetRoundLimit() const
{
	if( m_byRound == 0 ||
		m_byRound > E_ROUND::__MAX__ )
	{
		return static_cast<unsigned char>( E_ROUND::__MAX__ );
	}
	else
	{
		return m_byRound;
	}
}

void
Option::SetMapIndex( unsigned char i_byMap )
{
	m_byMap					= i_byMap;
}

unsigned char
Option::GetMapIndex() const
{
	return m_byMap;
}

void
Option::SetFree( const bool bFree )
{
	m_bFree					= bFree;
}

const bool
Option::IsFree() const
{
	return m_bFree;
}

} /* Room */				} /* LogicObject */