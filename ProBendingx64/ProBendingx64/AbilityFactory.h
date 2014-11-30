#pragma once
#include "ProbenderFlags.h"
#include "AbilityPrototypeDatabase.h"

///Does this work?
///Also, set this to be a friend to the factory, so only the factory can use the Prototype Database
//class AbilityPrototypeDatabase
//{
//public:
//	enum AbilityID;
//};

class Probender;
class AbilityDescriptor;

class AbilityFactory
{
public:
	AbilityFactory();
	AbilityFactory(ElementFlags::ElementFlags elementsToLoad);
	~AbilityFactory(void);

	AbilityDescriptor CreateAbility(AbilityPrototypeDatabase::AbilityID abilityID, Probender* const caster)const;
};

