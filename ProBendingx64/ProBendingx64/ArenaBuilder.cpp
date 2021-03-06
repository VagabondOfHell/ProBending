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

#pragma region Water
	///Create the water plane
	SharedGameObject waterObject = std::make_shared<GameObject>(scene, "WaterPlane");
	MeshRenderComponent* waterMesh = new MeshRenderComponent();
	waterObject->AttachComponent(waterMesh);
	waterMesh->LoadModel("BasicPlane");
	waterMesh->SetMaterial("Examples/Water1");
	RigidBodyComponent* waterRigid = new RigidBodyComponent();
	
	waterObject->AttachComponent(waterRigid);
	Ogre::Quaternion rot = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90.0f)), Ogre::Vector3(0, 0, 1));
	waterRigid->CreateRigidBody(RigidBodyComponent::STATIC);//, physx::PxVec3(0.0f, -5.0f, 0.0f), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
	waterRigid->AttachShape("BasicPlane");
	//waterRigid->CreateDebugDraw();

	waterObject->SetWorldTransform(Ogre::Vector3(0.0f, -5.0f, 0.0f), rot, Ogre::Vector3(50.0f, 50.0f, 50.0f));
	waterObject->Start();

	scene->AddGameObject(waterObject);

#pragma endregion

#pragma region ArenaSurface
	SharedGameObject arenaSurface = std::make_shared<GameObject>(scene, "ProbendArenaSurface");
	MeshRenderComponent* arenaSurfaceMesh = new MeshRenderComponent();
	arenaSurface->AttachComponent(arenaSurfaceMesh);
	arenaSurfaceMesh->LoadModel("ProbendArenaSurface.mesh");

	RigidBodyComponent* arenaSurfaceRigid = new RigidBodyComponent();
	arenaSurface->AttachComponent(arenaSurfaceRigid);
	arenaSurfaceRigid->CreateRigidBody(RigidBodyComponent::STATIC);
	arenaSurfaceRigid->AttachShape("ArenaSurfaceShape");
	arenaSurfaceRigid->CreateDebugDraw();

	arenaSurface->Start();
	scene->AddGameObject(arenaSurface);

#pragma endregion

#pragma region Pillars
	//////////////////////PILLARS//////////////////////

	//RL indicates Red Left from Red perspective
	SharedGameObject arenaPillarRL = std::make_shared<GameObject>(scene, "ProbendPillarRL");
	MeshRenderComponent* arenaPillarMesh = new MeshRenderComponent();
	arenaPillarRL->AttachComponent(arenaPillarMesh);
	arenaPillarMesh->LoadModel("ProbendPillar.mesh");

	RigidBodyComponent* arenaPillarRigid = new RigidBodyComponent();
	arenaPillarRL->AttachComponent(arenaPillarRigid);
	arenaPillarRigid->CreateRigidBody(RigidBodyComponent::STATIC);

	arenaPillarRigid->AttachShape("ArenaPillarShape");
	arenaPillarRigid->CreateDebugDraw();

	//Set transform as calculated by Ogitor
	arenaPillarRL->SetWorldTransform(Ogre::Vector3(5.9f, -2.85f, 2.15f), 
		Ogre::Quaternion(0.0f, -0.254602f, 0.0f, 0.967046f), Ogre::Vector3(1.0f));

	arenaPillarRL->Start();
	scene->AddGameObject(arenaPillarRL);

	SharedGameObject arenaPillarRR = arenaPillarRL->Clone(); 
	arenaPillarRR->SetWorldTransform(Ogre::Vector3(5.9f, -2.85f, -2.15f), 
		Ogre::Quaternion(0.0f, 0.258819f, 0.0f, 0.967046f), Ogre::Vector3(1.0f));

	arenaPillarRR->Start();
	scene->AddGameObject(arenaPillarRR);

	SharedGameObject arenaPillarCR = arenaPillarRL->Clone();
	arenaPillarCR->SetWorldTransform(Ogre::Vector3(0.0f, -2.85f, -4.3f), 
		Ogre::Quaternion(0.0f, -0.700909f, 0.0f, 0.71325f), Ogre::Vector3(1.0f));

	arenaPillarCR->Start();
	scene->AddGameObject(arenaPillarCR);

	SharedGameObject arenaPillarCL = arenaPillarRL->Clone();
	arenaPillarCL->SetWorldTransform(Ogre::Vector3(0.0f, -2.85f, 4.3f), 
		Ogre::Quaternion(0.0f, -0.700909f, 0.0f, 0.71325f), Ogre::Vector3(1.0f));

	arenaPillarCL->Start();
	scene->AddGameObject(arenaPillarCL);

	SharedGameObject arenaPillarBR = arenaPillarRL->Clone();
	arenaPillarBR->SetWorldTransform(Ogre::Vector3(-5.9f, -2.85f, -2.15f), 
		Ogre::Quaternion(0.0f, -0.258819f, 0.0f, 0.965926f), Ogre::Vector3(1.0f));

	arenaPillarBR->Start();
	scene->AddGameObject(arenaPillarBR);

	SharedGameObject arenaPillarBL = arenaPillarRL->Clone();
	arenaPillarBL->SetWorldTransform(Ogre::Vector3(-5.9f, -2.85f, 2.15f), 
		Ogre::Quaternion(0.0f, 0.258819f, 0.0f, 0.965926f), Ogre::Vector3(1.0f));

	arenaPillarBL->Start();
	scene->AddGameObject(arenaPillarBL);

	///////////////////END PILLARS///////////////////
#pragma endregion

#pragma region Walls
	
	SharedGameObject leftWall = std::make_shared<GameObject>(scene, "Wall");
	MeshRenderComponent* wallMesh = new MeshRenderComponent();
	leftWall->AttachComponent(wallMesh);
	wallMesh->LoadModel("BasicPlane");
	wallMesh->SetMaterial("StadiumWall");
	Ogre::Radian ninetyDeg = Ogre::Radian(Ogre::Degree(90.0f));
	Ogre::Quaternion leftWallRot = Ogre::Quaternion(ninetyDeg, Ogre::Vector3::UNIT_Y);
	RigidBodyComponent* wallRigid = new RigidBodyComponent();
	leftWall->AttachComponent(wallRigid);
	wallRigid->CreateRigidBody(RigidBodyComponent::STATIC);
	wallRigid->AttachShape("ArenaWallShape1");
	wallRigid->CreateDebugDraw();
	leftWall->SetWorldTransform(Ogre::Vector3(0.0f, 0.0f, 20.0f),leftWallRot, Ogre::Vector3(10.0f, 10.0f, 7.5f));

	leftWall->Start();
	scene->AddGameObject(leftWall);

#pragma endregion
}

void ArenaBuilder::CreateProbendingPhysXData(IScene* scene)
{
	PhysXDataManager* dataMan = PhysXDataManager::GetSingletonPtr();

	dataMan->CreateMaterial(1.0f, 1.0f, 0.0f, "101000");
	dataMan->CreateMaterial(0.5f, 0.5f, 0.5f, "050505");

	SharedGameObject temp = std::make_shared<GameObject>(scene);
	//Water PhysX Data
	ShapeDefinition waterShapeDef = ShapeDefinition(true);
	waterShapeDef.SetPlaneGeometry();
	waterShapeDef.AddMaterial("050505");
	waterShapeDef.SetFilterFlags(ArenaData::WATER);
	dataMan->CreateShape(waterShapeDef, "BasicPlane");

	//Arena Surface PhysX Data
	MeshRenderComponent* arenaSurfaceMesh = new MeshRenderComponent();
	temp->AttachComponent(arenaSurfaceMesh);
	arenaSurfaceMesh->LoadModel("ProbendArenaSurface.mesh");
	std::shared_ptr<MeshInfo> arenaSurfaceMeshInfo = arenaSurfaceMesh->GetMeshInfo();
	ShapeDefinition arenaSurfaceShapeDef = ShapeDefinition(false);
	arenaSurfaceShapeDef.SetConvexMeshGeometry(dataMan->CookConvexMesh(arenaSurfaceMeshInfo, "ArenaSurfaceMesh"),
		physx::PxVec3(2.0f, 1.0f, 2.0f));
	arenaSurfaceShapeDef.AddMaterial("101000");
	arenaSurfaceShapeDef.SetFilterFlags(ArenaData::ARENA_SURFACE);
	dataMan->CreateShape(arenaSurfaceShapeDef, "ArenaSurfaceShape");

	//Arena Pillars PhysX Data
	MeshRenderComponent* arenaPillarMesh =new MeshRenderComponent();
	temp->AttachComponent(arenaPillarMesh);
	arenaPillarMesh->LoadModel("ProbendPillar.mesh");
	ShapeDefinition arenaPillarShapeDef = ShapeDefinition();
	arenaPillarShapeDef.SetBoxGeometry(HelperFunctions::OgreToPhysXVec3(arenaPillarMesh->GetHalfExtents()));
	arenaPillarShapeDef.AddMaterial("101000");
	arenaPillarShapeDef.SetFilterFlags(ArenaData::PILLAR);
	dataMan->CreateShape(arenaPillarShapeDef, "ArenaPillarShape");

	//Arena Walls PhysX Data
	MeshRenderComponent* arenaWallMesh =new MeshRenderComponent();
	temp->AttachComponent(arenaWallMesh);
	arenaWallMesh->LoadModel("BasicPlane");
	Ogre::Vector3 originalPlaneSize = arenaWallMesh->GetHalfExtents();

	temp->SetScale(1.0f, 10.0f, 12.5f);
	Ogre::Vector3 planeSize = arenaWallMesh->GetHalfExtents();
	ShapeDefinition arenaWallShapeDef = ShapeDefinition();
	arenaWallShapeDef.SetBoxGeometry(physx::PxVec3(0.2f, planeSize.y, planeSize.z));
	arenaWallShapeDef.AddMaterial("101000");
	arenaWallShapeDef.SetFilterFlags(ArenaData::WALL);
	dataMan->CreateShape(arenaWallShapeDef, "ArenaWallShapeMedium");

	temp->SetScale(1.0f, 10.0f, 19.0f);
	planeSize = arenaWallMesh->GetHalfExtents();
	arenaWallShapeDef = ShapeDefinition();
	arenaWallShapeDef.SetBoxGeometry(physx::PxVec3(0.2f, planeSize.y, planeSize.z));
	arenaWallShapeDef.AddMaterial("101000");
	arenaWallShapeDef.SetFilterFlags(ArenaData::WALL);
	dataMan->CreateShape(arenaWallShapeDef, "ArenaWallShapeLarge");

	temp->SetScale(1.0f, 10.0f, 10.0f);
	planeSize = arenaWallMesh->GetHalfExtents();
	arenaWallShapeDef = ShapeDefinition();
	arenaWallShapeDef.SetBoxGeometry(physx::PxVec3(0.2f, planeSize.y, planeSize.z));
	arenaWallShapeDef.AddMaterial("101000");
	arenaWallShapeDef.SetFilterFlags(ArenaData::WALL);
	dataMan->CreateShape(arenaWallShapeDef, "ArenaWallShapeSmall");

	MeshRenderComponent* lowerPlatformMesh = new MeshRenderComponent();
	temp->AttachComponent(lowerPlatformMesh);
	temp->SetScale(2.0f, 1.0f, 1.0f);
	lowerPlatformMesh->LoadModel("ProbendLowerPlatform.mesh");	
	ShapeDefinition lowerPlatformShape = ShapeDefinition();
	lowerPlatformShape.SetBoxGeometry(HelperFunctions::OgreToPhysXVec3(lowerPlatformMesh->GetHalfExtents()));
	lowerPlatformShape.AddMaterial("101000");
	lowerPlatformShape.SetFilterFlags(ArenaData::PLATFORM);
	dataMan->CreateShape(lowerPlatformShape, "ArenaLowerPlatformShape");

	temp->SetScale(1.0f, 1.0f, 1.0f);

	//Zone Trigger Shapes 
	//Zone 1 Pieces
	MeshRenderComponent* z1p1 = new MeshRenderComponent();
	temp->AttachComponent(z1p1);
	z1p1->LoadModel("ProbendArenaZone1P1.mesh");
	std::shared_ptr<MeshInfo> z1p1MeshInfo = z1p1->GetMeshInfo();
	ShapeDefinition z1p1ShapeDef = ShapeDefinition(true);
	z1p1ShapeDef.SetConvexMeshGeometry(dataMan->CookConvexMesh(z1p1MeshInfo, "Z1P1Mesh"),
		physx::PxVec3(2.0f, 8.0f, 2.0f));
	z1p1ShapeDef.AddMaterial("101000");
	z1p1ShapeDef.SetFilterFlags(ArenaData::ZONE_TRIGGER);
	dataMan->CreateShape(z1p1ShapeDef, "Z1P1Shape");

	MeshRenderComponent* z1p2 = new MeshRenderComponent();
	temp->AttachComponent(z1p2);
	z1p2->LoadModel("ProbendArenaZone1P2.mesh");
	std::shared_ptr<MeshInfo> z1p2MeshInfo = z1p2->GetMeshInfo();
	ShapeDefinition z1p2ShapeDef = ShapeDefinition(true);
	z1p2ShapeDef.SetConvexMeshGeometry(dataMan->CookConvexMesh(z1p2MeshInfo, "Z1P2Mesh"),
		physx::PxVec3(2.0f, 8.0f, 2.0f));
	z1p2ShapeDef.AddMaterial("101000");
	z1p2ShapeDef.SetFilterFlags(ArenaData::ZONE_TRIGGER);
	dataMan->CreateShape(z1p2ShapeDef, "Z1P2Shape");

	//Zone 2 Pieces
	MeshRenderComponent* z2p1 = new MeshRenderComponent();
	temp->AttachComponent(z2p1);
	z2p1->LoadModel("ProbendArenaZone2P1.mesh");
	std::shared_ptr<MeshInfo> z2p1MeshInfo = z2p1->GetMeshInfo();
	ShapeDefinition z2p1ShapeDef = ShapeDefinition(true);
	z2p1ShapeDef.SetConvexMeshGeometry(dataMan->CookConvexMesh(z2p1MeshInfo, "Z2P1Mesh"),
		physx::PxVec3(2.20f, 8.0f, 2.0f));
	z2p1ShapeDef.AddMaterial("101000");
	z2p1ShapeDef.SetFilterFlags(ArenaData::ZONE_TRIGGER);
	dataMan->CreateShape(z2p1ShapeDef, "Z2P1Shape");

	MeshRenderComponent* z2p2 = new MeshRenderComponent();
	temp->AttachComponent(z2p2);
	z2p2->LoadModel("ProbendArenaZone2P2.mesh");
	std::shared_ptr<MeshInfo> z2p2MeshInfo = z2p2->GetMeshInfo();
	ShapeDefinition z2p2ShapeDef = ShapeDefinition(true);
	z2p2ShapeDef.SetConvexMeshGeometry(dataMan->CookConvexMesh(z2p2MeshInfo, "Z2P2Mesh"),
		physx::PxVec3(2.0f, 8.0f, 2.0f));
	z2p2ShapeDef.AddMaterial("101000");
	z2p2ShapeDef.SetFilterFlags(ArenaData::ZONE_TRIGGER);
	dataMan->CreateShape(z2p2ShapeDef, "Z2P2Shape");

	//Zone 3 Pieces
	MeshRenderComponent* z3 = new MeshRenderComponent();
	temp->AttachComponent(z3);
	z3->LoadModel("ProbendArenaZone3.mesh");
	std::shared_ptr<MeshInfo> z3MeshInfo = z3->GetMeshInfo();
	ShapeDefinition z3ShapeDef = ShapeDefinition(true);
	z3ShapeDef.SetConvexMeshGeometry(dataMan->CookConvexMesh(z3MeshInfo, "Z3Mesh"),
		physx::PxVec3(2.150f, 8.0f, 2.10f));
	z3ShapeDef.AddMaterial("101000");
	z3ShapeDef.SetFilterFlags(ArenaData::ZONE_TRIGGER);
	dataMan->CreateShape(z3ShapeDef, "Z3Shape");
}

