#include "ArenaBuilder.h"

#include "IScene.h"
#include "PhysXDataManager.h"
#include "GameObject.h"
#include "MeshRenderComponent.h"
#include "RigidBodyComponent.h"

#include "OgreMeshManager.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"

void ArenaBuilder::GenerateProbendingArena(IScene* scene)
{
	MeshRenderComponent::CreatePlane("WaterPlane", Ogre::Vector3::UNIT_Y, 0.0f, 1, 1, 3.0f, 3.0f, Ogre::Vector3::UNIT_Z);

	///Create the water plane
	SharedGameObject waterObject = std::make_shared<GameObject>(scene, "WaterPlane");
	MeshRenderComponent* waterMesh = new MeshRenderComponent();
	waterObject->AttachComponent(waterMesh);

	waterMesh->LoadModel("WaterPlane");
	waterMesh->SetMaterial("Examples/Water1");
	waterObject->SetWorldPosition(0.0f, -5.0f, 0.0f);
	waterObject->SetScale(1000.0f, 1.0f, 1000.0f);
	
	waterObject->Start();

	scene->AddGameObject(waterObject);
}

