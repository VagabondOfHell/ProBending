#include "AttackDatabase.h"
#include "AttackGestureEvaluators.h"
#include "Attack.h"
#include "Controllers.h"

void AttackDatabase::GetEarthAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal)
{
	outVal.reserve(5);

	AttackParams earthJabParams = AttackParams();

	AttackGesture* earthJabCreateGesture = new AttackGesture();
	earthJabCreateGesture->AddDiscreteEvaluator(0.0f, "Jab", false, 0.5f, true, true);
	//earthJabCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_EITHER);

	ProjectileIdentifier earthJabID = ProjectileIdentifier();
	earthJabID.Element = ElementEnum::Earth; earthJabID.AbilityID = AbilityIDs::EARTH_JAB;

	earthJabParams.CreationGesture = earthJabCreateGesture;
	earthJabParams.LaunchGesture = NULL;
	earthJabParams.ProjController = NULL;
	earthJabParams.PositionCalculator = SpawnPositionCalculator(GestureEnums::TRANRULE_SAME, GestureEnums::BODYSIDE_EITHER,
		SpawnPositionCalculator::LIMB_HANDS, SpawnPositionCalculator::CALC_OPTIONS_JOINT_POSITION);
	earthJabParams.LaunchOnCreate = true;

	outVal.emplace_back(0.20f, projManager, earthJabID, earthJabParams);

	AttackParams earthCoinParams = AttackParams();

	AttackGesture* earthCoinCreateGesture = new AttackGesture();
	earthCoinCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::KneeRaiseGesture, GestureEnums::BODYSIDE_EITHER);
	earthCoinCreateGesture->AddCustomEvaluator(0.5f, &AttackGestureEvaluators::KneeDownGesture, GestureEnums::BODYSIDE_EITHER, 
		GestureEnums::TRANRULE_SAME);
	
	//Controller that uses the same hand of the knee to control
	HandMoveController* earthCoinController = new HandMoveController(NULL, HandMoveController::CH_RIGHT,
		physx::PxVec3(-PROBENDER_HALF_EXTENTS.x, -PROBENDER_HALF_EXTENTS.y * 0.5f, 0.0f), 
		physx::PxVec3(PROBENDER_HALF_EXTENTS.x, PROBENDER_HALF_EXTENTS.y, 0.0f), GestureEnums::TRANRULE_SAME);

	//Punch with opposite to the controlling hand to launch the rock
	AttackGesture* earthCoinLaunchGesture = new AttackGesture();
	earthCoinLaunchGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_EITHER,
		GestureEnums::TRANRULE_OPPOSITE);

	ProjectileIdentifier earthCoinID = ProjectileIdentifier();
	earthCoinID.Element = ElementEnum::Earth; earthCoinID.AbilityID = AbilityIDs::EARTH_COIN;

	earthCoinParams.CreationGesture = earthCoinCreateGesture;
	earthCoinParams.LaunchGesture = earthCoinLaunchGesture;
	earthCoinParams.ProjController = earthCoinController;
	earthCoinParams.PositionCalculator = SpawnPositionCalculator(GestureEnums::TRANRULE_SAME, GestureEnums::BODYSIDE_EITHER,
		SpawnPositionCalculator::LIMB_HANDS, SpawnPositionCalculator::CALC_OPTIONS_JOINT_POSITION);

	//Use emplace to avoid the destructor being called (we could implement move or copy semantics to do deep copies,
	//but it's more efficient this way). May have to change this if problems in the future
	outVal.emplace_back(1.0f, projManager, earthCoinID, earthCoinParams);

}

void AttackDatabase::GetFireAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal)
{
	outVal.reserve(5);

	AttackParams fireJabParams = AttackParams();

	AttackGesture* fireJabCreateGesture = new AttackGesture();
	//fireJabCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_EITHER);
	fireJabCreateGesture->AddDiscreteEvaluator(0.0f, "Jab", false, 0.5f, true, true);

	ProjectileIdentifier fireJabID = ProjectileIdentifier();
	fireJabID.Element = ElementEnum::Fire; fireJabID.AbilityID = AbilityIDs::FIRE_JAB;

	fireJabParams.CreationGesture = fireJabCreateGesture;
	fireJabParams.LaunchGesture = NULL;
	fireJabParams.ProjController = NULL;
	fireJabParams.PositionCalculator = SpawnPositionCalculator(GestureEnums::TRANRULE_SAME, GestureEnums::BODYSIDE_EITHER,
		SpawnPositionCalculator::LIMB_HANDS, SpawnPositionCalculator::CALC_OPTIONS_JOINT_POSITION);
	fireJabParams.LaunchOnCreate = true;

	outVal.emplace_back(0.20f, projManager, fireJabID, fireJabParams);

	AttackParams fireBlastParams = AttackParams();

	AttackGesture* fireBlastCreateGesture = new AttackGesture();
	fireBlastCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmsWideGesture, GestureEnums::BODYSIDE_BOTH);
	//fireBlastCreateGesture->AddDiscreteEvaluator(0.3f, "Fire_Blast_Begin", true, 0.7f);
	fireBlastCreateGesture->AddCustomEvaluator(0.5f, &AttackGestureEvaluators::HandsClapGesture, GestureEnums::BODYSIDE_BOTH);

	AttackGesture* fireBlastLaunchGesture = new AttackGesture();
	fireBlastLaunchGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_BOTH);
	
	//HandMoveController* fireBlastController = new HandMoveController(NULL, HandMoveController::CH_BOTH,
	//	physx::PxVec3(-0.5f, -0.5f, 0.0f), physx::PxVec3(0.5f, 0.5f, 0.5f), GestureEnums::TRANRULE_SAME);
	HandMoveController* fireBlastController = new HandMoveController(NULL, HandMoveController::CH_RIGHT,
		physx::PxVec3(-PROBENDER_HALF_EXTENTS.x, -PROBENDER_HALF_EXTENTS.y * 0.5f, 0.0f), 
		physx::PxVec3(PROBENDER_HALF_EXTENTS.x, PROBENDER_HALF_EXTENTS.y, 0.0f), GestureEnums::TRANRULE_SAME);

	ProjectileIdentifier fireBlastID = ProjectileIdentifier();
	fireBlastID.Element = ElementEnum::Fire; fireBlastID.AbilityID = AbilityIDs::FIRE_BLAST;

	fireBlastParams.CreationGesture = fireBlastCreateGesture;
	fireBlastParams.LaunchGesture = fireBlastLaunchGesture;
	fireBlastParams.ProjController = fireBlastController;
	fireBlastParams.PositionCalculator = SpawnPositionCalculator(GestureEnums::TRANRULE_SAME, GestureEnums::BODYSIDE_BOTH,
		SpawnPositionCalculator::LIMB_HANDS, SpawnPositionCalculator::CALC_OPTIONS_AVERAGE);
	fireBlastParams.LaunchOnCreate = false;

	outVal.emplace_back(0.20f, projManager, fireBlastID, fireBlastParams);
}

void AttackDatabase::GetAirAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal)
{
	outVal = std::vector<Attack>();
}

void AttackDatabase::GetWaterAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal)
{
	outVal = std::vector<Attack>();
}
