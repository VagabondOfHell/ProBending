#pragma once
#include "KinectReader.h"

class KinectBodyListener;
class KinectAudioListener;

class InputManager
{
private:
	static InputManager* instance;
	KinectGestureReader* gestureReader;
	
	KinectReader* kinectReader;

	bool kinectConnected;
	bool speechConnected;

	InputManager(void);

	///<summary>Shuts down all capture threads</summary>
	void ShutdownThreads();

	void BodyCapture();

	void AudioCapture();

public:
	~InputManager(void);

	static inline InputManager* GetInstance()
	{
		if(!instance)
			instance = new InputManager();

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

	///<summary>Gets the Gesture Reader of the Input Manager
	///Meant to be a Default gesture reader</summary>
	///<returns>The Gesture Reader associated with the Input Manager</returns>
	inline KinectGestureReader* const GetDefaultGestureReader()
	{
		return gestureReader;
	}

	///<summary>Gets the Kinect Reader</summary>
	///<returns>Pointer to the Kinect Reader</returns>
	inline KinectReader* const GetKinectReader()
	{
		return kinectReader;
	}

	///<summary>Gets the Kinect Speech Reader</summary>
	///<returns>Pointer to the Kinect Speech Reader</returns>
	inline KinectSpeechReader* const GetSpeechReader()
	{
		return kinectReader->GetSpeechReader();
	}

	///<summary>Gets the Kinect Body Reader</summary>
	///<returns>Pointer to the Kinect Body Reader</returns>
	inline const KinectBodyReader* const GetBodyReader()
	{
		return kinectReader->GetBodyReader();
	}

	///<summary>Initializes the Kinect input system</summary>
	///<param "windowWidth">The width of the render window</param>
	///<param "windowHeight">The height of the render window</param>
	///<returns>True if successful, false for error</returns>
	bool InitializeKinect(const UINT32 windowWidth, const UINT32 windowHeight);

	///<summary>Fills the default gesture reader with all gestures
	///in the specified database</summary>
	///<param "databaseFileName">string representation of the 
	///file name to load from</param>
	///<returns>True if successful, false for error</returns>
	bool FillGestureReader(std::string databaseFileName);
	
	///<summary>Fills the default gesture reader with all gestures
	///in the specified database</summary>
	///<param "databaseFileName">string representation of the 
	///file name to load from</param>
	///<returns>True if successful, false for error</returns>
	bool FillGestureReader(std::wstring databaseFileName);
	
	///<summary>Registers a listener to the kinect sensor</summary>
	///<param "listener">The listener to register </param>
	///<returns>The old Sensor Listener, or NULL if none existed</returns>
	inline KinectSensorListener* RegisterSensorListener(KinectSensorListener* listener)
	{
		return kinectReader->RegisterSensorListener(listener);
	}

	///<summary>Registers the specified listener to a new unlistened body</summary>
	///<param "listener">The new body listener to register</param>
	///<returns>True if successful, false if not</returns>
	bool RegisterListenerToNewBody(KinectBodyListener* listener);

	///<summary>Registers the specified listener to the specified body</summary>
	///<param "bodyID">The ID of the body to register to</param>
	///<param "listener">The listener to attach</param>
	///<returns>True if successful, false if not</returns>
	bool RegisterListenerToBody(UINT8 bodyID, KinectBodyListener* listener);

	///<summary>Swaps the bodies that each listener is listening to</summary>
	///<param "listener1">The listener to switch from</param>
	///<param "listener2">The listener to switch to</param>
	///<returns>True if successful, false if not</returns>
	bool SwapBodyListeners(KinectBodyListener* listener1, KinectBodyListener* listener2);

	///<summary>Remove the listener from all listening</summary>
	///<param "listener">The listener to unregister</param>
	///<returns>True if successful, false if not</returns>
	void UnregisterBodyListener(KinectBodyListener* listener);

	///<summary>Registers an audio listener</summary>
	///<param "audioListener">The listener to register</param>
	///<returns>True if successful, false if not</returns>
	bool RegisterAudioListener(KinectAudioListener* audioListener);

	///<summary>Unregisters an audio listener</summary>
	///<param "audioListener">The listener to unregister</param>
	///<returns>True if successful, false if not</returns>
	bool UnregisterAudioListener(KinectAudioListener* audioListener);

	///<summary>Begins the capture threads</summary>
	void BeginAllCapture();

	///<summary>Set the pause state of Speech Capture</summary>
	///<param "enabled">True to pause, false to unpause</param>
	void SetSpeechCaptureState(bool enabled);

	///<summary>Set the pause state of Body Capture</summary>
	///<param "enabled">True to pause, false to unpause</param>
	void SetBodyCaptureState(bool enabled);

	///<summary>Processes the events of the Kinect</summary>
	void ProcessEvents();

	///<summary>Stops all capture listening</summary>
	inline void StopAllCapture()
	{
		ShutdownThreads();
	}
};

