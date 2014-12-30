#pragma once
#include "ParticleAffectors.h"

class ColourFadeParticleAffector :
	public ParticleAffector
{
protected:
	GPUColourFaderAffectorParams affectorAttributes;

public:
	ColourFadeParticleAffector();

	ColourFadeParticleAffector(physx::PxVec4 startColour, physx::PxVec4 endColour,  bool onGPU = false);
	virtual ~ColourFadeParticleAffector(void);

	virtual ParticleAffectorType::ParticleAffectorType GetAffectorType();

	virtual Ogre::VertexElementSemantic GetDesiredBuffer(){return Ogre::VES_DIFFUSE;}
	
	///<summary>Initializes the affector</summary>
	///<param name="contextManager">Required if performing on the GPU</param>
	///<param name="vertexData">The vertex data to add buffers to</param>
	///<param name="maxParticles">The maximum allowed particles</param>
	///<param name="source">The index the buffer should be bound to</param>
	///<returns>True if successful, false if not. If false, invalid vertex data was most likely passed</returns>
	virtual bool Initialize(ParticleSystemBase* owningSystem);

	virtual void Update(const float gameTime, GPUResourcePointers& pointers, const float percentile, const unsigned int particleIndex);

	virtual GPUColourFaderAffectorParams* const GetGPUParamaters();
};

