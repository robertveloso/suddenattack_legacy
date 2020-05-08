
#include "bdefs.h"
#include "processing.h"
#include "preworld.h"
#include "gettextureinfo.h"


// Applies texture flags to all the world's surfaces.  Returns the number
// of surfaces it applied flags to.
uint32 ApplyTextureFlagsToWorld(CGLinkedList<TInfo*> &theList, CPreMainWorld *pMainWorld)
{
	uint32		nFlagsGotten;
	GPOS		pos;
	CPreSurface *pSurface;
	TInfo		*pInfo;
	uint32		i;
	CPreWorld	*pWorld;


	for(i=0; i < pMainWorld->m_WorldModels; i++)
	{
		pWorld = pMainWorld->m_WorldModels[i];

		nFlagsGotten = 0;
		for(pos=pWorld->m_Surfaces; pos; )
		{
			pSurface = pWorld->m_Surfaces.GetNext(pos);

			for(uint32 nCurrTex = 0; nCurrTex < CPreSurface::NUM_TEXTURES; nCurrTex++)
			{
				CPreTexture& Tex = pSurface->m_Texture[nCurrTex];

				if(!Tex.IsValid())
					continue;

				pInfo = FindTInfo(theList, Tex.m_pTextureName);
				if(pInfo)
				{	
					Tex.m_TextureFlags = pInfo->m_Flags;
					Tex.m_TextureWidth = pInfo->m_Width;
					Tex.m_TextureHeight = pInfo->m_Height;
					++nFlagsGotten;
				}
			}
		}
	}

	return nFlagsGotten;
}


uint32 GetTextureFlags(CPreMainWorld *pWorld)
{
	CGLinkedList<TInfo*>	theList;
	uint32					nFlagsGotten;

	
	if(GetTextureInfo(pWorld, theList) == 0)
	{
		DrawStatusText(eST_Error, "Couldn't find any textures. Make sure that the project path is properly set");
		return 0;
	}

	nFlagsGotten = 0;
	nFlagsGotten += ApplyTextureFlagsToWorld(theList, pWorld);
	
	GDeleteAndRemoveElements(theList);
	DrawStatusText(eST_Normal, "Imported %d texture flags", nFlagsGotten);

	return nFlagsGotten;
}




