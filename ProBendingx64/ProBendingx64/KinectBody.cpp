#include "KinectBody.h"
#include "KinectBodyEventNotifier.h"
#include "KinectGestureReader.h"

KinectBody::KinectBody(KinectBodyReader* reader)
{
	bodyReader = reader;
	gestureReader = NULL;

	bodyArrayIndex = 0;
	bodyKinectTrackingIndex = 0;
	
}

KinectBody::~KinectBody(void)
{
}

KinectGestureReader* KinectBody::AttachGestureReader(KinectGestureReader* reader)
{
	if(gestureReader != reader)
	{
		KinectGestureReader* old = gestureReader;

		gestureReader = reader;

		//Connect the gesture reader to this body and tell it the tracking ID to follow
		gestureReader->SetBody(this);
		gestureReader->SetBodyID(bodyKinectTrackingIndex);

		return old;
	}
	return NULL;
}

KinectGestureReader* KinectBody::DetachGestureReader()
{
	KinectGestureReader* old = gestureReader;

	if(gestureReader)
	{
		gestureReader = NULL;
	}
	return old;
}

void KinectBody::RecieveBodyInformation(IBody* body)
{
	CompleteData currBodyData = CompleteData();

	HRESULT hr = body->get_TrackingId(&currBodyData.BodyTrackingID);

	if(FAILED(hr))
	{
		currBodyData.BodyTrackingID = 0;
	}

	//If we have a tracking ID of 0, there is no skeleton
	if(currBodyData.BodyTrackingID != 0)
	{
		//If we reach here, process the information of the skeleton
		hr = body->get_IsTracked(&currBodyData.IsTracked);			
		if(FAILED(hr))
			currBodyData.IsTracked = false;

		hr = body->get_Engaged(&currBodyData.IsEngaged);
		if(FAILED(hr))
			currBodyData.IsEngaged = DetectionResult_Unknown;

		hr = body->get_IsRestricted(&currBodyData.IsRestricted);
		if(FAILED(hr))
			currBodyData.IsRestricted = FALSE;

		DWORD clippedEdges = 0;
		hr = body->get_ClippedEdges(&clippedEdges);
		if(FAILED(hr))
			currBodyData.ClippedEdge = FrameEdges::FrameEdge_None;
		else
			currBodyData.ClippedEdge = (FrameEdges)clippedEdges;

		hr = body->GetJoints(JointType_Count, currBodyData.JointData);
		if(FAILED(hr))
			std::fill(currBodyData.JointData, currBodyData.JointData + JointType_Count, Joint());

		hr = body->GetJointOrientations(JointType_Count, currBodyData.JointOrientations);
		if(FAILED(hr))
			std::fill(currBodyData.JointOrientations, currBodyData.JointOrientations + JointType_Count, JointOrientation());

		hr = body->get_Lean(&currBodyData.LeanAmount);
		if(FAILED(hr))
			currBodyData.LeanAmount = PointF();

		hr = body->get_LeanTrackingState(&currBodyData.LeanTrackState);
		if(FAILED(hr))
			currBodyData.LeanTrackState = TrackingState_NotTracked;

		hr = body->get_HandLeftState(&currBodyData.LeftHandState);
		if(FAILED(hr))
			currBodyData.LeftHandState = HandState_NotTracked;

		hr = body->get_HandRightState(&currBodyData.RightHandState);
		if(FAILED(hr))
			currBodyData.RightHandState = HandState_NotTracked;

		hr = body->get_HandLeftConfidence(&currBodyData.LeftHandConfidence);
		if(FAILED(hr))
			currBodyData.LeftHandConfidence = TrackingConfidence_Low;

		hr = body->get_HandRightConfidence(&currBodyData.RightHandConfidence);
		if(FAILED(hr))
			currBodyData.RightHandConfidence = TrackingConfidence_Low;

	}

	//Pass the data to the KinectBodyEventNotifier
	KinectBodyEventNotifier::GetInstance()->InjectBodyFrameData(this, &previousData, &currBodyData);
	
	//If the tracking ID has changed, fill in the data for the body
	if(currBodyData.BodyTrackingID != previousData.BodyTrackingID)
	{
		SetBodyTrackingID(currBodyData.BodyTrackingID);	
	}

	previousData = currBodyData;
		
	if(gestureReader)
		//Capture gesture reader data
		gestureReader->Capture();
}

inline void KinectBody::SetBodyTrackingID(const UINT64 bodyID)
{
	bodyKinectTrackingIndex = bodyID;

	if(gestureReader)
		gestureReader->SetBodyID(bodyID);
}