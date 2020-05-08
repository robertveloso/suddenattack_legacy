// BaseScreen.h: interface for the CBaseScreen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_BASE_SCREEN_H_)
#define _BASE_SCREEN_H_


#include "LTGUIMgr.h"
#include "BaseScaleFX.h"
#include "ChainedFX.h"
#include "LTPoly.h"

struct INT_CHAR;

#define MAX_INT_ATTACHMENTS 5
struct INT_ATTACH
{
	INT_ATTACH() { fScale = 1.0f;nAttachmentID = -1;szSocket[0]=LTNULL;}

	LTFLOAT	fScale;
	int		nAttachmentID;
	char	szSocket[64];
};
struct AttachmentData
{
	AttachmentData() { socket = INVALID_MODEL_SOCKET; }

	CBaseScaleFX	sfx;
	HMODELSOCKET	socket;
};

class CScreenMgr;


const uint16 kNoSelection = 0xFFFF;
const LTIntPt kDefaultPos(-1,-1);

class CBaseScreen : public CLTGUICommandHandler
{
public:
	CBaseScreen();
	virtual ~CBaseScreen();

	// Initialization/Termination
    virtual LTBOOL   Init(int nScreenID);
	virtual void	Term();

	// Builds the screen
    virtual LTBOOL   Build();
    inline  LTBOOL   IsBuilt() const { return m_bBuilt; }


	virtual void	Escape();

    virtual uint32 OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);

	// Get the screen ID
	inline int	GetScreenID() const	{ return m_nScreenID; }


	// Renders the screen to a surface
    virtual LTBOOL   Render(HSURFACE hDestSurf);
	// Returns false if the screen should exit as a result of this update
	virtual bool	UpdateInterfaceSFX();

	// This is called when the screen gets or loses focus
    virtual void    OnFocus(LTBOOL bFocus);

	// This is called when the selected item changes
    virtual void    OnSelectionChange()			{};
	
	virtual void	PrePopupState() {};
	virtual void	PostPopupState() {};

	// Handles a key press.  Returns FALSE if the key was not processed through this method.
	// Left, Up, Down, Right, and Enter are automatically passed through OnUp(), OnDown(), etc.
    virtual LTBOOL   HandleKeyDown(int key, int rep);
    virtual LTBOOL   HandleKeyUp(int key) { return LTFALSE; }
    virtual LTBOOL   HandleChar(unsigned char c);
	virtual LTBOOL	 HandleHotKey( unsigned char c );
    virtual LTBOOL   HandleForceUpdate() { return LTFALSE; }

	//reset selection and help display
	virtual void	ForceMouseUpdate();
	
	// Mouse messages
    virtual LTBOOL   OnLButtonDown(int x, int y);
    virtual LTBOOL   OnLButtonUp(int x, int y);
    virtual LTBOOL   OnLButtonDblClick(int x, int y);
    virtual LTBOOL   OnRButtonDown(int x, int y);
    virtual LTBOOL   OnRButtonUp(int x, int y);
    virtual LTBOOL   OnRButtonDblClick(int x, int y);
    virtual LTBOOL   OnMouseMove(int x, int y);
	virtual LTBOOL   OnWheelUp(int x, int y);
	virtual LTBOOL   OnWheelDown(int x, int y);
	

	uint16			AddControl(CLTGUICtrl* pCtrl);
	void			RemoveControl(CLTGUICtrl* pControl,LTBOOL bDelete = LTTRUE);

	uint16			SetSelection(uint16 select, LTBOOL bFindSelectable = LTFALSE);
	uint16			NextSelection();
	uint16			PreviousSelection();
	inline uint16	GetSelection() const		{return m_nSelection;}
	inline uint16	GetOldSelection() const		{return m_nOldSelection;}

	void			SetCapture(CLTGUICtrl *pCtrl)	{m_pCaptureCtrl = pCtrl;}
	CLTGUICtrl*		GetCapture()					{return m_pCaptureCtrl;}

	inline CLTGUICtrl*		GetSelectedControl()  {return GetControl(m_nSelection);}
	CLTGUICtrl*				GetControl(uint16 nIndex);
	uint16					GetIndex(CLTGUICtrl* pCtrl);

	virtual void	ScreenDimsChanged();
	
	void			SetRenderState();


	//this function creates a string, the caller must free it
    virtual         void GetHelpString(uint32 dwHelpId, uint16 nControlIndex, char *buffer, int bufLen);

  	// Calls UpdateData on each control in the screen
    virtual void            UpdateData(LTBOOL bSaveAndValidate=LTTRUE);

	void AddDummyCtrl();
	void EnablePopup( LTBOOL bEnable, CLTGUICtrl* pCaptureCtrl );

protected:

	// Handle input
    virtual LTBOOL		OnUp();
    virtual LTBOOL		OnDown();
    virtual LTBOOL		OnLeft();
    virtual LTBOOL		OnRight();
    virtual LTBOOL		OnEnter();
	virtual LTBOOL		OnEscape();

	// Gets the index of the control that is under the specific screen point.
	// Returns FALSE if there isn't one under the specified point.
    LTBOOL       GetControlUnderPoint(int xPos, int yPos, uint16 *pnIndex);

protected:

	void			RemoveAll(LTBOOL bDelete = LTTRUE);

	virtual void	CreateInterfaceSFX();
	virtual void	RemoveInterfaceSFX();

	CBaseScaleFX*	CreateScaleFX(char *szFXName);

	void			CreateLightFX(char *szFXName);
	
	//virtual void			CreateCharFX(INT_CHAR *pChar);
	// ±¤½Ä
	//------------------------------------------------------------------
	void			CreateCharFX( INT_CHAR *pChar, int nTeam );
	void			CreateAttachFX(INT_ATTACH *pAttach, int nTeam);
	void			Update_Model();
	//------------------------------------------------------------------
	// ±¤½Ä
	//virtual void	CreateAttachFX(INT_ATTACH *pAttach);
	virtual void	ClearAttachFX();

	void			SetPolyRenderState();
	void			InitPoly(LTPoly_GT4* pPoly, LTIntPt pos, HTEXTURE hTex);
	void			ScalePoly(LTPoly_GT4* pPoly, LTIntPt pos, HTEXTURE hTex);

protected:
// Added By IN'T ==>
	bool			m_bUseScreenAfterFX;
// Added By IN'T <==

    LTBOOL			m_bInit;
    LTBOOL			m_bBuilt;
	LTBOOL			m_bVisited;

	
	CScreenMgr*		m_pScreenMgr;

	int				m_nScreenID;		// The ID of this screen
	int				m_nContinueID;		// The ID of the screen to show when continue is clicked

	LTBOOL			m_bHaveFocus;

	LTBOOL			m_bHaveLights;

	// Array of SFX owned by this screen
	SFXArray		m_SFXArray;

	// Array of free controls that this screen owns
	ControlArray		m_controlArray;

	CLTGUICtrl*		m_pCaptureCtrl;

    //CBaseScaleFX	m_CharSFX;
	int				m_nNumAttachments;
	AttachmentData	m_aAttachment[MAX_INT_ATTACHMENTS];
	// ±¤½Ä
	CBaseScaleFX	m_CharacterSFX[2];

	LTIntPt			m_MousePt;

	LTFLOAT			m_fLastScale;
    LTBOOL			m_bSelectFXCenter;

	uint16			m_nSelection;
	uint16			m_nOldSelection;
	uint16			m_nLMouseDownItemSel;
	uint16			m_nRMouseDownItemSel;

	//ChainFXList		m_Chains;

    static LTVector		s_vPos; 
	static LTVector		s_vU; 
	static LTVector		s_vR; 
	static LTVector		s_vF;
    static LTRotation	s_rRot;
	
	//[MURSUM]
	float	m_fStartSel;
	int		m_nTooltipSelIndex;
	LTBOOL	m_bDrawTooltip;
	CSAD3DText*	m_pTooltipText;

	LTPoly_GT4	m_PolyTooltipBack;
	LTPoly_GT4	m_PolyTooltipBorder;

	CLTGUIDummyCtrl*	m_pDummyCtrl;
	
	std::vector<CSAD3DText*>	m_TextList;

	void AddTextToList( CSAD3DText* pText );
	BOOL ClearTextList();
};

#endif // _BASE_SCREEN_H_

