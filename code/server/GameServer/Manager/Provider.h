#pragma once

#include "../IManager.h"

namespace GameServer 
{
	namespace Manager
	{

		class Provider			:	public					IManager
		{
		public:
			typedef					::Dispatcher::IQuery	IQUERY;

		public:
			Provider();
			virtual					~Provider();

			void					Capture( PEER * i_pPeer );
			void					Release();

			virtual PEER *			GetPeer();

			virtual void			OnOpen();
			virtual void			OnPacket( PACKET * i_pPacket );
			virtual void			OnClose();
			virtual void			OnSignal( CUINT i_uiMessage );

			virtual void			OnResponse( IQUERY * i_pQuery );

		private:
			PEER *					m_pPeer;

		};

	} /* Manager */				} /* GamerServer */