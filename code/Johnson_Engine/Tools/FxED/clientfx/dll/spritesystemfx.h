//------------------------------------------------------------------
//
//   MODULE  : SPRITESYSTEM.H
//
//   PURPOSE : Defines class CSpriteSystem
//
//   CREATED : On 10/26/98 At 4:00:00 PM
//
//------------------------------------------------------------------

#ifndef __SPRITESYSTEM__H_
	#define __SPRITESYSTEM__H_

	// Includes....

	#include "basefx.h"
	#include "linklist.h"
	#include "fastlist.h"
	#include "swaplist.h"

	// Structures....

	struct SPRITE
	{
		float								m_tmStart;
		LTVector								m_vPos;
		LTVector								m_vVel;
		CLinkListNode<FX_COLOURKEY>		   *m_pCurKeyFrame;
	};

	struct PRE_POSDIR
	{
		LTVector								m_vPos;
		LTVector								m_vDir;
	};

	// Defines....

	#define SS_POINT						0
	#define SS_PLANEUP						1
	#define SS_SPHERE						2
	#define SS_PLANEOUT						3
	#define SS_CONE							4
	#define SS_CIRCLE						5
	#define SS_LINE							6

	// Classes....

	class CSpriteSystem : public CBaseFX
	{
		public :

			// Constuctor

											CSpriteSystem();

			// Destructor

										   ~CSpriteSystem();

			// Member Functions

			bool							Init(ILTClient *pClientDE, FX_BASEDATA *pData, CLinkList<FX_PROP> *pList);
			void							Term();
			bool							Update(float tmCur);
			bool							IsFinished() { return (m_collSprites.GetUsed() == 0) ? true : false; }

			inline void						AddParticle(LTVector vPos, float fWidth, float fHeight, HTEXTURE hTexture, uint8 r, uint8 g, uint8 b, uint8 a);

			void							Unfreeze(float tmUnfreeze)
											{
												if (m_bFrozen)
												{
													float tmFreezeDelta = tmUnfreeze - m_tmFreeze;
													m_tmLastEmission += tmFreezeDelta;
												}

												// Call through to base class

												CBaseFX::Unfreeze(tmUnfreeze);
											}

			void							SetDetail(float fDetail);

			// Accessors

		protected :

			// Member Variables

			uint32							m_nInitialSpriteSystemEmission;
			
			char							m_sSpriteName[128];
			CSwapList<SPRITE>				m_collSprites;
			uint32							m_nSpriteSystemEmission;
			float							m_tmLastEmission;
			float							m_tmSpriteSystemEmission;
			float							m_tmSpriteLifespan;
			int								m_nEmissionType;
			float							m_fRadius;
			LTVector						m_vPlaneDir;
			LTVector						m_vUp;
			LTVector						m_vRight;
			float							m_fVel;
			LTVector						m_vWind;
			float							m_fWindAmount;
			LTMatrix						m_mSpin;
			bool							m_bUseSpin;
			float							m_xRot;
			float							m_yRot;
			float							m_zRot;
			PRE_POSDIR					   *m_pDirTable;
			int								m_nAlphaType;
			HOBJECT							m_hSprite;
			HTEXTURE					   *m_pTexArray;
			float							m_tmSpriteLen;
			uint32							m_nSpriteFrames;
			float							m_fStretchU;
			float							m_fStretchV;
	};

	// Secret little class to do some run time initialisation....

	class CRandVectorGen
	{
		public :

			// Constructor

											CRandVectorGen();
	};

#endif