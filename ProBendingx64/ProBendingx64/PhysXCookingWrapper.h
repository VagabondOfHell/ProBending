#pragma once
#include "cooking/PxCooking.h"
#pragma comment(lib, "PhysX3CookingCHECKED_x64.lib")

///Wrapper used to easily include/exclude the PhysX Cooking library///
class PhysXCookingWrapper
{
public:
	static physx::PxCooking* cooking;

	///<summary>Creates the Cooking Library</summary>
	///<param name="PXVersion">The currently used PhysX Version</param>
	///<param name="foundation">The created PhysX Foundation</param>
	///<param name="parameters">Any customizations for the Cooking Library</param>
	///<returns>True if successful, false if not</returns>
	static bool CreateCooking(physx::PxU32 PXVersion, 
		physx::PxFoundation& foundation, physx::PxCookingParams& parameters);

	///<summary>Checks if the cooking library is valid</summary>
	///<returns>True if valid, false if not</returns>
	static inline bool CookingValid()
	{
		return cooking != NULL;
	}

	///<summary>Shuts the Cooking Library down if it was initialized</summary>
	static inline void ShutdownCookingLibrary()
	{
		if(CookingValid())
		{
			cooking->release();
			cooking = NULL;
		}
	}
};

