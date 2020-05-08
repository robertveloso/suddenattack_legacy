#include "StdAfx.h"
#include "TextScroll.h"

CTextScroll::CTextScroll()
{
	m_pText_1 = LTNULL;
	m_pText_2 = LTNULL;
	m_pText_3 = LTNULL;
	m_pText_4 = LTNULL;
}

CTextScroll::~CTextScroll()
{
	Destroy();
}

LTBOOL CTextScroll::Create( float fPosX, float fPosY, float fWidth, float fHeight )
{
	m_fPosX		= fPosX;
	m_fPosY		= fPosY;
	m_fWidth	= fWidth;
	m_fHeight	= fHeight;

	Clear();
	
	m_pText_1 = new CSAD3DText();
	m_pText_1->CreateD3DText( 512, 16 );
	m_pText_1->SetTransparent(true);

	m_pText_2 = new CSAD3DText();
	m_pText_2->CreateD3DText( 512, 16 );
	m_pText_2->SetTransparent(true);

	m_pText_3 = new CSAD3DText();
	m_pText_3->CreateD3DText( 512, 16 );
	m_pText_3->SetTransparent(true);

	m_pText_4 = new CSAD3DText();
	m_pText_4->CreateD3DText( 512, 16 );
	m_pText_4->SetTransparent(true);

	return LTTRUE;
}

void CTextScroll::Destroy()
{
	Clear();

	SAFE_DELETE( m_pText_1 );
	SAFE_DELETE( m_pText_2 );
	SAFE_DELETE( m_pText_3 );
	SAFE_DELETE( m_pText_4 );
}

void CTextScroll::Clear()
{
	InitPos();
	m_nLastIndex	= -1;

	if( false == m_TextList.empty() )
	{
		T_TEXT_SCROLLptr pText;
		for( TEXT_SCROLL_LIST_ITR itr=m_TextList.begin() ; itr != m_TextList.end() ; ++itr )
		{
			pText = (T_TEXT_SCROLLptr)(*itr);
			SAFE_DELETE( pText );
		}

		m_TextList.clear();
	}
}

void CTextScroll::InitPos()
{
	m_nCurrentIndex = 0;	
	m_fPosX_1		= -1.0f;
	m_fEndPosX		= 0.0f;
}

void CTextScroll::AddText( const char* lpszText_1, COLORREF rgb_1,
						   const char* lpszText_2, COLORREF rgb_2,
						   const char* lpszText_3, COLORREF rgb_3, 
						   const char* lpszText_4, COLORREF rgb_4 )
{
	if( !lpszText_1 ) return;

	T_TEXT_SCROLLptr pText = new T_TEXT_SCROLL;
	strcpy( pText->szText1, lpszText_1 );
	pText->rgb_1 = rgb_1;

	if( lpszText_2 )
	{
		strcpy( pText->szText2, lpszText_2 );
		pText->rgb_2 = rgb_2;
	}
	else
	{
		pText->szText2[0] = LTNULL;
	}

	if( lpszText_3 )
	{
		strcpy( pText->szText3, lpszText_3 );
		pText->rgb_3 = rgb_3;
	}
	else
	{
		pText->szText3[0] = LTNULL;
	}

	if( lpszText_4 )
	{
		strcpy( pText->szText4, lpszText_4 );
		pText->rgb_4 = rgb_4;
	}
	else
	{
		pText->szText4[0] = LTNULL;
	}

	AddText( pText );
}

void CTextScroll::AddText( T_TEXT_SCROLLptr pText )
{
	if( pText ) 
	{
		m_TextList.push_back( pText );
		++m_nLastIndex;
	}
}

void CTextScroll::Update()
{
	if( m_nLastIndex < 0 ) return;

	if( m_fPosX_1 < m_fEndPosX )
	{
		if( m_nCurrentIndex >= m_nLastIndex )
		{
			m_nCurrentIndex = 0;
		}
		else
		{
			++m_nCurrentIndex;
		}
		
		SetNewText( m_nCurrentIndex );
	}
	else
	{
		float	fDelta = 0.25;//g_pLTClient->GetFrameTime()*60.0f;
		m_fPosX_1 -= fDelta;
		m_fPosX_2 -= fDelta;
		m_fPosX_3 -= fDelta;
		m_fPosX_4 -= fDelta;

		m_pText_1->SetPosition( m_fPosX_1, m_fPosY );
		m_pText_2->SetPosition( m_fPosX_2, m_fPosY );
		m_pText_3->SetPosition( m_fPosX_3, m_fPosY );
		m_pText_4->SetPosition( m_fPosX_4, m_fPosY );
	}
}

void CTextScroll::Render()
{
	if( m_nLastIndex < 0 ) return;

	m_pText_1->Render();
	m_pText_2->Render();
	m_pText_3->Render();
	m_pText_4->Render();
}

void CTextScroll::SetNewText( int nIndex )
{
	T_TEXT_SCROLLptr pText = (T_TEXT_SCROLLptr)m_TextList[nIndex];

	m_fPosX_1 = m_fPosX+m_fWidth;
	m_pText_1->DrawD3DText( pText->szText1, strlen(pText->szText1) );
	m_pText_1->SetTextColor( pText->rgb_1 );
	m_pText_1->SetPosition( m_fPosX_1, m_fPosY );

	m_fPosX_2 = m_fPosX_1 + m_pText_1->GetTextLength();
	m_pText_2->DrawD3DText( pText->szText2, strlen(pText->szText2) );
	m_pText_2->SetTextColor( pText->rgb_2 );
	m_pText_2->SetPosition( m_fPosX_2, m_fPosY );

	m_fPosX_3 = m_fPosX_2 + m_pText_2->GetTextLength();
	m_pText_3->DrawD3DText( pText->szText3, strlen(pText->szText3) );
	m_pText_3->SetTextColor( pText->rgb_3 );
	m_pText_3->SetPosition( m_fPosX_3, m_fPosY );

	m_fPosX_4 = m_fPosX_3 + m_pText_3->GetTextLength();
	m_pText_4->DrawD3DText( pText->szText4, strlen(pText->szText4) );
	m_pText_4->SetTextColor( pText->rgb_4 );
	m_pText_4->SetPosition( m_fPosX_4, m_fPosY );

	m_fEndPosX = m_fPosX - (m_pText_1->GetTextLength()
							+m_pText_2->GetTextLength()
							+m_pText_3->GetTextLength()
							+m_pText_4->GetTextLength()
							+TEXT_SCROLL_INTERVAL);
}