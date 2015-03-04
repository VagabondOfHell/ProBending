#include "TextureParticleAffector.h"

#include "ParticleSystemBase.h"

TextureParticleAffector::TextureParticleAffector(FluidAndParticleBase* partBase, bool animated /*= false*/, 
		float animationTime /*= 0.0f*/, bool onGPU /*= false*/)
	:ParticleAffector(onGPU), affectorParams()
{
	affectorParams.animationTime = animationTime;
	affectorParams.animated = animated;

	mat = partBase->GetMaterial();
}

TextureParticleAffector::TextureParticleAffector(ParticleSystemMaterial* partMat, 
		bool animated /*= false*/, float animationTime /*= 0.0f*/, bool onGPU /*= false*/)
	:ParticleAffector(onGPU), affectorParams()
{
	affectorParams.animationTime = animationTime;
	affectorParams.animated = animated;

	mat = partMat;
}

TextureParticleAffector::~TextureParticleAffector(void)
{
}

bool TextureParticleAffector::AddTextureToMaterial(const std::string& textureName)
{
	bool result = mat->AddTextureUnit(textureName);

	if(result)
	{
		affectorParams.maxTextures++;
		//affectorParams.animationTime = 1.0f / affectorParams.maxTextures;//1.0f / (affectorParams.maxTextures / affectorParams.animationTime);
	}
	return result;
}

bool TextureParticleAffector::Initialize(ParticleSystemBase* owningSystem)
{
	owningSystem->CreateVertexBuffer(GetDesiredBuffer());
	return true;
}

void TextureParticleAffector::Update(const float gameTime, GPUResourcePointers& pointers, 
			const float percentile, const unsigned int particleIndex)
{
	//Check for reset
	if(percentile <= 0.0f || percentile >= 0.99f)
		pointers.uv0[particleIndex].y = 1.0f - affectorParams.percentStep;

	//Using .y as our target, check if the percentile is less than the target
	if(percentile <= pointers.uv0[particleIndex].y)
	{
		//Update the target
		pointers.uv0[particleIndex].y -= affectorParams.percentStep;
		if(pointers.uv0[particleIndex].y < 0.0f)
			pointers.uv0[particleIndex].y = 0.0f;
		//Update the image
		pointers.uv0[particleIndex].x = ((int)pointers.uv0[particleIndex].x + 1) % affectorParams.maxTextures;

	}
}

GPUParticleAffectorParams* const TextureParticleAffector::GetGPUParamaters()
{
	if(!onGPU)
		return NULL;

	return &affectorParams;
}

TextureParticleAffector* TextureParticleAffector::Clone()
{
	TextureParticleAffector* clone = new TextureParticleAffector(mat, 
		affectorParams.animated, affectorParams.animationTime, affectorParams.onGPU);

	clone->onGPU = onGPU;

	clone->affectorParams = affectorParams;
	clone->affectorParams.onGPU = onGPU;

	return clone;
}
