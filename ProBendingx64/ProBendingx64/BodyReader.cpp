#include "BodyReader.h"
#include "BodyListener.h"
#include <array>

std::array<std::vector<BodyListener*>, BODY_COUNT> listeners;
std::array<CompleteData, BODY_COUNT> previousBodyData;

BodyReader::BodyReader(void)
{
}

BodyReader::~BodyReader(void)
{
	//Loop through the array of vectors
	for (unsigned int i = 0; i < listeners.size(); i++)
	{
		for (unsigned int x = 0; x < listeners[i].size(); x++)
		{
			//Delete each item in the vector (BodyListener*)
			if(listeners[i][x] != nullptr)
			{
				delete listeners[i][x];
				listeners[i][x] = nullptr;
			}
		}
	}
}

bool BodyReader::FrameArrived(const KinectReader* kinectReader, IBodyFrame* frame)
{
	//Use to get time between frames
    /* INT64 nTime = 0;
    hr = pBodyFrame->get_RelativeTime(&nTime);*/

	//Initialize and acquire body array
    IBody* ppBodies[BODY_COUNT] = {0};
    HRESULT hr = frame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		
	bool returnVal = SUCCEEDED(hr);
	//If no errors, process the bodies
	if (returnVal)
    {
        ProcessBodies(kinectReader, BODY_COUNT, ppBodies);
    }

	//When finished, dispose of the bodies (hehehehe)
    for (int i = 0; i < _countof(ppBodies); ++i)
    {
		if(ppBodies[i] != NULL)
		{
			ppBodies[i]->Release();
			ppBodies[i] = NULL;
		}
	}

	return returnVal;
}

void BodyReader::ProcessBodies(const KinectReader* kinectReader, const int numBodies, IBody** bodies)
{
	HRESULT hr;

	for (int i = 0; i < numBodies; i++)
	{
		IBody* pBody = bodies[i];

        if (pBody)
        {
			CompleteData currBodyData = CompleteData();

			hr = pBody->get_TrackingId(&currBodyData.BodyTrackingID);

			//If we have a Tracking ID of 0, we lost the skeleton
			if(FAILED(hr) || currBodyData.BodyTrackingID == 0)
			{
				//Notify any listeners watching this skeleton
				for (unsigned int x= 0; x < listeners[i].size(); x++)
				{
					listeners[i][x]->BodyLost(*this, previousBodyData[i]);
				}
			}

			//If we reach here, process the information of the skeleton
			hr = pBody->get_IsTracked(&currBodyData.IsTracked);			
			if(FAILED(hr))
				currBodyData.IsTracked = false;

			hr = pBody->get_Engaged(&currBodyData.IsEngaged);
			if(FAILED(hr))
				currBodyData.IsEngaged = DetectionResult_Unknown;

			hr = pBody->get_IsRestricted(&currBodyData.IsRestricted);
			if(FAILED(hr))
				currBodyData.IsRestricted = FALSE;

			DWORD clippedEdges = 0;
			hr = pBody->get_ClippedEdges(&clippedEdges);
			if(FAILED(hr))
				currBodyData.ClippedEdge = FrameEdges::FrameEdge_None;
			else
				currBodyData.ClippedEdge = (FrameEdges)clippedEdges;

			hr = pBody->GetJoints(JointType_Count, currBodyData.JointData);
			if(FAILED(hr))
				std::fill(currBodyData.JointData, currBodyData.JointData + JointType_Count, Joint());

			hr = pBody->GetJointOrientations(JointType_Count, currBodyData.JointOrientations);
			if(FAILED(hr))
				std::fill(currBodyData.JointOrientations, currBodyData.JointOrientations + JointType_Count, JointOrientation());

			hr = pBody->get_Lean(&currBodyData.LeanAmount);
			if(FAILED(hr))
				currBodyData.LeanAmount = PointF();

			hr = pBody->get_LeanTrackingState(&currBodyData.LeanTrackState);
			if(FAILED(hr))
				currBodyData.LeanTrackState = TrackingState_NotTracked;

			hr = pBody->get_HandLeftState(&currBodyData.LeftHandState);
			if(FAILED(hr))
				currBodyData.LeftHandState = HandState_NotTracked;

			hr = pBody->get_HandRightState(&currBodyData.RightHandState);
			if(FAILED(hr))
				currBodyData.RightHandState = HandState_NotTracked;

			hr = pBody->get_HandLeftConfidence(&currBodyData.LeftHandConfidence);
			if(FAILED(hr))
				currBodyData.LeftHandConfidence = TrackingConfidence_Low;

			hr = pBody->get_HandRightConfidence(&currBodyData.RightHandConfidence);
			if(FAILED(hr))
				currBodyData.RightHandConfidence = TrackingConfidence_Low;

			//Analyze the Frame Information to fire necessary events
			AnalyzeFrameInformation(kinectReader, //The reader used for coordinate mapping
				currBodyData, //Body data generated this frame
				previousBodyData[i],//Body data found from last frame
				listeners[i]); //The listeners related to this body ID
				
			previousBodyData[i] = currBodyData;
		}
	}
}

void BodyReader::AnalyzeFrameInformation(const KinectReader* kinectReader, const CompleteData& currentData, 
										 const CompleteData& previousData, std::vector<BodyListener*>& listenersOfData)
{
	char changedData = ChangedData::None;

	//Check what data has been changed to fire an appropriate event for it
	if(currentData.IsTracked != previousData.IsTracked)
		changedData |= ChangedData::TrackingChanged;

	if(currentData.ClippedEdge != previousData.ClippedEdge)
		changedData |= ChangedData::BodyClippedChanged;
	
	if(currentData.IsEngaged != previousData.IsEngaged)
		changedData |= ChangedData::EngagedChanged;

	if(currentData.IsRestricted != previousData.IsRestricted)
		changedData |= ChangedData::RestrictedChanged;

	if(currentData.LeftHandConfidence != previousData.LeftHandConfidence)
		changedData |= ChangedData::LeftHandConfidenceStateChanged;

	if(currentData.RightHandConfidence != previousData.RightHandConfidence)
		changedData |= ChangedData::RightHandConfidenceStateChanged;

	if(currentData.LeftHandState != previousData.LeftHandState)
		changedData |= ChangedData::LeftHandTrackingStateChanged;

	if(currentData.RightHandState != previousData.RightHandState)
		changedData |= ChangedData::RightHandTrackingStateChanged;

	if(currentData.LeanTrackState != previousData.LeanTrackState)
		changedData |= ChangedData::LeanTrackingStateChanged;

	//Check for a Lean Change event
	PointF difference = PointF();
	difference.X = currentData.LeanAmount.X - previousData.LeanAmount.X;
	difference.Y = currentData.LeanAmount.Y - previousData.LeanAmount.Y;
	float magnitude = (difference.X * difference.X) + (difference.Y * difference.Y);
	if(magnitude >= LeanHeuristic)
		changedData |= ChangedData::LeanChanged;

	//Notify each listener of the events that occured
	for (unsigned int i = 0; i < listenersOfData.size(); i++)
	{
		if(changedData & ChangedData::BodyClippedChanged)
			listenersOfData[i]->BodyClipChanged(kinectReader, currentData, previousData);
		if(changedData & ChangedData::EngagedChanged)
			listenersOfData[i]->BodyEngagedChanged(kinectReader, currentData, previousData);
		if(changedData & ChangedData::RestrictedChanged)
			listenersOfData[i]->BodyRestrictedChanged(kinectReader, currentData, previousData);
		if(changedData & ChangedData::TrackingChanged)
			listenersOfData[i]->BodyTrackChanged(kinectReader, currentData, previousData);

		if(changedData & ChangedData::LeanChanged)
			listenersOfData[i]->LeanChanged(kinectReader, currentData, previousData);
		if(changedData & ChangedData::LeanTrackingStateChanged)
			listenersOfData[i]->LeanTrackingStateChanged(kinectReader, currentData, previousData);

		if(changedData & ChangedData::LeftHandTrackingStateChanged)
			listenersOfData[i]->HandTrackingStateChanged(kinectReader, Hand::Left,currentData, previousData);
		if(changedData & ChangedData::RightHandTrackingStateChanged)
			listenersOfData[i]->HandTrackingStateChanged(kinectReader, Hand::Right, currentData, previousData);
		if(changedData & ChangedData::LeftHandConfidenceStateChanged)
			listenersOfData[i]->HandConfidenceChanged(kinectReader, Hand::Left, currentData, previousData);
		if(changedData & ChangedData::RightHandConfidenceStateChanged)
			listenersOfData[i]->HandConfidenceChanged(kinectReader, Hand::Right, currentData, previousData);

		listenersOfData[i]->BodyFrameAcquired(kinectReader, currentData, previousData);
	}
}

//Returns all BodyID's that do not have a listener registered to them
//Returns a vector containing the unlistened to Body ID's
const std::vector<UINT64> BodyReader::GetUnlistenedBodyIDs()const
{
	std::vector<UINT64> keys = std::vector<UINT64>();
	
	for (unsigned int i = 0; i < listeners.size(); i++)
	{
		//If we don't have a null skeleton
		if(previousBodyData[i].BodyTrackingID != 0)
			//And we have no listeners
			if(listeners[i].size() == 0)
				keys.push_back(i);
	}
	
	return keys;
}

void BodyReader::RegisterBodyListener(UINT64 bodyID, BodyListener* listener)const
{
	//If the skeleton ID is valid
	if(bodyID < listeners.size())
	{
		//loop through the listeners
		for (unsigned int i = 0; i < listeners[bodyID].size(); i++)
		{
			//And avoid any duplicates
			if(listeners[bodyID][i] == listener)
				return;
		}

		listeners[bodyID].push_back(listener);
	}
}

BodyListener* BodyReader::UnregisterBodyListener(UINT64 bodyID, BodyListener* listener)const
{
	BodyListener* retVal;

	//Check for valid Skeleton ID
	if(bodyID < listeners.size())
	{
		//Loop through the vector
		for(std::vector<BodyListener*>::iterator start = listeners[bodyID].begin();
			start != listeners[bodyID].end(); ++start)
		{
			//If we find a match
			if(*start == listener)
			{
				//Set the return value to allow user to delete the listener if need be
				retVal = *start;
				//Remove from vector and return
				listeners[bodyID].erase(start);
				return retVal;
			}
		}
	}

	//Return null if we reach here
	return nullptr;
}

BodyListener* BodyReader::UnregisterBodyListener(BodyListener* listener)const
{
	BodyListener* retVal;

	//Loop through the skeleton id's
	for (int i = 0; i < listeners.size(); i++)
	{
		//Loop through the vector
		for(std::vector<BodyListener*>::iterator start = listeners[i].begin();
			start != listeners[i].end(); ++start)
		{
			//If we find a match
			if(*start == listener)
			{
				//Set the return value to allow user to delete the listener if need be
				retVal = *start;
				//Remove from vector and return
				listeners[i].erase(start);
				return retVal;
			}
		}
	}

	//If we reach here, return null
	return nullptr;
}