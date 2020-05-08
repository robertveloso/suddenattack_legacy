#pragma once

namespace LogicObject {		namespace Room {

struct Option
{
public:
	struct E_MODE {			enum ENUM {
		RESPAWN				= 3,
		BOMB				= 5,
		__MAX__
	}; /* ENUM */			}; /* E_MODE */

	struct E_TEAMMAX {		enum ENUM {
		VS1_1				= 0,
		VS2_2,
		VS3_3,
		VS4_4,
		VS5_5,
		VS6_6,
		VS7_7,
		VS8_8,
		__MAX__
	}; /* ENUM */			}; /* E_TEAMMAX */

	struct E_TIME {			enum ENUM {
		T5					= 0,
		T10,
		T15,
		T20,
		T25,
		T30,
		T35,
		T40,
		__MAX__
	}; /* ENUM */			}; /* E_TIME */

	struct E_KILL {			enum ENUM {
		K20					= 0,
		K40,
		K60,
		K80,
		K100,
		K120,
		K140,
		K160,
		__MAX__
	}; /* ENUM */			}; /* E_KILL */

	struct E_ROUND {		enum ENUM {
		__MAX__				= 15,
	}; /* ENUM */			}; /* E_ROUND */

public:
	void					SetTeamBalance( bool i_bBalance );
	bool					IsTeamBalance();

	void					SetRunnableInBattle( bool i_bJoin );
	bool					CanRunnableInBattle() const;

	void					SetPassword( bool i_bPass );
	bool					IsPassword();

	void					SetPlaying( bool i_bPlaying );
	bool					IsPlaying() const;

	//

	void					SetMode( E_MODE::ENUM i_eMode );
	E_MODE::ENUM			GetMode();

	void					SetTeamMax( E_TEAMMAX::ENUM i_eTeamMax );
	void					SetTeamMaxForValue( const unsigned int i_nTeamMax );
	unsigned int			GetTeamMax() const;

	//

	void					SetTime( E_TIME::ENUM i_eTime );
	void					SetTimeForValue( const unsigned int i_nTime );
	unsigned int			GetTime() const;

	void					SetKillLimit( E_KILL::ENUM i_eKillLimit );
	void					SetKillLimitForValue( const unsigned int i_nKillLimit );
	unsigned int			GetKillLimit() const;

	//

	void					SetRoundLimit( unsigned char i_byRound );
	unsigned char			GetRoundLimit() const;

	void					SetMapIndex( unsigned char i_byMap );
	unsigned char			GetMapIndex() const;

private:
	struct E_BIT {			enum ENUM {
		BALANCE				= 0x01,		// 0000 0001
		RUNBATTLE			= 0x02,		// 0000 0010
		PASSWORD			= 0x04,		// 0000 0100
		PLAYING				= 0x08,		// 0000 1000
	}; /* ENUM */			}; /* E_BIT */

private:
	unsigned char			m_cOption;
	unsigned char			m_cModeTeam;
	unsigned char			m_cTimeKill;
	unsigned char			m_byRound;
	unsigned char			m_byMap;

	bool					m_bFree;	//0803
};

} /* Room */				} /* LogicObject */