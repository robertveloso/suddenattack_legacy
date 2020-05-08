// ----------------------------------------------------------------------- //
//
// MODULE  : LTGUIButton.h
//
// PURPOSE : button control with three states (normal, selected, and disabled)
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#if !defined(_LTGUIBUTTON_H_)
#define _LTGUIBUTTON_H_


class CSAD3DText;

// This control displays a string of text which fades out upon being unselected.
// It can contain multiple strings (see CLTGUICtrl::AddString()) which are cycled
// when OnLeft() and OnRight() are called.  This is useful in menu code for selecting
// options.
class CLTGUIButton : public CLTGUICtrl
{
public:
	CLTGUIButton();
	virtual ~CLTGUIButton();

	// Create the control
	// nCommandID		- The command ID which is sent when "enter" is pressed.
	// nHelpID			- The help ID which is used to display help text.
	// hNormal			- normal texture
	// hSelected		- highlighted texture
	// hDisabled		- disabled texture
	// pCommandHandler	- handler which receives messages when the "enter" key is pressed.
    LTBOOL           Create ( uint32 nCommandID, uint32 nHelpID, 
								HTEXTURE hNormal, HTEXTURE hSelected = LTNULL, HTEXTURE hDisabled = LTNULL, 
								CLTGUICommandHandler *pCommandHandler = LTNULL, uint32 nParam1 = 0, uint32 nParam2 = 0);

	// ±¤½Ä
	LTBOOL           Create ( uint32 nCommandID, uint32 nHelpID, float xRate, float yRate,
								HTEXTURE hNormal, HTEXTURE hSelected = LTNULL, HTEXTURE hDisabled = LTNULL, 
								CLTGUICommandHandler *pCommandHandler = LTNULL, uint32 nParam1 = 0, uint32 nParam2 = 0);

	// click ÀÖ´Â ³ð
	LTBOOL           Create ( uint32 nCommandID, uint32 nHelpID, float xRate, float yRate,
								HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, HTEXTURE hClicked,
								CLTGUICommandHandler *pCommandHandler = LTNULL, uint32 nParam1 = 0, uint32 nParam2 = 0);


    virtual void    SetBasePos ( LTIntPt pos );
	virtual void	SetScale(float fScale);
	virtual void	SetTextureScale(float fScale);

	// Render the control
	virtual void	Render ();

	// Width/Height calculations
	virtual uint16	GetWidth ( )						{ return m_nWidth; }
	virtual uint16	GetHeight ( )						{ return m_nHeight; }

	// Commonly used keyboard messages
    virtual LTBOOL   OnEnter ( );
	// ±¤½Ä
	//-------------------------------------------------------------------------
    //virtual LTBOOL   OnLButtonUp(int x, int y)			{ return OnEnter(); }
	virtual LTBOOL   OnLButtonUp(int x, int y)			{ Click(LTFALSE); return OnEnter(); }
	virtual	LTBOOL	 OnLButtonDown(int x, int y)		{ Click(LTTRUE); return LTTRUE; }
	
	virtual LTBOOL	 OnDblClick();
	virtual LTBOOL   OnLButtonDblClick(int x, int y)    { return OnDblClick(); }
	void			 SetDblCommand(uint32 nId)			{ m_nCommandID2 = nId; }
	//-------------------------------------------------------------------------

	// Set the font, pass in a NULL pointer to change only the font size
	//   pass in a font size of 0 to retain the current size.
    LTBOOL  SetFont ( CUIFont *pFont, uint8 nFontSize);

	//this fails, if no font has been defined
	LTBOOL	SetText(const char* pText, LTBOOL bHighlightText = LTFALSE);


	// ±¤½Ä
	void	SetTexture(HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, HTEXTURE hClick, LTBOOL bFreeOld = LTFALSE);

	void	SetTexture(HTEXTURE hNormal, HTEXTURE hSelected = LTNULL, HTEXTURE hDisabled = LTNULL, LTBOOL bFreeOld = LTFALSE);

	virtual void	SetCommandHandler(CLTGUICommandHandler *pCommandHandler)
									{ m_pCommandHandler = pCommandHandler; }
	
	//[MURSUM]
	void	SetPolyColor( uint32 nColor );



protected:
	void			SetRenderState();
	void			InitPoly();
	// ±¤½Ä
	//-------------------------------------------------
	void			InitPoly(float xRate, float yRate);
	//-------------------------------------------------

	void			ScalePoly();

protected:

	LT_POLYGT4	m_Poly;
	HTEXTURE	m_hNormal;			// normal texture
	HTEXTURE	m_hSelected;		// highlighted texture
	HTEXTURE	m_hDisabled;		// disabled texture
	HTEXTURE	m_hClick;			// click texture	// ±¤½Ä
	LTFLOAT		m_fTextureScale;

    uint16		m_nWidth;              // The width of the control
    uint16		m_nHeight;             // The height of the control

	// ±¤½Ä
	float		m_xRate;
	float		m_yRate;
	uint32		m_nCommandID2;

	// Receives a message when the "enter" key is pressed.
	CLTGUICommandHandler	*m_pCommandHandler;


public:
	// added by IN'T ===>
	void SetCheck(LTBOOL bCheck = LTTRUE);
	virtual void SetBasePos(int x, int y);
	void SetRect(LTRect rect);

	void SetSize(int16 nWidth, int16 nHeight);
	void SetCheckedTexture(void); //[yaiin]
	void SetCheckedTexture(HTEXTURE hCheckedTexture);
	bool GetCheck() {return !!m_bCheck;}
	
	//[MURSUM]
	LTBOOL	IsPlaySound()					{ return LTTRUE; }
	int		GetOverSound()					{ return m_nOverSound; }
	int		GetClickSound()					{ return m_nClickSound; }

	void	SetOverSound( int nSnd )		{ m_nOverSound = nSnd; }
	void	SetClickSound( int nSnd )		{ m_nClickSound = nSnd; }

protected:
	HTEXTURE	m_hChecked;			// Checked texture
	LTBOOL		m_bCheck;
	// Added by IN'T <===

	//[MURSUM]
	int		m_nOverSound;
	int		m_nClickSound;
};

#endif // _LTGUIBUTTON_H_