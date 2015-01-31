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
	using namespace physx;

	///Create the water plane
	SharedGameObject waterObject = std::make_shared<GameObject>(scene, "WaterPlane");
	MeshRenderComponent* waterMesh = new MeshRenderComponent();
	waterObject->AttachComponent(waterMesh);
	waterMesh->LoadModel("BasicPlane");
	waterMesh->SetMaterial("Examples/Water1");
	RigidBodyComponent* waterRigid = new RigidBodyComponent();
	ShapeDefinition waterShapeDef = ShapeDefinition(false);
	waterShapeDef.SetPlaneGeometry();
	waterShapeDef.AddMaterial("DefaultMaterial");
	waterObject->AttachComponent(waterRigid);
	Ogre::Quaternion rot = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90.0f)), Ogre::Vector3(0, 0, 1));
	waterRigid->CreateRigidBody(RigidBodyComponent::STATIC);//, physx::PxVec3(0.0f, -5.0f, 0.0f), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
	waterRigid->CreateAndAttachNewShape(waterShapeDef);
	//waterRigid->CreateDebugDraw();

	waterObject->SetWorldTransform(Ogre::Vector3(0.0f, -5.0f, 0.0f), rot, Ogre::Vector3(50.0f, 50.0f, 50.0f));
	waterObject->Start();

	scene->AddGameObject(waterObject);

	SharedGameObject arenaSurface = std::make_shared<GameObject>(scene, "ProbendArenaSurface");
	MeshRenderComponent* arenaSurfaceMesh = new MeshRenderComponent();
	arenaSurface->AttachComponent(arenaSurfaceMesh);
	arenaSurfaceMesh->LoadModel("ProbendArenaSurface.mesh");

	PhysXDataManager::GetSingletonPtr()->CreateMaterial(1.0f, 1.0f, 0.0f, "101000");
	RigidBodyComponent* arenaSurfaceRigid = new RigidBodyComponent();
	arenaSurface->AttachComponent(arenaSurfaceRigid);
	arenaSurfaceRigid->CreateRigidBody(RigidBodyComponent::STATIC);
	std::shared_ptr<MeshInfo> arenaSurfaceMeshInfo = arenaSurfaceMesh->GetMeshInfo();
	ShapeDefinition arenaSurfaceShapeDef = ShapeDefinition(false);
	arenaSurfaceShapeDef.SetConvexMeshGeometry(PhysXDataManager::GetSingletonPtr()->CookConvexMesh(arenaSurfaceMeshInfo, "ArenaSurfaceMesh"));
	arenaSurfaceShapeDef.AddMaterial("ProbendArenaSurface");
	arenaSurfaceRigid->CreateAndAttachNewShape(arenaSurfaceShapeDef);
	arenaSurfaceRigid->CreateDebugDraw();

	arenaSurface->Start();
	scene->AddGameObject(arenaSurface);

	//RL indicates Red Left from Red perspective
	SharedGameObject arenaPillarRL = std::make_shared<GameObject>(scene, "ProbendPillarRL");
	MeshRenderComponent* arenaPillarMesh = new MeshRenderComponent();
	arenaPillarRL->AttachComponent(arenaPillarMesh);
	arenaPillarMesh->LoadModel("ProbendPillar.mesh");

	RigidBodyComponent* arenaPillarRigid = new RigidBodyComponent();
	arenaPillarRL->AttachComponent(arenaPillarRigid);
	arenaPillarRigid->CreateRigidBody(RigidBodyComponent::STATIC);

	ShapeDefinition arenaPillarShapeDef = ShapeDefinition();
	arenaPillarShapeDef.SetBoxGeometry(HelperFunctions::OgreToPhysXVec3(arenaPillarMesh->GetHalfExtents()));
	arenaPillarRigid->CreateAndAttachNewShape(arenaPillarShapeDef);

	//Set transform as calculated by Ogitor
	arenaPillarRL->SetWorldTransform(Ogre::Vector3(5.9f, -2.85f, 2.15f), 
		Ogre::Quaternion(0.0f, -0.254602f, 0.0f, 0.967046f), Ogre::Vector3(1.0f));

	arenaPillarRL->Start();
	scene->AddGameObject(arenaPillarRL);

	SharedGameObject arenaPillarRR = arenaPillarRL->Clone(); 
	arenaPillarRR->SetWorldTransform(Ogre::Vector3(5.9f, -2.85f, -2.15f), 
		Ogre::Quaternion(0.0f, -0.254602f, 0.0f, 0.967046f), Ogre::Vector3(1.0f));

	arenaPillarRR->Start();
	scene->AddGameObject(arenaPillarRR);
}

