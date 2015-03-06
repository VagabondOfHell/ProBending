#pragma once
#include "ParticleAffectors.h"

class RotationAffector :
	public ParticleAffector
{
protected:
	GPURotationAffectorParams affectorAttributes;

public:
	RotationAffector();

	RotationAffector(float degreeMinRot, float degreeMaxRot, bool onGPU = false);
	~RotationAffector(void);

	virtual inline ParticleAffectorType::ParticleAffectorType GetAffectorType(){return ParticleAffectorType::Rotation;}

	virtual bool Initialize(ParticleSystemBase* owningSystem);

	virtual void Update(const float gameTime, GPUResourcePointers& pointers, const float percentile, const unsigned int particleIndex);

	virtual Ogre::VertexElementSemantic GetDesiredBuffer(){return Ogre::VES_NORMAL;}

	virtual GPUParticleAffectorParams* const GetGPUParamaters();

	virtual GPUParticleAffectorParams* const GetParameters();

	virtual ParticleAffector* Clone();

};

