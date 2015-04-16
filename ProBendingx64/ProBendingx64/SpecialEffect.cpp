#include "SpecialEffect.h"

#include "GameObject.h"


SpecialEffect::SpecialEffect(void)
	: gameObject(NULL), SpawnPosition(Ogre::Vector3::ZERO), passedTime(0.0f), Duration(0.0f)
{
}

SpecialEffect::SpecialEffect(SharedGameObject _gameObject)
	: gameObject(_gameObject), SpawnPosition(Ogre::Vector3::ZERO), passedTime(0.0f), Duration(0.0f)
{
	gameObject->SetWorldPosition(9999.0f, 99999.0f, 9999.0f);
}

SpecialEffect::~SpecialEffect(void)
{
}

void SpecialEffect::ShowEffect(bool resetIfAlreadyShown /*= true*/)
{
	if(!gameObject)
		return;

	if(gameObject->GetEnabled() && !resetIfAlreadyShown)
		return;

	passedTime = 0.0f;

	gameObject->Enable();
}

void SpecialEffect::HideEffect()
{
	gameObject->SetWorldPosition(9999.0f, 99999.0f, 9999.0f);
	gameObject->Disable();
	passedTime = 0.0f;
}

void SpecialEffect::Update(float gameTime)
{
	if(!gameObject->GetEnabled())
		return;

	passedTime += gameTime;
	Ogre::Vector3 currDerivedPosition;
	currDerivedPosition = gameObject->GetWorldPosition();

	Ogre::Vector3 newDerivedPosition;

	if(XIsAbsolute)
		newDerivedPosition.x = SpawnPosition.x;
	else
		newDerivedPosition.x = EventPosition.x - SpawnPosition.x;

	if(YIsAbsolute)
		newDerivedPosition.y = SpawnPosition.y;
	else
		newDerivedPosition.y = EventPosition.y - SpawnPosition.y;

	if(ZIsAbsolute)
		newDerivedPosition.z = SpawnPosition.z;
	else
		newDerivedPosition.z = EventPosition.z - SpawnPosition.z;

	gameObject->SetWorldPosition(newDerivedPosition);
	
	gameObject->Update(gameTime);

	if(passedTime >= Duration)
		HideEffect();
}

SpecialEffect* SpecialEffect::Clone()
{
	SpecialEffect* clone = new SpecialEffect(*this);
	clone->gameObject = gameObject->Clone();

	return clone;
}

void SpecialEffect::Start()
{
	gameObject->Start();
}
