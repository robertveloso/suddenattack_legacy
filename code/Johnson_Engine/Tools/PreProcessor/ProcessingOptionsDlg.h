#if !defined(AFX_PROCESSINGOPTIONSDLG_H__DF160206_53E6_11D1_93FA_A7108A20B522__INCLUDED_)
#define AFX_PROCESSINGOPTIONSDLG_H__DF160206_53E6_11D1_93FA_A7108A20B522__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ProcessingOptionsDlg.h : header file
//

#include "processing.h"


/////////////////////////////////////////////////////////////////////////////
// CProcessingOptionsDlg dialog

class CProcessingOptionsDlg : public CDialog
{
// Construction
public:
	CProcessingOptionsDlg(CWnd* pParent = NULL);   // standard constructor

	void SetInterfaceState();
	void ParseArguments(char *pStr);
	void MakeFinalStrings(CString &mainString, CString &extraParams);
	void ReadFinalOptions();
	void InitStartingOptions();

	CProcessorGlobs *m_pGlobs;
	
	CRect m_FullRect, m_PartialRect;
	BOOL m_bDATFileExists;

	int m_nArgs;
	char *m_ArgPointers[40];
	char m_TokenSpace[600];


// Dialog Data
	//{{AFX_DATA(CProcessingOptionsDlg)
	enum { IDD = IDD_PROCESSINGOPTIONS };

	CButton	m_LambertianLightmaps;
	CButton	m_SplitPolyForVisBSP;
	CButton	m_FixTJunc;
	CEdit	m_ProjectDir;
	CEdit	m_ExtraParams;
	CButton	m_Light;
	CButton m_LambertLight;
	CEdit	m_VisDist;
	CButton	m_VertexLighting;
	CButton	m_VolumetricAmbient;
	CButton m_LMSuperSample;
	CEdit	m_LMNumSamples;
	CButton	m_LightAnimations;
	CButton	m_Shadows;
	CButton	m_LogFile;
	CButton m_ErrorLog;
	CButton	m_ObjectsOnly;
	CSliderCtrl	m_BalanceSlider;
	CButton m_IgnoreHidden;
	CButton m_IgnoreFrozen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessingOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcessingOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdvanced();
	afx_msg void OnObjectsOnly();
	afx_msg void OnApplyLighting();
	afx_msg void OnLambertLighting();
	virtual void OnOK();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLightmapSuperSample();
	afx_msg void OnSplitpolyVisbsp();
	afx_msg void OnBrowseProjectPath();
	afx_msg void OnLambertianLightmaps();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSINGOPTIONSDLG_H__DF160206_53E6_11D1_93FA_A7108A20B522__INCLUDED_)
