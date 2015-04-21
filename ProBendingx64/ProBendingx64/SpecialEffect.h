#pragma once
#include "OgreVector3.h"
#include <memory>

class GameObject;

class SpecialEffect
{
private:
	typedef std::shared_ptr<GameObject> SharedGameObject;

	SharedGameObject gameObject;

	float passedTime;

public:
	bool XIsAbsolute, YIsAbsolute, ZIsAbsolute;
	Ogre::Vector3 SpawnPosition;
	Ogre::Vector3 EventPosition;

	float Duration;

	SpecialEffect(void);
	SpecialEffect(SharedGameObject _gameObject);

	virtual ~SpecialEffect(void);

	inline SharedGameObject GetGameObject()const{return gameObject;}

	inline bool IsValid(){return gameObject != NULL;}

	virtual void Start();

	void ShowEffect(bool resetIfAlreadyShown = true);
	void HideEffect();

	virtual void Update(float gameTime);

	virtual SpecialEffect* Clone();

};

