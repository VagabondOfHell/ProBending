#include "AbilityPrototypeDatabase.h"
#include "NotImplementedException.h"
#include "BuffAbilityEffect.h"
#include "CreationAbilityEffect.h"
#include "DecoratorAbilityEffect.h"
#include "StatusAbilityEffect.h"

//Dictionary of the abilities (sorted by element, then ability ID)
std::map<ElementEnum::Element, AbilityPrototypeDatabase::AbilityMap>
	AbilityPrototypeDatabase::abilitiesDictionary;

bool AbilityPrototypeDatabase::LoadFromCode(ElementFlags::ElementFlag elementTypeToLoad)
{
	AbilityDescriptor nextAbility(NULL);

	if(elementTypeToLoad & ElementFlags::Fire)
	{
		//Create the element to add to the dictionary
		std::pair<ElementDictionaryIterator, bool> result = 
			abilitiesDictionary.insert( ElementDictionaryValue(ElementEnum::Fire, AbilityMap()));

		//If insertion of the fire element was successful (which it should be)
		if(result.second)
		{
			nextAbility = AbilityDescriptor(NULL, AbilityDescriptor::Offensive, 0.0f, 
				ElementEnum::Fire, AbilityDescriptor::CollidedTarget);
			///ADD EFFECTS HERE///
			nextAbility.abilityEffects.push_back
				(SharedAbilityEffect(new StatusAbilityEffect(0.0f, StatusAbilityEffect::NormalDamage, 2.0f)));

			//Get the iterator from the result of the insertion of the element for direct access
			//to this elements' map. Then insert into the Ability Map
			result.first->second.insert(AbilityDictionaryValue(AbilityIDs::FIRE_JAB, nextAbility));
		}
	}
	
	if(elementTypeToLoad & ElementFlags::Earth)
	{
		//Create the element to add to the dictionary
		std::pair<ElementDictionaryIterator, bool> result = 
			abilitiesDictionary.insert( ElementDictionaryValue(ElementEnum::Earth, AbilityMap()));

		//If insertion of the earth element was successful (which it should be)
		if(result.second)
		{
			nextAbility = AbilityDescriptor(NULL, AbilityDescriptor::Offensive, 0.0f, 
				ElementEnum::Earth, AbilityDescriptor::CollidedTarget);
			///ADD EFFECTS HERE///

			//Get the iterator from the result of the insertion of the element for direct access
			//to this elements' map. Then insert into the Ability Map
			result.first->second.insert(AbilityDictionaryValue(AbilityIDs::EARTH_BOULDER, nextAbility));
		}
	}

	return true;
}

bool AbilityPrototypeDatabase::LoadFromXMLFile(std::string xmlFilePath, ElementFlags::ElementFlag elementTypeToLoad)
{
	throw NotImplementedException();

	return false;
}

bool AbilityPrototypeDatabase::LoadFromTextFile(std::string textFilePath, ElementFlags::ElementFlag elementTypeToLoad)
{
	throw NotImplementedException();

	return false;
}

AbilityPrototypeDatabase::SharedAbilityDescriptor AbilityPrototypeDatabase::
	GetAbilityClone(const ElementEnum::Element element, const AbilityIDs::AbilityID abilityID)
{
	//If invalid input, return nullptr
	if(element == ElementEnum::InvalidElement)
		return SharedAbilityDescriptor(nullptr);

	//Use Find function on map, passing element as the key
	ElementDictionaryIterator elementResult = abilitiesDictionary.find(element);

	//If valid search result
	if(elementResult != abilitiesDictionary.end())
	{
		//Use find function on the nested map, passing ability ID as key
		AbilityDictionaryIterator abilityResult = elementResult->second.find(abilityID);

		//If valid search result
		if(abilityResult != elementResult->second.end())
		{
			//Return a clone of the ability
			return SharedAbilityDescriptor(abilityResult->second.Clone());
		}
	}

	//If we reach here, a search failed and we should return a null ptr
	return SharedAbilityDescriptor(nullptr);
}
