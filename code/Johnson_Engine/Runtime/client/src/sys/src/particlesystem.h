//------------------------------------------------------------------
//
//  FILE      : particlesystem.h
//
//  PURPOSE   : Particle system instance header file.
//
//  CREATED   : May 30 1997
//
//  COPYRIGHT : MONOLITH Inc 1997 All Rights Reserved
//
//------------------------------------------------------------------

#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__


#ifndef __DE_OBJECTS_H__
#include "de_objects.h"
#endif


class CClientMgr;


// Set the texture for the particles.
LTRESULT ps_SetTexture(LTParticleSystem *pSystem, const char *pName);

// Add a bunch of particles.
void ps_AddParticles(LTParticleSystem *pSystem, uint32 nParticles,
                    const LTVector *minOffset, const LTVector *maxOffset,
                    const LTVector *minVel, const LTVector *maxVel,
                    const LTVector *minColor, const LTVector *maxColor,
                    LTFLOAT minLifetime, LTFLOAT maxLifetime);

// This should be called just before rendering so that it can update
// its bounding box.
void ps_UpdateParticles(LTParticleSystem *pSystem, LTFLOAT t);
void ps_UpdateParticleBoundingBox(LTParticleSystem *pSystem);

// Remove a particle.
inline void ps_RemoveParticle(LTParticleSystem *pSystem, PSParticle *pParticle) {
    pParticle->m_pPrev->m_pNext = pParticle->m_pNext;
    pParticle->m_pNext->m_pPrev = pParticle->m_pPrev;
    sb_Free(pSystem->m_pParticleBank, pParticle);
    pSystem->m_nParticles--;
}


// Stretch the bounding box to fit the vector's position.
inline void ps_UpdateBox(LTParticleSystem *pSystem, const LTVector &cPos) {
    if (cPos.x < pSystem->m_MinPos.x)   pSystem->m_MinPos.x = cPos.x;
    if (cPos.y < pSystem->m_MinPos.y)   pSystem->m_MinPos.y = cPos.y;
    if (cPos.z < pSystem->m_MinPos.z)   pSystem->m_MinPos.z = cPos.z;

    if (cPos.x > pSystem->m_MaxPos.x)   pSystem->m_MaxPos.x = cPos.x;
    if (cPos.y > pSystem->m_MaxPos.y)   pSystem->m_MaxPos.y = cPos.y;
    if (cPos.z > pSystem->m_MaxPos.z)   pSystem->m_MaxPos.z = cPos.z;
}

// Add a particle.
inline PSParticle* ps_AddParticle(LTParticleSystem *pSystem, const LTVector *pPos, const LTVector *pColor, const LTVector *pVel, float lifeTime) 
{
    PSParticle *pParticle;

    pParticle = (PSParticle*)sb_Allocate(pSystem->m_pParticleBank);
    if (!pParticle) {
        return LTNULL;
    }

    ps_UpdateBox(pSystem, *pPos);
    pParticle->m_Pos				= *pPos;
    pParticle->m_Color				= *pColor;
    pParticle->m_Velocity			= *pVel;
    pParticle->m_Lifetime			= lifeTime;
    pParticle->m_TotalLifetime		= lifeTime;
    pParticle->m_Alpha				= 1.0f;
    pParticle->m_Size				= pSystem->m_ParticleRadius;
	pParticle->m_nUserData			= 0;

	pParticle->m_fAngle				= 0.0f;
	pParticle->m_fAngularVelocity	= 0.0f;

    // Add it to the end of the list.
    pParticle->m_pPrev				= pSystem->m_ParticleHead.m_pPrev;
    pParticle->m_pNext				= &pSystem->m_ParticleHead;
    pParticle->m_pPrev->m_pNext		= pParticle->m_pNext->m_pPrev = pParticle;

    ++pSystem->m_nParticles;
    ++pSystem->m_nChangedParticles;

    return pParticle;
}

// Call before and after updating particle positions.  Updates the
// bounding sphere info.  EndUpdatingPositions returns LTTRUE if the
// system grew.
void ps_StartUpdatingPositions(LTParticleSystem *pSystem);
LTBOOL ps_EndUpdatingPositions(LTParticleSystem *pSystem);

// Recalculates the bounding box.
void ps_OptimizeParticles(LTParticleSystem *pSystem);

//sorts the particles in a system based upon the direction specified
void ps_SortParticles(LTParticleSystem *pSystem, const LTVector& vDir, uint32 nNumIters);


#endif  // __PARTICLESYSTEM_H__
