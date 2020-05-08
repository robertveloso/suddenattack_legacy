// ----------------------------------------------------------------------- //
//
// MODULE  : HUDChatbar.cpp
//
// PURPOSE : 아머게이지와 헬스게이지
//
// Author  : monk77
//
// GMOS 2003.12.01 
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "HUDChatbar.h"

//->Source 추가 Start.[추가되어진 요일:01.12.2003] Function name:-
#include "InterfaceMgr.h"
//->Source 추가 End.  [추가       사람:monk77]  [#|#]

CHUDCHATBAR::CHUDCHATBAR()
{

}

CHUDCHATBAR::~CHUDCHATBAR()
{

}

void CHUDCHATBAR::Init(const char *szTexName)
{
	UpdateLayout();
	
	//폰트를 생성한다.
	g_pTexInterface->CreateTextureFromName(hTex, szTexName);
	
	pMonoFont = g_pFontManager->CreateFont(hTex, (uint8)9, (uint8)9);

}

void CHUDCHATBAR::DrawArm(char* pBuffer)
{
	pMonoFont->SetDefCharHeight(m_ChatbarTextSize.y);

	pMonoFont->SetDefCharWidth(m_ChatbarTextSize.x);

	pMonoFont->SetDefSpacingH(-1);

	//DTX폰트로 글자를 찍는다.
	pMonoFont->SetDefColors(0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0x00000000 | CUI_SYSTEM_OPAQUE);

	pMonoFont->SetAttributes(CUI_FONT_BOLD);

	pMonoString[0] = g_pFontManager->CreatePolyString(pMonoFont, 
			pBuffer, 50, 550);

}

void CHUDCHATBAR::DrawHeal(char* pBuffer)
{	
	pMonoFont->SetDefCharHeight(m_ChatbarTextSize.y);

	pMonoFont->SetDefCharWidth(m_ChatbarTextSize.x);

	pMonoFont->SetDefSpacingH(-1);

	//DTX폰트로 글자를 찍는다.
	pMonoFont->SetDefColors(0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0x00000000 | CUI_SYSTEM_OPAQUE);
	
	pMonoFont->SetAttributes(CUI_FONT_BOLD);

	pMonoString[1] = g_pFontManager->CreatePolyString(pMonoFont, 
			pBuffer, 130, 550);

}

void CHUDCHATBAR::DrawIconArm()
{
	pMonoFont->SetDefCharHeight(15);

	pMonoFont->SetDefCharWidth(10);

	pMonoFont->SetDefSpacingH(-1);

	pMonoFont->SetDefColors(0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0x00000000 | CUI_SYSTEM_OPAQUE);

	pMonoFont->SetAttributes(CUI_FONT_BOLD);

	pMonoString[2] = g_pFontManager->CreatePolyString(pMonoFont, 
			"ARM", 10, 555);

}

void CHUDCHATBAR::DrawIconHeal()
{
	pMonoFont->SetDefCharHeight(15);

	pMonoFont->SetDefCharWidth(10);

	pMonoFont->SetDefSpacingH(-1);

	pMonoFont->SetDefColors(0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0x00000000 | CUI_SYSTEM_OPAQUE);
	
	pMonoFont->SetAttributes(CUI_FONT_BOLD);

	pMonoString[3] = g_pFontManager->CreatePolyString(pMonoFont, 
			"HP", 100, 555);

}

void CHUDCHATBAR::DrawBar()
{
	pMonoFont->SetDefCharHeight(m_ChatbarTextSize.y);

	pMonoFont->SetDefCharWidth(m_ChatbarTextSize.x);

	pMonoFont->SetDefSpacingH(-1);

	pMonoFont->SetDefColors(0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0xFFffff44 | CUI_SYSTEM_OPAQUE,
							   0x00000000 | CUI_SYSTEM_OPAQUE);
	
	pMonoFont->SetAttributes(CUI_FONT_BOLD);

	pMonoString[4] = g_pFontManager->CreatePolyString(pMonoFont, 
			"|", 87, 550);

}

void CHUDCHATBAR::Term()
{
	if (hTex)  g_pTexInterface->ReleaseTextureHandle(hTex);

	if (pMonoFont) 
	{
		g_pFontManager->DestroyFont(pMonoFont);

		if (pMonoString[0])	g_pFontManager->DestroyPolyString(pMonoString[0]);
		if (pMonoString[1])	g_pFontManager->DestroyPolyString(pMonoString[1]);
		if (pMonoString[2])	g_pFontManager->DestroyPolyString(pMonoString[2]);
		if (pMonoString[3])	g_pFontManager->DestroyPolyString(pMonoString[3]);
		if (pMonoString[4])	g_pFontManager->DestroyPolyString(pMonoString[4]);
	}
}

void CHUDCHATBAR::Render()
{
	if (m_ChatbarText)
	{	
		if (pMonoFont) 
		{
			pMonoString[0]->Render();
			pMonoString[1]->Render();
			pMonoString[2]->Render();
			pMonoString[3]->Render();
			pMonoString[4]->Render();
		}
	}
}

void CHUDCHATBAR::UpdateLayout()
{
	int nCurrentLayout = GetConsoleInt("HUDLayout",0);

	m_ChatbarText			= g_pLayoutMgr->GetUseChatbarText(nCurrentLayout);
	m_ChatbarTextSize		= g_pLayoutMgr->GetUseChatbarTextSize(nCurrentLayout);
	m_ChatbarTextSizeWidth	= g_pLayoutMgr->GetUseChatbarTextSizeWidth(nCurrentLayout);
}
