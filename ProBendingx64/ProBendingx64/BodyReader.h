#pragma once
#include <kinect.h>
#include <vector>
#include "BodyListener.h"

class BodyReader
{
	friend class KinectReader;

private:
	 // Body reader
    IBodyFrameReader*			mBodyFrameReader;
	
	enum ChangedData
	{
		None = 0,
		TrackingChanged = 1,
		EngagedChanged = 1 << 1,
		RestrictedChanged = 1 << 2,
		BodyClippedChanged = 1 << 3,
		LeanChanged = 1 << 4,
		LeanTrackingStateChanged = 1 << 5,
		LeftHandTrackingStateChanged = 1 << 6,
		RightHandTrackingStateChanged = 1 << 7,
		LeftHandConfidenceStateChanged = 1 << 8,
		RightHandConfidenceStateChanged = 1 << 9
	};

	//Creates CompleteData structs of the body data for each body
	void ProcessBodies(const KinectReader* kinectReader, const int numBodies, IBody** bodiesArray);
	
	//Analyzes any changes between frames and notifies the listeners of them
	void AnalyzeFrameInformation(const KinectReader* kinectReader, const CompleteData& currentData, 
		const CompleteData& previousData, std::vector<BodyListener*>& listenersOfData);

protected:
	bool FrameArrived(const KinectReader* kinectReader, IBodyFrame* frame);

public:
	int LeanHeuristic;//How far the lean has to move to fire a LeanChange event
	
	BodyReader(void);
	~BodyReader(void);

	//Returns all BodyID's that do not have a listener registered to them
	//Returns a vector containing the unlistened to Body ID's
	const std::vector<UINT64> GetUnlistenedBodyIDs()const;

	//Registers a listener to listen to the specified body.
	//Also attempts to remove listener from the 0 body
	//<Param: bodyID> The ID of the body to listen to
	//<Param: listener> Pointer to the listener to add
	void RegisterBodyListener(UINT64 bodyID, BodyListener* listener)const;

	//Removes the listener, using the body ID as an index to find it
	//<Param: bodyID> The ID of the body the listener listens to
	//<Param: listener> The listener to remove
	//Returns the listener that was found, or NULL if none were found. 
	//This allows the caller to delete it if desired
	BodyListener* UnregisterBodyListener(UINT64 bodyID, BodyListener* listener)const;

	//Slower than the other overload of this method, it searches through
	//all attached bodies and listeners for the listener to remove
	//<Param: listener> The listener to remove
	//Returns the listener that was found, or NULL if none were found. 
	//This allows the caller to delete it if desired
	BodyListener* UnregisterBodyListener(BodyListener* listener)const;

	
};

