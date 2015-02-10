#include "FluidAndParticleBase.h"

#include "ParticleKernel.h"

#include "OgreCamera.h"
#include "OgreHardwareBufferManager.h"

using namespace physx;

ParticleMaterialMap FluidAndParticleBase::materialsMap = ParticleMaterialMap();

ParticleKernelMap FluidAndParticleBase::kernelsMap = ParticleKernelMap();

//Constructor for material map
ParticleMaterialMap::ParticleMaterialMap()
{
	materialMap.insert(MaterialMap::value_type(ParticleAffectorType::None, "DefaultParticleShader"));
	materialMap.insert(MaterialMap::value_type(ParticleAffectorType::Scale, "ScaleParticleShader"));
	materialMap.insert(MaterialMap::value_type(ParticleAffectorType::ColourToColour, "ColorParticleShader"));
	materialMap.insert(MaterialMap::value_type(ParticleAffectorType::Scale | ParticleAffectorType::ColourToColour, "ColorParticleShader"));
}

//Constructor for Kernel Map
ParticleKernelMap::ParticleKernelMap()
{
	std::shared_ptr<ParticleKernel>mainKernel = std::shared_ptr<ParticleKernel>(new ParticleKernel());
	kernelMap.insert(KernelMap::value_type(ParticleAffectorType::None, mainKernel));
	kernelMap.insert(KernelMap::value_type(ParticleAffectorType::Scale, mainKernel));
	kernelMap.insert(KernelMap::value_type(ParticleAffectorType::ColourToColour, mainKernel));
	kernelMap.insert(KernelMap::value_type(ParticleAffectorType::Scale | ParticleAffectorType::ColourToColour, mainKernel));
}

void FluidAndParticleBase::SetParticleReadFlags(physx::PxParticleBase* pxParticleSystem, physx::PxParticleReadDataFlags newFlags)
{
	using namespace physx;

	//Set all the flags as indicated by the newFlags variable
	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::ePOSITION_BUFFER, 
		newFlags & PxParticleReadDataFlag::ePOSITION_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eVELOCITY_BUFFER, 
		newFlags & PxParticleReadDataFlag::eVELOCITY_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eREST_OFFSET_BUFFER, 
		newFlags & PxParticleReadDataFlag::eREST_OFFSET_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eFLAGS_BUFFER, 
		newFlags & PxParticleReadDataFlag::eFLAGS_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eCOLLISION_NORMAL_BUFFER, 
		newFlags & PxParticleReadDataFlag::eCOLLISION_NORMAL_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eCOLLISION_VELOCITY_BUFFER, 
		newFlags & PxParticleReadDataFlag::eCOLLISION_VELOCITY_BUFFER);

	pxParticleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eDENSITY_BUFFER, 
		newFlags & PxParticleReadDataFlag::eDENSITY_BUFFER);

	//Set the flags to the current data
	readableData = newFlags;
}

void FluidAndParticleBase::SetParticleBaseFlags(physx::PxParticleBase* pxParticleSystem, physx::PxParticleBaseFlags newFlags)
{
	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eCOLLISION_TWOWAY, 
		newFlags & PxParticleBaseFlag::eCOLLISION_TWOWAY);

	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eCOLLISION_WITH_DYNAMIC_ACTORS, 
		newFlags & PxParticleBaseFlag::eCOLLISION_WITH_DYNAMIC_ACTORS);

	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::eENABLED, 
		newFlags & PxParticleBaseFlag::eENABLED);

	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::ePER_PARTICLE_COLLISION_CACHE_HINT, 
		newFlags & PxParticleBaseFlag::ePER_PARTICLE_COLLISION_CACHE_HINT);

	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::ePER_PARTICLE_REST_OFFSET, 
		newFlags & PxParticleBaseFlag::ePER_PARTICLE_REST_OFFSET);

	pxParticleSystem->setParticleBaseFlag(physx::PxParticleBaseFlag::ePROJECT_TO_PLANE, 
		newFlags & PxParticleBaseFlag::ePROJECT_TO_PLANE);
}

void FluidAndParticleBase::SetSystemData(physx::PxParticleBase* pxParticleSystem, const ParticleSystemParams& paramsStruct)
{
	//Set parameters
	pxParticleSystem->setGridSize(paramsStruct.gridSize);
	pxParticleSystem->setMaxMotionDistance(paramsStruct.maxMotionDistance);
	pxParticleSystem->setParticleMass(paramsStruct.particleMass);
	pxParticleSystem->setRestitution(paramsStruct.restitution);
	pxParticleSystem->setStaticFriction(paramsStruct.staticFriction);
	pxParticleSystem->setContactOffset(paramsStruct.contactOffset);
	pxParticleSystem->setDamping(paramsStruct.damping);
	pxParticleSystem->setDynamicFriction(paramsStruct.dynamicFriction);
	pxParticleSystem->setExternalAcceleration(paramsStruct.externalAcceleration);
	pxParticleSystem->setRestOffset(paramsStruct.restOffset);
	pxParticleSystem->setSimulationFilterData(paramsStruct.filterData);
}

Ogre::HardwareVertexBufferSharedPtr FluidAndParticleBase::CreateVertexBuffer(Ogre::VertexElementSemantic semantic, unsigned short uvSource)
{
	Ogre::HardwareVertexBufferSharedPtr returnVal = GetBuffer(semantic);
	physx::PxVec4* lockedData;

	//If it already exists, return it
	if(!returnVal.isNull())
		return returnVal;

	bool success = false;

	switch (semantic)
	{
	case Ogre::VES_POSITION:
	case Ogre::VES_BLEND_WEIGHTS:
	case Ogre::VES_BLEND_INDICES:
	case Ogre::VES_DIFFUSE:
	case Ogre::VES_SPECULAR:
		//Bind to the underlying semantic value
		mRenderOp.vertexData->vertexDeclaration->addElement(semantic, 0, Ogre::VET_FLOAT4, semantic);

		returnVal = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4),
			maximumParticles,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
			false);

		success = true;

		lockedData = (physx::PxVec4*)returnVal->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
		for (int i = 0; i < maximumParticles; i++)
		{
			lockedData[i].x = 0.0f;
			lockedData[i].y = 0.0f;
			lockedData[i].z = 0.0f;
			lockedData[i].w = 1.0f;
		}
		returnVal->unlock();
		break;

	case Ogre::VES_BINORMAL:
	case Ogre::VES_TANGENT:
	case Ogre::VES_NORMAL:
		//Bind to the underlying semantic value
		mRenderOp.vertexData->vertexDeclaration->addElement(semantic, 0, Ogre::VET_FLOAT3, semantic);

		returnVal = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3),
			maximumParticles,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
			false);

		success = true;
		break;

	case Ogre::VES_TEXTURE_COORDINATES:
		//Bind to the underlying semantic value
		mRenderOp.vertexData->vertexDeclaration->addElement(semantic, 0, Ogre::VET_FLOAT4, semantic, uvSource);

		returnVal = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4),
			maximumParticles,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
			false);

		success = true;
		break;


	default:
		break;
	}

	if(success)
	{
		BufferMapInsertResult result = bufferMap.insert(BufferMap::value_type(semantic, returnVal));

		if(result.second = true)
		{
			//Set the binding to the semantic value
			mRenderOp.vertexData->vertexBufferBinding->setBinding(semantic, returnVal);
			return returnVal;
		}
	}

	return Ogre::HardwareVertexBufferSharedPtr(NULL);
}

void FluidAndParticleBase::InitializeVertexBuffers()
{
	using namespace physx;

	// allocate the vertex buffer
	mVertexBufferPosition = CreateVertexBuffer(Ogre::VES_POSITION);

	float* positions = (float*)(mVertexBufferPosition->lock(Ogre::HardwareBuffer::LockOptions::HBL_NORMAL));
	for (unsigned int i = 0; i < maximumParticles; i++)
	{
		positions[i] = std::numeric_limits<float>::quiet_NaN();
	}
	mVertexBufferPosition->unlock();

	//Set the vertex count to the maximum allowed particles in case we are running on the CPU
	//Otherwise GPU update will override
	//Eventually change this so CPU only tries to draw the number of valid particles, similar to GPU
	mRenderOp.vertexData->vertexCount = maximumParticles;
}

Ogre::HardwareVertexBufferSharedPtr FluidAndParticleBase::GetBuffer(Ogre::VertexElementSemantic semantic)
{
	BufferMap::iterator iter = bufferMap.find(semantic);

	if(iter != bufferMap.end())
		return iter->second;
	else
		return Ogre::HardwareVertexBufferSharedPtr(NULL);
}

#pragma region Virtual Methods for Inherited System Customization

//void FluidAndParticleBase::UnlockBuffersCPU()
//{
//	for (BufferMap::iterator start = bufferMap.begin(); start != bufferMap.end(); ++start)
//	{
//		start->second->unlock();
//	}
//}

GPUResourcePointers FluidAndParticleBase::LockBuffersCPU()
{
	GPUResourcePointers pointers;

	for (BufferMap::iterator start = bufferMap.begin(); start != bufferMap.end(); ++start)
	{
		switch (start->first)
		{
		case Ogre::VES_POSITION:
			pointers.positions = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_BLEND_WEIGHTS:
			pointers.blendWeights = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_BLEND_INDICES:
			pointers.blendIndices = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_DIFFUSE:
			pointers.primaryColour = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_SPECULAR:
			pointers.secondaryColour = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_TEXTURE_COORDINATES:
			pointers.uv0 = (physx::PxVec4*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;

		case Ogre::VES_BINORMAL:
			pointers.binormals = (physx::PxVec3*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_TANGENT:
			pointers.tangent = (physx::PxVec3*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;
		case Ogre::VES_NORMAL:
			pointers.normals = (physx::PxVec3*)start->second->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			break;

		default:
			break;
		}
	}

	return pointers;
}

#pragma endregion

bool FluidAndParticleBase::AssignAffectorKernel(ParticleKernel* newKernel)
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
		if(cudaKernel->PopulateData(this, NULL) == ParticleKernel::SUCCESS)
		{
			return true;
		}
	}

	return false;
}

std::string FluidAndParticleBase::FindBestShader(ParticleAffectorType::ParticleAffectorFlag combination)
{
	ParticleMaterialMap::MaterialMap::iterator iter = materialsMap.materialMap.find(combination);

	if(iter != materialsMap.materialMap.end())
		return iter->second;

	return "DefaultParticleShader";
}

ParticleKernel* FluidAndParticleBase::FindBestKernel(ParticleAffectorType::ParticleAffectorFlag combination)
{
	ParticleKernelMap::KernelMap::iterator iter = kernelsMap.kernelMap.find(combination);

	if(iter != kernelsMap.kernelMap.end())
		return iter->second->Clone();

	return NULL;
}

Ogre::Real FluidAndParticleBase::getSquaredViewDepth(const Ogre::Camera* cam)const
{
	Ogre::Vector3 min, max, mid, dist;
	min = mBox.getMinimum();
	max = mBox.getMaximum();
	mid = ((max - min) * 0.5) + min;
	dist = cam->getDerivedPosition() - mid;
	return dist.squaredLength();
}
