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
	friend class ParticleSystemBase;
protected:
	bool onGPU;

public:	
	ParticleAffector(bool _onGPU = false): onGPU(_onGPU){};

	virtual ~ParticleAffector();

	virtual ParticleAffectorType::ParticleAffectorType GetAffectorType() = 0;
	
	///<summary>Initializes the affector</summary>
	///<param name="contextManager">Required if performing on the GPU</param>
	///<param name="vertexData">The vertex data to add buffers to</param>
	///<param name="maxParticles">The maximum allowed particles</param>
	///<returns>True if successful, false if not. If false, invalid vertex data was most likely passed</returns>
	virtual bool Initialize(ParticleSystemBase* owningSystem) = 0;

	virtual void PreUpdate(){}

	///<summary>Used to update the specified particle index when ran on the CPU</summary>
	///<param name="gameTime">The gametime since last frame</param>
	///<param name="percentile">The percentage in decimal form through the effect (commonly lifetimes... so difference from initial lifetime to current)</param>
	///<param name="particleIndex">The index of the particle to update</param>
	virtual void Update(const float gameTime, GPUResourcePointers& pointers, const float percentile, const unsigned int particleIndex) = 0;

	virtual void PostUpdate(){}

	bool GetOnGPU()const{return onGPU;}

	virtual Ogre::VertexElementSemantic GetDesiredBuffer() = 0;

	///<summary>Creates the proper GPU Parameter structure for the derived type.
	///Also maps CUDA buffers if they are not already. Remember to call UnmapBuffers though.</summary>
	///<returns>Derived structure representing the derived affectors necessary data</returns>
	virtual GPUParticleAffectorParams* const GetGPUParamaters() = 0;

	virtual GPUParticleAffectorParams* const GetParameters() = 0;

	virtual ParticleAffector* Clone() = 0;

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
	///<param name="_minScale">The smallest scale permitted (lowest is 0.0f) (used as start scale if growing)</param>
	///<param name="_maxScale">The largest scale permitted (highest is 1.0f) (used as start scale if shrinking)</param>
	///<param name="onGPU">True to utilize CUDA, false if not. Will be overwritten 
	///if a valid Cuda Context is not given in Initialize</param>
	ScaleParticleAffector(const bool enlarge, const float _minScale, const float _maxScale, bool onGPU = false);

	~ScaleParticleAffector();

	virtual ParticleAffectorType::ParticleAffectorType GetAffectorType();

	///<summary>Gets the semantic that represents the buffer that this affector wants to place its data in</summary>
	///<returns>The Ogre Semantic of the OpenGL Buffer</returns>
	virtual Ogre::VertexElementSemantic GetDesiredBuffer(){return Ogre::VES_POSITION;}

	///<summary>Initializes the affector</summary>
	///<param name="contextManager">Required if performing on the GPU</param>
	///<param name="vertexData">The vertex data to add buffers to</param>
	///<param name="maxParticles">The maximum allowed particles</param>
	///<param name="source">The index the buffer should be bound to</param>
	///<returns>True if successful, false if not. If false, invalid vertex data was most likely passed</returns>
	virtual bool Initialize(ParticleSystemBase* owningSystem);

	///<summary>Used to update the specified particle index when ran on the CPU</summary>
	///<param name="gameTime">The gametime since last frame</param>
	///<param name="percentile">The percentage in decimal form through the effect (commonly lifetimes... so difference from initial lifetime to current)</param>
	///<param name="particleIndex">The index of the particle to update</param>
	virtual void Update(const float gameTime, GPUResourcePointers& pointers, const float percentile, const unsigned int particleIndex) ;

	virtual GPUScaleAffectorParams* const GetGPUParamaters() ;

	virtual GPUScaleAffectorParams* const GetParameters(){return &affectorAttributes;}

	virtual ParticleAffector* Clone();

};