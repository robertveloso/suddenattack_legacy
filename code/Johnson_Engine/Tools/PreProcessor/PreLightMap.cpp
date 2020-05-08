//------------------------------------------------------------------
//
//	FILE	  : PreLightMap.cpp
//
//	PURPOSE	  : Implements the CPreLightMap class.
//
//	CREATED	  : September 30 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

// Includes....

#include "bdefs.h"
#include "prelightmap.h"
#include "lightmap_compress.h"
#include "lightmapdefs.h"



CPreLightMap::CPreLightMap()
{
	m_Width = m_Height = 0;
}


void CPreLightMap::Term()
{
	m_Width = m_Height = 0;
	m_Data.Term();
}


bool CPreLightMap::AllBlack()
{
	LM_DATA tempData[LIGHTMAP_MAX_TOTAL_PIXELS * 3];
	BYTE *pBytes;
	uint32 i, nBytes;


	if(m_bShadowMap)
	{
		if(!DecompressShadowMap((BYTE*)tempData))
			return false;

		nBytes = m_Width * m_Height;
	}
	else
	{
		if(!Decompress(tempData))
			return false;

		nBytes = m_Width * m_Height * sizeof(LM_DATA) * 3;
	}

	pBytes = (BYTE*)tempData;
	for(i=0; i < nBytes; i++)
	{
		if(pBytes[i] != 0)
			return false;
	}

	return true;
}


bool CPreLightMap::Compress(uint32 width, uint32 height, LM_DATA *pData)
{
	BYTE compressedData[LIGHTMAP_MAX_DATA_SIZE];
	uint32 dataLen;

	Term();

	if(!CompressLMData(pData, width, height, compressedData, dataLen))
		return false;

	if(!m_Data.SetSize(dataLen))
		return false;

	memcpy(m_Data.GetArray(), compressedData, dataLen);
	m_Width = width;
	m_Height = height;

	m_bShadowMap = false;
	return true;
}


bool CPreLightMap::Decompress(LM_DATA *pData) const
{
	ASSERT(!m_bShadowMap);
	return (DecompressLMData(m_Data.GetArray(), m_Data.GetSize(), pData)) ? true : false;
}


bool CPreLightMap::CompressShadowMap(uint32 width, uint32 height, LM_DATA *pData)
{
	LM_DATA compressedData[LIGHTMAP_MAX_TOTAL_PIXELS];
	uint32 dataLen;

	Term();

	if(!::CompressShadowMap(pData, width, height, compressedData, dataLen))
		return false;

	if(!m_Data.SetSize(dataLen))
		return false;

	memcpy(m_Data.GetArray(), compressedData, dataLen);
	m_Width		= width;
	m_Height	= height;

	m_bShadowMap = true;
	return true;
}


bool CPreLightMap::DecompressShadowMap(LM_DATA *pData)
{
	ASSERT(m_bShadowMap);
	return ::DecompressShadowMap(m_Data.GetArray(), m_Data.GetSize(), pData) ? true : false;
}


void CFullLightMap::ColorFill( PVector color )
{
	PVector *pCur, *pEnd;
	uint32 y;

	pCur = m_Map.GetArray();
	for(y=0; y < m_Height; y++)
	{
		pEnd = pCur + m_Width;
		while(pCur < pEnd)
		{
			*pCur = color;
			pCur++;
		}
	}
}

