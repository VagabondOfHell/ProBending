#pragma once
#include "AttackGesture.h"

///This file is used to store all the custom static methods that will be used to check if a
///GestureEvaluator has been completed or not

namespace AttackGestureEvaluators
{
	GestureEnums::BodySide KneeRaiseGesture(const Probender* probender,	const CompleteData& currData, 
		const CompleteData& prevData, const ExtraCustomData& customData);

	GestureEnums::BodySide KneeDownGesture(const Probender* probender, const CompleteData& currData, 
		const CompleteData& prevData, const ExtraCustomData& customData);

	GestureEnums::BodySide ArmPunchGesture(const Probender* probender, const CompleteData& currData, 
		const CompleteData& prevData, const ExtraCustomData& customData);

	///For when the arms are extended out from the body towards the left and right sides
	GestureEnums::BodySide ArmsWideGesture(const Probender* probender, const CompleteData& currData, 
		const CompleteData& prevData, const ExtraCustomData& customData);

	GestureEnums::BodySide HandsClapGesture(const Probender* probender, const CompleteData& currData, 
		const CompleteData& prevData, const ExtraCustomData& customData);

	GestureEnums::BodySide HandsToSide(const Probender* probender, const CompleteData& currData,
		const CompleteData& prevData, const ExtraCustomData& customData);

	GestureEnums::BodySide HandsOverHead(const Probender* probender, const CompleteData& currData,
		const CompleteData& prevData, const ExtraCustomData& customData);
};

