#pragma once
#include "ParticleAffectors.h"

#include "ParticleSystemMaterial.h"

class FluidAndParticleBase;

class ParticleSystemMaterial;

class TextureParticleAffector :
	public ParticleAffector
{
protected:
	GPUTextureAffectorParams affectorParams;
	ParticleSystemMaterial* mat;

public:
	TextureParticleAffector(FluidAndParticleBase* partBase, bool animated = false, float animationTime = 0.0f, bool onGPU = false);
	
	TextureParticleAffector(ParticleSystemMaterial* partMat, bool animated = false, 
		float animationTime = 0.0f, bool onGPU = false);

	virtual ~TextureParticleAffector(void);

	virtual inline ParticleAffectorType::ParticleAffectorType GetAffectorType(){return ParticleAffectorType::Texture;}

	///<summary>Adds a texture to the material</summary>
	///<param name="textureName">The name of the texture to use</param>
	///<returns>True if successful, false if not</returns>
	virtual bool AddTextureToMaterial(const std::string& textureName);

	void CalculateFrameStep(float initialLifetime){affectorParams.CalculatePercentStep(initialLifetime);}

	virtual bool Initialize(ParticleSystemBase* owningSystem);

	virtual void Update(const float gameTime, GPUResourcePointers& pointers, const float percentile, const unsigned int particleIndex);

	virtual Ogre::VertexElementSemantic GetDesiredBuffer(){if(affectorParams.animated) return Ogre::VES_TEXTURE_COORDINATES; 
															else return Ogre::VES_COUNT;}

	virtual GPUParticleAffectorParams* const GetGPUParamaters();

	virtual GPUTextureAffectorParams* const GetParameters(){return &affectorParams;}

	virtual TextureParticleAffector* Clone();

};

