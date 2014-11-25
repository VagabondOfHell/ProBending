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

	///<summary>Emission method called by a particle system</summary>
	///<param name="gameTime">The game time that has passed </param>
	///<param name="availableIndiceCount">The count of currently available indices </summary>
	///<param name="creationData">The creation data to fill. This is given from the particle system</summary>
	virtual void Emit(float gameTime, unsigned int availableIndiceCount, physx::PxParticleCreationData& creationData) = 0;
};