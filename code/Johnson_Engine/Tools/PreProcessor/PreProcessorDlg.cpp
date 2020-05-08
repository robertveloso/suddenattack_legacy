// PreProcessorDlg.cpp : implementation file
//

#include "bdefs.h"
#include "preprocessor.h"
#include "preprocessordlg.h"
#include "preprocessorthread.h"
#include "processingoptionsdlg.h"

#include "proplist.h"
#include "editobjects.h"

#include <direct.h>


char				g_AppName[] = "PreProcessor";
CPreProcessorDlg	*g_pDlg = NULL;
CWinThread			*g_pPreprocessorThread=NULL;
CProcessorGlobs		g_Globs;




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreProcessorDlg dialog

CPreProcessorDlg::CPreProcessorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPreProcessorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreProcessorDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CPreProcessorDlg::~CPreProcessorDlg()
{
	DWORD		code;

	if( g_pPreprocessorThread )
	{
		GetExitCodeThread( g_pPreprocessorThread->m_hThread, (LPDWORD) &code );
		TerminateThread( g_pPreprocessorThread->m_hThread, code );
		delete g_pPreprocessorThread;
	}
}


void CPreProcessorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreProcessorDlg)
	DDX_Control(pDX, IDC_PRIORITY, m_Priority);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreProcessorDlg, CDialog)
	//{{AFX_MSG_MAP(CPreProcessorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreProcessorDlg message handlers

void CPreProcessorDlg::Shutdown()
{
	EndDialog(0);
}


BOOL CPreProcessorDlg::OnInitDialog()
{
	CProcessingOptionsDlg optionsDlg;

	#ifdef _DEBUG
		afxMemDF = 0;
	#endif

	//setup the global control parameters
	memset(&g_Globs, 0, sizeof(g_Globs));

	//see if the input file exists
	CFileStatus DummyStatus;
	if((__argc < 2) || !CFile::GetStatus(__argv[1], DummyStatus))
	{
		if(MessageBox(_T("Unable to load the specified world, would you like to browse for one?"), "Unable to find world", MB_ICONQUESTION | MB_YESNO) == IDNO)
		{
			PostQuitMessage(0);
			return TRUE;
		}
		//they want to browse for a level, make sure we save the current working directory though
		//since the open dialog tends to mess that up
		char pszCurrWD[MAX_PATH];
		_getcwd(pszCurrWD, MAX_PATH);

		//loop until they pick a valid file
		do
		{
			CFileDialog Dlg(TRUE, "*.*", NULL, OFN_FILEMUSTEXIST, "World Files (*.lta; *.ltc)|*.lta;*.ltc|All Files (*.*)|*.*||");

			if(Dlg.DoModal() != IDOK)
			{
				//they cancelled
				PostQuitMessage(0);
				return TRUE;
			}

			//save this file
			strcpy(g_Globs.m_InputFile, Dlg.GetPathName());
		}
		while(!CFile::GetStatus(g_Globs.m_InputFile, DummyStatus));

		//restore our current working directory
		_chdir(pszCurrWD);
	}
	else
	{
		// Setup the starting options.
		strcpy(g_Globs.m_InputFile, __argv[1]);
	}

	//set up the output name to match the input name, but change the extension to
	//dat
	uint32 nCurrInChar		= 0;
	uint32 nExtensionPos	= 0;

	//copy the string, and identify where the extension starts
	while(g_Globs.m_InputFile[nCurrInChar] != '\0')
	{
		g_Globs.m_OutputFile[nCurrInChar] = g_Globs.m_InputFile[nCurrInChar];

		//see if we hit the extension
		if(g_Globs.m_InputFile[nCurrInChar] == '.')
		{
			nExtensionPos = nCurrInChar;
		}
		nCurrInChar++;
	}
	
	//now switch the extension
	if(nCurrInChar > 0)
	{
		strcpy(&(g_Globs.m_OutputFile[nExtensionPos]), ".ltb");
	}	

	//run the starting options dialog
	optionsDlg.m_pGlobs = &g_Globs;
	if(optionsDlg.DoModal() != IDOK)
	{
		PostQuitMessage(0);
		return TRUE;
	}

	
	CDialog::OnInitDialog();

  BOOL Init(const char* sCompany, const char* sApp, const char* sVersion, const char* sSubKey = NULL, HANDLE hRootKey = HKEY_LOCAL_MACHINE, char* sRoot2 = NULL);

	m_cRegMgr.Init("LithTech Inc.", "Jupiter", "PreProcessor", "Settings", HKEY_CURRENT_USER);

	m_Priority.SetRange(0, 5);
	m_Priority.SetPos(m_cRegMgr.Get("Priority",3)); // THREAD_PRIORITY_NORMAL...

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	g_pDlg = this;
	g_pPreprocessorThread = AfxBeginThread( RUNTIME_CLASS(CPreProcessorThread), THREAD_PRIORITY_NORMAL );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPreProcessorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPreProcessorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPreProcessorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CPreProcessorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int pos;

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	pos = m_Priority.GetPos();
	m_cRegMgr.Set("Priority",(DWORD)pos);

	if(g_pPreprocessorThread)
	{
		if(pos == 0)
			g_pPreprocessorThread->SetThreadPriority(THREAD_PRIORITY_IDLE);
		else if(pos == 1)
			g_pPreprocessorThread->SetThreadPriority(THREAD_PRIORITY_LOWEST);
		else if(pos == 2)
			g_pPreprocessorThread->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
		else if(pos == 3)
			g_pPreprocessorThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
		else if(pos == 4)
			g_pPreprocessorThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
		else if(pos == 5)
			g_pPreprocessorThread->SetThreadPriority(THREAD_PRIORITY_HIGHEST);
	}
}
