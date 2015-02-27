#include "AttackGesture.h"


AttackGesture::AttackGesture(void)
	:currentIndex(0), timePassed(0)
{
}

AttackGesture::~AttackGesture(void)
{

}

void AttackGesture::Update(float gameTime)
{
	if(gestureEvaluators.size() == 0)
		return;

	timePassed += gameTime;

	//if time has exceeded how long the current gesture is allowed to be evaluated for, reset the progress tracker
	//Check if timeToComplete is a positive number, otherwise we consider it unlimited time to perform the gesture
	if(gestureEvaluators[currentIndex].TimeToComplete > 0.0f && timePassed >= gestureEvaluators[currentIndex].TimeToComplete)
		Reset();
}

GestureEnums::BodySide AttackGesture::Evaluate(const AttackData& attackData)
{
	//if no gestures, return false
	if(gestureEvaluators.size() == 0)
		return GestureEnums::BODYSIDE_INVALID;

	GestureEnums::BodySide result = gestureEvaluators[currentIndex].Evaluate(attackData);

	//if the current gesture evaluates to true
	if(result != GestureEnums::BODYSIDE_INVALID)
	{
		//increment the index and cap between 0 and max
		unsigned int newIndex =	(currentIndex + 1) % gestureEvaluators.size();

		if(newIndex != 0)
			SetNextTransitionData(result, currentIndex, newIndex);

		currentIndex = newIndex;

		//if we have reset the index, indicate success
		if(currentIndex == 0)
			return result;
	}

	return GestureEnums::BODYSIDE_INVALID;
}

void AttackGesture::SetNextTransitionData(GestureEnums::BodySide result, unsigned int currentIndex, unsigned int newIndex)
{
	GestureEnums::TransitionRules tranRule = gestureEvaluators[newIndex].TransitionFromLast;

	switch (tranRule)
	{
	case GestureEnums::TRANRULE_SAME:
		gestureEvaluators[newIndex].extraCustomData.Side = result;
		break;
	case GestureEnums::TRANRULE_OPPOSITE:
		if(result == GestureEnums::BODYSIDE_LEFT)
			gestureEvaluators[newIndex].extraCustomData.Side = GestureEnums::BODYSIDE_RIGHT;
		else if(result == GestureEnums::BODYSIDE_RIGHT)
			gestureEvaluators[newIndex].extraCustomData.Side = GestureEnums::BODYSIDE_LEFT;
		break;
	default:
		return;
		break;
	}
}

void AttackGesture::TransitionFromGesture(GestureEnums::BodySide result)
{
	if(gestureEvaluators.size() == 0)
		return;

	//Get how to transition from the last gesture
	GestureEnums::TransitionRules tranRule = gestureEvaluators[0].TransitionFromLast;

	switch (tranRule)
	{
	case GestureEnums::TRANRULE_SAME:
		gestureEvaluators[0].extraCustomData.Side = result;
		break;
	case GestureEnums::TRANRULE_OPPOSITE:
		if(result == GestureEnums::BODYSIDE_LEFT)
			gestureEvaluators[0].extraCustomData.Side = GestureEnums::BODYSIDE_RIGHT;
		else if(result == GestureEnums::BODYSIDE_RIGHT)
			gestureEvaluators[0].extraCustomData.Side = GestureEnums::BODYSIDE_LEFT;
		break;
	default:
		return;
		break;
	}
}
