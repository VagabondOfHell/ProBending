#pragma once
#include "ProbenderInputHandler.h"
#include "ProbenderStateManager.h"
#include "ProbenderData.h"
#include "Projectile.h"

#include "GUIProgressTracker.h"
#include "ProbenderEnergyMeter.h"

#include "OgreCamera.h"

#include <memory>

namespace physx
{
	class PxRigidDynamic;
};

namespace Ogre
{
	class Camera;
};

class MeshRenderComponent;
class IScene;
class Arena;

typedef std::shared_ptr<Projectile> SharedProjectile;

struct TransitionData
{
	physx::PxVec3 StartPos, EndPos;
	float Percentile;
};

class Probender : public GameObject
{
	friend class ProbenderInputHandler;

private:
	unsigned short contestantID;//The ID the contestant is stored in the arena with
	
	static const float DODGE_DISTANCE;
	static const float FALL_FORCE;

	Ogre::Camera* camera;

	ProbenderData characterData;//The characters game stats
	
	ProbenderInputHandler inputHandler;//The component handling input

	SharedProjectile leftHandAttack;//The projectile in the left hand
	SharedProjectile rightHandAttack;//The projectile in the right hand
	
	Arena* owningArena;//The arena the contestant is part of

	MeshRenderComponent* meshRenderComponent;

	TransitionData dodgeInfo;//The data for lerping dodge movements
	TransitionData transitionInfo;//Info used during the transition state

	std::string GetMeshAndMaterialName();

	GUIProgressTracker progressTracker;
	ProbenderEnergyMeter energyMeter;

public:
	Probender* currentTarget;//The probender currently targeted by this player

	ProbenderStateManager stateManager;//The state manager for probenders
	
	enum InputState{Listen, Pause, Stop};
	enum DodgeDirection{DD_INVALID, DD_RIGHT, DD_LEFT};

	Probender();
	Probender(const unsigned short _contestantID, const ProbenderData charData, Arena* _owningArena);
	~Probender(void);

	///<summary>At the moment this is used to differentiate between standard Game Objects and Projectiles and Probenders</summary>
	///<returns>True if serializable, false if not</returns>
	virtual inline bool IsSerializable()const{return false;}

	inline unsigned short GetContestantID()const {return contestantID;}

	inline ArenaData::Team GetTeam()const{return characterData.TeamDatas.Team;}

	inline TeamData::ContestantColour GetColour()const {return characterData.TeamDatas.PlayerColour;}

	inline ArenaData::Zones GetCurrentZone()const{return characterData.TeamDatas.CurrentZone;}
	
	inline Ogre::Camera* GetCamera()const{return camera;}

	void SetCamera(Ogre::Camera* newCamera);

	void TransitionToPoint(physx::PxVec3& positionToMoveTo);

	void Start();

	void Update(float gameTime);
	
	///<summary>Gets a read-only copy of the Probender Data</summary>
	///<returns>A read-only version of Probender In-Game Data</returns>
	const ProbenderData& GetInGameData(){return characterData;}

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
	inline const ElementEnum::Element GetCurrentElement()const{return characterData.CurrentElement;}

	///<summary>Sets the element that the probender has currently equipped</summary>
	///<param name="elementToSet">The element to set to</param>
	inline void SetCurrentElement(const ElementEnum::Element elementToSet)
	{
		characterData.CurrentElement = elementToSet;
	}

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
	
	void Dodge(DodgeDirection direction);

	void ApplyProjectileCollision(float damage, float knockback);

	virtual void OnCollisionEnter(const CollisionReport& collision);

	virtual void OnCollisionLeave(const CollisionReport& collision);

	virtual void OnTriggerEnter(GameObject* trigger, GameObject* other);

	virtual void OnTriggerLeave(GameObject* trigger, GameObject* other);

	virtual void OnCollisionStay(const CollisionReport& collision);

};

