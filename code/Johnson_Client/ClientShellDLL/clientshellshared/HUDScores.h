// ----------------------------------------------------------------------- //
//
// MODULE  : HUDScores.h
//
// PURPOSE : Definition of CHUDScores to display player scores
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_SCORES_H
#define __HUD_SCORES_H

#include "HUDItem.h"
#include "HanText.h"
#include "PlayerInfoMgr.h"

#define HUD_SCORE_TAG	"Scores"
#define MAX_TEAM_COUNT	2

struct sScore_Item
{
	sScore_Item()
		: m_nNumber(0), m_nGrade(0), m_nId(-1), m_nTeamId(-1), m_ePlayerState(0), m_nPing(0)
	{
		m_pNameFont	= LTNULL;
		m_pKillFont	= LTNULL;
		m_pDeadFont	= LTNULL;
	}
	~sScore_Item()
	{
		SAFE_DELETE( m_pNameFont );
		SAFE_DELETE( m_pKillFont );
		SAFE_DELETE( m_pDeadFont );
	}

	int				m_nId;
	uint8			m_nGrade;
	int				m_nNumber;
	int				m_nTeamId;
	int				m_nPing;
	uint32			m_ePlayerState;
	CSAD3DText*		m_pNameFont;
	CSAD3DText*		m_pKillFont;
	CSAD3DText*		m_pDeadFont;
	LTPoly_GT4		m_PolyGrade;
	LTPoly_GT4		m_PolyPing;
	LTBOOL			m_bRender;
};



class CHUDScores : public CHUDItem
{
public:
	CHUDScores();

    virtual LTBOOL      Init();
	virtual void		Term();

	virtual void		DeleteD3DText();
    virtual void        Render();
    virtual void        Update();
	
	bool				AddPlayer( const char* lpszNick, int nID, uint8 nGrade );
	bool				RemovePlayer( int nID );
	void				RemoveAll();
	
	void				ForceRender();
	void				RenderTopScore();

	void				ScreenDimChanged();
	virtual void        UpdateLayout();

	void				UpdateScores();
	void				SetUpdate( bool bUpdate );
	void				SetPing( uint8 nID, uint8 nPing );

	virtual	void		Show(bool bVisible);
	
	//[SPIKE]
	VOID				SetWinRound_Team(uint16 nWinRoundA, uint16 nWinRoundB);
	void				SetKillDeath( uint32 nRedKill, uint32 nBlueKill );
	void				ClearTotalScore();
	void				AddKillCount( uint8 nTeamID );
	void				SetChannelName( const char* lpszName );
	
	void				PrePlayingState();

	enum Constants
	{
		kMaxPlayers	= 16,
		kNumTeams	= 3,
		kMaxTeams	= 2,
	};

	enum EN_TEAM_NAME
	{
		ETN_RED		= 0,
		ETN_BLUE	= 1,
	};

	sScore_Item		m_ScorePrint[kMaxPlayers];
	
protected:

	sScore_Item*	GetScoreItem( int nPlayerID );
	void			SetScoreItem( sScore_Item* pScoreItem, T_PLAYER_INFOptr pPlayerInfo, int nCount );
	
	void			UpdateTeamPos(uint8 nTeam, LTIntPt pos);
	void			UpdateBoardPos();
	void			UpdateTopScores();
	
#ifndef _FINAL
	void			DoNoisyTextTest();
	void			DoSoundTest();
#endif	//#ifndef _FINAL

	// 광식 [04.06.16] 인스턴스 형태
	bool			m_bVisible;

	LTIntPt			m_ptScoreBoardBasePos;
	int				m_nScoreBoardWidth;
	int				m_nScoreBoardHeight;

	int				m_nGameTypeTopOffset;
	HTEXTURE		m_hGameType[2];
	LTPoly_GT4		m_PolyGameType;
	
	HTEXTURE		m_hCaptainTx;
	LTPoly_GT4		m_PolyCaptain;

	LTIntPt			m_ptTeamFrameOffset[kMaxTeams];
	int				m_nTeamFrameWidth;
	int				m_nTeamFrameHeight;

	LTIntPt			m_ptTeamTotalScoreOffset[kMaxTeams];
	
	int				m_nTopOffset;
	int				m_nMarkOffset;
	int				m_nGradeOffset;
	int				m_nNameOffset;
	int				m_nKillOffset;
	int				m_nDeathOffset;
	int				m_nPingOffset;
	LTIntPt			m_ptWinCountOffset;
	int				m_nTotalTopOffset;
	
	int				m_nLineHeight;

	LTPoly_GT4		m_PolyScore[2];	
	HTEXTURE		m_hScoreTx[2];

	bool			m_bNeedUpdate;	

	CSAD3DText*		m_pChannelName;
	CSAD3DText*		m_pTotalScore;
	uint16			m_nTotalScore[kMaxTeams];
	
	LTPoly_GT4		m_PolyTextCap_A;
	LTPoly_GT4		m_PolyTextCap_B;
	HTEXTURE		m_hTextCap;

	LTPoly_GT4		m_PolyTopScore;
	LTPoly_GT4		m_PolyRedLight;
	LTPoly_GT4		m_PolyBlueLight;

	HTEXTURE		m_hTopScore;
	HTEXTURE		m_hRedLight;
	HTEXTURE		m_hBlueLight;

	LTPoly_GT4		m_PolyRedScore_100;
	LTPoly_GT4		m_PolyRedScore_10;
	LTPoly_GT4		m_PolyRedScore_1;

	LTPoly_GT4		m_PolyBlueScore_100;
	LTPoly_GT4		m_PolyBlueScore_10;
	LTPoly_GT4		m_PolyBlueScore_1;

	LTPoly_GT4		m_PolyScore_100;
	LTPoly_GT4		m_PolyScore_10;
	LTPoly_GT4		m_PolyScore_1;

	HTEXTURE		m_hNumTex[10];

	int				m_nRedScore_100;
	int				m_nRedScore_10;
	int				m_nRedScore_1;
	int				m_nBlueScore_100;
	int				m_nBlueScore_10;
	int				m_nBlueScore_1;

	int				m_nObjectiveScore_100;
	int				m_nObjectiveScore_10;
	int				m_nObjectiveScore_1;

	GameType		m_eGameType;

	
	
	// TEST : NOVICE
#ifndef _FINAL
	char			m_szWeaponSoundList[20][256];
#endif //#ifndef _FINAL
};

#endif