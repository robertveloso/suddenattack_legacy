#pragma once

#include "../System.h"

namespace Dispatcher {

class IThread
{
public:
	class					IObserver
	{
	public:
		virtual				~IObserver() {}

		virtual void		OnRun() = 0;
		virtual void		OnStop() = 0;

	};

	class					IFactory
	:	public				Singleton<IFactory>
	{
	public:
		typedef				IThread::IObserver
							IOBSERVER;

	public:
		virtual				~IFactory() {}

		virtual IThread *	Create( IOBSERVER * i_pObserver ) = 0;
		virtual void		Destroy( IThread * i_pThread ) = 0;

	};
	static inline			IFactory *
	IFACTORY()				{ return IFactory::GetInstance(); }

public:
	virtual					~IThread() {}

	virtual void			Stop() = 0;

};

} /* Dispatcher */
