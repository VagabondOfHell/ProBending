#include "KinectController.h"
#include "BodyReader.h"
#include "KinectReader.h"

#include <CEGUI\CEGUI.h>
#include <CEGUI\RendererModules\Ogre\Renderer.h>

KinectController::KinectController(void)
{
	isListening = false;
}


KinectController::~KinectController(void)
{
}

bool KinectController::IsListening()const
{
	return isListening;
}

void KinectController::ListenToNewBody(const BodyReader& reader)
{
	std::vector<UINT64> bodyIDs = reader.GetUnlistenedBodyIDs();

	if(bodyIDs.size() > 0)
	{
		reader.RegisterBodyListener(bodyIDs[0], this);
		bodyID = bodyIDs[0];

		isListening = true;
		printf("REGISTERED at %i\n", bodyIDs[0]);
	}
}

void KinectController::BodyLost(const BodyReader& reader, const CompleteData& previousData)
{
	reader.UnregisterBodyListener(bodyID, this);
	isListening = false;
	ListenToNewBody(reader);
}

//Event Overriding
void KinectController::BodyTrackChanged(const KinectReader* kinectReader, 
					const CompleteData& currentData, const CompleteData& previousData)
{
	printf("Body Track Changed: %i\n", currentData.IsTracked);
}

void KinectController::BodyEngagedChanged(const KinectReader* kinectReader, 
					const CompleteData& currentData, const CompleteData& previousData)
{
	printf("Body Engaged Changed: %i\n", currentData.IsEngaged);
}

void KinectController::BodyRestrictedChanged(const KinectReader* kinectReader, 
					const CompleteData& currentData, const CompleteData& previousData)
{

}

void KinectController::BodyClipChanged(const KinectReader* kinectReader, 
					const CompleteData& currentData, const CompleteData& previousData)
{

}
	
void KinectController::LeanChanged(const KinectReader* kinectReader, 
					const CompleteData& currentData, const CompleteData& previousData)
{

}

void KinectController::LeanTrackingStateChanged(const KinectReader* kinectReader, 
					const CompleteData& currentData, const CompleteData& previousData)
{

}
	
void KinectController::HandTrackingStateChanged(const KinectReader* kinectReader, 
					const Hand hand, const CompleteData& currentData, const CompleteData& previousData)
{
	if(previousData.RightHandState != currentData.RightHandState)
	{
		switch (currentData.RightHandState)
		{
		case HandState_Open:
			printf("Hand is open\n");
			break;

		case HandState_Closed:
			printf("Hand is closed\n");
			break;

		case HandState_Lasso:
			printf("Hand is lasso\n");
			break;

		case HandState_Unknown:
			printf("Hand is unknown\n");
			break;

		case HandState_NotTracked:
			printf("Hand is no longer tracked\n");
			break;

		default:
			break;
		}
	}
}

void KinectController::HandConfidenceChanged(const KinectReader* kinectReader, 
					const Hand hand, const CompleteData& currentData, const CompleteData& previousData)
{

}

void KinectController::BodyJointDataReceived(const KinectReader* kinectReader, 
					const CompleteData& currentData, const CompleteData& previousData)
{

}

void KinectController::BodyFrameAcquired(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData)
{
	if(currentData.JointData[JointType_HandRight].TrackingState == TrackingState_Tracked)
	{
		CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();

		DepthSpacePoint screen = kinectReader->GetWindowSize();
	
		//Get the cursor from within the body's comfortable range
		DepthSpacePoint cursorPoint = PointToBodyRectangle(screen, currentData.JointData[JointType_HandRight], currentData);

		//If the point from above is valid, inject the position
		if(cursorPoint.X != -1.0f)
			context.injectMousePosition(cursorPoint.X, cursorPoint.Y);	
	}
}