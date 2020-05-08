#pragma once

#include "..\NJean\Application\Packet.h"
// 패킷 클래스는 절대로 virtual 함수를 사용해서는 안된다!!
#define __TEST__
#define __TEST_USERNO__	TEXT("1234")

namespace CasualGame { namespace Define
{
	class CCasualCommon
	{
	public:
		enum	UserMessage
		{
			UserSignal	= 1000,
			UserQuery	= 1000,
			UserPacket	= 1000,
		};

		enum	CasualQueryMessage
		{
			LoginQuery					= 100,
		};// enum CasualQueryMessage

		enum	CasualSignalMessage
		{
			ChangeCaptineSignal			= 100,		// 방장 변경 시그널
			CompulseExitSignal			= 101,		// 강제 퇴장
			//CompulseDisconnectSignal	= 102,		// 강제 접속 종료
		};// enum CasualSignalMessage

		enum	CasualPacketCommand
		{
			///////////////////////////////////
			// 세션 관리
			
			LoginRequest			= 101,		// 로그인 요청
			//LoginCompulsionRequest	= 102,		// 강제 로그인 요청
			SecLoginRequest			= 103,		// 암호화 로그인
			JoinChannelRequest		= 104,		// 채널에 입장
			LeaveChannelRequest		= 105,		// 채널에서 퇴장

			LoginReply				= 106,		// 로그인 결과
			//CompulsionDisconnect	= 107,		// 강제 종료
			JoinChannelReply		= 108,		// 채널에 입장
			LeaveChannelReply		= 109,		// 채널에서 퇴장


			///////////////////////////////////
			// Room 관리
			
			OpenRoomRequest			= 201,		// 방 개설 요청
			JoinRoomRequest			= 202,		// 방에 입장 요청
			AutoJoinRoomRequest		= 203,		// 방에 자동 입장 요청
			LeaveRoomRequest		= 204,		// 방에서 퇴장 요청

			OpenRoomReply			= 205,		// 방 개설 결과
			JoinRoomReply			= 206,		// 방에 입장 결과
			AutoJoinRoomReply		= 207,		// 방에 자동 입장 결과
			LeaveRoomReply			= 208,		// 방에서 퇴장 요청 결과


			///////////////////////////////////
			// Room 정보

			//RoomUserInfoRequest		= 301,		// 방 사용자 보기 요청
			//RoomWaitUserInfoRequest	= 302,		// 대기실 사용자 보기 요청

			//RoomUserInfoReply		= 303,		// 방 사용자 보기 결과
			//RoomWaitUserInfoReply	= 304,		// 대기실 사용자 보기 결과
			RoomInfo				= 305,		// 개설된 방
			RemoveRoom				= 306,		// 폐쇄된 방
			RoomInfoChange			= 307,		// 변경된 방 : 옵션, 인원, 비번 설정
			//RoomUserInfo			= 308,		// 방의 사용자
			//RoomWaitUserInfo		= 309,		// 대기실의 사용자
			RoomCaptineChange		= 310,		// 방장 변경


			///////////////////////////////////
			// User 정보
			
			FindRequest				= 401,		// 사용자 찾기
			CompulseExitRequest		= 402,		// 강제퇴장 요청
			//WaitUserPropertyRequest	= 403,		// 대기실에 있는 사용자 정보요청

			UserInfo				= 404,		// 대기실에 입장한 사용자의 기본 정보
			RemoveUser				= 405,		// 대기실에서 퇴장한 사용자
			CompulseExitReply		= 412,		// 강제퇴장 요청 결과
			CompulseExited			= 406,		// 강제 퇴장 당함
			FindReply				= 407,		// 사용자 찾기
			//RoomUserProperty		= 408,		// 방에 있는 사용자 정보(방에 입장하는 사용자)
			RemoveRoomUser			= 409,		// 방에서 퇴장한 사용자
			//WaitUserPropertyReply	= 410,		// 대기실에 있는 사용자 정보결과
			//MyProperty				= 411,		// 자기 자신의 정보 : 로그인시 전달


			///////////////////////////////////
			// Packet 중계

			Chat					= 501,		// 채팅
			Memo					= 502,		// 쪽지
			Invite					= 503,		// 초대하기

			MemoReply				= 504,		// 쪽지 보내기 결과
			InviteReply				= 505,		// 초대하기 결과
		};// enum CasualPacketCommand

		enum	CasualStringLength
		{
			SecLogin		= 1024,		// 암호화 된 로그인 정보의 길이
			IpAddr			= 16,		// IP Address
			UserNo			= 20,		// User Id Index
			UserID			= 40,		// User ID
			UserNick		= 36,		// User Nick
			RoomName		= 36,		// 방 이름
			RoomOption		= 16,		// 방 옵션
			RoomPass		= 16,		// 방 패스워드
			FindUser		= 40,		// 사용자 검색시 필요한 스트링중 긴 데이타 (아이디 또는 닉)
			ChatMsg			= 200,		// 채팅 메시지
			MemoMsg			= 200,		// 메모 메시지
			AssembleBody	= 220,		// 임시
			UserProperty	= 2000,		// 사용자 정보 버퍼의 크기
			SessionKey		= 128,		// 세션키
		};// enum CasualStringLength
	};// class CCasualCommon

}/*namespace Common*/ }/*namespace CasualGame*/
using namespace CasualGame::Define;
#define LIBCOMMAND CCasualCommon::CasualPacketCommand
#define LIBPACKETLENGTH CCasualCommon::CasualStringLength
