#include "ColourFadeParticleAffector.h"
#include "OgreHardwareBufferManager.h"
#include "CudaGPUData.h"

ColourFadeParticleAffector::ColourFadeParticleAffector()
	:ParticleAffector()
{

}

ColourFadeParticleAffector::ColourFadeParticleAffector(physx::PxVec4 startColour, physx::PxVec4 endColour, bool onGPU)
	:ParticleAffector(onGPU), affectorAttributes(startColour, endColour)
{
}

ColourFadeParticleAffector::~ColourFadeParticleAffector(void)
{
}

ParticleAffectorType::ParticleAffectorType ColourFadeParticleAffector::GetAffectorType()
{
	return ParticleAffectorType::ColourToColour;
}

bool ColourFadeParticleAffector::Initialize(physx::PxCudaContextManager* const contextManager, 
		Ogre::VertexData* vertexData, const unsigned int maxParticles, const unsigned int source)
{
	maxParticlesAllowed = maxParticles;

	if(CreateVertexBuffers(vertexData, Ogre::VertexElementSemantic::VES_DIFFUSE,
		Ogre::VertexElementType::VET_FLOAT4, source))
	{
		if(onGPU)
		{
			if(contextManager)
			{
				gpuData = new CudaGPUData(contextManager, 1, 0);
				//Try to register the graphics resource
				bool gpuAllocationResult = gpuData->RegisterCudaGraphicsResource(0, vertexBuffer);

				if(!gpuAllocationResult)//if registration failed
				{
					delete gpuData;//delete the GPU buffer
					onGPU = false;//indicate we are not using the GPU
				}
			}
			else
				onGPU = false;
		}

		return true;
	}

	return false;
}

void ColourFadeParticleAffector::LockBuffers()
{
	if(!onGPU)
		affectorAttributes.colours = (physx::PxVec4*)vertexBuffer->lock(Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY);
}

void ColourFadeParticleAffector::Update(const float gameTime, const float percentile, const unsigned int particleIndex)
{
	if(!onGPU)//if we are on the GPU, there is no need to update from CPU
	{
		if(!affectorAttributes.colours)
			LockBuffers();

		affectorAttributes.colours[particleIndex] = affectorAttributes.endColour - (affectorAttributes.colourDifference * percentile);
	}
}

void ColourFadeParticleAffector::UnlockBuffers()
{
	if(vertexBuffer->isLocked())
	{
		vertexBuffer->unlock();
		affectorAttributes.colours = NULL;
	}
}

void ColourFadeParticleAffector::MapCudaBuffers()
{
	if(!onGPU)
		return;

	affectorAttributes.colours = (physx::PxVec4*)gpuData->MapAndGetGPUDataPointer(0).devicePointer;
}

GPUColourFaderAffectorParams* const ColourFadeParticleAffector::GetGPUParamaters()
{
	if(!onGPU)
		return NULL;

	MapCudaBuffers();

	return &affectorAttributes;
}

void ColourFadeParticleAffector::UnmapCudaBuffers()
{
	if(!onGPU)
		return;

	affectorAttributes.colours = NULL;

	gpuData->UnmapResourceFromCuda(0);
}