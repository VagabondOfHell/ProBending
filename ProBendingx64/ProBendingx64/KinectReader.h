#pragma once
#include <kinect.h>
#include "KinectBodyReader.h"
#include "KinectSpeechReader.h"
#include <atomic>

class KinectSensorListener;

class KinectReader
{
public:
private:
	bool deleteReadersOnClose;

	static const int        cDepthWidth  = 512;
    static const int        cDepthHeight = 424;
	static const float		cDepthWidthMult;
	static const float		cDepthHeightMult;

	IKinectSensor*			mSensor;
	ICoordinateMapper*      mCoordinateMapper;

	DepthSpacePoint			mWindowSize;
	
	KinectBodyReader*		bodyReader;	
	KinectSpeechReader*		speechReader;
	
	KinectSensorListener*	kinectListener;

	BOOLEAN					isOpen;
	std::atomic_bool		isAvailable;

public:
	
	KinectReader(bool deleteReadersOnClose = false);
	~KinectReader(void);

	//Initializes Kinect
	//<Param: windowWidth> Width of the Render Window
	//<Param: windowHeight> Height of the Render Window
	HRESULT InitializeKinect(const UINT32 windowWidth, const UINT32 windowHeight);

	///True if connected and available, false if not
	inline bool KinectReader::KinectConnected()const
	{
		return isAvailable && isOpen;
	}

	///True if a kinect sensor is available, false if not
	inline bool KinectReader::IsAvailable()
	{
		BOOLEAN available;

		mSensor->get_IsAvailable(&available);

		isAvailable = available ? true : false;

		return isAvailable;
	}

	///Acquires the latest frames for any attached and 
	//initialized Readers in a single thread
	HRESULT Capture();

	///Captures the body reader frame data
	bool CaptureBodyReader();

	///Captures the speech reader frame data
	bool CaptureSpeechReader();

	///<summary>
	///Open the body reader
	///</summary>
	///<returns>True if successful, false if not</returns>
	bool OpenBodyReader();

	///<summary>
	///Open the speech reader
	///</summary>
	///<returns>True if successful, false if not</returns>
	bool OpenSpeechReader();

	///<summary>
	///Closes the body reader
	///</summary>
	///<returns>True if successful, false if not</returns>
	inline bool KinectReader::CloseBodyReader()
	{
		if(bodyReader)
		{
			delete bodyReader;
			bodyReader = NULL;
			return true;
		}
		return false;
	}

	///<summary>
	///Closes the speech reader
	///</summary>
	///<returns>True if successful, false if not</returns>
	inline bool KinectReader::CloseSpeechReader()
	{
		if(speechReader)
		{
			delete speechReader;
			speechReader = NULL;
			return true;
		}
		return false;
	}

	//Returns a pointer to the body reader currently attached to the Kinect Reader
	inline const KinectBodyReader* const KinectReader::GetBodyReader()const
	{
		return bodyReader;
	}

	///Returns a pointer to the Kinect Sensor
	inline IKinectSensor* const KinectReader::GetKinectSensor()
	{
		return mSensor;
	}

	///Returns a pointer to the Kinect Speech Reader
	inline KinectSpeechReader* const KinectReader::GetSpeechReader()const
	{
		return speechReader;
	}

	///Registers a Kinect Sensor Listener to the reader, and returns the old one, if any
	KinectSensorListener* RegisterSensorListener(KinectSensorListener* listener);

	///Unregisters a Kinect Sensor Listener from the reader and returns it
	KinectSensorListener* UnregisterSensorListener();

	//Set the size of the window. Call this when a window is resized
	//<Param: width> Width of the Render Window
	//<Param: height> Height of the Render Window
	//Will throw an exception if value is less than or equal to 0
	void SetWindowSize(const UINT32 width, const UINT32 height);
	
	//Gets the window size in a DepthSpacePoint struct
	inline const DepthSpacePoint KinectReader::GetWindowSize()const
	{
		return mWindowSize;
	}

	/// <summary>
	/// Converts a body point to screen space
	/// </summary>
	/// <param name="bodyPoint">body point to tranform</param>
	/// <returns>point in screen-space</returns>
	DepthSpacePoint BodyToScreen(const CameraSpacePoint& bodyPoint)const;
};

class KinectSensorListener
{
	friend class KinectReader;

public:

	KinectSensorListener(){}

	virtual ~KinectSensorListener(){}

protected:
	virtual void SensorDisconnected(){}

};

