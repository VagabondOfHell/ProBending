#include "AttackDatabase.h"
#include "AttackGestureEvaluators.h"
#include "Attack.h"
#include "Controllers.h"

void AttackDatabase::GetEarthAttacks(std::vector<Attack>& outVal)
{
	AttackGesture* earthCoinCreateGesture = new AttackGesture();
	earthCoinCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::KneeRaiseGesture, GestureEnums::BODYSIDE_EITHER);
	earthCoinCreateGesture->AddCustomEvaluator(0.5f, &AttackGestureEvaluators::KneeDownGesture, GestureEnums::BODYSIDE_EITHER, 
		GestureEnums::TRANRULE_SAME);
	
	//Controller that uses the same hand of the knee to control
	HandMoveController* earthCoinController = new HandMoveController(NULL, HandMoveController::CH_RIGHT,
		physx::PxVec3(-2.0f, -2.0f, -2.0f), physx::PxVec3(2.0f, 2.0f, 2.0f), GestureEnums::TRANRULE_SAME);

	//Punch with opposite to the controlling hand to launch the rock
	AttackGesture* earthCoinLaunchGesture = new AttackGesture();
	earthCoinLaunchGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_EITHER,
		GestureEnums::TRANRULE_OPPOSITE);

	ProjectileIdentifier earthCoinID = ProjectileIdentifier();
	earthCoinID.Element = ElementEnum::Earth; earthCoinID.AbilityID = AbilityIDs::EARTH_COIN;

	//Use emplace to avoid the destructor being called (we could implement move or copy semantics to do deep copies,
	//but it's more efficient this way). May have to change this if problems in the future
	outVal.emplace_back(1.0f, earthCoinID, earthCoinCreateGesture, earthCoinController, earthCoinLaunchGesture);
}

void AttackDatabase::GetFireAttacks(std::vector<Attack>& outVal)
{
	outVal = std::vector<Attack>();
}

void AttackDatabase::GetAirAttacks(std::vector<Attack>& outVal)
{
	outVal = std::vector<Attack>();
}

void AttackDatabase::GetWaterAttacks(std::vector<Attack>& outVal)
{
	outVal = std::vector<Attack>();
}
