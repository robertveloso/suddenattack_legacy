#include "StdAfx.h"
#include "HanText.h"

#include "GameText.h"
#include "GameString.h"

#define TEXEL_OFFSET -0.5f

CSAD3DTextMgr*	g_pSAD3DTextMgr	= LTNULL;
char			g_szTempBuffer[MAX_D3D_TEXT_BUFFER];

CSAD3DTextMgr::CSAD3DTextMgr()
{
	g_pSAD3DTextMgr	= this;
}

CSAD3DTextMgr::~CSAD3DTextMgr()
{
	Term();
}

LTBOOL CSAD3DTextMgr::Init()
{
	if( LTFALSE == CreateSharedDC( MAX_SHARED_BIT_W, MAX_SHARED_BIT_H ) )
	{
		g_pSAD3DTextMgr	= LTNULL;
		ASSERT( !"LTFALSE == CreateSharedDC()" );
		return LTFALSE;
	}

	return LTTRUE;
}

void CSAD3DTextMgr::Term()
{
	if( g_pSAD3DTextMgr )
	{
		DeleteSharedDC();
		m_SAD3DTextList.clear();
		g_pSAD3DTextMgr	= LTNULL;
	}
}

LTBOOL CSAD3DTextMgr::CreateSharedDC( int nWidth, int nHeight )
{
	int i;
	BITMAPINFO bmi;

	for( i=0 ; i<MAX_SHARED_DC ; ++i )
	{
		m_SharedDC[i].hDC = ::CreateCompatibleDC( NULL );
		if( LTNULL == m_SharedDC[i].hDC ) return LTFALSE;

		memset( &bmi.bmiHeader, 0, sizeof(BITMAPINFOHEADER) );
		bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth       = nWidth;
		bmi.bmiHeader.biHeight      = -nHeight;
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biBitCount    = 32;

		m_SharedDC[i].hBitmap	= CreateDIBSection( m_SharedDC[i].hDC, &bmi, DIB_RGB_COLORS,
                                        (void**)&m_SharedDC[i].pBitmapBits, NULL, 0 );
		if( LTNULL == m_SharedDC[i].hBitmap ) return LTFALSE;
		m_SharedDC[i].hOldBitmap	=(HBITMAP)SelectObject( m_SharedDC[i].hDC, m_SharedDC[i].hBitmap );

		::SetBkMode( m_SharedDC[i].hDC, TRANSPARENT );
		
		m_SharedDC[i].hFont = ::CreateFont( GetFontHeight(i), 0, 0, 0,
								IsBold(i)?FW_BOLD:FW_NORMAL, FALSE, FALSE, FALSE,
								LOCALIZING_CHARSET, 3, 2, 1,
								VARIABLE_PITCH|FF_DONTCARE,
								DEFAULT_FONT_NAME );

		m_SharedDC[i].hOldFont = (HFONT)::SelectObject( m_SharedDC[i].hDC, m_SharedDC[i].hFont );
	}

	return LTTRUE;
}

void CSAD3DTextMgr::DeleteSharedDC()
{
	int i;

	for( i=0 ; i<MAX_SHARED_DC ; ++i )
	{
		if( m_SharedDC[i].hDC && m_SharedDC[i].hFont )
		{
			::SelectObject( m_SharedDC[i].hDC, m_SharedDC[i].hOldFont );
			::DeleteObject( m_SharedDC[i].hFont );
			m_SharedDC[i].hOldFont = NULL;
			m_SharedDC[i].hFont	= NULL;
		}

		if( m_SharedDC[i].hDC && m_SharedDC[i].hBitmap )
		{
			::SelectObject( m_SharedDC[i].hDC, m_SharedDC[i].hOldBitmap );
			::DeleteObject( m_SharedDC[i].hBitmap );
			m_SharedDC[i].hOldBitmap = NULL;
			m_SharedDC[i].hBitmap = NULL;

			::DeleteDC( m_SharedDC[i].hDC );
			m_SharedDC[i].hDC = NULL;
		}
	}
}

int CSAD3DTextMgr::GetFontIndex( int nFontH, bool bBold )
{
	switch( nFontH )
	{
	case 12:
		return bBold ? SHARED_DC_12_B : SHARED_DC_12_N;
	case 14:
		return SHARED_DC_14_B;
	case 16:
		return SHARED_DC_16_B;
	default:
		ASSERT(!"ERROR GetFontIndex");
		break;
	}

	return SHARED_DC_12_N;
}

int CSAD3DTextMgr::GetFontHeight( int nIndex )
{
	switch( nIndex )
	{
	case SHARED_DC_12_N:
	case SHARED_DC_12_B:
		return 12;
	case SHARED_DC_14_B:
		return 14;
	case SHARED_DC_16_B:
		return 16;	
	}

	return 12;
}

LTBOOL CSAD3DTextMgr::IsBold( int nIndex )
{
	switch( nIndex )
	{
	case SHARED_DC_12_B:
	case SHARED_DC_14_B:
	case SHARED_DC_16_B:	
		return LTTRUE;
	}

	return LTFALSE;
}

HDC CSAD3DTextMgr::GetDC( int nIndex )
{
	if( nIndex < 0 || nIndex >= MAX_SHARED_DC ) return LTNULL;
	return m_SharedDC[nIndex].hDC;
}

unsigned long* CSAD3DTextMgr::DrawD3DText( int nIndex,
										   const char* lpszText, int nTextLen,
										   unsigned int uFormat,
										   unsigned long dwDrawType,
										   COLORREF rgbTextColor,
										   COLORREF rgbExtColor,
										   RECT* prcBitmap,
										   RECT* prcDrawRect )
{
	if( !lpszText || !prcBitmap || !prcDrawRect ) return LTNULL;
	
	HBRUSH hBrush = ::CreateSolidBrush( RGB(255,0,255) );	
	::FillRect ( m_SharedDC[nIndex].hDC, prcBitmap, hBrush );
	::DeleteObject( hBrush );
	
	switch( dwDrawType )
	{
	case D3D_TEXT_NORMAL:
		::SetTextColor( m_SharedDC[nIndex].hDC, rgbTextColor );
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		break;
	case D3D_TEXT_SHADOW:		
		::SetTextColor( m_SharedDC[nIndex].hDC, rgbExtColor );
		::OffsetRect( prcDrawRect, 1, 1 );
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		
		::SetTextColor( m_SharedDC[nIndex].hDC, rgbTextColor );
		::OffsetRect( prcDrawRect, -1, -1 );
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		break;
	case D3D_TEXT_OUTLINE:
		::SetTextColor( m_SharedDC[nIndex].hDC, rgbExtColor );
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		::OffsetRect( prcDrawRect, 1, 0 );		
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		::OffsetRect( prcDrawRect, 1, 0 );		
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		::OffsetRect( prcDrawRect, 0, 1 );		
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		::OffsetRect( prcDrawRect, 0, 1 );		
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		::OffsetRect( prcDrawRect, -1, 0 );		
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		::OffsetRect( prcDrawRect, -1, 0 );		
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		::OffsetRect( prcDrawRect, 0, -1 );		
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );

		::SetTextColor( m_SharedDC[nIndex].hDC, rgbTextColor );
		::OffsetRect( prcDrawRect, 1, 0 );			
		::DrawText( m_SharedDC[nIndex].hDC, lpszText, nTextLen, prcDrawRect, uFormat );
		::OffsetRect( prcDrawRect, -1, -1 );			
		break;
	}
	
	return m_SharedDC[nIndex].pBitmapBits;
}

void CSAD3DTextMgr::AddD3DText( CSAD3DText* pD3DText )
{
	if( !pD3DText ) return;
	m_SAD3DTextList.push_back( pD3DText );
}

void CSAD3DTextMgr::RemoveD3DText( CSAD3DText* pD3DText )
{
	if( !pD3DText ) return;

	SAD3DTEXT_LIST_ITR iter = m_SAD3DTextList.begin();

	while( iter != m_SAD3DTextList.end() && (*iter) != pD3DText )
		iter++;

	if( iter != m_SAD3DTextList.end() )
	{
		m_SAD3DTextList.erase(iter);
	}
}


void CSAD3DTextMgr::OnResetDevice( IDirect3DDevice8* lpDevice )
{
	this->SetDevice( lpDevice );

	if( m_SAD3DTextList.empty() ) return;

	for( uint16 i=0; i < m_SAD3DTextList.size(); ++i )
	{
		m_SAD3DTextList[i]->OnResetDevice( lpDevice );
	}	
}

void CSAD3DTextMgr::OnLostDevice()
{
	if( m_SAD3DTextList.empty() ) return;

	for( uint16 i=0; i < m_SAD3DTextList.size(); ++i )
	{
		m_SAD3DTextList[i]->OnLostDevice();
	}	
}



CSAD3DText::CSAD3DText()
:m_pd3dDevice(NULL)
,m_pVertexBuffer(NULL)
,m_ppTexture(NULL)
,m_nWidth(0)
,m_nHeight(0)
,m_nTextureCount(0)
,m_nTextureWidth(0)
,m_nTextureHeight(0)
,m_nTextureCols(0)
,m_nTextureRows(0)
,m_fBasePosX(0.0f)
,m_fBasePosY(0.0f)
,m_pszTextBuffer(NULL)
,m_nTextLen(0)
,m_rgbTextColor(RGB(0,0,0))
,m_rgbExtTextColor(RGB(255,255,0))
,m_uFormat(DT_LEFT) 
,m_nFontIndex(-1)
,m_hDC(LTNULL)
{
	if( g_pSAD3DTextMgr ) 
		g_pSAD3DTextMgr->AddD3DText( this );
}

CSAD3DText::~CSAD3DText()
{
	this->Term();
}

void CSAD3DText::Term()
{
	if( m_ppTexture )
	{
		for( int i=0 ; i<m_nTextureCount ; ++i )
		{
			if( m_ppTexture[i] )
			{
				m_ppTexture[i]->Release();
				m_ppTexture[i]	= NULL;
			}		
		}		
	
		delete [] m_ppTexture;
		m_ppTexture	= NULL;
	}

	if( m_pVertexBuffer )
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer	= NULL;
	}

	SAFE_DELETE_ARRAY( m_pszTextBuffer );

	m_nTextureCount		= 0;

	if( g_pSAD3DTextMgr )
		g_pSAD3DTextMgr->RemoveD3DText( this );	
}

bool CSAD3DText::Render()
{
	if( !m_pVertexBuffer || !m_ppTexture || !m_pszTextBuffer || m_nTextLen <= 0 ) return true;

	int	i;

	m_pd3dDevice->SetVertexShader( D3DFVF_TEXT_VERTEX );
	m_pd3dDevice->SetStreamSource( 0, m_pVertexBuffer, sizeof(T_TEXT_VERTEX) );

	for( i=0 ; i<m_nTextureCount ; ++i )
	{
		m_pd3dDevice->SetTexture( 0, m_ppTexture[i] );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, i*4 , 2 );
		m_pd3dDevice->SetTexture( 0, NULL );
	}

	return true;
}

bool CSAD3DText::DrawD3DText( DWORD dwIdx, 
							  unsigned int uFormat,
							  unsigned long dwDrawType,
							  bool bDotDot )
{
	const char* lpszText = NF_GetMsg(dwIdx);

	return DrawD3DText( lpszText, strlen(lpszText), uFormat, dwDrawType, bDotDot );
}

bool CSAD3DText::DrawD3DText( const char* lpszText,
							  int nLen,
							  unsigned int uFormat,
							  unsigned long dwDrawType,
							  bool bDotDot,
							  int  nDotDotWidth )

{
	RECT	rcSource;
	RECT	rcTemp;
	HRESULT	hr;
	int		i, j;
	int		n=0;
	unsigned long* pBitmap	= LTNULL;
	int nWidth = (nDotDotWidth>0) ? nDotDotWidth : m_nWidth;

	if( !g_pSAD3DTextMgr ) return false;
	
	if( lpszText )
	{
		m_uFormat		= uFormat;
		m_dwDrawType	= dwDrawType;

		SAFE_DELETE_ARRAY( m_pszTextBuffer );

		m_nTextLen		= nLen;
		m_pszTextBuffer = new char[m_nTextLen+1];
		strncpy( m_pszTextBuffer, lpszText, m_nTextLen );
		m_pszTextBuffer[m_nTextLen] = '\0';	
		
		if( bDotDot )
		{
			SIZE tTextSize;
			::GetTextExtentPoint32( m_hDC, m_pszTextBuffer, m_nTextLen, &tTextSize );
			if( tTextSize.cx > nWidth )
			{
				strcpy( g_szTempBuffer, m_pszTextBuffer );

				char* pEnd = g_szTempBuffer;
    
				while( *pEnd != '\0' )
				{
					pEnd = ::CharNext( pEnd );	
					GetTextExtentPoint32( m_hDC, g_szTempBuffer, pEnd-g_szTempBuffer, &tTextSize );
					if( tTextSize.cx >= nWidth-4 ) 
					{
						pEnd = ::CharPrev( g_szTempBuffer, pEnd );
						*(pEnd) = '.';
						*(pEnd+1) = '.';				
						*(pEnd+2) = '\0';
						break;					
					}
				}
				
				m_nTextLen	= strlen(g_szTempBuffer);
				SAFE_DELETE_ARRAY( m_pszTextBuffer );
				m_pszTextBuffer = new char[m_nTextLen+1];
				strncpy( m_pszTextBuffer, g_szTempBuffer, m_nTextLen );
				m_pszTextBuffer[m_nTextLen] = '\0';
			}
		}
	}

	if( m_nTextLen <= 0 ) return false;

	//[MURSUM] 포커스가 없는 경우는 여기서 그만~~
	if( !m_ppTexture || m_nFontIndex<0 ) return true;
		
	pBitmap = g_pSAD3DTextMgr->DrawD3DText( m_nFontIndex, 
											m_pszTextBuffer, m_nTextLen,											
											m_uFormat,
											m_dwDrawType,
											m_rgbTextColor,
											m_rgbExtTextColor,
											&m_rcSharedBitmap,
											&m_rcDrawRect );
	if( !pBitmap ) return false;

	for( j=0 ; j<m_nTextureRows ; ++j )
	{
		for( i=0 ; i<m_nTextureCols ; ++i )
		{
			::SetRect( &rcTemp,
					   m_nTextureWidth*i, m_nTextureHeight*j,
					   m_nTextureWidth*(i+1), m_nTextureHeight*(j+1) );
			::IntersectRect( &rcSource, &rcTemp, &m_rcSharedBitmap );
			LPDIRECT3DSURFACE8 pTextSurface;

			if( m_ppTexture[n] == LTNULL )
			{
				++n;
				continue;
			}

			m_ppTexture[n]->GetSurfaceLevel( 0, &pTextSurface );

			hr = D3DXLoadSurfaceFromMemory( pTextSurface,
										    NULL,
											NULL,
											pBitmap,
											D3DFMT_X8R8G8B8,											
											m_rcSharedBitmap.right*4,
											NULL,
											&rcSource,
											D3DX_FILTER_NONE,
											0xFFFF00FF );
			if( FAILED(hr) )
			{
				pTextSurface->Release();
				return false;
			}

			pTextSurface->Release();
			++n;
		}
	}
	
	return true;
}

bool CSAD3DText::CreateD3DText( int nWidth, int nHeight, bool bBold )
{
	return CreateD3DTextFullInfo( nWidth, nHeight, DEFAULT_FONT_NAME, 12, bBold );
}

bool CSAD3DText::CreateD3DTextWithFontSize( int nWidth, int nHeight, int nFontHeight, bool bBold )
{
	return CreateD3DTextFullInfo( nWidth, nHeight, DEFAULT_FONT_NAME, nFontHeight, bBold );
}

bool CSAD3DText::CreateD3DTextFullInfo( int nWidth, int nHeight,
							  const char* lpszFontname, int nFontHeight, bool bBold )
{
	if( !g_pSAD3DTextMgr ) return false;
	
	m_nFontIndex = g_pSAD3DTextMgr->GetFontIndex( nFontHeight, bBold );

	ASSERT( nWidth<=MAX_SHARED_BIT_W);
	ASSERT( nHeight<=MAX_SHARED_BIT_H);
	
	m_hDC = g_pSAD3DTextMgr->GetDC(m_nFontIndex);
	::SetRect( &m_rcBitmap, 0, 0, nWidth, nHeight );
	::SetRect( &m_rcSharedBitmap, 0, 0, MAX_SHARED_BIT_W, MAX_SHARED_BIT_H );
	::SetRect( &m_rcDrawRect, 0, 0, nWidth, nHeight );
	return (this->CreateTextSurface( g_pSAD3DTextMgr->GetDevice(), nWidth, nHeight ));
}

bool CSAD3DText::CreateTextSurface( IDirect3DDevice8* lpDevice, int nWidth, int nHeight )
{
	int				i, j;
	unsigned int	nTexW, nTexH;
	unsigned long	dwMaxTexW, dwMaxTexH;

	m_nWidth		= nWidth;
	m_nHeight		= nHeight;
	m_pd3dDevice	= lpDevice;

	if( 0 == g_dwMaxTextureWidth && 0 == g_dwMaxTextureHeight && 0 == g_dwTextureCaps )
	{
		this->GetDeviceCap();
	}

	dwMaxTexW	= g_dwMaxTextureWidth;
	dwMaxTexH	= g_dwMaxTextureHeight;
	
	if( g_dwTextureCaps&D3DPTEXTURECAPS_POW2 )
	{
		for( nTexW=1;  m_nWidth>(int)nTexW; nTexW<<=1 );
		for( nTexH=1; m_nHeight>(int)nTexH; nTexH<<=1 );
	}

	m_nTextureWidth = (nTexW<dwMaxTexW) ? nTexW : dwMaxTexW;
	m_nTextureHeight = (nTexH<dwMaxTexH) ? nTexH : dwMaxTexH;

	if( g_dwTextureCaps&D3DPTEXTURECAPS_SQUAREONLY )
	{
		if( m_nTextureWidth > m_nTextureHeight )
		{
			m_nTextureHeight = m_nTextureWidth;
		} 
		else 
		{
			m_nTextureWidth  = m_nTextureHeight;
		}
	}

	m_nTextureCols = m_nWidth/m_nTextureWidth;
	if( (m_nWidth%m_nTextureWidth) > 0 ) ++m_nTextureCols;

	m_nTextureRows = m_nHeight/m_nTextureHeight;
	if( (m_nHeight%m_nTextureHeight) > 0 ) ++m_nTextureRows;

	m_nTextureCount = m_nTextureCols*m_nTextureRows;

	m_ppTexture	= new LPDIRECT3DTEXTURE8[m_nTextureCount];
	memset( m_ppTexture, 0, sizeof(LPDIRECT3DTEXTURE8)*m_nTextureCount );


	HRESULT hResult = D3D_OK;

	hResult = m_pd3dDevice->CreateVertexBuffer( m_nTextureCount*4*sizeof(T_TEXT_VERTEX),
                                                0, D3DFVF_TEXT_VERTEX,
                                                D3DPOOL_MANAGED,
												&m_pVertexBuffer );
	if ( FAILED(hResult) )
    {
#ifdef _DEBUG
		switch ( hResult )
		{
		case D3DERR_INVALIDCALL:		ASSERT( !"D3DERR_INVALIDCALL" );		break;
		case D3DERR_OUTOFVIDEOMEMORY:	ASSERT( !"D3DERR_OUTOFVIDEOMEMORY" );	break;
		case E_OUTOFMEMORY:				ASSERT( !"E_OUTOFMEMORY" );				break;
		default:						ASSERT( !"m_pd3dDevice->CreateVertexBuffer() Failed!!!" );	break;
		}
#endif
		Term();
        return false;
    }

	T_TEXT_VERTEXptr	pVertices;
    if( FAILED( m_pVertexBuffer->Lock( 0, 
									   m_nTextureCount*4*sizeof(T_TEXT_VERTEX),
									   (unsigned char**)&pVertices, 0 ) ) )
	{
#ifdef _DEBUG
		ASSERT( !"m_pVertexBuffer->Lock() Failed!!" );	
#endif
        return false;
	}

	int nLeft,nTop,nRight,nBottom;
	int nIndex = 0;

	for( j=0 ; j<m_nTextureRows ; ++j )
	{
		for( i=0 ; i<m_nTextureCols ; ++i )
		{
			nLeft	= i*m_nTextureWidth;
			nTop	= j*m_nTextureHeight;
			nRight	= nLeft+m_nTextureWidth;
			nBottom	= nTop+m_nTextureHeight;

			hResult = m_pd3dDevice->CreateTexture(	nRight-nLeft, nBottom-nTop, 1, 0,	
													g_D3DFormat,
													D3DPOOL_MANAGED,
													&m_ppTexture[nIndex] );
			if ( FAILED(hResult) )
			{
				g_D3DFormat = D3DFMT_A8R8G8B8;
				hResult = m_pd3dDevice->CreateTexture(	nRight-nLeft, nBottom-nTop, 1, 0,	
														g_D3DFormat,
														D3DPOOL_MANAGED,
														&m_ppTexture[nIndex] );
				if ( FAILED(hResult) )
				{
					g_D3DFormat = D3DFMT_A1R5G5B5;
					hResult = m_pd3dDevice->CreateTexture(	nRight-nLeft, nBottom-nTop, 1, 0,	
															g_D3DFormat,
															D3DPOOL_MANAGED,
															&m_ppTexture[nIndex] );
					if ( FAILED(hResult) )
					{
						g_D3DFormat = D3DFMT_R5G6B5;
						hResult = m_pd3dDevice->CreateTexture(	nRight-nLeft, nBottom-nTop, 1, 0,	
																g_D3DFormat,
																D3DPOOL_MANAGED,
																&m_ppTexture[nIndex] );
						if( FAILED(hResult) )
						{
							Term();
							return false;
						}
					}
				}				
			}
			
			this->InitVertices( pVertices+(nIndex*4),
								float(nLeft), float(nTop), float(nRight-nLeft), float(nBottom-nTop) );
			++nIndex;
		}
	}

	m_pVertexBuffer->Unlock();

	return true;
}

void CSAD3DText::SetTextColor( COLORREF rgbTextColor, COLORREF rgbExtColor )
{
	if( m_rgbTextColor == rgbTextColor && m_rgbExtTextColor == rgbExtColor ) return;

	m_rgbTextColor = rgbTextColor;
	m_rgbExtTextColor = rgbExtColor;

	this->DrawD3DText( LTNULL, 0, 0, 0 );
}

void CSAD3DText::SetTransparent( bool bTransparent )
{
}

void CSAD3DText::SetPosition( LTIntPt pt )
{
	this->SetPosition( pt.x, pt.y );
}

void CSAD3DText::SetPosition( int nPosX, int nPosY )
{
	this->SetPosition( (float)nPosX, (float)nPosY );
}

void CSAD3DText::SetPosition( float fPosX, float fPosY )
{
	if( NULL == m_pVertexBuffer ) return;
	float	fOffsetX, fOffsetY;
	
	int					i;
	T_TEXT_VERTEXptr	pVertices;
    if( FAILED( m_pVertexBuffer->Lock( 0,
									   m_nTextureCount*4*sizeof(T_TEXT_VERTEX),
									   (unsigned char**)&pVertices, 0 ) ) ) return;

	fOffsetX	= fPosX + 0.5f - pVertices->x;
	fOffsetY	= fPosY + 0.5f - pVertices->y;

	m_fBasePosX	= fPosX;
	m_fBasePosY	= fPosY;

	for( i=0 ; i<m_nTextureCount*4 ; ++i )
	{
		pVertices->x	+= fOffsetX;
		pVertices->y	+= fOffsetY;
		++pVertices;
	}

	m_pVertexBuffer->Unlock();
}

void CSAD3DText::MoveText( int nPosX, int nPosY )
{
	this->MoveText( (float)nPosX, (float)nPosY );
}

void CSAD3DText::MoveText( float fPosX, float fPosY )
{
	if( NULL == m_pVertexBuffer ) return;
	
	int					i;
	T_TEXT_VERTEXptr	pVertices;
    if( FAILED( m_pVertexBuffer->Lock( 0,
									   m_nTextureCount*4*sizeof(T_TEXT_VERTEX),
									   (unsigned char**)&pVertices, 0 ) ) ) return;

	m_fBasePosX	= pVertices->x + fPosX;
	m_fBasePosY	= pVertices->y + fPosY;

	for( i=0 ; i<m_nTextureCount*4 ; ++i )
	{
		pVertices->x	+= fPosX;
		pVertices->y	+= fPosY;
		++pVertices;
	}

	m_pVertexBuffer->Unlock();
}

void CSAD3DText::SetDrawRegion( int nLeft, int nTop, int nRight, int nBotton )
{
	::SetRect( &m_rcDrawRect, m_rcBitmap.left+nLeft, m_rcBitmap.top+nTop,
							  m_rcBitmap.right+nRight, m_rcBitmap.bottom+nBotton );
}

void CSAD3DText::GetDeviceCap()
{
	D3DCAPS8 d3dCaps8;
	m_pd3dDevice->GetDeviceCaps( &d3dCaps8 );

	g_dwMaxTextureWidth		= 256;//d3dCaps8.MaxTextureWidth;
	g_dwMaxTextureHeight	= 256;//d3dCaps8.MaxTextureHeight;
	g_dwTextureCaps			= d3dCaps8.TextureCaps;
}

void CSAD3DText::InitVertices( T_TEXT_VERTEXptr pVertices, float fPosX, float fPosY, float fWidth, float fHeight )
{
	T_TEXT_VERTEXptr pVerticesPtr = pVertices;
		
	pVerticesPtr->x = fPosX+TEXEL_OFFSET;
	pVerticesPtr->y = fPosY+TEXEL_OFFSET;
	pVerticesPtr->z = 0.0f;
	pVerticesPtr->rhw = 1.0f;
	pVerticesPtr->color = 0xFFFFFFFF;
	pVerticesPtr->tu = 0.0f;
	pVerticesPtr->tv = 0.0f;

	++pVerticesPtr;

	pVerticesPtr->x = fPosX+fWidth+TEXEL_OFFSET;
	pVerticesPtr->y = fPosY+TEXEL_OFFSET;
	pVerticesPtr->z = 0.0f;
	pVerticesPtr->rhw = 1.0f;
	pVerticesPtr->color = 0xFFFFFFFF;
	pVerticesPtr->tu = fWidth/(float)m_nTextureWidth;
	pVerticesPtr->tv = 0.0f;

	++pVerticesPtr;

	pVerticesPtr->x = fPosX+fWidth + TEXEL_OFFSET;
	pVerticesPtr->y = fPosY+fHeight + TEXEL_OFFSET;
	pVerticesPtr->z = 0.0f;
	pVerticesPtr->rhw = 1.0f;
	pVerticesPtr->color = 0xFFFFFFFF;
	pVerticesPtr->tu = fWidth/(float)m_nTextureWidth;
	pVerticesPtr->tv = fHeight/(float)m_nTextureHeight;

	++pVerticesPtr;

	pVerticesPtr->x = fPosX+TEXEL_OFFSET;
	pVerticesPtr->y = fPosY+fHeight +TEXEL_OFFSET;
	pVerticesPtr->z = 0.0f;
	pVerticesPtr->rhw = 1.0f;
	pVerticesPtr->color = 0xFFFFFFFF;	
	pVerticesPtr->tu = 0.0f;	
	pVerticesPtr->tv = fHeight/(float)m_nTextureHeight;	
}

const char* CSAD3DText::GetText()
{
	return m_pszTextBuffer ? m_pszTextBuffer : "";
}

int CSAD3DText::GetTextLength()
{
	if( NULL == m_hDC || NULL == m_pszTextBuffer ) return 0;

	SIZE tTextSize;
	::GetTextExtentPoint32( m_hDC, m_pszTextBuffer, strlen(m_pszTextBuffer), &tTextSize);

	return (int)tTextSize.cx;
}

int CSAD3DText::GetTextLength( int nLimit )
{
	if( NULL == m_hDC || NULL == m_pszTextBuffer ) return 0;

	if( nLimit > (int)strlen(m_pszTextBuffer) ) nLimit = (int)strlen(m_pszTextBuffer);
	SIZE tTextSize;
	::GetTextExtentPoint32( m_hDC, m_pszTextBuffer, nLimit, &tTextSize);

	return (int)tTextSize.cx;
}

int CSAD3DText::GetLineLength( int nLineIndex )
{
	int			nCurLine	= 0;
	char*		lpszToken	= LTNULL;
	if( NULL == m_hDC || NULL == m_pszTextBuffer ) return 0;

	strcpy( g_szTempBuffer, m_pszTextBuffer );

	lpszToken = strtok( g_szTempBuffer, TEXT("\r\n") );

	while( LTNULL != lpszToken )
	{
		if( nCurLine == nLineIndex )
		{
			SIZE tTextSize;
			::GetTextExtentPoint32( m_hDC, lpszToken, strlen(lpszToken), &tTextSize);
			return (int)tTextSize.cx;
		}

		++nCurLine;
		lpszToken = strtok( NULL, TEXT("\r\n") );
	}

	return -1;
}

// 지정해준 라인의 높이 반환
int CSAD3DText::GetLineHeight( int nLineIndex /*= 0*/ )
{
	int		nTotHeight	= 0;
	int		nCurLine	= 0;
	char*	lpszToken	= LTNULL;
	bool	bEnd		= LTFALSE;
	if( NULL == m_hDC || NULL == m_pszTextBuffer ) return 0;
	
	strcpy( g_szTempBuffer, m_pszTextBuffer );
	
	char szOutput[256] = {0,};
	lpszToken = __strtok( g_szTempBuffer, TEXT("\r\n"), szOutput, bEnd );
	
	while( LTNULL != lpszToken )
	{
		if( nCurLine == nLineIndex )
		{
			SIZE tTextSize;
			::GetTextExtentPoint32( m_hDC, lpszToken, strlen(lpszToken), &tTextSize);
			return (int)tTextSize.cy;
		}

		if ( bEnd )
		{
			break;
		}
		
		++nCurLine;
		lpszToken = __strtok( lpszToken, TEXT("\r\n"), szOutput, bEnd );
	}
	
	return -1;
}

// 전체 텍스트의 높이 반환
int CSAD3DText::GetTextHeight()
{
	int		nTotHeight	= 0;
	int		nCurLine	= 0;
	char*	lpszToken	= LTNULL;
	bool	bEnd		= LTFALSE;
	if( NULL == m_hDC || NULL == m_pszTextBuffer ) return 0;
	
	strcpy( g_szTempBuffer, m_pszTextBuffer );
	
	char szOutput[256] = {0,};
	lpszToken = __strtok( g_szTempBuffer, TEXT("\r\n"), szOutput, bEnd );

	while( LTNULL != lpszToken )
	{
		SIZE tTextSize;
		::GetTextExtentPoint32( m_hDC, lpszToken, strlen(lpszToken), &tTextSize);
		nTotHeight += (int)tTextSize.cy;
		
		if ( bEnd )
		{
			break;
		}
		
		++nCurLine;
		lpszToken = __strtok( lpszToken, TEXT("\r\n"), szOutput, bEnd );
	}
	
	return nTotHeight;
}

void CSAD3DText::OnLostDevice()
{
	if( m_ppTexture )
	{
		for( int i=0 ; i<m_nTextureCount ; ++i )
		{
			if( m_ppTexture[i] )
			{
				m_ppTexture[i]->Release();
				m_ppTexture[i] = NULL;
			}		
		}		
	
		delete [] m_ppTexture;
		m_ppTexture	= NULL;
	}

	if( m_pVertexBuffer )
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	m_nTextureCount = 0;
}

void CSAD3DText::OnResetDevice( IDirect3DDevice8* lpDevice )
{
	if( !g_pSAD3DTextMgr ) return;

	this->CreateTextSurface( lpDevice, m_nWidth, m_nHeight );
	this->DrawD3DText( LTNULL, 0, 0, 0 );

	if( m_fBasePosX != 0 && m_fBasePosY != 0 ) this->SetPosition( m_fBasePosX, m_fBasePosY );
}



CSAD3DTextEx::CSAD3DTextEx() 
:m_nAlpha(255)
,CSAD3DText() {}

CSAD3DTextEx::~CSAD3DTextEx()
{
	//CSAD3DText::Term();
}

bool CSAD3DTextEx::Render()
{
	if( !m_pd3dDevice || !m_pVertexBuffer || !m_ppTexture || !m_pszTextBuffer || m_nTextLen <= 0 ) return true;

	int		i;
	
	m_pd3dDevice->SetVertexShader( D3DFVF_TEXT_VERTEX );
	m_pd3dDevice->SetStreamSource( 0, m_pVertexBuffer, sizeof(T_TEXT_VERTEX) );
	
	for( i=0 ; i<m_nTextureCount ; ++i )
	{
		m_pd3dDevice->SetTexture( 0, m_ppTexture[i] );		
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, i*4 , 2 );
		m_pd3dDevice->SetTexture( 0, NULL );
	}

	return true;
}

void CSAD3DTextEx::SetAlpha( unsigned char nAlpha )
{
	if( NULL == m_pVertexBuffer ) return;
	
	int					i;
	T_TEXT_VERTEXptr	pVertices;
    if( FAILED( m_pVertexBuffer->Lock( 0,
									   m_nTextureCount*4*sizeof(T_TEXT_VERTEX),
									   (unsigned char**)&pVertices, 0 ) ) ) return;

	unsigned long dwColorOP = m_nAlpha = nAlpha;
	dwColorOP <<= 24;

	for( i=0 ; i<m_nTextureCount*4 ; ++i )
	{
		pVertices->color	&= 0x00FFFFFF;
		pVertices->color	|= dwColorOP;
		++pVertices;
	}

	m_pVertexBuffer->Unlock();
}

void CSAD3DTextEx::OnLostDevice()
{
	CSAD3DText::OnLostDevice();
}

void CSAD3DTextEx::OnResetDevice( IDirect3DDevice8* lpDevice )
{
	CSAD3DText::OnResetDevice( lpDevice );

	if( m_nAlpha != 255 ) 
		this->SetAlpha( m_nAlpha );
}