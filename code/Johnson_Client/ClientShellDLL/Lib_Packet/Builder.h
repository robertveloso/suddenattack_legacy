#pragma once
//#include "../../IFramework.h"

namespace Packet {

class Builder
{
public:
	Builder()
	{
	}
	virtual ~Builder()
	{
	}

	template<class TYPE>
	static TYPE *			Build()
	{
#ifdef _SERVER
		aSsErT( sizeof( TYPE ) < s_nMaxBuff );
#endif
		TYPE * pPacket		= reinterpret_cast<TYPE *>( m_pBuffer );
		pPacket->SetLength( sizeof( TYPE ) );
		pPacket->SetCommand( TYPE::s_nCode );
		
		return pPacket;

	}
	
	template<class TYPE>
	static void				Destroy( TYPE * i_pPacket )
	{
	}

private:
	enum
	{
		s_nMaxBuff			= 1024,
	};
	static char				m_pBuffer[ s_nMaxBuff ];

};

} /* Packet */