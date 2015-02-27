#include "ProbenderStateManager.h"
#include "Probender.h"

ProbenderStateManager::ProbenderStateManager(void)
	:probender(NULL)
{
	timeInCurrentState = 0.0f;
	stateChangeBlockTimer = 0.0f;

	stateChangedThisFrame = false;
	disallowStateChange = false;

	currentState = StateFlags::IDLE_STATE;
}

ProbenderStateManager::ProbenderStateManager(Probender* bender)
	:probender(bender)
{
	timeInCurrentState = 0.0f;
	stateChangeBlockTimer = 0.0f;

	stateChangedThisFrame = false;
	disallowStateChange = false;

	States[StateFlags::IDLE_STATE - 1] = State(StateFlags::IDLE_STATE, 
		StateFlags::ALL_STATES & ~StateFlags::IDLE_STATE_FLAG, 0.0f, StateFlags::ALL_STATES & ~StateFlags::IDLE_STATE_FLAG);

	States[StateFlags::JUMP_STATE - 1] = State(StateFlags::JUMP_STATE, 
		StateFlags::FALLING_STATE_FLAG, 0.0f, StateFlags::REELING_STATE_FLAG | StateFlags::FALLING_STATE_FLAG);

	States[StateFlags::FALLING_STATE - 1] = State(StateFlags::FALLING_STATE,
		StateFlags::IDLE_STATE_FLAG, 0.0f, StateFlags::IDLE_STATE_FLAG | StateFlags::REELING_STATE_FLAG);

	States[StateFlags::BLOCK_STATE - 1] = State(StateFlags::BLOCK_STATE, StateFlags::IDLE_STATE_FLAG, 1.0f, StateFlags::REELING_STATE_FLAG);

	States[StateFlags::CATCH_STATE - 1] = State(StateFlags::CATCH_STATE, StateFlags::IDLE_STATE_FLAG, 1.0f, StateFlags::REELING_STATE_FLAG);

	States[StateFlags::HEAL_STATE - 1] = State(StateFlags::HEAL_STATE, StateFlags::IDLE_STATE_FLAG, 1.0f, StateFlags::REELING_STATE_FLAG);

	States[StateFlags::DODGE_STATE - 1] = State(StateFlags::DODGE_STATE, StateFlags::IDLE_STATE_FLAG, 0.0f, StateFlags::INVALID_STATE_FLAG);

	States[StateFlags::REELING_STATE - 1] = State(StateFlags::REELING_STATE, StateFlags::IDLE_STATE_FLAG, 1.0f, StateFlags::INVALID_STATE_FLAG);

	States[StateFlags::ATTACKING_STATE - 1] = State(StateFlags::PossibleStates::ATTACKING_STATE,
		StateFlags::IDLE_STATE_FLAG, 0.0f, StateFlags::REELING_STATE_FLAG);

	currentState = StateFlags::IDLE_STATE;
}

ProbenderStateManager::~ProbenderStateManager(void)
{
}

bool ProbenderStateManager::SetState(StateFlags::PossibleStates newState, float timeInNewState)
{
	if(newState == StateFlags::COUNT || newState == StateFlags::INVALID_STATE)
		return false;

	if(disallowStateChange)
		return false;

	if(stateChangedThisFrame)
		return false;

	if(States[currentState - 1].GetStateID() == newState)
		return false;

	if(States[currentState - 1].ValidTransition(newState))
	{
		States[currentState - 1].ExitState();

		probender->StateExitted(currentState);

		//Set the new state then call Reset on it
		currentState = newState;
		stateChangedThisFrame = true;

		probender->StateEntered(currentState);

		return true;
	}
	
	return false;
}

bool ProbenderStateManager::SetStateImmediate(StateFlags::PossibleStates newState, float timeInNewState)
{
	if(disallowStateChange)
		return false;
	
	if(States[currentState].GetStateID() == newState)
		return false;

	if(newState == StateFlags::COUNT)
		return false;

	States[currentState].ExitState();

	probender->StateExitted(currentState);

	//Set the new state then call Reset on it
	currentState = newState;
	stateChangedThisFrame = true;

	probender->StateEntered(currentState);

	return true;
}

void ProbenderStateManager::Update(float gameTime)
{
	//Increment the amount of time we have spent in the current state
	timeInCurrentState += gameTime;

	for (int i = 0; i < States.size(); i++)
	{
		States[i].Update(gameTime);
	}

	stateChangedThisFrame = false;
}

void ProbenderStateManager::SetOnGround(bool val)
{
	//if no change, ignore
	if(onGround == val)
		return;

	if(val)//if val is true and onGround is false
	{
		if(currentState == StateFlags::FALLING_STATE)
			SetStateImmediate(StateFlags::IDLE_STATE, 0.0f);
	}
	else
	{
		//if val is false, indicate we are falling regardless of current state if we are not jumping
		if(currentState != StateFlags::JUMP_STATE)
			SetStateImmediate(StateFlags::FALLING_STATE, 0.0f);
	}

	onGround = val;
}
