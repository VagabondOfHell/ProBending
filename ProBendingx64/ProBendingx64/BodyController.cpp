#include "BodyController.h"
#include <iostream>
#include "KinectBodyEventNotifier.h"
#include "KinectBodyReader.h"

#include <CEGUI\CEGUI.h>
#include <CEGUI\RendererModules\Ogre\Renderer.h>

BodyController::BodyController(void)
{
}


BodyController::~BodyController(void)
{
}

void BodyController::BodyLost( const CompleteData& currentData, const CompleteData& previousData)
{
	std::cout << "Body ID Changed Event Fired. \nOld ID: "<< previousData.BodyTrackingID <<
		"    New ID: "<< currentData.BodyTrackingID << std::endl;

	KinectBodyReader* bodyReader = GetBody()->GetBodyReader();

	KinectBody* body = GetBody();
	body->DetachGestureReader();

	KinectBodyEventNotifier::GetInstance()->FlagListenerForRemoval(GetBody(), this);
}

void BodyController::BodyTrackChanged(const CompleteData& currentData, const CompleteData& previousData)
{
	std::cout << "Body Track Changed Event Fired"<<std::endl;
}

void BodyController::BodyEngagedChanged(const CompleteData& currentData, const CompleteData& previousData)
{
	std::cout << "Body Engaged Changed Event Fired"<<std::endl;
}

void BodyController::BodyRestrictedChanged(const CompleteData& currentData, const CompleteData& previousData)
{
	std::cout << "Body Restricted Changed Event Fired"<<std::endl;
}

void BodyController::BodyClipChanged(const CompleteData& currentData, const CompleteData& previousData)
{
	std::cout << "Body Clip Changed Event Fired "<< currentData.ClippedEdge << std::endl;
}

void BodyController::LeanTrackingStateChanged(const CompleteData& currentData, const CompleteData& previousData)
{
}
	
void BodyController::HandTrackingStateChanged(const Hand hand, 
											  const CompleteData& currentData, const CompleteData& previousData)
{
	
}

void BodyController::BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData)
{
	if(currentData.JointData[JointType_HandRight].TrackingState == TrackingState_Tracked)
	{
		CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
		
		DepthSpacePoint screen = GetBody()->GetBodyReader()->GetKinectReader()->GetWindowSize();
	
		//Get the cursor from within the body's comfortable range
		DepthSpacePoint cursorPoint = PointToBodyRectangle(screen, currentData.JointData[JointType_HandRight], currentData);

		//If the point from above is valid, inject the position
		if(cursorPoint.X != -1.0f)
			context.injectMousePosition(cursorPoint.X, cursorPoint.Y);	
	}
}

void BodyController::DiscreteGesturesAcquired(const std::vector<KinectGestureResult>discreteGestureResults)
{
	if(discreteGestureResults[0].discreteFirstFrameDetected)
		std::cout<< "Hands Over Head Performed" <<std::endl;
}

void BodyController::ContinuousGesturesAcquired(const std::vector<KinectGestureResult>continuousGestureResults)
{
	std::cout << "Continuous Gesture Acquired" << std::endl;
}

void BodyController::SensorDisconnected()
{
	std::cout << "SENSOR LOST!" << std::endl;
}