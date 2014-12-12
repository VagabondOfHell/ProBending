#pragma once
#include <string>
#include "Component.h"

class ParticleSystemBase;

namespace Ogre
{
	class SceneNode;
}

class ParticleComponent : public Component
{
private:
	bool useLocalSpace;

	static Ogre::SceneNode* WORLD_PARTICLES_NODE;
	static int NUM_INSTANCES;

protected:	
	

	///<summary>Creates the scene node as a child to the game objects node</summary>
	virtual void CreateSceneNode();

	///<summary>Destroys the scene node. Called by destructor at the moment</summary>
	virtual void DestroySceneNode();

public:
	Ogre::SceneNode* sceneNode;//The scene node used to position the particle component
	ParticleSystemBase* particleSystem; //The manipulator of the particles
	///<summary>Constructor of the particle component</summary>
	///<param name="_owningObject">The object that owns the component</param>
	///<param name="_particleSystem">The particle system that controls particle data</param>
	///<param name="useLocalSpace">True to use local space, false to use world space</param>
	ParticleComponent(GameObject* _owningObject, ParticleSystemBase* _particleSystem, bool useLocalSpace = true);

	virtual ~ParticleComponent(void);

	///<summary>Gets the Component Type of this component</summary>
	///<returns>The Type of this component</returns>
	virtual inline ComponentType GetComponentType(){return ComponentType::PARTICLE_COMPONENT;}

	virtual void Start();

	virtual void Update(float gameTime);

	///<summary>Sets the transformation space for the particles to use</summary>
	///<param name="_useLocalSpace">True to convert the system to Local Space, false for World Space</param>
	void SetTransformationSpace(const bool _useLocalSpace);

	///<summary>Gets the currently set Transformation space</summary>
	///<returns>A boolean representing the transformation space. True for local space, false for world space</returns>
	bool GetTransformationSpace()const{return useLocalSpace;}
};

