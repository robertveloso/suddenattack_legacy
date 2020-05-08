// ----------------------------------------------------------------------- //
//
// MODULE  : InterfaceResMgr.cpp
//
// PURPOSE : Manager for resources associated with the interface
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "gameclientshell.h"
#include "InterfaceResMgr.h"
#include "ClientButeMgr.h"
// Added by IN'T ==>
#include "ControlConstant.h"
#include <d3dx8core.h>
// Added by IN'T <==

CInterfaceResMgr*   g_pInterfaceResMgr = LTNULL;

namespace
{
	const uint8 kIndent = 8;
	const uint16 kBaseWidth = 320;
	const uint16 kBaseHeight = 200;
	const uint16 kBigWidth = 480;
	const uint16 kMinWidth = 160;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInterfaceResMgr::CInterfaceResMgr()
{
	g_pInterfaceResMgr = this;

	m_hSurfCursor = NULL;

	m_dwScreenWidth = 0;
	m_dwScreenHeight = 0;

	m_fXRatio = 1.0f;
	m_fYRatio = 1.0f;
}

CInterfaceResMgr::~CInterfaceResMgr()
{
    g_pInterfaceResMgr = LTNULL;
}

//////////////////////////////////////////////////////////////////////
// Function name	: CInterfaceResMgr::Init
// Description	    :
// Return type      : LTBOOL
//////////////////////////////////////////////////////////////////////
LTBOOL CInterfaceResMgr::Init()
{
	// Set the English flag
    if (stricmp((const char*)"english", LoadTempString(IDS_GAME_LANGUAGE)) != 0)
    {
        m_bEnglish=LTFALSE;
	}
	else
    {
        m_bEnglish=LTTRUE;
	}

	// Load the virtual key codes for yes responses
	char szTmp[64] = "";
    LoadString(IDS_MENU_VKEY_YES,szTmp,sizeof(szTmp));
	if (strlen(szTmp))
	{
        m_nYesVKeyCode=atoi(szTmp);
	}

	// Load the virtual key codes for no responses
    LoadString(IDS_MENU_VKEY_NO,szTmp,sizeof(szTmp));
	if (strlen(szTmp))
	{
        m_nNoVKeyCode=atoi(szTmp);
	}

	// Init the InterfaceSurfMgr class
    m_InterfaceSurfMgr.Init(g_pLTClient);

	// set resolution dependant variables
	ScreenDimsChanged();

    return LTTRUE;
}

//////////////////////////////////////////////////////////////////////
// Function name	: CInterfaceResMgr::Term
// Description	    :
// Return type		: void
//////////////////////////////////////////////////////////////////////

void CInterfaceResMgr::Term()
{
	Clean();

	ReleaseDXFont();

	// Terminate the InterfaceSurfMgr class
	m_InterfaceSurfMgr.Term();

	// Destroy CUI fonts through the font manager
	

	// Release any textures that the InterfaceResMgr has.
	for (TextureSet::iterator iter = m_TextureSet.begin(); iter != m_TextureSet.end(); iter++)
	{
		g_pTexInterface->ReleaseTextureHandle(*iter);
	}
	m_TextureSet.clear();

	if (m_hSurfCursor)
	{
        g_pLTClient->DeleteSurface(m_hSurfCursor);
		m_hSurfCursor = NULL;
	}

}

//////////////////////////////////////////////////////////////////////
// Function name	: CInterfaceResMgr::Setup
// Description	    :
// Return type      : LTBOOL
//////////////////////////////////////////////////////////////////////

LTBOOL CInterfaceResMgr::Setup()
{
	//preload common surfaces

    return LTTRUE;
}

//////////////////////////////////////////////////////////////////////
// Function name	: CInterfaceResMgr::Clean
// Description	    :
// Return type		: void
//////////////////////////////////////////////////////////////////////

void CInterfaceResMgr::Clean()
{
    if (g_pLTClient)
	{

		// free shared surfaces
		m_InterfaceSurfMgr.FreeAllSurfaces();


	}
}

//////////////////////////////////////////////////////////////////////
// Function name	: CInterfaceResMgr::DrawScreen
// Description	    :
// Return type		: void
//////////////////////////////////////////////////////////////////////
void CInterfaceResMgr::DrawScreen()
{
    _ASSERT(g_pLTClient);
    if (!g_pLTClient) return;

	if (m_dwScreenWidth == 0)
		ScreenDimsChanged();

	// The screen surface
    HSURFACE hScreen = g_pLTClient->GetScreenSurface();

	// Render the current screen
	g_pInterfaceMgr->GetScreenMgr()->Render(hScreen);

	return;
}

void CInterfaceResMgr::ScreenDimsChanged()
{
    if (!g_pLTClient) return;

	RMode currentMode;
    g_pLTClient->GetRenderMode(&currentMode);

	m_fXRatio = (float)currentMode.m_Width / USABLE_WIDTH_F;
	m_fYRatio = (float)currentMode.m_Height / USABLE_HEIGHT_F;
	
	m_dwScreenWidth = currentMode.m_Width;
	m_dwScreenHeight = currentMode.m_Height;
}


HSURFACE CInterfaceResMgr::GetSurfaceCursor()
{
	if (!m_hSurfCursor)
        m_hSurfCursor = g_pLTClient->CreateSurfaceFromBitmap("interface\\cursor0.pcx");
	_ASSERT(m_hSurfCursor);
	return m_hSurfCursor;
};

void CInterfaceResMgr::ConvertScreenPos(int &x, int &y)
{
	if (m_dwScreenWidth == USABLE_WIDTH_I && m_dwScreenHeight == USABLE_HEIGHT_I)
		return;

	x = (int)( (LTFLOAT)x * m_fXRatio );
	y = (int)( (LTFLOAT)y * m_fYRatio );
};

void CInterfaceResMgr::ConvertScreenRect(LTRect &rect)
{
	if (m_dwScreenWidth == USABLE_WIDTH_I && m_dwScreenHeight == USABLE_HEIGHT_I)
		return;

	rect.left = (int)( (LTFLOAT)rect.left * m_fXRatio );
	rect.right = (int)( (LTFLOAT)rect.right * m_fXRatio );
	rect.top = (int)( (LTFLOAT)rect.top * m_fYRatio );
	rect.bottom = (int)( (LTFLOAT)rect.bottom * m_fYRatio );

};

//////////////////////////////////////////////////////////////////////
// Load font information from file and build the font
CUIFont* CInterfaceResMgr::CreateFont(char const* pszFontFile, char const* pszFontFace, uint8 ptSize)
{
	// Must specify a font face.
	if ( !pszFontFace ) 
		return LTNULL;

	CUIFont * pFont = g_pFontManager->CreateFont( pszFontFile, pszFontFace, ptSize, 33, 255);
	
	//this will set the width of the space character for proportional fonts
	if (pFont)
	{
		pFont->SetDefCharWidth((ptSize/4));
		pFont->SetDefColor(argbBlack);
	}

	if (pFont)
	{
		uint32 w,h;
		g_pTexInterface->GetTextureDims(pFont->GetTexture(),w,h);
		g_pLTClient->CPrint("Created font <%s> using a %dX%d texture.",pszFontFace,w,h);

	}

	
	return (pFont);
}


HTEXTURE CInterfaceResMgr::GetTexture(const char *szTexName)
{
	HTEXTURE hTex = LTNULL;

	LTRESULT res = g_pTexInterface->FindTextureFromName(hTex, szTexName);

	//if we found it, see if it's in our list
	if (res == LT_OK)
	{
		TextureSet::iterator iter = m_TextureSet.find(hTex);

		//if is not in our list, inc the ref count so no one else frees it
		if (iter == m_TextureSet.end())
		{
			g_pTexInterface->AddRefTextureHandle(hTex);
			m_TextureSet.insert(hTex);
		}
	}
	else
	{
		g_pTexInterface->CreateTextureFromName(hTex, szTexName);
		// Note valid textures which we will need to release during termination
		if (hTex)
			m_TextureSet.insert(hTex);

	}

	return hTex;
}

LPD3DXFONT CInterfaceResMgr::CreateDXFont(const char *szFontFace, uint8 nFontSize, uint8 nFontStyle)
{
	return CreateDXFont(szFontFace, nFontSize, !!(nFontStyle&SAFS_BOLD), !!(nFontStyle&SAFS_ITALIC), !!(nFontStyle&SAFS_UNDERLINE), !!(nFontStyle&SAFS_STRIKE));
}

LPD3DXFONT CInterfaceResMgr::CreateDXFont(const char *szFontFace, uint8 nFontSize, bool bBold, bool bItalic, bool bUnderLine, bool bStrike)
{
	char szFontName[MAX_PATH];
	MakeFontName(szFontName, szFontFace, nFontSize, bBold, bItalic, bStrike);
	std::string sFontName = std::string(szFontName);
	LPD3DXFONT pD3DXFont = GetFont(sFontName);
	if (pD3DXFont)
	{
		return pD3DXFont;
	}

	IDirect3DDevice8* lpD3DDevice = g_pLTClient->GetD3DDevice();
	if (lpD3DDevice == NULL)
		return LTNULL;
	
    HRESULT hr;
    HDC hDC;
    INT nHeight;
    HFONT hFont;

    hDC = GetDC( LTNULL );
    nHeight = -MulDiv( nFontSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 );
    ReleaseDC( LTNULL, hDC );
	int nWeight = bBold?FW_BOLD:0;//FW_DONTCARE
    hFont = ::CreateFont( nHeight, 0, 0, 0, nWeight, bItalic, bUnderLine, bStrike, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, szFontFace );
    if( hFont == LTNULL )
        return LTNULL;

    hr = D3DXCreateFont( lpD3DDevice, hFont, &pD3DXFont );
    DeleteObject( hFont );

    if( FAILED( hr ) )
		return LTNULL;

	m_mapFont[sFontName] = pD3DXFont;

	return pD3DXFont;
}

void CInterfaceResMgr::OnResetDevice()
{
	for (itrFontMap itr = m_mapFont.begin(); itr != m_mapFont.end();++itr)
	{
		LPD3DXFONT lpDXFont = static_cast<LPD3DXFONT>(itr->second);
		if (lpDXFont)
		{
			lpDXFont->OnResetDevice();
		}
	}
	
	//[MURSUM]===================================================================================
	if( g_pSAD3DTextMgr ) g_pSAD3DTextMgr->OnResetDevice( g_pLTClient->GetD3DDevice() );
	//===========================================================================================
}

void CInterfaceResMgr::OnLostDevice()
{
	for (itrFontMap itr = m_mapFont.begin(); itr != m_mapFont.end();++itr)
	{
		LPD3DXFONT lpDXFont = static_cast<LPD3DXFONT>(itr->second);
		if (lpDXFont)
		{
			lpDXFont->OnLostDevice();
		}
	}

	//[MURSUM]===================================================================================
	if( g_pSAD3DTextMgr ) g_pSAD3DTextMgr->OnLostDevice();
	//===========================================================================================
}

void CInterfaceResMgr::ReleaseDXFont()
{
	for (itrFontMap itr = m_mapFont.begin(); itr != m_mapFont.end();++itr)
	{
		LPD3DXFONT lpDXFont = static_cast<LPD3DXFONT>(itr->second);
		if (lpDXFont)
		{
			lpDXFont->OnResetDevice();
			lpDXFont->Release();
			lpDXFont = LTNULL;
		}
	}
	m_mapFont.clear();
}

LPD3DXFONT CInterfaceResMgr::GetFont(std::string& sFontName)
{
	itrFontMap itr = m_mapFont.find(sFontName);
	if (itr != m_mapFont.end())
	{
		return (LPD3DXFONT)itr->second;
	}
	return LTNULL;
}

LPD3DXFONT CInterfaceResMgr::GetFont(const char *szFontFace, uint8 nFontSize, bool bBold, bool bItalic, bool bStrike)
{
	char szFontName[MAX_PATH];
	MakeFontName(szFontName, szFontFace, nFontSize, bBold, bItalic, bStrike);
	std::string sFontName = std::string(szFontName);
	return GetFont(sFontName);
}

void CInterfaceResMgr::MakeFontName(char* szFontName, const char* szFontFace, uint8 nFontSize, bool bBold, bool bItalic, bool bStrike)
{
	ASSERT(szFontName);
	sprintf(szFontName, "%s%02d%c%c%c", szFontFace, nFontSize, bBold?'B':'N', bItalic?'I':'N', bStrike?'S':'N');
}