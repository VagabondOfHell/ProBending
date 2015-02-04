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
	//True to be notified of events, false to not. This is useful for temporarily disabling input notification
	//without removing the listener. Will still be notified of body lost and body acquired events
	bool Enabled;

	KinectBodyListener()
	{
		body = NULL;
		Enabled = true;
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
		return body;
	}

protected:
	virtual void BodyAcquired(){}
	virtual void BodyLost( const CompleteData& currentData, const CompleteData& previousData){}

	virtual void BodyTrackChanged(const CompleteData& currentData, const CompleteData& previousData){}
	virtual void BodyEngagedChanged(const CompleteData& currentData, const CompleteData& previousData){}
	virtual void BodyRestrictedChanged(const CompleteData& currentData, const CompleteData& previousData){}

	virtual void BodyClipChanged(const CompleteData& currentData, const CompleteData& previousData){}
	
	virtual void LeanTrackingStateChanged(const CompleteData& currentData, const CompleteData& previousData){}
	
	virtual void HandTrackingStateChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData){}
	virtual void HandConfidenceChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData){}

	virtual void BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData){}

	virtual void DiscreteGesturesAcquired(const std::vector<KinectGestureResult>discreteGestureResults){}
	virtual void ContinuousGesturesAcquired(const std::vector<KinectGestureResult>continuousGestureResults){}

};