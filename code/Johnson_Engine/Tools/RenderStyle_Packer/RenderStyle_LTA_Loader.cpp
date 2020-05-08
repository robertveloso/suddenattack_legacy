// RenderStyle_LTA.cpp
// 

#include <assert.h>
#include <d3dx8.h>
#include <ltinteger.h>

#include "d3d_renderstyle.h"
#include "ltamgr.h"
#include "fstream"
#include "Utilities.h"

// ------------------------------------------------------------------------
// LoadRenderStyle( CRenderStyle , CLTANode )
// converts lta rs into in-core rs
// ------------------------------------------------------------------------
bool LoadRenderStyle( CRenderStyle* pRenderStyle, CLTANode *RootNode )
					 
{
	// transfer from lta nodes
		CLTANode* pRenderStyleP  = RootNode;
		const char* szRenderStyleName = NULL;

		if (pRenderStyleP->GetElement(1)) szRenderStyleName = pRenderStyleP->GetElement(1)->GetValue();
	const char* szParam1; //const char* szParam2; const char* szParam3; const char* szParam4;

	// Lighting Material...
	CLTANode* pLightMaterial = CLTAUtil::ShallowFindList(pRenderStyleP,"lightmaterial");
	if (pLightMaterial) {
		LightingMaterial lightMaterial; 
		CLTANode* pAmbient = CLTAUtil::ShallowFindList(pLightMaterial,"ambient");
		if (pAmbient) {
			if (pAmbient->GetElement(1) && (szParam1 = pAmbient->GetElement(1)->GetValue()))		{ lightMaterial.Ambient.r = (float)atof(szParam1); }
			if (pAmbient->GetElement(2) && (szParam1 = pAmbient->GetElement(2)->GetValue()))		{ lightMaterial.Ambient.g = (float)atof(szParam1); }
			if (pAmbient->GetElement(3) && (szParam1 = pAmbient->GetElement(3)->GetValue()))		{ lightMaterial.Ambient.b = (float)atof(szParam1); }
			if (pAmbient->GetElement(4) && (szParam1 = pAmbient->GetElement(4)->GetValue()))		{ lightMaterial.Ambient.a = (float)atof(szParam1); } }
		CLTANode* pDiffuse = CLTAUtil::ShallowFindList(pLightMaterial,"diffuse");
		if (pDiffuse) {
			if (pDiffuse->GetElement(1) && (szParam1 = pDiffuse->GetElement(1)->GetValue()))		{ lightMaterial.Diffuse.r = (float)atof(szParam1); }
			if (pDiffuse->GetElement(2) && (szParam1 = pDiffuse->GetElement(2)->GetValue()))		{ lightMaterial.Diffuse.g = (float)atof(szParam1); }
			if (pDiffuse->GetElement(3) && (szParam1 = pDiffuse->GetElement(3)->GetValue()))		{ lightMaterial.Diffuse.b = (float)atof(szParam1); }
			if (pDiffuse->GetElement(4) && (szParam1 = pDiffuse->GetElement(4)->GetValue()))		{ lightMaterial.Diffuse.a = (float)atof(szParam1); } }
		CLTANode* pSpecular = CLTAUtil::ShallowFindList(pLightMaterial,"specular");
		if (pSpecular) {
			if (pSpecular->GetElement(1) && (szParam1 = pSpecular->GetElement(1)->GetValue()))	{ lightMaterial.Specular.r = (float)atof(szParam1); }
			if (pSpecular->GetElement(2) && (szParam1 = pSpecular->GetElement(2)->GetValue()))	{ lightMaterial.Specular.g = (float)atof(szParam1); }
			if (pSpecular->GetElement(3) && (szParam1 = pSpecular->GetElement(3)->GetValue()))	{ lightMaterial.Specular.b = (float)atof(szParam1); }
			if (pSpecular->GetElement(4) && (szParam1 = pSpecular->GetElement(4)->GetValue()))	{ lightMaterial.Specular.a = (float)atof(szParam1); } }
		CLTANode* pEmissive = CLTAUtil::ShallowFindList(pLightMaterial,"emissive");
		if (pEmissive) {
			if (pEmissive->GetElement(1) && (szParam1 = pEmissive->GetElement(1)->GetValue()))	{ lightMaterial.Emissive.r = (float)atof(szParam1); }
			if (pEmissive->GetElement(2) && (szParam1 = pEmissive->GetElement(2)->GetValue()))	{ lightMaterial.Emissive.g = (float)atof(szParam1); }
			if (pEmissive->GetElement(3) && (szParam1 = pEmissive->GetElement(3)->GetValue()))	{ lightMaterial.Emissive.b = (float)atof(szParam1); }
			if (pEmissive->GetElement(4) && (szParam1 = pEmissive->GetElement(4)->GetValue()))	{ lightMaterial.Emissive.a = (float)atof(szParam1); } }
		CLTANode* pSpecPower = CLTAUtil::ShallowFindList(pLightMaterial,"specularpower");
		if (pSpecPower) {
			if (pSpecPower->GetElement(1) && (szParam1 = pSpecPower->GetElement(1)->GetValue()))	{ lightMaterial.SpecularPower = (float)atof(szParam1); } }
		pRenderStyle->SetLightingMaterial(lightMaterial); }

	// Render Passes...
	while (pRenderStyle->GetRenderPassCount()) { pRenderStyle->RemoveRenderPass(0); }
	vector<CLTANode*> RenderRenderPasses;
	CLTAUtil::FindAll(RenderRenderPasses,pRenderStyleP,"renderpass");
	for (uint32 i = 0; i < RenderRenderPasses.size(); ++i) {
		RenderPassOp RenderPass; 
		CLTANode* pTestMode = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"testmode");
		if (pTestMode && pTestMode->GetElement(1) && (szParam1 = pTestMode->GetElement(1)->GetValue())) {
			if (stricmp(szParam1,"RENDERSTYLE_NOALPHATEST")==0)								{ RenderPass.AlphaTestMode = RENDERSTYLE_NOALPHATEST; }
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_LESS")==0)						{ RenderPass.AlphaTestMode = RENDERSTYLE_ALPHATEST_LESS; }
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_LESSEQUAL")==0)				{ RenderPass.AlphaTestMode = RENDERSTYLE_ALPHATEST_LESSEQUAL; } 
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_GREATER")==0)					{ RenderPass.AlphaTestMode = RENDERSTYLE_ALPHATEST_GREATER; } 
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_GREATEREQUAL")==0)				{ RenderPass.AlphaTestMode = RENDERSTYLE_ALPHATEST_GREATEREQUAL; } 
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_EQUAL")==0)					{ RenderPass.AlphaTestMode = RENDERSTYLE_ALPHATEST_EQUAL; } 
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_NOTEQUAL")==0)					{ RenderPass.AlphaTestMode = RENDERSTYLE_ALPHATEST_NOTEQUAL; } 
			else { RenderPass.AlphaTestMode = RENDERSTYLE_NOALPHATEST; OutputMsg("Warning: Unknown param %s",szParam1); } }
		else { RenderPass.AlphaTestMode = RENDERSTYLE_NOALPHATEST; }

		CLTANode* pZTestMode = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"ztestmode");
		if (pZTestMode && pZTestMode->GetElement(1) && (szParam1 = pZTestMode->GetElement(1)->GetValue())) {
			if (stricmp(szParam1,"RENDERSTYLE_NOALPHATEST")==0)								{ RenderPass.ZBufferTestMode = RENDERSTYLE_NOALPHATEST; }
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_LESS")==0)						{ RenderPass.ZBufferTestMode = RENDERSTYLE_ALPHATEST_LESS; }
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_LESSEQUAL")==0)				{ RenderPass.ZBufferTestMode = RENDERSTYLE_ALPHATEST_LESSEQUAL; } 
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_GREATER")==0)					{ RenderPass.ZBufferTestMode = RENDERSTYLE_ALPHATEST_GREATER; } 
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_GREATEREQUAL")==0)				{ RenderPass.ZBufferTestMode = RENDERSTYLE_ALPHATEST_GREATEREQUAL; } 
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_EQUAL")==0)					{ RenderPass.ZBufferTestMode = RENDERSTYLE_ALPHATEST_EQUAL; } 
			else if (stricmp(szParam1,"RENDERSTYLE_ALPHATEST_NOTEQUAL")==0)					{ RenderPass.ZBufferTestMode = RENDERSTYLE_ALPHATEST_NOTEQUAL; } 
			else { RenderPass.ZBufferTestMode = RENDERSTYLE_ALPHATEST_LESSEQUAL; OutputMsg("Warning: Unknown param %s",szParam1); } }
		else { RenderPass.ZBufferTestMode = RENDERSTYLE_ALPHATEST_LESSEQUAL; }

		CLTANode* pFillMode = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"fillmode");
		if (pFillMode && pFillMode->GetElement(1) && (szParam1 = pFillMode->GetElement(1)->GetValue())) {
			if (stricmp(szParam1,"RENDERSTYLE_WIRE")==0)									{ RenderPass.FillMode = RENDERSTYLE_WIRE; }
			else if (stricmp(szParam1,"RENDERSTYLE_FILL")==0)								{ RenderPass.FillMode = RENDERSTYLE_FILL; }
			else { RenderPass.FillMode = RENDERSTYLE_FILL; OutputMsg("Warning: Unknown param %s",szParam1); } } 
		else { RenderPass.FillMode = RENDERSTYLE_FILL; }

		CLTANode* pBlendOp = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"blendop");
		if (pBlendOp && pBlendOp->GetElement(1) && (szParam1 = pBlendOp->GetElement(1)->GetValue())) {
			if (stricmp(szParam1,"RENDERSTYLE_NOBLEND")==0)									{ RenderPass.BlendMode = RENDERSTYLE_NOBLEND; }
			else if (stricmp(szParam1,"RENDERSTYLE_BLEND_ADD")==0)							{ RenderPass.BlendMode = RENDERSTYLE_BLEND_ADD; }
			else if (stricmp(szParam1,"RENDERSTYLE_BLEND_SATURATE")==0)						{ RenderPass.BlendMode = RENDERSTYLE_BLEND_SATURATE; }
			else if (stricmp(szParam1,"RENDERSTYLE_BLEND_MOD_SRCALPHA")==0)					{ RenderPass.BlendMode = RENDERSTYLE_BLEND_MOD_SRCALPHA; }
			else if (stricmp(szParam1,"RENDERSTYLE_BLEND_MOD_SRCCOLOR")==0)					{ RenderPass.BlendMode = RENDERSTYLE_BLEND_MOD_SRCCOLOR; }
			else if (stricmp(szParam1,"RENDERSTYLE_BLEND_MOD_DSTCOLOR")==0)					{ RenderPass.BlendMode = RENDERSTYLE_BLEND_MOD_DSTCOLOR; }
			else if (stricmp(szParam1,"RENDERSTYLE_BLEND_MUL_SRCCOL_DSTCOL")==0)			{ RenderPass.BlendMode = RENDERSTYLE_BLEND_MUL_SRCCOL_DSTCOL; }
			else if (stricmp(szParam1,"RENDERSTYLE_BLEND_MUL_SRCCOL_ONE")==0)				{ RenderPass.BlendMode = RENDERSTYLE_BLEND_MUL_SRCCOL_ONE; }
			else if (stricmp(szParam1,"RENDERSTYLE_BLEND_MUL_SRCALPHA_ZERO")==0)			{ RenderPass.BlendMode = RENDERSTYLE_BLEND_MUL_SRCALPHA_ZERO; }
			else if (stricmp(szParam1,"RENDERSTYLE_BLEND_MUL_SRCALPHA_ONE")==0)				{ RenderPass.BlendMode = RENDERSTYLE_BLEND_MUL_SRCALPHA_ONE; }
			else if (stricmp(szParam1,"RENDERSTYLE_BLEND_MUL_DSTCOL_ZERO")==0)				{ RenderPass.BlendMode = RENDERSTYLE_BLEND_MUL_DSTCOL_ZERO; }
			else OutputMsg("Warning: Unknown param %s",szParam1); }

		CLTANode* pZBufferMode = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"zbuffermode");
		if (pZBufferMode && pZBufferMode->GetElement(1) && (szParam1 = pZBufferMode->GetElement(1)->GetValue())) {
			if (stricmp(szParam1,"RENDERSTYLE_ZRW")==0)										{ RenderPass.ZBufferMode = RENDERSTYLE_ZRW; }
			else if (stricmp(szParam1,"RENDERSTYLE_ZRO")==0)								{ RenderPass.ZBufferMode = RENDERSTYLE_ZRO; }
			else if (stricmp(szParam1,"RENDERSTYLE_NOZ")==0)								{ RenderPass.ZBufferMode = RENDERSTYLE_NOZ; } 
			else { RenderPass.ZBufferMode = RENDERSTYLE_ZRW; OutputMsg("Warning: Unknown param %s",szParam1); } }
		else { RenderPass.ZBufferMode = RENDERSTYLE_ZRW; }

		CLTANode* pCullMode = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"cullmode");
		if (pCullMode && pCullMode->GetElement(1) && (szParam1 = pCullMode->GetElement(1)->GetValue())) {
			if (stricmp(szParam1,"RENDERSTYLE_CULL_NONE")==0)								{ RenderPass.CullMode = RENDERSTYLE_CULL_NONE; }
			else if (stricmp(szParam1,"RENDERSTYLE_CULL_CCW")==0)							{ RenderPass.CullMode = RENDERSTYLE_CULL_CCW; }
			else if (stricmp(szParam1,"RENDERSTYLE_CULL_CW")==0)							{ RenderPass.CullMode = RENDERSTYLE_CULL_CW; } 
			else { RenderPass.CullMode = RENDERSTYLE_CULL_CCW; OutputMsg("Warning: Unknown param %s",szParam1); } } 
		else { RenderPass.CullMode = RENDERSTYLE_CULL_CCW; } 

		CLTANode* pTextureFactor = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"texturefactor");
		if (pTextureFactor && pTextureFactor->GetElement(1) && (szParam1 = pTextureFactor->GetElement(1)->GetValue())) {
			RenderPass.TextureFactor = atoi(szParam1); }
		else { RenderPass.TextureFactor = 0x80808080; }

		CLTANode* pAlphaRef = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"alpharef");
		if (pAlphaRef && pAlphaRef->GetElement(1) && (szParam1 = pAlphaRef->GetElement(1)->GetValue())) {
			RenderPass.AlphaRef = atoi(szParam1); }
		else { RenderPass.AlphaRef = 128; }

		CLTANode* pDynamicLight = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"dynamiclight");
		if (pDynamicLight && pDynamicLight->GetElement(1) && (szParam1 = pDynamicLight->GetElement(1)->GetValue())) {
			if (stricmp(szParam1,"1")==0)													{ RenderPass.DynamicLight = true; }
			else if (stricmp(szParam1,"0")==0)												{ RenderPass.DynamicLight = false; }
			else { RenderPass.DynamicLight = true; OutputMsg("Warning: Unknown param %s",szParam1); } }
		else { RenderPass.DynamicLight = true; }

		CLTANode* pUseBumpEnvMap = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"usebumpenvmap");
		if (pUseBumpEnvMap && pUseBumpEnvMap->GetElement(1) && (szParam1 = pUseBumpEnvMap->GetElement(1)->GetValue())) {
			if (stricmp(szParam1,"1")==0)													{ RenderPass.bUseBumpEnvMap = true; }
			else if (stricmp(szParam1,"0")==0)												{ RenderPass.bUseBumpEnvMap = false; }
			else { RenderPass.bUseBumpEnvMap = false; OutputMsg("Warning: Unknown param %s",szParam1); } }
		else { RenderPass.bUseBumpEnvMap = false; }

		CLTANode* pBumpEnvStage = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"bumpenvmapstage");
		if (pBumpEnvStage && pBumpEnvStage->GetElement(1) && (szParam1 = pBumpEnvStage->GetElement(1)->GetValue())) {
			RenderPass.BumpEnvMapStage = atoi(szParam1); }
		else { RenderPass.BumpEnvMapStage = 1; }

		CLTANode* pBumpEnvScale = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"bumpenvmapscale");
		if (pBumpEnvScale && pBumpEnvScale->GetElement(1) && (szParam1 = pBumpEnvScale->GetElement(1)->GetValue())) {
			RenderPass.fBumpEnvMap_Scale = (float)atof(szParam1); }
		else { RenderPass.fBumpEnvMap_Scale = 1.0f; }

		CLTANode* pBumpEnvOffset = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"bumpenvmapoffset");
		if (pBumpEnvOffset && pBumpEnvOffset->GetElement(1) && (szParam1 = pBumpEnvOffset->GetElement(1)->GetValue())) {
			RenderPass.fBumpEnvMap_Offset = (float)atof(szParam1); }
		else { RenderPass.fBumpEnvMap_Offset = 0.0f; }

		vector<CLTANode*> TextureStages;
		CLTAUtil::FindAll(TextureStages,RenderRenderPasses[i],"texturestageops"); 
		for (uint32 j = 0; j < TextureStages.size(); ++j) {
			assert(j < 4); if (j >= 4) continue;
			CLTANode* pTextureParam = CLTAUtil::ShallowFindList(TextureStages[j],"textureparam");
			if (pTextureParam && pTextureParam->GetElement(1) && (szParam1 = pTextureParam->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_NOTEXTURE")==0)							{ RenderPass.TextureStages[j].TextureParam = RENDERSTYLE_NOTEXTURE; }
				else if (stricmp(szParam1,"RENDERSTYLE_USE_TEXTURE1")==0)					{ RenderPass.TextureStages[j].TextureParam = RENDERSTYLE_USE_TEXTURE1; }
				else if (stricmp(szParam1,"RENDERSTYLE_USE_TEXTURE2")==0)					{ RenderPass.TextureStages[j].TextureParam = RENDERSTYLE_USE_TEXTURE2; }
				else if (stricmp(szParam1,"RENDERSTYLE_USE_TEXTURE3")==0)					{ RenderPass.TextureStages[j].TextureParam = RENDERSTYLE_USE_TEXTURE3; }
				else if (stricmp(szParam1,"RENDERSTYLE_USE_TEXTURE4")==0)					{ RenderPass.TextureStages[j].TextureParam = RENDERSTYLE_USE_TEXTURE4; }
				else OutputMsg("Warning: Unknown param %s",szParam1); }
	
			CLTANode* pColorOp = CLTAUtil::ShallowFindList(TextureStages[j],"colorop");
			if (pColorOp && pColorOp->GetElement(1) && (szParam1 = pColorOp->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_COLOROP_DISABLE")==0)						{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_DISABLE; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_SELECTARG1")==0)				{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_SELECTARG1; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_SELECTARG2")==0)				{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_SELECTARG2; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_MODULATE")==0)				{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_MODULATE; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_MODULATE2X")==0)				{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_MODULATE2X; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_MODULATEALPHA")==0)			{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_MODULATEALPHA; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_MODULATETFACTOR")==0)		{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_MODULATETFACTOR; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_ADD")==0)					{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_ADD; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_DOTPRODUCT3")==0)			{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_DOTPRODUCT3; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_BUMPENVMAP")==0)				{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_BUMPENVMAP; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_BUMPENVMAPLUM")==0)			{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_BUMPENVMAPLUM; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_DECAL")==0)					{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_DECAL; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_HIGHLIGHT")==0)				{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_HIGHLIGHT; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_HIGHLIGHT2")==0)				{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_HIGHLIGHT2; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_ADDSIGNED")==0)				{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_ADDSIGNED; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_ADDSIGNED2X")==0)			{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_ADDSIGNED2X; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_SUBTRACT")==0)				{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_SUBTRACT; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_ADDMODALPHA")==0)			{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_ADDMODALPHA; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLOROP_ADDMODINVALPHA")==0)			{ RenderPass.TextureStages[j].ColorOp = RENDERSTYLE_COLOROP_ADDMODINVALPHA; }
				else OutputMsg("Warning: Unknown param %s",szParam1); }

			CLTANode* pColorArg1 = CLTAUtil::ShallowFindList(TextureStages[j],"colorarg1");
			if (pColorArg1 && pColorArg1->GetElement(1) && (szParam1 = pColorArg1->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_COLORARG_CURRENT")==0)					{ RenderPass.TextureStages[j].ColorArg1 = RENDERSTYLE_COLORARG_CURRENT; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLORARG_DIFFUSE")==0)				{ RenderPass.TextureStages[j].ColorArg1 = RENDERSTYLE_COLORARG_DIFFUSE; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLORARG_TEXTURE")==0)				{ RenderPass.TextureStages[j].ColorArg1 = RENDERSTYLE_COLORARG_TEXTURE; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLORARG_TFACTOR")==0)				{ RenderPass.TextureStages[j].ColorArg1 = RENDERSTYLE_COLORARG_TFACTOR; }
				else OutputMsg("Warning: Unknown param %s",szParam1); }

			CLTANode* pColorArg2 = CLTAUtil::ShallowFindList(TextureStages[j],"colorarg2");
			if (pColorArg2 && pColorArg2->GetElement(1) && (szParam1 = pColorArg2->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_COLORARG_CURRENT")==0)					{ RenderPass.TextureStages[j].ColorArg2 = RENDERSTYLE_COLORARG_CURRENT; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLORARG_DIFFUSE")==0)				{ RenderPass.TextureStages[j].ColorArg2 = RENDERSTYLE_COLORARG_DIFFUSE; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLORARG_TEXTURE")==0)				{ RenderPass.TextureStages[j].ColorArg2 = RENDERSTYLE_COLORARG_TEXTURE; }
				else if (stricmp(szParam1,"RENDERSTYLE_COLORARG_TFACTOR")==0)				{ RenderPass.TextureStages[j].ColorArg2 = RENDERSTYLE_COLORARG_TFACTOR; }
				else OutputMsg("Warning: Unknown param %s",szParam1); }

			CLTANode* pAlphaOp = CLTAUtil::ShallowFindList(TextureStages[j],"alphaop");
			if (pAlphaOp && pAlphaOp->GetElement(1) && (szParam1 = pAlphaOp->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_ALPHAOP_DISABLE")==0)						{ RenderPass.TextureStages[j].AlphaOp = RENDERSTYLE_ALPHAOP_DISABLE; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAOP_SELECTARG1")==0)				{ RenderPass.TextureStages[j].AlphaOp = RENDERSTYLE_ALPHAOP_SELECTARG1; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAOP_SELECTARG2")==0)				{ RenderPass.TextureStages[j].AlphaOp = RENDERSTYLE_ALPHAOP_SELECTARG2; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAOP_MODULATE")==0)				{ RenderPass.TextureStages[j].AlphaOp = RENDERSTYLE_ALPHAOP_MODULATE; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAOP_MODULATEALPHA")==0)			{ RenderPass.TextureStages[j].AlphaOp = RENDERSTYLE_ALPHAOP_MODULATEALPHA; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAOP_MODULATETFACTOR")==0)		{ RenderPass.TextureStages[j].AlphaOp = RENDERSTYLE_ALPHAOP_MODULATETFACTOR; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAOP_ADD")==0)					{ RenderPass.TextureStages[j].AlphaOp = RENDERSTYLE_ALPHAOP_ADD; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAOP_ADDSIGNED")==0)				{ RenderPass.TextureStages[j].AlphaOp = RENDERSTYLE_ALPHAOP_ADDSIGNED; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAOP_ADDSIGNED2X")==0)			{ RenderPass.TextureStages[j].AlphaOp = RENDERSTYLE_ALPHAOP_ADDSIGNED2X; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAOP_SUBTRACT")==0)				{ RenderPass.TextureStages[j].AlphaOp = RENDERSTYLE_ALPHAOP_SUBTRACT; }
				else OutputMsg("Warning: Unknown param %s",szParam1); }

			CLTANode* pAlphaArg1 = CLTAUtil::ShallowFindList(TextureStages[j],"alphaarg1");
			if (pAlphaArg1 && pAlphaArg1->GetElement(1) && (szParam1 = pAlphaArg1->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_ALPHAARG_CURRENT")==0)					{ RenderPass.TextureStages[j].AlphaArg1 = RENDERSTYLE_ALPHAARG_CURRENT; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAARG_DIFFUSE")==0)				{ RenderPass.TextureStages[j].AlphaArg1 = RENDERSTYLE_ALPHAARG_DIFFUSE; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAARG_TEXTURE")==0)				{ RenderPass.TextureStages[j].AlphaArg1 = RENDERSTYLE_ALPHAARG_TEXTURE; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAARG_TFACTOR")==0)				{ RenderPass.TextureStages[j].AlphaArg1 = RENDERSTYLE_ALPHAARG_TFACTOR; }
				else { RenderPass.TextureStages[j].AlphaArg1 = RENDERSTYLE_ALPHAARG_CURRENT; OutputMsg("Warning: Unknown param %s",szParam1); } }

			CLTANode* pAlphaArg2 = CLTAUtil::ShallowFindList(TextureStages[j],"alphaarg2");
			if (pAlphaArg2 && pAlphaArg2->GetElement(1) && (szParam1 = pAlphaArg2->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_ALPHAARG_CURRENT")==0)					{ RenderPass.TextureStages[j].AlphaArg2 = RENDERSTYLE_ALPHAARG_CURRENT; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAARG_DIFFUSE")==0)				{ RenderPass.TextureStages[j].AlphaArg2 = RENDERSTYLE_ALPHAARG_DIFFUSE; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAARG_TEXTURE")==0)				{ RenderPass.TextureStages[j].AlphaArg2 = RENDERSTYLE_ALPHAARG_TEXTURE; }
				else if (stricmp(szParam1,"RENDERSTYLE_ALPHAARG_TFACTOR")==0)				{ RenderPass.TextureStages[j].AlphaArg2 = RENDERSTYLE_ALPHAARG_TFACTOR; }
				else { RenderPass.TextureStages[j].AlphaArg2 = RENDERSTYLE_ALPHAARG_CURRENT; OutputMsg("Warning: Unknown param %s",szParam1); } }

			CLTANode* pUVSource = CLTAUtil::ShallowFindList(TextureStages[j],"uvsource");
			if (pUVSource && pUVSource->GetElement(1) && (szParam1 = pUVSource->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_UVFROM_MODELDATA_UVSET1")==0)				{ RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_MODELDATA_UVSET1; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVFROM_MODELDATA_UVSET2")==0)		{ RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_MODELDATA_UVSET2; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVFROM_MODELDATA_UVSET3")==0)		{ RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_MODELDATA_UVSET3; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVFROM_MODELDATA_UVSET4")==0)		{ RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_MODELDATA_UVSET4; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVFROM_CAMERASPACENORMAL")==0)		{ RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_CAMERASPACENORMAL; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVFROM_CAMERASPACEPOSITION")==0)		{ RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_CAMERASPACEPOSITION; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVFROM_CAMERASPACEREFLTVECT")==0)	{ RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_CAMERASPACEREFLTVECT; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVFROM_WORLDSPACENORMAL")==0)		{ RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_WORLDSPACENORMAL; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVFROM_WORLDSPACEPOSITION")==0)		{ RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_WORLDSPACEPOSITION; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVFROM_WORLDSPACEREFLTVECT")==0)		{ RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_WORLDSPACEREFLTVECT; }
				else { RenderPass.TextureStages[j].UVSource = RENDERSTYLE_UVFROM_MODELDATA_UVSET1; OutputMsg("Warning: Unknown param %s",szParam1); } } 

			CLTANode* pUAddress = CLTAUtil::ShallowFindList(TextureStages[j],"uaddress");
			if (pUAddress && pUAddress->GetElement(1) && (szParam1 = pUAddress->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_UVADDR_WRAP")==0)							{ RenderPass.TextureStages[j].UAddress = RENDERSTYLE_UVADDR_WRAP; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVADDR_CLAMP")==0)					{ RenderPass.TextureStages[j].UAddress = RENDERSTYLE_UVADDR_CLAMP; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVADDR_MIRROR")==0)					{ RenderPass.TextureStages[j].UAddress = RENDERSTYLE_UVADDR_MIRROR; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVADDR_MIRRORONCE")==0)				{ RenderPass.TextureStages[j].UAddress = RENDERSTYLE_UVADDR_MIRRORONCE; }
				else { RenderPass.TextureStages[j].UAddress = RENDERSTYLE_UVADDR_WRAP; OutputMsg("Warning: Unknown param %s",szParam1); } }
			else { RenderPass.TextureStages[j].UAddress = RENDERSTYLE_UVADDR_WRAP; }

			CLTANode* pVAddress = CLTAUtil::ShallowFindList(TextureStages[j],"vaddress");
			if (pVAddress && pVAddress->GetElement(1) && (szParam1 = pVAddress->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_UVADDR_WRAP")==0)							{ RenderPass.TextureStages[j].VAddress = RENDERSTYLE_UVADDR_WRAP; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVADDR_CLAMP")==0)					{ RenderPass.TextureStages[j].VAddress = RENDERSTYLE_UVADDR_CLAMP; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVADDR_MIRROR")==0)					{ RenderPass.TextureStages[j].VAddress = RENDERSTYLE_UVADDR_MIRROR; }
				else if (stricmp(szParam1,"RENDERSTYLE_UVADDR_MIRRORONCE")==0)				{ RenderPass.TextureStages[j].VAddress = RENDERSTYLE_UVADDR_MIRRORONCE; }
				else { RenderPass.TextureStages[j].VAddress = RENDERSTYLE_UVADDR_WRAP; OutputMsg("Warning: Unknown param %s",szParam1); } }
			else { RenderPass.TextureStages[j].VAddress = RENDERSTYLE_UVADDR_WRAP; }
			CLTANode* pUVTransform_Enable = CLTAUtil::ShallowFindList(TextureStages[j],"uvtransform_enable");
			if (pUVTransform_Enable && pUVTransform_Enable->GetElement(1) && (szParam1 = pUVTransform_Enable->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"1")==0)												{ RenderPass.TextureStages[j].UVTransform_Enable = true; }
				else if (stricmp(szParam1,"0")==0)											{ RenderPass.TextureStages[j].UVTransform_Enable = false; }
				else { RenderPass.TextureStages[j].UVTransform_Enable = false; OutputMsg("Warning: Unknown param %s",szParam1); } } 
			
			CLTANode* pTexFilter = CLTAUtil::ShallowFindList(TextureStages[j],"texfilter");
			if (pTexFilter && pTexFilter->GetElement(1) && (szParam1 = pTexFilter->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"RENDERSTYLE_TEXFILTER_POINT")==0)						{ RenderPass.TextureStages[j].TexFilter = RENDERSTYLE_TEXFILTER_POINT; }
				else if (stricmp(szParam1,"RENDERSTYLE_TEXFILTER_BILINEAR")==0)				{ RenderPass.TextureStages[j].TexFilter = RENDERSTYLE_TEXFILTER_BILINEAR; }
				else if (stricmp(szParam1,"RENDERSTYLE_TEXFILTER_TRILINEAR")==0)			{ RenderPass.TextureStages[j].TexFilter = RENDERSTYLE_TEXFILTER_TRILINEAR; }
				else if (stricmp(szParam1,"RENDERSTYLE_TEXFILTER_ANISOTROPIC")==0)			{ RenderPass.TextureStages[j].TexFilter = RENDERSTYLE_TEXFILTER_ANISOTROPIC; }
				else if (stricmp(szParam1,"RENDERSTYLE_TEXFILTER_POINT_PTMIP")==0)			{ RenderPass.TextureStages[j].TexFilter = RENDERSTYLE_TEXFILTER_POINT_PTMIP; }
				else { RenderPass.TextureStages[j].TexFilter = RENDERSTYLE_TEXFILTER_TRILINEAR; OutputMsg("Warning: Unknown param %s",szParam1); } }
			else { RenderPass.TextureStages[j].TexFilter = RENDERSTYLE_TEXFILTER_TRILINEAR; }

			CLTANode* pTexCoordCount = CLTAUtil::ShallowFindList(TextureStages[j],"texcoordcount");
			if (pTexCoordCount && pTexCoordCount->GetElement(1) && (szParam1 = pTexCoordCount->GetElement(1)->GetValue())) 
			{
				RenderPass.TextureStages[j].TexCoordCount = atoi(szParam1);
				if(RenderPass.TextureStages[j].TexCoordCount < 1)
					RenderPass.TextureStages[j].TexCoordCount = 1;
				if(RenderPass.TextureStages[j].TexCoordCount > 4)
					RenderPass.TextureStages[j].TexCoordCount = 4;
			}
			else { RenderPass.TextureStages[j].TexCoordCount = 2; }

			CLTANode* pUVProject_Enable = CLTAUtil::ShallowFindList(TextureStages[j],"uvproject_enable");
			if (pUVProject_Enable && pUVProject_Enable->GetElement(1) && (szParam1 = pUVProject_Enable->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"1")==0)												{ RenderPass.TextureStages[j].ProjectTexCoord = true; }
				else if (stricmp(szParam1,"0")==0)											{ RenderPass.TextureStages[j].ProjectTexCoord = false; }
				else { RenderPass.TextureStages[j].ProjectTexCoord = false; OutputMsg("Warning: Unknown param %s",szParam1); } } 
			else { RenderPass.TextureStages[j].ProjectTexCoord = false; }

			CLTANode* pUVTransform_Matrix = CLTAUtil::ShallowFindList(TextureStages[j],"uvtransform_matrix");
			if (pUVTransform_Matrix) {
				if (pUVTransform_Matrix->GetElement(1)  && (szParam1 = pUVTransform_Matrix->GetElement(1)->GetValue()))  { RenderPass.TextureStages[j].UVTransform_Matrix[0]  = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(2)  && (szParam1 = pUVTransform_Matrix->GetElement(2)->GetValue()))  { RenderPass.TextureStages[j].UVTransform_Matrix[1]  = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(3)  && (szParam1 = pUVTransform_Matrix->GetElement(3)->GetValue()))  { RenderPass.TextureStages[j].UVTransform_Matrix[2]  = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(4)  && (szParam1 = pUVTransform_Matrix->GetElement(4)->GetValue()))  { RenderPass.TextureStages[j].UVTransform_Matrix[3]  = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(5)  && (szParam1 = pUVTransform_Matrix->GetElement(5)->GetValue()))  { RenderPass.TextureStages[j].UVTransform_Matrix[4]  = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(6)  && (szParam1 = pUVTransform_Matrix->GetElement(6)->GetValue()))  { RenderPass.TextureStages[j].UVTransform_Matrix[5]  = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(7)  && (szParam1 = pUVTransform_Matrix->GetElement(7)->GetValue()))  { RenderPass.TextureStages[j].UVTransform_Matrix[6]  = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(8)  && (szParam1 = pUVTransform_Matrix->GetElement(8)->GetValue()))  { RenderPass.TextureStages[j].UVTransform_Matrix[7]  = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(9)  && (szParam1 = pUVTransform_Matrix->GetElement(9)->GetValue()))  { RenderPass.TextureStages[j].UVTransform_Matrix[8]  = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(10) && (szParam1 = pUVTransform_Matrix->GetElement(10)->GetValue())) { RenderPass.TextureStages[j].UVTransform_Matrix[9]  = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(11) && (szParam1 = pUVTransform_Matrix->GetElement(11)->GetValue())) { RenderPass.TextureStages[j].UVTransform_Matrix[10] = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(12) && (szParam1 = pUVTransform_Matrix->GetElement(12)->GetValue())) { RenderPass.TextureStages[j].UVTransform_Matrix[11] = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(13) && (szParam1 = pUVTransform_Matrix->GetElement(13)->GetValue())) { RenderPass.TextureStages[j].UVTransform_Matrix[12] = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(14) && (szParam1 = pUVTransform_Matrix->GetElement(14)->GetValue())) { RenderPass.TextureStages[j].UVTransform_Matrix[13] = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(15) && (szParam1 = pUVTransform_Matrix->GetElement(15)->GetValue())) { RenderPass.TextureStages[j].UVTransform_Matrix[14] = (float)atof(szParam1); }
				if (pUVTransform_Matrix->GetElement(16) && (szParam1 = pUVTransform_Matrix->GetElement(16)->GetValue())) { RenderPass.TextureStages[j].UVTransform_Matrix[15] = (float)atof(szParam1); } } 
			else {
				for (uint32 r = 0; r < 16; ++r) if ((r-(r/4))%4==0) RenderPass.TextureStages[j].UVTransform_Matrix[r] = 1.0f; else RenderPass.TextureStages[j].UVTransform_Matrix[r] = 0.0f; } }
		pRenderStyle->AddRenderPass(RenderPass);

		// D3D Options...
		vector<CLTANode*> D3DRenderPassOptions;
		CLTANode* pD3DRenderPassOptions = CLTAUtil::ShallowFindList(RenderRenderPasses[i],"d3doptions"); 
		if (pD3DRenderPassOptions) {
			RSD3DRenderPass D3DOptions; 
			CLTANode* pUseVertexShader = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"usevertexshader");
			if (pUseVertexShader && pUseVertexShader->GetElement(1) && (szParam1 = pUseVertexShader->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"1")==0)													{ D3DOptions.bUseVertexShader = true; }
				else if (stricmp(szParam1,"0")==0)												{ D3DOptions.bUseVertexShader = false; }
				else OutputMsg("Warning: Unknown param %s",szParam1); }
			CLTANode* pVertexShaderFilename = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"vertexshaderfilename");
			if (pVertexShaderFilename && pVertexShaderFilename->GetElement(1) && (szParam1 = pVertexShaderFilename->GetElement(1)->GetValue())) {
				D3DOptions.VertexShaderFilename = szParam1; }

			CLTANode* pExpandForSkinning = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"expandskinning");
			if (pExpandForSkinning && pExpandForSkinning->GetElement(1) && (szParam1 = pExpandForSkinning->GetElement(1)->GetValue())) {
				if (stricmp(szParam1,"1")==0)													{ D3DOptions.bExpandForSkinning = true; }
				else if (stricmp(szParam1,"0")==0)												{ D3DOptions.bExpandForSkinning = false; }
				else { D3DOptions.bExpandForSkinning = false; OutputMsg("Warning: Unknown param %s",szParam1); } }
			else { D3DOptions.bExpandForSkinning = false; } 
			CLTANode* pConstVec_ConstReg1 = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"constvector_constreg1");
			if (pConstVec_ConstReg1) {
				if (pConstVec_ConstReg1->GetElement(1) && (szParam1 = pConstVec_ConstReg1->GetElement(1)->GetValue()))			{ D3DOptions.ConstVector_ConstReg1 = atoi(szParam1); } }
			CLTANode* pConstVec_Param1 = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"constvector_param1");
			if (pConstVec_Param1) {
				if (pConstVec_Param1->GetElement(1) && (szParam1 = pConstVec_Param1->GetElement(1)->GetValue()))					{ D3DOptions.ConstVector_Param1.x = (float)atof(szParam1); }
				if (pConstVec_Param1->GetElement(2) && (szParam1 = pConstVec_Param1->GetElement(2)->GetValue()))					{ D3DOptions.ConstVector_Param1.y = (float)atof(szParam1); }
				if (pConstVec_Param1->GetElement(3) && (szParam1 = pConstVec_Param1->GetElement(3)->GetValue()))					{ D3DOptions.ConstVector_Param1.z = (float)atof(szParam1); }
				if (pConstVec_Param1->GetElement(4) && (szParam1 = pConstVec_Param1->GetElement(4)->GetValue()))					{ D3DOptions.ConstVector_Param1.w = (float)atof(szParam1); } }
			CLTANode* pConstVec_ConstReg2 = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"constvector_constreg2");
			if (pConstVec_ConstReg2) {
				if (pConstVec_ConstReg2->GetElement(1) && (szParam1 = pConstVec_ConstReg2->GetElement(1)->GetValue()))			{ D3DOptions.ConstVector_ConstReg2 = atoi(szParam1); } }
			CLTANode* pConstVec_Param2 = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"constvector_param2");
			if (pConstVec_Param2) {
				if (pConstVec_Param2->GetElement(1) && (szParam1 = pConstVec_Param2->GetElement(1)->GetValue()))					{ D3DOptions.ConstVector_Param2.x = (float)atof(szParam1); }
				if (pConstVec_Param2->GetElement(2) && (szParam1 = pConstVec_Param2->GetElement(2)->GetValue()))					{ D3DOptions.ConstVector_Param2.y = (float)atof(szParam1); }
				if (pConstVec_Param2->GetElement(3) && (szParam1 = pConstVec_Param2->GetElement(3)->GetValue()))					{ D3DOptions.ConstVector_Param2.z = (float)atof(szParam1); }
				if (pConstVec_Param2->GetElement(4) && (szParam1 = pConstVec_Param2->GetElement(4)->GetValue()))					{ D3DOptions.ConstVector_Param2.w = (float)atof(szParam1); } }
			CLTANode* pConstVec_ConstReg3 = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"constvector_constreg3");
			if (pConstVec_ConstReg3) {
				if (pConstVec_ConstReg3->GetElement(1) && (szParam1 = pConstVec_ConstReg3->GetElement(1)->GetValue()))			{ D3DOptions.ConstVector_ConstReg3 = atoi(szParam1); } }
			CLTANode* pConstVec_Param3 = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"constvector_param3");
			if (pConstVec_Param3) {
				if (pConstVec_Param3->GetElement(1) && (szParam1 = pConstVec_Param3->GetElement(1)->GetValue()))					{ D3DOptions.ConstVector_Param3.x = (float)atof(szParam1); }
				if (pConstVec_Param3->GetElement(2) && (szParam1 = pConstVec_Param3->GetElement(2)->GetValue()))					{ D3DOptions.ConstVector_Param3.y = (float)atof(szParam1); }
				if (pConstVec_Param3->GetElement(3) && (szParam1 = pConstVec_Param3->GetElement(3)->GetValue()))					{ D3DOptions.ConstVector_Param3.z = (float)atof(szParam1); }
				if (pConstVec_Param3->GetElement(4) && (szParam1 = pConstVec_Param3->GetElement(4)->GetValue()))					{ D3DOptions.ConstVector_Param3.w = (float)atof(szParam1); } }
			CLTANode* pWorldViewTransform_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"worldviewtransform_constreg");
			if (pWorldViewTransform_ConstReg) {
				if (pWorldViewTransform_ConstReg->GetElement(1) && (szParam1 = pWorldViewTransform_ConstReg->GetElement(1)->GetValue()))			{ D3DOptions.WorldViewTransform_ConstReg = atoi(szParam1); } }
			else { D3DOptions.WorldViewTransform_ConstReg = -1; }
			CLTANode* pWorldViewTransform_Count = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"worldviewtransform_count");
			if (pWorldViewTransform_Count) {
				if (pWorldViewTransform_Count->GetElement(1) && (szParam1 = pWorldViewTransform_Count->GetElement(1)->GetValue()))				{ D3DOptions.WorldViewTransform_Count = atoi(szParam1); } }
			else { D3DOptions.WorldViewTransform_Count = 0; }
			CLTANode* pProjTransform_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"projtransform_constreg");
			if (pProjTransform_ConstReg) {
				if (pProjTransform_ConstReg->GetElement(1) && (szParam1 = pProjTransform_ConstReg->GetElement(1)->GetValue()))	{ D3DOptions.ProjTransform_ConstReg = atoi(szParam1); } }
			CLTANode* pWorldViewProjTransform_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"worldviewprojtransform_constreg");
			if (pWorldViewProjTransform_ConstReg) {
				if (pWorldViewProjTransform_ConstReg->GetElement(1) && (szParam1 = pWorldViewProjTransform_ConstReg->GetElement(1)->GetValue()))	{ D3DOptions.WorldViewProjTransform_ConstReg = atoi(szParam1); } }
			CLTANode* pViewProjTransform_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"viewprojtransform_constreg");
			if (pViewProjTransform_ConstReg) {
				if (pViewProjTransform_ConstReg->GetElement(1) && (szParam1 = pViewProjTransform_ConstReg->GetElement(1)->GetValue()))			{ D3DOptions.ViewProjTransform_ConstReg = atoi(szParam1); } }
			CLTANode* pCamPos_MSpc_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"campos_mspc_constreg");
			if (pCamPos_MSpc_ConstReg) {
				if (pCamPos_MSpc_ConstReg->GetElement(1) && (szParam1 = pCamPos_MSpc_ConstReg->GetElement(1)->GetValue()))		{ D3DOptions.CamPos_MSpc_ConstReg = atoi(szParam1); } }
			else { D3DOptions.CamPos_MSpc_ConstReg = -1; }
			CLTANode* pLight_Count = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"light_count");
			if (pLight_Count) {
				if (pLight_Count->GetElement(1) && (szParam1 = pLight_Count->GetElement(1)->GetValue()))			{ D3DOptions.Light_Count = atoi(szParam1); } }
			CLTANode* pLightPosition_MSpc_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"lightposition_mspc_constreg");
			if (pLightPosition_MSpc_ConstReg) {
				if (pLightPosition_MSpc_ConstReg->GetElement(1) && (szParam1 = pLightPosition_MSpc_ConstReg->GetElement(1)->GetValue()))	{ D3DOptions.LightPosition_MSpc_ConstReg = atoi(szParam1); } }
			else { D3DOptions.LightPosition_MSpc_ConstReg = -1; }
			CLTANode* pLightPosition_CSpc_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"lightposition_cspc_constreg");
			if (pLightPosition_CSpc_ConstReg) {
				if (pLightPosition_CSpc_ConstReg->GetElement(1) && (szParam1 = pLightPosition_CSpc_ConstReg->GetElement(1)->GetValue()))		{ D3DOptions.LightPosition_CSpc_ConstReg = atoi(szParam1); } }
			else { D3DOptions.LightPosition_CSpc_ConstReg = -1; }
			CLTANode* pLightColor_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"lightcolor_constreg");
			if (pLightColor_ConstReg) {
				if (pLightColor_ConstReg->GetElement(1) && (szParam1 = pLightColor_ConstReg->GetElement(1)->GetValue()))						{ D3DOptions.LightColor_ConstReg = atoi(szParam1); } }
			else { D3DOptions.LightColor_ConstReg = -1; }
			CLTANode* pLightAtt_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"lightatt_constreg");
			if (pLightAtt_ConstReg) {
				if (pLightAtt_ConstReg->GetElement(1) && (szParam1 = pLightAtt_ConstReg->GetElement(1)->GetValue()))							{ D3DOptions.LightAtt_ConstReg = atoi(szParam1); } }
			else { D3DOptions.LightAtt_ConstReg = -1; }
			CLTANode* pMaterial_AmbDifEm_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"material_ambdifem_constreg");
			if (pMaterial_AmbDifEm_ConstReg) {
				if (pMaterial_AmbDifEm_ConstReg->GetElement(1) && (szParam1 = pMaterial_AmbDifEm_ConstReg->GetElement(1)->GetValue()))			{ D3DOptions.Material_AmbDifEm_ConstReg = atoi(szParam1); } }
			CLTANode* pMaterial_Specular_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"material_specular_constreg");
			if (pMaterial_Specular_ConstReg) {
				if (pMaterial_Specular_ConstReg->GetElement(1) && (szParam1 = pMaterial_Specular_ConstReg->GetElement(1)->GetValue()))			{ D3DOptions.Material_Specular_ConstReg = atoi(szParam1); } }
			CLTANode* pAmbientLight_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"ambientlight_constreg");
			if (pAmbientLight_ConstReg) {
				if (pAmbientLight_ConstReg->GetElement(1) && (szParam1 = pAmbientLight_ConstReg->GetElement(1)->GetValue()))					{ D3DOptions.AmbientLight_ConstReg = atoi(szParam1); } }
			else { D3DOptions.AmbientLight_ConstReg = -1; }
			CLTANode* pPrevWorldViewTrans_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"prevworldviewtrans_constreg");
			if (pPrevWorldViewTrans_ConstReg) {
				if (pPrevWorldViewTrans_ConstReg->GetElement(1) && (szParam1 = pPrevWorldViewTrans_ConstReg->GetElement(1)->GetValue()))		{ D3DOptions.PrevWorldViewTrans_ConstReg = atoi(szParam1); } }
			else { D3DOptions.PrevWorldViewTrans_ConstReg = -1; }
			CLTANode* pPrevWorldViewTrans_Count = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"prevworldviewtrans_count");
			if (pPrevWorldViewTrans_Count) {
				if (pPrevWorldViewTrans_Count->GetElement(1) && (szParam1 = pPrevWorldViewTrans_Count->GetElement(1)->GetValue()))		{ D3DOptions.PrevWorldViewTrans_Count = atoi(szParam1); } }
			else { D3DOptions.PrevWorldViewTrans_Count = 0; }
//			CLTANode* pLast_ConstReg = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,"last_constreg");
//			if (pLast_ConstReg) {
//				if (pLast_ConstReg->GetElement(1) && (szParam1 = pLast_ConstReg->GetElement(1)->GetValue()))			{ D3DOptions.Last_ConstReg = atoi(szParam1); } }
//			else { D3DOptions.Last_ConstReg = -1; }
			for (uint32 z = 0; z < 4; ++z) {
				char szTmp[64]; sprintf(szTmp,"%s%d","declaration_stream_position",z);
				CLTANode* pDeclaration_Stream_Position = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,szTmp);
				if (pDeclaration_Stream_Position) {
					if (pDeclaration_Stream_Position->GetElement(1) && (szParam1 = pDeclaration_Stream_Position->GetElement(1)->GetValue()))		{ 
						if (stricmp(szParam1,"1")==0)		{ D3DOptions.bDeclaration_Stream_Position[z] = true; }
						else if (stricmp(szParam1,"0")==0)	{ D3DOptions.bDeclaration_Stream_Position[z] = false; }
						else OutputMsg("Warning: Unknown param %s",szParam1); } } 
				sprintf(szTmp,"%s%d","declaration_stream_normal",z);
				CLTANode* pDeclaration_Stream_Normal = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,szTmp);
				if (pDeclaration_Stream_Normal) {
					if (pDeclaration_Stream_Normal->GetElement(1) && (szParam1 = pDeclaration_Stream_Normal->GetElement(1)->GetValue()))			{ 
						if (stricmp(szParam1,"1")==0)		{ D3DOptions.bDeclaration_Stream_Normal[z] = true; }
						else if (stricmp(szParam1,"0")==0)	{ D3DOptions.bDeclaration_Stream_Normal[z] = false; }
						else OutputMsg("Warning: Unknown param %s",szParam1); } } 
				sprintf(szTmp,"%s%d","declaration_stream_uvsets",z);
				CLTANode* pDeclaration_Stream_UVSets = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,szTmp);
				if (pDeclaration_Stream_UVSets) {
					if (pDeclaration_Stream_UVSets->GetElement(1) && (szParam1 = pDeclaration_Stream_UVSets->GetElement(1)->GetValue()))			{ 
						if (stricmp(szParam1,"1")==0)		{ D3DOptions.bDeclaration_Stream_UVSets[z] = true; }
						else if (stricmp(szParam1,"0")==0)	{ D3DOptions.bDeclaration_Stream_UVSets[z] = false; }
						else OutputMsg("Warning: Unknown param %s",szParam1); } } 
				sprintf(szTmp,"%s%d","declaration_stream_uvcount",z);
				CLTANode* pDeclaration_Stream_UVCount = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,szTmp);
				if (pDeclaration_Stream_UVCount) {
					if (pDeclaration_Stream_UVCount->GetElement(1) && (szParam1 = pDeclaration_Stream_UVCount->GetElement(1)->GetValue()))		{ 
						D3DOptions.Declaration_Stream_UVCount[z] = atoi(szParam1); } } 
				sprintf(szTmp,"%s%d","declaration_stream_basis",z);
				CLTANode* pDeclaration_Stream_Basis = CLTAUtil::ShallowFindList(pD3DRenderPassOptions,szTmp);
				if (pDeclaration_Stream_Basis) {
					if (pDeclaration_Stream_Basis->GetElement(1) && (szParam1 = pDeclaration_Stream_Basis->GetElement(1)->GetValue()))			{ 
						if (stricmp(szParam1,"1")==0)		{ D3DOptions.bDeclaration_Stream_BasisVectors[z] = true; }
						else if (stricmp(szParam1,"0")==0)	{ D3DOptions.bDeclaration_Stream_BasisVectors[z] = false; }
						else OutputMsg("Warning: Unknown param %s",szParam1); } } }

			// Re-Calc the constant registers (just in case - especially for the last const reg)...
			uint32 iConstReg = 0; 
			if (D3DOptions.ConstVector_ConstReg1 > -1) { D3DOptions.ConstVector_ConstReg1 = iConstReg; iConstReg += 1; }
			else { D3DOptions.ConstVector_ConstReg1 = -1; }
			if (D3DOptions.ConstVector_ConstReg2 > -1) { D3DOptions.ConstVector_ConstReg2 = iConstReg; iConstReg += 1; }
			else { D3DOptions.ConstVector_ConstReg2 = -1; }
			if (D3DOptions.ConstVector_ConstReg3 > -1) { D3DOptions.ConstVector_ConstReg3 = iConstReg; iConstReg += 1; }
			else { D3DOptions.ConstVector_ConstReg3 = -1; }
			if (D3DOptions.WorldViewTransform_ConstReg > -1) { D3DOptions.WorldViewTransform_ConstReg = iConstReg; iConstReg += D3DOptions.WorldViewTransform_Count * 4; }
			else { D3DOptions.WorldViewTransform_ConstReg = -1; }
			if (D3DOptions.ProjTransform_ConstReg > -1) { D3DOptions.ProjTransform_ConstReg = iConstReg; iConstReg += 4; }
			else { D3DOptions.ProjTransform_ConstReg = -1; }
			if (D3DOptions.WorldViewProjTransform_ConstReg > -1) { D3DOptions.WorldViewProjTransform_ConstReg = iConstReg; iConstReg += 4; }
			else { D3DOptions.WorldViewProjTransform_ConstReg = -1; }
			if (D3DOptions.ViewProjTransform_ConstReg > -1) { D3DOptions.ViewProjTransform_ConstReg = iConstReg; iConstReg += 4; }
			else { D3DOptions.ViewProjTransform_ConstReg = -1; }
			if (D3DOptions.CamPos_MSpc_ConstReg > -1) { D3DOptions.CamPos_MSpc_ConstReg = iConstReg; iConstReg += 1; }
			else { D3DOptions.CamPos_MSpc_ConstReg = -1; }
			if (D3DOptions.LightPosition_MSpc_ConstReg > -1) { D3DOptions.LightPosition_MSpc_ConstReg = iConstReg; iConstReg += D3DOptions.Light_Count; }
			else { D3DOptions.LightPosition_MSpc_ConstReg = -1; }
			if (D3DOptions.LightPosition_CSpc_ConstReg > -1) { D3DOptions.LightPosition_CSpc_ConstReg = iConstReg; iConstReg += D3DOptions.Light_Count; }
			else { D3DOptions.LightPosition_CSpc_ConstReg = -1; }
			if (D3DOptions.LightColor_ConstReg > -1) { D3DOptions.LightColor_ConstReg = iConstReg; iConstReg += D3DOptions.Light_Count; }
			else { D3DOptions.LightColor_ConstReg = -1; }
			if (D3DOptions.LightAtt_ConstReg > -1) { D3DOptions.LightAtt_ConstReg = iConstReg; iConstReg += D3DOptions.Light_Count; }
			else { D3DOptions.LightAtt_ConstReg = -1; }
			if (D3DOptions.Material_AmbDifEm_ConstReg > -1) { D3DOptions.Material_AmbDifEm_ConstReg = iConstReg; iConstReg += 3; }
			else { D3DOptions.Material_AmbDifEm_ConstReg = -1; }
			if (D3DOptions.Material_Specular_ConstReg > -1) { D3DOptions.Material_Specular_ConstReg = iConstReg; iConstReg += 1; }
			else { D3DOptions.Material_Specular_ConstReg = -1; }
			if (D3DOptions.AmbientLight_ConstReg > -1) { D3DOptions.AmbientLight_ConstReg = iConstReg; iConstReg += 1; }
			else { D3DOptions.AmbientLight_ConstReg = -1; }
			if (D3DOptions.PrevWorldViewTrans_ConstReg > -1) { D3DOptions.PrevWorldViewTrans_ConstReg = iConstReg; iConstReg += D3DOptions.PrevWorldViewTrans_Count * 4; }
			else { D3DOptions.PrevWorldViewTrans_ConstReg = -1; }
			D3DOptions.Last_ConstReg = iConstReg;

			pRenderStyle->SetRenderPass_D3DOptions(i,&D3DOptions); } }

	return true;
}
