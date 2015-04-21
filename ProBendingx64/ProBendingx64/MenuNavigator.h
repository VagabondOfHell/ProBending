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

	virtual DepthSpacePoint PointToBodyRectangle(const DepthSpacePoint screenSize, bool rightHand,
		const CompleteData& currentData, const float zAxisTolerance = 0.15f)
	{
		DepthSpacePoint cursorPoint;
		cursorPoint.X = cursorPoint.Y = -1.0f;
		Joint hand = Joint();

		if(rightHand)
			hand = currentData.JointData[JointType_HandRight];
		else
			hand = currentData.JointData[JointType_HandLeft];
		
		if(hand.TrackingState == TrackingState::TrackingState_NotTracked)
			return cursorPoint;

		CameraSpacePoint spineBase = currentData.JointData[JointType_SpineBase].Position;
		CameraSpacePoint head = currentData.JointData[JointType_Head].Position;

		float diff = 0.0f;
		if(rightHand)
			diff = (hand.Position.X - head.X) * 2.0f * screenSize.X ;
		else
			diff = ((hand.Position.X - head.X) * 2.0f * screenSize.X) + screenSize.X ;

		cursorPoint.X = diff;
		cursorPoint.Y = (head.Y - hand.Position.Y) * 2.0f * screenSize.Y;

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

