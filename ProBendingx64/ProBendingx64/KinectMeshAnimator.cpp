#include "KinectMeshAnimator.h"

#include "OgreEntity.h"
#include "OgreSkeletonInstance.h"
#include "OgreBone.h"

const Ogre::Radian KinectMeshAnimator::RADIAN_180 = Ogre::Radian(Ogre::Degree(180.0f));

KinectMeshAnimator::KinectMeshAnimator(Ogre::Entity* mesh /* = NULL*/)
	:skeleton(NULL), ToleranceScale(JT_TRACKED_ONLY), validRootQuat(Ogre::Quaternion::IDENTITY), validRootPos(Ogre::Vector3::ZERO)
{
	if(mesh)
		skeleton = mesh->getSkeleton();

	for (unsigned int i = 0; i < MJ_COUNT; ++i)
	{
		boneMapping[i] = NULL;
	}
}


KinectMeshAnimator::~KinectMeshAnimator(void)
{
}

void KinectMeshAnimator::SetEntity(Ogre::Entity* mesh)
{
	if(mesh)
	{
		skeleton = mesh->getSkeleton();

		Ogre::Skeleton::BoneIterator boneIter = skeleton->getBoneIterator();

		while (boneIter.hasMoreElements())
		{
			boneIter.getNext()->setInitialState();
		}
	}
}

bool KinectMeshAnimator::MapJointToBone(const std::string& boneName, 
		const MappableJoints jointMapped, const bool inheritOrientation /*= false*/,
		const bool ignorePosition/* = false*/, const bool ignoreOrientation/* = false*/)
{
	//if no skeleton or invalid joint, break early
	if(!skeleton || jointMapped == MJ_COUNT)
		return false;

	Ogre::Bone* boneToMap;

	try
	{
		boneToMap = skeleton->getBone(boneName);
	}
	catch (Ogre::Exception e)
	{
		boneMapping[jointMapped] = NULL;
		return false;
	}

	if(boneToMap)
	{
		boneToMap->setManuallyControlled(true);
		boneToMap->setInheritOrientation(inheritOrientation);
		boneToMap->resetToInitialState();

		boneMapping[jointMapped] = boneToMap;

		ignorePositions[jointMapped] = ignorePosition;
		ignoreOrientations[jointMapped] = ignoreOrientation;

		return true;
	}

	return false;
}

void KinectMeshAnimator::SetInheritOrientation(const MappableJoints jointToMap, const bool val)
{
	Ogre::Bone* bone = boneMapping[jointToMap];

	if(bone)
		bone->setInheritOrientation(val);
}

Ogre::Quaternion KinectMeshAnimator::GetJointOrientation(const RenderableJointType::RenderableJointType joint, 
														 const CompleteData& currData)
{
	switch (joint)
	{
	case RenderableJointType::Head:
		return KinectToOgreQuaternion(currData.JointOrientations[RenderableJointType::Neck].Orientation);
		break;
	case RenderableJointType::HandTipRight:
		return KinectToOgreQuaternion(currData.JointOrientations[RenderableJointType::HandRight].Orientation);
		break;
	case RenderableJointType::ThumbRight:
		return KinectToOgreQuaternion(currData.JointOrientations[RenderableJointType::HandRight].Orientation);
		break;
	case RenderableJointType::HandTipLeft:
		return KinectToOgreQuaternion(currData.JointOrientations[RenderableJointType::HandLeft].Orientation);
		break;
	case RenderableJointType::ThumbLeft:
		return KinectToOgreQuaternion(currData.JointOrientations[RenderableJointType::HandLeft].Orientation);
		break;
	case RenderableJointType::FootLeft:
		return KinectToOgreQuaternion(currData.JointOrientations[RenderableJointType::AnkleLeft].Orientation);
		break;
	case RenderableJointType::FootRight:
		return KinectToOgreQuaternion(currData.JointOrientations[RenderableJointType::AnkleRight].Orientation);
		break;
	case RenderableJointType::Neck:
	case RenderableJointType::SpineShoulder:
	case RenderableJointType::SpineMid:
	case RenderableJointType::SpineBase:
	case RenderableJointType::ShoulderRight:
	case RenderableJointType::ElbowRight:
	case RenderableJointType::WristRight:
	case RenderableJointType::HandRight:
	case RenderableJointType::ShoulderLeft:
	case RenderableJointType::ElbowLeft:
	case RenderableJointType::WristLeft:
	case RenderableJointType::HandLeft:
	case RenderableJointType::HipLeft:
	case RenderableJointType::KneeLeft:
	case RenderableJointType::AnkleLeft:
	case RenderableJointType::HipRight:
	case RenderableJointType::KneeRight:
	case RenderableJointType::AnkleRight:
		return KinectToOgreQuaternion(currData.JointOrientations[joint].Orientation);
		break;
	default:
		return Ogre::Quaternion::IDENTITY;
		break;
	}
}

void KinectMeshAnimator::UpdateSkeleton(const CompleteData& currentData)
{
	//if the body is undecipherable, just use previous frames data
	if(currentData.IsEngaged == DetectionResult::DetectionResult_No //|| currentData.IsEngaged == DetectionResult::DetectionResult_Unknown
		|| currentData.IsTracked == FALSE)
		return;

	for (unsigned int i = 0; i < MappableJoints::MJ_COUNT; ++i)
	{
		MappableJoints mapCurrJoint = (MappableJoints)i;

		RenderableJointType::RenderableJointType kinectCurrJoint = ConvertBoneMappingJoint(mapCurrJoint);

		if(!IsValidJointData(currentData.JointData[kinectCurrJoint].TrackingState))
			continue;

		Ogre::Bone* currBone = boneMapping[mapCurrJoint];

		if(!currBone)
			continue;

		//Store the root position and root quaternion for use later on
		if(i == 0)
		{
			validRootPos = KinectToOgrePosition(currentData.JointData[RenderableJointType::SpineBase].Position);
			validRootQuat = KinectToOgreQuaternion(currentData.JointOrientations[RenderableJointType::SpineBase].Orientation);
		}

		if(!ignoreOrientations[mapCurrJoint])
		{
			currBone->resetOrientation(); 

			Ogre::Quaternion localOri = boneMapping[mapCurrJoint]->getParent()->_getDerivedOrientation() * 
				GetJointOrientation(kinectCurrJoint, currentData);

			if(mapCurrJoint == MJ_HEAD)
				currBone->setOrientation(localOri);
			else
				currBone->_setDerivedOrientation(localOri);
		}

		if(!ignorePositions[mapCurrJoint])
		{
			currBone->_setDerivedPosition(KinectToOgrePosition(currentData.JointData[kinectCurrJoint].Position));
		}
	}
}

