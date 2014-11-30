#pragma once

class ProbenderStateManager
{
public:
	//Countering isn't a state because it utilizes the catching motion
	//Reeling is used when taking damage. It interrupts all other states
	enum PossibleStates{Idle, Jumping, Blocking, Catching, Healing, Dodging, Moving, Reeling};

private:
	bool isFlying; //True if airborne, false if not

	PossibleStates currentState;//The current state held by the state manager

	float timeInCurrentState;//The amount of time that has been spent in the current state
	float timeSpentFlying;//The amount of time spent flying

public:

	ProbenderStateManager(void);
	~ProbenderStateManager(void);

	///<summary>Attempts to set the new state, depending on whether or not the old state is
	///interruptible or if the state has been completed///</summary>
	///<param name="newState">The new state to try to set to</param>
	void SetState(PossibleStates newState);

	///<summary>Sets the flying state to the specified value </summary>
	///<param name="isFlying">True if flying, false if not</param>
	void SetFlyState(bool _isFlying);

	///<summary>Get the current state held by the state manager</summary>
	///<returns>The current state represented by this manager</returns>
	inline PossibleStates GetCurrentState()const{return currentState;}

	///<summary>Get the current flying state held by the state manager</summary>
	///<returns> True if in a flying state, false if not</returns>
	bool GetFlyState()const {return isFlying;}

	///<summary>Updates the state manager timers</summary>
	void Update(float gameTime);
};

