// AddPatchDlg.cpp : implementation file
//

#include "bdefs.h"
#include "dedit.h"
#include "setpatchresdlg.h"
#include "editpatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddPatchDlg dialog


CSetPatchResDlg::CSetPatchResDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetPatchResDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddPatchDlg)
	m_nXRes		= DEFAULT_PATCH_RESOLUTION;
	m_nYRes		= DEFAULT_PATCH_RESOLUTION;
	//}}AFX_DATA_INIT
}


void CSetPatchResDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpherePrimitiveDlg)
	DDX_Control(pDX, IDC_SPIN_SET_PATCH_RES_X, m_spinXRes);
	DDX_Control(pDX, IDC_SPIN_SET_PATCH_RES_Y, m_spinYRes);

	DDX_Text(pDX, IDC_EDIT_SET_PATCH_RESOLUTION_X, m_nXRes);
	DDV_MinMaxInt(pDX, m_nXRes, 3, MAX_PATCH_RESOLUTION);
	DDX_Text(pDX, IDC_EDIT_SET_PATCH_RESOLUTION_Y, m_nYRes);
	DDV_MinMaxInt(pDX, m_nYRes, 3, MAX_PATCH_RESOLUTION);

	//}}AFX_DATA_MAP
}

BOOL CSetPatchResDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_spinXRes.SetRange(3, MAX_PATCH_RESOLUTION);
	m_spinYRes.SetRange(3, MAX_PATCH_RESOLUTION);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CSetPatchResDlg, CDialog)
	//{{AFX_MSG_MAP(CSpherePrimitiveDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

