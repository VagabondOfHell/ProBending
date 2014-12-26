#include "ProjectileFactory.h"
#include "Projectile.h"

#include "IScene.h"

#include "ParticleSystemBase.h"
#include "ParticleComponent.h"
#include "ParticlePointEmitter.h"
#include "ParticleAffectors.h"
#include "ColourFadeParticleAffector.h"

#include "PxScene.h"
#include "geometry/PxBoxGeometry.h"
#include "PxPhysics.h"
#include "extensions/PxSimpleFactory.h"
#include "PxRigidDynamic.h"
#include "foundation/PxVec4.h"

#include "OgreMaterialManager.h"
#include "OgreTechnique.h"
#include "OgrePass.h"
#include "OgreSceneNode.h"

Projectile* ProjectileFactory::CreateProjectile(IScene* const scene,const ElementEnum::Element element,const AbilityIDs::AbilityID abilityID)
{
	Projectile* newProjectile = NULL;

	switch (element)
	{
	case ElementEnum::InvalidElement:
		return NULL;
		break;
	case ElementEnum::Air:
		break;
	case ElementEnum::Earth:
		if(abilityID == AbilityIDs::EARTH_BOULDER)
		{
			newProjectile = new Projectile(scene, nullptr);
			newProjectile->LoadModel("Rock_01.mesh");
			newProjectile->gameObjectNode->setScale(0.1f, 0.1f, 0.1f);
			physx::PxBoxGeometry boxGeo;
			if(newProjectile->ConstructBoxFromEntity(boxGeo))
			{
				physx::PxMaterial* mat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

				physx::PxTransform tran = physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 0.0f), physx::PxQuat::createIdentity());
				newProjectile->rigidBody = physx::PxCreateDynamic(PxGetPhysics(), tran, boxGeo, *mat, 1.0f);

				newProjectile->CreatePhysXDebug();
				newProjectile->rigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
				scene->GetPhysXScene()->addActor(*newProjectile->rigidBody);
			}
		}
		break;
	case ElementEnum::Fire:
		if(abilityID == AbilityIDs::FIRE_JAB)
		{
			newProjectile = new Projectile(scene, nullptr);
			ParticlePointEmitter* emitter = new ParticlePointEmitter(150, physx::PxVec3(0.0f, 0.0f, 1.0f),
				physx::PxVec3(-1.0f, -1.0f, 0.0f).getNormalized(), physx::PxVec3(1.0f, 1.0f, 0.0f).getNormalized(),
				10.0f, 10.0f);

			ParticleSystemParams params = ParticleSystemParams(40.0f, 2.0f, scene->GetCudaContextManager(),
				physx::PxVec3(0.0f, 0.0f, 0.0f),1.0f, false);

			ParticleSystemBase* particles = new ParticleSystemBase(emitter, 500, 2.0f,params, true);
			
			ParticleComponent* particleComponent = new ParticleComponent(newProjectile, particles, false);

			newProjectile->AttachComponent(particleComponent);

			particles->AddAffector(new ScaleParticleAffector(false, 0, 10, true));
			particles->AddAffector(new ColourFadeParticleAffector(physx::PxVec4(1, 0.5, 0, 1.0f), 
				physx::PxVec4(0, 0, 1.0, 0.20f), true));/**/
			particles->AssignAffectorKernel(particles->FindBestKernel());
			particles->setMaterial(particles->FindBestShader());
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
