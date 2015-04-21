#include "KinectReader.h"
#include <thread>

const float		KinectReader::cDepthWidthMult = 1.0f / cDepthWidth;
const float		KinectReader::cDepthHeightMult = 1.0f / cDepthHeight;

KinectReader::KinectReader(bool _deleteReadersOnClose)
{
	mSensor = NULL;
	bodyReader = NULL;
	mCoordinateMapper = NULL;
	kinectListener = NULL;
	speechReader = NULL;

	deleteReadersOnClose = _deleteReadersOnClose;
	
	isOpen = false;
	isAvailable = false;
}

KinectReader::~KinectReader(void)
{
	CloseKinect();
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
		
		//Get the availability of the sensor
		bool avail = IsAvailable();
		
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

void KinectReader::CloseKinect()
{
	if(deleteReadersOnClose)
	{
		CloseBodyReader();
	}

	CloseSpeechReader();

	if(mCoordinateMapper != NULL)
	{
		mCoordinateMapper->Release();
		mCoordinateMapper = NULL;
	}

	if(mSensor != NULL)
	{
		mSensor->Close();
		mSensor->Release();
		mSensor = NULL;
	}
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

HRESULT KinectReader::Capture()
{
	HRESULT hr = S_OK; 
	
	bool oldAvailable = isAvailable ? true : false;

	//Get the availability of the sensor
	IsAvailable();
	
	if(isAvailable)
	{
		CaptureBodyReader();
		CaptureSpeechReader();
	}

	if(oldAvailable && !isAvailable)
		if(kinectListener)
			kinectListener->SensorDisconnected();
	
	return hr;
}

bool KinectReader::CaptureBodyReader()
{
	HRESULT hr = E_FAIL;

	if(bodyReader)
	{
		hr = bodyReader->Capture();
	}
	
	bool oldAvailable = isAvailable ? true : false;

	//Get the availability of the sensor
	IsAvailable();

	if(oldAvailable && !isAvailable)
		if(kinectListener)
			kinectListener->SensorDisconnected();

	return SUCCEEDED(hr) || hr == E_PENDING ? true : false;
}

bool KinectReader::CaptureSpeechReader()
{
	if(speechReader)
	{
		speechReader->Capture();
		return true;
	}

	return false;
}

bool KinectReader::OpenBodyReader()
{	
	if(IsAvailable())
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
	}
	return false;
}

bool KinectReader::OpenSpeechReader()
{
	if(isAvailable)
	{
		if(!speechReader)
		{
			speechReader = new KinectSpeechReader(this);
			if(speechReader->Initialize())
				return true;
			else
			{
				delete speechReader;
				speechReader = NULL;
			}
		}
	}
	return false;
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