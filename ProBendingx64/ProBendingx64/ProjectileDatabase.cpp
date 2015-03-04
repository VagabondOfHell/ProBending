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
	
	ProjectileAttributes earthCoinAttributes = ProjectileAttributes(1.0f, 2.0f, 150.0f, 150.0f, 30.0f, 50.0f);

	SharedProjectile projectile = std::make_shared<Projectile>(scene, earthCoinAttributes, projName);
	projectile->tag = TagsAndLayersManager::ProjectileTag;

	MeshRenderComponent* renderComponent = new MeshRenderComponent();
	projectile->AttachComponent(renderComponent);
	renderComponent->LoadModel("Rock_01.mesh");
	projectile->SetScale(0.005f, 0.001f, 0.005f);

	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	projectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC); //Create dynamic body at 0,0,0 with 0 rotation
	physx::PxVec3 entityHalfSize = HelperFunctions::OgreToPhysXVec3(renderComponent->GetHalfExtents());
	
	ShapeDefinition shapeDef = ShapeDefinition();
	//shapeDef.SetSphereGeometry(entityHalfSize.magnitude() * 0.5f);
	shapeDef.SetBoxGeometry(entityHalfSize);
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

	fireDictionary.insert(ProjectileDictionary::value_type(AbilityIDs::FIRE_BLAST, CreateFireBlast(scene)));

	return fireDictionary;
}

ProjectileDatabase::SharedProjectile ProjectileDatabase::CreateFireBlast(IScene* scene)
{
	std::string projName = AbilityIDs::FireEnumToString(AbilityIDs::FIRE_BLAST);

	ProjectileAttributes attributes = ProjectileAttributes();

	SharedProjectile newProjectile = std::make_shared<Projectile>(scene, ProjectileAttributes(), projName);

	std::shared_ptr<ParticlePointEmitter> emitter = std::make_shared<ParticlePointEmitter>
	(ParticlePointEmitter(70, physx::PxVec3(0.0f, 0.0f, 0.0f), 
	physx::PxVec3(0.0f, 1.0f, -0.50f).getNormalized(), physx::PxVec3(0.0f, 1.0f, 0.50f).getNormalized(),
		0.0f, 0.50f, 1.0f));

	ParticleSystemParams params = ParticleSystemParams(1.0f, 2.0f, scene->GetCudaContextManager(),
		physx::PxVec3(0.0f, 0.0f, 0.0f),1.0f, false, physx::PxParticleBaseFlag::eENABLED,
		0.50f, 0.0f, 0.0f, 0.30f, 0.3f, 0.0f);

	params.SetFluidParameters(100.0f, 5.0f, 3.0f);

	ParticleSystemBase* particles = new ParticleSystemBase(emitter, 50, 0.750f,params);

	ParticleComponent* particleComponent = new ParticleComponent(particles, false);

	newProjectile->AttachComponent(particleComponent);
						
	particles->AddAffector(std::make_shared<ScaleParticleAffector>(ScaleParticleAffector(false, 0.0f, 1.0f, true)));
	particles->AddAffector(std::make_shared<ColourFadeParticleAffector>(
		ColourFadeParticleAffector(physx::PxVec4(1.0f, 0.8f, 0.0f, 1.0f), 
		physx::PxVec4(1.0f, 0.2f, 0.1f, 0.20f), 
		true)));/**/

	std::shared_ptr<TextureParticleAffector> texShared = 
		std::make_shared<TextureParticleAffector>(particles, true, 0.750f, false);
			
	particles->AddAffector(texShared);

	particles->GetMaterial()->CreateMaterial(particles, 5);
			
	texShared->AddTextureToMaterial("Flame1.png");
	texShared->AddTextureToMaterial("Flame2.png");
	texShared->AddTextureToMaterial("Flame3.png");
	texShared->AddTextureToMaterial("Flame4.png");
	texShared->AddTextureToMaterial("Flame5.png");

	texShared->CalculateFrameStep(0.7500f);

	particles->AssignAffectorKernel(particles->FindBestKernel());
	particles->setMaterial(particles->GetMaterial()->GetMaterialName());
			
	RigidBodyComponent* rigidBody = new RigidBodyComponent();
	newProjectile->AttachComponent(rigidBody);
	rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC);

	ShapeDefinition shapeDef = ShapeDefinition();
	//shapeDef.SetSphereGeometry(entityHalfSize.magnitude() * 0.5f);
	shapeDef.SetSphereGeometry(0.15f);
	shapeDef.AddMaterial(PhysXDataManager::GetSingletonPtr()->CreateMaterial(1.0f, 1.0f, 0.0f, "101000"));
	physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, projName + ShapeString);
	if(shape)
	{
		rigidBody->AttachShape(*shape);
		rigidBody->CreateDebugDraw();
		rigidBody->SetUseGravity(false);
	}
	newProjectile->tag = TagsAndLayersManager::ProjectileTag;
	newProjectile->SetScale(0.1f, 0.1f, 0.1f);

	return newProjectile;
}

ProjectileDatabase::ProjectileDictionary ProjectileDatabase::GetWaterProjectiles(IScene* scene)
{
	return ProjectileDictionary();
}


