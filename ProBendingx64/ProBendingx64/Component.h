#pragma once

class GameObject;

class Component
{
	friend class SceneSerializer;

protected:
	GameObject* owningGameObject;

	bool enabled;

	///<summary>Event method for when the component has been attached to a game object</summary>
	virtual void OnAttach(){}

public:
	enum ComponentType
	{
		AUDIO_COMPONENT,
		MESH_RENDER_COMPONENT,
		PARTICLE_COMPONENT,
		RIGID_BODY_COMPONENT
	};

	Component()
	{
		owningGameObject = 0;
		Enable();//Call the enable method, that way if its overridden by child, it enables correctly
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

	///<summary>Enables the component</summary>
	virtual void Enable(){enabled = true;}

	///<summary>Disables the component</summary>
	virtual void Disable(){enabled = false;}

	///<summary>Checks if the component is currently enabled or not</summary>
	///<returns>True if enabled, false if not</returns>
	inline bool IsEnabled()const{return enabled;}

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