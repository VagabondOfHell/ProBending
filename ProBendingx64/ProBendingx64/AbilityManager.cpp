#include "AbilityManager.h"
#include "AbilityPrototypeDatabase.h"
#include "AbilityFactory.h"

#ifdef _DEBUG
#include "OgreLogManager.h"
#endif

AbilityManager::AbilityManager(void)
	:abilityFactory(new AbilityFactory())
{
}

AbilityManager::~AbilityManager(void)
{
	if(abilityFactory)
		delete abilityFactory;
	//Shared pointers take care of themselves, so no need to delete from here
}

AbilityManager::SharedAbilityDescriptor AbilityManager::
	CreateAbility(const ElementEnum::Element element, 
	const AbilityIDs::AbilityID abilityID, Probender* const caster)
{
	SharedAbilityDescriptor ability = abilityFactory->CreateAbility(element, abilityID, caster);
	ability->caster = caster;

	return ability;
}

void AbilityManager::RemoveAbility(SharedAbilityDescriptor abilityToRemove)
{
	
}

void AbilityManager::Initialize(ElementFlags::ElementFlag elementsToLoad)
{
#ifdef _DEBUG
	//If failed, indicate in the log
	if(!abilityFactory->LoadAbilities(elementsToLoad))
		Ogre::LogManager::getSingleton().logMessage
			(Ogre::LogMessageLevel::LML_CRITICAL, "Ability Factory Load Abilities Failed");
#else
	abilityFactory->LoadAbilities(elementsToLoad);
#endif

}

void AbilityManager::Update(const float gameTime)
{
	
}


