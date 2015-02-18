#pragma once
#include "ProbenderInputHandler.h"
#include "ProbenderInGameData.h"
#include "ProbenderStateManager.h"
#include "ProbenderData.h"
#include "Projectile.h"
#include "GUIProgressTracker.h"

#include <memory>

namespace physx
{
	class PxRigidDynamic;
};

class MeshRenderComponent;
class IScene;
class Arena;

typedef std::shared_ptr<Projectile> SharedProjectile;

class Probender : public GameObject
{
	friend class ProbenderInputHandler;

private:
	unsigned short contestantID;//The ID the contestant is stored in the arena with
	
	static const physx::PxVec3 HALF_EXTENTS;
	static const float DODGE_DISTANCE;

	TeamData::ContestantColour playerColour;
	ArenaData::Team currentTeam;

	ElementEnum::Element currentElement;//The current element used

	ProbenderInGameData characterData;//The characters game stats

	ProbenderInputHandler inputHandler;//The component handling input

	SharedProjectile leftHandAttack;//The projectile in the left hand
	SharedProjectile rightHandAttack;//The projectile in the right hand
	
	Probender* currentTarget;//The probender currently targeted by this player

	Arena* owningArena;//The arena the contestant is part of

	MeshRenderComponent* meshRenderComponent;

	Ogre::Vector3 jumpOrigin;

	physx::PxVec3 dodgeTargetPos;//The final position
	physx::PxVec3 dodgeDirection;//direction to move towards

	std::string GetMeshAndMaterialName();

	GUIProgressTracker progressTracker;

	void HandleJump();

	void HandleDodge(const float gameTime);

	void HandleFall();

public:
	ArenaData::Zones CurrentZone;
	ProbenderStateManager stateManager;//The state manager for probenders
	
	enum InputState{Listen, Pause, Stop};
	enum DodgeDirection{DD_INVALID, DD_RIGHT, DD_LEFT};

	Probender();
	Probender(const unsigned short _contestantID, Arena* _owningArena);
	~Probender(void);

	///<summary>At the moment this is used to differentiate between standard Game Objects and Projectiles and Probenders</summary>
	///<returns>True if serializable, false if not</returns>
	virtual inline bool IsSerializable()const{return false;}

	inline ArenaData::Team GetTeam()const{return currentTeam;}

	inline TeamData::ContestantColour GetColour()const {return playerColour;}

	inline ArenaData::Zones GetCurrentZone()const{return CurrentZone;}
	
	///<summary>Takes the menu created data of the probender and converts it to usable in-game data</summary>
	///<param name="data">The menu data to convert</param>
	void CreateInGameData(const ProbenderData& data);

	void Start();

	void Update(float gameTime);
	
	///<summary>Gets a read-only copy of the Probender Data</summary>
	///<returns>A read-only version of Probender In-Game Data</returns>
	const ProbenderInGameData GetInGameData(){return characterData;}

	///<summary>Has the probender acquire a new target</summary>
	///<param name="toRight">True to look to the right for the target, false to look to the left</param>
	void AcquireNewTarget(bool toRight);

	///<summary>Checks if the passed projectile is currently on one of the hands, and if so, removes it</summary>
	///<param name="projectileToRemove">The projectile to remove</param>
	void RemoveProjectile(SharedProjectile projectileToRemove);

	///<summary>Sets the input state of the probender</summary>
	///<param name="newState">The new state to set to</param>
	void SetInputState(const InputState newState);

	void SetKeyboardConfiguration(const ConfigurationLayout& newKeyLayout){inputHandler.keysLayout = newKeyLayout;}

	///<summary>Gets the arena that the probender is currently participating in</summary>
	///<returns>Pointer to the arena </returns>
	inline Arena*const GetOwningArena()const{return owningArena;}
	
	///<summary>Gets a read-only copy of the current element equipped</summary>
	///<returns>Read-only current element equipped</returns>
	inline const ElementEnum::Element GetCurrentElement()const{return currentElement;}

	///<summary>Sets the element that the probender has currently equipped</summary>
	///<param name="elementToSet">The element to set to</param>
	virtual void SetCurrentElement(const ElementEnum::Element elementToSet);

	///<summary>Creates the mesh for the specified colours</summary>
	///<param name="sceneMan">The Ogre Scene Manager used to create the Manual Object with</param>
	///<param name="red">True to create Red contestant, false if not</param>
	///<param name="blue">True to create Blue contestant, false if not</param>
	///<param name="green">True to create Green contestant, false if not</param>
	///<param name="yellow">True to create Yellow contestant, false if not</param>
	///<param name="purple">True to create Purple contestant, false if not</param>
	///<param name="orange">True to create Orange contestant, false if not</param>
	static void CreateContestantMeshes(Ogre::SceneManager* sceneMan, bool red, bool blue, 
		bool green, bool yellow, bool purple, bool orange);

	void StateExitted(StateFlags::PossibleStates exittedState);

	void StateEntered(StateFlags::PossibleStates enteredState);

	inline void Jump(){stateManager.SetState(StateFlags::JUMP_STATE, 0.0f);}
	
	inline void Dodge(DodgeDirection direction)
	{
		if(stateManager.SetState(StateFlags::DODGE_STATE, 0.0f))
		{
			float dirAndDist = DODGE_DISTANCE;
						
			if(direction == DD_LEFT)
				dirAndDist = -dirAndDist;

			dodgeTargetPos = HelperFunctions::OgreToPhysXVec3(GetWorldPosition() + (dirAndDist * Right()));
			dodgeDirection = dodgeTargetPos - HelperFunctions::OgreToPhysXVec3(GetWorldPosition());
		}
	}

	virtual void OnCollisionEnter(const CollisionReport& collision);

	virtual void OnCollisionLeave(const CollisionReport& collision);

	virtual void OnTriggerEnter(GameObject* trigger, GameObject* other);

	virtual void OnTriggerLeave(GameObject* trigger, GameObject* other);

};

