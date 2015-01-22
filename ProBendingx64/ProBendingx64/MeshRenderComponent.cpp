#include "MeshRenderComponent.h"

#include <stdexcept>

#include "IScene.h"
#include "GameObject.h"

#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreMesh.h"
#include "OgreSubMesh.h"

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

const std::string& MeshRenderComponent::GetMeshName()const
{
	return entity->getMesh()->getName();
}

Ogre::Vector3 MeshRenderComponent::GetHalfExtents()const
{
	if(entity)
		//Get the half size of the entity bounding box and modify by scale factor
		return entity->getBoundingBox().getHalfSize() * owningGameObject->GetWorldScale();

	return Ogre::Vector3(0.0f);
}

std::shared_ptr<MeshInfo> const MeshRenderComponent::GetMeshInfo() const
{
	std::shared_ptr<MeshInfo> retVal(new MeshInfo());
	
	HelperFunctions::GetMeshInformation(entity->getMesh().getPointer(), *retVal, owningGameObject->GetWorldPosition(), 
		owningGameObject->GetWorldOrientation(), owningGameObject->GetWorldScale());
	
	return retVal;
}

void MeshRenderComponent::Enable()
{
	if(!enabled)
		owningGameObject->gameObjectNode->attachObject(entity);

	enabled = true;
}

void MeshRenderComponent::Disable()
{
	if(enabled)
		owningGameObject->gameObjectNode->detachObject(entity);
	
	enabled = false;
}

void MeshRenderComponent::Start()
{
}

void MeshRenderComponent::Update(float gameTime)
{
}



