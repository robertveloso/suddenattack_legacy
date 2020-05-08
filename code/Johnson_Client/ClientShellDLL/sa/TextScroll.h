#ifndef __MURSUM_TEXT_SCROLL_H__
#define __MURSUM_TEXT_SCROLL_H__

#define MAX_TEXT_SCROLL_LEN		256
#define TEXT_SCROLL_INTERVAL	50.0f

typedef struct _tagTEXT_SCROLL
{
	char		szText1[MAX_TEXT_SCROLL_LEN];
	char		szText2[MAX_TEXT_SCROLL_LEN];
	char		szText3[MAX_TEXT_SCROLL_LEN];
	char		szText4[MAX_TEXT_SCROLL_LEN];
	
	COLORREF	rgb_1;
	COLORREF	rgb_2;
	COLORREF	rgb_3;
	COLORREF	rgb_4;
} T_TEXT_SCROLL, *T_TEXT_SCROLLptr;

typedef std::vector<T_TEXT_SCROLLptr> TEXT_SCROLL_LIST;
typedef TEXT_SCROLL_LIST::iterator	TEXT_SCROLL_LIST_ITR;

class CTextScroll
{
public:
	CTextScroll();
	~CTextScroll();

	LTBOOL	Create( float fPosX, float fPosY, float fWidth, float fHeight );
	void	Destroy();
	void	Clear();
	void	InitPos();

	void	AddText( T_TEXT_SCROLLptr pText );
	void	AddText( const char* lpszText_1, COLORREF rgb_1,
					 const char* lpszText_2=LTNULL, COLORREF rgb_2=0,
					 const char* lpszText_3=LTNULL, COLORREF rgb_3=0,
					 const char* lpszText_4=LTNULL, COLORREF rgb_4=0 );

	void	Update();
	void	Render();

protected:
	void	SetNewText( int nIndex );
	
	TEXT_SCROLL_LIST	m_TextList;

	CSAD3DText*			m_pText_1;
	CSAD3DText*			m_pText_2;
	CSAD3DText*			m_pText_3;
	CSAD3DText*			m_pText_4;

	float				m_fPosX;
	float				m_fPosY;
	float				m_fWidth;
	float				m_fHeight;
	
	float				m_fPosX_1;
	float				m_fPosX_2;
	float				m_fPosX_3;
	float				m_fPosX_4;
	float				m_fEndPosX;

	int					m_nCurrentIndex;
	int					m_nLastIndex;

	float				m_fStopTime;
};

#endif