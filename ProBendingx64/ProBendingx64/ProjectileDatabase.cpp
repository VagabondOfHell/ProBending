#include "ProjectileDatabase.h"
#include "Controllers.h"
#include "MeshRenderComponent.h"
#include "RigidBodyComponent.h"

#include "IScene.h"

#include "ParticlePointEmitter.h"
#include "ParticleSystemBase.h"
#include "ParticleComponent.h"
#include "ParticleAffectors.h"

#include "PhysXDataManager.h"
#include "ColourFadeParticleAffector.h"
#include "TextureParticleAffector.h"
#include "RotationAffector.h"

#include "ParticleFactory.h"

const std::string ProjectileDatabase::ShapeString = "Shape";

#pragma region Earth Element

ProjectileDatabase::ProjectileDictionary ProjectileDatabase::GetEarthProjectiles(IScene* scene)
{
	ProjectileDictionary earthDictionary = ProjectileDictionary();

	earthDictionary.insert(ProjectileDictionary::value_type(AbilityIDs::EARTH_COIN, CreateEarthCoin(scene)));
	earthDictionary.insert(ProjectileDictionary::value_type(AbilityIDs::EARTH_JAB, CreateEarthJab(scene)));

	return earthDictionary;
}

ProjectileDatabase::SharedProjectile ProjectileDatabase::CreateEarthJab(IScene* scene)
{
	std::string projName = AbilityIDs::EarthEnumToString(AbilityIDs::EARTH_JAB);

	/*ProjectileAttributes earthJabAttributes = 
		ProjectileAttributes(Ogre::Vector3(0.0f), 20.0f, 40.0f, 250.0f, 800.0f, 20.0f, 30.0f, true);*/
	ProjectileAttributes earthJabAttributes = 
		ProjectileAttributes(Ogre::Vector3(0.0f), 20.0f, 40.0f, 25.0f, 100.0f, 20.0f, 30.0f, false);

	SharedProjectile projectile = std::make_shared<Projectile>(scene, earthJabAttributes, projName);
	projectile->tag = TagsAndLayersManager::ProjectileTag;
	projectile->DestructionTriggers = ArenaData::PROJECTILE | ArenaData::WATER;

	MeshRenderComponent* renderComponent = new MeshRenderComponent();
	projectile->AttachComponent(renderComponent);
	renderComponent->LoadModel("Rock_01.mesh");
	projectile->SetScale(0.25f, 0.25f, 0.25f);

	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	projectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC); //Create dynamic body at 0,0,0 with 0 rotation
	projectile->SetHalfExtents(renderComponent->GetHalfExtents());
	physx::PxVec3 entityHalfSize = HelperFunctions::OgreToPhysXVec3(projectile->GetHalfExtents());

	ShapeDefinition shapeDef = ShapeDefinition();
	shapeDef.SetSphereGeometry(entityHalfSize.magnitude() * 0.5f);
	//shapeDef.SetBoxGeometry(entityHalfSize);
	shapeDef.SetFilterFlags(ArenaData::PROJECTILE);

	shapeDef.AddMaterial(PhysXDataManager::GetSingletonPtr()->CreateMaterial(1.0f, 1.0f, 0.0f, "101000"));
	physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, projName + ShapeString);
	if(shape)
	{
		rigidBody->AttachShape(*shape);
		rigidBody->CreateDebugDraw();
		rigidBody->SetUseGravity(false);
	}

	return projectile;
}

ProjectileDatabase::SharedProjectile ProjectileDatabase::CreateEarthCoin(IScene* scene)
{
	std::string projName = AbilityIDs::EarthEnumToString(AbilityIDs::EARTH_COIN);
	
	ProjectileAttributes earthCoinAttributes = ProjectileAttributes(Ogre::Vector3(0.0f), 1.0f, 2.0f, 400.0f, 500.0f, 30.0f, 50.0f, true);

	SharedProjectile projectile = std::make_shared<Projectile>(scene, earthCoinAttributes, projName);
	projectile->tag = TagsAndLayersManager::ProjectileTag;
	projectile->DestructionTriggers = ArenaData::PROJECTILE | ArenaData::WATER;

	MeshRenderComponent* renderComponent = new MeshRenderComponent();
	projectile->AttachComponent(renderComponent);
	renderComponent->LoadModel("Rock_01.mesh");
	projectile->SetScale(0.5f, 0.1f, 0.5f);

	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	projectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC); //Create dynamic body at 0,0,0 with 0 rotation
	projectile->SetHalfExtents(renderComponent->GetHalfExtents());

	physx::PxVec3 entityHalfSize = HelperFunctions::OgreToPhysXVec3(projectile->GetHalfExtents());
	
	ShapeDefinition shapeDef = ShapeDefinition();
	//shapeDef.SetSphereGeometry(entityHalfSize.magnitude() * 0.5f);
	shapeDef.SetBoxGeometry(entityHalfSize);
	shapeDef.SetFilterFlags(ArenaData::PROJECTILE);

	shapeDef.AddMaterial(PhysXDataManager::GetSingletonPtr()->CreateMaterial(1.0f, 1.0f, 0.0f, "101000"));
	physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, projName + ShapeString);
	if(shape)
	{
		rigidBody->AttachShape(*shape);
		//rigidBody->CreateDebugDraw();
		rigidBody->SetUseGravity(false);
	}

	return projectile;
}

#pragma endregion

ProjectileDatabase::ProjectileDictionary ProjectileDatabase::GetFireProjectiles(IScene* scene)
{
	ProjectileDictionary fireDictionary = ProjectileDictionary();

	fireDictionary.insert(ProjectileDictionary::value_type(AbilityIDs::FIRE_JAB, CreateFireJab(scene)));
	fireDictionary.insert(ProjectileDictionary::value_type(AbilityIDs::FIRE_BLAST, CreateFireBlast(scene)));

	return fireDictionary;
}

ProjectileDatabase::SharedProjectile ProjectileDatabase::CreateFireJab(IScene* scene)
{
	std::string projName = AbilityIDs::FireEnumToString(AbilityIDs::FIRE_JAB);

	ProjectileAttributes attributes = ProjectileAttributes(Ogre::Vector3(0.0f), 20.0f, 40.0f, 25.0f, 100.0f, 20.0f, 30.0f, false);

	SharedProjectile newProjectile = std::make_shared<Projectile>(scene, attributes, projName);
	newProjectile->tag = TagsAndLayersManager::ProjectileTag;
	newProjectile->DestructionTriggers = ArenaData::PROJECTILE | ArenaData::WATER | ArenaData::WALL | ArenaData::ARENA_SURFACE;

	ParticleComponent* fireEffect = ParticleFactory::CreateParticleSystem(ParticleFactory::PointFire, 
		newProjectile.get(), scene);
	fireEffect->particleSystem->GetEmitter()->SetEmissionRate(100.0f);

	ParticleComponent* smokeEffect = ParticleFactory::CreateParticleSystem(ParticleFactory::PointSmoke,
		newProjectile.get(), scene);

	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	newProjectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC);

	ShapeDefinition shapeDef = ShapeDefinition();
	shapeDef.SetSphereGeometry(0.15f);
	newProjectile->SetHalfExtents(Ogre::Vector3(0.15f));

	shapeDef.SetFilterFlags(ArenaData::PROJECTILE);

	shapeDef.AddMaterial(PhysXDataManager::GetSingletonPtr()->CreateMaterial(1.0f, 1.0f, 0.0f, "101000"));
	physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, projName + ShapeString);

	if(shape)
	{
		rigidBody->AttachShape(*shape);
		//rigidBody->CreateDebugDraw();
		rigidBody->SetUseGravity(false);
	}

	return newProjectile;
}

ProjectileDatabase::SharedProjectile ProjectileDatabase::CreateFireBlast(IScene* scene)
{
	std::string projName = AbilityIDs::FireEnumToString(AbilityIDs::FIRE_BLAST);

	ProjectileAttributes attributes = ProjectileAttributes(Ogre::Vector3(0.0f), 0.0f, 1.0f, 400.0f, 500.0f, 15.0f, 25.0f, false);

	SharedProjectile newProjectile = std::make_shared<Projectile>(scene, attributes, projName);
	newProjectile->tag = TagsAndLayersManager::ProjectileTag;
	newProjectile->DestructionTriggers = ArenaData::PROJECTILE | ArenaData::WATER | ArenaData::WALL;

	ParticleComponent* fireEffect = ParticleFactory::CreateParticleSystem(ParticleFactory::SphereFire, 
		newProjectile.get(), scene);

	ParticleComponent* smokeEffect = ParticleFactory::CreateParticleSystem(ParticleFactory::SphereSmoke,
		newProjectile.get(), scene);
	
	MeshRenderComponent* renderComponent = new MeshRenderComponent();
	newProjectile->AttachComponent(renderComponent);
	renderComponent->LoadModel("WaterSphere.mesh");
	renderComponent->SetMaterial("FireSphereMaterial");
	newProjectile->SetScale(0.25f, 0.25f, 0.25f);
	
	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	newProjectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC);

	newProjectile->SetHalfExtents(renderComponent->GetHalfExtents());
	physx::PxVec3 entityHalfSize = HelperFunctions::OgreToPhysXVec3(newProjectile->GetHalfExtents());

	ShapeDefinition shapeDef = ShapeDefinition();
	shapeDef.SetSphereGeometry(entityHalfSize.magnitude() * 0.45f);

	shapeDef.SetFilterFlags(ArenaData::PROJECTILE);

	shapeDef.AddMaterial(PhysXDataManager::GetSingletonPtr()->CreateMaterial(1.0f, 1.0f, 0.0f, "101000"));
	physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, projName + ShapeString);
	
	if(shape)
	{
		rigidBody->AttachShape(*shape);
		//rigidBody->CreateDebugDraw();
		rigidBody->SetUseGravity(false);
	}

	return newProjectile;
}

ProjectileDatabase::ProjectileDictionary ProjectileDatabase::GetWaterProjectiles(IScene* scene)
{
	ProjectileDictionary waterDictionary = ProjectileDictionary();

	waterDictionary.insert(ProjectileDictionary::value_type(AbilityIDs::WATER_JAB, CreateWaterJab(scene)));

	return waterDictionary;
}

ProjectileDatabase::SharedProjectile ProjectileDatabase::CreateWaterJab(IScene* scene)
{
	std::string projName = AbilityIDs::WaterEnumToString(AbilityIDs::WATER_JAB);

	/*ProjectileAttributes waterJabAttributes = 
		ProjectileAttributes(Ogre::Vector3(0.0f), 0.2f, 350.0f, 450.0f, 30.0f, 20.0f, 30.0f, false);*/
	ProjectileAttributes waterJabAttributes = 
		ProjectileAttributes(Ogre::Vector3(0.0f), 20.0f, 40.0f, 25.0f, 100.0f, 20.0f, 30.0f, false);

	SharedProjectile projectile = std::make_shared<Projectile>(scene, waterJabAttributes, projName);
	projectile->tag = TagsAndLayersManager::ProjectileTag;
	projectile->DestructionTriggers = ArenaData::PROJECTILE | ArenaData::WATER;

	ParticleComponent* waterMist = ParticleFactory::CreateParticleSystem
		(ParticleFactory::WaterMist, projectile.get(), scene);

	MeshRenderComponent* renderComponent = new MeshRenderComponent();
	projectile->AttachComponent(renderComponent);
	renderComponent->LoadModel("WaterSphere.mesh");
//	renderComponent->SetMaterial("Examples/WaterSphere");
	projectile->SetScale(0.25f, 0.25f, 0.25f);

	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	projectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC); //Create dynamic body at 0,0,0 with 0 rotation

	projectile->SetHalfExtents(renderComponent->GetHalfExtents());
	physx::PxVec3 entityHalfSize = HelperFunctions::OgreToPhysXVec3(projectile->GetHalfExtents());

	ShapeDefinition shapeDef = ShapeDefinition();
	shapeDef.SetSphereGeometry(entityHalfSize.magnitude() * 0.45f);
	shapeDef.SetFilterFlags(ArenaData::PROJECTILE);

	shapeDef.AddMaterial(PhysXDataManager::GetSingletonPtr()->CreateMaterial(1.0f, 1.0f, 0.0f, "101000"));
	physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, projName + ShapeString);
	if(shape)
	{
		rigidBody->AttachShape(*shape);
		//rigidBody->CreateDebugDraw();
		rigidBody->SetUseGravity(false);
	}

	return projectile;
}


