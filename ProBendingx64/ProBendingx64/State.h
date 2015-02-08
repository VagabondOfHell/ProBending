#pragma once
#include "StateFlags.h"

class State
{
private:
	StateFlags::PossibleStates thisState;//What this state is

	StateFlags::StateFlagCombo allowableTransitions;//What this state can transition to
	StateFlags::StateFlagCombo interruptingTransitions;//What this state can be interrupted by

	float timeSelectable;//Amount of time that must pass until the state can be selected again
	float timePassed; //Time passed since state was last selected

public:
	State(void)
		:thisState(StateFlags::PossibleStates::COUNT), allowableTransitions(StateFlags::PossibleStateFlags::INVALID_STATE_FLAG), 
		timeSelectable(0.0f), timePassed(0.0f), interruptingTransitions(StateFlags::PossibleStateFlags::INVALID_STATE_FLAG)
	{

	}

	State(StateFlags::PossibleStates _thisState, StateFlags::StateFlagCombo _allowableTransitions, float cooldownTimer = 0.0f,
		StateFlags::StateFlagCombo _interruptingTransitions = StateFlags::PossibleStateFlags::INVALID_STATE_FLAG)
		:thisState(_thisState), allowableTransitions(_allowableTransitions), 
		timeSelectable(cooldownTimer), timePassed(0.0f), interruptingTransitions(_interruptingTransitions)
	{

	}

	~State(void){}

	inline StateFlags::PossibleStates GetStateID()const {return thisState;}

	///<summary>Checks if the specified value is a permitted transition</summary>
	///<param name="newState">The new state to attempt to transition to</param>
	///<returns>True if the new state is a transitionable state and the state cooldown timer has expired,
	///or bypass the timer if new state is an interruptible state. False if none of the above</returns>
	inline bool ValidTransition(StateFlags::PossibleStates newState)
	{
		StateFlags::PossibleStateFlags flagToCompare = StateFlags::ConvertToFlag(newState);

		//Return true if allowable transitions and time passed is greater then current cooldown
		//or if it is an interruptible transition
		return (((allowableTransitions & flagToCompare) && timePassed >= timeSelectable) || 
			interruptingTransitions & flagToCompare);	
	}

	inline void Update(float gameTime)
	{
		if(timePassed < timeSelectable)
			timePassed += gameTime;
	}

	///<summary>Performs any necessary clean ups whenever the state is exitted from</summary>
	inline void ExitState()
	{
		timePassed = 0.0f;
	}
};

