#pragma once
#include "GameObject.h"

class Component
{
protected:
	GameObject* owningGameObject;

public:

	Component(GameObject* _owningGameObject)
	{
		owningGameObject = _owningGameObject;
	}

	virtual ~Component()
	{
		
	}

	virtual void Start() = 0;

	virtual void Update(float gameTime) = 0;

	///<summary>Gets the game object that this component is attached to</summary>
	///<returns>Pointer to the game object that the component is attached to</returns>
	const GameObject* const GetGameObject()
	{
		return owningGameObject;
	}
};