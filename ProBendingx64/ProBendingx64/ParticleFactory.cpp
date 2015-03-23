#include "ParticleFactory.h"

#include <memory>

#include "IScene.h"

#include "ParticleComponent.h"
#include "ParticlePointEmitter.h"
#include "SphereEmitter.h"

#include "ParticleSystemBase.h"
#include "ColourFadeParticleAffector.h"
#include "ParticleAffectors.h"
#include "RotationAffector.h"
#include "TextureParticleAffector.h"

ParticleComponent* ParticleFactory::CreateFireEffect(std::shared_ptr<AbstractParticleEmitter> emitter,
													 GameObject* object, IScene* scene)
{
//	/*std::shared_ptr<ParticlePointEmitter> emitter = std::make_shared<ParticlePointEmitter>
//		(ParticlePointEmitter(90, physx::PxVec3(0.0f, 0.0f, 0.0f), 
//		physx::PxVec3(-0.7f, 1.0f, -0.70f).getNormalized(), physx::PxVec3(0.70f, 1.0f, 0.70f).getNormalized(),
//		0.0f, 1.50f, 2.50f));*/
//
//	std::shared_ptr<SphereEmitter> emitter = std::make_shared<SphereEmitter>
//		(SphereEmitter(90, physx::PxVec3(0.0f), 0.0f, 0.15f, 0.250f, true, 0.40f, true));
//
	ParticleSystemParams params = ParticleSystemParams(1.0f, 2.0f, scene->GetCudaContextManager(),
		physx::PxVec3(0.0f, 0.0f, 0.0f),1.0f, false, physx::PxParticleBaseFlag::eENABLED,
		0.50f, 0.0f, 0.0f, 0.30f, 0.1f, 0.0f, physx::PxFilterData());

	ParticleSystemBase* particles = new ParticleSystemBase(emitter, 75, 0.750f,params);
	particles->ResetOnDisable = true;

	ParticleComponent* particleComponent = new ParticleComponent(particles, false);

	object->AttachComponent(particleComponent);

	particles->AddAffector(std::make_shared<ScaleParticleAffector>(ScaleParticleAffector(false, 0.0f, 1.0f, true)));
	particles->AddAffector(std::make_shared<ColourFadeParticleAffector>(
		ColourFadeParticleAffector(physx::PxVec4(1.0f, 0.8f, 0.0f, 1.0f), 
		physx::PxVec4(1.0f, 0.2f, 0.1f, 0.20f), 
		true)));

	particles->AddAffector(std::make_shared<RotationAffector>(0.0f, 360.0f, true));

	std::shared_ptr<TextureParticleAffector> texShared = 
		std::make_shared<TextureParticleAffector>(particles, true, 0.50f, false);

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

	return particleComponent;
}

ParticleComponent* ParticleFactory::CreateSmokeEffect(std::shared_ptr<AbstractParticleEmitter> emitter,
													  GameObject* object, IScene* scene)
{
	/*std::shared_ptr<SphereEmitter> emitter = std::make_shared<SphereEmitter>
	(SphereEmitter(90, physx::PxVec3(0.0f), 0.0f, 0.25f, 0.50f, true, 0.50f, false));*/

	ParticleSystemParams params = ParticleSystemParams(1.0f, 2.0f, scene->GetCudaContextManager(),
		physx::PxVec3(0.0f, 0.0f, 0.0f),1.0f, false, physx::PxParticleBaseFlag::eENABLED,
		0.50f, 0.0f, 0.0f, 0.30f, 0.1f, 0.0f);

	ParticleSystemBase* particles = new ParticleSystemBase(emitter, 30, 0.750f,params);
	particles->ResetOnDisable = true;
	
	ParticleComponent* particleComponent = new ParticleComponent(particles, false);

	object->AttachComponent(particleComponent);

	particles->AddAffector(std::make_shared<ScaleParticleAffector>(ScaleParticleAffector(false, 0.0f, 0.80f, true)));
	particles->AddAffector(std::make_shared<ColourFadeParticleAffector>(
		ColourFadeParticleAffector(physx::PxVec4(0.588240f, 0.568627f, 0.568627f, 0.00f), 
		physx::PxVec4(0.588240f, 0.568627f, 0.568627f, 0.30f), 
		true)));

	particles->AddAffector(std::make_shared<RotationAffector>(0.0f, -360.0f, true));

	std::shared_ptr<TextureParticleAffector> texShared = 
		std::make_shared<TextureParticleAffector>(particles, true, 0.50f, false);

	particles->AddAffector(texShared);

	particles->GetMaterial()->CreateMaterial(particles, 1);
	texShared->AddTextureToMaterial("smoke.png");

	texShared->CalculateFrameStep(0.7500f);

	particles->AssignAffectorKernel(particles->FindBestKernel());
	particles->setMaterial(particles->GetMaterial()->GetMaterialName());

	return particleComponent;
}

ParticleComponent* ParticleFactory::CreateWaterMistEffect(GameObject* object, IScene* scene)
{
	std::shared_ptr<SphereEmitter> emitter = std::make_shared<SphereEmitter>
		(SphereEmitter(150, physx::PxVec3(0.0f), 0.0f, 0.15f, 0.250f, true, 0.50f, true)); 

	ParticleSystemParams params = ParticleSystemParams(1.0f, 2.0f, scene->GetCudaContextManager(),
		physx::PxVec3(0.0f, 0.0f, 0.0f),1.0f, false, physx::PxParticleBaseFlag::eENABLED,
		0.50f, 0.0f, 0.0f, 0.30f, 0.1f, 0.0f);

	ParticleSystemBase* particles = new ParticleSystemBase(emitter, 150, 0.75f,params);
	particles->ResetOnDisable = true;

	ParticleComponent* particleComponent = new ParticleComponent(particles, false);

	object->AttachComponent(particleComponent);

	particles->AddAffector(std::make_shared<ScaleParticleAffector>(ScaleParticleAffector(false, 0.0f, 0.80f, true)));
	particles->AddAffector(std::make_shared<ColourFadeParticleAffector>(
		ColourFadeParticleAffector(physx::PxVec4(0.90f, 0.90f, 0.90f, 1.0f), 
		physx::PxVec4(0.220f, 0.30f, 1.0f , 0.50f),	true)));

	//particles->AddAffector(std::make_shared<RotationAffector>(0.0f, -360.0f, true));

	std::shared_ptr<TextureParticleAffector> texShared = 
		std::make_shared<TextureParticleAffector>(particles, true, 0.50f, false);

	particles->AddAffector(texShared);

	particles->GetMaterial()->CreateMaterial(particles, 1);
	texShared->AddTextureToMaterial("MistSprite2.png");

	texShared->CalculateFrameStep(0.7500f);

	particles->AssignAffectorKernel(particles->FindBestKernel());
	particles->setMaterial(particles->GetMaterial()->GetMaterialName());

	return particleComponent;
}

ParticleComponent* ParticleFactory::CreateParticleSystem(ParticlePrefabs prefab, GameObject* object, IScene* scene)
{
	switch (prefab)
	{
	case ParticleFactory::PointFire:
		{
			std::shared_ptr<ParticlePointEmitter> emitter = std::make_shared<ParticlePointEmitter>
				(ParticlePointEmitter(90, physx::PxVec3(0.0f, 0.0f, 0.0f), 
				physx::PxVec3(-0.7f, 1.0f, -0.70f).getNormalized(), physx::PxVec3(0.70f, 1.0f, 0.70f).getNormalized(),
				0.0f, 1.50f, 2.50f));
			return CreateFireEffect(emitter, object, scene);

			
		}
		break;
	case ParticleFactory::SphereFire:
		{
			std::shared_ptr<SphereEmitter> emitter = std::make_shared<SphereEmitter>
				(SphereEmitter(90, physx::PxVec3(0.0f), 0.0f, 0.15f, 0.250f, true, 0.40f, true));
			return CreateFireEffect(emitter, object, scene);
			
		}
		break;
	case ParticleFactory::PointSmoke:
		{
			std::shared_ptr<SphereEmitter> emitter = std::make_shared<SphereEmitter>
				(SphereEmitter(90, physx::PxVec3(0.0f), 0.0f, 0.25f, 0.50f, true, 0.50f, false));
			return CreateSmokeEffect(emitter, object, scene);
			
		}
		break;
	case ParticleFactory::SphereSmoke:
		{
			std::shared_ptr<SphereEmitter> emitter = std::make_shared<SphereEmitter>
				(SphereEmitter(90, physx::PxVec3(0.0f), 0.0f, 0.25f, 0.50f, true, 0.50f, false));
			return CreateSmokeEffect(emitter, object, scene);
		}
		break;
	case ParticleFactory::WaterMist:
		return CreateWaterMistEffect(object, scene);
		break;
	default:
		return nullptr;
		break;
	}
}

