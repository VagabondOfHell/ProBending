#include "ParticleAffectors.h"
#include "OgreHardwareBufferManager.h"
#include "CudaGPUData.h"
#include "foundation/PxVec2.h"

ParticleAffector::~ParticleAffector()
{
	if(gpuData)
	{
		//Does this work if they arent registered?
		gpuData->UnregisterAllGraphicsResources();
		gpuData->FreeAllGPUMemory();
		delete gpuData;
		gpuData = NULL;
	}
}

bool ParticleAffector::CreateVertexBuffers(Ogre::VertexData* vertexData, Ogre::VertexElementSemantic semantic, 
					Ogre::VertexElementType type, const unsigned int source, const unsigned int extraIndex)
{
	if(vertexData)
	{
		//Create the vertex element at the specified source with an offset of 0
		vertexData->vertexDeclaration->addElement(source, 0, type, semantic, extraIndex);

		// allocate the vertex buffer
		vertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			Ogre::VertexElement::getTypeSize(type),
			maxParticlesAllowed,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
			false);

		// bind positions to specified location
		vertexData->vertexBufferBinding->setBinding(source, vertexBuffer);

		bindingIndex = source;//store the index used for binding

		return true;
	}

	return false;
}

#pragma region Lifetime Scale Affector
ScaleParticleAffector::ScaleParticleAffector()
	: ParticleAffector(), affectorAttributes()
{
}

ScaleParticleAffector::ScaleParticleAffector(const bool _enlarge, const float _minScale, const float _maxScale,  bool onGPU)
	:ParticleAffector(onGPU), affectorAttributes(_enlarge, _minScale, _maxScale)
{
	//Verify scales
	if(_minScale > _maxScale)
	{
		affectorAttributes.maxScale = _minScale;
		affectorAttributes.minScale = _maxScale;
		affectorAttributes.scaleDiff = affectorAttributes.maxScale - affectorAttributes.minScale;
	}
}

ScaleParticleAffector::~ScaleParticleAffector()
{
}

ParticleAffectorType::ParticleAffectorType ScaleParticleAffector::GetAffectorType()
{
	return ParticleAffectorType::Scale;
}

bool ScaleParticleAffector::Initialize(physx::PxCudaContextManager* const contextManager, 
			Ogre::VertexData* vertexData, const unsigned int maxParticles, const unsigned int source)
{
	maxParticlesAllowed = maxParticles;

	if(CreateVertexBuffers(vertexData, Ogre::VertexElementSemantic::VES_BLEND_INDICES,
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

void ScaleParticleAffector::LockBuffers()
{
	if(!onGPU)
		affectorAttributes.scales = (physx::PxVec4*)vertexBuffer->lock(Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY);
}

void ScaleParticleAffector::Update(const float gameTime, const float percentile, const unsigned int particleIndex)
{
	if(!onGPU)//if we are on the GPU, there is no need to update from CPU
	{
		if(!affectorAttributes.scales)
			LockBuffers();

		if(affectorAttributes.enlarge)//Adjust for enlargement
			affectorAttributes.scales[particleIndex].x = affectorAttributes.maxScale - (affectorAttributes.scaleDiff * percentile);
		else//Adjust for shrinkage
			affectorAttributes.scales[particleIndex].x = affectorAttributes.minScale + (affectorAttributes.scaleDiff * percentile);
	}
}

void ScaleParticleAffector::UnlockBuffers()
{
	if(vertexBuffer->isLocked())
	{
		vertexBuffer->unlock();
		affectorAttributes.scales = NULL;
	}
}

GPUScaleAffectorParams* const ScaleParticleAffector::GetGPUParamaters()
{
	if(!onGPU)
		return NULL;

	MapCudaBuffers();

	return &affectorAttributes;
}

void ScaleParticleAffector::MapCudaBuffers()
{
	if(!onGPU)
		return;

	//Set the scales to the device pointer
	affectorAttributes.scales = (physx::PxVec4*)gpuData->MapAndGetGPUDataPointer(0).devicePointer;
}

void ScaleParticleAffector::UnmapCudaBuffers()
{
	if(!onGPU)
		return;

	affectorAttributes.scales = NULL;

	gpuData->UnmapResourceFromCuda(0);
}

#pragma endregion

