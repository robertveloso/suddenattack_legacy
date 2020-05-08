#ifndef __MURSUM_EDITBOX_H__
#define __MURSUM_EDITBOX_H__

#define MAX_TEXT_BUFFER 255
#define CARET_WIDTH		2

#define UIM_EDIT_ENTER			0x00000001
#define UIM_EDIT_ESCAPE			0x00000002
#define UIM_EDIT_CHAR			0x00000003
#define UIM_EDIT_NUMERIC_ONLY	0x00000004


class CSAD3DText;
class CImeCtrl;
class CLTGUIEditBox;
class CHUDChatInput;

class CImeCtrl
{
public:
	CImeCtrl();
	virtual ~CImeCtrl();
	void			SetMainHandle( HWND hWnd ) { m_hMainWnd = hWnd; }
	LRESULT			WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void			AddEditBox( CLTGUIEditBox* pEditBox, LTBOOL bEng, LTBOOL bPassword );
	void			RemoveEditBox( CLTGUIEditBox* pEditBox );

	void			SetIMEMode( DWORD dwCon, DWORD dwSen );
	void			GetIMEMode( DWORD* pdwCon, DWORD* pdwSen );
	void			ClearCompositionString();
	
protected:
	
	HWND			m_hMainWnd;
	CLTGUIEditBox*	m_pEditBox;
	LTBOOL			m_bPassword;
};

extern CImeCtrl*	g_pImeCtrl;


class CLTGUIEditBox : public CLTGUICtrl
{
public:
	CLTGUIEditBox();
	virtual ~CLTGUIEditBox();

	virtual LTBOOL		Create( uint16 nPosX, uint16 nPosY,
								uint16 nWidth, uint16 nHeight,
								COLORREF rgbTextColor, COLORREF rgbExtColor,  unsigned long dwCaretColor,
								unsigned long dwDrawType,
								LTBOOL bPassword,
								uint32 nCommandID,
								CLTGUICommandHandler* pCommandHandler = LTNULL );
	
	void				SetCommandHandler( CHUDChatInput* pHUDChatInput );
	void				Term();

	void				Render();
	void				RenderCaret();

	uint16				GetWidth()	{ return m_nWidth; }
	uint16				GetHeight()	{ return m_nHeight; }

	LTBOOL				HandleKeyDown(int key, int rep);

	LTBOOL				OnIMENotify( WPARAM wParam, LPARAM lParam );
	LTBOOL				OnIMEStartComp();
	LTBOOL				OnIMEEndComp();
	LTBOOL				OnIMECompString( const char* lpszCompStr, int nLen );
	LTBOOL				OnIMEResultString( const char* lpszResultStr, int nLen );
	
	LTBOOL				OnChar( char ch );
    LTBOOL				OnLButtonDown(int x, int y);
	LTBOOL				OnEnter();
	LTBOOL				OnEscape();
	LTBOOL				IsOnMe( int x, int y );
		
	LTBOOL				IsCaretOn();
	LTBOOL				IsFocused();
	void				OnLostFocus();

	void				SetNumericOnly( bool bNumeric );
	int					GetInt();
	
	void				SetText();
	void				SetText( const char* lpszText );
	void				StrCat( const char* lpszText );
	const char*			GetText()	{ return m_szTextBuffer; }
	const char			GetChar( int nIndex );
	void				ClearText();
	void				SetBasePos( LTIntPt pos );
	
	void				ForceAddEditBox();

	void				SetIMEMode( DWORD dwConMode, DWORD dwSenMode );

	DWORD				GetIMEConversionMode();
	DWORD				GetIMESentenceMode();
	
	LTBOOL				CheckEmptySpace();
	bool				IsLastBack() { return m_bLastBack; }

protected:
	LTBOOL				PointInMe( int x, int y );
	LTBOOL				IsDBCS( int nPos );
	int					GetPrevPos( int nPos );
	void				ClearZeroString();

	int					m_nTextWidth;
	int					m_nTextHeight;

	SIZE				m_cTextSize;
	CSAD3DText*			m_pText;
	unsigned long		m_dwDrawType;
	uint16				m_nWidth;
	uint16				m_nHeight;

	LTBOOL				m_bFocus;
	LTBOOL				m_bPassword;

	LT_POLYF4			m_Caret;
	int					m_nInteval;

	char				m_szTextBuffer[256];
	char				m_szPassBuffer[256];
	int					m_nTextLen;
	int					m_nTextPos;

	LTBOOL				m_bEng;
	DWORD				m_dwConversionMode;
	DWORD				m_dwSentenceMode;

	//Caret Timer
	unsigned long		m_dwLastTime;
	LTBOOL				m_bCaretOn;
	bool				m_bNumericOnly;
		
	//Command Handler
	CLTGUICommandHandler*	m_pCommandHandler;
	CHUDChatInput*			m_pHUDChatInput;
	
	bool					m_bLastBack;
};

#endif //#ifndef __MURSUM_EDITBOX_H__
