#pragma once
#include "KinectBodyListener.h"
#include "KinectReader.h"

class BodyController : public KinectBodyListener, public KinectSensorListener
{
public:
	BodyController(void);
	virtual ~BodyController(void);
protected:
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

		return DepthSpacePoint();
	}

	virtual void BodyLost( const CompleteData& currentData, const CompleteData& previousData);

	virtual void BodyTrackChanged(const CompleteData& currentData, const CompleteData& previousData);
	virtual void BodyEngagedChanged(const CompleteData& currentData, const CompleteData& previousData);
	virtual void BodyRestrictedChanged(const CompleteData& currentData, const CompleteData& previousData);

	virtual void BodyClipChanged(const CompleteData& currentData, const CompleteData& previousData);
	
	virtual void LeanTrackingStateChanged(const CompleteData& currentData, const CompleteData& previousData);
	
	virtual void HandTrackingStateChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData);

	virtual void BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData);

	virtual void DiscreteGesturesAcquired(const std::vector<KinectGestureResult>discreteGestureResults);
	virtual void ContinuousGesturesAcquired(const std::vector<KinectGestureResult>continuousGestureResults);

	virtual void SensorDisconnected();
};

