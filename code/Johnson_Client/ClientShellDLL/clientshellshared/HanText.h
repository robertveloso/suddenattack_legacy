#ifndef __MURSUM_D3DTEXT_H__
#define __MURSUM_D3DTEXT_H__

//#include <d3d8.h>
//#include <d3dx8tex.h>
#include "../../../Johnson_Libs/StdLith/linklist.h"

class CSAD3DTextMgr;
class CSAD3DText;
class CSAD3DTextEx;

#define D3D_TEXT_NORMAL		1
#define	D3D_TEXT_SHADOW		2
#define	D3D_TEXT_OUTLINE	3

#define SHARED_DC_12_N		0
#define SHARED_DC_12_B		1
#define SHARED_DC_14_B		2
#define SHARED_DC_16_B		3
#define MAX_SHARED_DC		4

#define MAX_SHARED_BIT_W	512
#define MAX_SHARED_BIT_H	256

#define MAX_D3D_TEXT_BUFFER	4096

typedef struct _tagSHAREDDC
{
	HDC				hDC;
	HBITMAP			hBitmap;
	HBITMAP			hOldBitmap;
	unsigned long*	pBitmapBits;
	
	HFONT			hFont;
	HFONT			hOldFont;
} T_SHARED_DC, *T_SHARED_DCptr;

typedef struct _tagTEXTVERTEX
{
	float		x, y, z;
	FLOAT		rhw;
    D3DCOLOR    color; 
    FLOAT       tu, tv;
} T_TEXT_VERTEX, *T_TEXT_VERTEXptr;

#define D3DFVF_TEXT_VERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

static	unsigned long	g_dwMaxTextureWidth		= 0;
static	unsigned long	g_dwMaxTextureHeight	= 0;
static	unsigned long	g_dwTextureCaps			= 0;
static	D3DFORMAT		g_D3DFormat				= D3DFMT_A4R4G4B4;

typedef std::vector<CSAD3DText *> SAD3DTEXT_LIST;
typedef SAD3DTEXT_LIST::iterator SAD3DTEXT_LIST_ITR;

extern CSAD3DTextMgr*	g_pSAD3DTextMgr;

class CSAD3DTextMgr
{
public:
	CSAD3DTextMgr();
	virtual ~CSAD3DTextMgr();
	
	LTBOOL					Init();
	void					Term();

	void					AddD3DText( CSAD3DText* pD3DText );
	void					RemoveD3DText( CSAD3DText* pD3DText );

	void					SetDevice( IDirect3DDevice8* lpDevice ) { m_pd3dDevice = lpDevice; }
	IDirect3DDevice8*		GetDevice() { return m_pd3dDevice; }

	void					OnResetDevice( IDirect3DDevice8* lpDevice );
	void					OnLostDevice();

	unsigned long*			DrawD3DText( int nIndex,
										 const char* lpszText, int nTextLen,
										 unsigned int uFormat,
										 unsigned long dwDrawType,
										 COLORREF rgbTextColor,
										 COLORREF rgbExtColor,
										 RECT* prcBitmap,
										 RECT* prcDrawRect );
	HDC						GetDC( int nIndex );
	int						GetFontIndex( int nFontH, bool bBold );
	
protected:
	LTBOOL					CreateSharedDC( int nWidth, int nHeight );
	void					DeleteSharedDC();

	int						GetFontHeight( int nIndex );
	LTBOOL					IsBold( int nIndex );

	SAD3DTEXT_LIST			m_SAD3DTextList;
	T_SHARED_DC				m_SharedDC[MAX_SHARED_DC];

	IDirect3DDevice8*		m_pd3dDevice;
};

class CSAD3DText
{
public:
	CSAD3DText();
	virtual ~CSAD3DText();

	virtual bool			Render();
	bool					DrawD3DText( DWORD dwIdx, 
										 unsigned int uFormat=DT_LEFT,
										 unsigned long dwDrawType = D3D_TEXT_NORMAL,
										 bool bDotDot = false );
	bool					DrawD3DText( const char* lpszText,
										 int nLen,
										 unsigned int uFormat=DT_LEFT,
										 unsigned long dwDrawType=D3D_TEXT_NORMAL,
										 bool bDotDot=false,
										 int  nDotDotWidth = -1 );

	bool					CreateD3DText( int nWidth, int nHeight, bool bBold=false );

	bool					CreateD3DTextWithFontSize( int nWidth, int nHeight, int nFontHeight, bool bBold=false );

	bool					CreateD3DTextFullInfo( int nWidth, int nHeight,
										   const char* lpszFontname, int nFontHeight, bool bBold=false );
	
	void					Term();

	void					SetTransparent( bool bTransparent );
	void					SetTextColor( COLORREF rgbTextColor, COLORREF rgbExtColor=0 );
	void					SetDrawRegion( int nLeft, int nTop, int nRight, int nBotton );

	void					SetPosition( LTIntPt pt );
	void					SetPosition( float fPosX, float fPosY );
	void					SetPosition( int nPosX, int nPosY );

	void					MoveText( float fPosX, float fPosY );
	void					MoveText( int nPosX, int nPosY );

	const char*				GetText();
	int						GetTextLength();
	int						GetLineLength( int nLineIndex = 0 );
	int						GetTextLength( int nLimit );

	int						GetTextHeight();
	int						GetLineHeight( int nLineIndex = 0 );

	HDC						GetDC() { return m_hDC; }


	virtual void			OnResetDevice(IDirect3DDevice8* lpDevice);
	virtual void			OnLostDevice();
			
protected:
	bool					CreateTextSurface( IDirect3DDevice8* lpDevice, int nWidth, int nHeight );
	
	void					GetDeviceCap();
	void					InitVertices( T_TEXT_VERTEXptr pVertices, float fPosX, float fPosY, float fWidth, float fHeight );
	
	IDirect3DDevice8*		m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER8	m_pVertexBuffer;
	LPDIRECT3DTEXTURE8*		m_ppTexture;

	int						m_nFontIndex;
	int						m_nWidth;
	int						m_nHeight;
	int						m_nTextureCount;

	RECT					m_rcBitmap;
	RECT					m_rcSharedBitmap;
	RECT					m_rcDrawRect;

	int						m_nTextureWidth;
	int						m_nTextureHeight;
	int						m_nTextureCols;
	int						m_nTextureRows;

	float					m_fBasePosX;
	float					m_fBasePosY;

	char*					m_pszTextBuffer;
	int						m_nTextLen;
	COLORREF				m_rgbTextColor;
	COLORREF				m_rgbExtTextColor;
	unsigned int			m_uFormat;
	unsigned long			m_dwDrawType;	

	HDC						m_hDC;
};




class CSAD3DTextEx : public CSAD3DText
{
public:
	CSAD3DTextEx();
	virtual ~CSAD3DTextEx();

	bool					Render();
	void					SetAlpha( unsigned char	nAlpha );
	void					OnResetDevice(IDirect3DDevice8* lpDevice);
	void					OnLostDevice();
	
protected:
	unsigned char			m_nAlpha;
};

#endif