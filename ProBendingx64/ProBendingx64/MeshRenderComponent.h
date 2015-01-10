#pragma once
#include "Component.h"
#include "OgreString.h"

namespace Ogre
{
	class SceneNode;
	class Entity;
};

namespace physx
{
	class PxBoxGeometry;
};

class MeshRenderComponent :
	public Component
{
private:
	
	Ogre::Entity* entity;

public:
	MeshRenderComponent();
	virtual ~MeshRenderComponent(void);

	///<summary>Loads a model for Ogre, providing a Try Catch to handle errors</summary>
	///<param name="modelFileName">The name of the model with the extension to load from Ogre resources</param>
	///<returns>True if loaded, false if failed</returns>
	bool LoadModel(const Ogre::String& modelFileName);

	///<summary>Creates a box from the ogre entity dimensions, with scaling</summary>
	///<param name="boxGeometry">The out value to be filled</param>
	///<returns>True if successful, false if not. Unsuccessful when entity has not been set</returns>
	bool ConstructBoxFromEntity(physx::PxBoxGeometry& boxGeometry)const;

	virtual void Start();

	virtual void Update(float gameTime);

	///<summary>Gets the component type of this component</summary>
	///<returns>The type of component</returns>
	virtual ComponentType GetComponentType()
	{
		return Component::MESH_RENDER_COMPONENT;
	}

};

