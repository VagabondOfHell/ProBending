#pragma once
#include "AttackGesture.h"
#include "ProjectileController.h"

struct ProjectileIdentifier
{
	ElementEnum::Element Element;
	AbilityIDs::AbilityID AbilityID;

	ProjectileIdentifier()
		:Element(ElementEnum::InvalidElement), AbilityID(0)
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

	AttackState currentState;

	ProjectileIdentifier projectileIdentifier;

	float cooldownTimePassed;

	///<summary>Gets an instance of the projectile from the Projectile Pool and places it under the projectile Controller</summary>
	void GetProjectileInstance();

public:
	
	float AttackCooldown;

	Attack( float attackCooldown = 0.0f, ProjectileIdentifier projID = ProjectileIdentifier(),
		AttackGesture* creationGesture = NULL, ProjectileController* controller = NULL, AttackGesture* launchGesture = NULL);

	~Attack(void);

	///NOT FINISHED///
	void Update(float gameTime);

	///NOT FINISHED///
	///<summary>Evaluates the body data and checks if an action should be performed</summary>
	///<param name="bodyData">The body data to evaluate</param>
	///<returns>The new state if there was a state change, or AS_NONE if no state change</returns>
	AttackState Evaluate(const AttackData& bodyData);

	///NOT FINISHED
	void Reset();
};

