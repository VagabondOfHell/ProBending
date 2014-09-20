#pragma once
#include "bodylistener.h"

class KinectController :
	public BodyListener
{
private:
	bool isListening;

public:

	KinectController(void);
	virtual ~KinectController(void);

	bool IsListening()const;

	virtual void ListenToNewBody(const BodyReader& reader);

	virtual void BodyLost(const BodyReader& reader, const CompleteData& previousData);

	//Event Overriding
	virtual void BodyTrackChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData);
	virtual void BodyEngagedChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData);
	virtual void BodyRestrictedChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData);

	virtual void BodyClipChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData);
	
	virtual void LeanChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData);
	virtual void LeanTrackingStateChanged(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData);
	
	virtual void HandTrackingStateChanged(const KinectReader* kinectReader, const Hand hand, const CompleteData& currentData, const CompleteData& previousData);
	virtual void HandConfidenceChanged(const KinectReader* kinectReader, const Hand hand, const CompleteData& currentData, const CompleteData& previousData);

	virtual void BodyJointDataReceived(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData);

	virtual void BodyFrameAcquired(const KinectReader* kinectReader, const CompleteData& currentData, const CompleteData& previousData);
};

