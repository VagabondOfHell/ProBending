#include "Controllers.h"

#include "Projectile.h"
#include "RigidBodyComponent.h"
#include "HelperFunctions.h"


HandMoveController::HandMoveController(Projectile* projectileToControl, ControllingHand handToUse, 
						const physx::PxVec3& _minOffset, const physx::PxVec3& _maxOffset,
						GestureEnums::TransitionRules transitionFromPrevious/* = GestureEnums::TRANRULE_NONE*/)
	:ProjectileController(projectileToControl, transitionFromPrevious), HandUsed(handToUse), 
		minOffset(_minOffset), maxOffset(_maxOffset), handAxis(0)
{
	DualHandThreshold = 1.0f;
	UpdateHandAxis();
}

void HandMoveController::UpdateHandAxis()
{
	if(minOffset.x != maxOffset.x)
		handAxis |= HA_X;
	if(minOffset.y != maxOffset.y)
		handAxis |= HA_Y;
	if(minOffset.z != maxOffset.z)
		handAxis |= HA_Z;
}

HandMoveController::~HandMoveController(void)
{
}

void HandMoveController::ControlProjectile(Probender* bender, const BodyDimensions& bodyDimensions, 
			const CompleteData& currentData, const CompleteData& previousData)
{
	if(HandUsed == CH_RIGHT)
	{
		CameraSpacePoint r_shoulder;
		CameraSpacePoint r_hand;

		//Try to get right shoulder position either this frame or last frame
		if(!currentData.GetTrackedOrInferredPosition(JointType_ShoulderRight, r_shoulder))
			if(!previousData.GetTrackedOrInferredPosition(JointType_ShoulderRight, r_shoulder))
				return;

		//Try to get right hand position either this frame or last frame
		if(!currentData.GetTrackedOrInferredPosition(JointType_HandRight, r_hand))
			if(!previousData.GetTrackedOrInferredPosition(JointType_HandRight, r_hand))
				return;

		UpdateProjectilePosition(bodyDimensions.RightArmLength, r_shoulder, r_hand);
	}
	else if(HandUsed == CH_LEFT)
	{
		CameraSpacePoint l_shoulder;
		CameraSpacePoint l_hand;

		//Try to get right shoulder position either this frame or last frame
		if(!currentData.GetTrackedOrInferredPosition(JointType_ShoulderLeft, l_shoulder))
			if(!previousData.GetTrackedOrInferredPosition(JointType_ShoulderLeft, l_shoulder))
				return;

		//Try to get right hand position either this frame or last frame
		if(!currentData.GetTrackedOrInferredPosition(JointType_HandLeft, l_hand))
			if(!previousData.GetTrackedOrInferredPosition(JointType_HandLeft, l_hand))
				return;

		UpdateProjectilePosition(bodyDimensions.LeftArmLength, l_shoulder, l_hand);
	}
	else if(HandUsed == CH_BOTH)
	{
		CameraSpacePoint l_shoulder;
		CameraSpacePoint r_shoulder;
		CameraSpacePoint l_hand;
		CameraSpacePoint r_hand;

		//Try to get right shoulder position either this frame or last frame
		if(!currentData.GetTrackedOrInferredPosition(JointType_ShoulderLeft, l_shoulder))
			if(!previousData.GetTrackedOrInferredPosition(JointType_ShoulderLeft, l_shoulder))
				return;

		//Try to get right hand position either this frame or last frame
		if(!currentData.GetTrackedOrInferredPosition(JointType_HandLeft, l_hand))
			if(!previousData.GetTrackedOrInferredPosition(JointType_HandLeft, l_hand))
				return;

		//Try to get right shoulder position either this frame or last frame
		if(!currentData.GetTrackedOrInferredPosition(JointType_ShoulderRight, r_shoulder))
			if(!previousData.GetTrackedOrInferredPosition(JointType_ShoulderRight, r_shoulder))
				return;

		//Try to get right hand position either this frame or last frame
		if(!currentData.GetTrackedOrInferredPosition(JointType_HandRight, r_hand))
			if(!previousData.GetTrackedOrInferredPosition(JointType_HandRight, r_hand))
				return;

		CameraSpacePoint handDiff; handDiff.X = r_hand.X - l_hand.X; handDiff.Y = r_hand.Y - l_hand.Y; handDiff.Z = r_hand.Z - l_hand.Z;
		float distBtwnHands = HelperFunctions::CalculateMagnitudeSqrd(handDiff.X, handDiff.Y, handDiff.Z);

		//if hands are too separated, ignore
		if(distBtwnHands > DualHandThreshold)
			return;

		//Calculate the average position between the two hands
		CameraSpacePoint handAvg; handAvg.X = (r_hand.X + l_hand.X) * 0.5f; 
			handAvg.Y = (r_hand.Y + l_hand.Y) * 0.5f; handAvg.Z = (r_hand.Z + l_hand.Z) * 0.5f;

		//Calculate average position between the shoulders
		CameraSpacePoint shoulderAvg; shoulderAvg.X = (r_shoulder.X + l_shoulder.X) * 0.5f; 
			shoulderAvg.Y = (r_shoulder.Y + l_shoulder.Y) * 0.5f; shoulderAvg.Z = (r_shoulder.Z + l_shoulder.Z) * 0.5f;

		//Calculate the arm length average
		float armLenAvg = (bodyDimensions.LeftArmLength + bodyDimensions.RightArmLength) * 0.5f;

		UpdateProjectilePosition(armLenAvg, shoulderAvg, handAvg);
	}
}

void HandMoveController::UpdateProjectilePosition(float armLength, const CameraSpacePoint& shoulderPos, const CameraSpacePoint& handPos)
{
	physx::PxVec3 newPos = projectile->GetRigidBody()->GetPosition();

	float maxVal = 0.0f;
	float minVal = 0.0f;
	float handPercentage = 0.0f;

	physx::PxVec3 xPos = physx::PxVec3(0.0f);
	physx::PxVec3 yPos = xPos, zPos = xPos;
	
	if(handAxis & HA_X)
	{
		maxVal = shoulderPos.X + armLength;
		minVal = shoulderPos.X - armLength;
		handPercentage = HelperFunctions::CalculatePercentage(minVal, maxVal, handPos.X);
		
		xPos = ProbenderRight * HelperFunctions::CalculateValue(minOffset.x, maxOffset.x, handPercentage);
		//newPos = ProjectileOrigin + ProbenderRight * HelperFunctions::CalculateValue(minOffset.x, maxOffset.x, handPercentage);
	}
	if(handAxis & HA_Y)
	{
		maxVal = shoulderPos.Y + armLength;
		minVal = shoulderPos.Y - armLength;
		handPercentage = HelperFunctions::CalculatePercentage(minVal, maxVal, handPos.Y);
		
		yPos = ProbenderUp * HelperFunctions::CalculateValue(minOffset.y, maxOffset.y, handPercentage);
		//newPos.y = ProjectileOrigin.y + HelperFunctions::CalculateValue(minOffset.y, maxOffset.y, handPercentage);
	}
	if(handAxis & HA_Z)
	{
		maxVal = shoulderPos.Z + armLength;
		minVal = shoulderPos.Z - armLength;
		handPercentage = HelperFunctions::CalculatePercentage(minVal, maxVal, handPos.Z);

		zPos = ProbenderForward * HelperFunctions::CalculateValue(minOffset.z, maxOffset.z, handPercentage);
		//newPos.z = ProjectileOrigin.z + HelperFunctions::CalculateValue(minOffset.z, maxOffset.z, handPercentage);
	}

	newPos = ProjectileOrigin + xPos + yPos + zPos;

	projectile->SetWorldPosition(newPos.x, newPos.y, newPos.z);
}

void HandMoveController::ReceivePreviousResults(GestureEnums::BodySide prevResults)
{
	switch (TransitionFromPrevious)
	{
	case GestureEnums::TRANRULE_NONE:
		return;
		break;
	case GestureEnums::TRANRULE_SAME:
		if(prevResults == GestureEnums::BODYSIDE_LEFT)
			HandUsed = CH_LEFT;
		else if(prevResults == GestureEnums::BODYSIDE_RIGHT)
			HandUsed = CH_RIGHT;
		break;
	case GestureEnums::TRANRULE_OPPOSITE:
		if(prevResults == GestureEnums::BODYSIDE_LEFT)
			HandUsed = CH_RIGHT;
		else if(prevResults == GestureEnums::BODYSIDE_RIGHT)
			HandUsed = CH_LEFT;
		break;
	default:
		break;
	}
}
