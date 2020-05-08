// This file contains all the dtx file loading code...

#include "stdafx.h"
#include "dtx_files.h"
#include "d3d_device.h"

uint32 GetBPP(D3DFORMAT Format) 
{
	uint32 iPitch = 0;
	if (Format == D3DFMT_DXT1 || Format == D3DFMT_DXT3 || Format == D3DFMT_DXT5) { return 0; }
	else if (Format == D3DFMT_A8R8G8B8) { return 4; }
	else if (Format == D3DFMT_X8R8G8B8) { return 4; }
	else if (Format == D3DFMT_R8G8B8)	{ return 3; }
	else if (Format == D3DFMT_R5G6B5)	{ return 2; }
	else if (Format == D3DFMT_A1R5G5B5)	{ return 2; }
	else if (Format == D3DFMT_X1R5G5B5)	{ return 2; }
	else if (Format == D3DFMT_A4R4G4B4)	{ return 2; }
	return 0;
}

uint32 GetPitch(D3DFORMAT Format, uint32 iWidth)
{
	if (Format == D3DFMT_DXT1) { return iWidth*2; }
	else if (Format == D3DFMT_DXT3) { return iWidth*4; }
	else if (Format == D3DFMT_DXT5) { return iWidth*4; }
	else return (iWidth * GetBPP(Format));
}

uint32 CalcImageSize(D3DFORMAT Format, uint32 iWidth, uint32 iHeight)
{
	if (Format == D3DFMT_DXT1) return ((iWidth * iHeight) >> 1);
	else if (Format == D3DFMT_DXT3) return (iWidth * iHeight);
	else if (Format == D3DFMT_DXT5) return (iWidth * iHeight);
	else return (iHeight * GetPitch(Format,iWidth));
}

bool ConvertTexDataToDD(uint8* pSrcData, D3DFORMAT D3DSrcFormat, uint32 SrcWidth, uint32 SrcHeight, uint8* pDstData, D3DFORMAT D3DDstFormat, uint32 DstWidth, uint32 DstHeight)
{
	// Create a quick little surface to convert into....
	LPDIRECT3DSURFACE8 pD3DDstSurface = NULL; LPDIRECT3DTEXTURE8 pD3DDstTexture = NULL;
	HRESULT hResult = PD3DDEVICE->CreateTexture(DstWidth,DstHeight,1,NULL,D3DDstFormat,D3DPOOL_SYSTEMMEM,&pD3DDstTexture);
	if (hResult != D3D_OK || !pD3DDstTexture) return false;
	hResult = pD3DDstTexture->GetSurfaceLevel(0,&pD3DDstSurface);
	if (hResult != D3D_OK || !pD3DDstSurface) return false;

	uint32 SrcPitch = GetPitch(D3DSrcFormat,SrcWidth);
	RECT SrcRect; SrcRect.left = 0; SrcRect.top = 0; SrcRect.right = SrcWidth; SrcRect.bottom = SrcHeight;

	hResult = D3DXLoadSurfaceFromMemory(pD3DDstSurface,NULL,NULL,pSrcData,D3DSrcFormat,SrcPitch,NULL,&SrcRect,D3DX_FILTER_LINEAR,0);

	// Lock it and copy out it's data...
	D3DLOCKED_RECT LockedRect; pD3DDstSurface->LockRect(&LockedRect,NULL,NULL); 
	uint8* pD3DSrcData = (uint8*)LockedRect.pBits;
	if (D3DDstFormat == D3DFMT_DXT1 || D3DDstFormat == D3DFMT_DXT3 || D3DDstFormat == D3DFMT_DXT5) {
		memcpy(pDstData,pD3DSrcData,CalcImageSize(D3DDstFormat, DstWidth, DstHeight)); }
	else {	// Not compressed...
		for (uint y = 0; y < DstHeight; ++y) {
			memcpy(pDstData,pD3DSrcData,GetPitch(D3DDstFormat,DstWidth));
			pDstData    += GetPitch(D3DDstFormat,DstWidth);
			pD3DSrcData += LockedRect.Pitch; } }
	pD3DDstSurface->UnlockRect();

	uint32 iRefCnt = pD3DDstSurface->Release();
	iRefCnt = pD3DDstTexture->Release();
	if (hResult != D3D_OK) return false;

	return true;
}

LPDIRECT3DTEXTURE8 LoadDTXFile(const char* szFilename) 
{
	FILE* f = fopen(szFilename,"rb");
	if (!f) return NULL;

	DtxHeader hdr;										// Read in the header...
	if (!fread(&hdr,sizeof(hdr),1,f))					{ return NULL; }

	if (hdr.m_ResType != LT_RESTYPE_DTX)				{ return NULL; }
	if (hdr.m_Version != CURRENT_DTX_VERSION)			{ return NULL; }
	if (hdr.m_nMipmaps == 0 || hdr.m_nMipmaps > 15)		{ return NULL; }

	LPDIRECT3DTEXTURE8 pTexture = NULL; D3DFORMAT SrcFormat; bool bIsCompressed = false;
	switch (hdr.GetBPPIdent()) {
	case BPP_32: SrcFormat = D3DFMT_A8R8G8B8;	 bIsCompressed = false; break;
	case BPP_S3TC_DXT1: SrcFormat = D3DFMT_DXT1; bIsCompressed = true;  break;
	case BPP_S3TC_DXT3: SrcFormat = D3DFMT_DXT3; bIsCompressed = true;  break;
	case BPP_S3TC_DXT5: SrcFormat = D3DFMT_DXT5; bIsCompressed = true;  break;
	default : return NULL; }							// Unsupported format...

	// Create it...
	D3DSURFACE_DESC Desc; D3DLOCKED_RECT LockedRect;
	if (D3DXCreateTexture(PD3DDEVICE,hdr.m_BaseWidth,hdr.m_BaseHeight,D3DX_DEFAULT,NULL,SrcFormat,D3DPOOL_MANAGED,&pTexture) != D3D_OK) return NULL;
	if (pTexture->GetLevelDesc(0,&Desc) != D3D_OK)	{ pTexture->Release(); return NULL; }

	// Ok, not convert it...
	uint8* pSrcBits = new uint8[CalcImageSize(SrcFormat,hdr.m_BaseWidth,hdr.m_BaseHeight)];		if (!pSrcBits) { pTexture->Release(); return NULL; }
	uint8* pDstBits = new uint8[CalcImageSize(Desc.Format,hdr.m_BaseWidth,hdr.m_BaseHeight)];	if (!pDstBits) { pTexture->Release(); delete[] pSrcBits; return NULL; }

	if (!fread(pSrcBits,CalcImageSize(SrcFormat,hdr.m_BaseWidth,hdr.m_BaseHeight),1,f))			{ pTexture->Release(); delete[] pSrcBits; delete[] pDstBits; return NULL; }
	for (uint i = 0; i < pTexture->GetLevelCount(); ++i) {
		pTexture->GetLevelDesc(i,&Desc);
		if (!ConvertTexDataToDD(pSrcBits,SrcFormat,hdr.m_BaseWidth,hdr.m_BaseHeight,pDstBits,Desc.Format,Desc.Width,Desc.Height)) {
			if (i ==0) { pTexture->Release(); delete[] pSrcBits; delete[] pDstBits; return NULL; } else break; } 

		// Copy pDstBits into the Texture...
		if (pTexture->LockRect(i,&LockedRect,NULL,NULL) != D3D_OK)								{ pTexture->Release(); delete[] pSrcBits; delete[] pDstBits; return NULL; }
		if (!bIsCompressed) {
			uint8* pDst = (uint8*)LockedRect.pBits;
			uint8* pSrc = pDstBits;
			for (uint32 h = 0; h < Desc.Height; ++h) {
				memcpy(pDst,pSrc,GetPitch(Desc.Format, Desc.Width));
				pDst += LockedRect.Pitch;
				pSrc += GetPitch(Desc.Format, Desc.Width); } }
		else {
			memcpy(LockedRect.pBits,pDstBits,CalcImageSize(Desc.Format, Desc.Width, Desc.Height)); }
		pTexture->UnlockRect(i); }

	delete[] pSrcBits; delete[] pDstBits; 

	fclose(f);
	return pTexture;
}