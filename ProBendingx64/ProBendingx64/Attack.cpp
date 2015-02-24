#include "Attack.h"

Attack::Attack(float attackCooldown /*= 0.0f*/, ProjectileIdentifier projID /*= ProjectileIdentifier()*/, AttackGesture* _creationGesture /*= NULL*/,
			   ProjectileController* controller /*= NULL*/, AttackGesture* _launchGesture /*= NULL*/)
	:creationGesture(_creationGesture), launchGesture(_launchGesture), currentState(AS_NONE), projectileController(controller),
		AttackCooldown(attackCooldown), cooldownTimePassed(0.0f), projectileIdentifier(projID)
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

void Attack::GetProjectileInstance()
{

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
		launchGesture->Update(gameTime);
		break;
		//If in the launched state, do nothing
	case Attack::AS_LAUNCHED:
		cooldownTimePassed += gameTime;

		if(cooldownTimePassed >= AttackCooldown)
			currentState = AS_NONE;

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
				//Create projectile
				currentState = AS_CREATED;

				//Let the controller update which side it should process based on the results
				if(projectileController)
					projectileController->ReceivePreviousResults(result);
			}
		}
		
		break;
	case Attack::AS_CREATED:

		break;
	case Attack::AS_CONTROLLED:
		if(projectileController)
		{
			if(bodyData.CurrentData && bodyData.PreviousData)
				projectileController->ControlProjectile(bodyData._Probender, bodyData._BodyDimensions, 
					*bodyData.CurrentData, *bodyData.PreviousData);
		}

		if(launchGesture)
		{
			if(launchGesture->Evaluate(bodyData))
			{
				//Launch the Projectile

				currentState = AS_LAUNCHED;
			}
		}
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

	//Destroy the projectile instance

	currentState = AS_NONE;
}
