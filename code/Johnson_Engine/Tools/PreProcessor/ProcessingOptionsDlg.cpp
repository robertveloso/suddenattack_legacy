// ProcessingOptionsDlg.cpp : implementation file
//

#include "bdefs.h"
#include "preprocessor.h"
#include "processingoptionsdlg.h"
#include "de_world.h"
#include "rpdmanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcessingOptionsDlg dialog


CProcessingOptionsDlg::CProcessingOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessingOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcessingOptionsDlg)
	//}}AFX_DATA_INIT
}


void CProcessingOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcessingOptionsDlg)
	//DDX_Control(pDX, IDC_LAMBERTIAN, m_LambertianLightmaps);
	DDX_Control(pDX, IDC_FIXTJUNC, m_FixTJunc);
	DDX_Control(pDX, IDC_PROJECTDIR, m_ProjectDir);
	DDX_Control(pDX, IDC_EXTRAPARAMETERS, m_ExtraParams);
	DDX_Control(pDX, IDC_OBJECTSONLY, m_ObjectsOnly);
	DDX_Control(pDX, IDC_APPLYLIGHTING, m_Light);
	DDX_Control(pDX, IDC_LAMBERT_LIGHTING, m_LambertLight);
	DDX_Control(pDX, IDC_LAMBERTIAN_LIGHTMAPS, m_LambertianLightmaps);
	DDX_Control(pDX, IDC_VERTEXLIGHTING, m_VertexLighting);
	DDX_Control(pDX, IDC_VOLUMETRICAMBIENT, m_VolumetricAmbient);
	DDX_Control(pDX, IDC_LIGHTANIMATIONS, m_LightAnimations);
	DDX_Control(pDX, IDC_SHADOWS, m_Shadows);
	DDX_Control(pDX, IDC_LOGFILE, m_LogFile);
	DDX_Control(pDX, IDC_ERRORLOG, m_ErrorLog);
	DDX_Control(pDX, IDC_BALANCESLIDER, m_BalanceSlider);
	DDX_Control(pDX, IDC_IGNOREHIDDEN, m_IgnoreHidden);
	DDX_Control(pDX, IDC_IGNOREFROZEN, m_IgnoreFrozen);
	DDX_Control(pDX, IDC_LIGHTMAP_SUPER_SAMPLE, m_LMSuperSample);
	DDX_Control(pDX, IDC_NUM_SUPER_SAMPLES, m_LMNumSamples);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcessingOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CProcessingOptionsDlg)
	ON_BN_CLICKED(IDC_OBJECTSONLY, OnObjectsOnly)
	ON_BN_CLICKED(IDC_APPLYLIGHTING, OnApplyLighting)
	ON_BN_CLICKED(IDC_LAMBERT_LIGHTING, OnLambertLighting)
	ON_BN_CLICKED(IDC_LAMBERTIAN_LIGHTMAPS, OnLambertianLightmaps)
	ON_BN_CLICKED(IDC_BROWSEPROJDIR, OnBrowseProjectPath)
	ON_BN_CLICKED(IDC_LIGHTMAP_SUPER_SAMPLE, OnLightmapSuperSample)
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CProcessingOptionsDlg::SetInterfaceState()
{
	//determine what type of process is going on
	bool bObjectsOnly = m_pGlobs->m_bObjectsOnly;
	m_ObjectsOnly.SetCheck(m_pGlobs->m_bObjectsOnly);
	
	m_IgnoreHidden.SetCheck(m_pGlobs->m_bIgnoreHidden);
	m_IgnoreFrozen.SetCheck(m_pGlobs->m_bIgnoreFrozen);

	m_FixTJunc.SetCheck(m_pGlobs->m_bFixTJunc);
	m_FixTJunc.EnableWindow(!bObjectsOnly);

	// Lighting.

	bool bLightEnable = m_pGlobs->m_bLight && !bObjectsOnly;

	m_Light.EnableWindow(!bObjectsOnly);
	m_Light.SetCheck(m_pGlobs->m_bLight);

	m_LightAnimations.EnableWindow(bLightEnable);
	m_Shadows.EnableWindow(bLightEnable);
	m_VertexLighting.EnableWindow(bLightEnable);
	m_VolumetricAmbient.EnableWindow(bLightEnable);
	m_LambertLight.EnableWindow(bLightEnable);
	m_LambertianLightmaps.EnableWindow(bLightEnable);
	m_LMSuperSample.EnableWindow(bLightEnable);
	m_LMNumSamples.EnableWindow(m_pGlobs->m_bLMSuperSample && bLightEnable);
}

static inline BOOL IsTerminator(char toTest)
{
	return (toTest == 0) || (toTest == '\n') || (toTest == ' ');
}

static char* GetNextArg(char* &pIn, char* &pOut)
{
	char *pRet;

	while(pIn[0] == ' ')
		++pIn;

	if(IsTerminator(pIn[0]))
		return NULL;

	pRet = pOut;
	while(!IsTerminator(pIn[0]))
	{
		*pOut = *pIn;
		pOut++;
		pIn++;
	}

	*pOut++ = 0;
	if(pRet[0] == 0)
		return NULL;
	else
		return pRet;
}


void CProcessingOptionsDlg::ParseArguments(char *pStr)
{
	char *pCurInPos, *pCurOutPos;
	
	m_nArgs = 0;
	pCurInPos = pStr;
	pCurOutPos = m_TokenSpace;
	while(m_ArgPointers[m_nArgs] = GetNextArg(pCurInPos, pCurOutPos))
	{
		++m_nArgs;
	}
}


void CProcessingOptionsDlg::ReadFinalOptions()
{
	m_pGlobs->m_bLightAnimations		= !!m_LightAnimations.GetCheck();
	m_pGlobs->m_bShadows				= !!m_Shadows.GetCheck();
	m_pGlobs->m_bLogFile				= !!m_LogFile.GetCheck();
	m_pGlobs->m_bErrorLog				= !!m_ErrorLog.GetCheck();
	m_pGlobs->m_bVerticesOnly			= !!m_VertexLighting.GetCheck();
	m_pGlobs->m_bVolumetricAmbient		= !!m_VolumetricAmbient.GetCheck();
	m_pGlobs->m_bLambertianLightmaps	= !!m_LambertianLightmaps.GetCheck();
	m_pGlobs->m_bObjectsOnly			= !!m_ObjectsOnly.GetCheck();
	m_pGlobs->m_bIgnoreHidden			= !!m_IgnoreHidden.GetCheck();
	m_pGlobs->m_bIgnoreFrozen			= !!m_IgnoreFrozen.GetCheck();
	m_pGlobs->m_bLambertLight			= !!m_LambertLight.GetCheck();
	m_pGlobs->m_bFixTJunc				= !!m_FixTJunc.GetCheck();
	m_pGlobs->m_bLMSuperSample			= !!m_LMSuperSample.GetCheck();

	//get the number of lightmap samples from the edit control
	CString sNumSamples;
	m_LMNumSamples.GetWindowText(sNumSamples);
	m_pGlobs->m_nLMNumSamplesOnSide		= atoi(sNumSamples);
	
	//read in the project directory
	m_ProjectDir.GetWindowText(m_pGlobs->m_ProjectDir, sizeof(m_pGlobs->m_ProjectDir)-1);

	//get the BSP weights
	m_pGlobs->m_BalanceWeight = (float)m_BalanceSlider.GetPos() / 1000.0f;
	m_pGlobs->m_SplitWeight = 1.0f - m_pGlobs->m_BalanceWeight;

	//get the platform being compiled for
	CString sPlatform;
	((CComboBox*)GetDlgItem(IDC_PLATFORM_DROPDOWN))->GetWindowText(sPlatform);
	strncpy(m_pGlobs->m_Platform, (const char*)sPlatform, MAX_PLATFORM_NAME_LEN);
}


void CProcessingOptionsDlg::InitStartingOptions()
{
	float ratio;
	
	m_LightAnimations.SetCheck(m_pGlobs->m_bLightAnimations);
	m_Shadows.SetCheck(m_pGlobs->m_bShadows);
	m_LogFile.SetCheck(m_pGlobs->m_bLogFile);
	m_ErrorLog.SetCheck(m_pGlobs->m_bErrorLog);
	m_VertexLighting.SetCheck(m_pGlobs->m_bVerticesOnly);
	m_VolumetricAmbient.SetCheck(m_pGlobs->m_bVolumetricAmbient);
	m_LambertianLightmaps.SetCheck(m_pGlobs->m_bLambertianLightmaps);
	m_IgnoreHidden.SetCheck(m_pGlobs->m_bIgnoreHidden);
	m_IgnoreFrozen.SetCheck(m_pGlobs->m_bIgnoreFrozen);
	m_LambertLight.SetCheck(m_pGlobs->m_bLambertLight);
	m_FixTJunc.SetCheck(m_pGlobs->m_bFixTJunc);
	m_LMSuperSample.SetCheck(m_pGlobs->m_bLMSuperSample);

	//setup the number of samples
	CString sNumSamples;
	sNumSamples.Format("%d", m_pGlobs->m_nLMNumSamplesOnSide);
	m_LMNumSamples.SetWindowText(sNumSamples);

	m_ProjectDir.SetWindowText(m_pGlobs->m_ProjectDir);

	ratio = m_pGlobs->m_BalanceWeight / (m_pGlobs->m_BalanceWeight + m_pGlobs->m_SplitWeight);
	m_BalanceSlider.SetRangeMin(0);
	m_BalanceSlider.SetRangeMax(1000);
	m_BalanceSlider.SetPos((int)(ratio * 1000.0f));

	//get the platform being compiled for
	int nPlatformPos = ((CComboBox*)GetDlgItem(IDC_PLATFORM_DROPDOWN))->FindStringExact(-1, m_pGlobs->m_Platform);
	if(nPlatformPos == CB_ERR)
	{
		//couldn't find the platform they specified, so just select the first one
		nPlatformPos = 0;
	}
		
	//select the platform
	((CComboBox*)GetDlgItem(IDC_PLATFORM_DROPDOWN))->SetCurSel(nPlatformPos);
	

}


void CProcessingOptionsDlg::MakeFinalStrings(CString &mainString, CString &extraParams)
{
	char str[100], extraStr[400];

	if(m_pGlobs->m_bObjectsOnly)
	{
		mainString += "-ObjectsOnly ";
	}

	if(m_pGlobs->m_bIgnoreHidden)
	{
		mainString += "-ignorehidden ";
	}

	if(!m_pGlobs->m_bCenterWorldAroundOrigin)
	{
		mainString += "-dontcenterworld ";
	}

	if(m_pGlobs->m_bIgnoreFrozen)
	{
		mainString += "-ignorefrozen ";
	}

	if(m_pGlobs->m_bFixTJunc)
	{
		mainString += "-FixTJunc ";
	}

	if(m_pGlobs->m_bLight)
	{
		mainString += "-light ";
	}

	if(!m_pGlobs->m_bShadows)
	{
		mainString += "-nocliplight ";
	}

	if(m_pGlobs->m_bLightAnimations)
	{
		mainString += "-lightanims ";
	}

	if(m_pGlobs->m_bVerticesOnly)
	{
		mainString += "-onlylightverts ";
	}

 
	if(m_pGlobs->m_bLambertianLightmaps)
	{
		mainString += "-LambertianLightmaps ";
	}

	if(m_pGlobs->m_bLMSuperSample)
	{
		CString sSuperSampleString;
		sSuperSampleString.Format("-LMSuperSample %d ", m_pGlobs->m_nLMNumSamplesOnSide);
		mainString += sSuperSampleString;
	}

	if(m_pGlobs->m_bLambertLight)
	{
		mainString += "-LambertLight ";
	}

	if(m_pGlobs->m_bShadowMesh)
	{
		mainString += "-ShadowMesh ";
	}

	if(m_pGlobs->m_bVolumetricAmbient)
	{
		mainString += "-VolumetricAmbient ";
	}

	if(m_pGlobs->m_bLogFile)
	{
		mainString += "-logfile ";
	}

	if(m_pGlobs->m_bErrorLog)
	{
		mainString += "-errorlog ";
	}

	//only output the project directory if it was entered
	//otherwise it will accidentally grab the split weight
	if(strlen(m_pGlobs->m_ProjectDir) > 0)
	{
		mainString += "-projectdir ";
		mainString += m_pGlobs->m_ProjectDir;
		mainString += " ";
	}

	//if there is a platform set, we should save that too
	if(strlen(m_pGlobs->m_Platform) > 0)
	{
		mainString += "-Platform ";
		mainString += m_pGlobs->m_Platform;
		mainString += " ";
	}

	sprintf(str, "-splitweight %f ", m_pGlobs->m_SplitWeight);
	mainString += str;

	m_ExtraParams.GetWindowText(extraStr, 400);
	extraParams = extraStr;
}


//given a filename, it will produce the appropriate filename for the DAT,
void GetDATFilename(const char* pszInFileName, char* pszOutFileName, uint32 nLen)
{
	ASSERT(pszInFileName);
	ASSERT(pszOutFileName);

	int32 nStrLen = strlen(pszInFileName);
	int32 nPeriodPos = nStrLen;

	for(int32 nCurrPos = nStrLen; nCurrPos >= 0; nCurrPos--)
	{
		//see if we hit a period
		if(pszInFileName[nCurrPos] == '.')
		{
			nPeriodPos = nCurrPos + 1;
			break;
		}
		//see if we hit a slash
		if((pszInFileName[nCurrPos] == '\\') || (pszInFileName[nCurrPos] == '/'))
		{
			break;
		}
	}

	uint32 nCopyLen = LTMIN(nLen, (uint32)nPeriodPos);

	//copy over the original string
	strncpy(pszOutFileName, pszInFileName, nCopyLen);

	//make sure that there is an end of string
	pszOutFileName[nCopyLen - 1] = '\0';

	//now tack on the extension
	strncat(pszOutFileName, ".dat", nLen);

	//success
}

/////////////////////////////////////////////////////////////////////////////
// CProcessingOptionsDlg message handlers

BOOL CProcessingOptionsDlg::OnInitDialog() 
{
	FILE *fp;
	char mainCommandLine[600];
	char extraString[600];
	CString windowTitle;
	CRect advancedRect;


	// Init basic default options.
	m_pGlobs->m_MaxDetail				= 10000;
	m_pGlobs->m_SplitWeight				= 1.0f;
	m_pGlobs->m_BalanceWeight			= 1.0f;
	m_pGlobs->m_bLogFile				= false;
	m_pGlobs->m_bErrorLog				= false;
	m_pGlobs->m_bLight					= false;
	m_pGlobs->m_bShadows				= true;
	m_pGlobs->m_bSkipDialog				= false;
	m_pGlobs->m_bIgnoreHidden			= false;
	m_pGlobs->m_bIgnoreFrozen			= false;
	m_pGlobs->m_bLambertLight			= true;
	m_pGlobs->m_bLambertianLightmaps	= false;
	m_pGlobs->m_bFixTJunc				= true;
	m_pGlobs->m_bLMSuperSample			= false;
	m_pGlobs->m_ProjectDir[0]			= '\0';


	//init the text buffers
	extraString[0]		= '\0';
	mainCommandLine[0]	= '\0';

	char pszRPDFile[MAX_PATH];
	GetDATFilename(m_pGlobs->m_InputFile, pszRPDFile, MAX_PATH);

	//determine if the RPD file exists
	CFileStatus Status;

	m_bDATFileExists = FALSE;
	if(CFile::GetStatus(pszRPDFile, Status))
	{
		//the file exists, but now we need to check and see if it is read only
		m_bDATFileExists = TRUE;
	}
	
	// Process options from the file.
	if(fp = fopen("processor.opt", "rt"))
	{
		fgets(mainCommandLine, 600, fp);
		fgets(extraString, 600, fp);

		if(strlen(mainCommandLine) > 0)
			mainCommandLine[strlen(mainCommandLine) - 1] = 0;
		
		if(strlen(extraString) > 0)
			extraString[strlen(extraString) - 1] = 0;
		
		ParseArguments(mainCommandLine);

		m_pGlobs->m_Argc = m_nArgs;
		m_pGlobs->m_Argv = m_ArgPointers;
		ProcessOptions(m_pGlobs);

		fclose(fp);
	}
	else
	{
		//there was no processor.opt file, so what we want to do is
		//create some better defaults
		ProcessOptions(m_pGlobs);
		m_pGlobs->m_bLight						= true;
	}

	//don't allow object only processing if there isn't an RPD file
	if(!m_bDATFileExists)
		m_pGlobs->m_bObjectsOnly		= false;

	if(m_pGlobs->m_bSkipDialog)
	{
		EndDialog(IDOK);
		return TRUE;
	}


	CDialog::OnInitDialog();


	windowTitle.FormatMessage(IDS_PROCESSINGOPTIONSTITLE, m_pGlobs->m_InputFile);
	SetWindowText(windowTitle);

	
	//have the combo box automatically select the first item in the dropdown box
	((CComboBox*)GetDlgItem(IDC_PLATFORM_DROPDOWN))->SetCurSel(0);


	// Now setup our interface.
	GetWindowRect(&m_FullRect);
	
	m_ExtraParams.SetWindowText(extraString);
	
	InitStartingOptions();
	SetInterfaceState();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CProcessingOptionsDlg::OnObjectsOnly() 
{
	m_pGlobs->m_bObjectsOnly = !!m_ObjectsOnly.GetCheck();
	SetInterfaceState();
}

void CProcessingOptionsDlg::OnApplyLighting() 
{
	m_pGlobs->m_bLight = !!m_Light.GetCheck();
	SetInterfaceState();
}

void CProcessingOptionsDlg::OnLambertLighting()
{
	m_pGlobs->m_bLambertLight = !!m_LambertLight.GetCheck();
	SetInterfaceState();
}

void CProcessingOptionsDlg::OnLightmapSuperSample()
{
	m_pGlobs->m_bLMSuperSample = !!m_LMSuperSample.GetCheck();
	SetInterfaceState();
}

void CProcessingOptionsDlg::OnLambertianLightmaps() 
{
	m_pGlobs->m_bLambertianLightmaps = !!m_Light.GetCheck();
	SetInterfaceState();
}

void CProcessingOptionsDlg::OnOK() 
{
	CString mainString, extraString, fullString;
	FILE *fp;

	// Read the final options in.
	ReadFinalOptions();

	// Process ALL the options.
	MakeFinalStrings(mainString, extraString);
	fullString = mainString + " " + extraString;

	ParseArguments((char*)(LPCTSTR)fullString);
	m_pGlobs->m_Argc = m_nArgs;
	m_pGlobs->m_Argv = m_ArgPointers;
	ProcessOptions(m_pGlobs);

	// Save the options.
	if(fp = fopen("processor.opt", "wt"))
	{
		fprintf(fp, "%s\n", (LPCTSTR)mainString);
		fprintf(fp, "%s\n", (LPCTSTR)extraString);
		fclose(fp);
	}

	CDialog::OnOK();
}

//brings up a file open dialog for the user to search for the dep file, then
//takes the filename, gets the path, and uses that for the project path
void CProcessingOptionsDlg::OnBrowseProjectPath() 
{
	//save the current working directory, since the file dialog messes it up
	char pszWorkingDir[MAX_PATH];

	GetCurrentDirectory(sizeof(pszWorkingDir), pszWorkingDir);

	//maybe make the default file so that it looks into the current path directory?
	CFileDialog FileDlg(	TRUE, "*.dep", NULL, OFN_FILEMUSTEXIST, 
							"DEdit Project Files (*.dep)|*.dep|All Files (*.*)|*.*||");

	//prompt the user
	if(FileDlg.DoModal() == IDOK)
	{
		//they selected a file
		
		//get the string
		CString sFilePath = FileDlg.GetPathName();

		//now get the file name
		CString sFileName = FileDlg.GetFileName();

		//make sure that the path is larger than the filename
		if(sFileName.GetLength() <= sFilePath.GetLength())
		{
			//strip out the file name
			sFilePath = sFilePath.Left(sFilePath.GetLength() - sFileName.GetLength());
		}

		//now set this string in the edit box
		CEdit* pProjPathBox = (CEdit*)GetDlgItem(IDC_PROJECTDIR);
		ASSERT(pProjPathBox);
		pProjPathBox->SetWindowText(sFilePath);
	}

	SetCurrentDirectory(pszWorkingDir);
}

void CProcessingOptionsDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonUp(nFlags, point);
}

void CProcessingOptionsDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CProcessingOptionsDlg::OnSplitpolyVisbsp() 
{
	m_pGlobs->m_bSplitPolyForVisBSP = !!m_SplitPolyForVisBSP.GetCheck();
	SetInterfaceState();
}
