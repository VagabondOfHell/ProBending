#pragma once
#include "AttackGesture.h"
#include "ProjectileController.h"
#include "Projectile.h"

class ProjectileManager;

struct AttackParams
{
	AttackGesture* CreationGesture;
	ProjectileController* ProjController;
	AttackGesture* LaunchGesture;

	AttackParams():CreationGesture(NULL), ProjController(NULL), LaunchGesture(NULL)
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

	ProjectileManager* projectileManager;

	float cooldownTimePassed;
	
public:
	
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
		projectileController->projectile = proj;
		if(calcOrigin && proj)
			projectileController->ProjectileOrigin = HelperFunctions::OgreToPhysXVec3(proj->GetWorldPosition());
	}

	inline Projectile* GetProjectile()const{return projectileController->projectile;}

	///NOT FINISHED
	void Reset();
};

