
#ifndef __PS2
#include <windows.h>
#endif
#include "bdefs.h"
#include "model.h"
#include "model_ops.h"
#include "conparse.h"
#include "geomroutines.h"

#include <math.h>
#include <float.h>


static int g_iCreateTransformKeys[2];
static float g_CreateTransformPercent;
static bool g_bRecurseFlipZ;
static Model *g_pRecurseModel;
static Model *g_pRecurseChildModel;
static LTMatrix *g_pRecurseTransforms;
static ModelAnim *g_pRecurseAnim;
static CMoArray<LTMatrix> *g_pNodeRelation;
static NodeControlFn g_pNC;
static void *g_pNCUserData;

extern void* dalloc(unsigned long size);
extern void* dalloc_z(unsigned long size);
extern void dfree(void *ptr);


#include <vector> // stl.
#include <set>

void model_SetAnimTrans(Model *pModel, uint32 iAnim, LTVector *pNewTrans)
{
	uint32 i;
	ModelAnim *pAnim = pModel->GetAnimCheck(iAnim);
	if(!pAnim)
		return;

	// Translate it.
	for(i=0; i < pAnim->m_KeyFrames; i++)
	{
		pAnim->GetRootNode()->m_KeyFrames[i].m_Translation = *pNewTrans;
	}
}


void model_OffsetAnimTrans(Model *pModel, uint32 iAnim, LTVector *pOffset)
{
	uint32 i;

	ModelAnim *pAnim = pModel->GetAnimCheck(iAnim);
	if(!pAnim)
		return;

	// Translate it.
	for(i=0; i < pAnim->m_KeyFrames; i++)
	{
		pAnim->GetRootNode()->m_KeyFrames[i].m_Translation += *pOffset;
	}
}


void model_RotateAnim(Model *pModel, uint32 iAnim, LTVector *pRot)
{
	uint32 i;
	LTMatrix mat[3];
	NodeKeyFrame *pKey;
	ModelAnim *pAnim;

	pAnim = pModel->GetAnimCheck(iAnim);
	if(!pAnim)
		return;

	gr_SetupMatrixEuler(mat[0].m, VEC_EXPAND(*pRot));

	// Translate it.
	for(i=0; i < pAnim->m_KeyFrames; i++)
	{
		pKey = &pAnim->GetRootNode()->m_KeyFrames[i];
		pKey->m_Quaternion.ConvertToMatrix(mat[1]);
		mat[2] = mat[0] * mat[1];
		pKey->m_Quaternion.ConvertFromMatrix(mat[2]);
	}
}


void model_SetAnimFramerate(Model *pModel, unsigned long iAnim, float framerate, std::vector<bool>* taggedKeys)
{
	unsigned long i, curMarkerTime, deltaTime, curTagNum;
	ModelAnim *pAnim;
	AnimKeyFrame *pKeyFrame;


	deltaTime = (unsigned long)(framerate * 1000.0f);
	if(deltaTime == 0)
		return;

	pAnim = pModel->GetAnimCheck(iAnim);
	if(!pAnim)
		return;

	if(pAnim->m_KeyFrames > 0)
	{
		curTagNum = 1;
		curMarkerTime = pAnim->m_KeyFrames[0].m_Time;

		// Remove all keyframes until we encounter one with delta time > deltaTime.
		for(i=1; i < (pAnim->m_KeyFrames-1); i++, curTagNum++)
		{
			pKeyFrame = &pAnim->m_KeyFrames[i];

			if(pKeyFrame->m_Time < (curMarkerTime + deltaTime))
			{
				bool hasKeyString = (pKeyFrame->m_pString[0] != '\0');
				bool isTagged = (taggedKeys && (*taggedKeys)[curTagNum]);

				// Get rid of this keyframe if it doesn't have a keystring and isn't tagged
				if( !(hasKeyString || isTagged) )
				{
					pAnim->RemoveKeyFrame(i);
					--i;
				}
			}
			else
			{
				curMarkerTime = pKeyFrame->m_Time;
			}
		}
	}
}


// {BP 1/20/98}
// Moved this code to this function because it was used in a couple places...
void model_GetDimsFromBounding( LTVector *pvBoundMax, LTVector *pvBoundMin, LTVector *pvDims )
{
	LTVector vDimsMax;

	// Set the model dims from the first frame of the first animation bounding box...
	// Not sure if BoundMin is always negative and BoundMax is always positive,
	// so got to do some absolute values...
	pvDims->x = ( pvBoundMin->x > 0.0f ) ? pvBoundMin->x : -pvBoundMin->x;
	pvDims->y = ( pvBoundMin->y > 0.0f ) ? pvBoundMin->y : -pvBoundMin->y;
	pvDims->z = ( pvBoundMin->z > 0.0f ) ? pvBoundMin->z : -pvBoundMin->z;
	vDimsMax.x = ( pvBoundMax->x > 0.0f ) ? pvBoundMax->x : -pvBoundMax->x;
	vDimsMax.y = ( pvBoundMax->y > 0.0f ) ? pvBoundMax->y : -pvBoundMax->y;
	vDimsMax.z = ( pvBoundMax->z > 0.0f ) ? pvBoundMax->z : -pvBoundMax->z;
	if( pvDims->x < vDimsMax.x )
		pvDims->x = vDimsMax.x;
	if( pvDims->y < vDimsMax.y )
		pvDims->y = vDimsMax.y;
	if( pvDims->z < vDimsMax.z )
		pvDims->z = vDimsMax.z;
}

// ------------------------------------------------------------------------
// GetExtends
// min max
// ------------------------------------------------------------------------
void GetExtends( LTVector &min, LTVector &max, const std::vector<LTVector> & points )
{
	min.Init(FLT_MAX,FLT_MAX,FLT_MAX);
	max.Init( -FLT_MAX,-FLT_MAX,-FLT_MAX);
	
	for( uint32 i = 0 ;i < points.size() ; i++ )
	{
		if( min.x > points[i].x )
			min.x = points[i].x ;
		if( min.y > points[i].y )
			min.y = points[i].y ;
		if( min.z > points[i].z )
			min.z = points[i].z ;

		if( max.x < points[i].x )
			max.x = points[i].x ;
		if( max.y < points[i].y )
			max.y = points[i].y ;
		if( max.z < points[i].z )
			max.z = points[i].z ;
	}
}

LTVector fabs(const LTVector vec )
{
	LTVector rvec ;
	rvec.x = fabsf(vec.x);
	rvec.y = fabsf(vec.y);
	rvec.z = fabsf(vec.z);
	return rvec;
}
// ------------------------------------------------------------------------
// SetOBB( model-node, parent-node )
//
// create an obb based on the positions of the two nodes. The obb will 
// be positioned half-way between
// ------------------------------------------------------------------------
static void SetOBB( ModelNode *pCur, CMoArray<ModelNode*, NoCache> &children )
{
	LTVector min,max;
	LTVector tmp_pos ;
	LTVector chl_trans, par_trans;
	SOBB obb = pCur->GetOBB();
	children[0]->GetGlobalTransform().GetTranslation(chl_trans);;
	pCur->GetGlobalTransform().GetTranslation(par_trans);


	tmp_pos = chl_trans - par_trans ;
	tmp_pos = tmp_pos /-2.0f ;
	//obb.m_Size = fabs( (tmp_pos /-2.0f) ) ;
	tmp_pos += chl_trans ;

	pCur->GetInvGlobalTransform().Apply(tmp_pos);
	//pCur->GetInvGlobalTransform().Apply(obb.m_Size);
	
	obb.m_Pos  = tmp_pos ;
	
	pCur->SetOBB( obb );
}

// params :
// node list
// link length (min
// min vert weight.

// ------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------
struct ExtentUtil
{ 
	ExtentUtil(LTVector m, LTVector mx) :m_min(m),m_max(mx) {} 

	void test_set( float *fval ) {
			LTVector val(fval[0],fval[1],fval[2]);
			
			if( m_min.x > val.x )	m_min.x = val.x ;
			if( m_min.y > val.y )	m_min.y = val.y ;
			if( m_min.z > val.z )	m_min.z = val.z ;
			if( m_max.x < val.x )	m_max.x = val.x ;
			if( m_max.y < val.y )	m_max.y = val.y ;
			if( m_max.z < val.z )	m_max.z = val.z ;
	}

	SOBB to_obb(  )
	{
		SOBB obb;
		//obb.m_Pos = (m_max  - m_min) / 2.0f ;
		obb.m_Size = ( m_max - m_min ) ;
		return obb;
	}

	LTVector m_min, m_max ;
	
	// float link_length ; // average link length
};

static
void GetOBBDimsFromGeom(Model *pModel, 
						std::vector<bool> selected_node,
						float min_weight,
						float min_linkdist)
 				
				
{
	uint32 i, j, k;
	ModelPiece *pPiece;
	ModelVert *pVert;
	NewVertexWeight *pWeight;
	PieceLOD *pLOD;
	float cur_lod_dist = 0;
	uint32 num_nodes = pModel->NumNodes();
	
	

	std::vector< ExtentUtil > obbext ;
	obbext.reserve(num_nodes);
	
	// termin inter node lengths here.

	for( i = 0 ; i < num_nodes ; i++ )
	{
		obbext.push_back(ExtentUtil(LTVector(FLT_MAX,FLT_MAX,FLT_MAX),
									LTVector(-FLT_MAX,-FLT_MAX,-FLT_MAX)));
	}

	for(i=0; i < pModel->NumPieces(); i++)
	{
		pPiece = pModel->GetPiece(i);
		pLOD = pPiece->GetLOD(cur_lod_dist);
  
		for(j=0; j < pLOD->m_Verts; j++)
		{
			pVert = &pLOD->m_Verts[j];

			//  this isn't quite accurate but close enough.         
			if(pVert->m_nWeights > 0 )
			{
				for(k=0; k < pVert->m_nWeights; k++)
				{
					pWeight = &pVert->m_Weights[k];
					
					if( pWeight->m_Vec[3] > min_weight )
						obbext[pWeight->m_iNode].test_set( pWeight->m_Vec);
				}
			}
		}// for every vertex
	}

	for( i = 0 ; i < pModel->m_FlatNodeList ; i++ )
	{
		pModel->m_FlatNodeList[i]->SetOBB( obbext[i].to_obb() );
		pModel->m_FlatNodeList[i]->EnableOBB();
	}
}



// ------------------------------------------------------------------------
// TraverseModelNodesToSetOBB( root-model-node )
//
// recursively traverse the hierachy setting up the obbs along the way.
// roots don't get obbs automatically since children's obbs encompass
// the parents.
// ------------------------------------------------------------------------
static void TraverseModelNodesToSetOBB( ModelNode *root, std::vector<bool> sel_nds )
{
	
	for( uint32 i = 0 ;i < root->NumChildren() ; i++ )
	{
		SetOBB( root, root->m_Children );
		TraverseModelNodesToSetOBB( root->GetChild(i), sel_nds );
	}

	// if no kids no obb.
	//if( !root->NumChildren() )
	//root->DisableOBB();
}


void model_CreateDefaultOBB( Model *pModel, 
							std::vector<bool> selected_nodes,
							float min_weight,
							float min_link_length							)
{
	ASSERT(pModel);
	GetOBBDimsFromGeom(pModel,selected_nodes,min_weight,min_link_length);

	if( pModel->GetRootNode() != NULL )
		TraverseModelNodesToSetOBB( pModel->GetRootNode(), selected_nodes );
}


// ------------------------------------------------------------------------
// model_CreateDefaultOBB( model )
// 
// given a model, create obbs based on the default hierarchy of the model.
// ------------------------------------------------------------------------
void model_CreateDefaultOBB( Model *pModel )
{
	ASSERT(pModel);

	std::vector<bool> selected_nodes(pModel->NumNodes(), true);
	float min_link_length = 1000.0f ;
	float min_weight      = 0.0f ;

	model_CreateDefaultOBB(pModel,selected_nodes,min_weight,min_link_length);

}
