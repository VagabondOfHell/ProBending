#pragma once
#include <Kinect.h>
#include <array>
class KinectGestureReader;

typedef _FrameEdges ClippedEdges;

struct CompleteData
{
	UINT64 BodyTrackingID;

	BOOLEAN IsTracked;
	BOOLEAN IsRestricted;
	DetectionResult IsEngaged;

	ClippedEdges ClippedEdge;

	Joint JointData[JointType_Count];
	JointOrientation JointOrientations[JointType_Count];

	PointF LeanAmount;
	TrackingState LeanTrackState;

	HandState LeftHandState;
	HandState RightHandState;
	TrackingConfidence LeftHandConfidence;
	TrackingConfidence RightHandConfidence;

	CompleteData()
	{
		BodyTrackingID = 0;
		IsTracked = 0;
		IsRestricted = 0;
		IsEngaged = DetectionResult_Unknown;
		ClippedEdge = ClippedEdges::FrameEdge_None;
		std::fill(JointData, JointData + JointType_Count, Joint());
		std::fill(JointOrientations, JointOrientations + JointType_Count, JointOrientation());
		LeanAmount = PointF();
		LeanTrackState = TrackingState::TrackingState_NotTracked;
		LeftHandState = RightHandState = HandState_NotTracked;
		LeftHandConfidence = RightHandConfidence = TrackingConfidence_Low;
	}

	inline bool GetTrackedPosition(const JointType jointType, CameraSpacePoint& outVal)const
	{
		if(JointData[jointType].TrackingState == TrackingState::TrackingState_Tracked)
		{
			outVal = JointData[jointType].Position;
			return true;
		}

		return false;
	}

	inline bool GetTrackedOrInferredPosition(const JointType jointType, CameraSpacePoint& outVal)const
	{
		if(JointData[jointType].TrackingState != TrackingState_NotTracked)
		{
			outVal = JointData[jointType].Position;
			return true;
		}

		return false;
	}
};

enum Hand
{
	Left, Right
};

class KinectBody
{
	friend class KinectBodyReader;

private:
	KinectBodyReader* bodyReader;
	KinectGestureReader* gestureReader;

	CompleteData previousData;
	
	//Index of the body in the array
	UINT8 bodyArrayIndex;
	//Index of the body as provided by the Kinect
	UINT64 bodyKinectTrackingIndex;

	//Receives the body information from the Body Reader and parses it
	void RecieveBodyInformation(IBody* body);

	void SetBodyTrackingID(const UINT64 bodyID);

public:
	KinectBody(KinectBodyReader* reader);
	~KinectBody(void);

	///Attach a gesture reader to be process by the body
	///<param "gestureReader">The gesture Reader to attach</param>
	///Returns the gesture reader that was attached to the body, or NULL
	KinectGestureReader* AttachGestureReader(KinectGestureReader* gestureReader);

	///Detaches the gesture reader and returns the attached gesture reader, or NULL if none
	KinectGestureReader* DetachGestureReader();

	//Gets the pointer to the Body Reader that contains this body
	inline KinectBodyReader* const KinectBody::GetBodyReader()const
	{
		return bodyReader;
	}

	//Gets the pointer to the Gesture Reader associated with this body
	inline KinectGestureReader* const KinectBody::GetGestureReader()const
	{
		return gestureReader;
	}

	///Get the ID of the body
	inline UINT8 KinectBody::GetBodyID()const
	{
		return bodyArrayIndex;
	}

	///Get the Tracking ID of the body
	inline UINT64 KinectBody::GetBodyTrackingID()const
	{
		return bodyKinectTrackingIndex;
	}

	//Check if the tracking id is valid
	inline bool KinectBody::GetTrackingIDIsValid()const
	{
		return bodyKinectTrackingIndex != 0;
	}

};

