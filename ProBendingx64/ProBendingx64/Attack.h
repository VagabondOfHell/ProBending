#pragma once
#include "AttackGesture.h"
#include "ProjectileController.h"
#include "Projectile.h"

class ProjectileManager;

struct SpawnPositionCalculator
{
public:
	GestureEnums::TransitionRules TransitionFromCreation;

	enum SpawnLimb{LIMB_NONE, LIMB_HANDS, LIMB_FEET, LIMB_HEAD};
	SpawnLimb Limb;
	
	GestureEnums::BodySide ValidSides;

	enum CalculationOptions{CALC_OPTIONS_NONE, CALC_OPTIONS_JOINT_POSITION, CALC_OPTIONS_AVERAGE};
	CalculationOptions CalculationOption;

	SpawnPositionCalculator( GestureEnums::TransitionRules transition = GestureEnums::TRANRULE_NONE, 
		GestureEnums::BodySide validSides = GestureEnums::BODYSIDE_INVALID, SpawnLimb limb = LIMB_NONE,
		CalculationOptions options = CALC_OPTIONS_NONE)
		: TransitionFromCreation(transition), Limb(limb), CalculationOption(options), 
		ValidSides(validSides)
	{

	}

	Ogre::Vector3 CalculateSpawnPoint(GestureEnums::BodySide sideToUse, const AttackData& attackData)
	{
		if(ValidateSide(sideToUse))
		{
			switch (Limb)
			{
			case SpawnPositionCalculator::LIMB_HANDS:
				if(CalculationOption == CALC_OPTIONS_JOINT_POSITION)
				{
					if(sideToUse == GestureEnums::BODYSIDE_LEFT)
						return GetOgrePositionRelative(attackData.CurrentData->JointData[JointType_HandLeft].Position,
							attackData.CurrentData->JointData[JointType_SpineBase].Position);
					else if(sideToUse == GestureEnums::BODYSIDE_RIGHT)
						return GetOgrePositionRelative(attackData.CurrentData->JointData[JointType_HandRight].Position,
							attackData.CurrentData->JointData[JointType_SpineBase].Position);
				}
				else if(CalculationOption == CALC_OPTIONS_AVERAGE)
				{
					CameraSpacePoint spineBase = attackData.CurrentData->JointData[JointType_SpineBase].Position;
					CameraSpacePoint leftHand = attackData.CurrentData->JointData[JointType_HandLeft].Position;
					//leftHand.X -= spineBase.X; leftHand.Y -= spineBase.Y; leftHand.Z -= spineBase.Z;
					Ogre::Vector3 leftHandDiff = GetOgrePositionRelative(attackData.CurrentData->JointData[JointType_HandLeft].Position,
						spineBase);
					Ogre::Vector3 rightHandDiff = GetOgrePositionRelative(attackData.CurrentData->JointData[JointType_HandRight].Position,
						spineBase);

					return ((rightHandDiff + leftHandDiff) * 0.5f);
				}
				break;
			case SpawnPositionCalculator::LIMB_FEET:
				break;
			case SpawnPositionCalculator::LIMB_HEAD:
				break;
			default:
				break;
			}
		}
		
		return Ogre::Vector3(0.0f);
	}

private:
	bool ValidateSide(GestureEnums::BodySide sideToUse)
	{
		switch (ValidSides)
		{
		case GestureEnums::BODYSIDE_LEFT:
			if(sideToUse == GestureEnums::BODYSIDE_LEFT)
				return true;
			break;
		case GestureEnums::BODYSIDE_RIGHT:
			if(sideToUse == GestureEnums::BODYSIDE_RIGHT)
				return true;
			break;
		case GestureEnums::BODYSIDE_EITHER:
			if(sideToUse == GestureEnums::BODYSIDE_LEFT || 
				sideToUse == GestureEnums::BODYSIDE_RIGHT)
				return true;
			break;
		case GestureEnums::BODYSIDE_BOTH:
			if(sideToUse == GestureEnums::BODYSIDE_BOTH)
				return true;
			break;
		default:
			return false;
			break;
		}

		return false;
	}
	Ogre::Vector3 GetOgrePosition(const CameraSpacePoint& kinectPosition)
	{
		return Ogre::Vector3(kinectPosition.X, kinectPosition.Y, kinectPosition.Z);
	}
	Ogre::Vector3 GetOgrePositionRelative(const CameraSpacePoint& limbPosition, const CameraSpacePoint& referencePoint);
	
};

struct AttackParams
{
	AttackGesture* CreationGesture;
	ProjectileController* ProjController;
	AttackGesture* LaunchGesture;
	SpawnPositionCalculator PositionCalculator;
	bool LaunchOnCreate;

	AttackParams():CreationGesture(NULL), ProjController(NULL), LaunchGesture(NULL), LaunchOnCreate(false)
	{

	}
};

class Attack
{
public:
	enum AttackState{AS_NONE, AS_CREATED, AS_CONTROLLED, AS_LAUNCHED};

private:
	AttackGesture* creationGesture;
	ProjectileController* projectileController;
	AttackGesture* launchGesture;
	SpawnPositionCalculator spawnPositionCalculator;

	Projectile* projectile;

	AttackState currentState;

	ProjectileIdentifier projectileIdentifier;

	ProjectileManager* projectileManager;

	GestureEnums::BodySide bodySideResult;
	bool spawnPositionValid;
	Ogre::Vector3 spawnPosition;

	float cooldownTimePassed;
	
public:
	bool LaunchOnCreate;

	float AttackCooldown;

	Attack( float attackCooldown = 0.0f, ProjectileManager* projManager = NULL,
		ProjectileIdentifier projID = ProjectileIdentifier(),
		AttackGesture* creationGesture = NULL, ProjectileController* controller = NULL, AttackGesture* launchGesture = NULL);

	Attack(float attackCooldown, ProjectileManager* projManager,
		ProjectileIdentifier projID = ProjectileIdentifier(), AttackParams params = AttackParams());

	~Attack(void);

	///NOT FINISHED///
	void Update(float gameTime);

	///NOT FINISHED///
	///<summary>Evaluates the body data and checks if an action should be performed</summary>
	///<param name="bodyData">The body data to evaluate</param>
	///<returns>The new state if there was a state change, or AS_NONE if no state change</returns>
	AttackState Evaluate(const AttackData& bodyData);

	inline ProjectileIdentifier GetProjectileID()const{return projectileIdentifier;}

	inline void SetActiveProjectile(Projectile* proj, bool calcOrigin = false)
	{
		if(!projectileController)
		{
			projectile = proj;
			return;
		}
		else
			projectileController->projectile = proj;

		if(calcOrigin && proj)
			projectileController->ProjectileOrigin = HelperFunctions::OgreToPhysXVec3(proj->GetWorldPosition());
	}

	inline Projectile* GetProjectile()const{
		if(projectileController)
			return projectileController->projectile;
		else
			return projectile;}

	inline bool SpawnPositionValid()const{return spawnPositionValid;}
	inline Ogre::Vector3 GetSpawnPosition()const{return spawnPosition;}

	///NOT FINISHED
	void Reset();
};

