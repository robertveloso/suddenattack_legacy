// ----------------------------------------------------------------------- //
//
// MODULE  : LTGUICtrl.h
//
// PURPOSE : Base clase for controls
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#if !defined(_LTGUICTRL_H_)
#define _LTGUICTRL_H_

#include "iltclient.h"


#define	LGCS_NORMAL		0	// control states
#define LGCS_SELECTED	1
#define LGCS_DISABLED	2

class CLTGUICtrl : public CLTGUICommandHandler
{
public:
	CLTGUICtrl();
	virtual ~CLTGUICtrl();

	// All controls should have a create function.
    virtual void    Create(uint32 nCommandID, uint32 nHelpID, uint32 nParam1 = 0, uint32 nParam2 = 0)
					{
						m_nCommandID = nCommandID;
						m_nHelpID = nHelpID;
						m_nParam1 = nParam1;
						m_nParam2 = nParam2;
					}

	// Destruction
	virtual void	Destroy() {}
	
	virtual void	SetTooltip( const char* lpszTooltip );
	virtual const char*	GetTooptip();
	virtual LTBOOL	IsEnableTooltip() { return m_bEnableTooltip; }

	virtual void	SetHotKey( short HotKey1, short HotKey2=-1 );
	virtual LTBOOL	IsHotKey( unsigned char HotKey );
	
	// Render the control
	virtual void	Render ( ) = 0;

	// Reset the animation of the control if it has one
	virtual void	ResetAnimation() {}

	// Width/Height calculations
	virtual uint16	GetWidth ( ) { return 0; }
	virtual uint16	GetHeight ( ) { return 0; }

// Added By IN'T ==>
	virtual uint16	GetBaseWidth ( ) { return ( uint16 )( (float)GetWidth() / GetScale()); }
	virtual uint16	GetBaseHeight ( ) { return ( uint16 )( (float)GetHeight() / GetScale()); }
// Added By IN'T <==
	
	virtual void	SetBasePos( LTIntPt pos )       { m_basePos=pos; SetScale(m_fScale);}
    virtual LTIntPt GetBasePos ( )                  { return m_basePos; }
    virtual LTIntPt GetPos ( )                      { return m_pos; }

	virtual void	SetScale(float fScale);
	virtual float	GetScale()					{return m_fScale;}

	
	// Commonly used input messages
    virtual LTBOOL  OnLeft ( ) {return LTFALSE;}
    virtual LTBOOL  OnRight ( ) {return LTFALSE;}
    virtual LTBOOL  OnUp ( ) {return LTFALSE;}
    virtual LTBOOL  OnDown ( ) {return LTFALSE;}
    virtual LTBOOL  OnEnter ( ) {return LTFALSE;}
	virtual LTBOOL	OnEscape( ) {return LTFALSE;}

	//check a point and see if it is over this control
	virtual LTBOOL  IsOnMe(int x, int y);

	// Handles gamepad input
    virtual LTBOOL   HandleInterfaceCommand(int command) {return LTFALSE;}

	// Handles a key press.  Returns FALSE if the key was not processed through this method.
	// Left, Up, Down, Right, and Enter are automatically passed through OnUp(), OnDown(), etc.
    virtual LTBOOL   HandleKeyDown(int key, int rep) {return LTFALSE;}

	// Handles a character input.
    virtual LTBOOL   HandleChar(unsigned char c) {return LTFALSE;}

	// Mouse messages
    virtual LTBOOL   OnLButtonDown(int x, int y) {return LTFALSE;}
    virtual LTBOOL   OnLButtonUp(int x, int y) {return LTFALSE;}
    virtual LTBOOL   OnLButtonDblClick(int x, int y) {return LTFALSE;}
    virtual LTBOOL   OnRButtonDown(int x, int y) {return LTFALSE;}
    virtual LTBOOL   OnRButtonUp(int x, int y) {return LTFALSE;}
    virtual LTBOOL   OnRButtonDblClick(int x, int y) {return LTFALSE;}
    virtual LTBOOL   OnMouseMove(int x, int y) {return LTFALSE;}
	virtual LTBOOL   OnWheelUp(int x, int y);
    virtual LTBOOL   OnWheelDown(int x, int y); 

	// Handle a command
    virtual void    OnCommand ( uint32 nCommandID ) {}

	// Select a control
    virtual void    Select(LTBOOL bSelected);

	// Click a control
	virtual void	Click(LTBOOL  bClicked);

	// Enable/Disable the control
    virtual void    Enable ( LTBOOL bEnabled )       { m_bEnabled=bEnabled; }

	// Show/Hide the control
    virtual void    Show ( LTBOOL bShow )       { m_bVisible=bShow; }

	// Update data
    virtual void    UpdateData ( LTBOOL bSaveAndValidate = LTTRUE ) {}

	virtual	int		GetState();
    virtual	uint32	GetCurrentColor();

	// Sets the colors to fade to and from
    virtual void    SetColors(uint32 argbSelected, uint32 argbNormal, uint32 argbDisabled) 
				{ m_argbSelected = argbSelected; m_argbNormal = argbNormal; m_argbDisabled = argbDisabled;}
	

	// Access to member variables
    //LTBOOL          IsSelected()                    { return m_bSelected; }
	LTBOOL          IsSelected()                    { return (m_bSelected && !IsClicked()); }
    LTBOOL          IsEnabled()                     { return m_bEnabled && IsVisible(); }
    //LTBOOL          IsNormal()                      { return(!IsSelected() && IsEnabled()); }
	LTBOOL          IsNormal()                      { return(!IsSelected() && IsEnabled() && !IsClicked()); }
    LTBOOL          IsDisabled()                    { return(!IsSelected() && !IsEnabled()); }
    LTBOOL          IsVisible()						{ return m_bVisible; }
	LTBOOL			IsClicked()						{ return m_bClicked; }		// ±¤½Ä

    virtual void    SetCommandID(uint32 id)            { m_nCommandID = id; }
    virtual uint32  GetCommandID()                     { return m_nCommandID; }

    void            SetParam1(uint32 n) { m_nParam1 = n; }
    uint32          GetParam1() { return(m_nParam1); }

    void            SetParam2(uint32 n) { m_nParam2 = n; }
    uint32          GetParam2() { return(m_nParam2); }

    virtual void    SetHelpID(uint32 id)            { m_nHelpID = id; }
    virtual uint32  GetHelpID()                     { return m_nHelpID; }

	virtual LTBOOL	IsPlaySound()					{ return LTFALSE; }
	virtual int		GetOverSound()					{ return 0; }
	virtual int		GetClickSound()					{ return 0; }

protected:
	// Called by Select().  The IsSelected() will return the newly selected state.
	virtual void	OnSelChange()					{}

protected:
    uint32          m_nCommandID;          // The command which is sent when "enter" is pressed
    uint32          m_nHelpID;
    uint32          m_nParam1;             // Extra params
    uint32          m_nParam2;             // Extra params

    LTBOOL          m_bCreated;

    LTIntPt         m_pos;
    LTIntPt         m_basePos;
	float			m_fScale;

	// Added by IN'T ==>
	uint16			m_baseWidth;
	uint16			m_baseHeight;
	// Added by IN'T <==


    LTBOOL          m_bSelected;
    LTBOOL          m_bEnabled;
    LTBOOL          m_bVisible;
	LTBOOL			m_bClicked;			   // ±¤½Ä
	// ±¤½Ä [05.03.16] : ToolTip test
	//--------------------------------
	LTIntPt			m_ptMousePos;
	//--------------------------------

    uint32	        m_argbSelected;        // The selected color
    uint32	        m_argbNormal;          // The non-selected color
    uint32	        m_argbDisabled;        // The disabled color

	char			m_szTooltipText[256];
	LTBOOL			m_bEnableTooltip;

	short			m_nHotKey1;
	short			m_nHotKey2;

};

inline int CLTGUICtrl::GetState()
{
	if (IsSelected()) return(LGCS_SELECTED);
	if (IsDisabled()) return(LGCS_DISABLED);
	return(LGCS_NORMAL);
}

inline	uint32	CLTGUICtrl::GetCurrentColor()
{
	if (IsSelected()) return(m_argbSelected);
	if (IsDisabled()) return(m_argbDisabled);
	return(m_argbNormal);
}


#include "../../Shared/LTPoly.h"

class CLTGUIDummyCtrl : public CLTGUICtrl
{
public:
	CLTGUIDummyCtrl();
	virtual ~CLTGUIDummyCtrl(){};

	void	Render();
	LTBOOL  IsOnMe(int x, int y);

	LTBOOL   HandleInterfaceCommand(int command) {return LTTRUE;}
	LTBOOL   HandleKeyDown(int key, int rep) {return LTTRUE;}
	LTBOOL   HandleChar(unsigned char c) {return LTTRUE;}
	LTBOOL   OnLButtonDown(int x, int y) {return LTTRUE;}
    LTBOOL   OnLButtonUp(int x, int y) {return LTTRUE;}
    LTBOOL   OnLButtonDblClick(int x, int y) {return LTTRUE;}
    LTBOOL   OnRButtonDown(int x, int y) {return LTTRUE;}
    LTBOOL   OnRButtonUp(int x, int y) {return LTTRUE;}
    LTBOOL   OnRButtonDblClick(int x, int y) {return LTTRUE;}
    LTBOOL   OnMouseMove(int x, int y) {return LTTRUE;}
	LTBOOL   OnWheelUp(int x, int y) {return LTTRUE;}
    LTBOOL   OnWheelDown(int x, int y) {return LTTRUE;}

private:
	LTPoly_GT4	m_Poly;
};

#endif // _LTGUICTRL_H_