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
		projectileManager(projManager), LaunchOnCreate(false)
{
}

Attack::Attack(float attackCooldown /*= 0.0f*/, ProjectileManager* projManager /*= NULL*/, 
		ProjectileIdentifier projID /*= ProjectileIdentifier()*/, AttackParams params /*= AttackParams()*/)
		:creationGesture(params.CreationGesture), launchGesture(params.LaunchGesture), currentState(AS_NONE), 
		projectileController(params.ProjController),
		AttackCooldown(attackCooldown), cooldownTimePassed(0.0f), projectileIdentifier(projID),
		projectileManager(projManager), spawnPositionCalculator(params.PositionCalculator), spawnPosition(0.0f),
		LaunchOnCreate(params.LaunchOnCreate)
{
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

				if(bodyData.CurrentData)
					spawnPosition = spawnPositionCalculator.CalculateSpawnPoint(result, bodyData);
				else
					spawnPosition = Ogre::Vector3(0.0f);

				//Let the controller update which side it should process based on the results
				if(projectileController)
					projectileController->ReceivePreviousResults(result);

				if(launchGesture)
					launchGesture->TransitionFromGesture(result);
			}
		}
		
		break;
	case Attack::AS_CREATED:

		break;
	case Attack::AS_CONTROLLED:
		if(projectileController)
		{
			if(bodyData.CurrentData && bodyData.PreviousData)
				projectileController->ControlProjectile(bodyData._Probender, *bodyData._BodyDimensions, 
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

	if(launchGesture)
		launchGesture->Reset();

	if(projectileController)
		projectileController->projectile = NULL;

	//Destroy the projectile instance
	currentState = AS_NONE;
}
