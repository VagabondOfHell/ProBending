#include "KinectReader.h"
#include <ostream>
#include <Kinect.VisualGestureBuilder.h>
#include "KinectBodyReader.h"

const float		KinectReader::cDepthWidthMult = 1.0f / cDepthWidth;
const float		KinectReader::cDepthHeightMult = 1.0f / cDepthHeight;

KinectReader::KinectReader(bool _deleteReadersOnClose)
{
	mSensor = NULL;
	bodyReader = NULL;
	mCoordinateMapper = NULL;
	kinectListener = NULL;
	
	deleteReadersOnClose = _deleteReadersOnClose;
	
	isOpen = false;
	isAvailable = false;
}

KinectReader::~KinectReader(void)
{
	if(deleteReadersOnClose)
	{
		if(bodyReader)
		{
			delete bodyReader;
			bodyReader = nullptr;
		}
	}

	if(mCoordinateMapper != NULL)
	{
		mCoordinateMapper->Release();
		mCoordinateMapper = NULL;
	}

	mSensor->Close();

	if(mSensor != NULL)
	{
		mSensor->Release();
		mSensor = NULL;
	}
}

HRESULT KinectReader::InitializeKinect(const UINT32 windowWidth, const UINT32 windowHeight)
{
	//Try to get the default kinect sensor and fill in our variable
	HRESULT hr = GetDefaultKinectSensor(&mSensor);
	
	//If sensor has been found
	if (mSensor)
    {
		if(windowWidth > 0 && windowHeight > 0)
		{
			mWindowSize = DepthSpacePoint();
		}
		else
			return E_FAIL;

		//Store window size
		mWindowSize.X = windowWidth;
		mWindowSize.Y = windowHeight;
		
		//Open/Initialize Kinect
		hr = mSensor->Open();

		mSensor->get_IsOpen(&isOpen);
		mSensor->get_IsAvailable(&isAvailable);

		//Get required components
		if (SUCCEEDED(hr))
		{
			hr = mSensor->get_CoordinateMapper(&mCoordinateMapper);
		}		
    }
	
	//Indicate failure if this point is hit
    if (!mSensor || FAILED(hr))
    {
		printf("No ready Kinect found!");
        return E_FAIL;
    }

	return hr;
}

bool KinectReader::KinectConnected()const
{
	return isAvailable && isOpen;
}

void KinectReader::SetWindowSize(const UINT32 width, const UINT32 height)
{
	//Validate
	if(width > 0 && height > 0)
	{
		mWindowSize = DepthSpacePoint();
		mWindowSize.X = width;
		mWindowSize.Y = height;
	}
	else
		throw EXCEPTION_FLT_INVALID_OPERATION;
}

const DepthSpacePoint KinectReader::GetWindowSize()const
{
	return mWindowSize;
}

HRESULT KinectReader::Capture()
{
	HRESULT hr; 
	
	bool oldAvailable = isAvailable;

	hr = mSensor->get_IsAvailable(&isAvailable);
	
	if(bodyReader)
	{
		hr = bodyReader->Capture();
	}
	
	if(oldAvailable && !isAvailable)
		if(kinectListener)
			kinectListener->SensorDisconnected();
	
	return hr;
}

bool KinectReader::OpenBodyReader()
{	
	if(bodyReader == NULL)
	{
		bodyReader = new KinectBodyReader();

		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;

		HRESULT hr = mSensor->get_BodyFrameSource(&pBodyFrameSource);
    
		//open the reader of our friend BodyReader
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&bodyReader->mBodyFrameReader);
		}

		//Release the Body Frame Source once we have the Reader from it
		if(pBodyFrameSource != NULL)
		{
			pBodyFrameSource->Release();
			pBodyFrameSource = NULL;
		}

		if(SUCCEEDED(hr))
		{
			bodyReader->mKinectReader = this;
			return true;
		}
		else
			return false;
	}
	
	return false;
}

const KinectBodyReader* const KinectReader::GetBodyReader()const
{
	return bodyReader;
}

///Returns a pointer to the Kinect Sensor
IKinectSensor* const KinectReader::GetKinectSensor() const
{
	return mSensor;
}

KinectSensorListener* KinectReader::RegisterSensorListener(KinectSensorListener* listener)
{
	KinectSensorListener* old = NULL;

	if(kinectListener)
		old = kinectListener;

	kinectListener = listener;

	return old;
}

KinectSensorListener* KinectReader::UnregisterSensorListener()
{
	KinectSensorListener* old = NULL;

	if(kinectListener)
		old = kinectListener;

	kinectListener = NULL;

	return old;
}

/// <summary>
/// Converts a body point to screen space
/// </summary>
/// <param name="bodyPoint">body point to tranform</param>
/// <param name="width">width (in pixels) of output buffer</param>
/// <param name="height">height (in pixels) of output buffer</param>
/// <returns>point in screen-space</returns>
DepthSpacePoint KinectReader::BodyToScreen(const CameraSpacePoint& bodyPoint)const
{
    // Calculate the body's position on the screen
    DepthSpacePoint depthPoint = {0};

	//Convert from camera space to depth space
    mCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);
	
	//Take the conversion and multiply by our window size, and then by
	//(1 / Kinect Resolution). 
	//Technically we are normalizing the coordinate and then multiplying by our window size
	depthPoint.X = static_cast<float>(depthPoint.X * mWindowSize.X) * cDepthWidthMult;
	depthPoint.Y = static_cast<float>(depthPoint.Y * mWindowSize.Y) * cDepthHeightMult;

	return depthPoint;
}