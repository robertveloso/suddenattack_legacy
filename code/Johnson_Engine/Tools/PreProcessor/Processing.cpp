// ------------------------------------------------------ //
// Processing.cpp - does all the main processing for 
// the preprocessor.
// ------------------------------------------------------ //

#include "bdefs.h"
#include <stdarg.h>

#include "bspgen.h"
#include "bsphullmaker.h"
#include "vislistbuilder.h"

#include "lightmapmaker.h"

#include "editpoly.h"
#include "editregion.h"
#include "brushtoworld.h"
#include "unseen.h"
#include "findworldmodel.h"
#include "processing.h"
#include "threads.h"
#include "leakfinders.h"
#include "replacetextures.h"
#include "gettextureflags.h"
#include "node_ops.h"
#include "pregeometry.h"
#include "create_world_tree.h"
#include "parse_world_info.h"
#include "createphysicsbsp.h"
#include "createphysicsaabb.h"
#include "createvisbsp.h"
#include "splitpoly.h"
#include "lightmapdefs.h"
#include "ltamgr.h"
#include "packerfactory.h"
#include "rpdmanager.h"
#include "preleaf.h"
#include "preportal.h"
#include "hullpoly.h"
#include "editpatch.h"
#include "uvtoopq.h"
#include "createdecals.h"
#include "createpolyedges.h"
#include "ApplyAmbientOverride.h"
#include "convertkeydata.h"
#include "convertscatter.h"
#include "fillingroupobjects.h"
#include "CenterWorldAroundOrigin.h"
#include "ApplyRenderGroups.h"

//number of plane lists to use for finding coplanar planes' hash table
#define NUM_PLANE_LISTS		128

CEditRegion				*g_InputRegion = NULL;
CStringHolder			g_StringHolder;

FILE					*g_pLogFile = NULL;
FILE					*g_pErrorLogFile = NULL;

static char				g_InputFileTitle[MAX_PATH];
CProcessorGlobs			*g_pGlobs = NULL;
bool					g_bInFullVis = false;




//---------------------Memory Allocation----------------------------//
void* dalloc(unsigned int size)
{
	void *pRet;
	
	pRet = malloc((size_t)size);
	if(!pRet)
		throw CLithMemException();

	return pRet;
}

void dfree(void *ptr)
{
	free(ptr);
}

void* dalloc_z(unsigned int size)
{
	void *pRet;
	
	pRet = malloc((size_t)size);
	if(!pRet)
		throw CLithMemException();
	
	memset(pRet, 0, size);
	return pRet;
}

void dsi_PrintToConsole(char *pMsg, ...)
{
}

void dsi_OnReturnError(int err)
{
}
//-----------------------------------------------------------------//

//given an argument list, this will build up a file name. This must be handled
//specifically in case there are spaces in the file name. nArgC should be the
//number of arguments that can possibly be in the file. Returns if the file
//was properly terminated with a quote or not
static bool BuildFileName(char** pArgV, uint32 nArgC, char* pszBuffer, uint32 nBuffLen)
{
	ASSERT(nArgC > 0);
	ASSERT(nBuffLen > 0);

	//clear out the buffer
	pszBuffer[0] = '\0';

	//see if our filename is just one argument
	if(pArgV[0][0] != '\"')
	{
		//it is just one arg. Copy it over and bail
		strncpy(pszBuffer, pArgV[0], nBuffLen);
		return true;
	}

	//copy over everything past the quote
	strncpy(pszBuffer, &(pArgV[0][1]), nBuffLen);

	//see if we ended on a quote
	if((strlen(pszBuffer) > 0) && (pszBuffer[strlen(pszBuffer) - 1] == '\"'))
	{
		//remove the quote and bail
		pszBuffer[strlen(pszBuffer) - 1] = '\0';
		return true;
	}

	//now add on the arguments until we find one with a quote on the end
	for(uint32 nCurrArg = 1; nCurrArg < nArgC; nCurrArg++)
	{
		//tack on the space
		strncat(pszBuffer, " ", nBuffLen);

		//see if we hit the end
		uint32 nArgLen = strlen(pArgV[nCurrArg]);
		if(nArgLen && (pArgV[nCurrArg][nArgLen - 1] == '\"'))
		{
			//take off the quote
			pArgV[nCurrArg][nArgLen - 1] = '\0';

			//we have found the end
			strncat(pszBuffer, pArgV[nCurrArg], nBuffLen);
			return true;
		}

		//not the end, just add this to it
		strncat(pszBuffer, pArgV[nCurrArg], nBuffLen);
	}

	//never found an ending quote
	return false;
}



void ProcessOptions(CProcessorGlobs *pGlobs)
{
	uint32 i;
	
	ASSERT(pGlobs);

	pGlobs->m_MaxBounds					= (PReal)400000.0;
	pGlobs->m_fLMSampleExtrude			= (PReal)0.05;
	pGlobs->m_fLMMaxPenThickness		= (PReal)1.0;
	pGlobs->m_fLMGlobalPenScale			= (PReal)1.0;
	pGlobs->m_bDistanceCullVisList		= false;
	pGlobs->m_bLMOverSample				= false;
	pGlobs->m_bRemoveProblemObjects		= false;
	pGlobs->m_bLMSuperSample			= false;
	pGlobs->m_bLightAnimations			= false;
	pGlobs->m_bLambertianLightmaps		= false;
	pGlobs->m_bSelectHullProblemBrushes = false;
	pGlobs->m_bIgnoreHidden				= false;
	pGlobs->m_bIgnoreFrozen				= false;
	pGlobs->m_bLambertLight				= false;
	pGlobs->m_bShadowMesh				= false;
	pGlobs->m_bVolumetricAmbient		= false;
	pGlobs->m_bFixTJunc					= false;
	pGlobs->m_bRemoveUnseen				= false;
	pGlobs->m_bSplitPolyForVisBSP		= false;
	pGlobs->m_bObjectsOnly				= false;
	pGlobs->m_bPortals					= true;
	pGlobs->m_bPortals					= true;
	pGlobs->m_bBalanceBSPs				= true;
	pGlobs->m_bCenterWorldAroundOrigin	= true;
	pGlobs->m_LeakFilename[0]			= '\0';
	pGlobs->m_PortalFilename[0]			= '\0';
	pGlobs->m_Platform[0]				= '\0';
	pGlobs->m_nLMNumSamplesOnSide		= 1;
	pGlobs->m_bValidateLightmaps		= false;

	strcpy(pGlobs->m_ProjectDir, ".");


	// Read command line parameters.
	for(i=0; i < pGlobs->m_Argc; i++)
	{
		char *pArg = pGlobs->m_Argv[i];
		
		// Skip the -.
		if(pArg[0] == '-' || pArg[0] == '+')
			++pArg;

			
		if(stricmp(pArg, "LeakFile") == 0)
		{
			if((i+1) < pGlobs->m_Argc)
			{
				strncpy(pGlobs->m_LeakFilename, pGlobs->m_Argv[i+1], sizeof(pGlobs->m_LeakFilename)-1);
			}
		}
		else if(stricmp(pArg, "OutFile") == 0)
		{
			if((i + 1) < pGlobs->m_Argc)
			{
				BuildFileName(	&(pGlobs->m_Argv[i + 1]), pGlobs->m_Argc - i, 
								pGlobs->m_OutputFile, MAX_PATH);
			}
		}
		else if(stricmp(pArg, "PortalFile") == 0)
		{
			if((i+1) < pGlobs->m_Argc)
			{
				strncpy(pGlobs->m_PortalFilename, pGlobs->m_Argv[i+1], sizeof(pGlobs->m_PortalFilename)-1);
			}
		}
		else if(stricmp(pArg, "MaxBounds") == 0)
		{
			if((i+1) < pGlobs->m_Argc)
			{
				pGlobs->m_MaxBounds = (PReal)atof(pGlobs->m_Argv[i+1]);
			}
		}
		else if(stricmp(pArg, "DistanceCullVisList") == 0)
		{
			//the user wants to perform distance culling on the visibility list
			pGlobs->m_bDistanceCullVisList = true;

			//need to read in the distance they want to perform culling at
			if((i+1) < pGlobs->m_Argc)
			{
				pGlobs->m_CullDistanceVisList = (PReal)atof(pGlobs->m_Argv[i+1]);
			}
		}
		else if(stricmp(pArg, "SelectHullProblemBrushes") == 0)
		{
			pGlobs->m_bSelectHullProblemBrushes = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "RemoveProblemObjects"))
		{
			pGlobs->m_bRemoveProblemObjects = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "ProjectDir"))
		{
			if((i+1) < pGlobs->m_Argc)
			{
				strncpy(pGlobs->m_ProjectDir, pGlobs->m_Argv[i+1], sizeof(pGlobs->m_ProjectDir)-1);
			}
		}
		else if(CHelpers::UpperStrcmp(pArg, "NoPortals"))
		{
			pGlobs->m_bPortals = false;
		}
		else if(CHelpers::UpperStrcmp(pArg, "ObjectsOnly"))
		{
			pGlobs->m_bObjectsOnly = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "numthreads"))
		{
			if((i+1) < pGlobs->m_Argc)
			{
				pGlobs->m_nThreads = atoi(pGlobs->m_Argv[i+1]);
				
				// Max 40 threads.
				if(pGlobs->m_nThreads < 0)
					pGlobs->m_nThreads = 0;

				if(pGlobs->m_nThreads >= MAX_THREADS)
					pGlobs->m_nThreads = MAX_THREADS;
			}
		}
		else if(stricmp(pArg, "LMMaxPenThickeness") == 0)
		{
			if((i + 1) < pGlobs->m_Argc)
			{
				pGlobs->m_fLMMaxPenThickness = (float)atof(pGlobs->m_Argv[i + 1]);
			}
		}
		else if(stricmp(pArg, "LMGlobalPenScale") == 0)
		{
			if((i + 1) < pGlobs->m_Argc)
			{
				pGlobs->m_fLMGlobalPenScale = (float)atof(pGlobs->m_Argv[i + 1]);
			}
		}
		else if(stricmp(pArg, "LMSampleExtrude") == 0)
		{
			if((i + 1) < pGlobs->m_Argc)
			{
				pGlobs->m_fLMSampleExtrude = (float)atof(pGlobs->m_Argv[i + 1]);
			}
		}
		else if(stricmp(pArg, "LMOverSample") == 0)
		{
			pGlobs->m_bLMOverSample = true;
		}
		else if( CHelpers::UpperStrcmp(pArg, "light") )
		{
			pGlobs->m_bLight = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "nocliplight"))
		{
			pGlobs->m_bShadows = false;
		}

		else if(CHelpers::UpperStrcmp(pArg, "skipdialog"))
			pGlobs->m_bSkipDialog = true;

		else if( CHelpers::UpperStrcmp(pArg, "LightAnims") )
			pGlobs->m_bLightAnimations = true;

		else if(CHelpers::UpperStrcmp(pArg, "onlylightverts"))
			pGlobs->m_bVerticesOnly = true;

		else if( CHelpers::UpperStrcmp(pArg, "maxdetail") )
		{
			if((i+1) < pGlobs->m_Argc)
			{
				pGlobs->m_MaxDetail = atoi(pGlobs->m_Argv[i+1]);
			}
		}

		else if(CHelpers::UpperStrcmp(pArg, "LambertLight"))
		{
			pGlobs->m_bLambertLight = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "ShadowMesh"))
		{
			pGlobs->m_bShadowMesh = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "VolumetricAmbient"))
		{
			pGlobs->m_bVolumetricAmbient = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "LambertianLightmaps"))
		{
			pGlobs->m_bLambertianLightmaps = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "splitweight"))
		{
			if((i + 1) < pGlobs->m_Argc)
			{
				pGlobs->m_SplitWeight	= (float)atof(pGlobs->m_Argv[i + 1]);
				pGlobs->m_BalanceWeight = 1.0f - pGlobs->m_SplitWeight;
			}
		}
		else if(CHelpers::UpperStrcmp(pArg, "LMSuperSample"))
		{
			if((i + 1) < pGlobs->m_Argc)
			{
				pGlobs->m_bLMSuperSample		= true;
				pGlobs->m_nLMNumSamplesOnSide	= atoi(pGlobs->m_Argv[i + 1]);
				
			}
		}
		else if(stricmp(pArg, "Platform") == 0)
		{
			if((i + 1) < pGlobs->m_Argc)
			{
				strcpy(pGlobs->m_Platform, pGlobs->m_Argv[i + 1]);
			}
		}
		else if(CHelpers::UpperStrcmp(pArg, "logfile"))
		{
			pGlobs->m_bLogFile = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "errorlog"))
		{
			pGlobs->m_bErrorLog = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "dontcenterworld"))
		{
			pGlobs->m_bCenterWorldAroundOrigin = false;
		}
		else if(CHelpers::UpperStrcmp(pArg, "ignorehidden"))
		{
			pGlobs->m_bIgnoreHidden = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "ignorefrozen"))
		{
			pGlobs->m_bIgnoreFrozen = true;
		}
		else if(CHelpers::UpperStrcmp(pArg, "FixTJunc"))
		{
			pGlobs->m_bFixTJunc = true;
		}
		else if (CHelpers::UpperStrcmp(pArg, "RemoveUnseen"))
		{
			pGlobs->m_bRemoveUnseen = true;
		}
		else if (CHelpers::UpperStrcmp(pArg, "NoBalanceBSPs"))
		{
			pGlobs->m_bBalanceBSPs = false;
		}
		else if (CHelpers::UpperStrcmp(pArg, "SplitPoly"))
		{
			pGlobs->m_bSplitPolyForVisBSP = true;
		}
		else if (CHelpers::UpperStrcmp(pArg, "ValidateLightmaps"))
		{
			pGlobs->m_bValidateLightmaps = true;
		}
	}
}


static void OpenLogFile(const char* pszPostFix, FILE*& OutFp, bool bDateTimeStamp)
{
	char	testFilename[MAX_PATH + 1];
	uint32	i;
	FILE	*fp;

	//default it to NULL
	OutFp = NULL;

	// Find a free file.
	for(i=0; i < 5000; i++)
	{
		LTSNPrintF(testFilename, sizeof(testFilename), "%s%s_%d.log", g_InputFileTitle, pszPostFix, i);
		fp = fopen(testFilename, "rt");
		if(!fp)
			break;
	}

	LTSNPrintF(testFilename, sizeof(testFilename), "%s%s_%d.log", g_InputFileTitle, pszPostFix, i);
	OutFp = fopen(testFilename, "wt");
	
	//write out a date/time stamp for this log
	if(OutFp && bDateTimeStamp)
	{
		char			timeString[128], dateString[128];

		thd_GetTimeString(timeString);
		thd_GetDateString(dateString);

		fprintf(OutFp, "Log file created on %s at %s\r\n", dateString, timeString);
	}
}


static void CloseLogFiles()
{
	if(g_pLogFile)
	{
		fclose(g_pLogFile);
		g_pLogFile = NULL;
	}

	if(g_pErrorLogFile)
	{
		fclose(g_pErrorLogFile);
		g_pErrorLogFile = NULL;
	}
}


void InternalDrawStatusText(EStatusTextType eType, const char *pMsg, va_list marker)
{
	char msg[512];

	LTVSNPrintF(msg, sizeof(msg), pMsg, marker);

	//see if we need to append a prefix
	char pszFinalMessage[512];
	bool bToErrorLog = false;

	switch(eType)
	{
	case eST_Error:
		LTSNPrintF(pszFinalMessage, sizeof(pszFinalMessage), "Error: %s", msg);
		bToErrorLog = true;
		break;
	case eST_Warning:
		LTSNPrintF(pszFinalMessage, sizeof(pszFinalMessage), "Warning: %s", msg);
		bToErrorLog = true;
		break;
	default:
		LTSNPrintF(pszFinalMessage, sizeof(pszFinalMessage), "%s", msg);
		bToErrorLog = false;
		break;
	}

	//write it out to the log file if it is open
	if(g_pLogFile)
	{
		fprintf(g_pLogFile, "%s\r\n", pszFinalMessage);
		fflush(g_pLogFile);
	}

	//now see if it needs to go out to an error log
	if(bToErrorLog)
	{
		//make sure that the file is open
		if(!g_pErrorLogFile)
		{
			OpenLogFile("Error", g_pErrorLogFile, true);
		}

		if(g_pErrorLogFile)
		{
			fprintf(g_pErrorLogFile, "%s\r\n", pszFinalMessage);
			fflush(g_pErrorLogFile);
		}
	}


	//and of course display it to the screen, but allow for printing of characters to get attention
	//of the user
	switch(eType)
	{
	case eST_Error:
		SysDrawStatusText("*************************");
		SysDrawStatusText(pszFinalMessage);
		SysDrawStatusText("*************************");
		break;
	case eST_Warning:
		SysDrawStatusText("+++++++++++++++++++++++++");
		SysDrawStatusText(pszFinalMessage);
		SysDrawStatusText("+++++++++++++++++++++++++");
		break;
	default:
		SysDrawStatusText(pszFinalMessage);
		break;
	}
}


void DrawStatusText(EStatusTextType eType, const char *pMsg, ...)
{
	va_list marker;

	va_start(marker, pMsg);
	InternalDrawStatusText(eType, pMsg, marker);
	va_end(marker);
}

void DrawStatusTextIfMainWorld(EStatusTextType eType, const char *pMsg, ...)
{
	va_list marker;

	if(g_pGlobs->m_bMakingWorldModel)
		return;

	va_start(marker, pMsg);
	InternalDrawStatusText(eType, pMsg, marker);
	va_end(marker);
}


static void HullPercentageCallback(CReal percent)
{
	SetCurrentProgressBar(percent);
}

static void BspStatusFunction(CBspGen *pBspGen, CReal num)
{
	SetCurrentProgressBar(num);
}

static void LightStatusCallback(CReal percent)
{
	SetCurrentProgressBar(percent);
}

static void FastVisPercentageCallback(CReal percent)
{
	SetCurrentProgressBar(percent);
}

static void VisPercentageCallback(CReal percent)
{
	if(!g_bInFullVis)
	{
		SetInFullVis();
		g_bInFullVis = true;
	}

	SetCurrentProgressBar(percent);
}

static void BrushToWorldPercentageCallback(CReal percent)
{
	SetCurrentProgressBar(percent);
}

//given a node, it will look at the processor settings and determine if it should be ignored
bool ShouldIgnoreNode(const CWorldNode* pNode)
{
	//determine if this is hidden and we are ignoring hidden
	if(g_pGlobs->m_bIgnoreHidden && pNode->IsFlagSet(NODEFLAG_HIDDEN))
		return true;

	//now check for ignoring frozen
	if(g_pGlobs->m_bIgnoreFrozen && pNode->IsFlagSet(NODEFLAG_FROZEN))
		return true;

	//we can handle it
	return false;
}

static void RemoveDetailBrushes(CEditRegion *pRegion)
{
	CEditBrush	*pBrush;
	LPOS		pos, curBrushPos;
	GPOS		gPos;

	for(pos=pRegion->m_Brushes; pos; )
	{
		curBrushPos = pos;
		pBrush = pRegion->m_Brushes.GetNext(pos);

		if(GetDetailLevel(pBrush) > g_pGlobs->m_MaxDetail)
		{
			for(gPos=pBrush->m_Children; gPos; )
				pRegion->DetachNode(pBrush->m_Children.GetNext(gPos), TRUE);

			pRegion->DetachNode(pBrush, FALSE);
			pRegion->m_Brushes.RemoveAt(curBrushPos);
			delete pBrush;
		}
	}
}


//recursively parses through the node tree looking for light groups and lights. Upon finding
//a light, it will fill in which light group it belongs to
static uint32 FillInLightGroupFields(CWorldNode* pNode, const char* pszLightGroupName)
{
	//sanity check
	if(!pNode)
		return 0;

	const char* pszFinalName = pszLightGroupName;

	uint32 nNumLightsUnder = 0;

	//see if this is an object...
	if(!ShouldIgnoreNode(pNode) && (pNode->GetType() == Node_Object))
	{
		//we have an object, check the class
		if(stricmp(pNode->GetClassName(), "LightGroup") == 0)
		{
			//see if we have any light groups within another light group
			if(pszLightGroupName)
			{
				DrawStatusText(eST_Warning, "Found lightgroup %s within lightgroup %s", pNode->GetName(), pszLightGroupName);
			}

			//this is a light group, we want to pass its name to our children
			pszFinalName = pNode->GetName();
		}
		else if(pszLightGroupName)
		{
			if(	(stricmp(pNode->GetClassName(), "Light") == 0) ||
						(stricmp(pNode->GetClassName(), "ObjectLight") == 0) ||
						(stricmp(pNode->GetClassName(), "DirLight") == 0) )
			{
				//this is a light, so we need to fill out the light group property
				CBaseProp* pProp = pNode->GetPropertyList()->GetProp("LightGroup");

				if(pProp)
				{
					if(pProp->GetType() == LT_PT_STRING)
					{
						//we have a valid property, copy the light group name into it
						strcpy(((CStringProp*)pProp)->m_String, pszFinalName);
					}
					else
					{
						DrawStatusText(eST_Error, "Found a property LightGroup that needs to be a string!");
					}
				}
				else
				{
					//we need to add this property
					CStringProp* pProp = new CStringProp("LightGroup");

					if(pProp)
					{
						strcpy(pProp->m_String, pszFinalName);
						pNode->GetPropertyList()->m_Props.Append(pProp);
					}

				}

				nNumLightsUnder++;
			}
		}
	}

	//now we need to recurse into all of our children filling out these properties
	for(GPOS pos = pNode->m_Children; pos; )
	{
		CWorldNode* pChild =  pNode->m_Children.GetNext(pos);
		nNumLightsUnder += FillInLightGroupFields(pChild, pszFinalName);
	}

	//see if we have an empty light group
	if(pszFinalName != pszLightGroupName)
	{
		//this was a lightgroup
		if(nNumLightsUnder == 0)
		{
			DrawStatusText(eST_Warning, "Found empty lightgroup %s", pszFinalName);
		}

		//we don't want to inform parents of our lights
		return 0;
	}
	else
	{
		//report how many lights are under this node
		return nNumLightsUnder;
	}
}

static void DisplayPrefabInstantiationError(const char *pMsg)
{
	DrawStatusText(eST_Error, "%s", pMsg);
}


static void RecurseAndDisconnectPrefabs(CEditRegion *pRegion, CWorldNode *pRoot)
{
	// Try to disconnect this node
	if (pRoot->GetType() == Node_PrefabRef)
	{
		CPrefabRef *pPrefabRoot = (CPrefabRef*)pRoot;

		if (!ShouldIgnoreNode(pRoot))
		{
			CWorldNode *pNewRoot = pPrefabRoot->InstantiatePrefab(pRegion, DisplayPrefabInstantiationError);
			if (!pNewRoot)
				DrawStatusText(eST_Error, "Unable to instantiate prefab %s (%s)", pPrefabRoot->GetName(), pPrefabRoot->GetPrefabFilename());
		}				
	}

	// Disconnect any children that might need to be disconnected
	GPOS iCurChild = pRoot->m_Children.GetHeadPosition();
	while (iCurChild)
	{
		CWorldNode *pChild = pRoot->m_Children.GetNext(iCurChild);
		RecurseAndDisconnectPrefabs(pRegion, pChild);
	}

	//now if it was a prefab, we need to delete that node
	if (pRoot->GetType() == Node_PrefabRef)
	{
		no_DestroyNode(pRegion, pRoot, FALSE);
	}
}


//given a triangle and its UV coordinates, it will create a singe brush for a patch
static bool CreatePatchBrush(CEditRegion* pRegion, CWorldNode* pParent, CEditPatch* pPatch,
							 PVector* pVerts, PReal* pUVs)
{
	ASSERT(pRegion);
	ASSERT(pParent);
	ASSERT(pVerts);
	ASSERT(pUVs);

	//first off create the new brush
	CEditBrush* pNewBrush = new CEditBrush;

	if(pNewBrush == NULL)
		return false;

	//add the vertices to the brush
	pNewBrush->m_Points.SetSize(3);
	pNewBrush->m_Points[0] = pVerts[0];
	pNewBrush->m_Points[1] = pVerts[1];
	pNewBrush->m_Points[2] = pVerts[2];

	//now add the polygon to the brush
	CEditPoly* pNewPoly = new CEditPoly(pNewBrush);

	if(pNewPoly == NULL)
	{
		delete pNewBrush;
		return false;
	}

	//setup the polygon's indices
	pNewPoly->m_Indices.SetSize(3);
	pNewPoly->m_Indices[0] = 0;
	pNewPoly->m_Indices[1] = 1;
	pNewPoly->m_Indices[2] = 2;

	pNewBrush->m_Polies.SetSize(1);
	pNewBrush->m_Polies[0] = pNewPoly;

	//copy over the texture information
	pNewPoly->GetTexture(0).CopyTextureAttributes(pPatch, pRegion->m_pStringHolder);

	//setup the OPQ's for this brush
	LTVector vO, vP, vQ;
	ConvertUVToOPQ(	pVerts, pUVs, pPatch->GetPatchTextureWidth(), 
					pPatch->GetPatchTextureHeight(), vO, vP, vQ);
	pNewPoly->SetTextureSpace(0, vO, vP, vQ);

	//have the brush update its info
	pNewBrush->UpdatePlanes();

	//now we need to copy the properties from the patch into the brush
	pNewBrush->m_PropList.CopyValues(&pPatch->m_PropList);

	//now we add this brush underneath the terrain object
	no_AttachNode(pRegion, pNewBrush, pParent);

	//success
	return true;
}

//converts a single patch to brushes underneath the supplied terrain object
static bool ConvertPatchToBrushes(CEditRegion* pRegion, CWorldNode* pParent, CEditPatch* pPatch)
{
	//sanity checks
	ASSERT(pRegion);
	ASSERT(pParent);
	ASSERT(pPatch);

	//get the patch resolution
	uint32 nXRes = pPatch->GetXResolution();
	uint32 nYRes = pPatch->GetYResolution();

	//texture increments
	PReal fTX = 0.0f;
	PReal fTY = 0.0f;
	PReal fTXInc = PATCH_TEXTURE_SCALE / (PReal)(nXRes - 1);
	PReal fTYInc = PATCH_TEXTURE_SCALE / (PReal)(nYRes - 1);

	LTVector O, P, Q;
	pPatch->GetTextureSpace(O, P, Q);

	PReal fPDotO = P.Dot(O);
	PReal fQDotO = Q.Dot(O);

	//position increments
	PReal fPX = 0.0f;
	PReal fPY = 0.0f;
	PReal fPXInc = 1.0f / (PReal)(nXRes - 1);
	PReal fPYInc = 1.0f / (PReal)(nYRes - 1);

	//the positions for the patch
	PVector vPos[3];

	//the texture coordinates (u, v)
	PReal	fUVs[6];

	for(uint32 nX = 0; nX < nXRes - 1; nX++, fTX += fTXInc, fPX += fPXInc)
	{
		//reset this
		fTY = 0.0f;
		fPY = 0.0f;

		for(uint32 nY = 0; nY < nYRes - 1; nY++, fTY += fTYInc, fPY += fPYInc)
		{
			//setup the first tri
			vPos[0] = pPatch->GetPatch().Evaluate(fPX, fPY + fPYInc);
			vPos[1] = pPatch->GetPatch().Evaluate(fPX, fPY);
			vPos[2] = pPatch->GetPatch().Evaluate(fPX + fPXInc, fPY);

			fUVs[0] = P.Dot(LTVector(fTX, 0, fTY + fTYInc)) - fPDotO;	
			fUVs[1] = Q.Dot(LTVector(fTX, 0, fTY + fTYInc)) - fQDotO;
			fUVs[2] = P.Dot(LTVector(fTX, 0, fTY)) - fPDotO;
			fUVs[3] = Q.Dot(LTVector(fTX, 0, fTY)) - fQDotO;
			fUVs[4] = P.Dot(LTVector(fTX + fTXInc, 0, fTY)) - fPDotO;	
			fUVs[5] = Q.Dot(LTVector(fTX + fTXInc, 0, fTY)) - fQDotO;

			uint32 nUV;
			for(nUV = 0; nUV < 3; nUV++)
			{
				fUVs[nUV * 2 + 0] /= pPatch->GetPatchTextureWidth();
				fUVs[nUV * 2 + 1] /= pPatch->GetPatchTextureHeight();
			}

			CreatePatchBrush(pRegion, pParent, pPatch, vPos, fUVs);

			//setup the second tri
			vPos[0] = pPatch->GetPatch().Evaluate(fPX, fPY + fPYInc);
			vPos[1] = pPatch->GetPatch().Evaluate(fPX + fPXInc, fPY);
			vPos[2] = pPatch->GetPatch().Evaluate(fPX + fPXInc, fPY + fPYInc);

			fUVs[0] = P.Dot(LTVector(fTX, 0, fTY + fTYInc)) - fPDotO;
			fUVs[1] = Q.Dot(LTVector(fTX, 0, fTY + fTYInc)) - fQDotO;
			fUVs[2] = P.Dot(LTVector(fTX + fTXInc, 0, fTY)) - fPDotO;	
			fUVs[3] = Q.Dot(LTVector(fTX + fTXInc, 0, fTY)) - fQDotO;
			fUVs[4] = P.Dot(LTVector(fTX + fTXInc, 0, fTY + fTYInc)) - fPDotO;
			fUVs[5] = Q.Dot(LTVector(fTX + fTXInc, 0, fTY + fTYInc)) - fQDotO;

			for(nUV = 0; nUV < 3; nUV++)
			{
				fUVs[nUV * 2 + 0] /= pPatch->GetPatchTextureWidth();
				fUVs[nUV * 2 + 1] /= pPatch->GetPatchTextureHeight();
			}

			CreatePatchBrush(pRegion, pParent, pPatch, vPos, fUVs);
		}
	}

	return true;
}

//this function recursively travels the node tree and removes patches from the world tree,
//adding them to the list of patches, and also stores the first terrain object encountered
static void CreatePatches_R(CEditRegion* pRegion, CWorldNode* pNode)
{
	ASSERT(pNode);

	//see if we have a patch
	if(pNode->GetType() == Node_Patch)
	{
		// Attach the children to the parent node.
		GPOS pos;
		for(pos=pNode->m_Children; pos; )
		{
			no_AttachNode(pRegion, pNode->m_Children.GetNext(pos), pNode->GetParent());
		}

		//remember who the parent of this patch was
		CWorldNode* pParent = pNode->GetParent();

		//now remove this node
		no_DetachNode(pRegion, pNode, FALSE);

		//now actually handle creating it
		ConvertPatchToBrushes(pRegion, pParent, (CEditPatch*)pNode);		
	}

	//now we need to recurse through all the children of the object
	// Attach the children to the parent node.
	GPOS pos;
	for(pos=pNode->m_Children; pos; )
	{
		CWorldNode* pChild =  pNode->m_Children.GetNext(pos);

		//recurse
		CreatePatches_R(pRegion, pChild);
	}
}


//now we need to instantiate all patches, converting them into terrain
static bool ConvertPatchesToBrushes(CEditRegion* pRegion)
{
	CreatePatches_R(pRegion, pRegion->GetRootNode());

	return true;
}


static bool LoadInputRegionLTA(LTVector& vWorldOffset)
{
	RegionLoadStatus status;

	uint32 nVersion;

	//time the loading of the LTA file
	clock_t nStartTime = clock();
	status = g_InputRegion->LoadLTA( g_pGlobs->m_InputFile, NULL, nVersion );
	clock_t nEndTime = clock();

	if(status == REGIONLOAD_INVALIDFILE)
	{
		DrawStatusText(eST_Error, "The file is corrupt (error code %d).", g_InputRegion->m_LoadErrorCode);
		return false;
	}
	
	//display the time needed to load the LTA file
	DrawStatusText(eST_Normal, "LTA file loaded in %.2f seconds", 
					(nEndTime - nStartTime) / (float)CLOCKS_PER_SEC);

	//bind all prefabs
	g_InputRegion->GetPrefabMgr()->SetRootPath(g_pGlobs->m_ProjectDir);
	g_InputRegion->GetPrefabMgr()->BindAllRefs(g_InputRegion->GetRootNode());

	//update our version
	bool bModified;
	g_InputRegion->PostLoadUpdateVersion(nVersion, bModified);

	//disconnect all prefabs
	DrawStatusText(eST_Normal, "Instantiating prefabs");
	RecurseAndDisconnectPrefabs(g_InputRegion, g_InputRegion->GetRootNode());

	//move the entire world so that it is centered around the origin, this greatly
	//helps our accuracy on most levels

	if(g_pGlobs->m_bCenterWorldAroundOrigin)
	{
		//see if we need to center the world
		DrawStatusText(eST_Normal, "Centering world around origin");
		CenterWorldAroundOrigin(*g_InputRegion, vWorldOffset);
		DrawStatusText(eST_Normal, "World offset by (%.2f %.2f %.2f)", vWorldOffset.x, vWorldOffset.y, vWorldOffset.z);
	}
	else
	{
		vWorldOffset.Init();
		DrawStatusText(eST_Warning, "Not offsetting world. This is only recommended for engineering and should not be used for other purposes");
	}


	//now we need to instantiate all patches, converting them into terrain
	DrawStatusText(eST_Normal, "Converting patches to brushes");
	ConvertPatchesToBrushes(g_InputRegion);

	//create any edges that are applicable
	DrawStatusText(eST_Normal, "Creating polygon edging");
	CreatePolyEdges(g_InputRegion);

	//removes all decal objects and if we are generating geometry, will create the
	//appropriate world model geometry
	DrawStatusText(eST_Normal, "Creating decal geometry");
	CreateDecals(g_InputRegion, !g_pGlobs->m_bObjectsOnly);

	//now we need to provide the oppertunity for overriding ambient on certain objects
	DrawStatusText(eST_Normal, "Applying ambient overrides");
	ApplyAmbientOverride(g_InputRegion);

	//fill in any group objects with the objects that they contain
	DrawStatusText(eST_Normal, "Filling in group objects");
	FillInGroupObjects(g_InputRegion);

	//have all the lights in the level figure out what light group they are in
	DrawStatusText(eST_Normal, "Filling in light group fields");
	FillInLightGroupFields(g_InputRegion->GetRootNode(), NULL);

	//apply the render group to objects under the render group objects
	DrawStatusText(eST_Normal, "Applying render groups");
	ApplyRenderGroups(g_InputRegion);

	uint32			i;
	CBaseEditObj	*pObject;
	LTVector		vPos;
	
	DrawStatusText(eST_Normal, "Searching for invalid objects and removing hidden");
	for(i=0; i < g_InputRegion->m_Objects; i++)
	{
		pObject = g_InputRegion->m_Objects[i];
		vPos	= pObject->GetPos();
		
		if(vPos.x > g_pGlobs->m_MaxBounds || vPos.x < -g_pGlobs->m_MaxBounds || 
			vPos.y > g_pGlobs->m_MaxBounds || vPos.y < -g_pGlobs->m_MaxBounds || 
			vPos.z > g_pGlobs->m_MaxBounds || vPos.z < -g_pGlobs->m_MaxBounds ||
			!_finite(vPos.x) || !_finite(vPos.y) || !_finite(vPos.z))
		{
			// Whine...
			DrawStatusText(eST_Warning, "Object '%s' (class '%s') has an invalid position (%.2f %.2f %.2f).", 
				pObject->GetName(), pObject->GetClassName(), vPos.x, vPos.y, vPos.z);
			
			no_DestroyNode(g_InputRegion, pObject, FALSE);
			--i;
		}

		// Delete hidden objects
		if (ShouldIgnoreNode(pObject))
		{
			no_DestroyNode(g_InputRegion, pObject, FALSE);
			--i;
		}		
	}
	
	g_pGlobs->m_nInputPolies = g_InputRegion->GetTotalNumPolies();
	g_pGlobs->m_nInputVertices = g_InputRegion->GetTotalNumPoints();
	g_pGlobs->m_nInputObjects = g_InputRegion->m_Objects.GetSize();
	
	// Get rid of brushes > max detail level.
	DrawStatusText(eST_Normal, "Removing extra detail brushes");
	g_InputRegion->ClearSelections();
	RemoveDetailBrushes(g_InputRegion);

	return true;  //! for now
}


bool SaveInputRegion()
{

	CLTAFile OutFile;

	//try and open the file
	OutFile.Open(	g_pGlobs->m_InputFile, false, 
					CLTAUtil::IsFileCompressed(g_pGlobs->m_InputFile));

	if(OutFile.IsValid())
	{
		g_InputRegion->SaveLTA(&OutFile);
		return true;
	}
	else
	{
		DrawStatusText(eST_Error, "Can't open LTA file %s for writing.", g_pGlobs->m_InputFile);
		return false;
	}
}
 
bool MakeBsp(
	CPreWorld *pWorld, 
	bool bMinimizePolySize, 
	bool bAddHullMakers, 
	bool bUseOriginals)
{
	CBspGen			bspGen;
	uint32			i;
	GPOS			pos;
	CPrePoly		*pPoly;
	CBspGenOptions	options;
	bool			bRet;


	// Show status stuff.
	StartProgressBar(PROGRESS_BSP);


	// Make the BSP tree.
	bspGen.SetStatusFunction(BspStatusFunction);
	
	options.m_pWorld			= pWorld;
	options.m_bCapPolySize		= bMinimizePolySize;
	options.m_bAddHullMakers	= bAddHullMakers;
	options.m_nThreadsToUse		= g_pGlobs->m_nThreads;
	options.m_bUseOriginals		= bUseOriginals;

	bRet = bspGen.GenerateBspTree(&options);
	
	EndProgressBar(PROGRESS_BSP, false);


	// Make all polies full brightness, so they're not required to light it.
	for(pos=pWorld->m_Polies; pos; )
	{
		pPoly = pWorld->m_Polies.GetNext(pos);
		
		for(i=0; i < pPoly->NumVerts(); i++)
		{
			pPoly->Color(i).Init(255.0f, 255.0f, 255.0f);
		}
	}

	DrawStatusTextIfMainWorld(eST_Normal, "Num detail nodes: %d", pWorld->NumDetailNodes() );
	return bRet;	
}


bool MakeHulls(
	CPreWorld *pMakeOn, 
	CBspHullMaker *pMaker)
{
	BHMStatus bhmStatus;
	CBspHullMaker defaultMaker;


	if(!pMaker)
		pMaker = &defaultMaker;

	// Show hull status stuff.
	StartProgressBar(PROGRESS_HULL);

	bhmStatus = MakeHulls(pMaker, pMakeOn, HullPercentageCallback);
	if(bhmStatus != BHM_Ok)
		DrawStatusText(eST_Error, "Error making hulls.");

	EndProgressBar(PROGRESS_HULL, false);

	return bhmStatus == BHM_Ok;
}


bool MakeVisList( CPreMainWorld *pMainWorld )
{
	CVisListBuilder builder;
	VLBStatus visStatus;
	CBspHullMaker maker;
	CPreWorld *pVisBSP;


	pVisBSP = pMainWorld->GetVisBSP();
	if(!pVisBSP)
	{
		DrawStatusText(eST_Error, "No visibility BSP!");
		return false;
	}

	// Setup hulls.
	if(!MakeVisBSPLeaves(pVisBSP))
		return false;


	// Show vis status stuff.
	StartProgressBar(PROGRESS_VIS);

	//set up the builder that will be used to create the fast visibility
	memset(&builder, 0, sizeof(builder));
	builder.m_pWorld		= pVisBSP;
	builder.m_FastCallback	= FastVisPercentageCallback;
	builder.m_Callback		= VisPercentageCallback;
	builder.m_bFast			= true;
	builder.m_nThreads		= g_pGlobs->m_nThreads;
	builder.m_bDistanceCull	= g_pGlobs->m_bDistanceCullVisList;
	builder.m_CullDistance	= g_pGlobs->m_CullDistanceVisList;

	//see if they are doing distance based culling, if so, we should
	//report this to them so they don't forget about it
	if(builder.m_bDistanceCull)
	{
		DrawStatusText(eST_Normal, "Using distance culling on PVS with a distance of %.2f", 
						builder.m_CullDistance);
	}

	g_bInFullVis = false;
	visStatus = MakeVisList(&builder);
	
	if( visStatus != VLB_Ok )
	{
		DrawStatusText(eST_Error, "Error making PVS.");
		return false;
	}

	EndProgressBar(PROGRESS_VIS, false);
	return true;
}


// Find a brush that contains the specified polygon.
static CEditBrush* FindBrushWithPoly(CEditRegion *pRegion, CEditPoly *pPoly)
{
	LPOS		lPos;
	CEditBrush	*pBrush;
	uint32		iPoly;

	for(lPos=pRegion->m_Brushes; lPos; )
	{
		pBrush = pRegion->m_Brushes.GetNext(lPos);

		for(iPoly=0; iPoly < pBrush->m_Polies; iPoly++)
		{
			if(pBrush->m_Polies[iPoly] == pPoly)
			{
				return pBrush;
			}
		}
	}

	return NULL;
}


// Returns FALSE if they had -SelectHullProblemBrushes.
bool RemoveUnseenPortions(CPreWorld *pWorld)
{
	CUnseen			unseen;
	CEditBrush		*pBrush;
	uint32			i, nSelected;
	CBspHullMaker	maker;

	
	MakeHulls(pWorld, &maker);
	
	// Select problem brushes?
	if(g_pGlobs->m_bSelectHullProblemBrushes && g_InputRegion)
	{
		nSelected = 0;
		for(i=0; i < maker.m_ProblemPolies; i++)
		{
			if(pBrush = FindBrushWithPoly(g_InputRegion, maker.m_ProblemPolies[i]))
			{
				g_InputRegion->SelectNode(pBrush);
				++nSelected;
			}
		}

		DrawStatusText(eST_Normal, "");
		DrawStatusText(eST_Normal, "---------------");
		DrawStatusText(eST_Normal, "SelectHullProblemBrushes: Selected %d brushes.", nSelected);
		DrawStatusText(eST_Normal, "---------------");

		SaveInputRegion();
		return false;
	}

	g_pGlobs->m_nUnseenPolies = unseen.RemoveUnseenPortions(pWorld, pWorld->m_pMainWorld->m_Objects);
	DrawStatusTextIfMainWorld(eST_Normal, "Number of (unseen) polies removed: %d", g_pGlobs->m_nUnseenPolies);

	return true;
}


static void RemoveHullMakers(CPreWorld *pWorld)
{
	GPOS		pos;
	CPrePoly	*pPoly;
	CPreSurface *pSurface;

	for(pos=pWorld->m_Polies; pos; )
	{
		pPoly = pWorld->m_Polies.GetNext(pos);

		if(pPoly->m_pSurface->m_Flags & SURF_HULLMAKER)
		{
			pWorld->m_Polies.RemoveAt(pPoly);
			DeletePoly(pPoly);
		}
	}


	for(pos=pWorld->m_Surfaces; pos; )
	{
		pSurface = pWorld->m_Surfaces.GetNext(pos);

		if(pSurface->m_Flags & SURF_HULLMAKER)
		{
			pWorld->m_Surfaces.RemoveAt(pSurface);
			delete pSurface;
		}
	}
}



// ------------------------------------------------------------------------- //
// Sets surface flags, gotten from the property lists.
// ------------------------------------------------------------------------- //

inline uint32 GetHashCode(PVector &normal, PReal &dist)
{
	return	(uint32)((double)normal.x * 10000.0f) +
			(uint32)((double)normal.y * 10000.0f) +
			(uint32)((double)normal.z * 10000.0f) +
			(uint32)((double)dist * 10000.0f);
}


// Finds a plane in the list that matches the specifications you pass in.
// The actual floating point values must match exactly for a plane to be returned.
CPrePlane* FindPlaneExact(CLinkedList<CPrePlane*> *pList, PVector &normal, PReal dist)
{
	LPOS		pos;
	CPrePlane	*pTestPlane, *pBestPlane;
	double		minDist;

	
	minDist = 10000000.0f;
	pBestPlane = NULL;
	for(pos=pList->GetHeadPosition(); pos; )
	{
		pTestPlane = pList->GetNext(pos);
		
		if(pTestPlane->m_Normal.x == normal.x &&
			pTestPlane->m_Normal.y == normal.y &&
			pTestPlane->m_Normal.z == normal.z && 
			pTestPlane->m_Dist == dist)
		{
			return pTestPlane;
		}
	}

	return NULL;
}


void GenerateSurfaces(GenList<CEditBrush*> &brushes, CPreWorld *pWorld)
{
	uint32		j;
	CEditBrush	*pBrush;
	CEditPoly	*pPoly;
	CPreSurface	*pSurface;
	CStringProp *pProp;
	CColorProp	*pColorProp;
	PrePortal	*pPortal;
	char		baseTextureName[512];
	PVector		normal;
	uint32		iList;
	PReal		dist;
	GenListPos	pos;

	CLinkedList<CPrePlane*> planeLists[NUM_PLANE_LISTS];


	for(pos=brushes.GenBegin(); brushes.GenIsValid(pos); )
	{
		pBrush = brushes.GenGetNext(pos);

		for(j=0; j < pBrush->m_Polies; j++)
		{
			pPoly = pBrush->m_Polies[j];

			// Setup the surface.
			pSurface = new CPreSurface;

			uint32 nCurrTex;
			for(nCurrTex = 0; nCurrTex < CPreSurface::NUM_TEXTURES; nCurrTex++)
			{
				pSurface->m_Texture[nCurrTex].m_TextureO = pPoly->GetTexture(nCurrTex).GetO();
				pSurface->m_Texture[nCurrTex].m_TextureP = pPoly->GetTexture(nCurrTex).GetP();
				pSurface->m_Texture[nCurrTex].m_TextureQ = pPoly->GetTexture(nCurrTex).GetQ();
				pSurface->m_Texture[nCurrTex].m_pTextureName = pPoly->GetTexture(nCurrTex).m_pTextureName;
			}

			//now we need to make sure that all the textures come before the NULL, this implementation
			//only works with 2 textures
			assert((CPreSurface::NUM_TEXTURES == 2) && "This implementation needs to be expanded for the specified number of textures");
			if(pSurface->m_Texture[1].IsValid() && !pSurface->m_Texture[0].IsValid())
			{
				//we need to move it into the first channel
				std::swap(pSurface->m_Texture[0].m_TextureO, pSurface->m_Texture[1].m_TextureO);
				std::swap(pSurface->m_Texture[0].m_TextureP, pSurface->m_Texture[1].m_TextureP);
				std::swap(pSurface->m_Texture[0].m_TextureQ, pSurface->m_Texture[1].m_TextureQ);
				std::swap(pSurface->m_Texture[0].m_pTextureName, pSurface->m_Texture[1].m_pTextureName);
			}

			//assume that the surface's O always points to the first texture's O
			pSurface->O = pSurface->m_Texture[0].m_TextureO;

			normal = pPoly->m_Plane.m_Normal;
			dist = (PReal)pPoly->m_Plane.m_Dist;

			// (Using the hash table makes this about 40 times faster for large levels).
			// NOTE: it still only accepts EXACT duplicate planes so it doesn't lose 
			// precision here.. the hash table is only for speed (at one point, it didn't
			// always use exact duplicates and some levels with rotated brushes had tons
			// of problems).
			iList = GetHashCode(normal, dist) % NUM_PLANE_LISTS;
			pSurface->m_pPlane = FindPlaneExact(&planeLists[iList], normal, dist);
			if(!pSurface->m_pPlane)
			{
				pSurface->m_pPlane = pWorld->AddPlane(
					pPoly->m_Plane.m_Normal, pPoly->m_Plane.m_Dist);
				
				planeLists[iList].AddTail(pSurface->m_pPlane);
			}

			pSurface->m_Flags = GetBrushType(pBrush);
			
			// Hack Mc. Hackelson.. force SURF_INVISIBLE if using a certain texture name..
			for(nCurrTex = 0; nCurrTex < CPreSurface::NUM_TEXTURES; nCurrTex++)
			{
				if(pPoly->GetTexture(nCurrTex).m_pTextureName)
				{
					CHelpers::ExtractNames(pPoly->GetTexture(nCurrTex).m_pTextureName, NULL, baseTextureName, NULL, NULL);
					
					if(stricmp(baseTextureName, "invisible.dtx") == 0)
					{
						pSurface->m_Flags |= SURF_INVISIBLE;

						//however, if this is invisible, make sure that it isn't lightmapped
						pSurface->m_Flags &= ~SURF_LIGHTMAP;
					}
				}
			}
			
			if((pProp = (CStringProp*)pBrush->m_PropList.GetProp("TextureEffect")) && (pProp->m_Type == PT_STRING) && (pProp->m_String[0] != 0))
			{
				pSurface->m_pTextureEffect = pWorld->m_StringHolder.AddString(pProp->m_String);
			}

			//copy over properties to the surface
			pSurface->m_DetailLevel = (uint8)GetDetailLevel(pBrush);
			pSurface->m_LMGridSize	= GetLMGridSize(pBrush);
			if (!pSurface->m_LMGridSize)
				pSurface->m_LMGridSize = pWorld->GetDefaultLMGridSize();
			
			if(	(pColorProp = (CColorProp*)pBrush->m_PropList.GetProp("AmbientLight")) && 
				(pColorProp->m_Type == PT_COLOR))
			{
				pSurface->m_Ambient[0] = (uint8)(pColorProp->m_Vector.x);
				pSurface->m_Ambient[1] = (uint8)(pColorProp->m_Vector.y);
				pSurface->m_Ambient[2] = (uint8)(pColorProp->m_Vector.z);
			}

			CRealProp* pRealProp = (CRealProp*)pBrush->m_PropList.GetProp("LightPenScale");
			if(pRealProp && pRealProp->m_Type == PT_REAL)
			{
				pSurface->m_fLightPenScale = pRealProp->m_Value;
			}

			pPoly->m_pUser1 = pSurface;
			pSurface->m_pOriginalPoly = pPoly;

			pWorld->m_Surfaces.AddTail(pSurface);

			// Setup a portal if this is a portal.
			if((pSurface->m_Flags & SURF_PORTAL) && g_pGlobs->m_bPortals)
			{
				if((pProp = (CStringProp*)pBrush->m_PropList.GetProp("Name")) && pProp->m_Type == PT_STRING)
				{
					pPortal = new PrePortal;
					pPortal->m_Index = pWorld->m_Portals.GetSize();
					pPortal->m_pName = pWorld->m_StringHolder.AddString(pProp->m_String);
					pWorld->m_Portals.Append(pPortal);
					
					pSurface->m_pPortal = pPortal;
					pPortal->m_pSurface = pSurface;
				}
			}
		}
	}

	pWorld->SetupSurfaceTextureVectors();
}


static void GetWorldStats(CPreMainWorld *pMainWorld)
{
	CPreWorld	*pWorld;
	uint32		i;

	g_pGlobs->m_nOutputPolies		= 0;
	g_pGlobs->m_nOutputVertices		= 0;
	g_pGlobs->m_TreeDepth			= 0;
	g_pGlobs->m_VisListMemUse		= 0;
	g_pGlobs->m_nLeaves				= 0;
	g_pGlobs->m_nVisibleLeaves		= 0;
	g_pGlobs->m_LMDataSize			= pMainWorld->CalcLMDataSize();
	
	for(i=0; i < pMainWorld->m_WorldModels; i++)
	{
		pWorld = pMainWorld->m_WorldModels[i];

		if(pWorld->m_WorldInfoFlags & WIF_MAINWORLD)
		{
			g_pGlobs->m_nOutputPolies += pWorld->m_Polies;
			g_pGlobs->m_nOutputVertices += pWorld->m_nPointsSaved;
			g_pGlobs->m_TreeDepth += pWorld->GetTreeDepth();
			g_pGlobs->m_VisListMemUse += pWorld->TotalVisListSize();
			g_pGlobs->m_nLeaves += pWorld->m_Leafs;
			g_pGlobs->m_nVisibleLeaves += pWorld->NumVisibleLeaves();
		}
	}
}


static void ShowStatusText(DWORD startTime, DWORD endTime)
{
	DrawStatusText(eST_Normal,  "Done in %.2f minutes", ((endTime - startTime) / 1000.0f) / 60.0f );
	
	DrawStatusText(eST_Normal,  "" );
	
	DrawStatusText(eST_Normal,  "Number of input polies: %d", g_pGlobs->m_nInputPolies );
	DrawStatusText(eST_Normal,  "Number of input vertices: %d", g_pGlobs->m_nInputVertices );
	
	DrawStatusText(eST_Normal,  "" );
	
	DrawStatusText(eST_Normal,  "Number of output polies: %d", g_pGlobs->m_nOutputPolies );
	DrawStatusText(eST_Normal,  "Number of output vertices: %d", g_pGlobs->m_nOutputVertices );
	DrawStatusText(eST_Normal,  "Tree depth: %d", g_pGlobs->m_TreeDepth );
	DrawStatusText(eST_Normal,  "Memory for vis lists: %d", g_pGlobs->m_VisListMemUse );
	
	DrawStatusText(eST_Normal,  "Number of (unseen) polies removed: %d", g_pGlobs->m_nUnseenPolies);
	
	DrawStatusText(eST_Normal,  "" );

	DrawStatusText(eST_Normal,  "Number of leaves: %d", g_pGlobs->m_nLeaves );
	DrawStatusText(eST_Normal,  "Number of visible leaves: %d", g_pGlobs->m_nVisibleLeaves );

	DrawStatusText(eST_Normal,  "" );

	DrawStatusText(eST_Normal,  "Lightmap data size: %d", g_pGlobs->m_LMDataSize );
	DrawStatusText(eST_Normal,  "Number of objects: %d", g_pGlobs->m_nInputObjects );

	DrawStatusText(eST_Normal,  "" );
}

static PReal GetMaxLMSize(const char *pInfoString)
{
	ConParse parse;
	uint32 size;

	parse.Init(pInfoString);
	if(parse.ParseFind("MaxLMSize", FALSE, 2))
	{
		size = atoi(parse.m_Args[1]);
		if(IsValidLightmapSize(size))
			return (PReal)size;
	}
	
	return DEFAULT_LIGHTMAP_PIXELS;
}


static PReal GetLMGridSize(const char *pInfoString)
{
	ConParse parse;

	parse.Init(pInfoString);
	if(parse.ParseFind("LMGridSize", FALSE, 2))
	{
		return (PReal)atoi(parse.m_Args[1]); // Needs to be an integer value.
	}
	
	return DEFAULT_LIGHTMAP_GRID_SIZE;
}


bool MergeBrushes(
	GenList<CEditBrush*> &brushes,
	CPreWorld *pWorld)
{
	DrawStatusTextIfMainWorld(eST_Normal, "Merging %d brushes.", brushes.GenGetSize());
	StartProgressBar(PROGRESS_SETUP);

	if(!BrushToWorld(brushes, pWorld, BrushToWorldPercentageCallback))
	{
		return false;
	}

	EndProgressBar(PROGRESS_SETUP, false);
	return true;
}



static bool CreateVisAndPhysicsBSPs(
	CEditRegion		*pRegion,
	CPreMainWorld	*pMainWorld, 
	CWorldModelDef	*pWorldModel)
{
	CPreWorld *pPhysicsBSP, *pVisBSP;


	DrawStatusText(eST_Normal, "Creating physics BSP");
	pPhysicsBSP = new CPreWorld(pMainWorld);
	pPhysicsBSP->m_WorldInfoFlags = WIF_PHYSICSBSP | WIF_MAINWORLD;
	pMainWorld->m_WorldModels.GenAppend(pPhysicsBSP);
	SAFE_STRCPY(pPhysicsBSP->m_WorldName, "PhysicsBSP");
	if(!CreatePhysicsBSP(pRegion, pWorldModel, pPhysicsBSP, g_pGlobs->m_bRemoveUnseen))
		return false;

	DrawStatusText(eST_Normal, "Creating visibility BSP");
	pVisBSP = new CPreWorld(pMainWorld);
	pVisBSP->m_WorldInfoFlags = WIF_VISBSP;
	pMainWorld->m_WorldModels.GenAppend(pVisBSP);
	SAFE_STRCPY(pVisBSP->m_WorldName, "VisBSP");
	if(!CreateVisBSP(pRegion, pWorldModel, pVisBSP))
		return false;

	return true;
}


// Creates the initial CPreMainWorld from the list of world models.
static bool ImportGeometry(
	CEditRegion *pRegion, 
	CMoArray<CWorldModelDef*> &worldModels, 
	CPreMainWorld *pMainWorld)
{
	CPreWorld		*pPreWorld;
	CWorldModelDef	*pWorldModel, *pMainWorldModel;
	PVector			min, max;
	uint32			i;


	if(worldModels.GetSize() == 0)
	{
		DrawStatusText(eST_Error, "Must have at least one brush!");
		return false;
	}

	// Create the world models.
	if(g_InputRegion->m_Objects.GetSize() != pMainWorld->m_Objects.GetSize())
	{
		DrawStatusText(eST_Error, "world object counts don't match!");
		return false;
	}

	// Create the visibility and physics BSPs.

	// Create CPreWorlds and generate surfaces for all the world models.  
	// The surfaces are needed so the polies have planes in CreateWorldTree callbacks.
	for(i=0; i < worldModels; i++)
	{
		pWorldModel = worldModels[i];

		// Create the world.
		pPreWorld = new CPreWorld(pMainWorld);
		pMainWorld->m_WorldModels.Append(pPreWorld);

		// Create the surfaces.
		GenerateSurfaces(pWorldModel->m_Brushes, pPreWorld);
	}


	// Determine WorldTree subdivision (get bounding box of WIF_MAINWORLDs and filter
	// polies down to minimum size).
	if(!CreateWorldTree(&pMainWorld->m_WorldTree, worldModels, pMainWorld->m_pInfoString))
		return false;

	DrawStatusText(eST_Normal, "WorldTree nodes: %d", pMainWorld->m_WorldTree.m_nNodes);
	DrawStatusText(eST_Normal, "WorldTree depth: %d", pMainWorld->m_WorldTree.m_nDepth);
	DrawStatusText(eST_Normal, "WorldTree root size: %.2f", pMainWorld->m_WorldTree.m_RootNode.m_MinSize);

	
	// Don't include the main world model in the rest of this stuff.
	// (it gets setup after all the worldmodels).
	pMainWorldModel = worldModels[0];
	delete pMainWorld->m_WorldModels[0];
	pMainWorld->m_WorldModels.Remove(0);
	worldModels.Remove(0);


	for(i=0; i < worldModels; i++)
	{
		pWorldModel = worldModels[i];
		pPreWorld = pMainWorld->m_WorldModels[i];


		// Copy info over.
		pPreWorld->m_WorldInfoFlags = pWorldModel->m_WorldInfoFlags;
		SAFE_STRCPY(pPreWorld->m_WorldName, pWorldModel->m_WorldName);

		g_pGlobs->m_bMakingWorldModel = !(pPreWorld->m_WorldInfoFlags & WIF_MAINWORLD);

		if(!CreatePhysicsBSP(
				pRegion,
				pWorldModel,
				pPreWorld,
				false
				)
			)
		{
			return false;
		}
	}


	g_pGlobs->m_bMakingWorldModel = false;

	// Now create the main visibility and physics BSPs.
	// This must happen after the other WorldModels have been processed because it 
	// uses the terrain polies.
	if(!CreateVisAndPhysicsBSPs(pRegion, pMainWorld, pMainWorldModel))
		return false;

	// Get the texture flags.
	GetTextureFlags(pMainWorld);
	
	// Minimize the surface texture vectors (so certain hardware accelerators don't 
	// get flickering polies).
	pMainWorld->MinimizeSurfaceTCoords();
	
	GetWorldStats(pMainWorld);
	g_pGlobs->m_bMakingWorldModel = false;
	return true;
}

// Supporting code for the visibility tree traversal debug reporting
// Get a specified bit from a bit list
static bool GetBit(CMoByteArray &leafList, uint32 &iFinger)
{
	bool bResult;
	bResult = (leafList[iFinger >> 3] & (1 << (iFinger & 7))) != 0;
	iFinger++;
	return bResult;
}

// Print out a single node's leaf index in the visibility results
static void ReportVisibility(CNode *pNode, FILE *fp)
{
	fprintf(fp, "%d ", pNode->m_pLeaf->m_Index);
}

// Print out a list of visible nodes from a specified node
static void TraverseVisibility(CMoByteArray &leafList, uint32 &iFinger, CNode *pNode, FILE *fp)
{
	CNode *pBack, *pFront;

	// Report this node if it's visible
	if ((pNode->m_pLeaf) && GetBit(leafList, iFinger))
		ReportVisibility(pNode, fp);

	// Get the sides
	pBack = pNode->m_Sides[BackSide];
	pFront = pNode->m_Sides[FrontSide];

	// Recurse through the back if the visibility list tells us to
	if (IsValidNode(pBack) && GetBit(leafList, iFinger))
		TraverseVisibility(leafList, iFinger, pBack, fp);

	// Recurse through the front if the visibility list tells us to
	if (IsValidNode(pFront) && GetBit(leafList, iFinger))
		TraverseVisibility(leafList, iFinger, pFront, fp);
}

// removes the SURF_LIGHTMAP flag from all surfaces in this world
static bool ClearLightmapFlag(CPreMainWorld *pMainWorld)
{
	uint32		i;
	CPreWorld	*pWorld;
	GPOS		pos;
	CPreSurface *pSurface;

	if (!pMainWorld)
		return false;

	// for each worldmodel
	for (i = 0; i < pMainWorld->m_WorldModels; i++)
	{
		pWorld = pMainWorld->m_WorldModels[i];

		// for each surface
		for(pos=pWorld->m_Surfaces; pos; )
		{
			pSurface = pWorld->m_Surfaces.GetNext(pos);
			// clear the lightmap flag for this surface
			pSurface->m_Flags &= ~SURF_LIGHTMAP;
		}
	}
	return true;
}

static bool SavePortalFile(CPreMainWorld *pMainWorld, char *pFilename)
{
	FILE		*fp;
	CPreLeaf	*pLeaf;
	CHullPoly	*pPoly;
	CPreWorld	*pWorld;
	uint32		iLeaf, iPoly, iVert, nLeaves, iCurLeaf;


	pWorld = pMainWorld->GetVisBSP();
	if(!pWorld)
		return false;

	// Count the number of leaves.
	nLeaves = pWorld->m_Leafs.GetSize();
	
	if(fp = fopen(pFilename, "wt"))
	{
		fprintf(fp, "File Version: %d\n\n", 1);
		
		fprintf(fp, "Leaves: %d\n", nLeaves);
		iCurLeaf = 0;

		for(iLeaf=0; iLeaf < pWorld->m_Leafs.GetSize(); iLeaf++)
		{
			pLeaf = pWorld->m_Leafs[iLeaf];

			fprintf(fp, "\n\nLeaf #%lu\n\n", iCurLeaf);

			// Save poly vertices and count connections.
			fprintf(fp, "  Polies: %d\n", pLeaf->m_HullPolies.GetSize());
			for(iPoly=0; iPoly < pLeaf->m_HullPolies; iPoly++)
			{
				pPoly = pLeaf->m_HullPolies[iPoly];

				if(!(pPoly->m_Flags & HULLPOLY_CANTSEETHRUPORTAL) &&
					pPoly->m_pAdjacent && 
					pPoly->m_pAdjacent->m_pLeaf)
				{
					fprintf(fp, "    Connection: %lu\n", pWorld->m_Leafs.FindElement(pPoly->m_pAdjacent->m_pLeaf));
				}
				else
				{
					fprintf(fp, "    Connection: %lu\n", 0xFFFFFFFF);
				}
								
				fprintf(fp, "    Verts: %d\n", pPoly->NumVerts());

				for(iVert=0; iVert < pPoly->NumVerts(); iVert++)
				{
					fprintf(fp, "      %f %f %f\n", VEC_EXPAND(pPoly->Pt(iVert)));
				}
			}

			// Save the visibility flags (Not currently used by the renderer)
			if (!pLeaf->m_VisList.m_LeafLists.GetSize())
				fputs("    Visibility: \n", fp);
			else
			{
				fputs("    Visibility: ", fp);
				uint32 iFinger = 0;
				TraverseVisibility(pLeaf->m_VisList.m_LeafLists[0]->m_LeafList, iFinger, pWorld->m_RootNode, fp);
				fputc('\n', fp);
			}

			// Save the center position of the leaf
			fprintf(fp, "    Center: %f %f %f\n", pLeaf->m_Center.x, pLeaf->m_Center.y, pLeaf->m_Center.z);

			iCurLeaf++;
		}

		fclose(fp);
	}

	return true;
}

//pack for a specific platform
static bool PackForPlatform(CPreMainWorld* pMainWorld, const char* pszPlatform)
{
	//first off, we need to get the name of the file that we are loading without the
	//extension

	char pszFilename[MAX_PATH];

	strncpy(pszFilename, g_pGlobs->m_OutputFile, MAX_PATH);

	//now pull out the extension
	for(int32 nCharPos = strlen(pszFilename) - 1; nCharPos >= 0; nCharPos--)
	{
		//see if we hit the end of the filename, and are moving into the path
		if((pszFilename[nCharPos] == '\\') || (pszFilename[nCharPos] == '/'))
			break;

		if(pszFilename[nCharPos] == '.')
		{
			pszFilename[nCharPos] = '\0';
			break;
		}
	}

	//now determine the platform we are packing for
	IWorldPacker* pPacker = CPackerFactory::Create(pszPlatform);

	if(pPacker == NULL)
	{
		DrawStatusText(eST_Error, "Unable to find a packer for the %s platform", g_pGlobs->m_Platform);
		return false;
	}

	DrawStatusText(eST_Normal, "Packing world for the %s platform", g_pGlobs->m_Platform);

	bool bSuccess = pPacker->PackWorld(pszFilename, pMainWorld, g_pGlobs->m_bObjectsOnly);

	if(bSuccess == false)
	{
		DrawStatusText(eST_Error, "An error occurred packing the file for the %s platform", g_pGlobs->m_Platform);
	}

	delete pPacker;
	return bSuccess;
}

void DoProcessing(CProcessorGlobs *pGlobs)
{
	CPreMainWorld	world;
	uint32			startTime;
	uint32			endTime, nProcessors;
	char			timeString[128], dateString[128];
	bool			bWorldLoaded;
	CLightMapMaker	lightMaker;
	std::vector<CLightingPoint> lightingPoints;

	CMoArray<CWorldModelDef*> worldModels;


	g_pGlobs = pGlobs;
	g_pGlobs->m_bMakingWorldModel = false;
	startTime = GetMSTime();

	// Setup the region.
	g_InputRegion = new CEditRegion;

	CHelpers::ExtractNames(g_pGlobs->m_InputFile, NULL, NULL, g_InputFileTitle, NULL);

	if(g_pGlobs->m_bLogFile)
		OpenLogFile("", g_pLogFile, false);

	DrawStatusText(eST_Normal, "Processing %s", g_pGlobs->m_InputFile);
	DrawStatusText(eST_Normal, "");
	
	thd_GetTimeString(timeString);
	thd_GetDateString(dateString);
	DrawStatusText(eST_Normal, "Date: %s, Time: %s", dateString, timeString);

	nProcessors = thd_Init();
	DrawStatusText(eST_Normal, "This machine has %d processor%s.", nProcessors, (nProcessors>1)?"s":"");
	DrawStatusText(eST_Normal, "Using %d threads.", g_pGlobs->m_nThreads);
	DrawStatusText(eST_Normal, "Max detail level: %d.", g_pGlobs->m_MaxDetail);

	g_InputRegion->m_pStringHolder = &g_StringHolder;

	LithTry
	{
		// Try to update the objects if the LTA file exists.
		DrawStatusText(eST_Normal, "Getting objects from LTA file.");
		if(LoadInputRegionLTA(world.m_vWorldOffset))
		{
			// Save the changes if they want those objects removed.
			if(g_pGlobs->m_bRemoveProblemObjects)
			{
				DrawStatusText(eST_Normal, "Removing problem objects");
				
				SaveInputRegion();
				goto EndProcessorThread;
			}


			// convert keyframer objects to use blind object data for key information (removes key objects)
			DrawStatusText(eST_Normal, "Converting key data");
			if( !ConvertKeyData( g_InputRegion, &world ) )
				goto EndProcessorThread;

			DrawStatusText(eST_Normal, "Copying object heirarchy");
			CopyObjectList(g_InputRegion->m_Objects, world.m_Objects);
			DuplicateObjectHeirarchy(g_InputRegion->m_Objects, world.m_Objects);
			world.m_pInfoString = world.m_StringHolder.AddString(g_InputRegion->m_pInfoString);

			bWorldLoaded = true;
		}
	
		if(!g_pGlobs->m_bObjectsOnly)  
		{
			world.TermGeometry();

			// Get stuff from the info string.
			g_pGlobs->m_MaxLMSize = GetMaxLMSize(world.m_pInfoString);
			world.m_DefaultLMGridSize = (uint32)GetLMGridSize(world.m_pInfoString);

			DrawStatusText(eST_Normal, "Max Lightmap Size: %.2f", g_pGlobs->m_MaxLMSize);
			DrawStatusText(eST_Normal, "Default Lightmap Grid Size: %d", world.GetDefaultLMGridSize());

			FindWorldModels(g_InputRegion, worldModels);

			if(!ImportGeometry(g_InputRegion, worldModels, &world))
				goto EndProcessorThread;

			//do some cleanup of all the worlds
			world.RemoveAllUnusedGeometry();
			world.UpdateBoundingBox();

			// create scatter particles (scatter object specific)
			if( !ConvertScatter( &world, lightingPoints ) )
				goto EndProcessorThread;

			// Lighting.
			if(g_pGlobs->m_bLight)
			{
				// Light the world...
				DrawStatusText(eST_Normal, "");
				DrawStatusText(eST_Normal, "-------------------");
				DrawStatusText(eST_Normal, "Lighting world");

				StartProgressBar(PROGRESS_LIGHT);

				lightMaker.Init(&world,
					g_pGlobs->m_bShadows, 
					g_pGlobs->m_bLambertLight,
					LightStatusCallback);

				world.m_LightTable.InitLightTableSize(
												&world.m_PosMin,
												&world.m_PosMax,
												ParseLightTableRes(world.m_pInfoString));

				clock_t Start = clock();
				lightMaker.ProcessLight(world.m_LightTable, &world.m_pLightGrid, lightingPoints);
				clock_t End = clock();

				DrawStatusText(eST_Normal, "Lighting took %.2f seconds", (float)(End - Start) / CLOCKS_PER_SEC);
				EndProgressBar(PROGRESS_LIGHT, false);
			}	

			// If vertex lighting only was specified, clear the lightmap flag from all surfaces
			if(g_pGlobs->m_bVerticesOnly)
			{
				ClearLightmapFlag(&world);
			}

			// Visibility lists.
			{
				DrawStatusText(eST_Normal, "");
				DrawStatusText(eST_Normal, "-------------------");
				DrawStatusText(eST_Normal, "Making PVS.");

				if( !MakeVisList(&world) )
					goto EndProcessorThread;
			}
		}

		//save out the final RPD file
		//if( !SaveRPDFile(&world) )
		//	goto EndProcessorThread;

		//pack the world file for the specified platform
		if( !PackForPlatform(&world, g_pGlobs->m_Platform))
			goto EndProcessorThread;

		GetWorldStats( &world );
	}
	LithCatch(CLithException &x)
	{
		if( x.GetExceptionType() == LITH_MEMEXCEPTION )
			DrawStatusText(eST_Error, "Ran out of memory!");
		else
			DrawStatusText(eST_Error, "Internal error!  (Old file format?)");
	
		goto EndProcessorThread;
	}



	// (Only shows the status text if everything went successfully.)
	endTime = GetMSTime();

	DrawStatusText(eST_Normal, "");
	DrawStatusText(eST_Normal, "-------------------");
	ShowStatusText( startTime, endTime );


EndProcessorThread:;

	g_InputRegion->Term();
	delete g_InputRegion;

	thd_Term();

	CloseLogFiles();
}
