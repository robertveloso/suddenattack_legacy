//-------------------------------------------------------------------
// FILE : HUDHelp.h
// 목적 : 게임 중 F1을 눌러 도움말 HUD를 나오게 함.
// 설명 : 다른 활성/비활성 HUD의 경우 OPTION의 [키설정]과 연결되어 있다.
//        그러나 도움말의 경우는 일관적으로 F1을 사용한다.
//        예를 들어 VK_TAB을 통해 나타나는 HUDScore는 [키설정]에 의해
//        VK_TAB이 눌리면 COMMAND_ID_MISSION을 OnCommand(On/Off)로 받아 처리한다.
//        그러나 HUDHelp의 경우는 OnKey(Down/Up)을 이용해 직접 처리한다.
//        (CInterfaceMgr::OnKeyDown, OnKeyUp 참조)

#ifndef __YAIIN_HUD_HELP_H_
#define __YAIIN_HUD_HELP_H_

#include "HUDItem.h"
#include "ltguimgr.h"
#include "../../Shared/LTPoly.h"

class CHUDHelp : public CHUDItem
{
public:
	CHUDHelp();
	virtual ~CHUDHelp();

	LTBOOL				Init(void);
	void				Show(LTBOOL bView);
	void				Render(void);
	void				ScreenDimChanged(void);
	void				Update(void);
	void				PrePlayingState(void);

	LTBOOL				IsVisible(void);

private:
	HTEXTURE			m_hHelp[2];
	LTPoly_GT4			m_PolyHelp[2];
	LTBOOL				m_bView;
	
	float				m_fHUDHelpPos;
};

#endif