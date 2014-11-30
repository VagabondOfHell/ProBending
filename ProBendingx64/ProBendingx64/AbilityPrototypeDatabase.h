#pragma once
#include <map>
#include "AbilityDescriptor.h"
#include "ProbenderFlags.h"

class AbilityPrototypeDatabase
{
private:
	
public:
	
	//static const int NUM_ENTRIES = 1;
	enum AbilityID{Fire_Jab, Count};

private:
	static std::map<AbilityID, AbilityDescriptor> abilitiesDictionary;//Dictionary of the abilities

	AbilityPrototypeDatabase(void);//Hide constructor

public:
	
	~AbilityPrototypeDatabase(void);

	///<summary>Loads the data from this method, which is filled with the code version of the data</summary>
	///<param name="elementTypeToLoad">Collection of bits that represent which elements to load</param>
	///<returns>True all the time</returns>
	static bool LoadFromCode(ElementFlags::ElementFlags elementTypeToLoad);

	///<summary>Loads the data from the specified XML filepath</summary>
	///<param name="xmlFilePath">The path to the xml file, including extension</param>
	///<returns>True if successful, false if not</returns>
	static bool LoadFromXMLFile(std::string xmlFilePath, ElementFlags::ElementFlags elementTypeToLoad);

	///<summary>Loads the data from the specified text file</summary>
	///<param name="textFilePath">The path to the text file, including extension</param>
	///<returns>True if successful, false if not</returns>
	static bool LoadFromTextFile(std::string textFilePath, ElementFlags::ElementFlags elementTypeToLoad);

	///<summary>Gets a clone of the specified ability</summary>
	///<param name="abilityID">The ID of the ability to retrieve. If not loaded correctly,
	///Count is supplied, or the element of the ability wasn't loaded, an invalid structure is returned</param>
	///<returns>The copy of the specified ability</returns>
	static AbilityDescriptor GetAbilityClone(AbilityID abilityID);

};

