// ----------------------------------------------------------------------- //
//
// MODULE  : InterfaceResMgr.h
//
// PURPOSE : Manager for resources associated with the interface
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#if !defined(_INTERFACERESMGR_H_)
#define _INTERFACERESMGR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LTGUIMgr.h"
#include "InterfaceSurfMgr.h"
#include "iltfontmanager.h"

struct ID3DXFont;
typedef ID3DXFont* LPD3DXFONT;

class CGameClientShell;
class CInterfaceResMgr;
extern CInterfaceResMgr* g_pInterfaceResMgr;

typedef std::vector<CUIFont*> FontArray;
typedef std::set<HTEXTURE> TextureSet;

const int kMaxSoundFilename = 64;

#include "HanText.h"

class CInterfaceResMgr
{
public:
	void OnLostDevice();
	void OnResetDevice();
	CInterfaceResMgr();
	virtual ~CInterfaceResMgr();

    LTBOOL              Init();
	void				Term();
	HSURFACE			GetSharedSurface(char *lpszPath)	{ return m_InterfaceSurfMgr.GetSurface(lpszPath); }
	void				FreeSharedSurface(char *lpszPath)	{ m_InterfaceSurfMgr.FreeSurface(lpszPath); }
	void				FreeSharedSurface(HSURFACE hSurf)	{ m_InterfaceSurfMgr.FreeSurface(hSurf); }

	HTEXTURE			GetTexture(const char *szTexName);

	HSURFACE			GetSurfaceCursor();

	void				ScreenDimsChanged();


    LTBOOL              IsEnglish()                         { return m_bEnglish; }

	void				DrawScreen();


	void				ConvertScreenRect(LTRect &rect);
	void				ConvertScreenPos(LTIntPt &pos) {ConvertScreenPos(pos.x,pos.y);}
	void				ConvertScreenPos(int &x, int &y);
    LTFLOAT             GetXRatio()                         {return m_fXRatio;}
    LTFLOAT             GetYRatio()                         {return m_fYRatio;}
	void				SetXRatio( LTFLOAT fXRatio )		{m_fXRatio=fXRatio;}
	void				SetYRatio( LTFLOAT fYRatio )		{m_fYRatio=fYRatio;}

    uint32              GetScreenWidth();
    uint32              GetScreenHeight();


	//call Setup() before entering a 2-d state (screen)
	//call Clean() before returning to the game
    LTBOOL              Setup();
	void				Clean();

	
	// Added By Bestline ==>
	// Time : 2004-06-07 ¿ÀÀü 11:09:06
	//CHanText*	CreateHanText();
	// Added By Bestline <==

	// added by IN'T ===>
	LPD3DXFONT CreateDXFont(const char *szFontFace, uint8 nFontSize, uint8 nFontStyle = 0x0);
	LPD3DXFONT CreateDXFont(const char* szFontFace, uint8 nFontSize, bool bBold, bool bItalic, bool bUnderLine, bool bStrike);
	LPD3DXFONT GetFont(std::string& sFontName);
	LPD3DXFONT GetFont(const char* szFontFace, uint8 nFontSize, bool bBold, bool bItalic, bool bStrike);
	void MakeFontName(char* szFontName, const char* szFontFace, uint8 nFontSize, bool bBold, bool bItalic, bool bStrike);
	// added by IN'T <===

protected:

	// More initialization
    CUIFont*			CreateFont(char const* pszFontFile, char const* pszFontFace, uint8 ptSize);


	// added by IN'T ===>
	void ReleaseDXFont();
	struct c_str_compare
	{
		bool operator ()( const std::string& l , const std::string& r ) const
		{
			return ( ::stricmp( l.c_str() , r.c_str () ) < 0 );
		}
	};
	typedef std::map<std::string, LPD3DXFONT, c_str_compare> CDXFontMap;
	typedef CDXFontMap::iterator		itrFontMap;
	CDXFontMap	m_mapFont;
	// added by IN'T <===

protected:
    LTBOOL				m_bEnglish;             // True if the resource file has English as the specified language

	TextureSet			m_TextureSet;			// list of used textures

	HSURFACE			m_hSurfCursor;			// The software cursor surface

	int					m_nYesVKeyCode;			// The virtual key code for "yes" responses
	int					m_nNoVKeyCode;			// The virtual key code for "no" responses

	CInterfaceSurfMgr	m_InterfaceSurfMgr;	// Used to share title graphics

    LTFLOAT              m_fXRatio;
    LTFLOAT              m_fYRatio;
    uint32              m_dwScreenWidth;
    uint32              m_dwScreenHeight;

	char				m_szSoundSelect[kMaxSoundFilename];
	char				m_szSoundPageChange[kMaxSoundFilename];
	char				m_szSoundUnselectable[kMaxSoundFilename];
	char				m_szSoundArrowUp[kMaxSoundFilename];
	char				m_szSoundArrowDown[kMaxSoundFilename];
	char				m_szSoundArrowLeft[kMaxSoundFilename];
	char				m_szSoundArrowRight[kMaxSoundFilename];
	char				m_szSoundObjAdd[kMaxSoundFilename];
	char				m_szSoundObjRemove[kMaxSoundFilename];
	char				m_szSoundObjComplete[kMaxSoundFilename];
};

#define TERMSHAREDSURF(surf) if(surf) { g_pInterfaceResMgr->FreeSharedSurface(surf); surf = NULL; }

inline uint32 CInterfaceResMgr::GetScreenWidth()
{
	if (m_dwScreenWidth <= 0)
		ScreenDimsChanged();
	return m_dwScreenWidth;
}
inline uint32 CInterfaceResMgr::GetScreenHeight()
{
	if (m_dwScreenHeight <= 0)
		ScreenDimsChanged();
	return m_dwScreenHeight;
}


#endif // !defined(_INTERFACERESMGR_H_)