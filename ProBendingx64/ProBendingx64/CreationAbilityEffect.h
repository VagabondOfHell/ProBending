#pragma once
#include "AbilityEffect.h"
#include "foundation\PxVec3.h"

class IScene;
class GameObject;

class CreationAbilityEffect :
	public AbilityEffect
{
protected:
	GameObject* objectToCreate; //The game object that will be created
	IScene* sceneToCreateTo; //The scene to create the object on

	physx::PxVec3 positionOfCreation; //Position to create at

public:
	CreationAbilityEffect(void);

	CreationAbilityEffect(GameObject* const _objectTocreate, const float _duration, const physx::PxVec3 creationLocation);

	virtual ~CreationAbilityEffect(void);

	///<summary>Clones the current CreationAbilityEffect into its own object</summary>
	///<returns> A deep copy of the CreationAbilityEffect</returns>
	virtual CreationAbilityEffect* Clone()
	{
		////DOES THIS NEED DEEP COPY OF OBJECTTOCREATE?!////
		return new CreationAbilityEffect(*this);
	}

	///<summary>Gets the type of effect as a string that matches the class name</summary>
	///<returns>A string matching the name of the class</returns>
	virtual inline std::string GetEffectType()
	{
		return "CreationAbilityEffect";
	}

	///<summary>Updates and applies the effect to the specified target</summary>
	///<param name="gameTime">The time that has passed between frames</param>
	///<param name="target">The Probender to apply the effect to</param>
	virtual void Update(const float gameTime, Probender* const target);
};

