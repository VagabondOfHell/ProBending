#include "MeshRenderComponent.h"
#include <stdexcept>
#include "IScene.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "GameObject.h"

#include "geometry/PxBoxGeometry.h"

MeshRenderComponent::MeshRenderComponent()
	:Component()
{
}

MeshRenderComponent::~MeshRenderComponent(void)
{
	//Detach and destroy any entities
	if(entity)
	{
		owningGameObject->gameObjectNode->detachObject(entity);
		owningGameObject->GetOwningScene()->GetOgreSceneManager()->destroyEntity(entity);
		entity = NULL;
	}
}

bool MeshRenderComponent::LoadModel(const Ogre::String& modelFileName)
{
	Ogre::SceneNode* sceneNode = owningGameObject->gameObjectNode;

	if(entity)
		//Detach, modify, reattach
		sceneNode->detachObject(entity);

	///SHOULD OLD ENTITY BE DESTROYED HERE?
	try
	{
		entity = owningGameObject->GetOwningScene()->GetOgreSceneManager()->createEntity(modelFileName);
	}
	catch(Ogre::Exception e)
	{
		entity = NULL;
		return false;
	}

	if(entity)
		sceneNode->attachObject(entity);	

	return entity != NULL;
}


bool MeshRenderComponent::ConstructBoxFromEntity(physx::PxBoxGeometry& boxGeometry)const
{
	if(entity)
	{
		//Get the half size of the entity bounding box and modify by scale factor
		Ogre::Vector3 boxHalfSize = entity->getBoundingBox().getHalfSize() * owningGameObject->GetWorldScale();
		//Generate physx geometry
		boxGeometry = physx::PxBoxGeometry(physx::PxVec3(boxHalfSize.x, boxHalfSize.y, boxHalfSize.z));
		//indicate success
		return true;
	}

	return false;
}

void MeshRenderComponent::Start()
{
}

void MeshRenderComponent::Update(float gameTime)
{
}


