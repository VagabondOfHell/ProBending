#pragma once
#include "KinectBodyListener.h"

class KinectBodyEventNotifier
{
private:
	
	enum ChangedData
	{
		None = 0,
		TrackingChanged = 1,
		TrackingIDLost = 1 << 1,
		EngagedChanged = 1 << 2,
		RestrictedChanged = 1 << 3,
		BodyClippedChanged = 1 << 4,
		LeanTrackingStateChanged = 1 << 5,
		LeftHandTrackingStateChanged = 1 << 6,
		RightHandTrackingStateChanged = 1 << 7
	};

	static KinectBodyEventNotifier* instance;

	KinectBodyEventNotifier(void);
	
public:
	
	~KinectBodyEventNotifier(void);

	static KinectBodyEventNotifier* const GetInstance()
	{
		if(!instance)
			instance = new KinectBodyEventNotifier();

		return instance;
	}
	
	
	inline void DestroySingleton()
	{
		if(instance)
		{
			delete instance;
			instance = NULL;
		}
	}

	/// <summary>
	///Checks if the indicated body index currently has a listener attached
	/// </summary>
	/// <param name="bodyIndex">The index to check</param>	
	///	<returns>True if it has listeners, false if not</returns>
	bool CheckBodyIndexHasListener(UINT8 bodyIndex)const;

	/// <summary>
	///Checks how many listeners are attached to the specified body
	/// </summary>
	/// <param name="bodyIndex">The index to check</param>	
	///	<returns>The number of listeners attached</returns>
	int GetBodyIndexListenerCount(UINT8 bodyIndex)const;

	/// <summary>
	///Gets the first body index that doesn't have a listener to it
	/// </summary>
	///	<returns>The index of the unlistened body, -1 if none found</returns>
	int GetFirstUnlistenedBodyIndex()const;

	/// <summary>
	///Gets all unlistened body indices
	/// </summary>
	///	<returns>A vector of unlistened body indices</returns>
	std::vector<UINT8> GetAllUnlistenedBodyIndices()const;

	/// <summary>
	///Registers the listener to the specified body. Will not add the same listener
	///twice to the same body, but it can be added to other bodies, so be sure to 
	///remove from other body's before registering to another unless this behaviour is desired
	/// </summary>
	/// <param name="body">The body to register to</param>
	/// <param name="listener">The listener to assign</param>	
	bool RegisterListener(KinectBody* _body, KinectBodyListener* listener)const;

	/// <summary>
	///Removes the listener to the specified body
	/// </summary>
	/// <param name="bodyID">The ID to unregister from</param>
	/// <param name="listener">The listener to remove</param>	
	bool UnregisterListener(KinectBody* const _body, KinectBodyListener* listener)const;
	
	/// <summary>
	///Flags the listener for removal. Use this if attempting to remove the listener from an event.
	///Listener will be removed after event registration
	/// </summary>
	/// <param name="bodyIndex">The body index of the listener</param>	
	/// <param name="listener">The listener to unregister</param>	
	void FlagListenerForRemoval(KinectBody* const _body, KinectBodyListener* listener);

	///<summary>Unregisters all listeners that were flagged for removal</summary>
	void FlushListeners();

	/// <summary>
	///Injects data from the current Body Frame
	/// </summary>
	/// <param name="body">The Body the events will pertain to</param>	
	/// <param name="previousData">The data from the last frame</param>	
	/// <param name="currentData">The data from this frame</param>	
	void InjectBodyFrameData(KinectBody* const body, 
						CompleteData* previousData, CompleteData* currentData);

	/// <summary>
	///Injects data from the current Gesture Frame
	/// </summary>
	/// <param name="body">The Body the events will pertain to</param>	
	/// <param name="discreteResults">The data for Discrete Gestures</param>	
	/// <param name="continuousResults">The data for Continuous Gestures</param>	
	void InjectGestureFrameData(KinectBody* const body, std::vector<KinectGestureResult>* discreteResults, 
	std::vector<KinectGestureResult>* continuousResults)const;

	/// <summary>
	///Processes the events that are currently available
	/// </summary>
	void ProcessEvents();
};

