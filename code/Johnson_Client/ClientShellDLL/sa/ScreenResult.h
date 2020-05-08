#ifndef _SCREEN_RESULT_H_
#define _SCREEN_RESULT_H_

#include "ScreenBase.h"
#include "ScreenCommands.h"
#include "NetDefs.h"

//////////////////////////////////////////////////////////////////////////
#define RESULT_DELTA_TIME		15
#define MAX_PLAYERS_COUNT		16
#define MAX_TEAM_COUNT			2
//////////////////////////////////////////////////////////////////////////

struct sResultData
{
	sResultData()
		: nTeam(0), nGrade(0), nKill(0), nDead(0), nMissionSuccessCount(0), nExp(0), nMoney(0xFFFFFFFF)
	{
		ZeroMemory(szNick, 128);
	}
	
	~sResultData()
	{
	}
	
	char	szNick[128];
	WORD	nTeam;
	UINT	nGrade;
	WORD	nKill;
	WORD	nDead;
	WORD	nMissionSuccessCount;
	int		nExp;
	int		nMoney;
	int		nMiscFlags;
};

struct sResultPaint
{
	sResultPaint()
		: pTextNick(LTNULL),  pTextKill(LTNULL),  pTextDeath(LTNULL), pTextMissionSuccessCount(LTNULL),
		  pTextPoint(LTNULL), pTextMoney(LTNULL), hGradeIcon(LTNULL)
	{
	}

	~sResultPaint()
	{
		SAFE_DELETE( pTextNick		);
		SAFE_DELETE( pTextKill		);
		SAFE_DELETE( pTextDeath		);
		SAFE_DELETE( pTextMissionSuccessCount );
		SAFE_DELETE( pTextPoint		);
		SAFE_DELETE( pTextMoney		);
	}

	CSAD3DText*		pTextNick;
	CSAD3DText*		pTextKill;
	CSAD3DText*		pTextDeath;
	CSAD3DText*		pTextMissionSuccessCount;
	CSAD3DText*		pTextPoint;
	CSAD3DText*		pTextMoney;
	
	HTEXTURE		hGradeIcon;
	LTPoly_GT4		PolyClanMark;
	LTPoly_GT4		PolyGrade;
	
};

class CScreenResult : public CScreenBase
{
public :
	CScreenResult();
	virtual ~CScreenResult();

	LTBOOL  Build();
	void	Term();
	
    void    OnFocus(LTBOOL bFocus);

	LTBOOL	Render(HSURFACE hDestSurf);

	// UI 다시 작업
	//--------------------------------------------------------------------------------
	void	SetRoundScore( WORD nWinRound_A, WORD nWinRound_B );
	void	SetResult_User( const char* szNick, WORD nTeam, UINT nGrade,
							WORD nKill, WORD nDead, WORD nMissionSuccessCount,
							UINT nExp, UINT nMoney, int nMiscFlags );
	void	SetResult_Team( WORD nWinTeam, WORD nResultCount );
	void	Result_Clear();
	void	Result_Sort();
	void	Result_Position();
	void	Result_Render();
	void	Result_Init();
	void	Result_WinTeam();
	void	Result_CreateD3DText();
	//--------------------------------------------------------------------------------

protected:
	LTBOOL	OnEscape();
	
	enum eCommand
	{
		CMD_RESULT_OK = CMD_CUSTOM + 100,
	};
	
	uint32				OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
	virtual LTBOOL		OnEnter();
	LTBOOL				ResultOKProc();

	CLTGUIButton*		m_pBtnOk;
	
	LTBOOL				m_bInitted;

	// UI 다시 작업
	WORD				m_nResultCount;
	WORD				m_nWinTeam;
	WORD				m_nTotalData_Red;
	WORD				m_nTotalData_Blue;

	int					m_nResultIndex;
	
	sResultData			m_ResultData[MAX_PLAYERS_COUNT];
	sResultPaint		m_ResultPaint[MAX_PLAYERS_COUNT];
	
	// [NOVICE]
	enum EN_TEAM_NAME
	{
		ETN_RED		= 0,
		ETN_BLUE	= 1,
		ETN_DRAW	= 2,
	};

	LTBOOL				m_bFirstUpdate;
	LTFLOAT				m_fLastShownTime;

	LTPoly_GT4			m_PolyWinTeam[MAX_TEAM_COUNT];
	HTEXTURE			m_hTxWinTeam[MAX_TEAM_COUNT + 1];
	int					m_nWinTxIndex;
	
	LTIntPt				m_ptTeamFrameOffset[MAX_TEAM_COUNT];
	HTEXTURE			m_hTxWinTeamFrame[2];
	HTEXTURE			m_hMyInfoBar;
	HTEXTURE			m_hClanMarkTx;
	
	int					m_nTopOffset;
	int					m_nInfoBarOffset;
	int					m_nClanMarkOffset;
	int					m_nGradeOffset;
	int					m_nNameOffset;
	int					m_nKillOffset;
	int					m_nDeathOffset;
	int					m_nMissionSuccessCountOffset;
	int					m_nPointOffset;
	int					m_nMoneyOffset;
	int					m_nETCOffset;
	int					m_nPlayerScoreLineHeight;
	LTIntPt				m_ptWinCountOffset;

	LTPoly_GT4			m_PolyMyInfoBar;
	LTPoly_GT4			m_PolyWinTeamFrame[MAX_TEAM_COUNT][2];

	CSAD3DText*			m_pWinRoundRed;
	CSAD3DText*			m_pWinRoundBlue;
};

extern CScreenResult* g_pScreenResult;

#endif // _SCREEN_RESULT_H_