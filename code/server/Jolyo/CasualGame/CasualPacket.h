#pragma once

#include ".\CasualCommon.h"
#include "..\NJean\Application\Packet.h"

// 패킷 클래스는 절대로 virtual 함수를 사용해서는 안된다!!
//---------------------------------------------------------------------------------------------------------------------------------
#define LIBCOMMAND								CCasualCommon::CasualPacketCommand
#define LIB_PACKET_LENGTH						CCasualCommon::CasualStringLength
#define SET_PACKET_COMMAND(TPacket,command)		public:TPacket():CPacket(sizeof(TPacket),command){}
#define GET_REAL_LENGTH(packet)					INT GetRealLength() { return sizeof(packet); }
//---------------------------------------------------------------------------------------------------------------------------------
namespace CasualGame { namespace CasualPacket 
{
	class CCasualPacketSecLoginRequest : public CPacket						// 암호화된 로그인을 요청한다.
	{
		SET_PACKET_COMMAND(CCasualPacketSecLoginRequest,LIBCOMMAND::SecLoginRequest)
	private:
		char	m_strIP			[ LIB_PACKET_LENGTH::IpAddr		+ 1 ];		// p2p IP Address
		short	m_nPort;													// p2p Port
		char	m_caSessionKey	[ LIB_PACKET_LENGTH::SessionKey	+ 1 ];		// 세션키
		short	m_nSecSize;													// 암호화 된 데이타의 길이
		char	m_caSecData		[ LIB_PACKET_LENGTH::SecLogin	+ 1 ];		// 암호화 된 데이타
	public:
		void SetProperty( const char* caSecData, short nDataLength, const char* caSessionKey, LPCTSTR strIP, int nIPLen, short nPort )
		{
			::memcpy( m_caSessionKey, caSessionKey, LIB_PACKET_LENGTH::SessionKey );
			m_caSessionKey[ LIB_PACKET_LENGTH::SessionKey ] = NULL;

			m_nPort			= nPort;
			if( nIPLen > LIB_PACKET_LENGTH::IpAddr )
				nIPLen = LIB_PACKET_LENGTH::IpAddr;
			::memcpy( m_strIP, strIP, nIPLen );
			m_strIP[nIPLen] = NULL;

			m_nSecSize = nDataLength;
			::memcpy( m_caSecData, caSecData, nDataLength );

			SetLength( sizeof( CCasualPacketSecLoginRequest ) - LIB_PACKET_LENGTH::SecLogin + nDataLength );
		}
		const char*	GetSecData()	{ return m_caSecData;	}
		const char* GetSessionKey()	{ return m_caSessionKey;}
		short		GetSecSize()	{ return m_nSecSize;	}
		LPCTSTR		GetIpAddr()		{ return m_strIP;		}
		SHORT		GetPort()		{ return m_nPort;		}
		INT			GetRealLength() { return sizeof( CCasualPacketSecLoginRequest ) - LIB_PACKET_LENGTH::SecLogin + m_nSecSize; }
	};

	class CCasualPacketLoginRequest : public CPacket						// 로그인을 요청한다.
	{
		SET_PACKET_COMMAND(CCasualPacketLoginRequest,LIBCOMMAND::LoginRequest)
	private:
		long	m_lSessionKey;												// SessionKey
		char	m_strUserNo		[ LIB_PACKET_LENGTH::UserNo		+ 1 ];		// UserNumber
		char	m_strID			[ LIB_PACKET_LENGTH::UserID		+ 1 ];		// ID
		char	m_strNick		[ LIB_PACKET_LENGTH::UserNick	+ 1 ];		// NickName
		char	m_strIP			[ LIB_PACKET_LENGTH::IpAddr		+ 1 ];		// p2p IP Address
		short	m_nPort;													// p2p Port
	public:
		void SetProperty( long lSessionKey, LPCTSTR strUserNo, int nUserNoLen, LPCTSTR strID, int nIdLen, LPCTSTR strNick, int nNickLen, LPCTSTR strIP, int nIPLen, short nPort )
		{
			m_lSessionKey	= lSessionKey;
			m_nPort			= nPort;

			if( nUserNoLen > LIB_PACKET_LENGTH::UserNo )
				nUserNoLen = LIB_PACKET_LENGTH::UserNo;
			::memcpy( m_strUserNo, strUserNo, nUserNoLen );
			m_strUserNo[nUserNoLen] = NULL;

			if( nIdLen > LIB_PACKET_LENGTH::UserID )
				nIdLen = LIB_PACKET_LENGTH::UserID;
			::memcpy( m_strID, strID, nIdLen );
			m_strID[nIdLen] = NULL;

			if( nNickLen > LIB_PACKET_LENGTH::UserNick )
				nNickLen = LIB_PACKET_LENGTH::UserNick;
			::memcpy( m_strNick, strNick, nNickLen );
			m_strNick[nNickLen] = NULL;

			if( nIPLen > LIB_PACKET_LENGTH::IpAddr )
				nIPLen = LIB_PACKET_LENGTH::IpAddr;
			::memcpy( m_strIP, strIP, nIPLen );
			m_strIP[nIPLen] = NULL;
		}

		long	GetSessionKey()	{ return m_lSessionKey;		}
		LPCTSTR	GetUserNo()		{ return m_strUserNo;		}
		LPCTSTR	GetID()			{ return m_strID;			}
		LPCTSTR GetNick()		{ return m_strNick;			}
		LPCTSTR	GetIpAddr()		{ return m_strIP;			}
		SHORT	GetPort()		{ return m_nPort;			}

		GET_REAL_LENGTH(CCasualPacketLoginRequest)
	};

	class CCasualPacketLoginReply : public CPacket								// 로그인 결과를 알려준다.
	{
		SET_PACKET_COMMAND(CCasualPacketLoginReply,LIBCOMMAND::LoginReply)
		enum	LoginResult
		{
			Success	= 0,	// 인증 성공
			Joined	= 1,	// 인증 성공 & 현재 접속중
			Fail	= 2,	// 인증 실패
		};
	private:
		short	m_nResult;		// 처리결과
	public:
		void		SetProperty	( LoginResult nResult ) { m_nResult = nResult; }
		short		GetResult	()						{ return m_nResult;		}
		GET_REAL_LENGTH(CCasualPacketLoginReply)
	};

	class CCasualPacketCompulsionExitRequest : public CPacket					// 강제 퇴장 요청 패킷
	{
		SET_PACKET_COMMAND(CCasualPacketCompulsionExitRequest,LIBCOMMAND::CompulseExitRequest)
	private:
		char	m_strID[ LIB_PACKET_LENGTH::UserID	+ 1 ];	// ID
	public:
		void		SetProperty			( LPCTSTR strID, int nIDLen )
		{
			if( nIDLen > LIB_PACKET_LENGTH::UserID )
				nIDLen = LIB_PACKET_LENGTH::UserID;
			::memcpy( m_strID, strID, nIDLen );
			m_strID[nIDLen] = NULL;
		}
		LPCTSTR		GetUserID()	{ return m_strID;	}
		GET_REAL_LENGTH(CCasualPacketCompulsionExitRequest)
	};

	class CCasualPacketCompulsionExitReply : public CPacket						// 강제 퇴장 요청 결과 패킷
	{
		SET_PACKET_COMMAND(CCasualPacketCompulsionExitReply,LIBCOMMAND::CompulseExitReply)
		enum	Result
		{
			Success			= 0,
			FailNotExist	= 1,
			FailETC			= 2,
		};
	private:
		SHORT	m_Result;
		CHAR	m_strID[ LIB_PACKET_LENGTH::UserID	+ 1 ];	// ID
	public:
		void		SetProperty			( SHORT result, LPCTSTR strID, int nIDLen )
		{
			m_Result = static_cast<SHORT>( result );
			if( nIDLen > LIB_PACKET_LENGTH::UserID )
				nIDLen = LIB_PACKET_LENGTH::UserID;
			::memcpy( m_strID, strID, nIDLen );
			m_strID[nIDLen] = NULL;
		}
		SHORT		GetResult()		{ return m_Result;	}
		LPCTSTR		GetUserID()	{ return m_strID;	}
		GET_REAL_LENGTH(CCasualPacketCompulsionExitReply)
	};

	class CCasualPacketCompulsionExited : public CPacket					// 강제 퇴장 당함 패킷
	{
		SET_PACKET_COMMAND(CCasualPacketCompulsionExited,LIBCOMMAND::CompulseExited)
		enum	Type
		{
			Mine		= 0,	// 자신의 강퇴 : 이 경우는 패킷으로 전달하지는 않는다.
			Normal		= 1,	// 보통의 강퇴
			CloseRoom	= 2,	// 방이 닫혀서 생기는 강퇴
		};
	private:
		SHORT	m_Type;
	public:
		VOID		SetProperty	( SHORT type )	{ m_Type	= type; }
		SHORT		GetType		()				{ return m_Type;	}
		GET_REAL_LENGTH(CCasualPacketCompulsionExited)
	};

	class CCasualPacketJoinChannelRequest : public CPacket					// 채널에 입장을 한다.
	{
		SET_PACKET_COMMAND(CCasualPacketJoinChannelRequest,LIBCOMMAND::JoinChannelRequest)
	private:
		short	m_nChannelIndex;	// 입장하고자 하는 채널 인덱스
	public:
		void		SetProperty			( short nChannelIndex ) { m_nChannelIndex = nChannelIndex; }
		short		GetChannelIndex()	{ return m_nChannelIndex;	}
		GET_REAL_LENGTH(CCasualPacketJoinChannelRequest)
	};

	class CCasualPacketJoinChannelReply : public CPacket					// 채널 입장 결과
	{
		SET_PACKET_COMMAND(CCasualPacketJoinChannelReply,LIBCOMMAND::JoinChannelReply)
		enum	Result
		{
			Success				= 0,	// 성공
			FailFull			= 1,	// 채널에 꽉차서 들어갈 수 없음
			FailExist			= 2,	// 이미 다른 채널에 입장 중
			FailOutOfChannel	= 3,	// 채널의 범위가 넘음
			FailETC				= 4,	// 기타 다른 이유
		};
	private:
		short	m_nResult;		// 입장 결과
		short	m_nUsers;		// 채널 대기실 사용자 수
		short	m_nRooms;		// open된 방의 수
	public:
		void		SetProperty			( SHORT result, short nUsers, short nRooms )
		{
			m_nResult	= result;
			m_nUsers	= nUsers;
			m_nRooms	= nRooms;
		}
		void		SetProperty			( SHORT result )
		{
			m_nResult	= result;
		}
		Result		GetResult()			{ return (Result) m_nResult;	}
		short		GetUsers()			{ return m_nUsers;				}
		short		GetRooms()			{ return m_nRooms;				}
		GET_REAL_LENGTH(CCasualPacketJoinChannelReply)
	};

	class CCasualPacketLeaveChannelRequest : public CPacket					// 채널에서 퇴장을 한다.
	{
		SET_PACKET_COMMAND(CCasualPacketLeaveChannelRequest,LIBCOMMAND::LeaveChannelRequest)
		GET_REAL_LENGTH(CCasualPacketLeaveChannelRequest)
	};

	class CCasualPacketLeaveChannelReply : public CPacket					// 채널에서 퇴장 결과
	{
		SET_PACKET_COMMAND(CCasualPacketLeaveChannelReply,LIBCOMMAND::LeaveChannelReply)
		GET_REAL_LENGTH(CCasualPacketLeaveChannelReply)
	};

	class CCasualPacketOpenRoomRequest : public CPacket						// 방 개설을 요청한다.
	{
		SET_PACKET_COMMAND(CCasualPacketOpenRoomRequest,LIBCOMMAND::OpenRoomRequest)
	private:
		short	m_nMaxUsers;												// Room에 참여할 수 있는 사용자의 수
		BOOL	m_bPass;													// 패스워드 유무
		char	m_strRoomName	[ LIB_PACKET_LENGTH::RoomName + 1 ];		// Room의 이름
		char	m_caOption		[ LIB_PACKET_LENGTH::RoomOption + 1 ];		// 옵션
		char	m_strPass		[ LIB_PACKET_LENGTH::RoomPass + 1 ];		// 패스워드
	public:
		void		SetProperty		( short nMaxUsers, LPCTSTR strRoomName, int nNameSize, LPCTSTR strPass, int nPassLength, const char* caOption )
		{
			m_nMaxUsers = nMaxUsers;

			if( nNameSize > LIB_PACKET_LENGTH::RoomName )
				nNameSize = LIB_PACKET_LENGTH::RoomName;
			::memcpy( m_strRoomName, strRoomName, nNameSize );
			m_strRoomName[nNameSize] = NULL;

			::memcpy( m_caOption, caOption, LIB_PACKET_LENGTH::RoomOption );

			if( nPassLength == 0 )
			{
				m_bPass = FALSE;
				m_strPass[0] = NULL;
			}
			else
			{
				m_bPass = TRUE;
				if( nPassLength > LIB_PACKET_LENGTH::RoomPass )
					nPassLength = LIB_PACKET_LENGTH::RoomPass;
				::memcpy( m_strPass, strPass, nPassLength );
				m_strPass[nPassLength] = NULL;
			}
		}
		short		GetMaxUsers()			{ return m_nMaxUsers;	}
		LPCTSTR		GetRoomName()			{ return m_strRoomName; }
		BOOL		IsPassword()			{ return m_bPass;		}
		LPCTSTR		GetPassword()			{ return m_strPass;		}
		CHAR		GetOption( INT nIndex ) { return m_caOption[nIndex]; }
		GET_REAL_LENGTH(CCasualPacketOpenRoomRequest)
	};

	
	class CCasualPacketOpenRoomReply : public CPacket						// 방개설 결과를 통보한다.
	{
		SET_PACKET_COMMAND(CCasualPacketOpenRoomReply,LIBCOMMAND::OpenRoomReply)
		enum	Result
		{
			Success				= 0,
			FailFull			= 1,
			FailOverUsers		= 2,
			FailExist			= 3,
			FailOutOfChannel	= 4,
		};
	private:
		short	m_nResult;		// 처리 결과
		short	m_nRoomIndex;	// Open한 Room의 Index
	public:
		void	SetProperty	( SHORT nResult, short nRoomIndex ) { m_nResult = nResult; m_nRoomIndex	= nRoomIndex; }
		short	GetResult	()									{ return m_nResult;		}
		short	GetRoomIndex()									{ return m_nRoomIndex;	}
		GET_REAL_LENGTH(CCasualPacketOpenRoomReply)
	};

	class CCasualPacketJoinRoomRequest : public CPacket						// 방에 참여한다.
	{
		SET_PACKET_COMMAND(CCasualPacketJoinRoomRequest,LIBCOMMAND::JoinRoomRequest)
	private:
		short	m_nRoomIndex;												// 참여할 Room의 Index
		char	m_strPass		[ LIB_PACKET_LENGTH::RoomPass + 1 ];		// 패스워드
	public:
		void	SetProperty		( short nRoomIndex, LPCTSTR strPass, int nPassLength )
		{
			m_nRoomIndex	= nRoomIndex;

			if( nPassLength > LIB_PACKET_LENGTH::RoomPass )
				nPassLength = LIB_PACKET_LENGTH::RoomPass;
			::memcpy( m_strPass, strPass, nPassLength );
			m_strPass[nPassLength] = NULL;
		}
		short	GetRoomIndex()	{ return m_nRoomIndex;	}
		char*	GetPass()		{ return m_strPass;		}
		GET_REAL_LENGTH(CCasualPacketJoinRoomRequest)
	};

	class CCasualPacketJoinRoomReply : public CPacket						// 방 참여 결과를 전달한다.
	{
		SET_PACKET_COMMAND(CCasualPacketJoinRoomReply,LIBCOMMAND::JoinRoomReply)
		enum	Result
		{
			Success				= 0,
			FailFull			= 1,
			FailClose			= 2,
			FailExist			= 3,
			FailOutOfRoom		= 4,
			FailOutOfChannel	= 5,
			FailWrongPass		= 6,
			FailETC				= 7,
		};
	private:
		short	m_nResult;		// 처리 결과
		short	m_nRoomIndex;	// 참여한 Room의 Index

	public:
		void	SetProperty		( SHORT nResult, short nRoomIndex )
		{
			m_nResult		= nResult;
			m_nRoomIndex	= nRoomIndex;		
		}
		short	GetResult()		{ return m_nResult;			}
		short	GetRoomIndex()	{ return m_nRoomIndex;		}		
		GET_REAL_LENGTH(CCasualPacketJoinRoomReply)
	};

	class CCasualPacketLeaveRoomRequest : public CPacket			// 방에서 나간다.
	{
		SET_PACKET_COMMAND(CCasualPacketLeaveRoomRequest,LIBCOMMAND::LeaveRoomRequest)
		GET_REAL_LENGTH(CCasualPacketLeaveRoomRequest)
	};

	class CCasualPacketLeaveRoomReply : public CPacket				// 방에서 나가기 요청 결과
	{
		SET_PACKET_COMMAND(CCasualPacketLeaveRoomReply,LIBCOMMAND::LeaveRoomReply)
		enum	Result
		{
			Success		= 0,
			Fail		= 1,
		};
	private:
		SHORT	m_Result;
	public:
		VOID	SetProperty	( SHORT result )	{ m_Result = result; }
		SHORT	GetResult	()					{ return m_Result;	}
		GET_REAL_LENGTH(CCasualPacketLeaveRoomReply)
	};

	class CCasualPacketUserInfo : public CPacket
	{
		SET_PACKET_COMMAND(CCasualPacketUserInfo,LIBCOMMAND::UserInfo)
	private:
		UINT	m_nGrade;
		char	m_strUserID		[ LIB_PACKET_LENGTH::UserID + 1 ];
		char	m_strUserNick	[ LIB_PACKET_LENGTH::UserNick + 1 ];
	public:
		void	SetProperty		( UINT nGrade, LPCTSTR strUserID, INT nIDLen, LPCTSTR strUserNick, INT nNickLen )
		{
			m_nGrade		= nGrade;
			
			if( nIDLen > LIB_PACKET_LENGTH::UserID )
				nIDLen = LIB_PACKET_LENGTH::UserID;
			::memcpy( m_strUserID, strUserID, nIDLen );
			m_strUserID[nIDLen] = NULL;

			if( nNickLen > LIB_PACKET_LENGTH::UserNick )
				nNickLen = LIB_PACKET_LENGTH::UserNick;
			::memcpy( m_strUserNick, strUserNick, nNickLen );
			m_strUserNick[nNickLen] = NULL;
		}
		UINT	GetGrade()		{ return m_nGrade;		}
		LPCTSTR GetUserID()		{ return m_strUserID;	}
		LPCTSTR	GetUserNick()	{ return m_strUserNick;	}
		GET_REAL_LENGTH(CCasualPacketUserInfo)
	};

	class CCasualPacketRoomInfo : public CPacket
	{
		SET_PACKET_COMMAND(CCasualPacketRoomInfo,LIBCOMMAND::RoomInfo)
	private:
		short	m_nRoomIndex;										// Room의 번호
		short	m_nMaxUsers;										// 방에 최대로 들어갈 수 있는 사용자의 수
		short	m_nUsers;											// 현재 입장된 사용자의 수
		BOOL	m_bPassword;										// 비밀번호 유무
		char	m_caOption[LIB_PACKET_LENGTH::RoomOption];			// 옵션
		char	m_strRoomName[ LIB_PACKET_LENGTH::RoomName + 1 ];	// 방 이름
		DWORD	m_dwRTT;
	public:
		void	SetProperty		( short nRoomIndex,
								  short nMaxUsers,
								  short nUsers,
								  const char* strRoomName, 
								  int nNameLen,
								  BOOL bPassword,
								  const char* caOption,
								  DWORD dwRTT = 0 )
		{
			m_nRoomIndex	= nRoomIndex	;
			m_nMaxUsers		= nMaxUsers		;
			m_nUsers		= nUsers		;
			m_bPassword		= bPassword		;
			::memcpy( m_caOption, caOption, LIB_PACKET_LENGTH::RoomOption );
			if( nNameLen > LIB_PACKET_LENGTH::RoomName )
				nNameLen = LIB_PACKET_LENGTH::RoomName;
			::memcpy( m_strRoomName, strRoomName, nNameLen );
			m_strRoomName[nNameLen] = NULL;			
			m_dwRTT					= dwRTT;
		}
		short	GetRoomIndex()			{ return m_nRoomIndex;									}
		short	GetMaxUsers()			{ return m_nMaxUsers;									}
		short	GetUsers()				{ return m_nUsers;										}
		CHAR	GetOption( INT nIndex )	{ return m_caOption[nIndex];							}
		LPCTSTR	GetRoomName()			{ return m_strRoomName;									}
		BOOL	IsPassword()			{ return m_bPassword;									}
		DWORD	GetRTT()				{ return m_dwRTT;										}
		GET_REAL_LENGTH(CCasualPacketRoomInfo)
	};

	class CCasualPacketRoomInfoChange : public CPacket
	{
		SET_PACKET_COMMAND(CCasualPacketRoomInfoChange,LIBCOMMAND::RoomInfoChange)
	private:
		short	m_nRoomIndex;		// Room의 번호
		short	m_nMaxUsers;		// 방에 최대로 들어갈 수 있는 사용자의 수
		short	m_nUsers;			// 현재 입장된 사용자의 수
		BOOL	m_bPassword;		// 비밀번호 유무
		char	m_caOption[LIB_PACKET_LENGTH::RoomOption];
		char	m_strRoomName[ LIB_PACKET_LENGTH::RoomName + 1 ];	// 방 이름
	public:
		void	SetProperty		( short nRoomIndex, short nMaxUsers, short nUsers, BOOL bPassword, const char* caOption )
		{
			m_nRoomIndex	= nRoomIndex	;
			m_nMaxUsers		= nMaxUsers		;
			m_nUsers		= nUsers		;
			m_bPassword		= bPassword		;

			::memcpy( m_caOption, caOption, LIB_PACKET_LENGTH::RoomOption );
			::memset( m_strRoomName, 0, sizeof(m_strRoomName) );
		}
		void	SetProperty		( short nRoomIndex, short nMaxUsers, short nUsers, const char* strRoomName, int nNameLen, BOOL bPassword, const char* caOption )
		{
			m_nRoomIndex	= nRoomIndex	;
			m_nMaxUsers		= nMaxUsers		;
			m_nUsers		= nUsers		;
			m_bPassword		= bPassword		;

			::memcpy( m_caOption, caOption, LIB_PACKET_LENGTH::RoomOption );

			if( nNameLen > LIB_PACKET_LENGTH::RoomName )
				nNameLen = LIB_PACKET_LENGTH::RoomName;
			::memcpy( m_strRoomName, strRoomName, nNameLen );
			m_strRoomName[nNameLen] = NULL;
		}
		short	GetRoomIndex()	{ return m_nRoomIndex;	}
		short	GetMaxUsers()	{ return m_nMaxUsers;	}
		short	GetUsers()		{ return m_nUsers;		}
		BOOL	IsPassword()	{ return m_bPassword;	}
		CHAR	GetOption( INT nIndex )	{ return m_caOption[nIndex];							}
		LPCTSTR	GetRoomName()			{ return m_strRoomName;									}
		GET_REAL_LENGTH(CCasualPacketRoomInfoChange)
	};

	class CCasualPacketRemoveRoom : public CPacket
	{
		SET_PACKET_COMMAND(CCasualPacketRemoveRoom,LIBCOMMAND::RemoveRoom)
	private:
		short	m_nRoomIndex;		// Room의 번호
	public:
		void	SetProperty	( short nRoomIndex )	{ m_nRoomIndex	= nRoomIndex; }
		short	GetRoomIndex()						{ return m_nRoomIndex;	}
		GET_REAL_LENGTH(CCasualPacketRemoveRoom)
	};

	class CCasualPacketRemoveUser : public CPacket
	{
		SET_PACKET_COMMAND(CCasualPacketRemoveUser,LIBCOMMAND::RemoveUser)
	private:
		char	m_strUserID	[ LIB_PACKET_LENGTH::UserID+1];
	public:
		void	SetProperty	( LPCTSTR strUserID, INT nIDLen )
		{
			if( nIDLen > LIB_PACKET_LENGTH::UserID )
				nIDLen = LIB_PACKET_LENGTH::UserID;
			::memcpy( m_strUserID, strUserID, nIDLen );
			m_strUserID[nIDLen] = NULL;
		}
		LPCTSTR	GetUserID()	{ return m_strUserID;	}
		GET_REAL_LENGTH(CCasualPacketRemoveUser)
	};

	class CCasualPacketRemoveRoomUser : public CPacket
	{
		SET_PACKET_COMMAND(CCasualPacketRemoveRoomUser,LIBCOMMAND::RemoveRoomUser)
	private:
		char	m_strUserID	[ LIB_PACKET_LENGTH::UserID	+ 1 ];
	public:
		void	SetProperty	( LPCTSTR strUserID, INT nIDLen )
		{
			if( nIDLen > LIB_PACKET_LENGTH::UserID )
				nIDLen = LIB_PACKET_LENGTH::UserID;
			::memcpy( m_strUserID, strUserID, nIDLen );
			m_strUserID[nIDLen] = NULL;
		}
		LPCTSTR	GetUserID()	{ return m_strUserID;	}
		GET_REAL_LENGTH(CCasualPacketRemoveRoomUser)
	};

	class CCasualPacketChat : public CPacket					// 채팅 메시지를 전달한다.
	{
		SET_PACKET_COMMAND(CCasualPacketChat,LIBCOMMAND::Chat)
	private:
		short	m_nMessageLength;										// 채팅 메시지의 실제 길이
		char	m_aMessage[ LIB_PACKET_LENGTH::ChatMsg + 1 ];			// 채팅 메시지
	public:
		void	SetProperty		( LPCTSTR pMessage, short nMessageLength )
		{
			m_nMessageLength = nMessageLength	;

			if( m_nMessageLength > LIB_PACKET_LENGTH::ChatMsg )
			{
				m_nMessageLength = LIB_PACKET_LENGTH::ChatMsg;
			}

			// 가변형 길이일 경우에는 Length를 계산하여 넣어준다.
			SetLength( sizeof( CCasualPacketChat ) - LIB_PACKET_LENGTH::ChatMsg + m_nMessageLength );

			::memcpy( m_aMessage, pMessage, m_nMessageLength );
			m_aMessage[ m_nMessageLength ] = NULL;
		}
		short	GetMessageLength()	{ return m_nMessageLength;	}
		LPCTSTR	GetMessage()		{ return m_aMessage;		}
		INT		GetRealLength()		{ return( sizeof( CCasualPacketChat ) - LIB_PACKET_LENGTH::ChatMsg + m_nMessageLength ); }
	};

	class CCasualPacketAutoJoinRoomRequest : public CPacket		// 바로들어가기 요청
	{
		SET_PACKET_COMMAND(CCasualPacketAutoJoinRoomRequest,LIBCOMMAND::AutoJoinRoomRequest)
	private:
		short	m_nLastRoomIndex;	// 가장 마지막에 들어간 방의 번호 : 이 번호로는 되도록 들어가지 않도록 한다.
	public:
		void	SetProperty		( short nLastRoomIndex = -1 )	{ m_nLastRoomIndex = nLastRoomIndex; }
		short	GetLastRoomIndex()								{ return m_nLastRoomIndex;	}
		GET_REAL_LENGTH(CCasualPacketAutoJoinRoomRequest)
	};

	// 바로들어가기 결과
	class CCasualPacketAutoJoinRoomReply : public CPacket
	{
		SET_PACKET_COMMAND(CCasualPacketAutoJoinRoomReply,LIBCOMMAND::AutoJoinRoomReply)
		enum	Result
		{
			Success				= 0,			// 입장 성공
			FailFull			= 1,			// 모든 방이 꽉 차 있는 경우
			FailExist			= 2,			// 요청한 사용자가 이미 다른방에 입장되어 있는 경우
			FailOutOfChannel	= 3,
			FailNone			= 4,
		};
	private:
		short	m_nResult;
		short	m_nRoomIndex;		// 입장한 방의 번호
	public:
		void	SetProperty			( SHORT nResult, SHORT nRoomIndex = 0 )
		{
			m_nResult		= nResult		;
			m_nRoomIndex	= nRoomIndex	;		
		}
		short	GetResult()				{ return m_nResult;			}
		short	GetRoomIndex()			{ return m_nRoomIndex;		}		
		GET_REAL_LENGTH(CCasualPacketAutoJoinRoomReply)
	};

	class CCasualPacketRoomCaptineChange : public CPacket				// 방장 변경 통보
	{
		SET_PACKET_COMMAND(CCasualPacketRoomCaptineChange,LIBCOMMAND::RoomCaptineChange)
	private:
		char	m_strCaptineID	[ LIB_PACKET_LENGTH::UserID	+ 1 ];
	public:
		void	SetProperty			( LPCTSTR strCaptineID, INT nIDLen )
		{
			if( nIDLen > LIB_PACKET_LENGTH::UserID )
				nIDLen = LIB_PACKET_LENGTH::UserID;
			::memcpy( m_strCaptineID, strCaptineID, nIDLen );
			m_strCaptineID[nIDLen] = NULL;
		}
		LPCTSTR	GetCaptineID()		{ return m_strCaptineID;	}
		GET_REAL_LENGTH(CCasualPacketRoomCaptineChange)
	};

	class CCasualPacketMemo : public CPacket						// 메모
	{
		SET_PACKET_COMMAND(CCasualPacketMemo,LIBCOMMAND::Memo)
		enum	FindType
		{
			ID		= 0,
			Nick	= 1,
		};
	private:
		short	m_nFindType;
		char	m_strSenderID	[ LIB_PACKET_LENGTH::UserID	+ 1 ];	// ID
		char	m_strSenderNick	[ LIB_PACKET_LENGTH::UserNick	+ 1 ];	// Nick
		char	m_strReceiver	[ LIB_PACKET_LENGTH::FindUser + 1 ];	// Receiver
		char	m_strMemo		[ LIB_PACKET_LENGTH::MemoMsg + 1 ];	// Memo
	public:
		void	SetProperty		( FindType nFindType, LPCTSTR strSenderID, int nIdLen, LPCTSTR strSenderNick, int nNickLen, LPCTSTR strFind, int nFindLen, LPCTSTR strMemo, int nMemoLen )
		{
			m_nFindType	= nFindType;

			if( nIdLen > LIB_PACKET_LENGTH::UserID )
				nIdLen = LIB_PACKET_LENGTH::UserID;
			::memcpy( m_strSenderID, strSenderID, nIdLen );
			m_strSenderID[nIdLen] = NULL;

			if( nNickLen > LIB_PACKET_LENGTH::UserNick )
				nNickLen = LIB_PACKET_LENGTH::UserNick;
			::memcpy( m_strSenderNick, strSenderNick, nNickLen );
			m_strSenderNick[nNickLen] = NULL;

			if( nFindLen > LIB_PACKET_LENGTH::FindUser )
				nFindLen = LIB_PACKET_LENGTH::FindUser;
			::memcpy( m_strReceiver, strFind, nFindLen );
			m_strReceiver[nFindLen] = NULL;

			if( nMemoLen > LIB_PACKET_LENGTH::MemoMsg )
				nMemoLen = LIB_PACKET_LENGTH::MemoMsg;
			::memcpy( m_strMemo, strMemo, nMemoLen );
			m_strMemo[nMemoLen] = NULL;
		}
		short	GetFindType()		{ return m_nFindType;		}
		LPCTSTR	GetSenderID()		{ return m_strSenderID;		}
		LPCTSTR	GetSenderNick()		{ return m_strSenderNick;	}
		LPCTSTR	GetReceiver()		{ return m_strReceiver;		}
		LPCTSTR	GetMemo()			{ return m_strMemo;			}
		GET_REAL_LENGTH(CCasualPacketMemo)
	};

	class CCasualPacketMemoReply : public CPacket					// 메모 결과
	{
		SET_PACKET_COMMAND(CCasualPacketMemoReply,LIBCOMMAND::MemoReply)
		enum	FindType
		{
			ID		= 0,
			Nick	= 1,
		};
		enum	Result
		{
			Success		= 0,
			Fail		= 1,
		};
	private:
		SHORT	m_nFindType;
		SHORT	m_nResult;
		CHAR	m_strReceiver	[ LIB_PACKET_LENGTH::FindUser + 1 ];	// Receiver
	public:
		void	SetProperty		( SHORT nFindType, Result result, LPCTSTR strFind, int nFindLen )
		{
			m_nFindType	= nFindType;
			m_nResult	= result;

			if( nFindLen > LIB_PACKET_LENGTH::FindUser )
				nFindLen = LIB_PACKET_LENGTH::FindUser;
			::memcpy( m_strReceiver, strFind, nFindLen );
			m_strReceiver[nFindLen] = NULL;
		}
		SHORT	GetFindType()		{ return m_nFindType;		}
		SHORT	GetResult()			{ return m_nResult;			}
		LPCTSTR	GetReceiver()		{ return m_strReceiver;		}
		GET_REAL_LENGTH(CCasualPacketMemoReply)
	};

	class CCasualPacketInvite : public CPacket						// 초대하기
	{
		SET_PACKET_COMMAND(CCasualPacketInvite,LIBCOMMAND::Invite)
	public:
		enum	FindType
		{
			ID		= 0,
			Nick	= 1,
		};
	private:
		SHORT	m_nFindType;
		SHORT	m_nChannelIndex;	// 초대한 채널의 Index
		SHORT	m_nRoomIndex;		// 초대한 방의 Room Index
		CHAR	m_strSenderID	[ LIB_PACKET_LENGTH::UserID	+ 1 ];	// ID
		CHAR	m_strSenderNick	[ LIB_PACKET_LENGTH::UserNick	+ 1 ];	// Nick
		CHAR	m_strReceiver	[ LIB_PACKET_LENGTH::FindUser + 1 ];	// Receiver
		BOOL	m_bPass;
		CHAR	m_strPass		[ LIB_PACKET_LENGTH::RoomPass + 1 ];		// 패스워드
	public:
		void	SetProperty	( FindType nFindType, LPCTSTR strSenderID, int nIdLen, LPCTSTR strSenderNick, int nNickLen, LPCTSTR strFind, int nFindLen )
		{
			m_nFindType	= nFindType;
			m_bPass		= FALSE;

			if( nIdLen > LIB_PACKET_LENGTH::UserID )
				nIdLen = LIB_PACKET_LENGTH::UserID;
			::memcpy( m_strSenderID, strSenderID, nIdLen );
			m_strSenderID[nIdLen] = NULL;

			if( nNickLen > LIB_PACKET_LENGTH::UserNick )
				nNickLen = LIB_PACKET_LENGTH::UserNick;
			::memcpy( m_strSenderNick, strSenderNick, nNickLen );
			m_strSenderNick[nNickLen] = NULL;

			if( nFindLen > LIB_PACKET_LENGTH::FindUser )
				nFindLen = LIB_PACKET_LENGTH::FindUser;
			::memcpy( m_strReceiver, strFind, nFindLen );
			m_strReceiver[nFindLen] = NULL;

			SetLength( sizeof( CCasualPacketInvite ) - LIB_PACKET_LENGTH::RoomPass - 1 );
		}
		void	SetIndex	( short nChannelIndex, short nRoomIndex )
		{
			m_nChannelIndex	= nChannelIndex;
			m_nRoomIndex	= nRoomIndex;
		}
		void	SetPass		( LPCTSTR strPass, int nPassLength )
		{
			m_bPass		= TRUE;
			if( nPassLength > LIB_PACKET_LENGTH::RoomPass )
				nPassLength = LIB_PACKET_LENGTH::RoomPass;
			::memcpy( m_strPass, strPass, nPassLength );
			m_strPass[nPassLength] = NULL;
			SetLength( sizeof( CCasualPacketInvite ) );
		}

		short	GetFindType()		{ return m_nFindType;		}
		short	GetRoomIndex()		{ return m_nRoomIndex;		}
		LPCTSTR	GetSenderID()		{ return m_strSenderID;		}
		LPCTSTR	GetSenderNick()		{ return m_strSenderNick;	}
		LPCTSTR	GetReceiver()		{ return m_strReceiver;		}
		BOOL	IsPass()			{ return m_bPass;			}
		char*	GetPass()
		{
			if( m_bPass )
				return m_strPass;
			else
				return "";
		}
//		GET_REAL_LENGTH(CCasualPacketInvite)
		INT		GetRealLength()
		{
			if( m_bPass )
				return sizeof( CCasualPacketInvite );
			else
				return sizeof( CCasualPacketInvite ) - LIB_PACKET_LENGTH::RoomPass - 1;
		}
	};

	class CCasualPacketInviteReply : public CPacket					// 초대하기 결과
	{
		SET_PACKET_COMMAND(CCasualPacketInviteReply,LIBCOMMAND::InviteReply)
		enum	FindType
		{
			ID		= 0,
			Nick	= 1,
		};
		enum	Result
		{
			Success	= 0,
			Fail	= 1,
		};
	private:
		SHORT	m_nFindType;
		SHORT	m_nResult;
		CHAR	m_strReceiver	[ LIB_PACKET_LENGTH::FindUser + 1 ];	// Receiver
	public:
		void	SetProperty	( SHORT nFindType, Result result, LPCTSTR strFind, int nFindLen )
		{
			m_nFindType	= nFindType;
			m_nResult	= result;

			if( nFindLen > LIB_PACKET_LENGTH::FindUser )
				nFindLen = LIB_PACKET_LENGTH::FindUser;
			::memcpy( m_strReceiver, strFind, nFindLen );
			m_strReceiver[nFindLen] = NULL;
		}
		SHORT	GetFindType()		{ return m_nFindType;		}
		SHORT	GetResult()			{ return m_nResult;			}
		LPCTSTR	GetReceiver()		{ return m_strReceiver;		}
		GET_REAL_LENGTH(CCasualPacketInviteReply)
	};

	class CCasualPacketFindRequest : public CPacket			// 사용자 찾기 요청
	{
		SET_PACKET_COMMAND(CCasualPacketFindRequest,LIBCOMMAND::FindRequest)
		enum	FindType
		{
			ID		= 0,
			Nick	= 1,
		};
	private:
		short	m_nFindType;
		char	m_strFindUser	[ LIB_PACKET_LENGTH::FindUser + 1 ];	// Receiver
	public:
		void	SetProperty		( FindType nFindType, LPCTSTR strFindUser, int nFindLen )
		{
			m_nFindType	= nFindType;

			if( nFindLen > LIB_PACKET_LENGTH::FindUser )
				nFindLen = LIB_PACKET_LENGTH::FindUser;
			::memcpy( m_strFindUser, strFindUser, nFindLen );
			m_strFindUser[nFindLen] = NULL;
		}
		short	GetFindType()		{ return m_nFindType;		}
		LPCTSTR	GetFindUser()		{ return m_strFindUser;		}
		GET_REAL_LENGTH(CCasualPacketFindRequest)
	};

	class CCasualPacketFindReply : public CPacket					// 사용자 찾기 결과
	{
		SET_PACKET_COMMAND(CCasualPacketFindReply,LIBCOMMAND::FindReply)
		enum	Result
		{
			Success		= 0,
			Fail		= 1,
		};
	private:
		short	m_nResult;
		short	m_nChannelIndex;
		short	m_nRoomIndex;
		BOOL	m_bWaitRoom;
		char	m_strID		[ LIB_PACKET_LENGTH::UserID + 1 ];
		char	m_strNick	[ LIB_PACKET_LENGTH::UserNick + 1 ];
	public:
		void	SetProperty		( Result nResult, short nChannelIndex = 0, short nRoomIndex = 0, LPCTSTR strID = "", int nIDLen = 0, LPCTSTR strNick = "", int nNickLen = 0, BOOL bWaitRoom = FALSE )
		{
			m_nResult		= nResult		;
			m_nChannelIndex	= nChannelIndex	;
			m_nRoomIndex	= nRoomIndex	;
			m_bWaitRoom		= bWaitRoom		;
			
			if( nIDLen > LIB_PACKET_LENGTH::UserID )
				nIDLen = LIB_PACKET_LENGTH::UserID;
			::memcpy( m_strID, strID, nIDLen );
			m_strID[nIDLen] = NULL;

			if( nNickLen > LIB_PACKET_LENGTH::UserNick )
				nNickLen = LIB_PACKET_LENGTH::UserNick;
			::memcpy( m_strNick, strNick, nNickLen );
			m_strNick[nNickLen] = NULL;
		}
		short	GetResult()			{ return m_nResult;			}
		short	GetChannelIndex()	{ return m_nChannelIndex;	}
		short	GetRoomIndex()		{ return m_nRoomIndex;		}
		BOOL	IsWaitRoom()		{ return m_bWaitRoom;		}
		LPCTSTR	GetID()				{ return m_strID;			}
		LPCTSTR	GetNick()			{ return m_strNick;			}
		GET_REAL_LENGTH(CCasualPacketFindReply)
	};

	struct	TLibPackets
	{
		// Server
		CCasualPacketRemoveUser					RemoveUser;
		CCasualPacketRemoveRoom					RemoveRoom;
		CCasualPacketRoomInfoChange				RoomInfoChange;
		CCasualPacketLoginReply					LoginReply;
		CCasualPacketJoinChannelReply			JoinChannelReply;
		CCasualPacketUserInfo					UserInfo;
		CCasualPacketRoomInfo					RoomInfo;
		CCasualPacketOpenRoomReply				OpenRoomReply;
		CCasualPacketJoinRoomReply				JoinRoomReply;
		CCasualPacketAutoJoinRoomReply			AutoJoinRoomReply;
		CCasualPacketFindReply					FindReply;
		CCasualPacketRoomCaptineChange			RoomCaptineChange;
		CCasualPacketCompulsionExited			CompulsionExited;
		CCasualPacketRemoveRoomUser				RemoveRoomUser;
		CCasualPacketLeaveRoomReply				LeaveRoomReply;
		CCasualPacketCompulsionExitReply		CompulseExitReply;
		CCasualPacketMemoReply					MemoReply;
		CCasualPacketInviteReply				InviteReply;
	};
}/*namespace CasualPacket*/ }/*namespace CasualGame*/
//---------------------------------------------------------------------------------------------------------------------------------
using namespace CasualGame::CasualPacket;
