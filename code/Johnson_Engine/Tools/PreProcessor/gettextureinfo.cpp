
#include "bdefs.h"
#include "processing.h"
#include "preworld.h"
#include "dtxmgr.h"
#include "streamsim.h"
#include "conparse.h"
#include "gettextureinfo.h"
#include "spritefile.h"

#define TEX_LIGHT_TOKEN_SEPARATOR	':'
#define MAX_TEX_LIGHT_TOKEN_ID_LEN	64


TInfo* FindTInfo(CGLinkedList<TInfo*> &theList, const char *pTextureName)
{
	GPOS pos;
	TInfo *pFlags;

	for(pos=theList; pos; )
	{
		pFlags = theList.GetNext(pos);
	
		if(CHelpers::UpperStrcmp(pTextureName, pFlags->m_pTextureName))
		{
			return pFlags;
		}
	}

	return NULL;
}


//Given a string holding a value for a texture light, will strip out the identifier
//part of the token, and put it into the buffer. It will not overwrite the end of
//the buffer
static void GetTexLightTokenID(const char* pszToken, char* pszBuffer, uint32 nBufferLen)
{
	//the buffer needs to be at least one byte
	if(nBufferLen == 0)
	{
		ASSERT(false);
		return;
	}

	//just copy over the token until we hit a separator character
	uint32 nCurrIndex		= 0;

	do
	{
		//get the current character
		char ch = pszToken[nCurrIndex];

		//if we have hit the end of the string, there is no ID
		if(ch == '\0')
		{
			pszBuffer[0] = '\0';
			return;
		}

		//if we hit the separator, we want to bail before we add it into the list
		if(ch == TEX_LIGHT_TOKEN_SEPARATOR)
		{
			break;
		}

		//copy over the character if we have room
		if(nCurrIndex < nBufferLen)
		{
			pszBuffer[nCurrIndex] = ch;
		}
		//next character
		nCurrIndex++;
	}
	while(1); //this will exit when the separator or end of string is hit

	//end the string and bail
	pszBuffer[LTMIN(nCurrIndex, nBufferLen - 1)] = '\0';
}

//this function will take a texture light token, strip off the ID, take the value, and
//return the value as a real number
static PReal GetTexLightTokenNumValue(const char* pszToken)
{
	//skip over the token until we hit the separator
	uint32 nCurrChar = 0;

	while(1) //loop exits when separator or end of string is hit
	{
		//check to see if we hit the separator
		if(pszToken[nCurrChar] == TEX_LIGHT_TOKEN_SEPARATOR)
		{
			//skip over the separator
			nCurrChar++;
			break;
		}

		//see if we hit the end of the string, in which case we need to reset
		//the current character to the first value (meaning the whole thing)
		//is a value
		if(pszToken[nCurrChar] == '\0')
		{
			nCurrChar = 0;
			break;
		}

		nCurrChar++;
	}

	//now we have the part of the string that we need to convert, so just convert and
	//bail
	return (PReal)atof(&(pszToken[nCurrChar]));
}


void ParseLightDefs(TInfo *pInfo, char *pString)
{
	BOOL bMore;
	char tokenSpace[PARSE_MAXARGS*PARSE_MAXARGLEN];
	char *pTokens[PARSE_MAXARGS], *pCommandPos, *pCommand;
	int nNumTokens;
	TLightDef *pLight;

	pCommand = pCommandPos = pString;
	bMore = 1;
	while(bMore)
	{
		pCommand = pCommandPos;
		bMore = cp_Parse(pCommand, &pCommandPos, tokenSpace, pTokens, &nNumTokens);

		//create the new light definition
		pLight = new TLightDef;

		//make sure that the allocation succeeded
		if(pLight == NULL)
		{
			//critically low memory
			return;
		}

		//add this light to the list of them
		pInfo->m_LightDefs.Append(pLight);

		//run through the list of tokens, and for each, determine what value this
		//corresponds to, and set the appropriate value. Start after the first token
		//since that is just the 'TextureLight' token
		for(uint32 nCurrToken = 1; nCurrToken < (uint32)nNumTokens; nCurrToken++)
		{
			//determine the type of the token
			char pszTokenID[MAX_TEX_LIGHT_TOKEN_ID_LEN];
			GetTexLightTokenID(pTokens[nCurrToken], pszTokenID, MAX_TEX_LIGHT_TOKEN_ID_LEN);

			//now get the value of the token as a number
			PReal fTokenVal = (PReal)GetTexLightTokenNumValue(pTokens[nCurrToken]);
			
			//the boolean equivilant of the token value
			bool bTokenVal = (fabs(fTokenVal) < 0.1) ? false : true;

			//handle the coordinates
			if(stricmp(pszTokenID, "U") == 0)
			{
				pLight->m_UOffset = fTokenVal;
			}
			else if(stricmp(pszTokenID, "V") == 0)
			{
				pLight->m_VOffset = fTokenVal;
			}
			else if(stricmp(pszTokenID, "Z") == 0)
			{
				pLight->m_ZOffset = fTokenVal;
			}

			//now the light color and intensity
			else if(stricmp(pszTokenID, "BrightScale") == 0)
			{
				pLight->m_BrightScale = fTokenVal;
			}
			else if(stricmp(pszTokenID, "IR") == 0)
			{
				pLight->m_InnerColor.x = fTokenVal;
			}
			else if(stricmp(pszTokenID, "IG") == 0)
			{
				pLight->m_InnerColor.y = fTokenVal;
			}
			else if(stricmp(pszTokenID, "IB") == 0)
			{
				pLight->m_InnerColor.z = fTokenVal;
			}

			//the properties of the light
			else if(stricmp(pszTokenID, "Directional") == 0)
			{
				pLight->m_bDirectional = bTokenVal;
			}
			else if(stricmp(pszTokenID, "Centered") == 0)
			{
				pLight->m_bCentered = bTokenVal;
			}
			else if(stricmp(pszTokenID, "LightModels") == 0)
			{
				pLight->m_bLightModels = bTokenVal;
			}
			else if(stricmp(pszTokenID, "Clip") == 0)
			{
				pLight->m_bClipLight = bTokenVal;
			}
			else if(stricmp(pszTokenID, "ApproxLinear") == 0)
			{
				pLight->m_bApproxLinear = bTokenVal;
			}
			else if(stricmp(pszTokenID, "FOV") == 0)
			{
				pLight->m_FOVDegrees = fTokenVal;
			}

			//now the lighting model to be used
			else if(stricmp(pszTokenID, "Radius") == 0)
			{
				pLight->m_Radius = fTokenVal;
			}
			else if(stricmp(pszTokenID, "A") == 0)
			{
				pLight->m_Coefficients.x = fTokenVal;
			}
			else if(stricmp(pszTokenID, "B") == 0)
			{
				pLight->m_Coefficients.y = fTokenVal;
			}
			else if(stricmp(pszTokenID, "C") == 0)
			{
				pLight->m_Coefficients.z = fTokenVal;
			}

			else
			{
				//unrecognized token. Perhaps should warn user
			}			
		}
	}
}


uint32 GetWorldTextureInfo(CPreWorld *pWorld, CGLinkedList<TInfo*> &theList)
{
	GPOS			pos;
	CPreSurface		*pSurface;
	uint32			nFlagsGotten;
	TInfo			*pFlags;
	char			fileName[MAX_PATH];
	ILTStream		*pStream;
	TextureData		*pData;
	DtxSection		*pSection;

	char			pszTextureName[MAX_PATH];

	nFlagsGotten = 0;

	for(pos=pWorld->m_Surfaces; pos; )
	{
		pSurface = pWorld->m_Surfaces.GetNext(pos);
	
		for(uint32 nCurrTex = 0; nCurrTex < CPreSurface::NUM_TEXTURES; nCurrTex++)
		{
			CPreTexture& Tex = pSurface->m_Texture[nCurrTex];

			if(!Tex.IsValid())
				continue;

			pFlags = FindTInfo(theList, Tex.m_pTextureName);
			if(pFlags)
			{
				++nFlagsGotten;
			}
			else
			{
				//see if this is a sprite
				if(CSpriteFile::IsSprite(Tex.m_pTextureName))
				{
					//build up the sprite filename
					sprintf(fileName, "%s\\%s", g_pGlobs->m_ProjectDir, Tex.m_pTextureName);

					//open up the file
					CSpriteFile Sprite;

					if(!Sprite.Load(fileName))
						continue;

					if(Sprite.GetNumFrames() > 0)
					{
						//read in the first string
						strcpy(pszTextureName, Sprite.GetFrame(0));
					}
				}
				else
				{
					//we have a texture, so lets copy it over
					strcpy(pszTextureName, Tex.m_pTextureName);
				}

				sprintf(fileName, "%s\\%s", g_pGlobs->m_ProjectDir, pszTextureName);
				
				pStream = streamsim_Open(fileName, "rb");
				if(pStream)
				{
					if(dtx_Create(pStream, &pData, TRUE, TRUE) == DE_OK)
					{
						pFlags = new TInfo;
						pFlags->m_Width = pData->m_Header.m_BaseWidth;
						pFlags->m_Height = pData->m_Header.m_BaseHeight;
						pFlags->m_pTextureName = (char*)Tex.m_pTextureName;
						pFlags->m_Flags = (uint16)pData->m_Header.m_UserFlags;
						theList.Append(pFlags);
						
						// Parse the LIGHTDEFS section.
						if(pSection = dtx_FindSection(pData, "LIGHTDEFS"))
						{
							ParseLightDefs(pFlags, pSection->m_Data);
						}

						++nFlagsGotten;
						dtx_Destroy(pData);
					}
				
					pStream->Release();
				}
			}
		}
	}

	return nFlagsGotten;
}


uint32 GetTextureInfo(CPreMainWorld *pMainWorld, CGLinkedList<TInfo*> &theList)
{
	uint32 nFlagsGotten = 0;

	for(uint32 i = 0; i < pMainWorld->m_WorldModels.GetSize(); i++)
	{
		CPreWorld *pWorld = pMainWorld->m_WorldModels[i];
		
		nFlagsGotten += GetWorldTextureInfo(pMainWorld->m_WorldModels[i], theList);
	}
	
	return nFlagsGotten;
}




