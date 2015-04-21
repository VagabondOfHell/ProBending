#include "KinectBodyReader.h"
#include "KinectBodyEventNotifier.h"

std::array<KinectBody*, BODY_COUNT> bodies;

KinectBodyReader::KinectBodyReader(void)
{
	for (int i = 0; i < bodies.size(); i++)
	{
		//bodies[i] = NULL;
		bodies[i] = new KinectBody(this);
		bodies[i]->bodyArrayIndex = i;
	}
}

KinectBodyReader::~KinectBodyReader(void)
{
	if(mBodyFrameReader)
	{
		mBodyFrameReader->Release();
		mBodyFrameReader = nullptr;
	}

	for (int i = 0; i < bodies.size(); i++)
	{
		if(bodies[i])
		{
			delete bodies[i];
			bodies[i] = NULL;
		}
	}
}

bool KinectBodyReader::OverrideHandTracking(UINT64 bodyTrackingID)const
{
	IBodyFrameSource* source;

	HRESULT hr = mBodyFrameReader->get_BodyFrameSource(&source);

	if(SUCCEEDED(hr))
	{
		hr = source->OverrideHandTracking(bodyTrackingID);

		if(SUCCEEDED(hr))
			return true;
	}

	return false;
}

bool KinectBodyReader::ReplaceAndOverrideHandTracking(UINT64 oldTrackingID, UINT64 newTrackingID)const
{
	IBodyFrameSource* source;

	HRESULT hr = mBodyFrameReader->get_BodyFrameSource(&source);

	if(SUCCEEDED(hr))
	{
		hr = source->OverrideAndReplaceHandTracking(oldTrackingID, newTrackingID);

		if(SUCCEEDED(hr))
			return true;
	}

	return false;
}

HRESULT KinectBodyReader::Capture()const
{
	//Create and acquire a new BodyFrame
	IBodyFrame* bodyFrame = NULL;
	HRESULT hr = mBodyFrameReader->AcquireLatestFrame(&bodyFrame);
	
	//If frame acquisition was successful, let the body reader know
	if(SUCCEEDED(hr))
	{
		//Initialize and acquire body array
		IBody* ppBodies[BODY_COUNT] = {0};
		HRESULT hr = bodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		
		//If no errors, process the bodies
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < BODY_COUNT; i++)
			{
				if (ppBodies[i])
				{
					bodies[i]->RecieveBodyInformation(ppBodies[i]);
				}
			}
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
	}
		
	//Dispose of the frame
	if(bodyFrame != NULL)
	{
		bodyFrame->Release();
		bodyFrame = NULL;
	}

	return hr;
}

int KinectBodyReader::FindBodyIndexFromTrackID(UINT64 trackingID)const
{
	for (int i = 0; i < bodies.size(); i++)
	{
		if(bodies[i]->GetBodyTrackingID() == trackingID)
			return i;
	}
	
	return -1;
}

UINT64 KinectBodyReader::FindTrackIDFromBodyIndex(UINT8 bodyIndex)const
{
	return bodies[bodyIndex]->GetBodyTrackingID();
}

bool KinectBodyReader::CheckBodyIndexHasListener(const UINT8 bodyIndex)const
{
	return KinectBodyEventNotifier::GetInstance()->CheckBodyIndexHasListener(bodyIndex);
}

KinectBody* const KinectBodyReader::GetBodyAtIndex(UINT8 bodyIndex)const
{
	if(bodyIndex < bodies.size())
		return bodies[bodyIndex];

	return NULL;
}

int KinectBodyReader::GetFirstValidBodyIndex()const
{
	for (int i = 0; i < bodies.size(); i++)
	{
		if(bodies[i])
		{
			if(bodies[i]->GetTrackingIDIsValid())
			{
				return i;
			}
		}
	}

	return -1;
}

std::vector<UINT8> KinectBodyReader::GetAllValidBodyIndices()const
{
	std::vector<UINT8> validIDs = std::vector<UINT8>();

	for (int i = 0; i < bodies.size(); i++)
	{
		if(bodies[i])
		{
			if(bodies[i]->GetTrackingIDIsValid())
			{
				validIDs.push_back(i);
			}
		}
	}

	return validIDs;
}

int KinectBodyReader::GetFirstUnlistenedBodyIndex()const
{
	return KinectBodyEventNotifier::GetInstance()->GetFirstUnlistenedBodyIndex();
}

std::vector<UINT8> KinectBodyReader::GetAllUnlistenedBodyIndices()const
{
	return KinectBodyEventNotifier::GetInstance()->GetAllUnlistenedBodyIndices();
}

int KinectBodyReader::GetFirstValidUnlistenedBodyIndex()const
{
	std::vector<UINT8> validIDs = GetAllValidBodyIndices();
	std::vector<UINT8> unlistenedIDs = GetAllUnlistenedBodyIndices();

	//Loop through all the valid IDs and compare to the Unlistened IDs
	for (int v = 0; v < validIDs.size(); v++)
	{
		for (int u = 0; u < unlistenedIDs.size(); u++)
		{
			//If they match, return it
			if(validIDs[v] == unlistenedIDs[u])
				return validIDs[v];
		}
	}

	return -1;
}

std::vector<UINT8> KinectBodyReader::GetValidUnlistenedBodyIndices()const
{
	std::vector<UINT8> validIDs = GetAllValidBodyIndices();
	std::vector<UINT8> unlistenedIDs = GetAllUnlistenedBodyIndices();

	std::vector<UINT8> results = std::vector<UINT8>(bodies.size());

	//Loop through all the valid IDs and compare to the Unlistened IDs
	for (int v = 0; v < validIDs.size(); v++)
	{
		for (int u = 0; u < unlistenedIDs.size(); u++)
		{
			//If they match, add to the results list
			if(validIDs[v] == unlistenedIDs[u])
				results.push_back(v);
		}
	}

	return results;
}