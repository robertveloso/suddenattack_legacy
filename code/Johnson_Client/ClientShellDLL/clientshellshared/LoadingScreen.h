// ----------------------------------------------------------------------- //
//
// MODULE  : LoadingScreen.h
//
// PURPOSE : Background-thread loading screen encapsulation class
//
// CREATED : 2000
//
// (c) 2000, 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __LOADINGSCREEN_H__
#define __LOADINGSCREEN_H__

#include "BaseScreen.h"
#include "ScreenMgr.h"

class CLoadingScreen
{
// External functions
public:
	CLoadingScreen();
	~CLoadingScreen();

	// Note : Init/Term will be called automatically when needed, so
	// you shouldn't ever actually need to call them.

	// Initialize and load the screen resources
	// Returns LTFALSE if not in STATE_NONE
	LTBOOL Init();
	// Terminate the screen and its resources
	// Returns LTFALSE if not in STATE_INIT
	LTBOOL Term();

	// Show the loading screen
	// Setting bRun to LTTRUE will start the update thread
	// Returns LTFALES if the state isn't STATE_NONE or STATE_INIT
	LTBOOL Show(LTBOOL bRun = LTTRUE);
	// Update the loading screen
	// Returns LTFALSE if the state isn't STATE_SHOW
	LTBOOL Update();
	// Take the loading screen out of threaded mode
	// Returns LTFALSE if the state isn't STATE_ACTIVE
	LTBOOL Pause();
	// Resume threaded mode
	// Returns LTFALSE if the state isn't STATE_SHOW
	LTBOOL Resume();
	// Hide the loading screen
	// Returns LTFALSE if the state isn't STATE_SHOW or STATE_ACTIVE
	LTBOOL Hide();

	// Returns whether or not the loading screen is currently being actively updated
	inline LTBOOL IsActive() const { return m_eCurState == STATE_ACTIVE; };
	// Returns whether or not the loading screen is visible
	inline LTBOOL IsVisible() const { return ((m_eCurState == STATE_ACTIVE) || (m_eCurState == STATE_SHOW)); };

	//is loading a new mission?
	bool	NeedsPostLoadScreen() const;
	
// Internal functions
protected:
#ifdef NOPS2
	static unsigned long WINAPI ThreadBootstrap(void *pData);
	int RunThread();
#endif
private:
#ifdef NOPS2
	// The thread handle
	HANDLE m_hThreadHandle;
	// Events...
	HANDLE m_hEventEnd; // Stop rendering, damnit!
	HANDLE m_hEventThreadRunning; // Ok, the thread is done initializing, continue on

	CRITICAL_SECTION m_MissionUpdate;
	
#endif

	LTBOOL	m_bOldFogEnable;
	int		m_nOldFarZ;

	// State of the loading screen
	enum EScreenState { 
		STATE_NONE, // Not active in any way
		STATE_INIT, // It's been initialized, but that's it..
		STATE_SHOW, // It's on screen
		STATE_ACTIVE // It's in a thread and running
	};
	EScreenState m_eCurState;

	//////////////////////////////////////////////////////////////////////////////
	// Variables needed for updating (i.e. stuff only Init/Update/Term should touch)
	HTEXTURE		m_hFrame[10];
	LTPoly_GT4		m_PolyBack[10];

	float	m_fIncrease;
	float	m_fAlpha;
	
	uint32 m_nFrameCounter; // Frame tracking
	float m_fLastFrameTime, m_fCurFrameDelta, m_fSendPing;
	
	int32		 m_nCurrLoadingState, m_nLastLoadingState;
	HTEXTURE	 m_hLoadingBar, m_hCompleteText;
	LT_POLYGT4	 m_LoadingPolyBar;
	LTBOOL		 m_bStartLoading;
};

#endif //__LOADINGSCREEN_H__















