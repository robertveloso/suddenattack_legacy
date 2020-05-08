// ----------------------------------------------------------------------- //
//
// MODULE  : HUDRadar.h
//
// PURPOSE : HUDItem to display radar
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_RADAR_H__
#define __HUD_RADAR_H__


//
// Includes...
//

	#include "HUDItem.h"
	#include "ltguimgr.h"
#include "../../Shared/LTPoly.h"

struct RADAR_PLAYER_OBJ
{
	RADAR_PLAYER_OBJ()
	{
		nID = -1;
		hObj = NULL;
		bDead = false;
		fFlashTime = 0.0f;
		fCurFlashTime = 0.0f;
		bFlashOn = false;
	}

	void Render();

	uint32					nID;
	HOBJECT					hObj;
	bool					bDead;
	float					fFlashTime;
	float					fCurFlashTime;
	bool					bFlashOn;

};

struct RADAROBJECT
{
	RADAROBJECT()
	{
		m_bDraw			= false;
		m_hTex			= LTNULL;
		m_hTexRing		= LTNULL;
		m_nDrawOrder	= 0;
		m_bFlashOn		= false;
		m_fRingAlpha	= 255.0f;
		memset( &m_Poly, 0, sizeof( m_Poly ));
		memset( &m_PolyRing, 0, sizeof( m_Poly ));
	}

	bool		m_bDraw;
	LTPoly_GT4	m_Poly;
	HTEXTURE	m_hTex;

	LTPoly_GT4	m_PolyRing;
	HTEXTURE	m_hTexRing;

	int			m_nDrawOrder;
	uint8		m_nVisibleTeamId;
	bool		m_bFlashOn;
	float		m_fCenterX;
	float		m_fCenterY;
	float		m_fRadius;
	float		m_fRingAlpha;
	float		m_fObjectSize;
	float		m_fBasePosX;
	float		m_fBasePosY;
};



class CHUDRadar : public CHUDItem
{
	public: // Methods...

		CHUDRadar();
		~CHUDRadar();

		LTBOOL	Init();
		void	Term();

		void	Render();
		void	Update();

		void	UpdateLayout();
		
		void	Toggle() { m_bDraw = !m_bDraw; }
		void	SetDraw( bool bDraw ) { m_bDraw = bDraw; }
		bool	GetDraw( ) { return m_bDraw; }

		void	AddObject( HOBJECT hObj, uint8 nRadarType, uint8 nVisibleTeamId );
		void	AddObject( HOBJECT hObj, const char *pRadarType, uint8 nVisibleTeamId );
		void	RemoveObject( HOBJECT hObj );

		void	ChangeRadarType( HOBJECT hObj, const char *pRadarType );

		void	AddPlayer( HOBJECT hObj, uint32 nId );
		void	RemovePlayer( HOBJECT hObj);
		void	SetPlayerDead( HOBJECT hObj, bool bDead );
		void	SetPlayerTalk(uint32 nId);
		void	UpdatePlayerID( HOBJECT hObj, uint32 nId);

		void	Reset();
		
		void	ScreenDimChanged();


	private: // Members...

		typedef std::map<HOBJECT, RADAROBJECT*> RadarObjectList;
		RadarObjectList m_mapRadarObjects;

		//jrg- 9/7/02 - added as a quick and dirty solution to radar clutter
		//					for a cleaner solution maybe these lists could be consolidated
		typedef std::list<RADAROBJECT*> RadarObjectSortList;
		RadarObjectSortList m_ROSortList;

		LTIntPt		m_BasePos;
		uint16		m_nBaseSize;
				
		bool		m_bDraw;
		uint32		m_nMaxShowDist;
		float		m_fLastRotation;

		typedef std::vector<RADAR_PLAYER_OBJ*> RadarPlayerList;
		RadarPlayerList		m_Players;
		
};

#endif // __HUD_RADAR_H__
