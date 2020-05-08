#pragma once

// 2004년 3월 22일
#include <Windows.h>

namespace Session
{
	namespace Message
	{
		class CPacketCheckSum
		{
		public:
			CPacketCheckSum(void){}
			~CPacketCheckSum(void){}

		public:
			enum	CheckSumType
			{
				None			= 0,	// 첵섬을 사용하지 않는다.
				TwoByteZoroOne	= 1,	// 첵섬값으로 010101의 2Byte를 사용한다.
			};

			static void	SetCheckSum( CheckSumType checkSumType, char* pPacket, int nPacketSize )
			{
				switch( checkSumType )
				{
					// None일 경우에는 아무것도 하지 않는다.
				case CheckSumType::None :
					int a;
					a = 3;
					break;

				case CheckSumType::TwoByteZoroOne :
					CheckSumTwoByteZeroOne	( pPacket, nPacketSize );
					break;

				default :
					int b;
					b = 3;
					break;
				}
			}

			static BOOL	IsCorrectCheckSum( CheckSumType checkSumType, const char* pPacket, int nPacketSize )
			{
				switch( checkSumType )
				{
					// None일 경우에는 아무것도 하지 않는다.
				case CheckSumType::None :
					return TRUE;

				case CheckSumType::TwoByteZoroOne :
					return IsCorrectCheckSumTwoByteZeroOne	( pPacket, nPacketSize );

				default :
					return FALSE;
				}
			}

			static int	GetCheckSumSize( CheckSumType checkSumType )
			{
				switch( checkSumType )
				{
					// None일 경우에는 아무것도 하지 않는다.
				case CheckSumType::None :
					return 0;

				case CheckSumType::TwoByteZoroOne :
					return 2;

				default :
					return -1;
				}
			}

		private:
			////////////////////////////////////////////////////////////////////////////////////////////////////
			// check sum을 패킷의 뒤에 채운다
			//

			// TwoByteZeroOne인 경우에는 뒤에 2byte를 010101로 채운다.
			static void	CheckSumTwoByteZeroOne	( char* pPacket, int nPacketSize )
			{
				short nCheckSum = 0x1010;

				if( pPacket == NULL || nPacketSize > 300 )
				{
					int a = 3;
					return;
				}

				memcpy( &pPacket[nPacketSize], &nCheckSum, 2 );
			}


			////////////////////////////////////////////////////////////////////////////////////////////////////
			// check이 맞는지를 검사한다.
			//

			// 뒤의 2byte가 01010인지를 검사한다.
			static bool	IsCorrectCheckSumTwoByteZeroOne	( const char* pPacket, int nPacketSize )
			{
				short nCheckSum = 0x1010;

				if( pPacket == NULL || nPacketSize > 300 )
				{
					int a = 3;
					return false;
				}

				return( memcmp( &pPacket[nPacketSize], &nCheckSum, 2 ) == 0 );
			}

		};// class CPacketCheckSum

	}// namespace Message

}// namespace Session
