#include "Controllers.h"

#include "Projectile.h"
#include "RigidBodyComponent.h"
#include "HelperFunctions.h"
#include "Probender.h"


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

void HandMoveController::ControlProjectile(Probender* bender, const CompleteData& currentData, 
	const CompleteData& previousData)
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

		UpdateProjectilePosition(bender, 0.4f, r_shoulder, r_hand);
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

		UpdateProjectilePosition(bender, 0.4f, l_shoulder, l_hand);
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
		float armLenAvg = (0.4f + 0.4f) * 0.5f;

		UpdateProjectilePosition(bender, armLenAvg, shoulderAvg, handAvg);
	}
}

void HandMoveController::UpdateProjectilePosition(Probender* probender, float armLength, 
		const CameraSpacePoint& shoulderPos, const CameraSpacePoint& handPos)
{
	if(!projectile)
		return;

	physx::PxVec3 newPos = projectile->GetRigidBody()->GetPosition();

	float maxVal = 0.0f;
	float minVal = 0.0f;
	float handPercentage = 0.0f;

	Ogre::Vector3 xPos = Ogre::Vector3(0.0f);
	Ogre::Vector3 yPos = xPos, zPos = xPos;
	
	if(handAxis & HA_X)
	{
		maxVal = shoulderPos.X - armLength;
		minVal = shoulderPos.X + armLength;
		handPercentage = HelperFunctions::CalculatePercentage(minVal, maxVal, handPos.X);
		
		xPos = probender->Right() * HelperFunctions::CalculateValue(minOffset.x, maxOffset.x, handPercentage);
		//newPos = ProjectileOrigin + ProbenderRight * HelperFunctions::CalculateValue(minOffset.x, maxOffset.x, handPercentage);
	}
	if(handAxis & HA_Y)
	{
		maxVal = shoulderPos.Y + armLength;
		minVal = shoulderPos.Y - armLength;
		handPercentage = HelperFunctions::CalculatePercentage(minVal, maxVal, handPos.Y);
		
		yPos = probender->Up() * HelperFunctions::CalculateValue(minOffset.y, maxOffset.y, handPercentage);
		//newPos.y = ProjectileOrigin.y + HelperFunctions::CalculateValue(minOffset.y, maxOffset.y, handPercentage);
	}
	if(handAxis & HA_Z)
	{
		maxVal = shoulderPos.Z - armLength;
		minVal = shoulderPos.Z + armLength;
		handPercentage = HelperFunctions::CalculatePercentage(minVal, maxVal, handPos.Z);

		zPos = probender->Forward() * HelperFunctions::CalculateValue(minOffset.z, maxOffset.z, handPercentage);
		//newPos.z = ProjectileOrigin.z + HelperFunctions::CalculateValue(minOffset.z, maxOffset.z, handPercentage);
	}

	Ogre::Vector3 result = xPos + yPos + zPos;

	newPos = ProjectileOrigin + HelperFunctions::OgreToPhysXVec3(result);

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
		else if(prevResults == GestureEnums::BODYSIDE_BOTH)
			HandUsed = CH_BOTH;
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
