// PreProcessorThread.cpp : implementation file
//

#include "bdefs.h"
#include <mmsystem.h>

#include "preprocessor.h"
#include "preprocessorthread.h"
#include "processing.h"
#include "preprocessordlg.h"



extern char g_AppName[];

extern CPreProcessorDlg		*g_pDlg;
extern CWinThread			*g_pPreprocessorThread;

extern CProcessorGlobs		g_Globs;


uint32 GetMSTime()
{
	return timeGetTime();
}

// All the system-dependent functions.
void SysDrawStatusText(const char *pMsg)
{
	char		msg[500];
	CWnd		*pWnd = g_pDlg->GetDlgItem(IDC_TEXT_BAR);
	int			nLen;

	if(pWnd)
	{
		sprintf(msg, "%s\r\n", pMsg);
		nLen = pWnd->SendMessage(EM_GETLIMITTEXT, 0, 0);
		pWnd->SendMessage(EM_SETSEL, nLen, nLen);
		pWnd->SendMessage(EM_REPLACESEL, FALSE, (LPARAM)msg);
	}
}



// Status stuff.
static CProgressCtrl *g_pCurrentBar=NULL;

static void InternalStartProgressBar(UINT text, UINT bar)
{
	g_pCurrentBar = (CProgressCtrl*)g_pDlg->GetDlgItem(bar);

	if(g_pCurrentBar)
	{
		g_pDlg->GetDlgItem( text )->ShowWindow(SW_SHOW);
		g_pDlg->GetDlgItem( bar )->ShowWindow(SW_SHOW);
		g_pCurrentBar->SetRange( 0, 1000 );
	}
}

static void InternalEndProgressBar(UINT text, UINT bar, BOOL bHide)
{
	if(g_pCurrentBar)
	{
		((CProgressCtrl*)g_pDlg->GetDlgItem(bar))->SetPos( 1000 );

		if( bHide )
		{
			g_pDlg->GetDlgItem( text )->ShowWindow( SW_SHOW );
			g_pDlg->GetDlgItem( bar )->ShowWindow( SW_SHOW );
		}

		g_pCurrentBar = NULL;
	}
}


void StartProgressBar(uint32 progressBar)
{
	if(progressBar == PROGRESS_BSP)
		InternalStartProgressBar(IDC_BSP_TEXT, IDC_BSP_PROGRESS);
	
	else if(progressBar == PROGRESS_LIGHT)
		InternalStartProgressBar(IDC_LIGHTING_TEXT, IDC_LIGHTING_PROGRESS);

	else if(progressBar == PROGRESS_HULL)
		InternalStartProgressBar(IDC_HULL_TEXT, IDC_HULL_PROGRESS);

	else if(progressBar == PROGRESS_VIS)
	{
		InternalStartProgressBar(IDC_VIS_TEXT, IDC_VIS_PROGRESS);
		g_pDlg->SetDlgItemText(IDC_VIS_TEXT, "Approximation");
	}
	
	else if(progressBar == PROGRESS_SETUP)
		InternalStartProgressBar(IDC_SETTINGUP_TEXT, IDC_SETTINGUP_PROGRESS);
}

void EndProgressBar(uint32 progressBar, bool bHide)
{
	if(progressBar == PROGRESS_BSP)
		InternalEndProgressBar(IDC_BSP_TEXT, IDC_BSP_PROGRESS, bHide);
	
	else if(progressBar == PROGRESS_LIGHT)
		InternalEndProgressBar(IDC_LIGHTING_TEXT, IDC_LIGHTING_PROGRESS, bHide);

	else if(progressBar == PROGRESS_HULL)
		InternalEndProgressBar(IDC_HULL_TEXT, IDC_HULL_PROGRESS, bHide);

	else if(progressBar == PROGRESS_VIS)
		InternalEndProgressBar(IDC_VIS_TEXT, IDC_VIS_PROGRESS, bHide);
	
	else if(progressBar == PROGRESS_SETUP)
		InternalEndProgressBar(IDC_SETTINGUP_TEXT, IDC_SETTINGUP_PROGRESS, bHide);
}

void SetCurrentProgressBar(float percent)
{
	if(g_pCurrentBar)
	{
		g_pCurrentBar->SetPos((int)(percent*1000.0f));
	}
}

void SetInFullVis()
{
	g_pDlg->SetDlgItemText(IDC_VIS_TEXT, "Full Optimization");
}

uint32 AskQuestion(const char *pQuestion, uint32 type)
{
	uint32		mbType = MB_YESNO;
	int32		ret;

	if((type & QUES_YES) && (type & QUES_NO) && (type & QUES_CANCEL))
		mbType = MB_YESNOCANCEL;
	else if((type & QUES_YES) && (type & QUES_NO))
		mbType = MB_YESNO;


	ret = g_pDlg->MessageBox(pQuestion, g_AppName, mbType);
	if(ret == IDYES)
	{
		ret = QUES_YES;
	}
	else if(ret == IDNO)
	{
		ret = QUES_NO;
	}
	else if(ret == IDCANCEL)
	{
		ret = QUES_CANCEL;
	}

	return (uint32)ret;
}



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreProcessorThread

IMPLEMENT_DYNCREATE(CPreProcessorThread, CWinThread)

CPreProcessorThread::CPreProcessorThread()
{
}

CPreProcessorThread::~CPreProcessorThread()
{
}

BOOL CPreProcessorThread::InitInstance()
{
	CWnd	*pOkButton;

	DoProcessing(&g_Globs);
	g_pPreprocessorThread = NULL;
	
	// If the command line requested to skip the
	// dialog window, let's skip ALL the dialog windows.
	if(g_Globs.m_bSkipDialog)
	{
		g_pDlg->EndDialog( IDOK );
		return FALSE;
	}

	if(pOkButton = g_pDlg->GetDlgItem(IDOK))
	{
		pOkButton->ShowWindow(SW_SHOW);
		pOkButton->SetFocus();
	}

	// Ding!
	PlaySound("Default", NULL, SND_SYNC | SND_NOWAIT | SND_NOSTOP);

	return FALSE;
}

int CPreProcessorThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPreProcessorThread, CWinThread)
	//{{AFX_MSG_MAP(CPreProcessorThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreProcessorThread message handlers







