#ifndef __SETPATCHRESDLG_H__
#define __SETPATCHRESDLG_H__

// AddPatchDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddPatchDlg dialog

class CSetPatchResDlg : public CDialog
{
// Construction
public:
	CSetPatchResDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpherePrimitiveDlg)
	enum { IDD = IDD_SETPATCHRESOLUTION };

	CSpinButtonCtrl	m_spinXRes;
	CSpinButtonCtrl	m_spinYRes;

	int		m_nXRes;
	int		m_nYRes;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpherePrimitiveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpherePrimitiveDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif 
