#pragma once
#include "AffectorParameters.h"
#include "particles/PxParticleReadData.h"
#include "OgreHardwareVertexBuffer.h"

class CudaGPUData;

namespace physx
{
	class PxCudaContextManager;
};

class ParticleAffector
{
protected:
	bool onGPU;
	unsigned int maxParticlesAllowed;
	unsigned int bindingIndex;//The index of the buffer this affector bound itself to

	Ogre::HardwareVertexBufferSharedPtr vertexBuffer;//The GL vertex buffer to hold the data
	CudaGPUData* gpuData;//Used as pointer so it can be NULL if not on GPU

	///<summary>Initializes the vertex buffer data</summary>
	///<param name="vertexData">The vertex data that holds the bindings and declarations</param>
	///<param name="semantic">What to create the buffer as</param>
	///<param name="type">The data the buffer will hold</param>
	///<param name="source">The index to place the buffer in the vertex data at</param>
	///<param name="extraIndex">The additional index for UV coordinates</param>
	///<returns>True if successful, false if not. If false, invalid vertex data was passed most likely</summary>
	virtual bool CreateVertexBuffers(Ogre::VertexData* vertexData, Ogre::VertexElementSemantic semantic, 
		Ogre::VertexElementType type, const unsigned int source, const unsigned int extraIndex = 0);

public:	
	ParticleAffector(bool _onGPU = false):maxParticlesAllowed(0), onGPU(_onGPU){gpuData = NULL;};

	virtual ~ParticleAffector();

	virtual ParticleAffectorType::ParticleAffectorType GetAffectorType() = 0;
	
	///<summary>Initializes the affector</summary>
	///<param name="contextManager">Required if performing on the GPU</param>
	///<param name="vertexData">The vertex data to add buffers to</param>
	///<param name="maxParticles">The maximum allowed particles</param>
	///<param name="source">The index the buffer should be bound to</param>
	///<returns>True if successful, false if not. If false, invalid vertex data was most likely passed</returns>
	virtual bool Initialize(physx::PxCudaContextManager* const contextManager, 
		Ogre::VertexData* vertexData, const unsigned int maxParticles, const unsigned int source) = 0;

	virtual void LockBuffers() = 0;

	///<summary>Used to update the specified particle index when ran on the CPU</summary>
	///<param name="gameTime">The gametime since last frame</param>
	///<param name="percentile">The percentage in decimal form through the effect (commonly lifetimes... so difference from initial lifetime to current)</param>
	///<param name="particleIndex">The index of the particle to update</param>
	virtual void Update(const float gameTime, const float percentile, const unsigned int particleIndex) = 0;

	virtual void UnlockBuffers() = 0;

	virtual void MapCudaBuffers() = 0;

	bool GetOnGPU()const{return onGPU;}

	const unsigned int GetBindingIndex()const{return bindingIndex;}

	///<summary>Creates the proper GPU Parameter structure for the derived type.
	///Also maps CUDA buffers if they are not already. Remember to call UnmapBuffers though.</summary>
	///<returns>Derived structure representing the derived affectors necessary data</returns>
	virtual GPUParticleAffectorParams* const GetGPUParamaters() = 0;

	virtual void UnmapCudaBuffers() = 0;
};

class ScaleParticleAffector : public ParticleAffector
{
protected:	
	//Used to return a GPU friendly data structure as well as to store
	//the attributes of the affector. The Scales variable holds the 
	//CUdeviceptr after GetGPUParams is called, or holds the locked
	//vertex buffer pointer when LockBuffer is called
	GPUScaleAffectorParams affectorAttributes;

public:
	ScaleParticleAffector();

	///<summary>Constructor of the Particle Scaling Affector</summary>
	///<param name="_onGPU">True if on GPU, false if not</param>
	///<param name="enlarge">True to grow, false to shrink</param>
	///<param name="_minScale">The smallest scale permitted (used as start scale if growing)</param>
	///<param name="_maxScale">The largest scale permitted (used as start scale if shrinking)</param>
	///<param name="onGPU">True to utilize CUDA, false if not. Will be overwritten 
	///if a valid Cuda Context is not given in Initialize</param>
	ScaleParticleAffector(const bool enlarge, const float _minScale, const float _maxScale, bool onGPU = false);

	~ScaleParticleAffector();

	virtual ParticleAffectorType::ParticleAffectorType GetAffectorType();


	///<summary>Initializes the affector</summary>
	///<param name="contextManager">Required if performing on the GPU</param>
	///<param name="vertexData">The vertex data to add buffers to</param>
	///<param name="maxParticles">The maximum allowed particles</param>
	///<param name="source">The index the buffer should be bound to</param>
	///<returns>True if successful, false if not. If false, invalid vertex data was most likely passed</returns>
	virtual bool Initialize(physx::PxCudaContextManager* const contextManager, 
		Ogre::VertexData* vertexData, const unsigned int maxParticles, const unsigned int source);

	virtual void LockBuffers() ;

	///<summary>Used to update the specified particle index when ran on the CPU</summary>
	///<param name="gameTime">The gametime since last frame</param>
	///<param name="percentile">The percentage in decimal form through the effect (commonly lifetimes... so difference from initial lifetime to current)</param>
	///<param name="particleIndex">The index of the particle to update</param>
	virtual void Update(const float gameTime, const float percentile, const unsigned int particleIndex) ;

	virtual void UnlockBuffers();

	virtual void MapCudaBuffers();

	virtual GPUScaleAffectorParams* const GetGPUParamaters() ;

	virtual void UnmapCudaBuffers();
};