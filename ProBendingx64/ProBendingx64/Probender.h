#pragma once
#include "ProbenderInputHandler.h"
#include "ProbenderInGameData.h"
#include "ProbenderStateManager.h"
#include "ProbenderData.h"
#include "Projectile.h"

#include <memory>

namespace physx
{
	class PxRigidDynamic;
};

class IScene;
class Arena;

typedef std::shared_ptr<Projectile> SharedProjectile;

class Probender : public GameObject
{
	friend class ProbenderInputHandler;

private:
	unsigned short contestantID;//The ID the contestant is stored in the arena with

	ElementEnum::Element currentElement;//The current element used

	ProbenderInGameData characterData;//The characters game stats

	ProbenderInputHandler inputHandler;//The component handling input

	///Flight Game Object here

	SharedProjectile leftHandAttack;//The projectile in the left hand
	SharedProjectile rightHandAttack;//The projectile in the right hand

	ProbenderStateManager stateManager;//The state manager for probenders

	Probender* currentTarget;//The probender currently targeted by this player

	Arena* owningArena;//The arena the contestant is part of

public:
	enum InputState{Listen, Pause, Stop};

	Probender();
	Probender(const unsigned short _contestantID, Arena* _owningArena);
	~Probender(void);

	///<summary>At the moment this is used to differentiate between standard Game Objects and Projectiles and Probenders</summary>
	///<returns>True if serializable, false if not</returns>
	virtual inline bool IsSerializable()const{return false;}

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

	///<summary>Gets the arena that the probender is currently participating in</summary>
	///<returns>Pointer to the arena </returns>
	inline Arena*const GetOwningArena()const{return owningArena;}
	
	///<summary>Gets a read-only copy of the current element equipped</summary>
	///<returns>Read-only current element equipped</returns>
	inline const ElementEnum::Element GetCurrentElement()const{return currentElement;}

	///<summary>Sets the element that the probender has currently equipped</summary>
	///<param name="elementToSet">The element to set to</param>
	virtual void SetCurrentElement(const ElementEnum::Element elementToSet);

	static void CreateContestantMeshes(Ogre::SceneManager* sceneMan, bool red, bool blue);
};

