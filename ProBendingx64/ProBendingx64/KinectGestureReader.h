#pragma once
#include "KinectBody.h"
#include "KinectReader.h"
#include <Kinect.VisualGestureBuilder.h>
#include <string>
#include <map>
#include <vector>

struct KinectGestureResult
{
	std::wstring gestureName;
	GestureType gestureType;

	float continuousProgress;
	BOOLEAN discreteDetected;
	float discreteConfidence;
	BOOLEAN discreteFirstFrameDetected;

	KinectGestureResult()
	{
		
	}

	~KinectGestureResult()
	{
	}
};

class KinectGestureReader
{
	friend class KinectGestureDatabase;

private:
	KinectBody* bodyOwner;

	IVisualGestureBuilderFrameReader* gestureReader;
	IVisualGestureBuilderFrameSource* gestureSource;
	
	std::map<std::wstring, IGesture*> gesturesInSource;

	///Loops through the map and calls release on each IGesture*
	///Then calls Clear on the map. 
	/////////THIS MAY THROW ERRORS////////
	void ClearGesturesInSource();

	/// <summary>
	///Gets the IGesture associated to the specified name
	/// </summary>
	/// <param name="gestureName">The name of the gesture to get the related IGesture* from</param>
	/// <returns>The IGesture if successful, nullptr if failure or error</returns>	
	IGesture* GetGestureByName(std::wstring gestureName);

public:
	KinectGestureReader(void);
	~KinectGestureReader(void);
	
	bool Initialize(KinectReader* sensor);

	///Sets the body to the gesture reader, and returns the old body, or Null if there wasnt one
	inline KinectBody* KinectGestureReader::SetBody(KinectBody* body)
	{
		KinectBody* old = body;

		bodyOwner = body;

		return old;
	}

	inline void KinectGestureReader::SetBodyID(UINT64 bodyID)
	{
		//Set the body ID for the gesture to record
		gestureSource->put_TrackingId(bodyID);
	}

	inline KinectBody* KinectGestureReader::GetBody()const
	{
		return bodyOwner;
	}

	UINT64 GetBodyID()const;
	bool GetBodyIDIsValid()const;

	//Pause the reader to stop receiving Gesture Events
	inline void KinectGestureReader::PauseReader()const
	{
		gestureReader->put_IsPaused(true);
	}

	//Resume the reader to continue receiving Gesture Events
	inline void KinectGestureReader::ResumeReader()const
	{
		gestureReader->put_IsPaused(false);
	}

	//Get the current pause state of the reader
	inline bool KinectGestureReader::GetPauseState()const
	{
		BOOLEAN result;
		HRESULT hr = gestureReader->get_IsPaused(&result);
	
		if(result)
			return true;
		else
			return false;
	}

	///Gets the number of gestures currently in the GestureReader
	///Returns -1 on error, otherwise it returns the count
	int GetGestureCount()const;

	/// <summary>
	///Sets the corresponding gestures' enabled state
	/// </summary>
	/// <param name="gestureName">The name of the gesture to set</param>	
	/// <param name="value">The boolean value to set</param>	
	/// <returns>True if successful, false if not</returns>	
	bool SetGestureEnabled(std::wstring& gestureName, bool value);

	/// <summary>
	///Gets the corresponding gestures' enabled state
	/// </summary>
	/// <param name="gestureName">The name of the gesture to set</param>
	/// <returns>The enabled state of the gesture</returns>	
	bool GetGestureEnabled(std::wstring& gestureName);
	
	/// <summary>
	///Removes the specified gesture from the source and map
	/// </summary>
	/// <param name="gestureName">The name of the gesture to remove</param>
	/// <returns>True if removal was successful, false for failure or error</returns>	
	bool RemoveGesture(std::wstring& gestureName);

	///Captures the gesture data
	bool Capture();
};


