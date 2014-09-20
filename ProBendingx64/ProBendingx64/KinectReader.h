#pragma once
#include <kinect.h>

class BodyReader;

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

	BodyReader*				bodyReader;				

	BOOLEAN					isOpen;
	BOOLEAN					isAvailable;

public:
	
	KinectReader(bool deleteReadersOnClose = false);
	~KinectReader(void);

	//Initializes Kinect
	//<Param: windowWidth> Width of the Render Window
	//<Param: windowHeight> Height of the Render Window
	HRESULT InitializeKinect(const UINT32 windowWidth, const UINT32 windowHeight);

	///Acquires the latest frames for any attached and initialized Readers
	HRESULT Capture();

	///<summary>
	///Register a BodyReader to the Kinect
	///</summary>
	///<param name="reader">The reader to register to the Kinect</param>
	///<returns>True if successful, false if not</returns>
	bool RegisterBodyReader(BodyReader* reader);

	//Returns a pointer to the body reader currently attached to the Kinect Reader
	const BodyReader* const GetBodyReader()const;

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


