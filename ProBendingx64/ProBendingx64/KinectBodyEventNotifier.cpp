#include "KinectBodyEventNotifier.h"
#include <vector>
#include <concurrent_queue.h>

struct BodyFrameData
{
	KinectBody* Body;
	CompleteData CurrentData;
	CompleteData PreviousData;

	BodyFrameData()
	{
		Body = NULL;
	}

	BodyFrameData(KinectBody* const body, CompleteData currentData, CompleteData previousData)
	{
		Body = body;
		CurrentData = currentData;
		PreviousData = previousData;
	}

	~BodyFrameData()
	{
	}
};

struct GestureFrameData
{
	KinectBody* Body;
	std::vector<KinectGestureResult> ContinuousGestureResult;
	std::vector<KinectGestureResult> DiscreteGestureResult;

	GestureFrameData(){}

	GestureFrameData(KinectBody* const body, std::vector<KinectGestureResult> continuousGestureResult, 
		std::vector<KinectGestureResult>discreteGestureResult)
	{
		Body = body;
		ContinuousGestureResult = continuousGestureResult;
		DiscreteGestureResult = discreteGestureResult;
	}

	~GestureFrameData(){}
};

//Array of body listeners
std::array<std::vector<KinectBodyListener*>, BODY_COUNT> bodyListeners;

std::array<std::vector<KinectBodyListener*>, BODY_COUNT> listenersToRemove;

concurrency::concurrent_queue<BodyFrameData> bodyDataToProcess;
concurrency::concurrent_queue<GestureFrameData> gestureDataToProcess;

KinectBodyEventNotifier* KinectBodyEventNotifier::instance;

KinectBodyEventNotifier::KinectBodyEventNotifier(void)
{
}

KinectBodyEventNotifier::~KinectBodyEventNotifier(void)
{
}

bool KinectBodyEventNotifier::CheckBodyIndexHasListener(UINT8 bodyIndex)const
{
	if(bodyIndex < bodyListeners.size())
		return (bodyListeners[bodyIndex].size() > 0);

	return false;
}

int KinectBodyEventNotifier::GetBodyIndexListenerCount(UINT8 bodyIndex)const
{
	if(bodyIndex < bodyListeners.size())
		return (int)bodyListeners[bodyIndex].size();

	//If we hit here, something went wrong
	return -1;
}

int KinectBodyEventNotifier::GetFirstUnlistenedBodyIndex()const
{
	for (int i = 0; i < bodyListeners.size(); i++)
	{
		if(bodyListeners[i].size() == 0)
			return i;
	}

	return -1;
}

std::vector<UINT8> KinectBodyEventNotifier::GetAllUnlistenedBodyIndices()const
{
	std::vector<UINT8> unlistenedIDs = std::vector<UINT8>();

	for (int i = 0; i < bodyListeners.size(); i++)
	{
		if(bodyListeners[i].size() == 0)
			unlistenedIDs.push_back(i);
	}

	return unlistenedIDs;
}

bool KinectBodyEventNotifier::RegisterListener(KinectBody* _body, KinectBodyListener* listener)const
{
	UINT8 bodyIndex = _body->GetBodyID();

	bool found = false;

	for (int i = 0; i < bodyListeners[bodyIndex].size(); i++)
	{
		if(bodyListeners[bodyIndex][i] == listener)
		{
			found = true;
			break;
		}
	}

	if(!found)
	{
		bodyListeners[bodyIndex].push_back(listener);

		//Add the index to the listener
		listener->AddNewConnection(_body);
		return true;
	}
	

	return false;
}

bool KinectBodyEventNotifier::UnregisterListener(KinectBody* const _body, KinectBodyListener* listener)const
{
	UINT8 bodyIndex = _body->GetBodyID();

	if(bodyIndex < bodyListeners.size())
	{
		std::vector<KinectBodyListener*>::iterator result = 
			std::find(bodyListeners[bodyIndex].begin(), bodyListeners[bodyIndex].end(), listener);

		if(result != bodyListeners[bodyIndex].end())
		{
			bodyListeners[bodyIndex].erase(result);

			//Remove this index from the listener
			listener->RemoveConnection();

			return true;
		}
	}

	return false;
}

void KinectBodyEventNotifier::FlagListenerForRemoval(KinectBody* const _body, KinectBodyListener* listener)
{
	if(_body)
	{
		UINT8 bodyIndex = _body->GetBodyID();

		if(bodyIndex < bodyListeners.size())
		{
			listenersToRemove[bodyIndex].push_back(listener);
		}
	}
}

void KinectBodyEventNotifier::InjectBodyFrameData(KinectBody* const body, 
					CompleteData* previousData, CompleteData* currentData)
{
	bodyDataToProcess.push(BodyFrameData(body, *currentData, *previousData));
}

/// <summary>
///Injects data from the current Gesture Frame
/// </summary>
/// <param name="body">The Body the events will pertain to</param>	
/// <param name="discreteResults">The data for Discrete Gestures</param>	
/// <param name="continuousResults">The data for Continuous Gestures</param>	
void KinectBodyEventNotifier::InjectGestureFrameData(KinectBody* const body, std::vector<KinectGestureResult>* discreteResults, 
std::vector<KinectGestureResult>* continuousResults)const
{
	gestureDataToProcess.push(GestureFrameData(body, *continuousResults, *discreteResults));
}

void KinectBodyEventNotifier::ProcessEvents()
{
	bool processing = true;

	while (processing)
	{
		BodyFrameData frameData;

		processing = bodyDataToProcess.try_pop(frameData);

		if(processing)
		{
			char changedData = ChangedData::None;

			if(frameData.CurrentData.BodyTrackingID != frameData.PreviousData.BodyTrackingID)
				changedData |= ChangedData::TrackingIDLost;

			//Check what data has been changed to fire an appropriate event for it
			if(frameData.CurrentData.IsTracked != frameData.PreviousData.IsTracked)
				changedData |= ChangedData::TrackingChanged;

			if(frameData.CurrentData.ClippedEdge != frameData.PreviousData.ClippedEdge)
				changedData |= ChangedData::BodyClippedChanged;
	
			if(frameData.CurrentData.IsEngaged != frameData.PreviousData.IsEngaged)
				changedData |= ChangedData::EngagedChanged;

			if(frameData.CurrentData.IsRestricted != frameData.PreviousData.IsRestricted)
				changedData |= ChangedData::RestrictedChanged;

			if(frameData.CurrentData.LeftHandState != frameData.PreviousData.LeftHandState)
				changedData |= ChangedData::LeftHandTrackingStateChanged;

			if(frameData.CurrentData.RightHandState != frameData.PreviousData.RightHandState)
				changedData |= ChangedData::RightHandTrackingStateChanged;

			if(frameData.CurrentData.LeanTrackState != frameData.PreviousData.LeanTrackState)
				changedData |= ChangedData::LeanTrackingStateChanged;

			UINT8 bodyID = frameData.Body->GetBodyID();

				//Notify each listener of the events that occured
			for (unsigned int i = 0; i < bodyListeners[bodyID].size(); i++)
			{
				if(changedData & ChangedData::TrackingIDLost)
					bodyListeners[bodyID][i]->BodyLost(frameData.CurrentData, frameData.PreviousData);

				if(changedData & ChangedData::BodyClippedChanged)
					bodyListeners[bodyID][i]->BodyClipChanged(frameData.CurrentData, frameData.PreviousData);
				if(changedData & ChangedData::EngagedChanged)
					bodyListeners[bodyID][i]->BodyEngagedChanged(frameData.CurrentData, frameData.PreviousData);
				if(changedData & ChangedData::RestrictedChanged)
					bodyListeners[bodyID][i]->BodyRestrictedChanged(frameData.CurrentData, frameData.PreviousData);
				if(changedData & ChangedData::TrackingChanged)
					bodyListeners[bodyID][i]->BodyTrackChanged(frameData.CurrentData, frameData.PreviousData);

				if(changedData & ChangedData::LeanTrackingStateChanged)
					bodyListeners[bodyID][i]->LeanTrackingStateChanged(frameData.CurrentData, frameData.PreviousData);

				if(changedData & ChangedData::LeftHandTrackingStateChanged)
					bodyListeners[bodyID][i]->HandTrackingStateChanged(Hand::Left,frameData.CurrentData, frameData.PreviousData);
				if(changedData & ChangedData::RightHandTrackingStateChanged)
					bodyListeners[bodyID][i]->HandTrackingStateChanged(Hand::Right, frameData.CurrentData, frameData.PreviousData);

				bodyListeners[bodyID][i]->BodyFrameAcquired(frameData.CurrentData, frameData.PreviousData);
			}

			
			//If there are listeners to remove from this body index
			if(listenersToRemove[bodyID].size() > 0)
			{
				//Loop through each one and unregister it
				for (int i = 0; i < listenersToRemove[bodyID].size(); i++)
				{
					UnregisterListener(frameData.Body, listenersToRemove[bodyID][i]);
					listenersToRemove[bodyID].erase(listenersToRemove[bodyID].begin() + i);
				}
			}
		}
	}
		
	processing = true;

	while (processing)
	{
		GestureFrameData currentData;

		processing = gestureDataToProcess.try_pop(currentData);

		if(processing)
		{
			UINT8 bodyID = currentData.Body->GetBodyID();

			int discreteSize = (int)currentData.DiscreteGestureResult.size();
			int continuousSize = (int)currentData.ContinuousGestureResult.size();

			//If there are results
			if(discreteSize > 0 || continuousSize > 0)
			{
				for (unsigned int i = 0; i < bodyListeners[bodyID].size(); i++)
				{
					//Notify listeners if we have results for the specified type
					if(discreteSize > 0)
						bodyListeners[bodyID][i]->DiscreteGesturesAcquired(currentData.DiscreteGestureResult);

					if(continuousSize > 0)
						bodyListeners[bodyID][i]->ContinuousGesturesAcquired(currentData.ContinuousGestureResult);
				}
			}
		}
	}
	
}