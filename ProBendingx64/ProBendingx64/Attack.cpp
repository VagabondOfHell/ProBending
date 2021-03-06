#include "Attack.h"
#include "Probender.h"
#include "ProjectileManager.h"
#include "Projectile.h"
#include "RigidBodyComponent.h"

Attack::Attack(float attackCooldown /*= 0.0f*/, ProjectileManager* projManager/* = NULL*/,
			   ProjectileIdentifier projID /*= ProjectileIdentifier()*/, AttackGesture* _creationGesture /*= NULL*/,
			   ProjectileController* controller /*= NULL*/, AttackGesture* _launchGesture /*= NULL*/)
	:creationGesture(_creationGesture), launchGesture(_launchGesture), currentState(AS_NONE), projectileController(controller),
		AttackCooldown(attackCooldown), cooldownTimePassed(0.0f), projectileIdentifier(projID),
		projectileManager(projManager), LaunchOnCreate(false), spawnPositionValid(false), 
		bodySideResult(GestureEnums::BODYSIDE_INVALID), projectile(NULL)
{
	creationGesture->UpdateGUI = true;
	creationGesture->ShowImage();
}

Attack::Attack(float attackCooldown /*= 0.0f*/, ProjectileManager* projManager /*= NULL*/, 
		ProjectileIdentifier projID /*= ProjectileIdentifier()*/, AttackParams params /*= AttackParams()*/)
		:creationGesture(params.CreationGesture), launchGesture(params.LaunchGesture), currentState(AS_NONE), 
		projectileController(params.ProjController),
		AttackCooldown(attackCooldown), cooldownTimePassed(0.0f), projectileIdentifier(projID),
		projectileManager(projManager), spawnPositionCalculator(params.PositionCalculator), spawnPosition(0.0f),
		LaunchOnCreate(params.LaunchOnCreate), spawnPositionValid(false), bodySideResult(GestureEnums::BODYSIDE_INVALID),
		projectile(NULL)
{
	creationGesture->UpdateGUI = true;
	creationGesture->ShowImage();
}

Attack::~Attack(void)
{
	if(creationGesture)
		delete creationGesture;

	if(projectileController)
		delete projectileController;

	if(launchGesture)
		delete launchGesture;
}

void Attack::Update(float gameTime)
{
	switch (currentState)
	{
		//If no state, update the creation gesture
	case Attack::AS_NONE:
		creationGesture->Update(gameTime);
		break;
		//if in the created state update to the controlled state
	case Attack::AS_CREATED:
		if(spawnPositionValid)
			currentState = AS_CONTROLLED;
		break;
		//If in the control state, check if the projectile should be launched
	case Attack::AS_CONTROLLED:
		if(launchGesture)
			launchGesture->Update(gameTime);
		break;
		//If in the launched state, do nothing
	case Attack::AS_LAUNCHED:
		cooldownTimePassed += gameTime;

		if(cooldownTimePassed >= AttackCooldown)
			Reset();

		break;
	default:
		break;
	}
}

Attack::AttackState Attack::Evaluate(const AttackData& bodyData)
{
	AttackState prevState = currentState;

	switch (currentState)
	{
	case Attack::AS_NONE:
		{
			GestureEnums::BodySide result = creationGesture->Evaluate(bodyData);
			if(result != GestureEnums::BODYSIDE_INVALID)
			{
				currentState = AS_CREATED;

				bodySideResult = result;

				if(bodyData.CurrentData)
				{
					spawnPosition = spawnPositionCalculator.CalculateSpawnPoint(result, bodyData);
					spawnPositionValid = true;
				}
				else
					spawnPosition = Ogre::Vector3(0.0f);

				//Let the controller update which side it should process based on the results
				if(projectileController)
					projectileController->ReceivePreviousResults(result);

				if(launchGesture)
				{
					creationGesture->UpdateGUI = false;
					launchGesture->UpdateGUI = true;
					launchGesture->ShowImage();
					launchGesture->TransitionFromGesture(result);
				}
			}
		}
		
		break;
	case Attack::AS_CREATED:
		if(bodyData.CurrentData)
		{
			spawnPosition = spawnPositionCalculator.CalculateSpawnPoint(bodySideResult, bodyData);
			spawnPositionValid = true;
		}
		break;
	case Attack::AS_CONTROLLED:
		if(projectileController)
		{
			if(bodyData.CurrentData && bodyData.PreviousData)
				projectileController->ControlProjectile(bodyData._Probender,
					*bodyData.CurrentData, *bodyData.PreviousData);
		}

		if(launchGesture)
		{
			GestureEnums::BodySide result = launchGesture->Evaluate(bodyData);
			if(result != GestureEnums::BODYSIDE_INVALID)
			{
				currentState = AS_LAUNCHED;
			}
		}
		else
			currentState = AS_LAUNCHED;

		break;
	case Attack::AS_LAUNCHED:

		break;
	default:
		break;
	}

	//if there has been a state change, indicate with return value, otherwise return no state
	if(prevState != currentState)
		return currentState;
	else
		return AS_NONE;
}

void Attack::Reset()
{
	cooldownTimePassed = 0.0f;

	creationGesture->Reset();
	creationGesture->UpdateGUI = true;
	
	if(launchGesture)
	{
		launchGesture->Reset();
		launchGesture->UpdateGUI = false;
	}

	creationGesture->ShowImage();
	
	if(projectileController)
		projectileController->projectile = NULL;

	spawnPositionValid = false;

	//Destroy the projectile instance
	currentState = AS_NONE;
}

Ogre::Vector3 SpawnPositionCalculator::GetOgrePositionRelative(const CameraSpacePoint& limbPosition, const CameraSpacePoint& referencePoint)
{
	{
		CameraSpacePoint result = CameraSpacePoint();

		result.X = (-limbPosition.X - -referencePoint.X);
		result.Y = (limbPosition.Y - referencePoint.Y);
		result.Z = (limbPosition.Z - referencePoint.Z);
		
		float mag = Ogre::Math::Sqrt(result.X * result.X + result.Y * result.Y + result.Z * result.Z);

		float div = 1.0f / mag;
		
		return Ogre::Vector3(result.X * div * PROBENDER_HALF_EXTENTS.x, 
			result.Y * div * PROBENDER_HALF_EXTENTS.y, result.Z * div * PROBENDER_HALF_EXTENTS.z);
	}
}
