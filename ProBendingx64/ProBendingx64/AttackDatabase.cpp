#include "AttackDatabase.h"
#include "AttackGestureEvaluators.h"
#include "Attack.h"
#include "Controllers.h"

#include "ProjectileManager.h"

void AttackDatabase::GetEarthAttacks(ProjectileManager* projManager, unsigned short contestantID, std::vector<Attack>& outVal)
{
	outVal.reserve(5);

	AttackParams earthJabParams = AttackParams();

	AttackGesture* earthJabCreateGesture = new AttackGesture(projManager->GetOwningScene()->GetGUIManager());
	//earthJabCreateGesture->AddDiscreteEvaluator(0.0f, "Jab", "", GestureEnums::INVALID_GESTURE_SLOT, false, 0.8f, true, true);
	earthJabCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_EITHER, "", 
		GestureEnums::INVALID_GESTURE_SLOT);

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

	GestureEnums::GUIGestureSlot earthCoinGestureSlot;

	if(contestantID == 0)
		earthCoinGestureSlot = GestureEnums::P1_GESTURE_SLOT_1;
	else
		earthCoinGestureSlot = GestureEnums::P2_GESTURE_SLOT_1;

	AttackGesture* earthCoinCreateGesture = new AttackGesture(projManager->GetOwningScene()->GetGUIManager());
	earthCoinCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::KneeRaiseGesture, GestureEnums::BODYSIDE_EITHER,
		GestureEnums::EARTH_STOMP_BEGIN_IMAGE, earthCoinGestureSlot);
	earthCoinCreateGesture->AddCustomEvaluator(0.5f, &AttackGestureEvaluators::KneeDownGesture, GestureEnums::BODYSIDE_EITHER,
		GestureEnums::EARTH_STOMP_END_IMAGE, earthCoinGestureSlot, GestureEnums::TRANRULE_SAME);
	
	//Controller that uses the same hand of the knee to control
	HandMoveController* earthCoinController = new HandMoveController(NULL, HandMoveController::CH_RIGHT,
		physx::PxVec3(-PROBENDER_HALF_EXTENTS.x, -PROBENDER_HALF_EXTENTS.y * 0.5f, 0.0f), 
		physx::PxVec3(PROBENDER_HALF_EXTENTS.x, PROBENDER_HALF_EXTENTS.y, 0.0f), GestureEnums::TRANRULE_SAME);

	//Punch with opposite to the controlling hand to launch the rock
	AttackGesture* earthCoinLaunchGesture = new AttackGesture(projManager->GetOwningScene()->GetGUIManager());
	earthCoinLaunchGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_EITHER,
		GestureEnums::EARTH_STOMP_LAUNCH_IMAGE, earthCoinGestureSlot, GestureEnums::TRANRULE_OPPOSITE);

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

void AttackDatabase::GetFireAttacks(ProjectileManager* projManager, unsigned short contestantID, std::vector<Attack>& outVal)
{
	outVal.reserve(5);

	AttackParams fireJabParams = AttackParams();

	AttackGesture* fireJabCreateGesture = new AttackGesture(projManager->GetOwningScene()->GetGUIManager());
	fireJabCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_EITHER, "", 
		GestureEnums::INVALID_GESTURE_SLOT);
	//fireJabCreateGesture->AddDiscreteEvaluator(0.0f, "Jab", std::string(""), 
		//GestureEnums::INVALID_GESTURE_SLOT, false, 0.5f, true, true);

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

	GestureEnums::GUIGestureSlot fireBlastGestureSlot;

	if(contestantID == 0)
		fireBlastGestureSlot = GestureEnums::P1_GESTURE_SLOT_1;
	else
		fireBlastGestureSlot = GestureEnums::P2_GESTURE_SLOT_1;

	AttackGesture* fireBlastCreateGesture = new AttackGesture(projManager->GetOwningScene()->GetGUIManager());
	fireBlastCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmsWideGesture, GestureEnums::BODYSIDE_BOTH,
		GestureEnums::FIRE_BLAST_BEGIN_IMAGE, fireBlastGestureSlot);
	//fireBlastCreateGesture->AddDiscreteEvaluator(0.3f, "Fire_Blast_Begin", true, 0.7f);
	fireBlastCreateGesture->AddCustomEvaluator(0.5f, &AttackGestureEvaluators::HandsClapGesture, GestureEnums::BODYSIDE_BOTH,
		GestureEnums::FIRE_BLAST_END_IMAGE, fireBlastGestureSlot);

	AttackGesture* fireBlastLaunchGesture = new AttackGesture(projManager->GetOwningScene()->GetGUIManager());
	fireBlastLaunchGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_BOTH,
		GestureEnums::FIRE_BLAST_LAUNCH_IMAGE, fireBlastGestureSlot);
	
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

void AttackDatabase::GetAirAttacks(ProjectileManager* projManager, unsigned short contestantID, std::vector<Attack>& outVal)
{
	outVal = std::vector<Attack>();
}

void AttackDatabase::GetWaterAttacks(ProjectileManager* projManager, unsigned short contestantID, std::vector<Attack>& outVal)
{
	outVal.reserve(5);

	AttackParams waterJabParams = AttackParams();

	AttackGesture* waterJabCreateGesture = new AttackGesture(projManager->GetOwningScene()->GetGUIManager());
	waterJabCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_EITHER, "", 
		GestureEnums::INVALID_GESTURE_SLOT);
	//waterJabCreateGesture->AddDiscreteEvaluator(0.0f, "Jab", "", GestureEnums::INVALID_GESTURE_SLOT, false, 0.5f, true, true);

	ProjectileIdentifier waterJabID = ProjectileIdentifier();
	waterJabID.Element = ElementEnum::Water; waterJabID.AbilityID = AbilityIDs::WATER_JAB;

	waterJabParams.CreationGesture = waterJabCreateGesture;
	waterJabParams.LaunchGesture = NULL;
	waterJabParams.ProjController = NULL;
	waterJabParams.PositionCalculator = SpawnPositionCalculator(GestureEnums::TRANRULE_SAME, GestureEnums::BODYSIDE_EITHER,
		SpawnPositionCalculator::LIMB_HANDS, SpawnPositionCalculator::CALC_OPTIONS_JOINT_POSITION);
	waterJabParams.LaunchOnCreate = true;

	outVal.emplace_back(0.20f, projManager, waterJabID, waterJabParams);

	AttackParams waterRiseParams = AttackParams();

	GestureEnums::GUIGestureSlot waterRiseGestureSlot;

	if(contestantID == 0)
		waterRiseGestureSlot = GestureEnums::P1_GESTURE_SLOT_1;
	else
		waterRiseGestureSlot = GestureEnums::P2_GESTURE_SLOT_1;

	AttackGesture* waterRiseCreateGesture = new AttackGesture(projManager->GetOwningScene()->GetGUIManager());
	waterRiseCreateGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::HandsToSide, GestureEnums::BODYSIDE_BOTH,
		GestureEnums::WATER_RISE_BEGIN_IMAGE, waterRiseGestureSlot);
	waterRiseCreateGesture->AddCustomEvaluator(0.5f, &AttackGestureEvaluators::HandsOverHead, GestureEnums::BODYSIDE_BOTH,
		GestureEnums::WATER_RISE_END_IMAGE, waterRiseGestureSlot);

	AttackGesture* waterRiseLaunchGesture = new AttackGesture(projManager->GetOwningScene()->GetGUIManager());
	waterRiseLaunchGesture->AddCustomEvaluator(0.0f, &AttackGestureEvaluators::ArmPunchGesture, GestureEnums::BODYSIDE_BOTH,
		GestureEnums::WATER_RISE_LAUNCH_IMAGE, waterRiseGestureSlot);

	ProjectileIdentifier waterRiseID = ProjectileIdentifier();
	waterRiseID.Element = ElementEnum::Water; waterRiseID.AbilityID = AbilityIDs::WATER_RISE;

	HandMoveController* waterRiseController = new HandMoveController(NULL, HandMoveController::CH_BOTH,
		physx::PxVec3(-PROBENDER_HALF_EXTENTS.x, -PROBENDER_HALF_EXTENTS.y, 0.0f), 
		physx::PxVec3(PROBENDER_HALF_EXTENTS.x, PROBENDER_HALF_EXTENTS.y * 0.5f, 0.0f), GestureEnums::TRANRULE_SAME);

	waterRiseParams.CreationGesture = waterRiseCreateGesture;
	waterRiseParams.LaunchGesture = waterRiseLaunchGesture;
	waterRiseParams.ProjController = waterRiseController;
	waterRiseParams.PositionCalculator = SpawnPositionCalculator(GestureEnums::TRANRULE_SAME, GestureEnums::BODYSIDE_BOTH,
		SpawnPositionCalculator::LIMB_HANDS, SpawnPositionCalculator::CALC_OPTIONS_AVERAGE);

	outVal.emplace_back(0.20f, projManager, waterRiseID, waterRiseParams);
}
