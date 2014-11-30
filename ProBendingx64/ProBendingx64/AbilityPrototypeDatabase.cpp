#include "AbilityPrototypeDatabase.h"
#include "NotImplementedException.h"

//Dictionary of the abilities
std::map<AbilityPrototypeDatabase::AbilityID, AbilityDescriptor> 
	AbilityPrototypeDatabase::abilitiesDictionary;

AbilityPrototypeDatabase::AbilityPrototypeDatabase(void)
{
}

AbilityPrototypeDatabase::~AbilityPrototypeDatabase(void)
{
}

bool AbilityPrototypeDatabase::LoadFromCode(ElementFlags::ElementFlags elementTypeToLoad)
{
	AbilityDescriptor nextAbility(NULL);

	if(elementTypeToLoad & ElementFlags::Fire)
	{
		nextAbility = AbilityDescriptor(NULL, 0.0f, ElementEnum::Fire, AbilityDescriptor::CollidedTarget);
		///ADD EFFECTS HERE///


		abilitiesDictionary[Fire_Jab] = nextAbility;
	}

	return true;
}

bool AbilityPrototypeDatabase::LoadFromXMLFile(std::string xmlFilePath, ElementFlags::ElementFlags elementTypeToLoad)
{
	throw NotImplementedException();

	return false;
}

bool AbilityPrototypeDatabase::LoadFromTextFile(std::string textFilePath, ElementFlags::ElementFlags elementTypeToLoad)
{
	throw NotImplementedException();

	return false;
}

AbilityDescriptor AbilityPrototypeDatabase::GetAbilityClone(AbilityID abilityID)
{
	//Validate selection
	if(abilityID == AbilityID::Count)
		return AbilityDescriptor();

	throw NotImplementedException();
}