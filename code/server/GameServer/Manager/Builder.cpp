#include "StdAfx.h"

#include "./Builder.h"
#include "../../Share/Packet/Manager.h"

namespace GameServer 
{
	namespace Manager 
	{

		Builder::Builder()
		{
			m_pPool					= new POOL;

			this->install();

		}

		Builder::~Builder()
		{
			this->uninstall();

			SAFE_DELETE( m_pPool );
		}

		Builder::ICLIENT *
			Builder::Build( PACKET * i_pPacket, PEER * i_pPeer )
		{
			Assert( i_pPacket != NULL );

			if( i_pPacket->GetCommand() != ::Packet::Manager::CQ_Login::s_nCode )
			{
				return NULL;
			}

			Provider * pProvider	= m_pPool->Capture();
			pProvider->Capture( i_pPeer );

			return pProvider;
		}

		void
			Builder::Destroy( IManager * i_pManager )
		{
			Assert( i_pManager != NULL );

			Provider * pProvider	= static_cast<Provider *>( i_pManager );
			pProvider->Release();

			m_pPool->Release( pProvider );
		}

	} /* Manager */				} /* GamerServer */