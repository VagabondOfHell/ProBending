#pragma once
#include <kinect.h>

class KinectBodyReader;
class KinectSensorListener;

class KinectReader
{

public:
KinectBodyReader*		bodyReader;	
private:
	bool deleteReadersOnClose;

	static const int        cDepthWidth  = 512;
    static const int        cDepthHeight = 424;
	static const float		cDepthWidthMult;
	static const float		cDepthHeightMult;

	IKinectSensor*			mSensor;
	ICoordinateMapper*      mCoordinateMapper;

	DepthSpacePoint			mWindowSize;

		
	
	KinectSensorListener*	kinectListener;

	BOOLEAN					isOpen;
	BOOLEAN					isAvailable;

public:
	
	KinectReader(bool deleteReadersOnClose = false);
	~KinectReader(void);

	//Initializes Kinect
	//<Param: windowWidth> Width of the Render Window
	//<Param: windowHeight> Height of the Render Window
	HRESULT InitializeKinect(const UINT32 windowWidth, const UINT32 windowHeight);

	///True if connected and available, false if not
	bool KinectConnected()const;

	///Acquires the latest frames for any attached and initialized Readers
	HRESULT Capture();

	///<summary>
	///Open the body reader
	///</summary>
	///<returns>True if successful, false if not</returns>
	bool OpenBodyReader();

	//Returns a pointer to the body reader currently attached to the Kinect Reader
	const KinectBodyReader* const GetBodyReader()const;

	///Returns a pointer to the Kinect Sensor
	IKinectSensor* const GetKinectSensor() const;

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
	const DepthSpacePoint GetWindowSize()const;

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

