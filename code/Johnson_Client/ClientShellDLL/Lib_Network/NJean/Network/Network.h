/*****************************************************************************************************************
*****************************************************************************************************************/

#pragma once
#include <Windows.h>
#include <Winsock2.h>

//#ifndef unsigned long
//#define unsigned long ULONG*
//#endif

#ifndef interface
#define interface struct
#endif

namespace GamehiSolution
{
	namespace Network
	{
		// 소켓 통신 관련 이벤트를 받기 위한 인터페이스
		interface ITcpNetworkHandler
		{
			virtual void	OnAccept	( unsigned long lpNetworkKey )	= 0;
			virtual void	OnConnect	( unsigned long lpNetworkKey )	= 0;
			virtual void	OnBeginConnect	( unsigned long lpNetworkKey )	= 0;
			virtual void	OnReceive	( unsigned long lpNetworkKey, char* pRecvBuff, int nReceivedLenth )	= 0;
			virtual void	OnSend		( unsigned long lpNetworkKey, char* pRecvBuff, int nSenedLenth )	= 0;
			virtual void	OnClose		( unsigned long lpNetworkKey )	= 0;

			virtual void	OnAllocNetworkObject	( unsigned long lpCompletionKey )	= 0;
			virtual void	OnFreeNetworkObject		( unsigned long lpCompletionKey )	= 0;
		};

		// 네트워크 레이어의 클래스들의 기본 인터페이스
		interface INetwork
		{
			virtual BOOL	Create( ITcpNetworkHandler* pNetworkHandler, int nNumOfListener, int nNumOfConnectSockets, INT nRecvBuffSize, INT nSendBuffSize )	= 0;
			// nAcceptPoolSize, nAcceptRecvEvent : IOCP Server용 변수
			// hWindow, uiMessage : WSASelect Client용 변수
			virtual BOOL	CreateListener( int nServerIndex, int nNumOfCommSockets, int nAcceptPoolSize, BOOL bAcceptRecvEvent = FALSE )		= 0;
			virtual void	Destroy()																											= 0;
			
			// AsyncSelect용 함수
			virtual void	FirePacket( DWORD wParam, DWORD lParam )	= 0;

			virtual BOOL	Connect( unsigned long lpOwnerKey, const sockaddr* addr, int namelen )													= 0;
			virtual BOOL	Bind( int nServerIndex, const sockaddr* addr, int namelen, int nListenQueue = 5 )									= 0;
			virtual BOOL	Bind( int nServerIndex, int nPort, u_long hostAddr = INADDR_ANY, short family = AF_INET, INT nListenQueue = 5 )		= 0;
			virtual BOOL	StartListen( INT nServerIndex )																						= 0;
			virtual BOOL	Send( unsigned long lpCompletionKey, const char* sendBuff, INT nLen, INT& nOutError )									= 0;

			virtual void	CloseSocket( unsigned long lpCompletionKey )	= 0;
			virtual void	ShutDown( unsigned long lpCompletionKey )		= 0;

			virtual void	SetConnectRecvEvent			( unsigned long lpCompletionKey )	= 0;
			virtual void	ResetConnectRecvEvent		( unsigned long lpCompletionKey )	= 0;
			virtual void	WaitConnectRecvEvent		( unsigned long lpCompletionKey )	= 0;
			virtual void	SetRecvDisconnectEvent		( unsigned long lpCompletionKey )	= 0;
			virtual void	ResetRecvDisconnectEvent	( unsigned long lpCompletionKey )	= 0;
			virtual void	WaitRecvDisconnectEvent		( unsigned long lpCompletionKey )	= 0;

			// RTT
			virtual void	ResetRttTime				( unsigned long lpCompletionKey	)	= 0;
			virtual BOOL	CanStartRTT					( unsigned long lpCompletionKey	)	= 0;
			virtual void	SetRttStartTime				( unsigned long lpCompletionKey )	= 0;
			virtual void	SetRttEndTime				( unsigned long lpCompletionKey )	= 0;
			virtual void	GetRttTime					( unsigned long lpCompletionKey, DWORD& out_dwRttTime )	= 0;

			// Get/Set virtual Fuctions
			virtual INT			GetServicerIndex( unsigned long lpCompletionKey )						= 0;
			virtual VOID		SetServicerIndex( unsigned long lpCompletionKey, INT nIndex )			= 0;
			virtual unsigned long	GetOwnerKey( unsigned long lpNetworkKey )								= 0;
			virtual VOID		SetOwnerKey( unsigned long lpNetworkKey, unsigned long lpOwnerKey )			= 0;
			virtual VOID		GetAssemblerKey( unsigned long lpNetworkKey, unsigned long& lpAssemblerKey )	= 0;
			virtual VOID		SetAssemblerKey( unsigned long lpNetworkKey, unsigned long lpAssemblerKey )	= 0;

			virtual VOID		FreeNetworkKey	( unsigned long lpNetworkKey )		= 0;

			virtual LPCTSTR		GetLocalAddr	( unsigned long lpNetworkKey )		= 0;
			virtual LPCTSTR		GetPeerAddr		( unsigned long lpNetworkKey )		= 0;
		};
	}// namespace Network
}// namespace GamehiSolution