#ifndef __HUD_MINIMAP_H
#define __HUD_MINIMAP_H

#include "HUDItem.h"

struct PLAYER_IN_MINIMAP
{
	PLAYER_IN_MINIMAP()
	{
		nID		= -1;
		hObj	= LTNULL;
		hTex	= LTNULL;
		bDead	= false;
	}

	uint32					nID;
	HOBJECT					hObj;
	HTEXTURE				hTex;
	LTPoly_GT4				hPoly;
	bool					bDead;
};

class CHUDMiniMap : public CHUDItem
{
public:
	CHUDMiniMap();

    LTBOOL      Init();
	void		Term();

    void        Render();
    void        Update();

	void		ScreenDimChanged();

    void		Toggle() {m_bDraw = !m_bDraw;}
	void		SetDraw( bool bDraw ) { m_bDraw = bDraw; }
	bool		GetDraw( ) { return m_bDraw; }

	void		SetMapSize(int nx, int ny);
	void		SetMapSourceOffset(LTVector vOffset) { m_vMapSourceOffset = vOffset; }
	void		SetMapTexture(HTEXTURE hTex);
	void		MiniMapReset();
	void		AddPlayer( HOBJECT hObj, uint32 nId );
	void		RemovePlayer( HOBJECT hObj );
	void		SetPlayerState( HOBJECT hObj, bool bDead );

private:
    bool		m_bDraw;

	HTEXTURE		m_hTexMiniMap;
	LTPoly_GT4		m_hPolyMiniMap;
	
	int				m_nMapSize_X;
	int				m_nMapSize_Y;
	float			m_fMapScale_X;
	float			m_fMapScale_Y;
	LTVector		m_vMapSourceOffset;

	typedef std::vector<PLAYER_IN_MINIMAP*> MiniMapPlayerList;
	MiniMapPlayerList		m_Players;
};

#endif // __HUD_MINIMAP_H