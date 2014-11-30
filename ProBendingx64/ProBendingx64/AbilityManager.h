#pragma once
#include <vector>
#include "AbilityDescriptor.h"
#include <memory>

class Probender;

//Easy typedef for a shared pointer of Ability Descriptor
typedef std::shared_ptr<AbilityDescriptor> SharedAbilityDescriptor;

class AbilityManager
{


private:
	std::vector<SharedAbilityDescriptor> abilitiesList;
	
public:
	AbilityManager(void);
	~AbilityManager(void);

	///<summary>Adds an ability to the manager. THIS MAY NOT BE NEEDED AND SHOULD PROBABLY BE REMOVED</summary>
	///<param name="abilityToAdd">The ability to add to the manager</param>
	void inline AddAbility(SharedAbilityDescriptor abilityToAdd){abilitiesList.push_back(abilityToAdd);}
	
	///<summary>Gets the shared pointer created by the Ability Factory, adds it to the manager, and returns it.
	///If the factory returns an invalid result, it is not added to the manager.</summary>
	///<param name="abilityName">The name of the ability to have the factory create</param>
	///<param name="caster">The caster of the ability, so that the attributes of the ability can be appropriately modified</param>
	///<returns>A shared pointer representing the created ability. If the factory could not produce the ability, it is NULL</returns>
	SharedAbilityDescriptor CreateAbility(const std::string abilityName, Probender* const caster);

	///<summary>Removes the specified ability from the vector</summary>
	///<param name="abilityToRemove">The ability to remove</param>
	void RemoveAbility(SharedAbilityDescriptor abilityToRemove);

	///<summary>Updates the ability manager. This consists of looping and checking all the
	///abilities, performing their effects, and removing them if they should now be removed</summary>
	///<param name="gameTime">The game time that has passed between frames</param>
	void Update(const float gameTime);
};

