//------------------------------------------------------------------
//
//   MODULE    : CLIENTFX.CPP
//
//   PURPOSE   : Implements class CClientFX
//
//   CREATED   : On 8/23/00 At 6:45:30 PM
//
//   COPYRIGHT : (C) 2000 LithTech Inc
//
//------------------------------------------------------------------


// Includes....

#include "clientfx.h"
#include "fxflags.h"
#include "sfxdefs.h"
#include "iltmessage.h"

BEGIN_CLASS(CClientFX)
	ADD_STRINGPROP(FxName, "")
	ADD_BOOLPROP(Loop, true)
	ADD_BOOLPROP(SmoothShutdown, false)
	ADD_BOOLPROP(StartOn, true)
END_CLASS_DEFAULT(CClientFX, BaseClass, NULL, NULL)

//-------
// Step 1
//-------

// !!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!

ILTServer *g_pLTServer_ClientFX = NULL;

FX_LIST	g_ClientFX;

//------------------------------------------------------------------
//
//   FUNCTION : SetupClientFX()
//
//   PURPOSE  : Sets gloval ILTServer pointer
//
//------------------------------------------------------------------

void SetupClientFX(ILTServer *pLTServer)
{
	g_pLTServer_ClientFX = pLTServer;
}

//------------------------------------------------------------------
//
//   FUNCTION : CClientFX()
//
//   PURPOSE  : Standard constructor
//
//------------------------------------------------------------------

CClientFX::CClientFX()
{
	m_bStartOn	= true;
}

//------------------------------------------------------------------
//
//   FUNCTION : CClientFX()
//
//   PURPOSE  : Standard destructor
//
//------------------------------------------------------------------

CClientFX::~CClientFX()
{
}

//------------------------------------------------------------------
//
//   FUNCTION : EngineMessageFn()
//
//   PURPOSE  : Handles engine messages
//
//------------------------------------------------------------------

uint32 CClientFX::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
	switch (messageID)
	{
		case MID_PRECREATE :
		{
			uint32 dwRet = BaseClass::EngineMessageFn(messageID, pData, fData);

			ObjectCreateStruct *pOcs = (ObjectCreateStruct *)pData;

			if ((uint32)fData == PRECREATE_WORLDFILE || (uint32)fData == PRECREATE_STRINGPROP)
			{
				// Read in object properties

				ReadProps(pOcs);
			}
			
			return dwRet;
		}
		break;
		
		case MID_INITIALUPDATE :
		{
			if ((uint32)fData == INITIALUPDATE_SAVEGAME)
				break;

			ASSERT(g_pLTServer_ClientFX);

			if (g_pLTServer_ClientFX->GetServerFlags( ) & SS_CACHING)
				CacheFiles();

			// Setup the client FX special effect message
			
			SetClientFXMessage(this, m_sFxName, m_dwFxFlags);

			// All done....

			return LT_OK;
		}
		break;

		case MID_SAVEOBJECT :
		{			
			// Handle saving

			BaseClass::EngineMessageFn(messageID, pData, fData);	
			OnSave((HMESSAGEWRITE)pData, (uint32)fData);
			return LT_OK;
		}
		break;

		case MID_LOADOBJECT :
		{
			// Handle loading
			
			BaseClass::EngineMessageFn(messageID, pData, fData);
			OnLoad((HMESSAGEREAD)pData, (uint32)fData);
			return LT_OK;
		}
		break;
	}

	return BaseClass::EngineMessageFn(messageID, pData, fData);
}

//------------------------------------------------------------------
//
//   FUNCTION : ObjectMessageFn()
//
//   PURPOSE  : Handles object messages
//
//------------------------------------------------------------------

uint32 CClientFX::ObjectMessageFn(HOBJECT hSender, uint32 messageID, HMESSAGEREAD hRead)
{
	// TO DO - Add game specific code to receive ON/OFF messages.
	// You can turn on/off ClientFX by adding/removing the FLAG_FORCECLIENTUPDATE
	// flag respectively.

/*	Example -

	switch (messageID)
	{
		case OBJECT_TRIGGER : (Object trigger message)
		{
			// Read out command
			
			char *sTriggerMsg = g_pLTServer->ReadFromMessageString(hRead);

			uint32 uFlags = g_pLTServer->GetObjectFlags(m_hObject);

			if (!stricmp(sTriggerMsg), "ON"))
			{
				g_pLTServer->SetObjectFlags(m_hObject, uFlags | FLAG_FORCECLIENTUPDATE);
			}
			else if (!stricmp(sTriggerMsg), "OFF"))
			{
				g_pLTServer->SetObjectFlags(m_hObject, uFlags & ~FLAG_FORCECLIENTUPDATE);				
			}
		}
		break;
	}
*/

	return BaseClass::ObjectMessageFn(hSender, messageID, hRead);
}

//------------------------------------------------------------------
//
//   FUNCTION : CacheFiles()
//
//   PURPOSE  : Caches in all files used for this client FX
//
//------------------------------------------------------------------

void CClientFX::CacheFiles()
{
	CacheClientFX(m_sFxName);
}

//------------------------------------------------------------------
//
//   FUNCTION : OnSave()
//
//   PURPOSE  : Handles saving
//
//------------------------------------------------------------------

void CClientFX::OnSave(HMESSAGEWRITE hWrite, uint32 dwFlags)
{
	ILTMessage *pMsg = (ILTMessage *)hWrite;
	
	pMsg->WriteByte(m_bStartOn);
	pMsg->WriteString(m_sFxName);
	pMsg->WriteDWord(m_dwFxFlags);
	pMsg->WriteByte(m_bLoop);
}

//------------------------------------------------------------------
//
//   FUNCTION : OnLoad()
//
//   PURPOSE  : Handles loading
//
//------------------------------------------------------------------

void CClientFX::OnLoad(HMESSAGEREAD hRead, uint32 dwFlags)
{
	ASSERT(g_pLTServer_ClientFX);

	ILTMessage *pMsg = (ILTMessage *)hRead;

	m_bStartOn = pMsg->ReadByte() ? true : false;
	pMsg->ReadStringFL(m_sFxName, 128);
	m_dwFxFlags = pMsg->ReadDWord();
	m_bLoop = pMsg->ReadByte() ? true : false;

	if (!m_bLoop)
	{
		g_pLTServer_ClientFX->SetObjectFlags(m_hObject, 0);
		g_pLTServer_ClientFX->SetNextUpdate(m_hObject, 0.0f);
	}

	if (g_pLTServer_ClientFX->GetServerFlags( ) & SS_CACHING)
		CacheFiles();
}

//------------------------------------------------------------------
//
//   FUNCTION : ReadProps()
//
//   PURPOSE  : Reads in object properties
//
//------------------------------------------------------------------

void CClientFX::ReadProps(ObjectCreateStruct *pOcs)
{
	ASSERT(g_pLTServer_ClientFX);

	GenericProp startOn;
	g_pLTServer_ClientFX->GetPropGeneric("StartOn", &startOn);
	m_bStartOn = startOn.m_Bool ? true : false;

	if (startOn.m_Bool) 
	{		
		pOcs->m_Flags |= FLAG_FORCECLIENTUPDATE;
	}

	// Read name prop

	GenericProp fxName;
	g_pLTServer_ClientFX->GetPropGeneric("FxName", &fxName);
	strcpy(m_sFxName, fxName.m_String);

	// Read loop prop
	
	GenericProp loop;
	g_pLTServer_ClientFX->GetPropGeneric("Loop", &loop);
	m_bLoop = loop.m_Bool ? true : false;

	// Read smooth shutdown prop
	
	GenericProp smoothShutdown;
	g_pLTServer_ClientFX->GetPropGeneric("SmoothShutdown", &smoothShutdown);

	// Figure out client FX flags

	m_dwFxFlags = m_bLoop ? FXFLAG_LOOP : 0;
	m_dwFxFlags |= smoothShutdown.m_Bool ? FXFLAG_SMOOTHSHUTDOWN : 0;
}






//-----------------
// Global functions
//-----------------

//------------------------------------------------------------------
//
//   FUNCTION : RemoveClientFX()
//
//   PURPOSE  : Removes the global client FX list
//
//------------------------------------------------------------------

void RemoveClientFX()
{
	CLinkListNode<FX_HEADER *> *pHeaderNode = g_ClientFX.m_collClientFX.GetHead();

	while (pHeaderNode)
	{
		CLinkListNode<FX_HEADER *> *pDelNode = pHeaderNode;

		pHeaderNode = pHeaderNode->m_pNext;

		if (pDelNode)
		{
			pDelNode->m_Data->m_collResources.RemoveAll();

			delete pDelNode->m_Data;
		}
	}

	g_ClientFX.m_collClientFX.RemoveAll();
}

//------------------------------------------------------------------
//
//   FUNCTION : LoadClientFX()
//
//   PURPOSE  : Loads the client FX into a holding structure
//
//------------------------------------------------------------------

bool LoadClientFX(const char *sDir, const char *sFileName, const bool bMultiplayer)
{
	ASSERT(g_pLTServer_ClientFX);

	RemoveClientFX();

	// Retrieve the file list

	FileEntry *pEntry = g_pLTServer_ClientFX->GetFileList((char *)sDir);

	if(!pEntry)
		return false;

	ILTStream *pFxFile;

	while (pEntry)
	{
		// Validate.....

		if ((pEntry->m_Type == TYPE_FILE) && (!stricmp(sFileName, pEntry->m_pBaseFilename)))
		{
			// Attempt to open the client fx file
			
			g_pLTServer_ClientFX->OpenFile(pEntry->m_pFullFilename, &pFxFile);
			if (pFxFile)
			{
				// Read in the number of FX groups in this file

				uint32 dwNumGroups;
				pFxFile->Read(&dwNumGroups, sizeof(uint32));

				for (uint32 i = 0; i < dwNumGroups; i ++)
				{
					FX_HEADER *pHeader = new FX_HEADER;
					pHeader->m_tmLen = 0.0f;

					uint32 dwDummy;
					char sTmp[256];

					// Read in the number of FX in this group

					uint32 dwNumFx;
					pFxFile->Read(&dwNumFx, sizeof(uint32));
					
					// Read in the name of this FX group

					pFxFile->Read(sTmp, 128);

					strcpy(pHeader->m_sName, sTmp);

					// Read in the phase length

					pFxFile->Read(&dwDummy, sizeof(uint32));

					// Read in the FX

					for (uint32 j = 0; j < dwNumFx; j ++)
					{
						float fDummy;

						// Read in the reference name

						char sTmp[256];
						pFxFile->Read(sTmp, 128);

						// Read in the key ID

						pFxFile->Read(&dwDummy, sizeof(uint32));

						// Read in the link status

						LINK_STATUS ls;
						pFxFile->Read(&ls, sizeof(LINK_STATUS));

						// Read in the start time

						pFxFile->Read(&fDummy, sizeof(float));

						// Read in the end time

						pFxFile->Read(&fDummy, sizeof(float));

						if (fDummy > pHeader->m_tmLen) pHeader->m_tmLen = fDummy;			

						// Read in the key repeat

						pFxFile->Read(&dwDummy, sizeof(uint32));

						// Read in dummy values

						pFxFile->Read(&dwDummy, sizeof(uint32));
						pFxFile->Read(&dwDummy, sizeof(uint32));
						pFxFile->Read(&dwDummy, sizeof(uint32));

						// Read in the number of properties

						uint32 dwNumProps;
						pFxFile->Read(&dwNumProps, sizeof(uint32));

						for (uint32 k = 0; k < dwNumProps; k ++)
						{
							FX_PROP fxProp;
							memset(fxProp.m_sName, 0, 128);

							// Read the length of the name

							uint8 nameLen;
							pFxFile->Read(&nameLen, 1);

							// Read in the name

							pFxFile->Read(&fxProp.m_sName, nameLen);

							// Read the type

							pFxFile->Read(&fxProp.m_nType, sizeof(FX_PROP::eDataType));

							// Read the data

							switch (fxProp.m_nType)
							{
								case FX_PROP::PATH :
								{
									pFxFile->Read(&fxProp.m_data.m_sVal, 128);

									// Possible resource.... check it....

									char sTmp[256];
									strcpy(sTmp, fxProp.m_data.m_sVal);
									
									char *sRes = sTmp;

									if (strlen(sRes) > 4)
									{
										// Check for a resource 

										if (sTmp[3] == '|')
										{
											char *sRealRes = sTmp + 4;
											sTmp[3] = 0;

											if (!stricmp(sTmp, "SPR"))
											{
												FX_REZ fxRez;
												strcpy(fxRez.m_sRez, sRealRes);
												fxRez.m_nType = FT_SPRITE;

												pHeader->m_collResources.AddTail(fxRez);
											}
											else if (!stricmp(sTmp, "ABC"))
											{
												FX_REZ fxRez;
												strcpy(fxRez.m_sRez, sRealRes);
												fxRez.m_nType = FT_MODEL;

												pHeader->m_collResources.AddTail(fxRez);
											}
											else if (!stricmp(sTmp, "WAV"))
											{
												if (bMultiplayer)
												{
													if (!stricmp(fxProp.m_sName, "MultiPlaySound"))
													{
														FX_REZ fxRez;
														strcpy(fxRez.m_sRez, sRealRes);
														fxRez.m_nType = FT_SOUND;

														pHeader->m_collResources.AddTail(fxRez);
													}
												}
												else
												{
													if (!stricmp( fxProp.m_sName, "Sound"))
													{
														FX_REZ fxRez;
														strcpy(fxRez.m_sRez, sRealRes);
														fxRez.m_nType = FT_SOUND;

														pHeader->m_collResources.AddTail(fxRez);
													}
												}

												if (!stricmp(fxProp.m_sName, "ChunkSound"))
												{
													FX_REZ fxRez;
													strcpy(fxRez.m_sRez, sRealRes);
													fxRez.m_nType = FT_SOUND;

													pHeader->m_collResources.AddTail(fxRez);
												}
											}

										}
									}
								}
								break;

								case FX_PROP::INTEGER : pFxFile->Read(&fxProp.m_data.m_nVal, sizeof(int)); break;
								case FX_PROP::FLOAT   : pFxFile->Read(&fxProp.m_data.m_fVal, sizeof(float)); break;
								case FX_PROP::COMBO   : pFxFile->Read(&fxProp.m_data.m_sVal, 128); break;
								case FX_PROP::VECTOR  : pFxFile->Read(&fxProp.m_data.m_fVec, sizeof(float) * 3); break;
								case FX_PROP::VECTOR4 : pFxFile->Read(&fxProp.m_data.m_fVec4, sizeof(float) * 4); break;
								case FX_PROP::CLRKEY  : pFxFile->Read(&fxProp.m_data.m_clrKey, sizeof(FX_PROP::FX_CLRKEY)); break;
							}
						}
					}

					g_ClientFX.m_collClientFX.AddTail(pHeader);
				}

				// Close the file

				pFxFile->Release();
			}
		}

		pEntry = pEntry->m_pNext;
	}

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : CacheClientFX()
//
//   PURPOSE  : Called to cache in a specific Client FX
//
//------------------------------------------------------------------

bool CacheClientFX(const char *szGuid)
{
	ASSERT(g_pLTServer_ClientFX);

	if(!szGuid || !szGuid[0])
		return false;

	// Scan the global client FX list and look for the one
	// we need

	CLinkListNode<FX_HEADER *> *pHeaderNode = g_ClientFX.m_collClientFX.GetHead();

	while (pHeaderNode)
	{
		if (!stricmp(pHeaderNode->m_Data->m_sName, szGuid))
		{
			// Cache all these resources

			CLinkListNode<FX_REZ> *pRezNode = pHeaderNode->m_Data->m_collResources.GetHead();

			while (pRezNode)
			{
				if ((pRezNode->m_Data.m_sRez[0]) && (strcmp(pRezNode->m_Data.m_sRez, "...")))
				{
					switch (pRezNode->m_Data.m_nType)
					{
						case FT_SPRITE : g_pLTServer_ClientFX->CacheFile(FT_SPRITE, pRezNode->m_Data.m_sRez); break;
						case FT_MODEL : g_pLTServer_ClientFX->CacheFile(FT_MODEL, pRezNode->m_Data.m_sRez); break;
						case FT_TEXTURE : g_pLTServer_ClientFX->CacheFile(FT_TEXTURE, pRezNode->m_Data.m_sRez); break;
						case FT_SOUND : g_pLTServer_ClientFX->CacheFile(FT_SOUND, pRezNode->m_Data.m_sRez); break;
					}
				}

				pRezNode = pRezNode->m_pNext;
			}

			// All done !!!
			
			return true;
		}
		
		pHeaderNode = pHeaderNode->m_pNext;
	}

	// Success !!

	return true;
}

//------------------------------------------------------------------
//
//   FUNCTION : SetClientFXMessage()
//
//   PURPOSE  : Sets up the special effect message for a particular
//				object
//
//------------------------------------------------------------------

void SetClientFXMessage(BaseClass *pClass, char *sName, uint32 uFlags)
{
	ASSERT(g_pLTServer_ClientFX);

	if (g_pLTServer_ClientFX)
	{
		HMESSAGEWRITE hSpecialFxWrite = g_pLTServer_ClientFX->StartSpecialEffectMessage(pClass);
		g_pLTServer_ClientFX->WriteToMessageByte(hSpecialFxWrite, SFX_CLIENTFXGROUP);
		g_pLTServer_ClientFX->WriteToMessageString(hSpecialFxWrite, sName);
		g_pLTServer_ClientFX->WriteToMessageDWord(hSpecialFxWrite, uFlags);
		g_pLTServer_ClientFX->EndMessage(hSpecialFxWrite);
	}
}
