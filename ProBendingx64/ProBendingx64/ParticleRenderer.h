#pragma once
#include "OgreSimpleRenderable.h"
#include "PxPhysicsAPI.h"

class IScene;

class ParticleRenderer : public Ogre::SimpleRenderable
{

private:
	UINT32 particleCount;
	UINT32 volumeSize;

	IScene* owningScene;

	CUgraphicsResource cudaBuffer;

	//Pointers to buffer data
	Ogre::HardwareVertexBufferSharedPtr mVertexBufferPosition;
	Ogre::HardwareVertexBufferSharedPtr mVertexBufferColor;

	void CreateVertexBuffers();

	void MapToCudaBuffer();

public:
	ParticleRenderer(IScene* sceneOwner, UINT32 numParticles = 100);
	virtual ~ParticleRenderer(void);

	
};

