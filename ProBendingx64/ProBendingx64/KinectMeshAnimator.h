#pragma once
#include <string>

#include "ProbenderFlags.h"

#include "KinectBody.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"

namespace Ogre
{
	class Bone;
};

class KinectMeshAnimator
{
public:
	enum MappableJoints{MJ_SPINE_BASE, MJ_SPINE_MID, MJ_SPINE_SHOULDER, MJ_HEAD, MJ_NECK, 
		MJ_SHOULDER_R, MJ_UPPER_ARM_R, MJ_FOREARM_R, MJ_WRIST_R,
		MJ_SHOULDER_L, MJ_UPPER_ARM_L, MJ_FOREARM_L, MJ_WRIST_L,
		MJ_HIPS_R, MJ_THIGH_R, MJ_SHIN_R, MJ_ANKLE_R, MJ_FOOT_R,
		MJ_HIPS_L, MJ_THIGH_L, MJ_SHIN_L, MJ_ANKLE_L, MJ_FOOT_L,
		MJ_COUNT
	};

	//enum detailing how to filter data. Tracked only to update joints that are 
	//currently tracked by the kinect, inferred to have tracked and inferred,
	//All to have tracked, inferred, and not tracked
	enum JointTolerance{JT_TRACKED_ONLY, JT_INFERRED, JT_ALL};

	JointTolerance ToleranceScale;

private:
	static const Ogre::Radian RADIAN_180;

	Ogre::Bone* boneMapping[MJ_COUNT];
	bool ignorePositions[MJ_COUNT];
	bool ignoreOrientations[MJ_COUNT];

	Ogre::SkeletonInstance* skeleton;

	Ogre::Quaternion validRootQuat;
	Ogre::Vector3 validRootPos;

	inline bool IsValidJointData(const TrackingState trackState)const
	{
		switch (ToleranceScale)
		{
		case KinectMeshAnimator::JT_TRACKED_ONLY:
			return trackState == TrackingState::TrackingState_Tracked;
			break;
		case KinectMeshAnimator::JT_INFERRED:
			return trackState == TrackingState::TrackingState_Inferred || trackState == TrackingState::TrackingState_Tracked;
			break;
		case KinectMeshAnimator::JT_ALL:
			return true;
			break;
		}
		return true;
	}

	///<summary>Gets the Kinect Joint representation of the Mappable Joints enum</summary>
	///<param name="mapJoint">The joint to convert to the Kinect Enum</param>
	///<returns>The Kinect Enum version of the mappable joint</returns>
	inline static RenderableJointType::RenderableJointType ConvertBoneMappingJoint(const MappableJoints mapJoint)
	{
		switch (mapJoint)
		{
		case KinectMeshAnimator::MJ_SPINE_BASE:
			return RenderableJointType::SpineBase;
			break;
		case KinectMeshAnimator::MJ_SPINE_MID:
			return RenderableJointType::SpineMid;
			break;
		case KinectMeshAnimator::MJ_SPINE_SHOULDER:
			return RenderableJointType::SpineShoulder;
			break;
		case KinectMeshAnimator::MJ_HEAD:
			return RenderableJointType::Head;
			break;
		case KinectMeshAnimator::MJ_NECK:
			return RenderableJointType::Neck;
			break;
		case KinectMeshAnimator::MJ_SHOULDER_R:
			return RenderableJointType::ShoulderRight;
			break;
		case KinectMeshAnimator::MJ_UPPER_ARM_R:
			return RenderableJointType::ElbowRight;
			break;
		case KinectMeshAnimator::MJ_FOREARM_R:
			return RenderableJointType::WristRight;
			break;
		case KinectMeshAnimator::MJ_WRIST_R:
			return RenderableJointType::WristRight;
			break;
		case KinectMeshAnimator::MJ_SHOULDER_L:
			return RenderableJointType::ShoulderLeft;
			break;
		case KinectMeshAnimator::MJ_UPPER_ARM_L:
			return RenderableJointType::ElbowLeft;
			break;
		case KinectMeshAnimator::MJ_FOREARM_L:
			return RenderableJointType::WristLeft;
			break;
		case KinectMeshAnimator::MJ_WRIST_L:
			return RenderableJointType::WristLeft;
			break;
		case KinectMeshAnimator::MJ_HIPS_R:
			return RenderableJointType::HipRight;
			break;
		case KinectMeshAnimator::MJ_THIGH_R:
			return RenderableJointType::KneeRight;
			break;
		case KinectMeshAnimator::MJ_SHIN_R:
			return RenderableJointType::AnkleRight;
			break;
		case KinectMeshAnimator::MJ_ANKLE_R:
			return RenderableJointType::AnkleRight;
			break;
		case KinectMeshAnimator::MJ_FOOT_R:
			return RenderableJointType::FootRight;
			break;
		case KinectMeshAnimator::MJ_HIPS_L:
			return RenderableJointType::HipLeft;
			break;
		case KinectMeshAnimator::MJ_THIGH_L:
			return RenderableJointType::KneeLeft;
			break;
		case KinectMeshAnimator::MJ_SHIN_L:
			return RenderableJointType::AnkleLeft;
			break;
		case KinectMeshAnimator::MJ_ANKLE_L:
			return RenderableJointType::AnkleLeft;
			break;
		case KinectMeshAnimator::MJ_FOOT_L:
			return RenderableJointType::FootLeft;
			break;
		case KinectMeshAnimator::MJ_COUNT:
			return RenderableJointType::Count;
			break;
		default:
			return RenderableJointType::Count;
			break;
		}
	}

	Ogre::Quaternion GetJointOrientation(const RenderableJointType::RenderableJointType joint, const CompleteData& currData);

public:
	KinectMeshAnimator(Ogre::Entity* mesh = NULL);
	~KinectMeshAnimator(void);

	///<summary>Sets the skeleton to be used for this animator</summary>
	///<param name="mesh">The entity containing the skeleton</param>
	void SetEntity(Ogre::Entity* mesh);

	///<summary>Converts a Kinect Vector4 to an Ogre Quaternion with the option to rotate to Ogre space</summary>
	///<param name="kinectQuat">The kinect quaternion to convert</param>
	///<param name="rotateY">True to rotate 180 on the Y axis to convert to Ogre Space</param>
	///<returns>The ogre version of the quaternion</returns>
	inline Ogre::Quaternion KinectToOgreQuaternion(const Vector4& kinectQuat, const bool rotateY = true)
	{
		return Ogre::Quaternion(-kinectQuat.y, kinectQuat.z, kinectQuat.w, -kinectQuat.x);	
	}

	///<summary>Converts a kinect camera space point to an Ogre Vector3</summary>
	///<param name="kinectPos">The kinect space point to convert</param>
	///<returns>An Ogre Vector3 representing the camera space point</returns>
	inline Ogre::Vector3 KinectToOgrePosition(const CameraSpacePoint& kinectPos)
	{
		return Ogre::Vector3(kinectPos.X, kinectPos.Y, kinectPos.Z);
	}

	///<summary>Map a mesh bone to a kinect joint</summary>
	///<param name="boneName">The name of the bone to map</param>
	///<param name="jointMapped">The joint to connect with</param>
	///<returns>True if successful, false if invalid joint, invalid bone, or no skeleton set</returns>
	bool MapJointToBone(const std::string& boneName, const MappableJoints jointMapped, const bool inheritOrientation = false,
		const bool ignorePosition = false, const bool ignoreOrientation = false);

	///<summary>Sets whether or not to inherit orientation for the specified bone</summary>
	///<param name="jointToMap">The joint that the bone is stored under</param>
	///<param name="val">The new value for inheriting orientation</param>
	///<returns>True if successful, false if the bone wasn't found</returns>
	void SetInheritOrientation(const MappableJoints jointToMap, const bool val);

	///<summary>Updates the mesh with the new data according to the tolerance scale</summary>
	///<param name="currentData">The new frame data to use to update the skeleton with</param>
	void UpdateSkeleton(const CompleteData& currentData);

};

