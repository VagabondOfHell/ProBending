#pragma once
#include "Component.h"
#include "OgreString.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "MeshInfo.h"

#include <memory>

namespace Ogre
{
	class SceneNode;
	class Entity;
};

namespace physx
{
	class PxBoxGeometry;
};

typedef std::shared_ptr<MeshInfo> SharedMeshInfo;

class MeshRenderComponent :
	public Component
{
	friend class SceneSerializer;

protected:
	Ogre::Entity* entity;

public:
	MeshRenderComponent();
	virtual ~MeshRenderComponent(void);

	///<summary>Loads a model for Ogre, providing a Try Catch to handle errors</summary>
	///<param name="modelFileName">The name of the model with the extension to load from Ogre resources</param>
	///<returns>True if loaded, false if failed</returns>
	bool LoadModel(const Ogre::String& modelFileName);

	///<summary>Creates it if it doesn't exist</summary>
	///<param name="planeMeshName">The name to search for or create under</param>
	///<param name="position">The position/normal of the plane</param>
	///<param name="distance">The distance along the position</param>
	///<param name="xSegments">Number of segments along the horizontal axis</param>
	///<param name="ySegments">Number of segments along the vertical axis</param>
	///<param name="uRepeat">Number of times to repeat the texture on the U axis</param>
	///<param name="vRepeat">Number of times to repeat the texture on the V axis</param>
	///<param name="upDir">Up direction of the plane</param>
	///<returns>True if successful, false if not</returns>
	bool static CreatePlane(const std::string& planeMeshName, const Ogre::Vector3& normal = Ogre::Vector3::UNIT_X, 
		const Ogre::Real distance = 0.0f, const int xSegments = 1, const int ySegments = 1, 
		const Ogre::Real uRepeat = 1.0f, const Ogre::Real vRepeat = 1.0f, 
		const Ogre::Vector3& upDir = Ogre::Vector3::UNIT_Y );

	///<summary>Gets the name of the entity used</summary>
	///<returns>Name of the entity. Error if no entity</returns>
	const std::string& GetMeshName()const;

	void SetMaterial(const std::string& matName);

	///<summary>Creates a box from the ogre entity dimensions, with scaling</summary>
	///<returns>Gets the half size of the rectangle representing the entity</returns>
	Ogre::Vector3 GetHalfExtents()const;

	///<summary>Gets the vertices and indices of the mesh. Expensive method, call as few times as possible</summary>
	///<returns>A shared pointer to the mesh info that was created</returns>
	std::shared_ptr<MeshInfo> const GetMeshInfo()const;

	///<summary>Enables the render component, resulting in the mesh being rendered</summary>
	virtual void Enable();

	///<summary>Disables the render component, resulting in the mesh not being rendered</summary>
	virtual void Disable();

	virtual void Start();

	virtual void Update(float gameTime);

	///<summary>Gets the component type of this component</summary>
	///<returns>The type of component</returns>
	virtual ComponentType GetComponentType()
	{
		return Component::MESH_RENDER_COMPONENT;
	}

	MeshRenderComponent* Clone(GameObject* gameObject);
};

