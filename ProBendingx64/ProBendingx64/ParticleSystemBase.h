#pragma once
#include "OgreSimpleRenderable.h"
#include "particles\PxParticleSystem.h"
#include "pxtask\PxCudaContextManager.h"
#include "ParticleAffectorEnum.h"
#include "AffectorParameters.h"
#include <vector>
#include <map>

class AbstractParticleEmitter;
class ParticleAffector;
class ParticleKernel;

struct ParticleSystemParams
{
	physx::PxReal gridSize;
	physx::PxReal maxMotionDistance;
	physx::PxReal particleMass;
	physx::PxReal staticFriction;
	physx::PxReal restitution;
	physx::PxReal contactOffset;
	physx::PxReal damping;
	physx::PxReal dynamicFriction;
	physx::PxVec3 externalAcceleration;
	physx::PxReal restOffset;
	bool useGravity;
	physx::PxParticleBaseFlags baseFlags;
	physx::PxCudaContextManager* cudaContext;
	physx::PxFilterData filterData;

	///<summary>Constructor used to set all values</summary>
	///<param name="_gridSize">The size of the grid between 0 and 1000</param>
	///<param name="_maxMotionDistance">The maximum distance the particle is allowed to travel in a frame</param>
	///<param name="_cudaContext">The Cuda Context to use for GPU acceleration. Set NULL to use CPU</param>
	///<param name="_externalAcceleration">The external acceleration to apply to all particles each frame</param>
	///<param name="_particleMass">The mass of each particle</param>
	///<param name="_useGravity">True to have particles affected by gravity, false if not</param>
	///<param name="_baseFlags">The flags to configure the system</param>
	///<param name="_restOffset">The rest offset of the particles, if enabled </param>
	///<param name="_staticFriction">The friction used for collisions. Must be non-negative</param>
	///<param name="_dynamicFriction">The dynamic friction to use. Must be non-negative</param>
	///<param name="_restitution">The bounciness of the collision, between 0.0f and 1.0f</param>
	///<param name="_contactOffset">The distance that contacts are generated between particle and geometry</param>
	///<param name="_damping">Damping for particles. Must be non-negative</param>
	///<param name="_filterData">Any filter data to be used during the simulation</param>
	ParticleSystemParams(physx::PxReal _gridSize = 1.0f, physx::PxReal _maxMotionDistance = 2.0f,
		physx::PxCudaContextManager* _cudaContext = NULL, physx::PxVec3 _externalAcceleration = physx::PxVec3(0.0f), 
		physx::PxReal _particleMass = 1.0f, bool _useGravity = true, physx::PxParticleBaseFlags _baseFlags = 
		physx::PxParticleBaseFlag::eENABLED, physx::PxReal _restOffset = 1.0f, physx::PxReal _staticFriction = 0.0f, 
		physx::PxReal _dynamicFriction = 0.0f, physx::PxReal _restitution = 1.0f, physx::PxReal _contactOffset = 0.0f, 
		physx::PxReal _damping = 0.0f, physx::PxFilterData _filterData = physx::PxFilterData())
		
		:gridSize(_gridSize), maxMotionDistance(_maxMotionDistance), cudaContext(_cudaContext),
		externalAcceleration(_externalAcceleration), 
		useGravity(_useGravity), particleMass(_particleMass), baseFlags(_baseFlags), restOffset(_restOffset), 
		staticFriction(_staticFriction), dynamicFriction(_dynamicFriction), 
		restitution(_restitution), contactOffset(_contactOffset), damping(_damping), filterData(_filterData)
	{
	}
};

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

class ParticleSystemBase: public Ogre::SimpleRenderable
{
	friend class SceneSerializer;

protected:
	static ParticleMaterialMap materialsMap;//Shared instance of a list of shaders
	static ParticleKernelMap kernelsMap;//Shared instance of a list of kernels

	std::shared_ptr<AbstractParticleEmitter> emitter;//The emitter used to create particles

	std::vector<physx::PxU32> availableIndices; //The available indices within the particle system
	
	std::vector<const physx::PxU32> indicesToRemove;//Vector of indices that should be removed this frame

	physx::PxParticleSystem* pxParticleSystem; ///The physX particle system
	physx::PxCudaContextManager* cudaContextManager; ///The physX cuda context manager

	Ogre::HardwareVertexBufferSharedPtr mVertexBufferPosition;//The ogre GL buffer to hold the positions

	size_t maximumParticles; ///The maximum number of particles allowed in the particle system
	bool onGPU; ///Whether or not the particle system is used on the GPU
	ParticleKernel* cudaKernel; //Kernel used to update particles through cuda

	physx::PxParticleReadDataFlags readableData; ///The readable data of the particle system
		
	ParticleAffectorType::ParticleAffectorFlag gpuTypeCombination; //Flag of affectors on GPU
	ParticleAffectorType::ParticleAffectorFlag allTypesCombination; //Flag of all affectors

	///<summary>Sets the read data flags as specified by the newFlags argument</summary>
	///<param name="newFlags">The new read data flags to use</param>
	void SetParticleReadFlags(physx::PxParticleReadDataFlags newFlags);

	///<summary>Sets the base flags as specified by the newFlags argument. Does not handle the GPU flag</summary>
	///<param name="newFlags">The new base flags to use</param>
	void SetParticleBaseFlags(physx::PxParticleBaseFlags newFlags);

	typedef std::map<ParticleAffectorType::ParticleAffectorType, std::shared_ptr<ParticleAffector>> AffectorMap;
	typedef std::pair<AffectorMap::iterator, bool> AffectorMapInsertResult;

	AffectorMap affectorMap;//list of affectors organized by type

	//Map of buffers organized by semantic, holding the buffer reference
	typedef std::map<Ogre::VertexElementSemantic, Ogre::HardwareVertexBufferSharedPtr> BufferMap;
	typedef std::pair<BufferMap::iterator, bool> BufferMapInsertResult;

	BufferMap bufferMap;//list of buffers organized by semantic 

	float initialLifetime;//Starting lifetime for particles
	float* lifetimes;//array of lietimes for particles

	bool hostAffector; //True if there is an affector on the CPU

#pragma region Virtual Methods for Inherited System Customization

	///<summary>Initializes vertex buffers. Particle System Base handles position buffer. Others are up to children</summary>
	virtual void InitializeVertexBuffers();

	///<summary>Provides children the opportunity to initialize their custom data</summary>
	virtual void InitializeParticleSystemData(){};

	///<summary>Callback for when particles are created, so children can set custom data</summary>
	///<param name="createdCount>The amount of particles that were created</param>
	///<param name="emittedIndices>The indices that were used this creation frame</param>
	virtual void ParticlesCreated(const unsigned int createdCount, physx::PxStrideIterator<const physx::PxU32> emittedIndices);

	virtual GPUResourcePointers LockBuffersCPU();

	virtual void UnlockBuffersCPU();

	///<summary>Called before updating to allow children to lock extra buffers, or other pre-update actions</summary>
	virtual void PreUpdateCPU(){}

	///<summary>Updates the policy. Any CPU focused updating should go here. Returns a vector of indices that represent
	///the indices that should be removed</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX</param>
	virtual void UpdateParticleSystemCPU(const float time, const physx::PxParticleReadData* const readData) ;

	///<summary>Called per particle to check if the particle should be removed or not. Children can also
	///apply removal changes here (i.e. reset value to initials)</summary>
	///<param name="particleIndex">The index of the particle queried</param>
	///<param name="readData">The PhysX particle data</param>
	///<returns>True to remove, false if valid</returns>
	virtual inline bool QueryParticleRemoval(const unsigned int particleIndex, const physx::PxParticleReadData* const readData);

	///<summary>Allows children to add additional update information on a per-particle basis. Children don't have to implement this method</summary>
	///<param name="particleIndex">The index of the current particle</param>
	///<param name="readData">Any physx particle data that may be required</param>
	virtual inline void UpdateParticle(const unsigned int particleIndex, const physx::PxParticleReadData* const readData){}

	///<summary>Called after updating to allow children to unlock extra buffers, or other post-update actions</summary>
	virtual void PostUpdateCPU(){}

	///<summary>Updates the policy. Any GPU focused updating should go here. This is only called if physX 
	///is running on the GPU</summary>
	///<param name="time">The time that has passed between frames</param>
	///<param name="readData">The particle data as provided by PhysX in a GPU format</param>
	virtual void UpdatePolicyGPU(const float time, physx::PxParticleReadData* const readData){};

#pragma endregion

public:
	ParticleSystemBase(physx::PxParticleSystem* physxParticleSystem,
		std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime);

	ParticleSystemBase(std::shared_ptr<AbstractParticleEmitter> _emitter, size_t _maximumParticles, float _initialLifetime,
		ParticleSystemParams& paramsStruct = ParticleSystemParams());

	virtual ~ParticleSystemBase(void);

	///<summary>Gets the maximum number of particles allowed to be managed by the particle system at a time</summary>
	///<returns>The maximum number of particles, as declared when constructing the particle system</returns>
	inline size_t GetMaximumParticlesAllowed()const{	return maximumParticles;}

	///<summary>Gets the emitter used by the particle system</summary>
	///<returns>Pointer to the abstract particle emitter</returns>
	inline std::shared_ptr<AbstractParticleEmitter> const GetEmitter()const{return emitter;}

	///<summary>Gets the PhysX Particle System that this class wraps</summary>
	///<returns>The physx Particle system</returns>
	inline physx::PxParticleSystem* const GetPhysXParticleSystem()const{return pxParticleSystem;}

	///<summary>Gets the cuda manager associated with this particle system</summary>
	///<returns>The cuda context manager, or NULL if none set</returns>
	inline physx::PxCudaContextManager* const GetPhysXCudaManager()const{return cudaContextManager;}

	inline void SetPhysXCudaManager(physx::PxCudaContextManager* manager){cudaContextManager = manager;}

	///<summary>Creates a vertex buffer for this system</summary>
	///<param name="semantic">The semantic to create</param>
	///<param name="uvSource">The uv index</param>
	///<returns>The hardware vertex buffer that already exists, or the newly created one</returns>
	virtual Ogre::HardwareVertexBufferSharedPtr CreateVertexBuffer
		(Ogre::VertexElementSemantic semantic, unsigned short uvSource = 0);

	///<summary>Initializes particle system and attaches to the specified scene</summary>
	///<param name="scene">The physX scene to add the particle system to</param>
	void Initialize(physx::PxScene* scene);

	///<summary>Updates the information of the particle system</summary>
	///<param name="time">The time step of the frame</param>
	void Update(float time);

	///<summary>Adds an affector to the particle system. The system claims ownership of the pointer</summary>
	///<param name="affectorToAdd">The affector to add</param>
	///<returns>True if added, false if an identical type exists (uses Affector.GetType())</returns>
	virtual bool AddAffector(std::shared_ptr<ParticleAffector> affectorToAdd);

	///<summary>Remove and deletes the affector of the specified type</summary>
	///<param name="typeToRemove">The type of affector to remove</param>
	///<returns>True if found and deleted, false if not found</returns>
	virtual bool RemoveAffector(ParticleAffectorType::ParticleAffectorType typeToRemove);

	///<summary>Removes the affector from the system and returns it, allowing it to be used elsewhere.
	///If the return value is not caught, there is a good chance for a memory leak, as particle systems
	///own affectors that are added to them</summary>
	///<param name="typeToRemove">The type of affector to remove</param>
	///<returns>The found affector, or NULL if none exist by that type in the list</returns>
	virtual std::shared_ptr<ParticleAffector> RemoveAndGetAffector(ParticleAffectorType::ParticleAffectorType typeToRemove);

	///<summary>Sets the kernel to the specified one</summary>
	///<param name="newKernel">The new kernel to use</param>
	virtual bool AssignAffectorKernel(ParticleKernel* newKernel);

	///<summary>Finds the best matching shader for the current collection of affectors</summary>
	///<returns>The name of the best matching shader, or DefaultParticleShader if not found</returns>
	virtual std::string FindBestShader();

	///<summary>Finds the best matching shader according to the passed flag</summary>
	///<param name="combination">Combination of affector types to check for</param>
	///<returns>The name of the best matching shader, or DefaultParticleShader if not found</returns>
	static std::string FindBestShader(ParticleAffectorType::ParticleAffectorFlag combination);

	///<summary>Finds the best kernel suited to the current combination of GPU affectors</summary>
	///<returns>The best suited Kernel, or NULL if none exist</returns>
	virtual ParticleKernel* FindBestKernel();

	///<summary>Finds the kernel that matches the specified combination</summary>
	///<param name="combination">The combination of Affector Types</param>
	///<returns>The found Kernel, or NULL if none were found</returns>
	static ParticleKernel* FindBestKernel(ParticleAffectorType::ParticleAffectorFlag combination);

	///<summary>Gets the buffer associated with the specified semantic</summary>
	///<returns>The shared pointer to the specified vertex buffer</returns>
	virtual Ogre::HardwareVertexBufferSharedPtr GetBuffer(Ogre::VertexElementSemantic semantic);

	virtual Ogre::Real getBoundingRadius(void) const
	{
		return 0;
	}

	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam)const;
};

