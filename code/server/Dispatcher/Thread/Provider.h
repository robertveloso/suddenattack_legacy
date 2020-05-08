#pragma once

#include "../IThread.h"

namespace Dispatcher {		namespace Thread {

class Provider
:	public					IThread
{
public:
							Provider();
	virtual					~Provider();

	void					Capture( IObserver * i_pObserver );
	void					Release();

	virtual void			Stop();

private:
	static ULONG WINAPI		bridge( void * pArgument );

private:
	HANDLE					m_hThread;
	IObserver *				m_pObserver;
	BOOL					m_bRun;

};

} /* Thread */				} /* Dispatcher */
