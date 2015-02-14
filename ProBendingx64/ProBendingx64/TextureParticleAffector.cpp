#include "TextureParticleAffector.h"

#include "FluidAndParticleBase.h"

TextureParticleAffector::TextureParticleAffector(FluidAndParticleBase* partBase, bool animated /*= false*/, bool onGPU /*= false*/)
	:ParticleAffector(onGPU), affectorParams()
{
	mat = partBase->GetMaterial();
}

TextureParticleAffector::~TextureParticleAffector(void)
{
}

bool TextureParticleAffector::AddTextureToMaterial(const std::string& textureName)
{
	return mat->AddTextureUnit(textureName);
}

bool TextureParticleAffector::Initialize(ParticleSystemBase* owningSystem)
{
	return true;
}

void TextureParticleAffector::Update(const float gameTime, GPUResourcePointers& pointers, 
			const float percentile, const unsigned int particleIndex)
{
	affectorParams.timePassed += gameTime;

	if(affectorParams.timePassed >= affectorParams.timeToSwitch)
	{
		affectorParams.timePassed = 0.0f;
		affectorParams.textureIndex = (affectorParams.textureIndex + 1) % affectorParams.maxTextures;
	}
}

GPUParticleAffectorParams* const TextureParticleAffector::GetGPUParamaters()
{
	if(!onGPU)
		return NULL;

	return &affectorParams;
}
