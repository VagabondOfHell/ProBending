#include "MeshRenderComponent.h"

#include <stdexcept>

#include "IScene.h"
#include "GameObject.h"

#include "OgreSceneManager.h"
#include "OgreMeshManager.h"
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

bool MeshRenderComponent::CreatePlane(const std::string& planeMeshName, const Ogre::Vector3& normal, const Ogre::Real distance,
					const int xSegments, const int ySegments, const Ogre::Real uRepeat, const Ogre::Real vRepeat,
					const Ogre::Vector3& upDir)
{
	Ogre::MeshPtr plane = Ogre::MeshManager::getSingletonPtr()->getByName(planeMeshName);

	if(plane.get() == NULL)
		plane = Ogre::MeshManager::getSingletonPtr()->createPlane(planeMeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			Ogre::Plane(normal, distance), 1.0f, 1.0f, xSegments, ySegments, true, 1, uRepeat, vRepeat, upDir);
	
	if(plane.get() != NULL)
		return true;
	
	return false;
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

void MeshRenderComponent::SetMaterial(const std::string& matName)
{
	if(entity)
		entity->setMaterialName(matName);	
}

std::shared_ptr<MeshInfo> const MeshRenderComponent::GetMeshInfo() const
{
	std::shared_ptr<MeshInfo> retVal = std::make_shared<MeshInfo>(MeshInfo());
	
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

MeshRenderComponent* MeshRenderComponent::Clone(GameObject* gameObject)
{
	MeshRenderComponent* clone = new MeshRenderComponent();
	
	clone->enabled = enabled;
	clone->owningGameObject = gameObject;

	if(entity)
	{
		Ogre::NameGenerator nameGenerator = Ogre::NameGenerator(entity->getName());
		clone->LoadModel(GetMeshName());
			//entity->clone(nameGenerator.generate());
	}
	
	return clone;
}

void MeshRenderComponent::UpdateMesh(const std::vector<Ogre::Vector3>& newData, 
			unsigned int startPos, Ogre::VertexElementSemantic semantic, unsigned int submeshSource)
{
	if(!entity)
		return;

	if(newData.size() == 0)
		return;

	Ogre::MeshPtr mesh = entity->getMesh();
	Ogre::SubMesh* submesh = mesh->getSubMesh(submeshSource);
	
	Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

	const Ogre::VertexElement* posElem =
		vertex_data->vertexDeclaration->findElementBySemantic(semantic);

	//if the element wasn't found, return
	if(!posElem)
		return;

	Ogre::HardwareVertexBufferSharedPtr vbuf =
		vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

	size_t vertexCount = vbuf->getNumVertices();

	//if starting position isn't valid, return
	if(startPos >= vertexCount)
		return;

	Ogre::Vector3* positionData = (Ogre::Vector3*)vbuf->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);

	unsigned int newDataIndex = 0;

	for (unsigned int i = startPos; i < vertexCount; i++, newDataIndex++)
	{
		if(newDataIndex < newData.size())
			positionData[i] = newData[newDataIndex];
		else
			break;
	}

	vbuf->unlock();
}



