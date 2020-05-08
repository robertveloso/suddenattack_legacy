#pragma once

// 패킷 클래스는 절대로 virtual 함수를 사용해서는 안된다!!

namespace Application
{
	namespace Packets
	{
		struct CPacket
		{
		public:
			// 미리 지정된 Commands : 0~99 사이
			enum	PredefineCommand
			{
				// Ping : Ping -> PingAck -> Ack
				Ping		= 0,
				PingAck		= 1,
				Ack			= 2,

				Assemble	= 3,
			};

			enum	PredefineLength
			{
				BasePacketSize	= 256,
				AssembleSize	= 200,
				
				GetQuerySize	= 32767,
//					PutQueryAddr	= 30,
//					PutQueryCommand	= 30,
//					PutQueryData	= 100,
			};

		private:
			short	m_nLength;
			short	m_nCommand;

		public:
			CPacket	( short nLength, short nCommand )
				:	m_nLength	( nLength	)
				,	m_nCommand	( nCommand	)
			{}
			CPacket()
			{}

		public:
			short	GetLength()
			{
				return m_nLength;
			}
			void	SetLength( int nLength )
			{
				m_nLength = nLength;
			}
			short	GetCommand()
			{
				return m_nCommand;
			}
			void	SetCommand( short nNewCommand )
			{
				m_nCommand = nNewCommand;
			}

			static int	GetLengthPos()
			{
				return 0;
			}

			static BOOL	IsPredefinePacket( int nCommand )
			{
				if( 0 <= nCommand && nCommand < 100 )
					return TRUE;
				else
					return FALSE;
			}
		};

		// Ping
		struct CPacketPing : public CPacket
		{
		private : 
			DWORD	m_dwIndex;

		public :
			CPacketPing	( DWORD dwIndex = 0 )
				:	CPacket		( sizeof( CPacketPing ), CPacket::PredefineCommand::Ping )
				,	m_dwIndex	( dwIndex )
			{
			}
            
			void	SetIndex	( DWORD dwIndex	)
			{
				m_dwIndex = dwIndex;
			}
			DWORD	GetIndex()	{ return m_dwIndex;		}

			INT		GetRealLength()
			{
				return sizeof( CPacketPing );
			}
		};// class CPacketPing


		// PingAck
		struct CPacketPingAck : public CPacket
		{
		public :
			CPacketPingAck	()
				:	CPacket( sizeof( CPacketPingAck ), CPacket::PredefineCommand::PingAck )
			{
			}

			INT		GetRealLength()
			{
				return sizeof( CPacketPingAck );
			}
		};// class CPacketPingAck


		// Ack
		struct CPacketAck : public CPacket
		{
		public :
			CPacketAck		()
				:	CPacket	( sizeof( CPacketAck ), CPacket::PredefineCommand::Ack )
			{
			}

			INT		GetRealLength()
			{
				return sizeof( CPacketAck );
			}
		};// class CPacketAck

		// Assemble
		struct CPacketAssemble : public CPacket
		{
		public:
			enum	Type
			{
				More	= 1,
				End		= 2,
			};

		private:
			short	m_Type;
			short	m_nBodySize;
			char	m_Body[ CPacket::PredefineLength::AssembleSize ];

		public :
			CPacketAssemble	( Type type, char* pBody, INT nBodyLen )
				:	CPacket	( sizeof( CPacketAssemble ), CPacket::PredefineCommand::Assemble )
				,	m_Type	( type )
			{
				if( nBodyLen > CPacket::PredefineLength::AssembleSize )
					nBodyLen = CPacket::PredefineLength::AssembleSize;
				memcpy( m_Body, pBody, nBodyLen );
				m_nBodySize		= nBodyLen;
			}

			Type	GetType()		{ return (Type)m_Type;	}
			short	GetBodySize()	{ return m_nBodySize;	}
			char*	GetBody()		{ return m_Body;		}

			INT		GetRealLength()
			{
				return sizeof( CPacketAssemble );
			}
		};

	}// namespace Packets
}// namespace Application

using namespace Application::Packets;