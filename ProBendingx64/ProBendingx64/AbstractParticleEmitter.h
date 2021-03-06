#pragma once
#include "foundation\PxVec3.h"
#include "particles\PxParticleCreationData.h"

#include <vector>

class AbstractParticleEmitter
{
	friend class SceneSerializer;

private:
	///<summary>Helper method to ensure the minimum is the minimum and the maximum is the maximum.
	///If there is a discrepancy, the two illegal values will be swapped</summary>
	///<param name="minVal">The minimum value</param>
	///<param name="maxVal">The maximum value</param>
	void ValidateMinAndMax(physx::PxVec3& minVal, physx::PxVec3& maxVal)
	{
		///Loop through 
		for (int i = 0; i < 3; i++)
		{
			if(maxVal[i] < minVal[i])
			{
				float temp = maxVal[i];

				maxVal[i] = minVal[i];
				minVal[i] = temp;
			}
		}
	}

protected:
	physx::PxVec3 minimumDirection;
	physx::PxVec3 maximumDirection;

	std::vector<physx::PxVec3>forces;//The forces to apply to newly created particles

	float particlesPerSecond; //Amount of particles to launch per second. Can be a fractional number to take longer than a second
	float particlesToEmitThisFrame;//Number of particles to emit this frame

	float minSpeed, maxSpeed;
	float timePassed;

	unsigned int GetEmissionCount(const float gameTime, 
		const unsigned int availableIndiceCount)
	{
		unsigned int emissionCount(0);

		bool infiniteEmission = duration <= 0.0f;

		if(!infiniteEmission)
			timePassed += gameTime;

		//if time passed has not exceeded system duration or there should be infinite emission
		if((timePassed <= duration && !infiniteEmission) || (infiniteEmission))
		{
			//Check amount of particles available
			if(availableIndiceCount > 0)
			{
				//Calculate the number of particles to emit this frame
				particlesToEmitThisFrame += particlesPerSecond * gameTime;
				emissionCount = (unsigned int)physx::PxFloor(particlesToEmitThisFrame);
				particlesToEmitThisFrame -= emissionCount;

				//Gather available indices
				if(availableIndiceCount < emissionCount)
					emissionCount = availableIndiceCount;
			}
		}

		return emissionCount;
	}

public:
	enum ParticleEmitterType{NONE, POINT_EMITTER, LINE_EMITTER, SPHERE_EMITTER, MESH_EMITTER};

	physx::PxVec3 position;
	
	float duration;

	///<summary>Constructor of the abstract class to fill common values</summary>
	///<param name="emitterPosition">The position of the emitter</param>
	///<param name="minEmissionDirection">The minimum values on the X, Y, Z dimensions for random velocity </param>
	///<param name="maxEmissionDirection">The maximum values on the X, Y, Z dimensions for random velocity </param>
	///<param name="_duration">How long the emitter should emit if not looping</param>
	///<param name="minParticleSpeed">The minimum value for creating random particle speed </param>
	///<param name="maxParticleSpeed">The maximum value for creating random particle speed </param>
	AbstractParticleEmitter(physx::PxVec3 _position = physx::PxVec3(0.0f), float _particlesPerSecond = 1.0f,
		physx::PxVec3 minEmissionDirection  = physx::PxVec3(0.0f), physx::PxVec3 maxEmissionDirection  = physx::PxVec3(0.0f),
		float _duration = 1.0f, float minParticleSpeed = 1.0f, float maxParticleSpeed = 2.0f)
		: particlesPerSecond(_particlesPerSecond), position(_position), timePassed(0), 
		duration(_duration), particlesToEmitThisFrame(0)
	{
		SetDirections(minEmissionDirection, maxEmissionDirection);

		if(duration < 0.0f)
			duration = 0.0f;

		SetSpeeds(minParticleSpeed, maxParticleSpeed);
	}

	virtual ~AbstractParticleEmitter(){}

	virtual ParticleEmitterType GetEmitterType() = 0;

	///<summary>Emission method called by a particle system</summary>
	///<param name="gameTime">The game time that has passed </param>
	///<param name="availableIndiceCount">The count of currently available indices </summary>
	///<param name="creationData">The creation data to fill. This is given from the particle system</summary>
	virtual void Emit(const float gameTime, const unsigned int availableIndiceCount, physx::PxParticleCreationData& creationData) = 0;

	///<summary>Sets a new maximum direction and ensures it is not less than the min</summary>
	///<param name="newMax"> The new value for the maximum direction</param>
	void SetMaximumDirection(const physx::PxVec3 newMax)
	{
		maximumDirection = newMax;
		ValidateMinAndMax(minimumDirection, maximumDirection);
	}

	///<summary>Sets a new minimum direction and ensures it is not greater than the max</summary>
	///<param name="newMin">The new value for the minimum direction</param>
	void SetMinimumDirection(const physx::PxVec3 newMin)
	{
		minimumDirection = newMin;
		ValidateMinAndMax(minimumDirection, maximumDirection);
	}

	///<summary>Sets new values to the minimum and maximum directions and ensures that maximum is greater
	///than the minimum</summary>
	///<param name="newMin">The new minimum direction to use</param>
	///<param name="newMax">The new maximum direction to use</param>
	void SetDirections(const physx::PxVec3 newMin, const physx::PxVec3 newMax)
	{
		minimumDirection = newMin;
		maximumDirection = newMax;

		ValidateMinAndMax(minimumDirection, maximumDirection);
	}

	void SetEmissionRate(float pps){particlesPerSecond = pps;}

	///<summary>Sets new values to the minimum and maximum speeds and ensures that maximum is greater
	///than the minimum</summary>
	///<param name="newMin">The new minimum speed to use</param>
	///<param name="newMax">The new maximum speed to use</param>
	void SetSpeeds(const float newMin, const float newMax)
	{
		//If the values are reversed, assign them reverse
		if(newMin > newMax)
		{
			minSpeed = newMax;
			maxSpeed = newMin;
		}
		else
		{
			//Otherwise assign them normally
			maxSpeed = newMax;
			minSpeed = newMin;
		}
	}

	virtual AbstractParticleEmitter* Clone() = 0;

};