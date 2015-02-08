#include "PhysXCookingWrapper.h"

physx::PxCooking* PhysXCookingWrapper::cooking = NULL;

bool PhysXCookingWrapper::CreateCooking(physx::PxU32 PXVersion, 
			physx::PxFoundation& foundation, physx::PxCookingParams& parameters)
{
	if(!CookingValid())
	{
		cooking = PxCreateCooking(PXVersion, foundation, parameters);
	}

	return CookingValid();
}