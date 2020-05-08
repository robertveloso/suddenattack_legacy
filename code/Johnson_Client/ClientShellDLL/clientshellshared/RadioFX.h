// 사람 머리 위에 Radio Message 날릴 때 붙이는 효과
#ifndef __RADIO_FX_H__
#define __RADIO_FX_H__

#include "BaseParticleSystemFX.h"

struct RDCREATESTRUCT : public BPSCREATESTRUCT
{
    RDCREATESTRUCT();

    LTVector vPos;
    LTVector vColor1;
    LTVector vColor2;
    LTVector vMinDriftVel;
    LTVector vMaxDriftVel;
    LTFLOAT  fLifeTime;
    LTFLOAT  fVolumeRadius;
    LTFLOAT  fRadius;
    LTFLOAT  fParticleCreateDelta;
    LTFLOAT  fMinParticleLife;
    LTFLOAT  fMaxParticleLife;
    uint8   nNumParticles;
    uint32  dwSystemFlags;
    LTBOOL   bIgnoreWind;
	HSTRING hstrTexture;
};

inline RDCREATESTRUCT::RDCREATESTRUCT()
{
	vPos.Init();
	vColor1.Init();
	vColor2.Init();
	vMinDriftVel.Init();
	vMaxDriftVel.Init();
	fLifeTime				= 0.0f;
	fVolumeRadius			= 0.0f;
	fRadius					= 0.0f;
	fParticleCreateDelta	= 0.0f;
	fMinParticleLife		= 0.0f;
	fMaxParticleLife		= 0.0f;
	nNumParticles			= 0;
	dwSystemFlags			= 0;
    bIgnoreWind             = LTFALSE;
    hstrTexture             = LTNULL;
}

class CRadioFX : public CBaseParticleSystemFX
{
	public :

		CRadioFX() : CBaseParticleSystemFX()
		{
			m_vMinDriftVel.Init();
			m_vMaxDriftVel.Init();
			m_fElapsedTime			= 0.0f;
			m_fElapsedEmissionTime	= 0.0f;
			m_fVolumeRadius			= 0.0f;
			m_fLifeTime				= 0.0f;
			m_fParticleCreateDelta	= 0.0f;
			m_nNumParticles			= 5;
			m_fMinParticleLife		= 5.0f;
			m_fMaxParticleLife		= 10.0f;
            m_bIgnoreWind           = LTFALSE;
            m_hstrTexture           = LTNULL;
		}

        virtual LTBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
        virtual LTBOOL Update();
        virtual LTBOOL CreateObject(ILTClient* pClientDE);

        inline void SetDriftVel(LTVector vMinVel, LTVector vMaxVel)
		{
			m_vMinDriftVel = vMinVel;
			m_vMaxDriftVel = vMaxVel;
		}

		virtual uint32 GetSFXID() { return (SFX_TOTAL_NUMBER + 1); }

		void	RemoveRadioFX()	{ RemoveAllParticles(); }

	private :

        LTFLOAT  m_fVolumeRadius;        // Radius of radio volume
        LTFLOAT  m_fLifeTime;            // How long each particle stays around
        LTFLOAT  m_fElapsedTime;         // How much time has elapsed on this system
        LTFLOAT  m_fElapsedEmissionTime; // How much time has passed since we last emitted particles

        LTVector m_vMinDriftVel;         // Min Drift velocity
        LTVector m_vMaxDriftVel;         // Max Drift velocity

        LTFLOAT  m_fParticleCreateDelta; // How often we create radio particles
        uint8   m_nNumParticles;        // Number we create every delta
        LTFLOAT  m_fMaxParticleLife;     // Maximum lifetime of a particle
        LTFLOAT  m_fMinParticleLife;     // Minimum lifetime of a particle
		HSTRING m_hstrTexture;			// Texture to sprite to use

        LTBOOL   m_bIgnoreWind;          // Ignore world wind
};

#endif // __RADIO_FX_H__