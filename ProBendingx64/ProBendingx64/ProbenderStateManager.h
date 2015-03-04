#pragma once
#include "State.h"

#include <array>
class Probender;

class ProbenderStateManager
{
private:
	Probender* probender;

	bool onGround;

	StateFlags::PossibleStates currentState;//The current state held by the state manager

	//8 States allowed in total. Don't like having the 8 there but no choice
	std::array<State, StateFlags::PossibleStates::COUNT> States;

	float timeInCurrentState;//The amount of time that has been spent in the current state
	float timeForcedInState;//Amount of time that must pass before another switch is allowed

	float expirationTime;
	float stateChangeBlockTimer;//The amount that time in current state must exceed before a new state is allowed

	bool stateChangedThisFrame;
	
	///<summary>Gets the state as represented by the specified flag</summary>
	///<param name="stateToGet">The state to retrieve</param>
	///<returns>The state, or NULL if invalid value was passed</returns>
	inline State* GetState(StateFlags::PossibleStates stateToGet)
	{
		if(stateToGet == StateFlags::COUNT)
			return NULL;

		return &States[stateToGet - 1];
	}

public:
	bool disallowStateChange;

	ProbenderStateManager(void);

	ProbenderStateManager(Probender* bender);

	~ProbenderStateManager(void);

	///<summary>Attempts to set the new state, depending on whether or not the old state is
	///interruptible or if the state has been completed///</summary>
	///<param name="timeInNewState">The time forced in this state before being allowed to change</param>
	///<param name="newState">The new state to try to set to</param>
	///<returns>True if successful, false if disallowStateChange is true, if state has already been changed this frame,
	///if newState is not an allowable transition, newState is currentState or if not enough time has been spent in current state. If state must
	///be overridden in place of these conditions, call SetStateImmediate<returns>
	bool SetState(StateFlags::PossibleStates newState, float timeInNewState);

	///<summary>Sets the current state to the specified state regardless of any conditional statements
	///will not set new state if disallowStateChange is active. Will interrupt time in new state however</summary>
	///<param name="newState">The new state to immediately set to</param>
	///<param name="timeInNewState">The amount of time forced to be in this state</param>
	///<returns>True if successful, false if disallowStateChange is true or newState is current state<returns>
	bool SetStateImmediate(StateFlags::PossibleStates newState, float timeInNewState);

	inline void ResetCurrentState()
	{
		timeInCurrentState = 0.0f;
	}

	inline bool GetOnGround()const{return onGround;}

	///<summary>Sets whether or not the player is now on the ground. Sets state accordingly</summary>
	///<param name="val">True if on ground, false if not</param>
	void SetOnGround(bool val);

	///<summary>Get the current state held by the state manager</summary>
	///<returns>The current state represented by this manager</returns>
	inline StateFlags::PossibleStates GetCurrentState()const{return currentState;}

	///<summary>Updates the state manager timers</summary>
	void Update(float gameTime);
};

