#pragma once
#include "AttackGesture.h"

///This file is used to store all the custom static methods that will be used to check if a
///GestureEvaluator has been completed or not

namespace AttackGestureEvaluators
{
	GestureEnums::BodySide KneeRaiseGesture(const Probender* probender, const BodyDimensions& bodyDimensions, 
		const CompleteData& currData, const CompleteData& prevData, const ExtraCustomData& customData);

	GestureEnums::BodySide KneeDownGesture(const Probender* probender, const BodyDimensions& bodyDimensions,
		const CompleteData& currData, const CompleteData& prevData, const ExtraCustomData& customData);

	GestureEnums::BodySide ArmPunchGesture(const Probender* probender, const BodyDimensions& bodyDimensions, 
		const CompleteData& currData, const CompleteData& prevData, const ExtraCustomData& customData);
};

