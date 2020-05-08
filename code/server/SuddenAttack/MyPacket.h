#ifndef __MURSUM_MYPACKET_H__
#define __MURSUM_MYPACKET_H__

#include "../jolyo/NJean/Application/packet.h"
#include "MyCommon.h"

namespace MyPacket
{
	class CMyPacketMyProperty : public CPacket
	{
	public:
		__int64	m_nExp;
		UINT	m_nGrade;
		char	m_szID[MAX_ID_LEN+1];
		char	m_szNick[MAX_NICK_LEN+1];
		UINT	m_nKill;
		UINT	m_nDeath;
		UINT	m_nWin;
		UINT	m_nLose;
		UINT	m_nDraw;
		__int64	m_nMoney;

		BOOL	m_bCanbeCaptain;

	public:
		CMyPacketMyProperty()
			: CPacket( sizeof( CMyPacketMyProperty ), CMyCommon::MyPacketCommand::MyProperty ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketMyProperty ) == nPacketLength;
			}
	};// class CMyPacketMyProperty

	class CMyPacketJoinRoomReply : public CPacket
	{
	public:
		int	m_nRoomIndex;

	public:
		CMyPacketJoinRoomReply()
			: CPacket( sizeof( CMyPacketJoinRoomReply ), CMyCommon::MyPacketCommand::JoinRoomReply ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketJoinRoomReply ) == nPacketLength;
			}
	};// class CMyPacketJoinRoomReply

	class CMyPacketGetRoomInfo : public CPacket
	{
	public:
		CMyPacketGetRoomInfo()
			: CPacket( sizeof( CMyPacketGetRoomInfo ), CMyCommon::MyPacketCommand::GetRoomInfo ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketGetRoomInfo ) == nPacketLength;
			}
	};// class CMyPacketGetRoomInfo

	class CMyPacketRoomUserProperty : public CPacket
	{
	public:
		char	m_szID[MAX_ID_LEN+1];
		char	m_szNick[MAX_NICK_LEN+1];
		short	m_nState;
		short	m_nTeam;
		UINT	m_nGrade;

	public:
		CMyPacketRoomUserProperty()
			: CPacket( sizeof( CMyPacketRoomUserProperty ), CMyCommon::MyPacketCommand::RoomUserProperty ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketRoomUserProperty ) == nPacketLength;
			}
	};// class CMyPacketRoomUserProperty

	class CMyPacketRoomOption : public CPacket
	{
	public:
		BYTE	m_nMapIndex;
		BYTE	m_nGameType;
		BYTE	m_nRoundLimit;
		BYTE	m_nTimeLimit;
		BYTE	m_nKillLimit;
		BYTE	m_nGradeMinLimit;
		BYTE	m_nGradeMaxLimit;		
		BYTE	m_nMaxUser;		
		BYTE	m_bInstrusion;
		BYTE	m_bTeamBalance;
		BYTE	m_nRoomStep;

	public:
		CMyPacketRoomOption()
			: CPacket( sizeof( CMyPacketRoomOption ), CMyCommon::MyPacketCommand::RoomOption ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketRoomOption ) == nPacketLength;
			}
	};// class CMyPacketRoomOption

	class CMyPacketRequestUserInfo : public CPacket
	{
	public:
		BOOL	m_bDetail;
		TCHAR	m_szID[CCasualCommon::CasualStringLength::UserID+1];

	public:
		CMyPacketRequestUserInfo()
			: CPacket( sizeof( CMyPacketRequestUserInfo ), CMyCommon::MyPacketCommand::RequestUserInfo ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketRequestUserInfo ) == nPacketLength;
			}
	};// class CMyPacketRequestUserInfo

	class CMyPacketUserInfo : public CPacket
	{
	public:
		UINT	m_nGrade;
		char	m_szNick[CCasualCommon::CasualStringLength::UserNick+1];
		UINT	m_nKill;
		UINT	m_nDeath;
		UINT	m_nWin;
		UINT	m_nLose;
		UINT	m_nDraw;
		__int64	m_nMoney;

	public:
		CMyPacketUserInfo()
			: CPacket( sizeof( CMyPacketUserInfo ), CMyCommon::MyPacketCommand::UserInfo ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketUserInfo ) == nPacketLength;
			}
	};// class CMyPacketUserInfo

	class CMyPacketChangeOptMap : public CPacket
	{
	public:
		short	m_nRoomIndex;
		BYTE	m_nGameType;
		BYTE	m_nMapIndex;

	public:
		CMyPacketChangeOptMap()
			: CPacket( sizeof( CMyPacketChangeOptMap ), CMyCommon::MyPacketCommand::ChangeOptMap ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketChangeOptMap ) == nPacketLength;
			}
	};// class CMyPacketChangeOptMap

	class CMyPacketChangeOptMaxUsers : public CPacket
	{
	public:
		short	m_nRoomIndex;
		short	m_nMaxUsers;

	public:
		CMyPacketChangeOptMaxUsers()
			: CPacket( sizeof( CMyPacketChangeOptMaxUsers ), CMyCommon::MyPacketCommand::ChangeOptMaxUsers ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketChangeOptMaxUsers ) == nPacketLength;
			}
	};// class CMyPacketChangeOptMaxUsers

	class CMyPacketChangeOptGameLimit : public CPacket
	{
	public:
		short	m_nRoomIndex;
		BYTE	m_nKillLimit;
		BYTE	m_nTimeLimit;
		BYTE	m_nRoundLimit;

	public:
		CMyPacketChangeOptGameLimit()
			: CPacket( sizeof( CMyPacketChangeOptGameLimit ), CMyCommon::MyPacketCommand::ChangeOptGameLimit ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketChangeOptGameLimit ) == nPacketLength;
			}
	};// class CMyPacketChangeOptGameLimit

	class CMyPacketChangeOptGradeLimit : public CPacket
	{
	public:
		short	m_nRoomIndex;
		BYTE	m_nMinLimit;
		BYTE	m_nMaxLimit;

	public:
		CMyPacketChangeOptGradeLimit()
			: CPacket( sizeof( CMyPacketChangeOptGradeLimit ), CMyCommon::MyPacketCommand::ChangeOptGradeLimit ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketChangeOptGradeLimit ) == nPacketLength;
			}
	};// class CMyPacketChangeOptGradeLimit

	class CMyPacketChangeOptInstrusion : public CPacket
	{
	public:
		short	m_nRoomIndex;
		short	m_bEnable;

	public:
		CMyPacketChangeOptInstrusion()
			: CPacket( sizeof( CMyPacketChangeOptInstrusion ), CMyCommon::MyPacketCommand::ChangeOptInstrusion ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketChangeOptInstrusion ) == nPacketLength;
			}
	};// class CMyPacketChangeOptInstrusion

	class CMyPacketChangeOptTeamBalance : public CPacket
	{
	public:
		short	m_nRoomIndex;
		short	m_bEnable;

	public:
		CMyPacketChangeOptTeamBalance()
			: CPacket( sizeof( CMyPacketChangeOptTeamBalance ), CMyCommon::MyPacketCommand::ChangeOptTeamBalance ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketChangeOptTeamBalance ) == nPacketLength;
			}
	};// class CMyPacketChangeOptTeamBalance

	class CMyPacketChangeTeam : public CPacket
	{
	public:
		char	m_szID[MAX_ID_LEN+1];
		int		m_nTeam;
		int		m_nType;
		
	public:
		CMyPacketChangeTeam()
			: CPacket( sizeof( CMyPacketChangeTeam ), CMyCommon::MyPacketCommand::ChangeTeam ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketChangeTeam ) == nPacketLength;
			}
	};// class CMyPacketChangeTeam

	class CMyPacketStartGameRequest : public CPacket
	{
	public:
		BOOL	m_bCanBeCaptain;

	public:
		CMyPacketStartGameRequest()
			: CPacket( sizeof( CMyPacketStartGameRequest ), CMyCommon::MyPacketCommand::StartGameRequest ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketStartGameRequest ) == nPacketLength;
			}
	};// class CMyPacketStartGameRequest

	class CMyPacketStartHost : public CPacket
	{
	public:
		CMyPacketStartHost()
			: CPacket( sizeof( CMyPacketStartHost ), CMyCommon::MyPacketCommand::StartHost ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketStartHost ) == nPacketLength;
			}
	};// class CMyPacketStartHost

	class CMyPacketStartHostReply : public CPacket
	{
	public:
		BOOL	m_bHostStarted;
		TCHAR	m_szHostIP[MAX_IP_LEN];
		int		m_nHostPort;

	public:
		CMyPacketStartHostReply()
			: CPacket( sizeof( CMyPacketStartHostReply ), CMyCommon::MyPacketCommand::StartHostReply ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketStartHostReply ) == nPacketLength;
			}
	};// class CMyPacketStartHostReply

	class CMyPacketStartGame : public CPacket
	{
	public:
		TCHAR	m_szIP[MAX_IP_LEN];
		int		m_nPort;
		BYTE	m_bSpectator;

	public:
		CMyPacketStartGame()
			: CPacket( sizeof( CMyPacketStartGame ), CMyCommon::MyPacketCommand::StartGame ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketStartGame ) == nPacketLength;
			}
	};// class CMyPacketStartGame

	class CMyPacketChangeRoomState : public CPacket
	{
	public:
		int	m_nState;

	public:
		CMyPacketChangeRoomState()
			: CPacket( sizeof( CMyPacketChangeRoomState ), CMyCommon::MyPacketCommand::ChangeRoomState ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketChangeRoomState ) == nPacketLength;
			}
	};// class CMyPacketChangeRoomState

	class CMyPacketFailStartGame : public CPacket
	{
	public:
		int	m_nResult;

	public:
		CMyPacketFailStartGame()
			: CPacket( sizeof( CMyPacketFailStartGame ), CMyCommon::MyPacketCommand::FailStartGame ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketFailStartGame ) == nPacketLength;
			}
	};// class CMyPacketFailStartGame

	class CMyPacketGameResultUser : public CPacket
	{
	public:
		TCHAR	m_szID[MAX_ID_LEN+1];
		TCHAR	m_szNick[MAX_NICK_LEN+1];
		WORD	m_nTeam;
		WORD	m_nGameType;
		UINT	m_nGrade;
		WORD	m_nKill;
		WORD	m_nDead;
		UINT	m_nExp;
		UINT	m_nMoney;
		WORD	m_nWinCount;
		WORD	m_nLoseCount;
		WORD	m_nDrawCount;		

	public:
		CMyPacketGameResultUser()
			: CPacket( sizeof( CMyPacketGameResultUser ), CMyCommon::MyPacketCommand::GameResultUser ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketGameResultUser ) == nPacketLength;
			}
	};// class CMyPacketGameResultUser

	class CMyPacketGameResultTeam : public CPacket
	{
	public:
		WORD	m_nGameType;
		WORD	m_nWinTeam;
	public:
		CMyPacketGameResultTeam()
			: CPacket( sizeof( CMyPacketGameResultTeam ), CMyCommon::MyPacketCommand::GameResultTeam ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketGameResultTeam ) == nPacketLength;
			}
	};// class CMyPacketGameResultTeam

	class CMyPacketEndRound : public CPacket
	{
	public:
		WORD	m_nGameType;
		WORD	m_nGameResult;
		WORD	m_nWinRound_A;
		WORD	m_nWinRound_B;
		BYTE	m_bRestartRound;
		BYTE	m_nHow;
		BYTE	m_bReserved2;
		BYTE	m_bReserved3;

	public:
		CMyPacketEndRound()
			: CPacket( sizeof( CMyPacketEndRound ), CMyCommon::MyPacketCommand::EndRound ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketEndRound ) == nPacketLength;
			}
	};// class CMyPacketEndRound

	class CMyPacketReady : public CPacket
	{
	public:
		TCHAR	m_szID[MAX_ID_LEN+1];
		BOOL	m_bReady;

	public:
		CMyPacketReady()
			: CPacket( sizeof( CMyPacketReady ), CMyCommon::MyPacketCommand::Ready ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketReady ) == nPacketLength;
			}
	};// class CMyPacketReady

	class CMyPacketKill : public CPacket
	{
	public:
		TCHAR	m_szKillerID[MAX_ID_LEN+1];
		TCHAR	m_szVictimID[MAX_ID_LEN+1];
		UINT	m_nHitNode;
		
	public:
		CMyPacketKill()
			: CPacket( sizeof( CMyPacketKill ), CMyCommon::MyPacketCommand::Kill ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketKill ) == nPacketLength;
			}
	};// class CMyPacketKill

	class CMyPacketEndGameRequest : public CPacket
	{
	public:
		CMyPacketEndGameRequest()
			: CPacket( sizeof( CMyPacketEndGameRequest ), CMyCommon::MyPacketCommand::EndGameRequest ) {}

			BOOL IsCorrectLength( INT nPacketLength )
			{
				return sizeof( CMyPacketEndGameRequest ) == nPacketLength;
			}
	};// class CMyPacketEndGameRequest

	class CMyPacketRoomStateChange : public CPacket
	{
	public:
		int	m_nRoomIndex;
		int	m_nRoomState;

	public:
		CMyPacketRoomStateChange()
			:	CPacket( sizeof( CMyPacketRoomStateChange ), CMyCommon::MyPacketCommand::RoomStateChange ) {}
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketRoomStateChange ) == nPacketLength;
			}
	};// class CMyPacketRoomStateChange

	class CMyPacketInstrution : public CPacket
	{
	public:
		CMyPacketInstrution()
			:	CPacket( sizeof( CMyPacketInstrution ), CMyCommon::MyPacketCommand::Instrution ) {}
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketInstrution ) == nPacketLength;
			}
	};// class CMyPacketInstrution

	class CMyPacketInstrutionReply : public CPacket
	{
	public:
		int	m_nResult;
	public:
		CMyPacketInstrutionReply()
			:	CPacket( sizeof( CMyPacketInstrutionReply ), CMyCommon::MyPacketCommand::InstrutionReply ) {}
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketInstrutionReply ) == nPacketLength;
			}
	};// class CMyPacketInstrutionReply

	class CMyPacketExitGameRequest : public CPacket
	{
	public:
		CMyPacketExitGameRequest()
			:	CPacket( sizeof( CMyPacketExitGameRequest ), CMyCommon::MyPacketCommand::ExitGameRequest ) {}
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketExitGameRequest ) == nPacketLength;
			}
	};// class CMyPacketExitGameRequest

	class CMyPacketExitGameUser : public CPacket
	{
	public:
		TCHAR	m_szExitGameUser[MAX_ID_LEN];

	public:
		CMyPacketExitGameUser()
			:	CPacket( sizeof( CMyPacketExitGameUser ), CMyCommon::MyPacketCommand::ExitGameUser ) {}
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketExitGameUser ) == nPacketLength;
			}
	};// class CMyPacketExitGameUser

	class CMyPacketLoadComplete : public CPacket
	{
	public:
		CMyPacketLoadComplete()
			:	CPacket( sizeof( CMyPacketLoadComplete ), CMyCommon::MyPacketCommand::LoadComplete ) {}
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketLoadComplete ) == nPacketLength;
			}
	};// class CMyPacketLoadComplete

	class CMyPacketStartNextRound : public CPacket
	{
	public:
		CMyPacketStartNextRound()
			:	CPacket( sizeof( CMyPacketStartNextRound ), CMyCommon::MyPacketCommand::StartNextRound ) {}
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketStartNextRound ) == nPacketLength;
			}
	};// class CMyPacketStartNextRound

	class CMyPacketGameChat : public CPacket
	{
	public:
		short	m_nMessageLength;
		char	m_aMessage[ CCasualCommon::CasualStringLength::ChatMsg + 1 ];

	public:
		CMyPacketGameChat()
			:	CPacket( sizeof( CMyPacketGameChat ), CMyCommon::MyPacketCommand::GameChat ) {}		
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketGameChat ) == nPacketLength;
			}

	public:
		void	SetProperty		( LPCTSTR pMessage, short nMessageLength )
		{
			m_nMessageLength = nMessageLength	;

			if( m_nMessageLength > CCasualCommon::CasualStringLength::ChatMsg )
			{
				m_nMessageLength = CCasualCommon::CasualStringLength::ChatMsg;
			}

			// 가변형 길이일 경우에는 Length를 계산하여 넣어준다.
			SetLength( sizeof( CMyPacketGameChat ) - CCasualCommon::CasualStringLength::ChatMsg + m_nMessageLength );

			::memcpy( m_aMessage, pMessage, m_nMessageLength );
			m_aMessage[ m_nMessageLength ] = NULL;
		}
		short	GetMessageLength()	{ return m_nMessageLength;	}
		LPCTSTR	GetMessage()		{ return m_aMessage;		}
		INT		GetRealLength()		{ return( sizeof( CMyPacketGameChat ) - CCasualCommon::CasualStringLength::ChatMsg + m_nMessageLength ); }
	};	//class CMyPacketGameChat


	class CMyPacketTeamChat : public CPacket
	{
	public:
		short	m_nMessageLength;
		char	m_aMessage[ CCasualCommon::CasualStringLength::ChatMsg + 1 ];

	public:
		CMyPacketTeamChat()
			:	CPacket( sizeof( CMyPacketTeamChat ), CMyCommon::MyPacketCommand::TeamChat ) {}		
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketTeamChat ) == nPacketLength;
			}

	public:
		void	SetProperty		( LPCTSTR pMessage, short nMessageLength )
		{
			m_nMessageLength = nMessageLength	;

			if( m_nMessageLength > CCasualCommon::CasualStringLength::ChatMsg )
			{
				m_nMessageLength = CCasualCommon::CasualStringLength::ChatMsg;
			}

			// 가변형 길이일 경우에는 Length를 계산하여 넣어준다.
			SetLength( sizeof( CMyPacketTeamChat ) - CCasualCommon::CasualStringLength::ChatMsg + m_nMessageLength );

			::memcpy( m_aMessage, pMessage, m_nMessageLength );
			m_aMessage[ m_nMessageLength ] = NULL;
		}
		short	GetMessageLength()	{ return m_nMessageLength;	}
		LPCTSTR	GetMessage()		{ return m_aMessage;		}
		INT		GetRealLength()		{ return( sizeof( CMyPacketTeamChat ) - CCasualCommon::CasualStringLength::ChatMsg + m_nMessageLength ); }
	};	//class CMyPacketTeamChat

	class CMyPacketWhisper : public CPacket
	{
	public:
		char	m_szSenderID[MAX_ID_LEN];
		char	m_szSenderNick[MAX_NICK_LEN];
		char	m_szReceiverID[MAX_ID_LEN];
		char	m_szReceiverNick[MAX_NICK_LEN];
		short	m_nMessageLength;
		char	m_aMessage[ CCasualCommon::CasualStringLength::ChatMsg + 1 ];

	public:
		CMyPacketWhisper()
			:	CPacket( sizeof( CMyPacketWhisper ), CMyCommon::MyPacketCommand::Whisper ) {}		
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketWhisper ) == nPacketLength;
			}

	public:
		void	SetProperty		( LPCTSTR pMessage, short nMessageLength )
		{
			m_nMessageLength = nMessageLength	;

			if( m_nMessageLength > CCasualCommon::CasualStringLength::ChatMsg )
			{
				m_nMessageLength = CCasualCommon::CasualStringLength::ChatMsg;
			}

			// 가변형 길이일 경우에는 Length를 계산하여 넣어준다.
			SetLength( sizeof( CMyPacketWhisper ) - CCasualCommon::CasualStringLength::ChatMsg + m_nMessageLength );

			::memcpy( m_aMessage, pMessage, m_nMessageLength );
			m_aMessage[ m_nMessageLength ] = NULL;
		}
		short	GetMessageLength()	{ return m_nMessageLength;	}
		LPCTSTR	GetMessage()		{ return m_aMessage;		}
		INT		GetRealLength()		{ return( sizeof( CMyPacketWhisper ) - CCasualCommon::CasualStringLength::ChatMsg + m_nMessageLength ); }
	};	//class CMyPacketWhisper

	class CMyPacketTimeBombState : public CPacket
	{
	public:
		WORD	m_nState;
	public:
		CMyPacketTimeBombState()
			:	CPacket( sizeof( CMyPacketTimeBombState ), CMyCommon::MyPacketCommand::TimeBombState ) {}
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketTimeBombState ) == nPacketLength;
			}
	};// class CMyPacketTimeBombState

	class CMyPacketRoundScore : public CPacket
	{
	public:
		WORD	m_nWinRound_A;
		WORD	m_nWinRound_B;
	public:
		CMyPacketRoundScore()
			:	CPacket( sizeof( CMyPacketRoundScore ), CMyCommon::MyPacketCommand::RoundScore ) {}
			BOOL	IsCorrectLength	( INT nPacketLength )
			{
				return sizeof( CMyPacketRoundScore ) == nPacketLength;
			}
	};// class CMyPacketRoundScore
}

#endif	//#ifndef __MURSUM_MYPACKET_H__