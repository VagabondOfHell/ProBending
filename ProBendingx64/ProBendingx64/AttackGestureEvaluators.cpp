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
		{
			return GestureEnums::BODYSIDE_RIGHT;
		}
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
			CameraSpacePoint rightShoulderPos = currData.JointData[JointType_ShoulderRight].Position;
			CameraSpacePoint leftShoulderPos = currData.JointData[JointType_ShoulderLeft].Position;

			//Use the difference between the shoulders (X axis) as the y difference between knee and ground
			float desiredLength =  Ogre::Math::Abs(rightShoulderPos.X - leftShoulderPos.X);

			float yDiff = Ogre::Math::Abs(
				currData.JointData[JointType_KneeRight].Position.Y - currData.JointData[JointType_HipRight].Position.Y);
			
			if(yDiff >= desiredLength)
				return GestureEnums::BODYSIDE_RIGHT;
		}
	}

	if(customData.Side == GestureEnums::BODYSIDE_LEFT || customData.Side == GestureEnums::BODYSIDE_EITHER)
	{
		if(currData.JointData[JointType_KneeLeft].Position.Y < currData.JointData[JointType_HipLeft].Position.Y)
		{
			CameraSpacePoint rightShoulderPos = currData.JointData[JointType_ShoulderRight].Position;
			CameraSpacePoint leftShoulderPos = currData.JointData[JointType_ShoulderLeft].Position;

			//Use the difference between the shoulders (X axis) as the y difference between knee and ground
			float desiredLength =  Ogre::Math::Abs(rightShoulderPos.X - leftShoulderPos.X);

			float yDiff = Ogre::Math::Abs(
				currData.JointData[JointType_KneeLeft].Position.Y - currData.JointData[JointType_HipLeft].Position.Y);

			if(yDiff >= desiredLength)
				return GestureEnums::BODYSIDE_LEFT;
		}
	}

	if (customData.Side == GestureEnums::BODYSIDE_BOTH)
	{
		if(currData.JointData[JointType_KneeRight].Position.Y < currData.JointData[JointType_HipRight].Position.Y &&
			currData.JointData[JointType_KneeLeft].Position.Y < currData.JointData[JointType_HipLeft].Position.Y)
		{
			CameraSpacePoint rightShoulderPos = currData.JointData[JointType_ShoulderRight].Position;
			CameraSpacePoint leftShoulderPos = currData.JointData[JointType_ShoulderLeft].Position;

			//Use the difference between the shoulders (X axis) as the y difference between knee and ground
			float desiredLength =  Ogre::Math::Abs(rightShoulderPos.X - leftShoulderPos.X);

			float rightYDiff = currData.JointData[JointType_KneeRight].Position.Y - currData.JointData[JointType_HipRight].Position.Y;
			float leftYDiff = currData.JointData[JointType_KneeLeft].Position.Y - currData.JointData[JointType_HipLeft].Position.Y;

			if(rightYDiff >= desiredLength && leftYDiff >= desiredLength)
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
		bool validData = true;

		//Check hand validity (tracked state only)
		if(currData.JointData[JointType_HandRight].TrackingState != TrackingState::TrackingState_Tracked ||
			prevData.JointData[JointType_HandRight].TrackingState != TrackingState::TrackingState_Tracked )
			validData = false;

		if(currData.RightHandState != HandState::HandState_Closed || currData.RightHandConfidence < 0.35f)
			validData = false;

		//Check shoulder validity (inferred is fine)
		if(currData.JointData[JointType_ShoulderRight].TrackingState == TrackingState::TrackingState_NotTracked
			|| currData.JointData[JointType_ShoulderLeft].TrackingState == TrackingState::TrackingState_NotTracked)
			validData = false;

		if(validData)
		{
			CameraSpacePoint currHandPos = currData.JointData[JointType_HandRight].Position;
			CameraSpacePoint prevHandPos = prevData.JointData[JointType_HandRight].Position;
			CameraSpacePoint rightShoulderPos = currData.JointData[JointType_ShoulderRight].Position;
			CameraSpacePoint leftShoulderPos = currData.JointData[JointType_ShoulderLeft].Position;

			float zDiff = currHandPos.Z - prevHandPos.Z;
			float shoulderDist = rightShoulderPos.X - leftShoulderPos.X;
			float zDiffFromShoulders = currHandPos.Z - rightShoulderPos.Z;
			float yDiffFromShoulder = currHandPos.Y - rightShoulderPos.Y;

			if(-zDiff >= shoulderDist * 0.3f && //Check if the hand has moved quickly towards the kinect, 
				-zDiffFromShoulders >= shoulderDist * 0.75f && //That the arm is extended at least as long as 75% the difference in shoulder size
				-yDiffFromShoulder <= 0.5f * shoulderDist)//And that the hand is roughly as high as the shoulder
				return GestureEnums::BODYSIDE_RIGHT;
		}
	}

	if(customData.Side == GestureEnums::BODYSIDE_LEFT || customData.Side == GestureEnums::BODYSIDE_EITHER)
	{
		bool validData = true;

		if(currData.JointData[JointType_HandLeft].TrackingState != TrackingState::TrackingState_Tracked ||
			prevData.JointData[JointType_HandLeft].TrackingState != TrackingState::TrackingState_Tracked )
			validData = false;

		if(currData.LeftHandState != HandState::HandState_Closed || currData.LeftHandConfidence < 0.35f)
			validData = false;

		//Check shoulder validity (inferred is fine)
		if(currData.JointData[JointType_ShoulderRight].TrackingState == TrackingState::TrackingState_NotTracked
			|| currData.JointData[JointType_ShoulderLeft].TrackingState == TrackingState::TrackingState_NotTracked)
			validData = false;

		if(validData)
		{
			CameraSpacePoint currHandPos = currData.JointData[JointType_HandLeft].Position;
			CameraSpacePoint prevHandPos = prevData.JointData[JointType_HandLeft].Position;
			CameraSpacePoint rightShoulderPos = currData.JointData[JointType_ShoulderRight].Position;
			CameraSpacePoint leftShoulderPos = currData.JointData[JointType_ShoulderLeft].Position;

			float zDiff = currHandPos.Z - prevHandPos.Z;
			float shoulderDist = rightShoulderPos.X - leftShoulderPos.X;
			float zDiffFromShoulders = currHandPos.Z - leftShoulderPos.Z;
			float yDiffFromShoulder = currHandPos.Y - leftShoulderPos.Y;

			if(-zDiff >= shoulderDist * 0.3f && //Check if the hand has moved quickly towards the kinect, 
				-zDiffFromShoulders >= shoulderDist * 0.75f && //That the arm is extended at least as long as the difference in shoulder size
				-yDiffFromShoulder <= 0.5f * shoulderDist)//And that the hand is roughly as high as the shoulder
				return GestureEnums::BODYSIDE_LEFT;
		}
	}

	if (customData.Side == GestureEnums::BODYSIDE_BOTH)
	{
		bool validData = true;

		if(currData.JointData[JointType_HandRight].TrackingState == TrackingState::TrackingState_NotTracked ||
			prevData.JointData[JointType_HandRight].TrackingState == TrackingState::TrackingState_NotTracked ||
			currData.JointData[JointType_HandLeft].TrackingState == TrackingState::TrackingState_NotTracked ||
			prevData.JointData[JointType_HandLeft].TrackingState == TrackingState::TrackingState_NotTracked)
			validData = false;

		//Check shoulder validity (inferred is fine)
		if(currData.JointData[JointType_ShoulderRight].TrackingState == TrackingState::TrackingState_NotTracked
			|| currData.JointData[JointType_ShoulderLeft].TrackingState == TrackingState::TrackingState_NotTracked)
			validData = false;

		if(validData)
		{
			CameraSpacePoint rightCurrHandPos = currData.JointData[JointType_HandRight].Position;
			CameraSpacePoint rightPrevHandPos = prevData.JointData[JointType_HandRight].Position;
			CameraSpacePoint leftCurrHandPos = currData.JointData[JointType_HandLeft].Position;
			CameraSpacePoint leftPrevHandPos = prevData.JointData[JointType_HandLeft].Position;
			CameraSpacePoint rightShoulderPos = currData.JointData[JointType_ShoulderRight].Position;
			CameraSpacePoint leftShoulderPos = currData.JointData[JointType_ShoulderLeft].Position;

			float rightZDiff = rightCurrHandPos.Z - rightPrevHandPos.Z;
			float leftZDiff = leftCurrHandPos.Z - leftPrevHandPos.Z;

			float shoulderDist = rightShoulderPos.X - leftShoulderPos.X;
			float rightZDiffFromShoulders = rightCurrHandPos.Z - rightShoulderPos.Z;
			float leftZDiffFromShoulders = leftCurrHandPos.Z - leftShoulderPos.Z;

			float rightYDiffFromShoulder = rightCurrHandPos.Y - rightShoulderPos.Y;
			float leftYDiffFromShoulder = leftCurrHandPos.Y - leftShoulderPos.Y;

			if(-rightZDiff >= shoulderDist * 0.3f && -leftZDiff >= shoulderDist * 0.3f &&
				-rightZDiffFromShoulders >= shoulderDist * 0.75f && -leftZDiffFromShoulders >= shoulderDist * 0.75f &&
				-rightYDiffFromShoulder <= 0.5f * shoulderDist && -leftYDiffFromShoulder <= 0.5f * shoulderDist)
				return GestureEnums::BODYSIDE_BOTH;
		}
	}

	return GestureEnums::BODYSIDE_INVALID;
}

GestureEnums::BodySide AttackGestureEvaluators::ArmsWideGesture(const Probender* probender, 
		const BodyDimensions& bodyDimensions, const CompleteData& currData, 
		const CompleteData& prevData, const ExtraCustomData& customData)
{
	if(currData.JointData[JointType_HandRight].TrackingState == TrackingState::TrackingState_NotTracked ||
		currData.JointData[JointType_HandLeft].TrackingState == TrackingState::TrackingState_NotTracked)
		return GestureEnums::BODYSIDE_INVALID;

	//Check shoulder validity (inferred is fine)
	if(currData.JointData[JointType_ShoulderRight].TrackingState == TrackingState::TrackingState_NotTracked
		|| currData.JointData[JointType_ShoulderLeft].TrackingState == TrackingState::TrackingState_NotTracked)
		return GestureEnums::BODYSIDE_INVALID;	

	CameraSpacePoint rightCurrHandPos = currData.JointData[JointType_HandRight].Position;
	CameraSpacePoint leftCurrHandPos = currData.JointData[JointType_HandLeft].Position;
	CameraSpacePoint rightShoulderPos = currData.JointData[JointType_ShoulderRight].Position;
	CameraSpacePoint leftShoulderPos = currData.JointData[JointType_ShoulderLeft].Position;

	float xRightShoulderDiff = Ogre::Math::Abs(rightCurrHandPos.X - rightShoulderPos.X);
	float xLeftShoulderDiff = Ogre::Math::Abs(leftCurrHandPos.X - leftShoulderPos.X);

	float xShoulderLength = Ogre::Math::Abs(rightShoulderPos.X - leftShoulderPos.X);
	float xTarget = xShoulderLength * 1.2f;

	if(xRightShoulderDiff >= xTarget && xLeftShoulderDiff >= xTarget)
	{
		printf("Arms Extended\n");
		return GestureEnums::BODYSIDE_BOTH;
	}
	return GestureEnums::BODYSIDE_INVALID;
}

GestureEnums::BodySide AttackGestureEvaluators::HandsClapGesture(const Probender* probender, 
		const BodyDimensions& bodyDimensions, const CompleteData& currData, 
		const CompleteData& prevData, const ExtraCustomData& customData)
{
	//printf("Before Hands\n");
	if(currData.JointData[JointType_HandRight].TrackingState == TrackingState::TrackingState_NotTracked ||
		currData.JointData[JointType_HandLeft].TrackingState == TrackingState::TrackingState_NotTracked)
		return GestureEnums::BODYSIDE_INVALID;

	CameraSpacePoint rightCurrHandPos = currData.JointData[JointType_HandRight].Position;
	CameraSpacePoint leftCurrHandPos = currData.JointData[JointType_HandLeft].Position;

	float xDist = Ogre::Math::Abs(rightCurrHandPos.X - leftCurrHandPos.X);

	//printf("Clap Val: %f\n", xDist);

	if(xDist <= 0.5f)
	{
		printf("Hands Clapped\n");
		return GestureEnums::BODYSIDE_BOTH;
	}

	return GestureEnums::BODYSIDE_INVALID;
}

