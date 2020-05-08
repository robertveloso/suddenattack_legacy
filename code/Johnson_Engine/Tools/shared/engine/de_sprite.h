
// This module defines the Sprite structures.

#ifndef __DE_SPRITE_H__
#define __DE_SPRITE_H__

    #ifndef __LTBASETYPES_H__
	#include "ltbasetypes.h"
    #endif

    #ifndef __ILTSPRITECONTROL_H__
	#include "iltspritecontrol.h"
    #endif


	class SharedTexture;



	struct SpriteEntry
	{
		SharedTexture *m_pTex;
	};
	
	
	struct SpriteAnim
	{
		char			m_sName[32];
		SpriteEntry		*m_Frames;
		uint32			m_nFrames;
		uint32			m_MsAnimLength;
		uint32			m_MsFrameRate;
		LTBOOL			m_bKeyed;
		uint32			m_ColourKey;
		LTBOOL			m_bTranslucent;
	};


	struct Sprite
	{
		LTLink		m_Link;
		SpriteAnim	*m_Anims;
		uint32		m_nAnims;
	};


	// Tracks sprite frames.
	struct SpriteTracker
	{
		Sprite			*m_pSprite;
		SpriteAnim		*m_pCurAnim;
		SpriteEntry		*m_pCurFrame;			
		uint32	m_MsCurTime;
		uint32	m_Flags;
	};


#endif  // __DE_SPRITE_H__
