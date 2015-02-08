#pragma once
#include "OgreSimpleRenderable.h"
#include "ParticleSystemParams.h"

class FluidBase: public Ogre::SimpleRenderable
{
	friend class SceneSerializer;

protected:
	physx::PxParticleFluid* pxFluidBase;

public:
	FluidBase(void);
	virtual ~FluidBase(void);
};

