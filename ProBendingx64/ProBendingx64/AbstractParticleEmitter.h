#pragma once
#include "foundation\PxVec3.h"
#include "particles\PxParticleCreationData.h"

class AbstractParticleEmitter
{
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
	float minSpeed, maxSpeed;

public:
	physx::PxVec3 position;

	///<summary>Constructor of the abstract class to fill common values</summary>
	///<param name="emitterPosition">The position of the emitter</param>
	///<param name="minEmissionDirection">The minimum values on the X, Y, Z dimensions for random velocity </param>
	///<param name="maxEmissionDirection">The maximum values on the X, Y, Z dimensions for random velocity </param>
	///<param name="minParticleSpeed">The minimum value for creating random particle speed </param>
	///<param name="maxParticleSpeed">The maximum value for creating random particle speed </param>
	AbstractParticleEmitter(physx::PxVec3 emitterPosition = physx::PxVec3(0.0f), 
		physx::PxVec3 minEmissionDirection  = physx::PxVec3(0.0f), physx::PxVec3 maxEmissionDirection  = physx::PxVec3(0.0f),
		float minParticleSpeed = 1.0f, float maxParticleSpeed = 2.0f)
	{
		position = emitterPosition;
		minimumDirection = minEmissionDirection;
		maximumDirection = maxEmissionDirection;

		ValidateMinAndMax(minimumDirection, maximumDirection);

		minSpeed = minParticleSpeed;
		maxSpeed = maxParticleSpeed;
	}

	virtual ~AbstractParticleEmitter(){}

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
};