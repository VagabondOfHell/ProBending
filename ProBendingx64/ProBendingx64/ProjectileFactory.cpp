#include "ProjectileFactory.h"
#include "Projectile.h"

#include "IScene.h"

#include "ParticleSystemBase.h"
#include "ParticleComponent.h"
#include "ParticlePointEmitter.h"
#include "ParticleAffectors.h"
#include "ColourFadeParticleAffector.h"
#include "TextureParticleAffector.h"

#include "Controllers.h"

#include "MeshRenderComponent.h"
#include "RigidBodyComponent.h"
#include "PhysXDataManager.h"

#include "PxScene.h"
#include "geometry/PxBoxGeometry.h"
#include "PxPhysics.h"
#include "extensions/PxSimpleFactory.h"
#include "PxRigidDynamic.h"
#include "foundation/PxVec4.h"

#include "OgreEntity.h"
#include "OgreMaterialManager.h"
#include "OgreTechnique.h"
#include "OgrePass.h"
#include "OgreSceneNode.h"
#include "OgreMeshManager.h"

SharedProjectile ProjectileFactory::CreateProjectile(IScene* const scene,const ElementEnum::Element element,const AbilityIDs::AbilityID abilityID)
{
	SharedProjectile newProjectile = NULL;

	switch (element)
	{
	case ElementEnum::InvalidElement:
		return NULL;
		break;
	case ElementEnum::Air:
		break;
	case ElementEnum::Earth:
		if(abilityID == AbilityIDs::EARTH_COIN)
		{
			newProjectile = std::make_shared<Projectile>(scene, "EarthBoulder", nullptr);

			HandMoveController* controller = new HandMoveController(newProjectile.get(), HandMoveController::CH_BOTH, 
					physx::PxVec3(-2.0f, -2.0f, -2.0f), physx::PxVec3(2.0f, 2.0f, 2.0f));

			//newProjectile->AttachController(controller);

			MeshRenderComponent* renderComponent = new MeshRenderComponent();
		//	newProjectile->SetWorldPosition(-80, 0, 0);
			
			newProjectile->AttachComponent(renderComponent);
			
			renderComponent->LoadModel("Rock_01.mesh");
			//renderComponent->SetMaterial("RedProbender");
			newProjectile->SetScale(0.01f, 0.01f, 0.01f);

			RigidBodyComponent* rigidBody = new RigidBodyComponent();
			newProjectile->AttachComponent(rigidBody);

			SharedMeshInfo info = renderComponent->GetMeshInfo();

			rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC, physx::PxVec3(0, 0, 0)); //Create dynamic body at 0,0,0 with 0 rotation
			//rigidBody->SetPosition(physx::PxVec3(0, 0, 0));
			physx::PxVec3 entityHalfSize = HelperFunctions::OgreToPhysXVec3(renderComponent->GetHalfExtents());
			//SharedBoxGeo geo = PhysXDataManager::GetSingletonPtr()->CreateBoxGeometry(entityHalfSize, "RockBox");
			
			physx::PxConvexMesh* convexMesh = PhysXDataManager::GetSingletonPtr()->CookConvexMesh(info, "RockMesh");

			if(!convexMesh)
				convexMesh = PhysXDataManager::GetSingletonPtr()->GetConvexMesh("RockMesh");

			if(convexMesh)
			{
				PhysXDataManager::GetSingletonPtr()->CreateMaterial(1.0f, 1.0f, 0.0f, "RockMaterial");

				ShapeDefinition shapeDef = ShapeDefinition();
				//shapeDef.SetConvexMeshGeometry(convexMesh);
				//shapeDef.SetBoxGeometry(entityHalfSize);
				shapeDef.SetSphereGeometry(entityHalfSize.magnitude());
				shapeDef.AddMaterial("RockMaterial");
				
				physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, "RockShape");

				if(shape)
				{
					rigidBody->AttachShape(*shape);
					rigidBody->GetDynamicActor()->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
					rigidBody->CreateDebugDraw();
					rigidBody->SetUseGravity(false);
				}
				//rigidBody->CreateAndAttachNewShape(shapeDef);
				
				//renderComponent->Disable();
			}
			newProjectile->SetWorldPosition(0, 40, 0);
			scene->AddGameObject(newProjectile);
		}
		break;
	case ElementEnum::Fire:
		if(abilityID == AbilityIDs::FIRE_BLAST)
		{
			newProjectile = std::make_shared<Projectile>(scene, "Fire Jab", nullptr);

			HandMoveController* controller = new HandMoveController(newProjectile.get(), HandMoveController::CH_RIGHT, 
				physx::PxVec3(0.0f, 2.0f, 0.0f), physx::PxVec3(0.0f, -2.0f, 0.0f));

			//newProjectile->AttachController(controller);

			std::shared_ptr<ParticlePointEmitter> emitter = std::make_shared<ParticlePointEmitter>
			(ParticlePointEmitter(50, physx::PxVec3(0.0f, 0.0f, 0.0f), 
			physx::PxVec3(0.0f, 1.0f, 0.0f).getNormalized(), physx::PxVec3(0.0f, 1.0f, 1.0f).getNormalized(),
			true, 2.0f, 1.0f, 5.0f));
			/*std::shared_ptr<ParticlePointEmitter> emitter = std::make_shared<ParticlePointEmitter>
				(ParticlePointEmitter(50, physx::PxVec3(0.0f, 0.0f, 0.0f), 
				physx::PxVec3(0.0f, 0.0f, -1.0f).getNormalized(), physx::PxVec3(0.0f, 0.0f, 1.0f).getNormalized(),
				true, 2.0f, 10.0f, 50.0f));*/

			ParticleSystemParams params = ParticleSystemParams(1.0f, 2.0f, scene->GetCudaContextManager(),
				physx::PxVec3(0.0f, 0.0f, 0.0f),1.0f, true, physx::PxParticleBaseFlag::eENABLED,
				0.50f, 0.0f, 0.0f, 0.30f, 0.3f, 0.0f);

			params.SetFluidParameters(100.0f, 5.0f, 3.0f);

			ParticleSystemBase* particles = new ParticleSystemBase(emitter, 200, 2.0f,params);
			//FluidAndParticleBase* particles = new ParticleSystemBase(emitter, 500, 2.0f,params);
			//particles->SetInfiniteLifetime(true);

			ParticleComponent* particleComponent = new ParticleComponent(particles, false);

			newProjectile->AttachComponent(particleComponent);
						
			particles->AddAffector(std::make_shared<ScaleParticleAffector>(ScaleParticleAffector(false, 0.0f, 1.0f, true)));
			particles->AddAffector(std::make_shared<ColourFadeParticleAffector>(ColourFadeParticleAffector(physx::PxVec4(1, 0.5, 0, 1.0f), 
				physx::PxVec4(0, 0, 1.0, 0.20f), true)));/**/

			std::shared_ptr<TextureParticleAffector> texShared = std::make_shared<TextureParticleAffector>(particles, true, 1.0f, false);
			
			particles->AddAffector(texShared);

			particles->GetMaterial()->CreateMaterial(particles, 0);
			
			texShared->AddTextureToMaterial("smoke.png");
			/*texShared->AddTextureToMaterial("Flame_1.jpg");
			texShared->AddTextureToMaterial("Flame_2.jpg");
			texShared->AddTextureToMaterial("Flame_3.jpg");
			texShared->AddTextureToMaterial("Flame_4.jpg");
			texShared->AddTextureToMaterial("Flame_5.jpg");*/

			texShared->CalculateFrameStep(2.0f);

			//particles->GetMaterial().Recompile();
			/*//particles->GetMaterial().AddAffector(ParticleAffectorType::Scale);*/
		//	particles->GetMaterial().AddAffector(ParticleAffectorType::Texture);
			//particles->GetMaterial().AddAffector(ParticleAffectorType::ColourToColour);
			
			//particles->GetMaterial().AddTextureUnit("smoke.png");

			particles->AssignAffectorKernel(particles->FindBestKernel());
			particles->setMaterial(particles->GetMaterial()->GetMaterialName());
			
			RigidBodyComponent* rigidBody = new RigidBodyComponent();
			newProjectile->AttachComponent(rigidBody);
			rigidBody->CreateRigidBody(RigidBodyComponent::DYNAMIC);
			rigidBody->SetUseGravity(false);
			/*
			MeshRenderComponent* renderComponent = new MeshRenderComponent();
			newProjectile->AttachComponent(renderComponent);

			renderComponent->LoadModel("Rock_01.mesh");*/

			newProjectile->SetScale(0.1f, 0.1f, 0.1f);
			newProjectile->SetWorldPosition(0.0f, 2.5f, 0.0f);
			scene->AddGameObject(newProjectile);

			//SharedProjectile newProjectile2 = std::make_shared<Projectile>(scene, nullptr);
			// emitter = std::make_shared<ParticlePointEmitter>
			//	(ParticlePointEmitter(50, physx::PxVec3(0.0f, 0.0f, 0.0f), 
			//	physx::PxVec3(-1.0f, -1.0f, 0.0f).getNormalized(), physx::PxVec3(1.0f, 1.0f, 0.0f).getNormalized(),
			//	false, 0.0f, 10.0f, 20.0f));

			// params = ParticleSystemParams(40.0f, 2.0f, NULL, // scene->GetCudaContextManager(),
			//	physx::PxVec3(0.0f, 0.0f, 0.0f),1.0f, false);

			// particles = new ParticleSystemBase(emitter, 500, 2.0f,params);

			// particleComponent = new ParticleComponent(particles, false);

			//newProjectile2->AttachComponent(particleComponent);

			//particles->AddAffector(std::make_shared<ScaleParticleAffector>(ScaleParticleAffector(false, 0, 10, false)));
			//particles->AddAffector(std::make_shared<ColourFadeParticleAffector>(ColourFadeParticleAffector(physx::PxVec4(1, 0.5, 0, 1.0f), 
			//	physx::PxVec4(0, 0, 1.0, 0.20f), false)));
			//particles->AssignAffectorKernel(particles->FindBestKernel());
			//particles->setMaterial(particles->FindBestShader());

			//renderComponent = new MeshRenderComponent();
			//newProjectile2->AttachComponent(renderComponent);

			//renderComponent->LoadModel("Rock_01.mesh");

			//newProjectile2->SetScale(0.1f, 0.1f, 0.1f);
			//newProjectile2->SetInheritScale(false);

			//newProjectile->AddChild(newProjectile2);

			//newProjectile2->SetLocalPosition(95.0f, 0.0f, 0.0f);



			//newProjectile2->SetWorldPosition(5.0f, 0.0f, 0.0f);
			/*physx::PxBoxGeometry geo = physx::PxBoxGeometry(0.5f, 0.5f, 0.5f);
			rigidBody->AttachShape(geo);
			rigidBody->SetUseGravity(false);
			rigidBody->CreateDebugDraw();*/

			//Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("DefaultParticleShader");

			//Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
			//pass->setPointSpritesEnabled(true);
			//pass->setPointAttenuation(true, 1, 0, 0);
			/*pass->setPointMaxSize(0.001f);
			pass->setPointMinSize(0.00f);*/

			//particles->setMaterial("ColorParticleShader");

			
		}
		break;
	case ElementEnum::Water:
		break;
	case ElementEnum::Toxin:
		break;
	case ElementEnum::Sonic:
		break;
	case ElementEnum::Astral:
		break;
	case ElementEnum::Metal:
		break;
	case ElementEnum::Sand:
		break;
	case ElementEnum::Vine:
		break;
	case ElementEnum::Lightning:
		break;
	case ElementEnum::Laser:
		break;
	case ElementEnum::Lava:
		break;
	case ElementEnum::Ice:
		break;
	case ElementEnum::Blood:
		break;
	case ElementEnum::Acid:
		break;
	default:
		return NULL;
		break;
	}

	return newProjectile;
}
