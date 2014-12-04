#pragma once
#include <vector>
#include "AbilityDescriptor.h"
#include <memory>

///Multimap sorted by Ability Type for Abilities List?

class Probender;
class AbilityFactory;

class AbilityManager
{
public:
	//Easy typedef for a shared pointer of Ability Descriptor
	typedef std::shared_ptr<AbilityDescriptor> SharedAbilityDescriptor;

private:
	std::vector<SharedAbilityDescriptor> abilitiesList;
	AbilityFactory* abilityFactory;

public:
	AbilityManager(void);
	~AbilityManager(void);

	///<summary>Adds an ability to the manager. THIS MAY NOT BE NEEDED AND SHOULD PROBABLY BE REMOVED</summary>
	///<param name="abilityToAdd">The ability to add to the manager</param>
	void inline AddAbility(SharedAbilityDescriptor abilityToAdd){abilitiesList.push_back(abilityToAdd);}
	
	///<summary>Gets the shared pointer created by the Ability Factory, adds it to the manager, and returns it.
	///If the factory returns an invalid result, it is not added to the manager.</summary>
	///<param name="element">The element that the desired ability belongs to</param>
	///<param name="abilityID">The ID representing the ability name</param>
	///<param name="caster">The Probender casting the ability</param>
	///<returns>A shared pointer containing the ability descriptor</returns>
	AbilityManager::SharedAbilityDescriptor CreateAbility(const ElementEnum::Element element, 
		const AbilityIDs::AbilityID abilityID, Probender* const caster);

	///<summary>Removes the specified ability from the vector</summary>
	///<param name="abilityToRemove">The ability to remove</param>
	void RemoveAbility(SharedAbilityDescriptor abilityToRemove);

	void Initialize(ElementFlags::ElementFlag elementsToLoad = ElementFlags::All);

	///<summary>Updates the ability manager. This consists of looping and checking all the
	///abilities, performing their effects, and removing them if they should now be removed</summary>
	///<param name="gameTime">The game time that has passed between frames</param>
	void Update(const float gameTime);
};

