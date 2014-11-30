#include "ProbenderStateManager.h"
#include "NotImplementedException.h"

ProbenderStateManager::ProbenderStateManager(void)
{
	isFlying = false;
	currentState = Idle;
	timeInCurrentState = 0.0f;
	timeSpentFlying = 0.0f;
}


ProbenderStateManager::~ProbenderStateManager(void)
{
}

void ProbenderStateManager::SetState(PossibleStates newState)
{
	if(currentState == newState)
		return;

	//Reset the timer upon new state assignment
	timeInCurrentState = 0.0f;

	//Throw this to complete this method later on
	throw NotImplementedException();
}

void ProbenderStateManager::SetFlyState(bool _isFlying)
{
	if(isFlying == _isFlying)
		return;

	///Reset the flying counter when we begin flying
	if(isFlying)
		timeSpentFlying = 0.0f;
	
	isFlying = _isFlying;

	//Throw this to make sure I double check requirements of this method
	throw NotImplementedException();
}

void ProbenderStateManager::Update(float gameTime)
{
	if(isFlying)
		timeSpentFlying += gameTime;

	//Increment the amount of time we have spent in the current state
	timeInCurrentState += gameTime;
}