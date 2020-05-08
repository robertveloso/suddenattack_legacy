// ----------------------------------------------------------------------- //
//
// MODULE  : HUDChatbar.h
//
// PURPOSE : 아머게이지와 헬스게이지
//
// Author  : monk77
//
// GMOS 2003.12.01
//
// ----------------------------------------------------------------------- //

#ifndef __HUD_CHATBAR_H
#define __HUD_CHATBAR_H

//->Source 추가 Start.[추가되어진 요일:01.12.2003]
//엔진의 헤더파일 추가
#include "ilttexinterface.h"
#include "cuifont.h"
#include "iltfontmanager.h"
#include "cuipolystring.h"

#include "LayoutMgr.h"
//->Source 추가 End.  [추가       사람:monk77]  [#|#]

class CHUDCHATBAR  
{
public:
	CUIFont*		pMonoFont; 
	HTEXTURE 		hTex;
	CUIPolyString*	pMonoString[5];

	CHUDCHATBAR();

	virtual ~CHUDCHATBAR();

public:
	void DrawBar();
	void DrawIconHeal();
	void DrawIconArm();

	void UpdateLayout();
	void DrawHeal(char* pBuffer);
	void Render();

	void DrawArm(char* pBuffer);

	void Term();

	void Init(const char *szTexName);

private:
	LTBOOL	m_ChatbarText;
	LTIntPt	m_ChatbarTextSize;
	LTIntPt m_ChatbarTextSizeWidth;
};

#endif
