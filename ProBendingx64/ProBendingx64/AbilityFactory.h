#pragma once
#include "ProbenderFlags.h"
#include "AbilityPrototypeDatabase.h"

class Probender;
class AbilityDescriptor;

class AbilityFactory
{
public:
	AbilityFactory();
	~AbilityFactory(void);

	///<summary>Loads the abilities of the specified elements into the Prototype Database</summary>
	///<param name="elementsToLoad">Bitflag representing the elements to load</param>
	///<returns>True if successful, false if something went wrong</returns>
	inline bool LoadAbilities(const ElementFlags::ElementFlag elementsToLoad = ElementFlags::All)const
	{
		return AbilityPrototypeDatabase::LoadFromCode(elementsToLoad);
	}

	///<summary>Creates the specified ability and applies the character bonuses to the values</summary>
	///<param name="element">The element that the ability belongs to</param>
	///<param name="abilityID">The ID of the ability to clone</param>
	///<param name="caster">The caster creating the ability</param>
	///<returns>A shared pointer containing the cloned ability</returns>
	AbilityPrototypeDatabase::SharedAbilityDescriptor CreateAbility(const ElementEnum::Element element, 
		const AbilityIDs::AbilityID abilityID, Probender* const caster)const;
};

