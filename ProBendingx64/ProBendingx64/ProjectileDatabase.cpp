#include "ProjectileDatabase.h"
#include "Controllers.h"
#include "MeshRenderComponent.h"
#include "RigidBodyComponent.h"
#include "PhysXDataManager.h"

const std::string ProjectileDatabase::ShapeString = "Shape";

ProjectileDatabase::ProjectileDictionary ProjectileDatabase::GetAirProjectiles(IScene* scene)
{
	return ProjectileDictionary();
}

ProjectileDatabase::ProjectileDictionary ProjectileDatabase::GetEarthProjectiles(IScene* scene)
{
	ProjectileDictionary earthDictionary = ProjectileDictionary();

	earthDictionary.insert(ProjectileDictionary::value_type(AbilityIDs::EARTH_COIN, CreateEarthCoin(scene)));

	return earthDictionary;
}

ProjectileDatabase::SharedProjectile ProjectileDatabase::CreateEarthCoin(IScene* scene)
{
	std::string projName = AbilityIDs::EarthEnumToString(AbilityIDs::EARTH_COIN);
	
	ProjectileAttributes earthCoinAttributes = ProjectileAttributes(1.0f, 2.0f, 150.0f, 150.0f);

	SharedProjectile projectile = std::make_shared<Projectile>(scene, earthCoinAttributes, projName);
	projectile->tag = TagsAndLayersManager::ProjectileTag;

	MeshRenderComponent* renderComponent = new MeshRenderComponent();
	projectile->AttachComponent(renderComponent);
	renderComponent->LoadModel("Rock_01.mesh");
	projectile->SetScale(0.01f, 0.01f, 0.01f);

	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	projectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC); //Create dynamic body at 0,0,0 with 0 rotation
	physx::PxVec3 entityHalfSize = HelperFunctions::OgreToPhysXVec3(renderComponent->GetHalfExtents());
	
	ShapeDefinition shapeDef = ShapeDefinition();
	shapeDef.SetSphereGeometry(entityHalfSize.magnitude() * 0.5f);
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

ProjectileDatabase::ProjectileDictionary ProjectileDatabase::GetFireProjectiles(IScene* scene)
{
	ProjectileDictionary fireDictionary = ProjectileDictionary();


	return fireDictionary;
}

ProjectileDatabase::SharedProjectile ProjectileDatabase::CreateFireBlast(IScene* scene)
{
	ProjectileAttributes attributes = ProjectileAttributes();

	return std::make_shared<Projectile>(Projectile(scene, attributes, AbilityIDs::FireEnumToString(AbilityIDs::FIRE_BLAST)));
}

ProjectileDatabase::ProjectileDictionary ProjectileDatabase::GetWaterProjectiles(IScene* scene)
{
	return ProjectileDictionary();
}


