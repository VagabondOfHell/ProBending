#pragma once
#include <map>
#include "AbilityDescriptor.h"
#include "ProbenderFlags.h"

class AbilityPrototypeDatabase
{
	friend class AbilityFactory;

public:
	//Easy typedef for a shared pointer of Ability Descriptor
	typedef std::shared_ptr<AbilityDescriptor> SharedAbilityDescriptor;

private:
	typedef std::map<AbilityIDs::AbilityID, AbilityDescriptor> AbilityMap; //Shortcut to second half of map (ability ID, Descriptor)
	typedef std::map<ElementEnum::Element, AbilityMap>::value_type ElementDictionaryValue;//Shortcut to ValueType of first half of map
	typedef AbilityMap::value_type AbilityDictionaryValue;//Shortcut to ValueType of second half of map
	typedef std::map<ElementEnum::Element, AbilityMap>::iterator ElementDictionaryIterator;//Shortcut to iterator of first half of map
	typedef AbilityMap::iterator AbilityDictionaryIterator;//Shortcut to iterator of second half of map

	//Dictionary of the abilities(sorted by element, then ability ID)
	static std::map<ElementEnum::Element, AbilityMap> abilitiesDictionary;

	///<summary>Loads the data from this method, which is filled with the code version of the data</summary>
	///<param name="elementTypeToLoad">Collection of bits that represent which elements to load</param>
	///<returns>True all the time</returns>
	static bool LoadFromCode(ElementFlags::ElementFlag elementTypeToLoad);

	///<summary>Loads the data from the specified XML filepath</summary>
	///<param name="xmlFilePath">The path to the xml file, including extension</param>
	///<returns>True if successful, false if not</returns>
	static bool LoadFromXMLFile(std::string xmlFilePath, ElementFlags::ElementFlag elementTypeToLoad);

	///<summary>Loads the data from the specified text file</summary>
	///<param name="textFilePath">The path to the text file, including extension</param>
	///<returns>True if successful, false if not</returns>
	static bool LoadFromTextFile(std::string textFilePath, ElementFlags::ElementFlag elementTypeToLoad);

	///<summary>Gets a clone of the specified ability</summary>
	///<param name="element">The element the ability pertains to</param>
	///<param name="abilityID">The ID of the ability, as specified in the ProbenderFlags.h header</param>
	///<returns>A shared pointer containing a clone of the discovered ability, or nullptr if search failed</returns>
	static SharedAbilityDescriptor GetAbilityClone(const ElementEnum::Element element, const AbilityIDs::AbilityID abilityID);

};

