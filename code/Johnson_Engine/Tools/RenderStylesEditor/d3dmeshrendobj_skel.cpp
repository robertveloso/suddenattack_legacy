// d3dmeshrendobj_skel.cpp

#include "stdafx.h"

#include "d3dmeshrendobj_skel.h"
#include "d3d_device.h"
#include "d3d_renderstatemgr.h"
#include "ltb.h"

CD3DSkelMesh::CD3DSkelMesh()
{
	Reset();
}

CD3DSkelMesh::~CD3DSkelMesh()
{
	FreeAll();
}

void CD3DSkelMesh::Reset()
{
	m_VBController.Reset();
	m_iMaxBonesPerVert	= 0;
	m_iMaxBonesPerTri	= 0;
	m_iVertCount		= 0;
	m_iPolyCount		= 0;
	m_eRenderMethod		= eD3DRenderMatrixPalettes;
	m_iBoneSetCount		= 0;
	m_pBoneSetArray		= NULL;
	m_bReIndexedBones	= false;
	m_pReIndexedBoneList= NULL;
}

void CD3DSkelMesh::FreeAll()
{
	m_VBController.FreeAll();
	if (m_pBoneSetArray)		{ delete[] m_pBoneSetArray; m_pBoneSetArray = NULL; }
	if (m_pReIndexedBoneList)	{ delete[] m_pReIndexedBoneList; m_pReIndexedBoneList = NULL; }
	Reset();
}

bool CD3DSkelMesh::Load(uint8* pSrcData)
{
	// Note: This is a renderobject file...
	LTB_Header Header; uint32 iSizeOf;
	memcpy(&Header,pSrcData,sizeof(Header));											pSrcData += sizeof(Header);
	memcpy(&iSizeOf,pSrcData,sizeof(iSizeOf));											pSrcData += sizeof(iSizeOf);

	// Read in the basics...
	uint32 iObjSize; memcpy(&iObjSize,pSrcData,sizeof(iObjSize));		pSrcData += sizeof(iObjSize);
	memcpy(&m_iVertCount,pSrcData,sizeof(m_iVertCount));				pSrcData += sizeof(m_iVertCount);
	memcpy(&m_iPolyCount,pSrcData,sizeof(m_iPolyCount));				pSrcData += sizeof(m_iPolyCount);
	memcpy(&m_iMaxBonesPerTri,pSrcData,sizeof(m_iMaxBonesPerTri));		pSrcData += sizeof(m_iMaxBonesPerTri);
	memcpy(&m_iMaxBonesPerVert,pSrcData,sizeof(m_iMaxBonesPerVert));	pSrcData += sizeof(m_iMaxBonesPerVert);
	memcpy(&m_bReIndexedBones,pSrcData,sizeof(m_bReIndexedBones));		pSrcData += sizeof(m_bReIndexedBones);
	memcpy(&m_VertStreamFlags[0],pSrcData,sizeof(uint32)*4);			pSrcData += sizeof(uint32)*4;

	// Are we using Matrix Palettes...
	bool bUseMatrixPalettes; 
	memcpy(&bUseMatrixPalettes,pSrcData,sizeof(bool));					pSrcData += sizeof(bool);
	if (bUseMatrixPalettes) {
		m_eRenderMethod = eD3DRenderMatrixPalettes;
		return Load_MP(pSrcData); }
	else { return false; }
}

bool CD3DSkelMesh::Load_MP(uint8* pSrcData)
{
	// Read in out Min/Max Bones (effecting this guy)...
	memcpy(&m_iMinBone,pSrcData,sizeof(m_iMinBone));					pSrcData += sizeof(m_iMinBone);
	memcpy(&m_iMaxBone,pSrcData,sizeof(m_iMaxBone));					pSrcData += sizeof(m_iMaxBone);

	// What type of Vert do we need?
	VertexBufferController::VERTEX_BLEND_TYPE VertType;
	switch (m_iMaxBonesPerVert) {
	case 2  : VertType = VertexBufferController::eINDEXED_B1; break;
	case 3  : VertType = VertexBufferController::eINDEXED_B2; break;
	case 4  : VertType = VertexBufferController::eINDEXED_B3; break; 
	default : assert(0); return false; }

	// If we are using re-indexed bones, read them in...
	if (m_bReIndexedBones) {
		uint32 iBoneCount = 0;
		memcpy(&iBoneCount,pSrcData,sizeof(iBoneCount));				pSrcData += sizeof(iBoneCount);
		assert(iBoneCount < 10000 && "Crazy bone count, checked your packed model format.");
		m_pReIndexedBoneList = new uint32[iBoneCount];
		memcpy(m_pReIndexedBoneList,pSrcData,sizeof(uint32)*iBoneCount); pSrcData += sizeof(uint32)*iBoneCount; }

	// Create our VB...
	for (uint32 i=0;i<4;++i) {
		if (!m_VertStreamFlags[i]) continue;
		if (!m_VBController.CreateStream(i,m_iVertCount,m_VertStreamFlags[i],VertType,false,true)) { FreeAll(); return false; } }
	if (!m_VBController.CreateIndexBuffer(m_iPolyCount*3)) { FreeAll(); return false; }

	// Read in our Verts...
	for (i=0;i<4;++i) {
		if (!m_VertStreamFlags[i]) continue;
		m_VBController.Lock((VertexBufferController::VB_TYPE)(VertexBufferController::eVERTSTREAM0 + i),false); //true);
		uint8* pVertData = (uint8*)m_VBController.getVertexData(i);
		memcpy(pVertData,pSrcData,m_VBController.getVertexSize(i) * m_iVertCount);	pSrcData += m_VBController.getVertexSize(i) * m_iVertCount;
		m_VBController.UnLock((VertexBufferController::VB_TYPE)(VertexBufferController::eVERTSTREAM0 + i)); }

	// Read in pIndexList...
	m_VBController.Lock(VertexBufferController::eINDEX,false); //true);
	memcpy(m_VBController.getIndexData(),pSrcData,sizeof(uint16) * m_iPolyCount * 3); pSrcData += sizeof(uint16) * m_iPolyCount * 3;
	m_VBController.UnLock(VertexBufferController::eINDEX);

	return true;
}

inline void CD3DSkelMesh::SetTransformsToBoneSet(BoneSetListItem* pBoneSet,D3DMATRIX* pTransforms)
{
	uint32 iBoneCount = 0;
	for (uint32 i=0;i<4;++i) {
		if (pBoneSet->BoneSetArray[i] == 0xFF) continue;
		g_RenderStateMgr.SetTransform(D3DTS_WORLDMATRIX(i),&pTransforms[pBoneSet->BoneSetArray[i]]);
		++iBoneCount; }

	switch (iBoneCount) {
		case 2  : PD3DDEVICE->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_1WEIGHTS); break;
		case 3  : PD3DDEVICE->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_2WEIGHTS); break;
		case 4  : PD3DDEVICE->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_3WEIGHTS); break;
		default : PD3DDEVICE->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE); }  
}

inline void CD3DSkelMesh::SetMatrixPalette(uint32 MinBone,uint32 MaxBone,D3DMATRIX* pTransforms)
{
	for (uint32 i=MinBone;i<=MaxBone;++i) {
		if (m_bReIndexedBones) {
			g_RenderStateMgr.SetTransform(D3DTS_WORLDMATRIX(i),&pTransforms[m_pReIndexedBoneList[i]]); }
		else {
			g_RenderStateMgr.SetTransform(D3DTS_WORLDMATRIX(i),&pTransforms[i]); } }

	switch (m_iMaxBonesPerVert) {
		case 2  : PD3DDEVICE->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_1WEIGHTS); break;
		case 3  : PD3DDEVICE->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_2WEIGHTS); break;
		case 4  : PD3DDEVICE->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_3WEIGHTS); break;
		default : PD3DDEVICE->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE); }

	PD3DDEVICE->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE,true);
}

// THIS Function should be removed when we go to the full render object implementation - it's 
//	temporary on the path to full render objects. The D3D pipe render model path call this guy to do 
//	the transform and lighting stuff.
void CD3DSkelMesh::Render(D3DMATRIX* pD3DTransforms, CD3DRenderStyle* pRenderStyle, vector<LPDIRECT3DTEXTURE8>& TextureList)
{
	PD3DDEVICE->SetRenderState(D3DRS_FOGTABLEMODE,  D3DFOG_NONE);
	PD3DDEVICE->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);	// DX8 has bug with table fog with blended meshes...

	bool bSoftwareProcessing = false;
	switch (m_eRenderMethod) {										// Check if we need to do software processing...
	case eD3DRenderMatrixPalettes	: if (m_iMaxBonesPerVert*2 > g_Device.GetDeviceCaps()->MaxVertexBlendMatrixIndex) bSoftwareProcessing = true; break; } // Note: I am multiplying by two because the spec sais, if you're doing normals as well, it's half of the cap sais...
	PD3DDEVICE->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING,bSoftwareProcessing);

	m_VBController.SetStreamSources();

	for (uint32 iRenderPass = 0; iRenderPass < pRenderStyle->GetRenderPassCount(); ++iRenderPass) {
		HD3DVERTEXSHADER VertexShader = pRenderStyle->GetVertexShader(iRenderPass,m_iMaxBonesPerVert-1); RSD3DRenderPass D3DRenderPass;
		if (VertexShader) {																		// Using a custom vertex shader...
			if (FAILED(g_RenderStateMgr.SetVertexShader(VertexShader)))							{ return; }
			if (!pRenderStyle->GetRenderPass_D3DOptions(iRenderPass,&D3DRenderPass))			{ return; } }
		else {																					// Using the standand pipe...
			if (!m_VBController.getVertexFormat(0))	return;										// This is a non fixed function pipe VB - bail out...
			if (FAILED(g_RenderStateMgr.SetVertexShader(m_VBController.getVertexFormat(0))))	{ return; } }

		g_RenderStateMgr.SetRenderStyleStates(pRenderStyle,iRenderPass,TextureList);			// Set your render states with the render state mgr...

		switch (m_eRenderMethod) {
		case eD3DRenderMatrixPalettes :
			SetMatrixPalette(m_iMinBone,m_iMaxBone,pD3DTransforms);
			if (VertexShader) { g_RenderStateMgr.SetVertexShaderConstants(pRenderStyle, &D3DRenderPass,m_iMaxBone+1); }
			m_VBController.Render(0,0,m_iVertCount,m_iPolyCount);
			PD3DDEVICE->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE,false);
			break; } }

	D3DXMATRIX mIdentity; D3DXMatrixIdentity(&mIdentity);			// Reset our World Matrix...
	g_RenderStateMgr.SetTransform(D3DTS_WORLDMATRIX(0),&mIdentity);
	PD3DDEVICE->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE);
	PD3DDEVICE->SetRenderState(D3DRS_FOGTABLEMODE,  D3DFOG_LINEAR);
	PD3DDEVICE->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	if ((g_Device.GetDeviceCaps()->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && bSoftwareProcessing) 
		PD3DDEVICE->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING,true);
}

