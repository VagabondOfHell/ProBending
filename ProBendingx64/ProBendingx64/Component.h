#pragma once

class GameObject;

class Component
{
protected:
	GameObject* owningGameObject;

	///<summary>Event method for when the component has been attached to a game object</summary>
	virtual void OnAttach(){}

public:
	enum ComponentType
	{
		AUDIO_COMPONENT,
		MESH_RENDER_COMPONENT,
		PARTICLE_COMPONENT,
		RIGID_BODY_COMPONENT,
		COLLIDER_COMPONENT
	};

	Component()
	{
		owningGameObject = 0;
	}

	virtual ~Component()
	{
		
	}

	///<summary>Attaches the component to the specified game object</summary>
	///<param name="gameObject">The game object to attach to. Does not NULL-check</param>
	inline void AttachToObject(GameObject* gameObject)
	{
		owningGameObject = gameObject;

		OnAttach();
	}

	

	virtual void Start() = 0;

	virtual void Update(float gameTime) = 0;

	///<summary>Gets the Component Type of this component</summary>
	///<returns>The Type of this component</returns>
	virtual inline ComponentType GetComponentType() = 0;

	///<summary>Gets the game object that this component is attached to</summary>
	///<returns>Pointer to the game object that the component is attached to</returns>
	const GameObject* const GetGameObject()
	{
		return owningGameObject;
	}
};