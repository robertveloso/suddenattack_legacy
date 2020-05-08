/*****************************************************************************************************************
*****************************************************************************************************************/

#pragma once
#include <Windows.h>

namespace Network
{
	// 소켓 통신 관련 이벤트를 받기 위한 인터페이스
	interface ITcpNetworkHandler
	{
		virtual void	OnAccept	( ULONG_PTR lpNetworkKey )	= 0;
		virtual void	OnConnect	( ULONG_PTR lpNetworkKey )	= 0;
		virtual void	OnReceive	( ULONG_PTR lpNetworkKey, char* pRecvBuff, int nReceivedLenth )	= 0;
		virtual void	OnSend		( ULONG_PTR lpNetworkKey, char* pRecvBuff, int nSenedLenth )	= 0;
		virtual void	OnClose		( ULONG_PTR lpNetworkKey )	= 0;

		virtual void	OnAllocNetworkObject	( ULONG_PTR lpCompletionKey )	= 0;
		virtual void	OnFreeNetworkObject		( ULONG_PTR lpCompletionKey )	= 0;
	};

	// 네트워크 레이어의 클래스들의 기본 인터페이스
	interface INetwork
	{
		virtual BOOL	Create( ITcpNetworkHandler* pNetworkHandler, INT nNumOfListener, INT nNumOfConnectSockets, INT nRecvBuffSize, INT nSendBuffSize )	= 0;
		// nAcceptPoolSize, nAcceptRecvEvent : IOCP Server용 변수
		// hWindow, uiMessage : WSASelect Client용 변수
		virtual BOOL	CreateListener( INT nServerIndex, INT nNumOfCommSockets, INT nAcceptPoolSize, BOOL bAcceptRecvEvent = FALSE )		= 0;
		virtual void	Destroy()																											= 0;
		
		// AsyncSelect용 함수
		virtual void	FirePacket( DWORD wParam, DWORD lParam )	= 0;

		virtual BOOL	Connect( ULONG_PTR lpOwnerKey, const sockaddr* addr, INT namelen )													= 0;
		virtual BOOL	Bind( INT nServerIndex, const sockaddr* addr, INT namelen, INT nListenQueue = 5 )									= 0;
		virtual BOOL	Bind( INT nServerIndex, INT nPort, u_long hostAddr = INADDR_ANY, short family = AF_INET, INT nListenQueue = 5 )		= 0;
		virtual BOOL	StartListen( INT nServerIndex )																						= 0;
		virtual BOOL	Send( ULONG_PTR lpCompletionKey, const char* sendBuff, INT nLen, INT& nOutError )									= 0;

		virtual void	CloseSocket( ULONG_PTR lpCompletionKey )	= 0;
		virtual void	ShutDown( ULONG_PTR lpCompletionKey )		= 0;

		virtual void	SetConnectRecvEvent			( ULONG_PTR lpCompletionKey )	= 0;
		virtual void	ResetConnectRecvEvent		( ULONG_PTR lpCompletionKey )	= 0;
		virtual void	WaitConnectRecvEvent		( ULONG_PTR lpCompletionKey )	= 0;
		virtual void	SetRecvDisconnectEvent		( ULONG_PTR lpCompletionKey )	= 0;
		virtual void	ResetRecvDisconnectEvent	( ULONG_PTR lpCompletionKey )	= 0;
		virtual void	WaitRecvDisconnectEvent		( ULONG_PTR lpCompletionKey )	= 0;

		// RTT
		virtual void	ResetRttTime				( ULONG_PTR lpCompletionKey	)	= 0;
		virtual BOOL	CanStartRTT					( ULONG_PTR lpCompletionKey	)	= 0;
		virtual void	SetRttStartTime				( ULONG_PTR lpCompletionKey )	= 0;
		virtual void	SetRttEndTime				( ULONG_PTR lpCompletionKey )	= 0;
		virtual void	GetRttTime					( ULONG_PTR lpCompletionKey, DWORD& out_dwRttTime )	= 0;

		// Get/Set virtual Fuctions
		virtual INT			GetServicerIndex( ULONG_PTR lpCompletionKey )						= 0;
		virtual VOID		SetServicerIndex( ULONG_PTR lpCompletionKey, INT nIndex )			= 0;
		virtual ULONG_PTR	GetOwnerKey( ULONG_PTR lpNetworkKey )								= 0;
		virtual VOID		SetOwnerKey( ULONG_PTR lpNetworkKey, ULONG_PTR lpOwnerKey )			= 0;
		virtual VOID		GetAssemblerKey( ULONG_PTR lpNetworkKey, ULONG_PTR& lpAssemblerKey, DWORD& dwAsswemblerHandle )	= 0;
		virtual VOID		SetAssemblerKey( ULONG_PTR lpNetworkKey, ULONG_PTR lpAssemblerKey, DWORD dwAsswemblerHandle )	= 0;

		virtual VOID		FreeNetworkKey	( ULONG_PTR lpNetworkKey )		= 0;

//			virtual BOOL		IsConnected		( ULONG_PTR lpNetworkKey )		= 0;

		virtual LPCTSTR		GetLocalAddr	( ULONG_PTR lpNetworkKey )		= 0;
		virtual LPCTSTR		GetPeerAddr		( ULONG_PTR lpNetworkKey )		= 0;
	};
}// namespace Network
