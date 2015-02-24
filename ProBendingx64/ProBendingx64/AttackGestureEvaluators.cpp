#include "AttackGestureEvaluators.h"

GestureEnums::BodySide AttackGestureEvaluators::KneeRaiseGesture(const Probender* probender, 
		const BodyDimensions& bodyDimensions, const CompleteData& currData, 
		const CompleteData& prevData, const ExtraCustomData& customData)
{
	if(customData.Side == GestureEnums::BODYSIDE_INVALID)
		return GestureEnums::BODYSIDE_INVALID;

	if(customData.Side == GestureEnums::BODYSIDE_RIGHT || customData.Side == GestureEnums::BODYSIDE_EITHER)
	{
		if(currData.JointData[JointType_KneeRight].Position.Y >= currData.JointData[JointType_SpineBase].Position.Y)
			return GestureEnums::BODYSIDE_RIGHT;
	}
	
	if(customData.Side == GestureEnums::BODYSIDE_LEFT || customData.Side == GestureEnums::BODYSIDE_EITHER)
	{
		if(currData.JointData[JointType_KneeLeft].Position.Y >= currData.JointData[JointType_SpineBase].Position.Y)
			return GestureEnums::BODYSIDE_LEFT;
	}
	
	if (customData.Side == GestureEnums::BODYSIDE_BOTH)
	{
		if(currData.JointData[JointType_KneeRight].Position.Y >= currData.JointData[JointType_SpineBase].Position.Y &&
			currData.JointData[JointType_KneeLeft].Position.Y >= currData.JointData[JointType_SpineBase].Position.Y)
			return GestureEnums::BODYSIDE_BOTH;
	}

	return GestureEnums::BODYSIDE_INVALID;

}

GestureEnums::BodySide AttackGestureEvaluators::KneeDownGesture(const Probender* probender, 
		const BodyDimensions& bodyDimensions, const CompleteData& currData, 
		const CompleteData& prevData, const ExtraCustomData& customData)
{
	if(customData.Side == GestureEnums::BODYSIDE_INVALID)
		return GestureEnums::BODYSIDE_INVALID;

	if(customData.Side == GestureEnums::BODYSIDE_RIGHT || customData.Side == GestureEnums::BODYSIDE_EITHER)
	{ 
		if(currData.JointData[JointType_KneeRight].Position.Y < currData.JointData[JointType_HipRight].Position.Y)
		{
			float yDiff = currData.JointData[JointType_KneeRight].Position.Y - currData.JointData[JointType_HipRight].Position.Y;

			if(yDiff >= bodyDimensions.RightHipToKnee)
				return GestureEnums::BODYSIDE_RIGHT;
		}
	}

	if(customData.Side == GestureEnums::BODYSIDE_LEFT || customData.Side == GestureEnums::BODYSIDE_EITHER)
	{
		if(currData.JointData[JointType_KneeLeft].Position.Y < currData.JointData[JointType_HipLeft].Position.Y)
		{
			float yDiff = currData.JointData[JointType_KneeLeft].Position.Y - currData.JointData[JointType_HipLeft].Position.Y;

			if(yDiff >= bodyDimensions.LeftHipToKnee)
				return GestureEnums::BODYSIDE_LEFT;
		}
	}

	if (customData.Side == GestureEnums::BODYSIDE_BOTH)
	{
		if(currData.JointData[JointType_KneeRight].Position.Y < currData.JointData[JointType_HipRight].Position.Y &&
			currData.JointData[JointType_KneeLeft].Position.Y < currData.JointData[JointType_HipLeft].Position.Y)
		{
			float rightYDiff = currData.JointData[JointType_KneeRight].Position.Y - currData.JointData[JointType_HipRight].Position.Y;
			float leftYDiff = currData.JointData[JointType_KneeLeft].Position.Y - currData.JointData[JointType_HipLeft].Position.Y;

			if(rightYDiff >= bodyDimensions.RightHipToKnee && leftYDiff >= bodyDimensions.LeftHipToKnee)
				return GestureEnums::BODYSIDE_BOTH;
		}
	}

	return GestureEnums::BODYSIDE_INVALID;
}

GestureEnums::BodySide AttackGestureEvaluators::ArmPunchGesture(const Probender* probender, 
		const BodyDimensions& bodyDimensions, const CompleteData& currData, 
		const CompleteData& prevData, const ExtraCustomData& customData)
{
	if(customData.Side == GestureEnums::BODYSIDE_INVALID)
		return GestureEnums::BODYSIDE_INVALID;

	if(customData.Side == GestureEnums::BODYSIDE_RIGHT || customData.Side == GestureEnums::BODYSIDE_EITHER)
	{
		float zDiff = currData.JointData[JointType_HandRight].Position.Z - prevData.JointData[JointType_HandRight].Position.Z;
		//Check if the right hand has moved over half the length of the arm towards the sensor
		if(zDiff >= 0.5f * bodyDimensions.RightArmLength)
			return GestureEnums::BODYSIDE_RIGHT;
	}

	if(customData.Side == GestureEnums::BODYSIDE_LEFT || customData.Side == GestureEnums::BODYSIDE_EITHER)
	{
		float zDiff = currData.JointData[JointType_HandLeft].Position.Z - prevData.JointData[JointType_HandLeft].Position.Z;
		//Check if the right hand has moved over half the length of the arm towards the sensor
		if(zDiff >= 0.5f * bodyDimensions.LeftArmLength)
			return GestureEnums::BODYSIDE_LEFT;
	}

	if (customData.Side == GestureEnums::BODYSIDE_BOTH)
	{
		float rightZDiff = currData.JointData[JointType_HandRight].Position.Z - prevData.JointData[JointType_HandRight].Position.Z;
		float leftZDiff = currData.JointData[JointType_HandLeft].Position.Z - prevData.JointData[JointType_HandLeft].Position.Z;

		if(rightZDiff >= 0.5f * bodyDimensions.RightArmLength && leftZDiff >= 0.5f * bodyDimensions.LeftArmLength)
			return GestureEnums::BODYSIDE_BOTH;
	}

	return GestureEnums::BODYSIDE_INVALID;
}

