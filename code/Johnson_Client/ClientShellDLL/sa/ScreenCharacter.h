#ifndef _SCREEN_CHARACTER_H_
#define _SCREEN_CHARACTER_H_

#include "ScreenBase.h"
#include "LayoutMgr.h"

class CPopupNickName;

class CScreenCharacter : public CScreenBase
{
public :
	CScreenCharacter(){};
	virtual ~CScreenCharacter(){};

	

	void	SetFirstStep( LTBOOL bNickName ){};

	void			ResponseNickName( int nResult ){};
	void			ResponseCharInfo( int nResult ){};

	void			ShowPopupNick();

protected :
};

extern CScreenCharacter *g_pScreenCharacter;

#endif // _SCREEN_CHARACTER_H_