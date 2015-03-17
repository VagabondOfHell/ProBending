#include "ParticleFactory.h"

#include <memory>

#include "IScene.h"

#include "ParticleComponent.h"
#include "ParticlePointEmitter.h"
#include "ParticleSystemBase.h"
#include "ColourFadeParticleAffector.h"
#include "ParticleAffectors.h"
#include "RotationAffector.h"
#include "TextureParticleAffector.h"

ParticleComponent* ParticleFactory::CreateFireEffect(GameObject* object, IScene* scene)
{
	std::shared_ptr<ParticlePointEmitter> emitter = std::make_shared<ParticlePointEmitter>
		(ParticlePointEmitter(90, physx::PxVec3(0.0f, 0.0f, 0.0f), 
		physx::PxVec3(-0.7f, 1.0f, -0.70f).getNormalized(), physx::PxVec3(0.70f, 1.0f, 0.70f).getNormalized(),
		0.0f, 1.50f, 2.50f));

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

ParticleComponent* ParticleFactory::CreateSmokeEffect(GameObject* object, IScene* scene)
{
	std::shared_ptr<ParticlePointEmitter> emitter = std::make_shared<ParticlePointEmitter>
		(ParticlePointEmitter(40, physx::PxVec3(0.0f, 0.50f, 0.0f), 
		physx::PxVec3(-0.50f, 1.0f, -0.50f).getNormalized(), physx::PxVec3(0.50f, 1.0f, 0.50f).getNormalized(),
		0.0f, 1.50f, 2.0f));

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

ParticleComponent* ParticleFactory::CreateParticleSystem(ParticlePrefabs prefab, GameObject* object, IScene* scene)
{
	switch (prefab)
	{
	case ParticleFactory::Fire:
		return CreateFireEffect(object, scene);
		break;
	case ParticleFactory::Smoke:
		return CreateSmokeEffect(object, scene);
		break;
	default:
		return nullptr;
		break;
	}
}
