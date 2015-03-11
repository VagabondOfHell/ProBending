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

ProjectileDatabase::ProjectileDictionary ProjectileDatabase::GetAirProjectiles(IScene* scene)
{
	return ProjectileDictionary();
}

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

	ProjectileAttributes earthJabAttributes = ProjectileAttributes(Ogre::Vector3(0.0f), 0.2f, 350.0f, 450.0f, 30.0f, 20.0f, 30.0f, true);

	SharedProjectile projectile = std::make_shared<Projectile>(scene, earthJabAttributes, projName);
	projectile->tag = TagsAndLayersManager::ProjectileTag;
	projectile->DestructionTriggers = ArenaData::PROJECTILE | ArenaData::WATER;

	MeshRenderComponent* renderComponent = new MeshRenderComponent();
	projectile->AttachComponent(renderComponent);
	renderComponent->LoadModel("Rock_01.mesh");
	projectile->SetScale(0.0025f, 0.0025f, 0.0025f);

	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	projectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC); //Create dynamic body at 0,0,0 with 0 rotation
	
	earthJabAttributes.HalfExtents = renderComponent->GetHalfExtents();
	physx::PxVec3 entityHalfSize = HelperFunctions::OgreToPhysXVec3(earthJabAttributes.HalfExtents);

	ShapeDefinition shapeDef = ShapeDefinition();
	shapeDef.SetSphereGeometry(entityHalfSize.magnitude() * 0.45f);
	//shapeDef.SetBoxGeometry(entityHalfSize);
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
	projectile->SetScale(0.005f, 0.001f, 0.005f);

	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	projectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC); //Create dynamic body at 0,0,0 with 0 rotation
	earthCoinAttributes.HalfExtents = renderComponent->GetHalfExtents();

	physx::PxVec3 entityHalfSize = HelperFunctions::OgreToPhysXVec3(earthCoinAttributes.HalfExtents);
	
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

	ProjectileAttributes attributes = ProjectileAttributes(Ogre::Vector3(0.0f), 0.0f, 1.0f, 350.0f, 450.0f, 15.0f, 25.0f, false);

	SharedProjectile newProjectile = std::make_shared<Projectile>(scene, attributes, projName);
	newProjectile->tag = TagsAndLayersManager::ProjectileTag;
	newProjectile->DestructionTriggers = ArenaData::PROJECTILE | ArenaData::WATER | ArenaData::WALL | ArenaData::ARENA_SURFACE;

	ParticleComponent* fireEffect = ParticleFactory::CreateParticleSystem(ParticleFactory::Fire, 
		newProjectile.get(), scene);
	fireEffect->particleSystem->GetEmitter()->SetEmissionRate(100.0f);

	ParticleComponent* smokeEffect = ParticleFactory::CreateParticleSystem(ParticleFactory::Smoke,
		newProjectile.get(), scene);

	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	newProjectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC);

	ShapeDefinition shapeDef = ShapeDefinition();
	shapeDef.SetSphereGeometry(0.15f);
	attributes.HalfExtents = Ogre::Vector3(0.15f);

	shapeDef.SetFilterFlags(ArenaData::PROJECTILE);

	shapeDef.AddMaterial(PhysXDataManager::GetSingletonPtr()->CreateMaterial(1.0f, 1.0f, 0.0f, "101000"));
	physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, projName + ShapeString);

	if(shape)
	{
		rigidBody->AttachShape(*shape);
		//rigidBody->CreateDebugDraw();
		rigidBody->SetUseGravity(false);
	}

	newProjectile->SetScale(0.1f, 0.1f, 0.1f);

	return newProjectile;
}

ProjectileDatabase::SharedProjectile ProjectileDatabase::CreateFireBlast(IScene* scene)
{
	std::string projName = AbilityIDs::FireEnumToString(AbilityIDs::FIRE_BLAST);

	ProjectileAttributes attributes = ProjectileAttributes(Ogre::Vector3(0.0f), 0.0f, 1.0f, 400.0f, 500.0f, 15.0f, 25.0f, false);

	SharedProjectile newProjectile = std::make_shared<Projectile>(scene, attributes, projName);
	newProjectile->tag = TagsAndLayersManager::ProjectileTag;
	newProjectile->DestructionTriggers = ArenaData::PROJECTILE | ArenaData::WATER | ArenaData::WALL;

	ParticleComponent* fireEffect = ParticleFactory::CreateParticleSystem(ParticleFactory::Fire, 
		newProjectile.get(), scene);

	ParticleComponent* smokeEffect = ParticleFactory::CreateParticleSystem(ParticleFactory::Smoke,
		newProjectile.get(), scene);
			
	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	newProjectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC);

	ShapeDefinition shapeDef = ShapeDefinition();
	shapeDef.SetSphereGeometry(0.15f);

	attributes.HalfExtents = Ogre::Vector3(0.15f);

	shapeDef.SetFilterFlags(ArenaData::PROJECTILE);

	shapeDef.AddMaterial(PhysXDataManager::GetSingletonPtr()->CreateMaterial(1.0f, 1.0f, 0.0f, "101000"));
	physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, projName + ShapeString);
	
	if(shape)
	{
		rigidBody->AttachShape(*shape);
		//rigidBody->CreateDebugDraw();
		rigidBody->SetUseGravity(false);
	}

	newProjectile->SetScale(0.1f, 0.1f, 0.1f);

	return newProjectile;
}

ProjectileDatabase::ProjectileDictionary ProjectileDatabase::GetWaterProjectiles(IScene* scene)
{
	return ProjectileDictionary();
}


