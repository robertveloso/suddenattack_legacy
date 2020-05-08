#ifndef __ADDPATCHDLG_H__
#define __ADDPATCHDLG_H__

// AddPatchDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddPatchDlg dialog

class CAddPatchDlg : public CDialog
{
// Construction
public:
	CAddPatchDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpherePrimitiveDlg)
	enum { IDD = IDD_ADDPATCH };

	CSpinButtonCtrl	m_spinXRes;
	CSpinButtonCtrl	m_spinYRes;
	CSpinButtonCtrl	m_spinXCP;
	CSpinButtonCtrl	m_spinYCP;

	int		m_nWidth;
	int		m_nHeight;

	int		m_nXSubdiv;
	int		m_nYSubdiv;

	int		m_nXPoints;
	int		m_nYPoints;

	CString	m_sName;

	int		m_nOrientation;

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
