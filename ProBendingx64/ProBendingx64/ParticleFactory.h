#pragma once

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

public:
	enum ParticlePrefabs{PointFire, SphereFire, PointSmoke, SphereSmoke, WaterMist};

	static ParticleComponent* CreateParticleSystem(ParticlePrefabs prefab, GameObject* object, IScene* scene);

};

