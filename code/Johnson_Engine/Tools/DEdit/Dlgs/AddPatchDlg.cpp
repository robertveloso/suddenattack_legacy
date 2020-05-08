// AddPatchDlg.cpp : implementation file
//

#include "bdefs.h"
#include "dedit.h"
#include "addpatchdlg.h"
#include "bezierpatch.h"
#include "editpatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddPatchDlg dialog


CAddPatchDlg::CAddPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddPatchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddPatchDlg)
	m_nXPoints		= 4;
	m_nYPoints		= 4;
	m_nXSubdiv		= DEFAULT_PATCH_RESOLUTION;
	m_nYSubdiv		= DEFAULT_PATCH_RESOLUTION;
	m_nWidth		= 256;
	m_nHeight		= 256;
	m_sName			= "Patch";
	m_nOrientation	= 0;
	//}}AFX_DATA_INIT
}


void CAddPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpherePrimitiveDlg)

	DDX_Control(pDX, IDC_SPIN_ADD_PATCH_X_RES, m_spinXRes);
	DDX_Control(pDX, IDC_SPIN_ADD_PATCH_Y_RES, m_spinYRes);
	DDX_Control(pDX, IDC_SPIN_ADD_PATCH_X_CP, m_spinXCP);
	DDX_Control(pDX, IDC_SPIN_ADD_PATCH_Y_CP, m_spinYCP);

	DDX_Text(pDX, IDC_EDIT_PATCH_X_POINTS, m_nXPoints);
	DDV_MinMaxInt(pDX, m_nXPoints, 3, CBezierPatch::MAX_DIMENSION);
	DDX_Text(pDX, IDC_EDIT_PATCH_Y_POINTS, m_nYPoints);
	DDV_MinMaxInt(pDX, m_nYPoints, 3, CBezierPatch::MAX_DIMENSION);

	DDX_Text(pDX, IDC_EDIT_PATCH_X_SUBDIV, m_nXSubdiv);
	DDV_MinMaxInt(pDX, m_nXSubdiv, 3, MAX_PATCH_RESOLUTION);
	DDX_Text(pDX, IDC_EDIT_PATCH_Y_SUBDIV, m_nYSubdiv);
	DDV_MinMaxInt(pDX, m_nYSubdiv, 3, MAX_PATCH_RESOLUTION);

	DDX_Text(pDX, IDC_EDIT_PATCH_WIDTH, m_nWidth);
	DDX_Text(pDX, IDC_EDIT_PATCH_HEIGHT, m_nHeight);

	DDX_Text(pDX, IDC_EDIT_PATCH_NAME, m_sName);

	DDX_Radio(pDX, IDC_RADIO_PATCH_XY_POS_PLANE, m_nOrientation);
	//}}AFX_DATA_MAP
}

BOOL CAddPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_spinXRes.SetRange(3, MAX_PATCH_RESOLUTION);
	m_spinYRes.SetRange(3, MAX_PATCH_RESOLUTION);
	m_spinXCP.SetRange(3, CBezierPatch::MAX_DIMENSION);
	m_spinYCP.SetRange(3, CBezierPatch::MAX_DIMENSION);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAddPatchDlg, CDialog)
	//{{AFX_MSG_MAP(CSpherePrimitiveDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

