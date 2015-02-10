#pragma once
#include "OgreSimpleRenderable.h"
#include "ParticleSystemParams.h"
#include "AffectorParameters.h"

class AbstractParticleEmitter;
class ParticleKernel;

//Static container for particle material list. Populates itself on construction
struct ParticleMaterialMap
{
	typedef std::map<ParticleAffectorType::ParticleAffectorFlag, std::string> MaterialMap;

	MaterialMap materialMap;

	ParticleMaterialMap();
};

//Static container for particle kernel list. Populates itself on construction
struct ParticleKernelMap
{
	typedef std::map<ParticleAffectorType::ParticleAffectorFlag, std::shared_ptr<ParticleKernel>> KernelMap;

	KernelMap kernelMap;

	ParticleKernelMap();
};

///Base class shared between Fluid and Particle System to be inherited from
class FluidAndParticleBase: public Ogre::SimpleRenderable
{
protected:

#pragma region Variables
	static ParticleMaterialMap materialsMap;//Shared instance of a list of shaders
	static ParticleKernelMap kernelsMap;//Shared instance of a list of kernels

	physx::PxParticleReadDataFlags readableData; ///The readable data of the particle system

	std::shared_ptr<AbstractParticleEmitter> emitter;//The emitter used to create particles

	std::vector<physx::PxU32> availableIndices; //The available indices within the particle system

	std::vector<const physx::PxU32> indicesToRemove;//Vector of indices that should be removed this frame

	physx::PxCudaContextManager* cudaContextManager; ///The physX cuda context manager

	Ogre::HardwareVertexBufferSharedPtr mVertexBufferPosition;//The ogre GL buffer to hold the positions

	size_t maximumParticles; ///The maximum number of particles allowed in the particle system
	bool onGPU; ///Whether or not the particle system is used on the GPU
	ParticleKernel* cudaKernel; //Kernel used to update particles through cuda

	//Map of buffers organized by semantic, holding the buffer reference
	typedef std::map<Ogre::VertexElementSemantic, Ogre::HardwareVertexBufferSharedPtr> BufferMap;
	typedef std::pair<BufferMap::iterator, bool> BufferMapInsertResult;

	BufferMap bufferMap;//list of buffers organized by semantic 

	float initialLifetime;//Starting lifetime for particles
	float* lifetimes;//array of lifetimes for particles
#pragma endregion

	///<summary>Sets the read data flags as specified by the newFlags argument</summary>
	///<param name="newFlags">The new read data flags to use</param>
	void SetParticleReadFlags(physx::PxParticleBase* pxParticleSystem, physx::PxParticleReadDataFlags newFlags);

	///<summary>Sets the base flags as specified by the newFlags argument. Does not handle the GPU flag</summary>
	///<param name="newFlags">The new base flags to use</param>
	void SetParticleBaseFlags(physx::PxParticleBase* pxParticleSystem, physx::PxParticleBaseFlags newFlags);

	void SetSystemData(physx::PxParticleBase* pxParticleSystem, const ParticleSystemParams& params);

#pragma region Virtual Methods for Inherited System Customization

	///<summary>Initializes vertex buffers. Particle System Base handles position buffer. Others are up to children</summary>
	virtual void InitializeVertexBuffers();
	
	///<summary>Provides children the opportunity to initialize their custom data</summary>
	virtual void InitializeParticleSystemData(){};
	
	///<summary>Callback for when particles are created, so children can set custom data</summary>
	///<param name="createdCount>The amount of particles that were created</param>
	///<param name="emittedIndices>The indices that were used this creation frame</param>
	virtual inline void ParticlesCreated(const unsigned int createdCount, physx::PxStrideIterator<const physx::PxU32> emittedIndices)
	{
		//Loop through all the created particles and set their lifetimes
		for (unsigned int i = 0; i < createdCount; i++)
		{
			lifetimes[emittedIndices[i]] = initialLifetime;
		}
	}

	virtual GPUResourcePointers LockBuffersCPU();

	virtual inline void UnlockBuffersCPU()
	{
		for (BufferMap::iterator start = bufferMap.begin(); start != bufferMap.end(); ++start)
		{
			start->second->unlock();
		}
	}

#pragma endregion

public:
	FluidAndParticleBase(std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime,
		physx::PxCudaContextManager* _cudaMan)
		: emitter(_emitter), maximumParticles(_maximumParticles), initialLifetime(_initialLifetime), cudaContextManager(_cudaMan)
	{

	}

	virtual ~FluidAndParticleBase()
	{

	}

#pragma region Getters and Setters

	///<summary>Gets the maximum number of particles allowed to be managed by the particle system at a time</summary>
	///<returns>The maximum number of particles, as declared when constructing the particle system</returns>
	inline size_t GetMaximumParticlesAllowed()const{return maximumParticles;}

	///<summary>Gets the emitter used by the particle system</summary>
	///<returns>Pointer to the abstract particle emitter</returns>
	inline std::shared_ptr<AbstractParticleEmitter> const GetEmitter()const{return emitter;}
	
	///<summary>Gets the cuda manager associated with this particle system</summary>
	///<returns>The cuda context manager, or NULL if none set</returns>
	inline physx::PxCudaContextManager* const GetPhysXCudaManager()const{return cudaContextManager;}

	inline void SetPhysXCudaManager(physx::PxCudaContextManager* manager){cudaContextManager = manager;}

	///<summary>Sets the kernel to the specified one</summary>
	///<param name="newKernel">The new kernel to use</param>
	virtual bool AssignAffectorKernel(ParticleKernel* newKernel);
	
	///<summary>Finds the best matching shader according to the passed flag</summary>
	///<param name="combination">Combination of affector types to check for</param>
	///<returns>The name of the best matching shader, or DefaultParticleShader if not found</returns>
	static std::string FindBestShader(ParticleAffectorType::ParticleAffectorFlag combination);
	
	///<summary>Finds the kernel that matches the specified combination</summary>
	///<param name="combination">The combination of Affector Types</param>
	///<returns>The found Kernel, or NULL if none were found</returns>
	static ParticleKernel* FindBestKernel(ParticleAffectorType::ParticleAffectorFlag combination);

#pragma endregion

	///<summary>Creates a vertex buffer for this system</summary>
	///<param name="semantic">The semantic to create</param>
	///<param name="uvSource">The uv index</param>
	///<returns>The hardware vertex buffer that already exists, or the newly created one</returns>
	virtual Ogre::HardwareVertexBufferSharedPtr CreateVertexBuffer
		(Ogre::VertexElementSemantic semantic, unsigned short uvSource = 0);

	///<summary>Gets the buffer associated with the specified semantic</summary>
	///<returns>The shared pointer to the specified vertex buffer</returns>
	virtual Ogre::HardwareVertexBufferSharedPtr GetBuffer(Ogre::VertexElementSemantic semantic);

	virtual Ogre::Real getBoundingRadius(void) const
	{
		return 0;
	}

	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam)const;
};