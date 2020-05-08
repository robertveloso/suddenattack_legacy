#pragma once

#include "../System.h"

namespace GameServer {

class ILog
{
public:
	class					IFacade
	:	public				Singleton<IFacade>
	{
	public:
		struct				Path
		{
			STRING			strRecordFail;
			STRING			strChatting;
		};
		typedef				const Path
							CPATH;

	public:
		virtual				~IFacade() {}

		virtual void		Open( CPATH & stPath ) = 0;

		virtual void		ChangeFile() = 0;

		virtual void		RecordFail( CSTRING & strMessage ) = 0;
		virtual void		Chatting( CSTRING & strMessage ) = 0;

	};
	static inline			IFacade *
	IFACADE()				{ return IFacade::GetInstance(); }

	class					IFactory
	:	public				Singleton<IFactory>
	{
	public:
		virtual				~IFactory() {}

		virtual ILog *		Create( const STRING & strPath ) = 0;
		virtual void		Destroy( ILog * pLog ) = 0;

	};
	static inline			IFactory *
	IFACTORY()				{ return IFactory::GetInstance(); }

public:
	virtual					~ILog() {}

	virtual void			ChangeFile() = 0;
	virtual void			Write( CSTRING & strMessage ) = 0;

};

} /* GameServer */
