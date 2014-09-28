#include "KinectBodyEventNotifier.h"
#include <array>
#include <vector>

//Array of body listeners
std::array<std::vector<KinectBodyListener*>, BODY_COUNT> bodyListeners;

static KinectBodyEventNotifier* instance;

KinectBodyEventNotifier::KinectBodyEventNotifier(void)
{
}

KinectBodyEventNotifier::~KinectBodyEventNotifier(void)
{
}

void KinectBodyEventNotifier::DestroySingleton()
{
	if(instance)
	{
		delete instance;
		instance = NULL;
	}
}

KinectBodyEventNotifier* const KinectBodyEventNotifier::GetInstance()
{
	if(!instance)
		instance = new KinectBodyEventNotifier();

	return instance;
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
	UINT8 bodyIndex = _body->GetBodyID();

	if(bodyIndex < bodyListeners.size())
	{
		listenersToRemove[bodyIndex].push_back(listener);
	}
}

void KinectBodyEventNotifier::InjectBodyFrameData(KinectBody* const body, 
						CompleteData& previousData, CompleteData& currentData)
{
	char changedData = ChangedData::None;

	if(currentData.BodyTrackingID != previousData.BodyTrackingID)
		changedData |= ChangedData::TrackingIDLost;

	//Check what data has been changed to fire an appropriate event for it
	if(currentData.IsTracked != previousData.IsTracked)
		changedData |= ChangedData::TrackingChanged;

	if(currentData.ClippedEdge != previousData.ClippedEdge)
		changedData |= ChangedData::BodyClippedChanged;
	
	if(currentData.IsEngaged != previousData.IsEngaged)
		changedData |= ChangedData::EngagedChanged;

	if(currentData.IsRestricted != previousData.IsRestricted)
		changedData |= ChangedData::RestrictedChanged;

	if(currentData.LeftHandState != previousData.LeftHandState)
		changedData |= ChangedData::LeftHandTrackingStateChanged;

	if(currentData.RightHandState != previousData.RightHandState)
		changedData |= ChangedData::RightHandTrackingStateChanged;

	if(currentData.LeanTrackState != previousData.LeanTrackState)
		changedData |= ChangedData::LeanTrackingStateChanged;

	UINT8 bodyID = body->GetBodyID();

	//Notify each listener of the events that occured
	for (unsigned int i = 0; i < bodyListeners[bodyID].size(); i++)
	{
		if(changedData & ChangedData::TrackingIDLost)
			bodyListeners[bodyID][i]->BodyLost(currentData, previousData);

		if(changedData & ChangedData::BodyClippedChanged)
			bodyListeners[bodyID][i]->BodyClipChanged(currentData, previousData);
		if(changedData & ChangedData::EngagedChanged)
			bodyListeners[bodyID][i]->BodyEngagedChanged(currentData, previousData);
		if(changedData & ChangedData::RestrictedChanged)
			bodyListeners[bodyID][i]->BodyRestrictedChanged(currentData, previousData);
		if(changedData & ChangedData::TrackingChanged)
			bodyListeners[bodyID][i]->BodyTrackChanged(currentData, previousData);

		if(changedData & ChangedData::LeanTrackingStateChanged)
			bodyListeners[bodyID][i]->LeanTrackingStateChanged(currentData, previousData);

		if(changedData & ChangedData::LeftHandTrackingStateChanged)
			bodyListeners[bodyID][i]->HandTrackingStateChanged(Hand::Left,currentData, previousData);
		if(changedData & ChangedData::RightHandTrackingStateChanged)
			bodyListeners[bodyID][i]->HandTrackingStateChanged(Hand::Right, currentData, previousData);

		bodyListeners[bodyID][i]->BodyFrameAcquired(currentData, previousData);
	}

	//If there are listeners to remove from this body index
	if(listenersToRemove[bodyID].size() > 0)
	{
		//Loop through each one and unregister it
		for (int i = 0; i < listenersToRemove[bodyID].size(); i++)
		{
			UnregisterListener(body, listenersToRemove[bodyID][i]);
			listenersToRemove[bodyID].erase(listenersToRemove[bodyID].begin() + i);
		}
	}
}

void KinectBodyEventNotifier::InjectGestureFrameData(const KinectBody* const body, std::vector<KinectGestureResult>& discreteResults, 
	std::vector<KinectGestureResult>& continuousResults)const
{
	UINT8 bodyID = body->GetBodyID();

	int discreteSize = (int)discreteResults.size();
	int continuousSize = (int)continuousResults.size();

	//If there are results
	if(discreteSize > 0 || continuousSize > 0)
	{
		for (unsigned int i = 0; i < bodyListeners[bodyID].size(); i++)
		{
			//Notify listeners if we have results for the specified type
			if(discreteSize > 0)
				bodyListeners[bodyID][i]->DiscreteGesturesAcquired(discreteResults);

			if(continuousSize > 0)
				bodyListeners[bodyID][i]->ContinuousGesturesAcquired(continuousResults);
		}
	}
}
