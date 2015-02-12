#include "ParticleSystemBase.h"

#include "AbstractParticleEmitter.h"
#include "ParticleAffectors.h"
#include "ParticleKernel.h"

#include "PxPhysics.h"
#include "PsBitUtils.h"
#include "PxScene.h"
#include "pxtask/PxCudaContextManager.h"

#include "OgreCamera.h"
#include "OgreHardwareBufferManager.h"

using namespace physx;

ParticleSystemBase::ParticleSystemBase(std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, 
		float _initialLifetime, ParticleSystemParams& paramsStruct)
		: FluidAndParticleBase(_emitter, _maximumParticles, _initialLifetime, paramsStruct.cudaContext)
{	
	//Create the particle system on PhysX's end. Ignores per particle rest offset arguments here because set base flags handles it
	pxParticleSystem = PxGetPhysics().createParticleSystem(maximumParticles);
		
	//place the system on the GPU if it should be
	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eGPU, onGPU);
	
	//Check that PhysX didn't overwrite our GPU selection. If they did, reset our information
	if(!(pxParticleSystem->getParticleBaseFlags() & physx::PxParticleBaseFlag::eGPU))
	{
		onGPU = false;
		cudaContextManager = NULL;
	}

	SetParticleBaseFlags(pxParticleSystem, paramsStruct.baseFlags);

	//Set the gravity flag
	pxParticleSystem->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !paramsStruct.useGravity);
	
	SetSystemData(pxParticleSystem, paramsStruct);
	
	particleBase = pxParticleSystem;
}

ParticleSystemBase::ParticleSystemBase(physx::PxParticleSystem* physxParticleSystem, 
				std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime)
		: FluidAndParticleBase(_emitter, _maximumParticles, _initialLifetime, NULL)
{
	cudaKernel = NULL;

	//Set parent copy and this copy to the specified system
	particleBase = pxParticleSystem = physxParticleSystem;

	readableData = pxParticleSystem->getParticleReadDataFlags();
}


ParticleSystemBase::~ParticleSystemBase(void)
{
	if(cudaKernel)
		delete cudaKernel;

	if(lifetimes)
	{
		delete[] lifetimes;
		lifetimes = NULL;
	}
}

void ParticleSystemBase::UpdateParticleSystemCPU(const float time, const physx::PxParticleReadData* const readData)
{
	using namespace physx;

	//Check if there is any updating to do
	if (readData->validParticleRange > 0)
	{
		GPUResourcePointers lockedBufferData;
		int numParticles(0);

		if(affectors.hostAffector)
		{
			lockedBufferData = LockBuffersCPU();

			//Lock Ogre GL Buffers if not using CUDA so we can update them
			for (AffectorMap::iterator start = affectors.GetMapBegin(); start != affectors.GetMapEnd(); ++start)
			{
				//Apply affectors if on CPU
				start->second->PreUpdate();
			}
		}
		
		for (PxU32 w = 0; w <= (readData->validParticleRange-1) >> 5; w++)
		{
			//For each bit of the bitmap
			for (PxU32 b = readData->validParticleBitmap[w]; b; b &= b-1)
			{
				PxU32 index = (w << 5 | shdfnd::lowestSetBit(b));

				++numParticles;

				//Check particle validity
				if(QueryParticleRemoval(index, readData))
				{
					//If lifetime is equal or below zero
					indicesToRemove.push_back(index); //indicate removal
					lifetimes[index] = 0;//set lifetime to 0
				
					if(!onGPU)
						//Reset position. Not concerned of W component here because its a dead particle anyways
						lockedBufferData.positions[index] = PxVec4(std::numeric_limits<float>::quiet_NaN());

					continue;
				}
				//If the particle is valid and lifetimes are above 0, subtract this frame
				else
				{
					lifetimes[index] -= time;

					if(!onGPU)
					{
						// copy particle positions over
						const PxVec3& position = readData->positionBuffer[index];
						lockedBufferData.positions[index].x = position.x;
						lockedBufferData.positions[index].y = position.y;
						lockedBufferData.positions[index].z = position.z;
					}
					
					if(affectors.hostAffector)
					{
						float percentile = lifetimes[index] / initialLifetime;
						for (AffectorMap::iterator start = affectors.GetMapBegin(); start != affectors.GetMapEnd(); ++start)
						{
							//Apply affectors if on CPU
							start->second->Update(time, lockedBufferData, percentile, index);								
						}
					}
					
					//Allow children to update their own particle data
					UpdateParticle(index, readData);
				}
			}//end of bitmap for loop (b)
		}//end of particle range for loop

		//If this system is on the CPU, unlock the vertex buffer
		if(affectors.hostAffector)
		{
			UnlockBuffersCPU();
			for (AffectorMap::iterator start = affectors.GetMapBegin(); start != affectors.GetMapEnd(); ++start)
			{
				//Apply affectors if on CPU
				start->second->PostUpdate();								
			}
		}
		
		if(onGPU)
			mRenderOp.vertexData->vertexCount = numParticles;
	}//end if valid range > 0
}

bool ParticleSystemBase::AddAffector(std::shared_ptr<ParticleAffector> affectorToAdd)
{
	//Try to insert and return results (if it exists, it will return false, so no need to perform a find first)
	AffectorMapInsertResult result = affectors.affectorMap.insert(AffectorMap::value_type(affectorToAdd->GetAffectorType(), affectorToAdd));//Try to insert

	//if successful
	if(result.second)
	{
		//update flag for CPU and GPU affectors
		affectors.allTypesCombination |= result.first->first;
		//if system and affector on GPU, update GPU flag
		if(onGPU && affectorToAdd->GetOnGPU())
		{
			affectors.gpuTypeCombination |= result.first->first;
		}
		else
		{
			affectors.hostAffector = true;
			affectorToAdd->onGPU = false;
		}

		affectorToAdd->Initialize(this);
	}
	return result.second;
}

bool ParticleSystemBase::RemoveAffector(ParticleAffectorType::ParticleAffectorType typeToRemove)
{
	return RemoveAndGetAffector(typeToRemove).get() != NULL;
}

std::shared_ptr<ParticleAffector> ParticleSystemBase::RemoveAndGetAffector(ParticleAffectorType::ParticleAffectorType typeToRemove)
{
	AffectorMap::iterator result = affectors.affectorMap.find(typeToRemove);

	if(result != affectors.GetMapEnd())
	{
		//Update flag for CPU and GPU
		affectors.allTypesCombination &= result->first;

		//Update flag for GPU if system and affector are on GPU
		if(onGPU && result->second->GetOnGPU())
			affectors.gpuTypeCombination &= result->first;

		Ogre::VertexElementSemantic semantic = result->second->GetDesiredBuffer();
		bool anotherFound = false;

		//Check if any other affectors use the buffer the removed affector did
		for (AffectorMap::iterator start = affectors.GetMapBegin();start != affectors.GetMapEnd();++start)
		{
			if(start->second->GetDesiredBuffer() == semantic)
			{
				anotherFound = true;
				break;
			}
		}

		if(!anotherFound)//If no other affector is using the buffer, destroy it
		{
			mRenderOp.vertexData->vertexBufferBinding->unsetBinding(semantic);
			mRenderOp.vertexData->vertexDeclaration->removeElement(semantic);
		}

		affectors.affectorMap.erase(result);//remove from map if found

		return result->second; //return the affector
	}

	return NULL;
}

bool ParticleSystemBase::AssignAffectorKernel(ParticleKernel* newKernel)
{
	if(!onGPU)
		return false;

	if(cudaKernel)
	{
		delete cudaKernel;
		cudaKernel = NULL;
	}

	cudaKernel = newKernel;

	if(cudaKernel)
	{
		//Fill the kernel with the necessary data
		if(cudaKernel->PopulateData(this, &affectors.affectorMap) == ParticleKernel::SUCCESS)
		{
			return true;
		}
	}

	return false;
}

std::string ParticleSystemBase::FindBestShader()
{
	ParticleMaterialMap::MaterialMap::iterator iter = materialsMap.materialMap.find(affectors.allTypesCombination);

	if(iter != materialsMap.materialMap.end())
		return iter->second;
	else
	{
		iter = materialsMap.materialMap.find(affectors.gpuTypeCombination);
		if(iter != materialsMap.materialMap.end())
			return iter->second;
	}

	return "DefaultParticleShader";
}

