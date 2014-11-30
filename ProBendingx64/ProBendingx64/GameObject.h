#pragma once
#include "OgreVector3.h"
#include "IScene.h"
#include "PxPhysics.h"
#include "HelperFunctions.h"

class GameObject
{
protected:


public:
	IScene* owningScene;
	
	Ogre::SceneNode* gameObjectNode;
	Ogre::Entity* entity;

	Ogre::ManualObject* physxDebugDraw;
	Ogre::SceneNode* debugNode;

	physx::PxRigidActor* rigidBody;
	physx::PxParticleBase* particleSystem;

	enum ComponentType
	{
		AUDIOCOMPONENT
	};

	GameObject(IScene* owningScene);

	virtual ~GameObject(void);

	virtual void Start();

	virtual bool Update(float gameTime);

	void AttachComponent(ComponentType componentType);
	
	///<summary>Loads a model for Ogre, providing a Try Catch to handle errors</summary>
	///<param name="modelFileName">The name of the model with the extension to load from Ogre resources</param>
	///<returns>True if loaded, false if failed</returns>
	bool LoadModel(const Ogre::String& modelFileName);

	///<summary>Creates a box from the ogre entity dimensions, with scaling</summary>
	///<param name="boxGeometry">The out value to be filled</param>
	///<returns>True if successful, false if not. Unsuccessful when entity has not been set</returns>
	bool ConstructBoxFromEntity(physx::PxBoxGeometry& boxGeometry)const;

	///<summary>Creates an outline of all physX shapes attached to the rigid body
	///CURRENTLY ONLY SUPPORTS CUBES</summary>
	void CreatePhysXDebug();

	///<summary>Destroys and deletes the Debug component of the physx rigidbody</summary>
	void DestroyPhysXDebug();

	///<summary>Provides efficient, inlined, type casting to a Dynamic Rigid Body</summary>
	///<returns>The dynamic rigid body representation of the rigid body, or NULL if failed</returns>
	inline physx::PxRigidDynamic* GetDynamicRigidBody()
	{
		if(rigidBody->isRigidDynamic())
			return (physx::PxRigidDynamic*)rigidBody;

		return NULL;
	}

};

