//------------------------------------------------------------------
//
//   MODULE  : FXMGR.CPP
//
//   PURPOSE : Implements class CFxMgr
//
//   CREATED : On 10/2/98 At 5:33:14 PM
//
//------------------------------------------------------------------

// Includes....

#include "fxmgr.h"
#include "io.h"
#include "stdio.h"
#include "fxflags.h"
#include "sfxdefs.h"
#include "iltmessage.h"
#include "iltdrawprim.h"

#ifdef _WIN32
	#include "windows.h"
#endif

struct LINK_STATUS
{
	bool			m_bLinked;
	uint32			m_dwLinkedID;
	char			m_sLinkedNodeName[32];
};

// Globals....

uint32 g_dwID = 0;
ILTClient *CFxMgr::s_pClientDE = NULL;

//------------------------------------------------------------------
//
//   FUNCTION : CFxMgr()
//
//   PURPOSE  : Standard constuctor
//
//------------------------------------------------------------------

CFxMgr::CFxMgr()
{
	m_tmUpdateDelta   = 1.0f / 50.0f;
	m_tmLastUpdate    = 0.0f;
	m_pfnSetMultiplay = NULL;
	m_pfnDetailFunc	  = NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : ~CFxMgr
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CFxMgr::~CFxMgr()
{
	// Call Term()

	Term();
}

//------------------------------------------------------------------
//
//   FUNCTION : Init()
//
//   PURPOSE  : Initialises class CFxMgr
//
//------------------------------------------------------------------

bool CFxMgr::Init(CClientDE *pClientDE)
{
	// Enumerate the current directory for any .fxd dll's which 
	// contain the code implementation for special fx

	m_pClientDE = pClientDE;
	CFxMgr::s_pClientDE = pClientDE;

	_finddata_t fxdFiles;

	long handle = _findfirst("..\\*.fxd", &fxdFiles);
	if (handle == -1)
	{
		// Couldn't locate any .fxd files, no problem

		return true;
	}

	if (!LoadFxDll(fxdFiles.name)) return false;
	
	while (!_findnext(handle, &fxdFiles))
	{
		// This could be a valid .fxd so go ahead and try loading it.
		// If successful then load all the function pointers to
		// the specific fx classes

		if (!LoadFxDll(fxdFiles.name)) return false;
	}

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : Term()
//
//   PURPOSE  : Terminates class CFxMgr
//
//------------------------------------------------------------------

void CFxMgr::Term()
{
	m_collFX.RemoveAll();

	// Delete all the FX group instances

	CLinkListNode<CLIENTFX_INSTANCE *> *pInstNode = m_collActiveGroupFX.GetHead();

	while (pInstNode)
	{
		CLinkListNode<FX_LINK> *pLinkNode = pInstNode->m_Data->m_collActiveFX.GetHead();

		while (pLinkNode)
		{
			pLinkNode->m_Data.m_pFX->Term();
			
			pLinkNode = pLinkNode->m_pNext;
		}
		
		delete pInstNode->m_Data;
		
		pInstNode = pInstNode->m_pNext;
	}

	m_collActiveGroupFX.RemoveAll();

	// Delete all the FX group inactive instances

	CLinkListNode<CLIENTFX_INSTANCE *> *pInactiveInstNode = m_collInactiveGroupFX.GetHead();

	while (pInactiveInstNode)
	{
		delete pInactiveInstNode->m_Data;
		
		pInactiveInstNode = pInactiveInstNode->m_pNext;
	}

	m_collInactiveGroupFX.RemoveAll();

	// Delete all the FX groups

	CLinkListNode<FX_GROUP *> *pGroupNode = m_collGroupFX.GetHead();

	while (pGroupNode)
	{
		CLinkListNode<FX_KEY *> *pKeyNode = pGroupNode->m_Data->m_collFx.GetHead();

		while (pKeyNode)
		{
			delete pKeyNode->m_Data;

			pKeyNode = pKeyNode->m_pNext;
		}

		delete pGroupNode->m_Data;
		
		pGroupNode = pGroupNode->m_pNext;
	}

	m_collGroupFX.RemoveAll();

}

//------------------------------------------------------------------
//
//   FUNCTION : GetClientDE()
//
//   PURPOSE  : Returns static pointer to ILTClient interface
//
//------------------------------------------------------------------

ILTClient* CFxMgr::GetClientDE()
{
	return CFxMgr::s_pClientDE;
}

//------------------------------------------------------------------
//
//   FUNCTION : LoadFxDll()
//
//   PURPOSE  : Loads a specific fx dll
//
//------------------------------------------------------------------

bool CFxMgr::LoadFxDll(char *sName)
{

#ifdef WIN32
	// Load the library
	
	char sTmp[256];
	sprintf(sTmp, "%s", sName);

	HINSTANCE hInst = ::LoadLibrary(sTmp);
	if (!hInst) return true;

	// Attempt to retrieve the FX reference structure function

	FX_GETNUM pfnNum = (FX_GETNUM)::GetProcAddress(hInst, "fxGetNum");
	if (!pfnNum) return true;

	FX_GETREF pfnRef = (FX_GETREF)::GetProcAddress(hInst, "fxGetRef");
	if (!pfnRef) return true;

	FX_SETMULTIPLAYFUNC pfnMultiPlay = (FX_SETMULTIPLAYFUNC)::GetProcAddress(hInst, "fxSetMultiplay");
	if (!pfnMultiPlay) return true;

	FX_DETAILFUNC pfnDetail = (FX_DETAILFUNC)::GetProcAddress(hInst, "fxSetDetail");
	if (!pfnDetail) return true;

	FX_SETPLAYERFUNC pfnSetPlayer = (FX_SETPLAYERFUNC)::GetProcAddress(hInst, "fxSetPlayer");
	if (!pfnSetPlayer) return true;

	FX_SETPARAMS pfnSetParams = (FX_SETPARAMS)::GetProcAddress(hInst, "fxSetParams");
	if (!pfnSetParams) return true;

	m_pfnSetMultiplay = pfnMultiPlay;
	m_pfnDetailFunc   = pfnDetail;
	m_pfnSetPlayer    = pfnSetPlayer;
	m_pfnSetParams	  = pfnSetParams;

	// Okay, if we got here then this is a valid dll with some special
	// fx in it....

	int nFx = pfnNum();
	
	for (int i = 0; i < nFx; i ++)
	{
		// Retrieve the FX reference structure
		
		FX_REF fxRef = pfnRef(i);

		// Add it to the list of FX

		m_collFX.AddTail(fxRef);
	}
#endif

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : FindFX()
//
//   PURPOSE  : Returns a named FX
//
//------------------------------------------------------------------

FX_REF* CFxMgr::FindFX(char *sName)
{
	CLinkListNode<FX_REF> *pNode = m_collFX.GetHead();

	while (pNode)
	{
		if (!stricmp(sName, pNode->m_Data.m_sName)) return &pNode->m_Data;

		pNode = pNode->m_pNext;
	}

	// Failure !!

	return NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : CreateFX()
//
//   PURPOSE  : Creates a named FX
//
//------------------------------------------------------------------

CBaseFX* CFxMgr::CreateFX(char *sName, FX_BASEDATA *pBaseData, CLinkList<FX_PROP> *pProps)
{
	CBaseFX *pNewFX = NULL;

	// Locate the named FX

	FX_REF *pFxRef = FindFX(sName);

	if (pFxRef)
	{
		pNewFX = pFxRef->m_pfnCreate();
	}

	// If we have a new fx, go ahead and add it onto the active list

	if (pNewFX) 
	{
		// Assign a unique ID for this FX

		pBaseData->m_dwID = GetUniqueID();
		
		if (pNewFX->Init(m_pClientDE, pBaseData, pProps))
		{
			// Give it an update

			pNewFX->Update(m_pClientDE->GetTime());
		}
		else
		{
			pNewFX->Term();
			pNewFX = NULL;
		}
	}

	// All done....
	
	return pNewFX;
}

//------------------------------------------------------------------
//
//   FUNCTION : PreUpdateAllActiveFX()
//
//   PURPOSE  : Calls PreUpdate on all active FX
//
//------------------------------------------------------------------

bool CFxMgr::PreUpdateAllActiveFX(HOBJECT hCamera)
{	
	float tmCur = m_pClientDE->GetTime();

	// Pre-update all the group FX

	CLinkListNode<CLIENTFX_INSTANCE *> *pInstNode = m_collActiveGroupFX.GetHead();

	while (pInstNode)
	{
		CLIENTFX_INSTANCE *pInst = pInstNode->m_Data;

		// Update the active list of FX

		CLinkListNode<FX_LINK> *pActiveNode = pInst->m_collActiveFX.GetHead();

		while (pActiveNode)
		{
			pActiveNode->m_Data.m_pFX->PreUpdate(tmCur);
			
			pActiveNode = pActiveNode->m_pNext;
		}

		pInstNode = pInstNode->m_pNext;
	}

	// Success !!
	
	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : UpdateAllActiveFX()
//
//   PURPOSE  : Updates all the active FX in the world
//
//------------------------------------------------------------------

bool CFxMgr::UpdateAllActiveFX(bool bAppHasFocus, HOBJECT hCamera)
{
	HCONSOLEVAR hVar = m_pClientDE->GetConsoleVar("UpdateClientFX");
	if (hVar)
	{
		float fVal = m_pClientDE->GetVarValueFloat(hVar);

		if (!fVal) return true;
	}

	hVar = m_pClientDE->GetConsoleVar("GlobalDetail");
	if (hVar)
	{
		int nVal = (int)m_pClientDE->GetVarValueFloat(hVar);

		float fVal;

		switch (nVal)
		{
			case 0 : fVal = 0.15f; break;
			case 1 : fVal = 0.5f; break;
			case 2 : fVal = 1.0f; break;
		}

		m_pfnDetailFunc(fVal);	
	}

	// Set params....

	m_params.m_bAppFocus = bAppHasFocus;

	m_pfnSetParams(&m_params);
	
	// Get the current time
	
	float tmCur = m_pClientDE->GetTime();
	m_tmLastUpdate = tmCur;	

	// Update the group FX

	CLinkListNode<CLIENTFX_INSTANCE *> *pInstNode = m_collActiveGroupFX.GetHead();

	while (pInstNode)
	{
		CLIENTFX_INSTANCE *pInst = pInstNode->m_Data;

		// Update the active list of FX

		CLinkListNode<FX_LINK> *pActiveNode = pInst->m_collActiveFX.GetHead();

		while (pActiveNode)
		{
			CLinkListNode<FX_LINK> *pDelNode = NULL;

			CBaseFX *pFX = pActiveNode->m_Data.m_pFX;

			// Check for expiration

			if (pFX) 
			{
				bool bShutdown = pFX->HasExpired(tmCur);

				if ((pActiveNode->m_Data.m_pRef->m_bContinualLoop) && (pInst->m_bLoop))
				{
					bShutdown = false;
				}

				if (pInst->m_bShutdown)
				{
					pFX->Shutdown();
					bShutdown = true;
				}

				if (bShutdown)
				{
					if (pFX->IsFinished())
					{
						pDelNode = pActiveNode;
						pFX->Term();
					}
					else
					{
						pFX->Shutdown();
						pFX->SetCamera(hCamera);
						pFX->Update(tmCur);
					}
				}
				else
				{
					pFX->SetCamera(hCamera);
					pFX->Update(tmCur);
				}
			}

			pActiveNode = pActiveNode->m_pNext;

			if (pDelNode) pInst->m_collActiveFX.Remove(pDelNode);
		}

		// If we are shutdown, delete all the inactive FX

		if (pInst->m_bShutdown)
		{
			if (pInst->m_collInactiveFX.GetHead()) pInst->m_collInactiveFX.RemoveAll();
		}
		
		CLinkListNode<CLIENTFX_INSTANCE *> *pDelNode = NULL;

		// Run through the FX and check to see if we have to create new ones or
		// restart the whole thing

		if (pInst->m_collInactiveFX.GetHead())
		{
			// We have some inactive FX, check for startup

			CLinkListNode<FX_KEY *> *pKeyNode = pInst->m_collInactiveFX.GetHead();

			while (pKeyNode)
			{				
				FX_KEY *pKey = pKeyNode->m_Data;

				if (tmCur - pInst->m_tmCreate >= pKey->m_tmStart)
				{
					// We need to go ahead and start this FX

					FX_BASEDATA fxData;
					fxData.m_tmStart = pInst->m_tmCreate + pKey->m_tmStart;
					fxData.m_tmEnd   = pInst->m_tmCreate + pKey->m_tmEnd;
					fxData.m_dwReps  = pKey->m_dwKeyRepeat;
					fxData.m_vPos    = pInst->m_vPos;
					fxData.m_dwID    = pInst->m_dwID;
					fxData.m_hTarget = pInst->m_hTarget;

					if (pInst->m_bUseTargetData)
					{
						fxData.m_bUseTargetData = true;
						fxData.m_vTargetPos		= pInst->m_vTargetPos;
						fxData.m_vTargetNorm	= pInst->m_vTargetNorm;
					}

					if (pInst->m_sNode[0])
					{
						// Copy in the node of the parent

						strcpy(fxData.m_sNode, pInst->m_sNode);
					}

					uint32 dwFlags = pKey->m_pFxRef->m_dwType;

					// Save off the camera
					
					fxData.m_hCamera = hCamera;

					// Save the parent
					
					fxData.m_hParent = pInst->m_hParent;

					if ((!fxData.m_hParent) && (pInst->m_hServerObject))
					{
						fxData.m_hParent = pInst->m_hServerObject;
					}

					if (pKey->m_bLinked)
					{
						CLinkListNode<FX_LINK> *pNode = pInst->m_collActiveFX.GetHead();

						while (pNode)
						{
							if (pNode->m_Data.m_dwID == pKey->m_dwLinkedID)
							{
								// This is the one !!!

								if (pInst->ExistFX(pNode->m_Data.m_pFX))
								{
									CBaseFX *pFX = pNode->m_Data.m_pFX;
									fxData.m_hParent = pFX->GetFXObject();

									strcpy(fxData.m_sNode, pKey->m_sLinkedNodeName);
								}

								break;
							}
							
							pNode = pNode->m_pNext;
						}
					}

					// Create the FX

					CBaseFX *pNewFX = CreateFX(pKey->m_pFxRef->m_sName, &fxData, &pKey->m_collProps);

					if( pNewFX )
					{
						// Add it onto the started list for link referencing

						FX_LINK fxLink;
						fxLink.m_dwID = pKey->m_dwID;
						fxLink.m_pFX  = pNewFX;
						fxLink.m_pRef = pKey;

						pInst->m_collActiveFX.AddHead(fxLink);
					}

					// Advance to the next FX

					CLinkListNode<FX_KEY *> *pDelNode = pKeyNode;

					pKeyNode = pKeyNode->m_pNext;

					// And remove it from the list
					
					pInst->m_collInactiveFX.Remove(pDelNode);
				}
				else
				{
					pKeyNode = pKeyNode->m_pNext;
				}
			}
		}
		else if (tmCur - pInst->m_tmCreate > pInst->m_pData->m_tmTotalTime)
		{
			if ((pInst->m_bLoop) && (!pInst->m_bShutdown))
			{
				// Group has ended, perform a restart

				CLinkListNode<FX_KEY *> *pKeyNode = pInst->m_pData->m_collFx.GetHead();

				while (pKeyNode)
				{
					if (!pKeyNode->m_Data->m_bContinualLoop)
					{
						// Only restart this FX if it's not set for continual looping

						pInst->m_collInactiveFX.AddTail(pKeyNode->m_Data);
					}
					
					pKeyNode = pKeyNode->m_pNext;
				}

				// Set the restart time

				pInst->m_tmCreate = tmCur;
			}
			else
			{
				if (pInst->m_collActiveFX.GetSize() == 0)
				{				
					// Destroy the instance

					delete pInst;

					pDelNode = pInstNode;
				}
			}
		}
		
		pInstNode = pInstNode->m_pNext;

		if (pDelNode)
		{
			m_collActiveGroupFX.Remove(pDelNode);
		}
	}

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : ShutdownAllFX()
//
//   PURPOSE  : Shuts down all active FX
//
//------------------------------------------------------------------

void CFxMgr::ShutdownAllFX()
{
	// Delete all the FX group instances

	CLinkListNode<CLIENTFX_INSTANCE *> *pInstNode = m_collActiveGroupFX.GetHead();

	while (pInstNode)
	{
		CLinkListNode<FX_LINK> *pLinkNode = pInstNode->m_Data->m_collActiveFX.GetHead();

		while (pLinkNode)
		{
			pLinkNode->m_Data.m_pFX->Term();
			
			pLinkNode = pLinkNode->m_pNext;
		}
		
		delete pInstNode->m_Data;
		
		pInstNode = pInstNode->m_pNext;
	}

	m_collActiveGroupFX.RemoveAll();
}

//------------------------------------------------------------------
//
//   FUNCTION : ShutdownGroupByRef()
//
//   PURPOSE  : Shuts down a group FX given a reference
//
//------------------------------------------------------------------

void CFxMgr::ShutdownClientFX(CLIENTFX_INSTANCE *pFxGroup)
{	
	CLinkListNode<CLIENTFX_INSTANCE *> *pActiveNode = m_collActiveGroupFX.GetHead();

	while (pActiveNode)
	{	
		CLinkListNode<CLIENTFX_INSTANCE *> *pDelNode = NULL;
		CLIENTFX_INSTANCE *pDelInst = NULL;

		if (pActiveNode->m_Data == pFxGroup)
		{		
			bool bSmooth = pActiveNode->m_Data->m_bSmoothShutdown;

			// Shut it down !!

			pActiveNode->m_Data->m_bShutdown = true;

			HOBJECT hReplaceParent = NULL;
			
			if (bSmooth)
			{
				LTVector3f vPos;
				if (pFxGroup->m_hParent)
				{
					m_pClientDE->Physics()->GetPosition(pFxGroup->m_hParent, &vPos);
				}
				else
				{
					vPos = pFxGroup->m_vPos;
				}

				// Create a temporary parent object....

				ObjectCreateStruct ocs;
				INIT_OBJECTCREATESTRUCT(ocs);

				ocs.m_ObjectType = OT_NORMAL;
				ocs.m_Pos		 = vPos;
				ocs.m_Flags		 = 0;

				pFxGroup->m_hAlternateParent = m_pClientDE->CreateObject(&ocs);
				if (pFxGroup->m_hAlternateParent)
				{
					hReplaceParent = pFxGroup->m_hAlternateParent;
				}
			}

			// Run through all the active FX and clear out their parents

			CLinkListNode<FX_LINK> *pActiveFxNode = pActiveNode->m_Data->m_collActiveFX.GetHead();

			while (pActiveFxNode)
			{
				CBaseFX *pFX = pActiveFxNode->m_Data.m_pFX;
				if (pFX)
				{
					pFX->SetParent(hReplaceParent);
					if (!hReplaceParent) pFX->Term();
				}
				
				pActiveFxNode = pActiveFxNode->m_pNext;
			}

			if (!hReplaceParent) pActiveNode->m_Data->m_collActiveFX.RemoveAll();

			return;
		}

		if (pDelNode)
		{
			m_collActiveGroupFX.Remove(pDelNode);
			delete pDelInst;
		}

		pActiveNode = pActiveNode->m_pNext;
	}

	CLinkListNode<CLIENTFX_INSTANCE *> *pInactiveNode = m_collActiveGroupFX.GetHead();

	while (pInactiveNode)
	{
		if (pInactiveNode->m_Data == pFxGroup)
		{
			// Shut it down !!

			pInactiveNode->m_Data->m_bShutdown = true;

			return;
		}

		pInactiveNode = pInactiveNode->m_pNext;
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : LoadFxGroups()
//
//   PURPOSE  : Loads an FX group file
//
//------------------------------------------------------------------

bool CFxMgr::LoadFxGroups(const char *sDir, const char *sFileName)
{
	// Retrieve the file list

	FileEntry *pEntry = m_pClientDE->GetFileList((char *)sDir);

	if(!pEntry)
		return false;

	ILTStream *pFxFile;

	while (pEntry)
	{
		// Validate.....

		if ((pEntry->m_Type == TYPE_FILE) && (!stricmp(sFileName, pEntry->m_pBaseFilename)))
		{
			// Attempt to open the client fx file
			
			m_pClientDE->OpenFile(pEntry->m_pFullFilename, &pFxFile);
			if (pFxFile)
			{
				// Read in the number of FX groups in this file

				uint32 dwNumGroups;
				pFxFile->Read(&dwNumGroups, sizeof(uint32));

				for (uint32 i = 0; i < dwNumGroups; i ++)
				{
					FX_GROUP *pFxGroup = new FX_GROUP;

					// Read in the number of FX in this group

					uint32 dwNumFx;
					pFxFile->Read(&dwNumFx, sizeof(uint32));
					
					// Read in the name of this FX group

					pFxFile->Read(pFxGroup->m_sName, 128);

					// Read in the phase length

					uint32 dummy;
					pFxFile->Read(&dummy, sizeof(uint32));

					// Read in the FX

					for (uint32 j = 0; j < dwNumFx; j ++)
					{
						FX_KEY *pKey = new FX_KEY;

						// Read in the reference name

						char sTmp[128];
						pFxFile->Read(sTmp, 128);

						pKey->m_pFxRef = FindFX(sTmp);

						// Read in the key ID

						pFxFile->Read(&pKey->m_dwID, sizeof(uint32));

						// Read in the link status

						LINK_STATUS ls;
						pFxFile->Read(&ls, sizeof(LINK_STATUS));

						pKey->m_bLinked = ls.m_bLinked;
						pKey->m_dwLinkedID = ls.m_dwLinkedID;
						strcpy(pKey->m_sLinkedNodeName, ls.m_sLinkedNodeName);

						// Read in the start time

						pFxFile->Read(&pKey->m_tmStart, sizeof(float));

						// Read in the end time

						pFxFile->Read(&pKey->m_tmEnd, sizeof(float));

						// Read in the key repeat

						pFxFile->Read(&pKey->m_dwKeyRepeat, sizeof(uint32));

						// Read in dummy values

						uint32 dwDummy;
						pFxFile->Read(&dwDummy, sizeof(uint32));
						pFxFile->Read(&dwDummy, sizeof(uint32));
						pFxFile->Read(&dwDummy, sizeof(uint32));

						// Read in the number of properties

						uint32 dwNumProps;
						pFxFile->Read(&dwNumProps, sizeof(uint32));

						for (uint32 k = 0; k < dwNumProps; k ++)
						{
							FX_PROP fxProp;

							// Read the length of the name

							BYTE nameLen;
							pFxFile->Read(&nameLen, 1);

							// Read in the name

							pFxFile->Read(&fxProp.m_sName, nameLen);

							// Read the type

							pFxFile->Read(&fxProp.m_nType, sizeof(FX_PROP::eDataType));

							// Read the data

							switch (fxProp.m_nType)
							{
								case FX_PROP::STRING  : pFxFile->Read(&fxProp.m_data.m_sVal, 128); break;
								case FX_PROP::INTEGER : pFxFile->Read(&fxProp.m_data.m_nVal, sizeof(int)); break;
								case FX_PROP::FLOAT   : pFxFile->Read(&fxProp.m_data.m_fVal, sizeof(float)); break;
								case FX_PROP::COMBO   : pFxFile->Read(&fxProp.m_data.m_sVal, 128); break;
								case FX_PROP::VECTOR  : pFxFile->Read(&fxProp.m_data.m_fVec, sizeof(float) * 3); break;
								case FX_PROP::VECTOR4 : pFxFile->Read(&fxProp.m_data.m_fVec4, sizeof(float) * 4); break;
								case FX_PROP::CLRKEY  : pFxFile->Read(&fxProp.m_data.m_clrKey, sizeof(FX_PROP::FX_CLRKEY) ); break;
								case FX_PROP::PATH	  : pFxFile->Read(&fxProp.m_data.m_sVal, 128); break;
							}							

							pKey->m_collProps.AddTail(fxProp);
						}

						pFxGroup->m_collFx.AddTail(pKey);
					}

					// Compute the total time of this group

					CLinkListNode<FX_KEY *> *pKeyNode = pFxGroup->m_collFx.GetHead();

					pFxGroup->m_tmTotalTime = 0.0f;
					
					while (pKeyNode)
					{
						if (pKeyNode->m_Data->m_tmEnd > pFxGroup->m_tmTotalTime) 
						{
							pFxGroup->m_tmTotalTime = pKeyNode->m_Data->m_tmEnd;
						}

						pKeyNode = pKeyNode->m_pNext;
					}

					m_collGroupFX.AddTail(pFxGroup);
				}

				pFxFile->Release();
			}
		}

		pEntry = pEntry->m_pNext;
	}

	// Run through the FX groups and see which keys should be
	// setup to loop continually

	CLinkListNode<FX_GROUP *> *pFxGroupNode = m_collGroupFX.GetHead();

	while (pFxGroupNode)
	{
		CLinkListNode<FX_KEY *> *pFxNode = pFxGroupNode->m_Data->m_collFx.GetHead();

		while (pFxNode)
		{
			FX_KEY *pKey = pFxNode->m_Data;

			float tmLength = pKey->m_tmEnd - pKey->m_tmStart;

			if (tmLength >= pFxGroupNode->m_Data->m_tmTotalTime - 0.01f)
			{
				pKey->m_bContinualLoop = true;
			}
			else
			{
				pKey->m_bContinualLoop = false;
			}
			
			pFxNode = pFxNode->m_pNext;
		}

		pFxGroupNode = pFxGroupNode->m_pNext;
	}

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : FindGroupFX()
//
//   PURPOSE  : Locates a group FX by a name
//
//------------------------------------------------------------------

FX_GROUP* CFxMgr::FindGroupFX(char *sName)
{
	// Locate the group

	CLinkListNode<FX_GROUP *> *pGroupNode = m_collGroupFX.GetHead();

	while (pGroupNode)
	{
		if (!stricmp(pGroupNode->m_Data->m_sName, sName))
		{
			// This is the one we want

			return pGroupNode->m_Data;
		}

		pGroupNode = pGroupNode->m_pNext;
	}

	// Failure....

	return NULL;
}

//------------------------------------------------------------------
//
//   FUNCTION : CreateClientFX()
//
//   PURPOSE  : Creates a group FX at a given position
//
//------------------------------------------------------------------

CLIENTFX_INSTANCE* CFxMgr::CreateClientFX(const CLIENTFX_CREATESTRUCT &fxInit)
{
	FX_GROUP *pRef = FindGroupFX((char *)fxInit.m_sName);
	if (!pRef) return NULL;

	CLIENTFX_INSTANCE *pNewInst = new CLIENTFX_INSTANCE;

	pNewInst->m_pData			= pRef;
	pNewInst->m_dwServerID		= 0;
	pNewInst->m_dwID			= GetUniqueID();

	pNewInst->m_hParent			= fxInit.m_hParent;	
	pNewInst->m_bLoop			= (fxInit.m_dwFlags & FXFLAG_LOOP) ? true : false;
	pNewInst->m_bSmoothShutdown	= (fxInit.m_dwFlags & FXFLAG_SMOOTHSHUTDOWN) ? true : false;
	pNewInst->m_hServerObject	= fxInit.m_hServerObject;
	pNewInst->m_bShutdown		= false;
	pNewInst->m_hTarget			= fxInit.m_hTarget;
	
	pNewInst->m_bUseTargetData  = fxInit.m_bUseTargetData;
	pNewInst->m_vTargetPos		= fxInit.m_vTargetPos;
	pNewInst->m_vTargetNorm		= fxInit.m_vTargetNorm;

	if (fxInit.m_sParentNode[0]) strcpy(pNewInst->m_sNode, fxInit.m_sParentNode);

	// Load the instance with all the inactive FX

	CLinkListNode<FX_KEY *> *pKeyNode = pRef->m_collFx.GetHead();

	while (pKeyNode)
	{
		pNewInst->m_collInactiveFX.AddTail(pKeyNode->m_Data);
		
		pKeyNode = pKeyNode->m_pNext;
	}

	// Add it to the list of active group FX

	m_collActiveGroupFX.AddTail(pNewInst);

	// Set the position

	pNewInst->m_vPos = fxInit.m_vPos;

	// And finally set the creation time

	float tmCur = m_pClientDE->GetTime();
	pNewInst->m_tmCreate = tmCur;

	// Success !!

	return pNewInst;
}

//------------------------------------------------------------------
//
//   FUNCTION : GetUniqueID()
//
//   PURPOSE  : Returns a unique ID
//
//------------------------------------------------------------------

uint32 CFxMgr::GetUniqueID()
{
	return g_dwID ++;
}

//------------------------------------------------------------------
//
//   FUNCTION : OnSpecialEffectNotify()
//
//   PURPOSE  : OnSpecialEffectNotify
//
//------------------------------------------------------------------

void CFxMgr::OnSpecialEffectNotify(BYTE nId, HOBJECT hObject, HMESSAGEREAD hMessage)
{
	LMessage *pMsg = (LMessage *)hMessage;

	uint32 dwServerID = 0;
	char sName[256];
	uint32 dwFxFlags;
	DVector vPos;

	switch( nId )
	{
		case SFX_CLIENTFXGROUP :
		{
			if( !hObject )
				return;

			// Retrieve the ID of the object
			
			pMsg->ReadStringFL(sName, 256);
			dwFxFlags = pMsg->ReadDWord();

			m_pClientDE->Physics()->GetPosition(hObject, &vPos);
		}
		break;

		case SFX_CLIENTFXGROUPINSTANT :
		{
			// Retrieve the ID of the object

			pMsg->ReadStringFL(sName, 256);
			dwFxFlags = pMsg->ReadDWord();
			vPos = pMsg->ReadCompPos();
			hObject = pMsg->ReadObject();
		}
	}

	// Check for the existence of this FX on the client

	CLinkListNode<CLIENTFX_INSTANCE *> *pNode = m_collInactiveGroupFX.GetHead();

	while (pNode)
	{
		CLIENTFX_INSTANCE *pInst = pNode->m_Data;

		if (pInst->m_dwServerID == dwServerID)
		{
			// Log the server object that we got this time

			pInst->m_hParent	   = hObject;
			pInst->m_hServerObject = hObject;
			pInst->m_vPos          = vPos;

			// Unfreeze the instance

			UnfreezeInstance(pInst, hObject);

			// Remove from the inactive list
			
			m_collInactiveGroupFX.Remove(pNode);

			// And place on the active list

			m_collActiveGroupFX.AddTail(pInst);

			// All done

			return;
		}
		
		pNode = pNode->m_pNext;
	}

	// If we got here we don't yet have this special FX so we have to start it running

	CLIENTFX_CREATESTRUCT fxcs(sName, dwFxFlags, vPos);
	fxcs.m_hParent       = hObject;
	fxcs.m_hServerObject = hObject;
	
	CLIENTFX_INSTANCE *pNewInst = CreateClientFX(fxcs);
	if (!pNewInst) return;

	pNewInst->m_hServerObject = hObject;
}

//------------------------------------------------------------------
//
//   FUNCTION : OnObjectRemove()
//
//   PURPOSE  : Removes an object
//
//------------------------------------------------------------------

bool CFxMgr::OnObjectRemove(HOBJECT hObject)
{
	CLinkListNode<CLIENTFX_INSTANCE *> *pNode = m_collActiveGroupFX.GetHead();

	while (pNode)
	{
		CLIENTFX_INSTANCE *pInst = pNode->m_Data;

		if ((pInst->m_hServerObject) && ((pInst->m_hServerObject == hObject) || (pInst->m_hParent == hObject)))
		{		
			pInst->m_bShutdown	   = true;
			pInst->m_hServerObject = NULL;

			ShutdownClientFX(pInst);
		}
		
		pNode = pNode->m_pNext;
	}

	return false;
}

//------------------------------------------------------------------
//
//   FUNCTION : FreezeInstance()
//
//   PURPOSE  : Freezes all the FX associated with this instance
//
//------------------------------------------------------------------

void CFxMgr::FreezeInstance(CLIENTFX_INSTANCE *pInst)
{
	// Freeze all the FX in this instance

	CLinkListNode<FX_LINK> *pNode = pInst->m_collActiveFX.GetHead();

	float tmFreeze = m_pClientDE->GetTime();

	while (pNode)
	{
		pNode->m_Data.m_pFX->Freeze(tmFreeze);
		
		pNode = pNode->m_pNext;
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : UnfreezeInstance()
//
//   PURPOSE  : Unfreezes all the FX associated with this instance
//
//------------------------------------------------------------------

void CFxMgr::UnfreezeInstance(CLIENTFX_INSTANCE *pInst, HOBJECT hNewParent)
{
	// Unfreeze all the FX in this instance

	CLinkListNode<FX_LINK> *pNode = pInst->m_collActiveFX.GetHead();

	float tmUnfreeze = m_pClientDE->GetTime();

	while (pNode)
	{
		// Unfreeze the instance
		
		pNode->m_Data.m_pFX->Unfreeze(tmUnfreeze);

		// Set the new parent

		pNode->m_Data.m_pFX->SetParent(hNewParent);
		
		pNode = pNode->m_pNext;
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : ExistFX()
//
//   PURPOSE  : Checks to see whether a group FX instance exists
//
//------------------------------------------------------------------

bool CFxMgr::ExistFX(CLIENTFX_INSTANCE *pFxGroup)
{
	CLinkListNode<CLIENTFX_INSTANCE *> *pNode = m_collActiveGroupFX.GetHead();

	while (pNode)
	{
		if (pNode->m_Data == pFxGroup) return true;

		pNode = pNode->m_pNext;
	}
	
	// Failure !!

	return false;
}

//------------------------------------------------------------------
//
//   FUNCTION : SetGroupParent()
//
//   PURPOSE  : Sets a parent for all FX in a group
//
//------------------------------------------------------------------

void CFxMgr::SetGroupParent(CLIENTFX_INSTANCE *pInstance, HOBJECT hParent)
{
	CLinkListNode<FX_LINK> *pLinkNode = pInstance->m_collActiveFX.GetHead();

	while (pLinkNode)
	{
		pLinkNode->m_Data.m_pFX->SetParent(hParent);

		pLinkNode = pLinkNode->m_pNext;
	}
}

//------------------------------------------------------------------
//
//   FUNCTION : OnRendererShutdown()
//
//   PURPOSE  : Informs each FX that the renderer just shut down
//
//------------------------------------------------------------------

void CFxMgr::OnRendererShutdown()
{
	CLinkListNode<CLIENTFX_INSTANCE *> *pNode = m_collActiveGroupFX.GetHead();

	while (pNode)
	{
		CLinkListNode<FX_LINK> *pLinkNode = pNode->m_Data->m_collActiveFX.GetHead();

		while (pLinkNode)
		{
			pLinkNode->m_Data.m_pFX->OnRendererShutdown();

			pLinkNode = pLinkNode->m_pNext;
		}

		pNode = pNode->m_pNext;
	}
}
