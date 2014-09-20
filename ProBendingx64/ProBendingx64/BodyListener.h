#pragma once
#include <kinect.h>

class KinectReader;

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
};

enum Hand
{
	Left, Right
};

class BodyListener
{
	friend class BodyReader;

protected:
	UINT64 bodyID;

public:

	BodyListener(void);
	virtual ~BodyListener(void);

	const UINT64 GetBodyID()
	{
		return bodyID;
	}
	
	//Converts a 3D depth point into a 2D point within the players reach using their
	//Shoulders to determine width and SpineMid and Head to determine height
	//to create a dynamic dimension that moves with them
	//<param: screenSize> The size of the render window
	//<param: jointToConvert> The joint to convert to Body Screen space
	//<param: currentData> The skeleton data from the current Kinect frame
	//<param: upperBodyZTolerance> Distance the joint must be from the shoulders on the Z-Plane in the upper
	//body area to register a movement
	//<param: lowerBodyZTolerance> Distance the joint must be from the shoulders on the Z-Plane in the lower
	//body area to register a movement
	virtual DepthSpacePoint PointToBodyRectangle(const DepthSpacePoint screenSize, const Joint& jointToConvert,
		const CompleteData& currentData, const float zAxisTolerance = 0.45f)
	{
		//Create a bounding box out of the players joints to represent the screen
		CameraSpacePoint leftShoulder = currentData.JointData[JointType_ShoulderLeft].Position;
		CameraSpacePoint rightShoulder = currentData.JointData[JointType_ShoulderRight].Position;
		CameraSpacePoint spineMid = currentData.JointData[JointType_SpineMid].Position;
		CameraSpacePoint head = currentData.JointData[JointType_Head].Position;

		CameraSpacePoint hand = jointToConvert.Position;
		
		DepthSpacePoint cursorPoint;
		
		//Make sure the player has suggested they want to select something
		if(rightShoulder.Z - hand.Z > zAxisTolerance)
		{
			//Use the shoulders and spine base to create a rectangle that will represent the reachable screen area
			//We multiply by two in the first line because we are using twice the distance between the shoulders as 
			//the width of the screen
			cursorPoint.X = ((hand.X - leftShoulder.X) / (2 * (rightShoulder.X - leftShoulder.X))) * screenSize.X;
			cursorPoint.Y = ((hand.Y - head.Y) / (spineMid.Y - head.Y)) * screenSize.Y;

			return cursorPoint;
		}

		//If we hit here, invalidate our results
		cursorPoint.X = cursorPoint.Y = -1;

		return cursorPoint;
	}

	//Event Overriding

	///Use this method to decide what body to listen to
	virtual void ListenToNewBody(const BodyReader& reader){}
	virtual void BodyLost(const BodyReader& reader, const CompleteData& previousData){}
	
	virtual void BodyTrackChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData){}
	virtual void BodyEngagedChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData){}
	virtual void BodyRestrictedChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData){}

	virtual void BodyClipChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData){}
	
	virtual void LeanChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData){}
	virtual void LeanTrackingStateChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData){}
	
	virtual void HandTrackingStateChanged(const KinectReader* kinectReader, const Hand hand, const CompleteData& currentData, const CompleteData& previousData){}
	virtual void HandConfidenceChanged(const KinectReader* kinectReader, const Hand hand, const CompleteData& currentData, const CompleteData& previousData){}

	virtual void BodyJointDataReceived(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData){}

	virtual void BodyFrameAcquired(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData){}
};

