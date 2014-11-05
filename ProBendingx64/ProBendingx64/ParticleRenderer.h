#pragma once
#include "OgreSimpleRenderable.h"
#include "PxPhysicsAPI.h"

#include "CudaInteropBuffer.h"

class IScene;
//class CudaInteropBuffer;

class ParticleRenderer : public Ogre::SimpleRenderable
{

private:
	UINT32 particleCount;
	UINT32 volumeSize;

	CUmodule module;
	CUfunction updateParticlesKernel;

	IScene* owningScene;

	physx::PxParticleSystem* particleSystem;

	CudaInteropBuffer* positionBuffer;

	//Ogre::HardwareVertexBufferSharedPtr mVertexBufferColor;
	
	void CreateVertexBuffers();


public:
	ParticleRenderer(IScene* sceneOwner, UINT32 numParticles = 100);
	virtual ~ParticleRenderer(void);

	void Resize(UINT32 newParticleNumber);
	void UpdateParticles(physx::PxVec3* posArray, UINT32 size);

	void Update(float gameTime);

	Ogre::Real getBoundingRadius(void) const;

	Ogre::Real getSquaredViewDepth(const Ogre::Camera*)const;
CUgraphicsResource cudaPositionBuffer;
};

