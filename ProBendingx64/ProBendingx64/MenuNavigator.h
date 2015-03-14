#pragma once
#include "KinectBodyListener.h"
#include "KinectAudioListener.h"
#include "InputObserver.h"

class MenusScene;

class MenuNavigator :
	public KinectBodyListener, public KinectAudioListener, public InputObserver
{
private:
	MenusScene* menuScene;

	virtual DepthSpacePoint PointToBodyRectangle(const DepthSpacePoint screenSize, const Joint& jointToConvert,
		const CompleteData& currentData, const float zAxisTolerance = 0.45f)
	{
		//Create a bounding box out of the players joints to represent the screen
		CameraSpacePoint leftShoulder = currentData.JointData[JointType_ShoulderLeft].Position;
		CameraSpacePoint rightShoulder = currentData.JointData[JointType_ShoulderRight].Position;
		CameraSpacePoint spineBase = currentData.JointData[JointType_SpineBase].Position;
		CameraSpacePoint head = currentData.JointData[JointType_Head].Position;

		CameraSpacePoint hand = jointToConvert.Position;

		DepthSpacePoint cursorPoint;
		cursorPoint.X = cursorPoint.Y = -1.0f;
		
		float zCompare = 0.0f;

		float yDistToShoulder = hand.Y - rightShoulder.Y;
		float yDistToSpineBase = hand.Y - spineBase.Y;

		float spineZModifier = 1.75f; //used to reduce the z requirement when the hand is near the waist

		if(yDistToShoulder < 0)
			yDistToShoulder *= -1.0f;
		if(yDistToSpineBase < 0)
			yDistToSpineBase *= -1.0f;

		if(yDistToShoulder < yDistToSpineBase)
			zCompare = rightShoulder.Z - hand.Z;
		else
			zCompare = (spineBase.Z - hand.Z) * spineZModifier;

		if(zCompare < 0)
			zCompare *= -1.0f;

		//Make sure the player has suggested they want to select something
		if(zCompare >= zAxisTolerance)
		{
			//Use the shoulders and spine base to create a rectangle that will represent the reachable screen area
			//We multiply by two in the first line because we are using twice the distance between the shoulders as 
			//the width of the screen
			cursorPoint.X = ((hand.X - leftShoulder.X) / (2.0f * (rightShoulder.X - leftShoulder.X))) * screenSize.X;
			cursorPoint.Y = ((head.Y - hand.Y) / (head.Y - spineBase.Y)) * screenSize.Y;

			return cursorPoint;
		}

		return cursorPoint;
	}

public:
	bool AllowMenuControls;

	MenuNavigator(MenusScene* menuScene = NULL);
	~MenuNavigator(void);

	virtual void Update(float gameTime);

	virtual void BodyLost(const CompleteData& currentData, const CompleteData& previousData);
	
	virtual void BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData);

	virtual void mouseMoved(const OIS::MouseEvent &arg);

	virtual void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	virtual void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	virtual void AudioDataReceived(AudioData* audioData);

};

