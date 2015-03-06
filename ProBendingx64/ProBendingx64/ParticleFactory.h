#pragma once

class IScene;
class GameObject;
class ParticleComponent;

class ParticleFactory
{
private:
	static ParticleComponent* CreateFireEffect(GameObject* object, IScene* scene);
	static ParticleComponent* CreateSmokeEffect(GameObject* object, IScene* scene);

public:
	enum ParticlePrefabs{Fire, Smoke};

	static ParticleComponent* CreateParticleSystem(ParticlePrefabs prefab, GameObject* object, IScene* scene);

};

