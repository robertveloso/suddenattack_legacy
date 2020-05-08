#pragma once

#undef	interface
#define	interface struct

#include "../IHandler.h"
#include "../Time/Provider.h"
#include "../../jolyo/NJean/Session/SessionGate.h"
//#include "../../jolyo/DataStructure/MemoryPool.h"

namespace Dispatcher {		namespace Handler {
class Peer;
} /* Handler */				} /* Dispatcher */

namespace Dispatcher {		namespace Query {
class Requester;
class Responser;
class Factory;
class Builder;
} /* Query */				} /* Dispatcher */

namespace Dispatcher {		namespace Client {
class Register;
class Session;
} /* Query */				} /* Dispatcher */

namespace Dispatcher {
class IConsole;
} /* Dispatcher */

namespace Dispatcher {		namespace Handler {

class Provider
:	public					IHandler
,	public					::Session::Gate::CSessionGate
{
public:
	typedef					::Session::Message::CPacketCheckSum
							CHECKSUM;
	typedef					::Session::Message::CPacketAssembler
							PACKETASSEMBLER;

public:
							Provider();
	virtual					~Provider();

	virtual CBOOL			Initialize( const Argument & i_stArg );
	virtual void			Start( const Argument & i_stArg );
	
	virtual
	const String<17> &		GetPeerIP( PEER * i_pPeer );

	virtual CDWORD			GetTickCount();
	
	COMMAND *				AllocDbCommand();
	void					FreeDbCommand( COMMAND * i_pCommand );

	void					Request( Application::CPeer* pPeer, UINT uiMessage, ::Session::Manage::CDbObject* pDbObject, void * i_pArg );

	void					Alarm( UINT uiMessage, UINT uiSecond, ULONG_PTR pParam );
	void					Signal( UINT uiMessage, ULONG_PTR pParam );
	void					Send( Application::CPeer* pPeer, const char * pData, INT nDataLen );
	void					Close( Application::CPeer * i_pPeer );

	CDWORD					GetRTT( Application::CPeer * i_pPeer );

private:
	// SessionGate
	virtual void			OnSignal( UINT uiMessage, ULONG_PTR wParam, ULONG_PTR lParam );
	virtual void			OnResponse( Application::CPeer* pPeer, ::Session::Manage::CDbObject * pDbObject );

//	virtual void			OnAccept( Application::CPeer* pPeer, INT nServicerIndex );
//	virtual void			OnConnect( Application::CPeer* pPeer, INT nServicerIndex );
	virtual void			OnClose( Application::CPeer* pPeer, INT nServicerIndex );
	virtual void			OnData( Application::CPeer* pPeer, INT nServicerIndex, char* pPacket, int nPacketLength );

	virtual void			OnTimeout( Application::CPeer* pPeer );

	virtual
	Application::CPeer*		AllocPeer( INT nListenIndex );
	virtual void			FreePeer( Application::CPeer* pPeer, INT nListenIndex );

	virtual void			InitListenerPacketType( INT nServicerIndex, CHECKSUM::CheckSumType & out_CheckSumType, PACKETASSEMBLER::PacketUnitType & out_PacketUnitType );
	virtual void			InitLengthFieldOfPacket( INT nServicerIndex, INT & out_nLengthFieldPos, PACKETASSEMBLER::LengthSizeType & out_lengthSizeType, PACKETASSEMBLER::LengthType & out_lengthType, BOOL & out_bLittleEndian );
	virtual BOOL			IsAliveManageObject( INT nServicerIndex );

private:
	typedef					Common::DataStructure::CMemoryPool<Peer>
							PEER_POOL;
	static CINT				s_nService = 1; //CNJeanGate中定义为ServicerIndex::Client

private:
	PEER_POOL *				m_pPeerPool;
	INT						m_nClientCount;

	Query::Factory *		m_pQueryFactory;
	Query::Builder *		m_pQueryBuilder;
	Query::Requester *		m_pQueryRequester;
	Query::Responser *		m_pQueryResponser;

	Client::Register *		m_pClientRegister;
	Client::Session *		m_pClientSession;

	Time::Provider *		m_pTime;

	IConsole *				m_pConsole;

	ULONG					m_ulTickCount;
	BOOL					m_bTickCount;

	String<17>				m_strIP;
};

} /* Handler */				} /* Dispatcher */