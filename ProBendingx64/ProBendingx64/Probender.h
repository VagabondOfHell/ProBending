#pragma once
#include "PxRigidDynamic.h"
#include "ProbenderInputHandler.h"
#include "ProbenderInGameData.h"
#include "ProbenderStateManager.h"

class IScene;
class Projectile;

class Probender
{
private:
	ElementEnum::Element currentElement;//The current element used

	ProbenderInGameData characterData;//The characters game stats

	ProbenderInputHandler inputHandler;//The component handling input

	physx::PxRigidDynamic* physicsBody;//The PhysX rigid body around the character

	///Flight Game Object here

	Projectile* leftHandAttack;//The projectile in the left hand
	Projectile* rightHandAttack;//The projectile in the right hand

	ProbenderStateManager stateManager;//The state manager for probenders

	Probender* currentTarget;

public:
	Probender();
	~Probender(void);

	void AttachToScene(IScene* scene);

	///<summary>Has the probender acquire a new target</summary>
	///<param name="toRight">True to look to the right for the target, false to look to the left</param>
	void AcquireNewTarget(bool toRight);

	void Update(float gameTime);
};

