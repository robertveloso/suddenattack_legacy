#include "ltguimgr.h"
#include "../../ClientShellDLL/ClientShellShared/HanText.h"

CImeCtrl*		g_pImeCtrl	= LTNULL;
static CImeCtrl	g_ImeCtrl;

CImeCtrl::CImeCtrl()
{
	g_pImeCtrl	= this;
	m_pEditBox	= NULL;
	m_bPassword	= LTFALSE;
}

CImeCtrl::~CImeCtrl()
{
	g_pImeCtrl	= LTNULL;
}

void CImeCtrl::AddEditBox( CLTGUIEditBox* pEditBox, LTBOOL bEng, LTBOOL bPassword )
{
	if( m_pEditBox ) RemoveEditBox( m_pEditBox );

	m_pEditBox	= pEditBox;
	m_bPassword	= bPassword;

	if( m_bPassword )
	{
		this->SetIMEMode( 0, IME_SMODE_NONE );
	}
	else
	{
		this->SetIMEMode( m_pEditBox->GetIMEConversionMode(), m_pEditBox->GetIMESentenceMode() );
	}
}

void CImeCtrl::RemoveEditBox( CLTGUIEditBox* pEditBox )
{
	if( NULL != m_pEditBox && m_pEditBox == pEditBox )
	{
		DWORD	dwConversion=0, dwSentence=0;
		GetIMEMode( &dwConversion, &dwSentence );
		m_pEditBox->SetIMEMode( dwConversion, dwSentence );
		m_pEditBox	= LTNULL;
		this->SetIMEMode( 0, IME_SMODE_NONE );
	}
}

LRESULT CImeCtrl::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	int			nLen;		
	HIMC		hIMC=NULL;  
//	static char	szString[16];
	static char	szString[0xff];		// mod by minjin. 2005. 01. 15.

	if( !m_pEditBox )
	{
		if( uMsg == WM_IME_COMPOSITION ) return 1L;
		return 0L;
	}
	
	switch( uMsg )
	{
	case WM_IME_STARTCOMPOSITION:
		m_pEditBox->OnIMEStartComp();
		return 1L;

	case WM_IME_ENDCOMPOSITION:
		m_pEditBox->OnIMEEndComp();
		return 1L;

	case WM_IME_NOTIFY:
		m_pEditBox->OnIMENotify( wParam, lParam );
		if( m_bPassword ) SetIMEMode( 0, IME_SMODE_NONE );
		return 1L;

	case WM_IME_COMPOSITION:		
		hIMC = ImmGetContext( hWnd );			
		if( lParam&GCS_RESULTSTR )		//조합완료
		{
			if( (nLen = ImmGetCompositionString( hIMC, GCS_RESULTSTR, NULL, 0 ) ) > 0 )
			{
				ImmGetCompositionString( hIMC, GCS_RESULTSTR, szString, nLen );
				szString[nLen] = LTNULL;

				m_pEditBox->OnIMEResultString( szString, nLen );				
			}
		}
		else if( lParam&GCS_COMPSTR )	//조합중
		{
			nLen = ImmGetCompositionString( hIMC, GCS_COMPSTR, NULL, 0 );
			ImmGetCompositionString( hIMC, GCS_COMPSTR, szString, nLen );							
			szString[nLen] = LTNULL;

			m_pEditBox->OnIMECompString( szString, nLen );
		}
		ImmReleaseContext( hWnd, hIMC );		
		return 1L;

	case WM_CHAR:
		if( m_pEditBox->OnChar( (char)wParam ) ) return 1L;
		return 0L;

	case WM_LBUTTONDOWN:
		m_pEditBox->OnLButtonDown( (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) );
		return 0L;

	case WM_KEYDOWN:
		if( LOWORD(wParam) == VK_ESCAPE ) return 0L;
		if( LOWORD(wParam) == VK_F1) return 0L; //[yaiin] 에디트 상태라도 F1 먹게 하기 위함.
		return 1L;
	}

	return 0L;
}

void CImeCtrl::SetIMEMode( DWORD dwCon, DWORD dwSen )
{
	HIMC hIMC = ImmGetContext( m_hMainWnd );
	if( hIMC )
	{
		ImmSetConversionStatus( hIMC, dwCon, dwSen );
		ImmReleaseContext( m_hMainWnd, hIMC );
	}
}

void CImeCtrl::GetIMEMode( DWORD* pdwCon, DWORD* pdwSen )
{
	HIMC hIMC = ImmGetContext( m_hMainWnd );	
	if( hIMC )
	{
		ImmGetConversionStatus( hIMC, pdwCon, pdwSen );
		ImmReleaseContext( m_hMainWnd, hIMC );
	}
}

void CImeCtrl::ClearCompositionString()
{
	HIMC hIMC = ImmGetContext( m_hMainWnd );	
	if( hIMC )
	{
		ImmSetCompositionString( hIMC, SCS_SETSTR, NULL, 0, NULL, 0 );
		ImmReleaseContext( m_hMainWnd, hIMC );
	}
}



#include "../../ClientShellDLL/clientshellshared/GameText.h"
#include "../../ClientShellDLL/clientshellshared/HUDChatInput.h"

CLTGUIEditBox::CLTGUIEditBox()
{
	m_bFocus			= LTFALSE;
	m_pText				= LTNULL;
	m_bPassword			= LTFALSE;
	m_nTextHeight		= 12;
	m_cTextSize.cx		= 0;
	m_cTextSize.cy		= m_nTextHeight;
	m_nInteval			= 0;
	m_bEng				= LTFALSE;
	m_bNumericOnly		= false;
	m_bLastBack			= false;
	
#if defined		KOR_LOCALIZING
	m_dwConversionMode	= IME_CMODE_NATIVE;
	m_dwSentenceMode	= IME_SMODE_NONE;
#elif defined	JPN_LOCALIZING
	m_dwConversionMode	= IME_CMODE_FULLSHAPE|IME_CMODE_ROMAN|IME_CMODE_NATIVE;
	m_dwSentenceMode	= IME_SMODE_PHRASEPREDICT;
#elif defined	CHN_LOCALIZING
	m_dwConversionMode	= IME_CMODE_NATIVE;
	m_dwSentenceMode	= IME_SMODE_NONE;
#endif
		
	m_pCommandHandler	= LTNULL;
	m_pHUDChatInput		= LTNULL;
	
	memset( &m_Caret, 0, sizeof(m_Caret) );
}

CLTGUIEditBox::~CLTGUIEditBox()
{
	this->Term();
}

LTBOOL CLTGUIEditBox::Create( uint16 nPosX, uint16 nPosY,
							  uint16 nWidth, uint16 nHeight,
							  COLORREF rgbTextColor, COLORREF rgbExtColor, unsigned long dwCaretColor,
							  unsigned long dwDrawType,
							  LTBOOL bPassword,
							  uint32 nCommandID,
							  CLTGUICommandHandler* pCommandHandler )
{
	m_pos.x				= nPosX;
	m_pos.y				= nPosY;	
	m_nWidth			= nWidth;
	m_nHeight			= nHeight;
	m_nCommandID		= nCommandID;
	m_pCommandHandler	= pCommandHandler;	
	m_dwDrawType		= dwDrawType;
	m_bPassword			= bPassword;

	if( m_nTextHeight < m_nHeight )
	{
		m_nInteval = (m_nHeight-m_nTextHeight)/2;
		m_nTextWidth = m_nWidth-(m_nInteval*2);
	}
	else
	{
		m_nTextWidth = m_nWidth;
	}

	m_pText	= new CSAD3DText();
	m_pText->CreateD3DText( m_nTextWidth, m_nTextHeight+4, false );
	m_pText->SetTransparent(true);
	m_pText->SetTextColor( rgbTextColor, rgbExtColor );
	m_pText->SetPosition( (float)nPosX+m_nInteval, (float)nPosY+m_nInteval );
	ClearText();	
	
	m_nTextWidth -= CARET_WIDTH;

	g_pDrawPrim->SetRGBA( &m_Caret, dwCaretColor );
	g_pDrawPrim->SetXYWH( &m_Caret,
						   (float)m_pos.x+m_nInteval+3, (float)m_pos.y+m_nInteval-1,
						   (float)CARET_WIDTH, (float)m_nTextHeight+2 );

	return LTTRUE;
}

void CLTGUIEditBox::Term()
{
	if( m_pText )
	{
		delete m_pText;
		m_pText = LTNULL;
	}
}

void CLTGUIEditBox::SetCommandHandler( CHUDChatInput* pHUDChatInput )
{
	m_pHUDChatInput = pHUDChatInput;
}

void CLTGUIEditBox::Render()
{
	if( m_bFocus && IsCaretOn() )
	{
		RenderCaret();
	}

	m_pText->Render();
}

void CLTGUIEditBox::RenderCaret()
{
	if( m_cTextSize.cx > m_nTextWidth-CARET_WIDTH-1 )
	{
		m_cTextSize.cx = m_nTextWidth-CARET_WIDTH-1;
	}

	g_pDrawPrim->SetTexture(LTNULL);
	g_pDrawPrim->DrawPrim(&m_Caret);
}

void CLTGUIEditBox::SetNumericOnly( bool bNumeric )
{
	m_bNumericOnly	= bNumeric;
}

LTBOOL CLTGUIEditBox::HandleKeyDown( int key, int rep )
{
	return LTTRUE;
}

void CLTGUIEditBox::SetText()
{
	m_nTextLen	= strlen(m_szTextBuffer);

	m_bCaretOn		= LTTRUE;
	m_dwLastTime	= ::timeGetTime();	

	if( m_bPassword )
	{
		::GetTextExtentPoint32( m_pText->GetDC(), m_szPassBuffer, strlen(m_szPassBuffer), &m_cTextSize );
	}
	else
	{	
		::GetTextExtentPoint32( m_pText->GetDC(), m_szTextBuffer, m_nTextLen, &m_cTextSize );
	}
	
	if( m_cTextSize.cx > m_nTextWidth )
	{
		m_pText->SetDrawRegion( m_nTextWidth-m_cTextSize.cx, 0, -CARET_WIDTH, 0 );
		g_pDrawPrim->SetXYWH( &m_Caret,
							  (float)m_pos.x+m_nTextWidth+m_nInteval+3, (float)m_pos.y+m_nInteval-1,
						      (float)CARET_WIDTH, (float)m_nTextHeight+2 );
	}
	else
	{
		m_pText->SetDrawRegion( 0, 0, -CARET_WIDTH, 0 );
		g_pDrawPrim->SetXYWH( &m_Caret,
						      (float)m_pos.x+m_nInteval+m_cTextSize.cx+3, (float)m_pos.y+m_nInteval-1,
						      (float)CARET_WIDTH, (float)m_nTextHeight+2 );
	}

	if( m_bPassword )
	{
		m_pText->DrawD3DText( m_szPassBuffer, strlen(m_szPassBuffer), DT_LEFT, m_dwDrawType, false );
	}
	else
	{
		m_pText->DrawD3DText( m_szTextBuffer, m_nTextLen, DT_LEFT, m_dwDrawType, false );
	}
}

void CLTGUIEditBox::SetText( const char* lpszText )
{
	if( !lpszText || m_bPassword ) return;

	strcpy( m_szTextBuffer, lpszText );
	m_nTextPos = strlen( m_szTextBuffer );

	this->SetText();
}

void CLTGUIEditBox::StrCat( const char* lpszText )
{
	if( !lpszText ) return;

	strcat( m_szTextBuffer, lpszText );
	m_nTextPos = strlen( m_szTextBuffer );

	g_ImeCtrl.ClearCompositionString();
	this->SetText();
}

const char CLTGUIEditBox::GetChar( int nIndex )
{
	if( m_nTextLen <= nIndex ) return ' ';
	return m_szTextBuffer[nIndex];
}

int CLTGUIEditBox::GetInt()
{
	if( !m_bNumericOnly || strlen(m_szTextBuffer)<=0 ) return 0;

	return atoi( m_szTextBuffer );
}

LTBOOL CLTGUIEditBox::OnIMENotify( WPARAM wParam, LPARAM lParam )
{
	return LTTRUE;
}

LTBOOL CLTGUIEditBox::OnIMEStartComp()
{
	return LTTRUE;
}

LTBOOL CLTGUIEditBox::OnIMEEndComp()
{
	return LTTRUE;
}

LTBOOL CLTGUIEditBox::OnIMECompString( const char* lpszCompStr, int nLen )
{
	if( m_nTextPos >= MAX_TEXT_BUFFER ) return LTFALSE;

	if( m_bNumericOnly )
	{
		if( lpszCompStr[0] < 0x30 || lpszCompStr[0] > 0x39 )
		{
			m_pCommandHandler->SendCommand( m_nCommandID, UIM_EDIT_NUMERIC_ONLY, LTNULL );
			return LTFALSE;
		}
	}

	for( int i=0 ; i<nLen ; ++i )
	{
		m_szTextBuffer[m_nTextPos+i] = lpszCompStr[i];
	}
	m_szTextBuffer[m_nTextPos+i] = LTNULL;

	if( m_pCommandHandler )
	{
		m_pCommandHandler->SendCommand( m_nCommandID, UIM_EDIT_CHAR, LTNULL );
	}
	else if( m_pHUDChatInput )
	{
		m_pHUDChatInput->OnChar();
	}

	SetText();
	return LTTRUE;
}

LTBOOL CLTGUIEditBox::OnIMEResultString( const char* lpszResultStr, int nLen )
{
	if( m_nTextPos >= MAX_TEXT_BUFFER ) return LTFALSE;

	if( m_bNumericOnly )
	{
		if( lpszResultStr[0] < 0x30 || lpszResultStr[0] > 0x39 )
		{
			m_pCommandHandler->SendCommand( m_nCommandID, UIM_EDIT_NUMERIC_ONLY, LTNULL );
			return LTFALSE;
		}
	}

	for( int i=0 ; i<nLen ; ++i )
	{
		m_szTextBuffer[m_nTextPos++] = lpszResultStr[i];
	}
	m_szTextBuffer[m_nTextPos] = LTNULL;

	if( m_pCommandHandler )
	{
		m_pCommandHandler->SendCommand( m_nCommandID, UIM_EDIT_CHAR, LTNULL );
	}
	else if( m_pHUDChatInput )
	{
		m_pHUDChatInput->OnChar();
	}

	SetText();
	return LTTRUE;
}

LTBOOL CLTGUIEditBox::OnChar( char ch )
{
	m_bLastBack = false;

	switch( ch )	
	{
	case 9:
		return LTFALSE;
		
	case 13:
		this->OnEnter();
		return LTTRUE;
		
	case 27:
		this->OnEscape();
		return LTTRUE;

	case 8:		//Back Space
		{
			if( m_nTextPos <= 0 ) return LTTRUE;
			
			m_bLastBack = true;

			m_nTextPos = GetPrevPos( m_nTextPos );
			m_szTextBuffer[m_nTextPos] = LTNULL;
			if( m_bPassword ) m_szPassBuffer[strlen(m_szPassBuffer)-1] = LTNULL;
			
			if( m_pCommandHandler )
			{
				m_pCommandHandler->SendCommand( m_nCommandID, UIM_EDIT_CHAR, LTNULL );
			}
		}
		break;

	default:
		if( m_nTextPos >= MAX_TEXT_BUFFER || ch < 0x20 ) return LTFALSE;

		if( m_bNumericOnly )
		{
			if( ch < 0x30 || ch > 0x39 )
			{
				m_pCommandHandler->SendCommand( m_nCommandID, UIM_EDIT_NUMERIC_ONLY, LTNULL );
				return LTTRUE;
			}
		}

		m_szTextBuffer[m_nTextPos++] = ch;
		m_szTextBuffer[m_nTextPos] = LTNULL;
		
		if( m_bNumericOnly )
		{
			ClearZeroString();
		}
		else if( m_bPassword )
		{
			strcat( m_szPassBuffer, "*" );
		}

		if( m_pCommandHandler )
		{
			m_pCommandHandler->SendCommand( m_nCommandID, UIM_EDIT_CHAR, LTNULL );
		}
		else if( m_pHUDChatInput )
		{
			m_pHUDChatInput->OnChar();
		}
		break;
	}

	SetText();
	return LTTRUE;
}

LTBOOL CLTGUIEditBox::OnLButtonDown( int x, int y )
{
	if( m_bFocus )
	{
		if( !PointInMe( x, y ) )
		{
			m_bFocus = LTFALSE;

			if( m_bNumericOnly && strlen(m_szTextBuffer)<=0 )
			{
				SetText( "0" );
			}

			g_ImeCtrl.RemoveEditBox( this );
		}
	}
	else
	{
		if( PointInMe( x, y ) ) ForceAddEditBox();
	}

	return LTFALSE;
}

void CLTGUIEditBox::ForceAddEditBox()
{
	m_bFocus		= LTTRUE;
	m_bCaretOn		= LTTRUE;
	m_dwLastTime	= GetTickCount();
	m_bLastBack		= false;
	g_ImeCtrl.AddEditBox( this, m_bEng, m_bPassword );
}

LTBOOL CLTGUIEditBox::OnEnter()
{
	if( m_pCommandHandler )
	{
		m_pCommandHandler->SendCommand( m_nCommandID, UIM_EDIT_ENTER, LTNULL );
	}
	else if( m_pHUDChatInput )
	{
		m_pHUDChatInput->OnEnter();
	}

	return LTTRUE;
}

LTBOOL CLTGUIEditBox::OnEscape()
{
	if( m_pCommandHandler )
	{
		m_pCommandHandler->SendCommand( m_nCommandID, UIM_EDIT_ESCAPE, LTNULL );
	}
	else if( m_pHUDChatInput )
	{
		m_pHUDChatInput->OnEscape();
	}
	return LTTRUE;
}

void CLTGUIEditBox::ClearText()
{
	memset( m_szTextBuffer, 0, 256 );
	memset( m_szPassBuffer, 0, 256 );
	m_nTextPos	= 0;
	m_nTextLen	= 0;
	SetText();
}

void CLTGUIEditBox::OnLostFocus()
{
	m_bFocus	= LTFALSE;

	if( m_bNumericOnly && strlen(m_szTextBuffer)<=0 )
	{
		SetText( "0" );
	}

	g_ImeCtrl.RemoveEditBox( this );
}

LTBOOL CLTGUIEditBox::IsOnMe( int x, int y )
{
	if( m_bFocus ) return LTFALSE;
	return CLTGUICtrl::IsOnMe( x, y );
}

LTBOOL CLTGUIEditBox::PointInMe( int x, int y )
{
	if( m_pHUDChatInput ) return LTTRUE;
	if( m_pos.x<=x && x<=(m_pos.x+m_nWidth) && m_pos.y<=y && y<=(m_pos.y+m_nHeight) ) return LTTRUE;
	return LTFALSE;
}

LTBOOL CLTGUIEditBox::IsCaretOn()
{
	unsigned long dwCurTime   = ::GetTickCount();

	if( dwCurTime - m_dwLastTime > 500 )
	{
		m_dwLastTime = dwCurTime;
		m_bCaretOn	^= 1;
	}

	return m_bCaretOn;
}

LTBOOL CLTGUIEditBox::IsFocused()
{
	return m_bFocus;
}

LTBOOL CLTGUIEditBox::IsDBCS( int nPos )
{
	return IsDBCSLeadByte(m_szTextBuffer[nPos]);
}

void CLTGUIEditBox::SetBasePos( LTIntPt pos )
{
	m_pText->SetPosition( (float)pos.x+m_nInteval, (float)pos.y+m_nInteval );
	CLTGUICtrl::SetBasePos( pos );
	
	g_pDrawPrim->SetXYWH( &m_Caret,
						   (float)m_pos.x+m_nInteval+3, (float)m_pos.y+m_nInteval-1,
						   (float)CARET_WIDTH, (float)m_nTextHeight+2 );
}

void CLTGUIEditBox::SetIMEMode( DWORD dwConMode, DWORD dwSenMode )
{
	m_dwConversionMode	= dwConMode;
	m_dwSentenceMode	= dwSenMode;
}

int CLTGUIEditBox::GetPrevPos( int nPos )
{
	int n = 0, size;
	
	do
	{
		if( IsDBCS(n) )
		{
			size=2;
		}
		else
		{
			size=1;
		}
		n+=size;
	} while( n < nPos );


	return n-size;
}

DWORD CLTGUIEditBox::GetIMEConversionMode()
{
	return m_dwConversionMode;
}

DWORD CLTGUIEditBox::GetIMESentenceMode()
{
	return m_dwSentenceMode;
}

void CLTGUIEditBox::ClearZeroString()
{
	if( m_nTextPos == 2 )
	{
		if( m_szTextBuffer[0] == '0' )
		{
			m_szTextBuffer[0] = m_szTextBuffer[1];
			m_szTextBuffer[1] = LTNULL;
			m_nTextPos = 1;
		}
	}
}

LTBOOL CLTGUIEditBox::CheckEmptySpace()
{
	int nLen = strlen(m_szTextBuffer);

	for( int i=0 ; i<nLen ; ++i )
	{
		if( IsDBCSLeadByte( m_szTextBuffer[i] ) )
		{
			if( m_szTextBuffer[i] == -95 && m_szTextBuffer[i+1] == -95 ) return LTTRUE;
			++i;
			continue;
		}

		switch( m_szTextBuffer[i] )
		{
		case ' ':	return LTTRUE;
		case '\t':	return LTTRUE;	
		case '\r':	return LTTRUE;
		case '\n':	return LTTRUE;
		}
	}

	return LTFALSE;
}