#pragma once
#include "KinectBody.h"
#include "KinectGestureReader.h"

class KinectBodyListener
{
	friend class KinectBodyEventNotifier;
private:
	KinectBody* body;
	
	//Connect a body to this listener for easy access
	inline void AddNewConnection(KinectBody* const _body)
	{
		//Because KinectBodyEventNotifier calls this method, we trust it not
		//to send us a duplicate body index
 		body = _body;

		BodyAcquired();
	}

	inline void RemoveConnection()
	{
		body = NULL;	
	}

public:
	KinectBodyListener()
	{
		body = NULL;
	}

	virtual ~KinectBodyListener(){}

	inline bool IsListening()const
	{
		return body != NULL;
	}

	//Returns the body associated to the reader, 
	//otherwise it returns NULL if its invalid or not attached
	inline KinectBody* const GetBody()const
	{
		if(body)
		{
			return body;
		}

		return NULL;
	}

protected:
	virtual void BodyAcquired(){}
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