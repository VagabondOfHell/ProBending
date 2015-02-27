#include "AttackDatabase.h"
#include "AttackGestureEvaluators.h"
#include "Attack.h"
#include "Controllers.h"

void AttackDatabase::GetEarthAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal)
{
	AttackParams earthCoinParams = AttackParams();

	AttackGesture* earthCoinCreateGesture = new AttackGesture();
	earthCoinCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::KneeRaiseGesture, GestureEnums::BODYSIDE_EITHER);
	earthCoinCreateGesture->AddCustomEvaluator(0.5f, &AttackGestureEvaluators::KneeDownGesture, GestureEnums::BODYSIDE_EITHER, 
		GestureEnums::TRANRULE_SAME);
	
	//Controller that uses the same hand of the knee to control
	HandMoveController* earthCoinController = new HandMoveController(NULL, HandMoveController::CH_RIGHT,
		physx::PxVec3(-2.0f, 0.0f, 0.0f), physx::PxVec3(2.0f, 2.0f, 2.0f), GestureEnums::TRANRULE_SAME);

	//Punch with opposite to the controlling hand to launch the rock
	AttackGesture* earthCoinLaunchGesture = new AttackGesture();
	earthCoinLaunchGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_EITHER,
		GestureEnums::TRANRULE_OPPOSITE);

	ProjectileIdentifier earthCoinID = ProjectileIdentifier();
	earthCoinID.Element = ElementEnum::Earth; earthCoinID.AbilityID = AbilityIDs::EARTH_COIN;

	earthCoinParams.CreationGesture = earthCoinCreateGesture;
	earthCoinParams.LaunchGesture = earthCoinLaunchGesture;
	earthCoinParams.ProjController = earthCoinController;

	//Use emplace to avoid the destructor being called (we could implement move or copy semantics to do deep copies,
	//but it's more efficient this way). May have to change this if problems in the future
	outVal.emplace_back(1.0f, projManager, earthCoinID, earthCoinParams);
}

void AttackDatabase::GetFireAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal)
{
	outVal = std::vector<Attack>();
}

void AttackDatabase::GetAirAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal)
{
	outVal = std::vector<Attack>();
}

void AttackDatabase::GetWaterAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal)
{
	outVal = std::vector<Attack>();
}
