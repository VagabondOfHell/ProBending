#pragma once
#include "foundation/PxVec4.h"

class IScene;
class GameObject;
class ParticleComponent;
class AbstractParticleEmitter;

#include <memory>

class ParticleFactory
{
private:
	static ParticleComponent* CreateFireEffect(std::shared_ptr<AbstractParticleEmitter> emitter, 
		GameObject* object, IScene* scene);
	static ParticleComponent* CreateSmokeEffect(std::shared_ptr<AbstractParticleEmitter> emitter,
		GameObject* object, IScene* scene);
	static ParticleComponent* CreateWaterMistEffect(GameObject* object, IScene* scene);

	static ParticleComponent* CreateExplosion(physx::PxVec4& startColour,
		physx::PxVec4& endColour, GameObject* object, IScene* scene);

public:
	enum ParticlePrefabs{PointFire, SphereFire, PointSmoke, SphereSmoke, WaterMist, 
		EarthExplosion, FireExplosion, WaterExplosion};

	static ParticleComponent* CreateParticleSystem(ParticlePrefabs prefab, GameObject* object, IScene* scene);

};

