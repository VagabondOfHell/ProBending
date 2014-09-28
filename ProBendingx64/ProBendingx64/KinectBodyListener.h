#pragma once
#include "KinectBody.h"
#include "KinectGestureReader.h"
#include <vector>

class KinectBodyListener
{
	friend class KinectBodyEventNotifier;
private:
	KinectBody* body;

	void AddNewConnection(KinectBody* const _body)
	{
		//Because KinectBodyEventNotifier calls this method, we trust it not
		//to send us a duplicate body index
 		body = _body;
	}

	void RemoveConnection()
	{
		body = NULL;	
	}

public:
	KinectBodyListener()
	{
		body = NULL;
	}

	virtual ~KinectBodyListener(){}

	bool IsListening()const
	{
		return body != NULL;
	}

	KinectBody* const GetBody()const
	{
		return body;
	}

protected:

	virtual void BodyLost( const CompleteData& currentData, const CompleteData& previousData){}

	virtual void BodyTrackChanged(const CompleteData& currentData, const CompleteData& previousData){}
	virtual void BodyEngagedChanged(const CompleteData& currentData, const CompleteData& previousData){}
	virtual void BodyRestrictedChanged(const CompleteData& currentData, const CompleteData& previousData){}

	virtual void BodyClipChanged(const CompleteData& currentData, const CompleteData& previousData){}
	
	virtual void LeanChanged(const CompleteData& currentData, const CompleteData& previousData){}
	virtual void LeanTrackingStateChanged(const CompleteData& currentData, const CompleteData& previousData){}
	
	virtual void HandTrackingStateChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData){}
	virtual void HandConfidenceChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData){}

	virtual void BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData){}

	virtual void DiscreteGesturesAcquired(const std::vector<KinectGestureResult>discreteGestureResults){}
	virtual void ContinuousGesturesAcquired(const std::vector<KinectGestureResult>continuousGestureResults){}

};