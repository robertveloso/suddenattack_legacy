// PreProcessorDlg.h : header file
//

#include "regmgr.h"

/////////////////////////////////////////////////////////////////////////////
// CPreProcessorDlg dialog

class CPreProcessorDlg : public CDialog
{
// Construction
public:
	CPreProcessorDlg(CWnd* pParent = NULL);	// standard constructor
	~CPreProcessorDlg();

	// Called when the thread is done.
	void		Shutdown();


// Dialog Data
	//{{AFX_DATA(CPreProcessorDlg)
	enum { IDD = IDD_PREPROCESSOR_DIALOG };
	CSliderCtrl	m_Priority;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreProcessorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL



// Implementation
protected:
	HICON m_hIcon;
	CRegMgr m_cRegMgr;

	// Generated message map functions
	//{{AFX_MSG(CPreProcessorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
