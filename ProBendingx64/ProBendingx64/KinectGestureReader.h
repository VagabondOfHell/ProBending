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
	
	bool Initialize(KinectReader* const sensor);

	///Sets the body to the gesture reader, and returns the old body, or Null if there wasnt one
	KinectBody* SetBody(KinectBody* body);

	void SetBodyID(UINT64 bodyID);

	KinectBody* GetBody()const;

	UINT64 GetBodyID()const;
	bool GetBodyIDIsValid()const;

	//Pause the reader to stop receiving Gesture Events
	void PauseReader()const;
	//Resume the reader to continue receiving Gesture Events
	void ResumeReader()const;
	//Get the current pause state of the reader
	bool GetPauseState()const;

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


