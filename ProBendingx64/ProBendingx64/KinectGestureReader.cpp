#include "KinectGestureReader.h"
#include "KinectBodyReader.h"
#include "KinectReader.h"
#include "KinectGestureDatabase.h"
#include "KinectBodyEventNotifier.h"

KinectGestureReader::KinectGestureReader(void)
{
	gestureReader = NULL;
	gestureSource = NULL;
}

KinectGestureReader::~KinectGestureReader(void)
{
	if(gestureReader)
	{
		gestureReader->Release();
		gestureReader = NULL;
	}

	ClearGesturesInSource();

	if(gestureSource)
	{
		gestureSource->Release();
		gestureSource = NULL;
	}
}

bool KinectGestureReader::Initialize(KinectReader* const sensor)
{
	HRESULT hr;

	//Go through each owning entity to acquire the KinectSensor and build the Gesture Source from it
	hr = CreateVisualGestureBuilderFrameSource(sensor->GetKinectSensor(), 0, &gestureSource);
	
	if(FAILED(hr))
	{
		if(gestureSource)
		{
			gestureSource->Release();
			gestureSource = NULL;
		}
		return false;
	}

	hr = gestureSource->OpenReader(&gestureReader);

	if(FAILED(hr))
	{
		if(gestureReader)
		{
			gestureReader->Release();
			gestureReader = NULL;
		}

		return false;
	}

	return true;
}

UINT64 KinectGestureReader::GetBodyID()const
{
	UINT64 bodyID;

	HRESULT hr = gestureSource->get_TrackingId(&bodyID);
	
	if(SUCCEEDED(hr))
		return bodyID;
	else
		return 0;
}

bool KinectGestureReader::GetBodyIDIsValid() const
{
	BOOLEAN result;

	HRESULT hr = gestureSource->get_IsTrackingIdValid(&result);

	if(FAILED(hr))
		return false;
	else
		return true;
}

int KinectGestureReader::GetGestureCount()const
{
	UINT32 count;
	HRESULT hr = gestureSource->get_GestureCount(&count);

	if(FAILED(hr))
		return -1;

	return count;
}

bool KinectGestureReader::SetGestureEnabled(std::wstring& gestureName, bool value)
{
	IGesture* gesture = GetGestureByName(gestureName);

	if(gesture)
	{
		HRESULT hr = gestureSource->SetIsEnabled(gesture, value);

		if(SUCCEEDED(hr))
			return true;
	}

	return false;
}

bool KinectGestureReader::GetGestureEnabled(std::wstring& gestureName)
{
	IGesture* gesture = GetGestureByName(gestureName);

	if(gesture)
	{
		BOOLEAN result; 

		HRESULT hr = gestureSource->GetIsEnabled(gesture, &result);

		if(SUCCEEDED(hr))
			return true;
	}

	return false;
}

bool KinectGestureReader::RemoveGesture(std::wstring& gestureName)
{
	IGesture* gesture = GetGestureByName(gestureName);

	if(gesture)
	{
		HRESULT hr = gestureSource->RemoveGesture(gesture);	
		if(SUCCEEDED(hr))
		{
			std::map<std::wstring, IGesture*>::iterator findResult = gesturesInSource.find(gestureName);

			if(findResult != gesturesInSource.end())
			{
				gesturesInSource.erase(findResult);
				return true;
			}
		}
	}

	return false;
}

void KinectGestureReader::ClearGesturesInSource()
{
	///CHECK IF THIS REMOVES THE SOURCES' GESTURES AS WELL
	std::map<std::wstring, IGesture*>::iterator start = gesturesInSource.begin();
	std::map<std::wstring, IGesture*>::iterator end = gesturesInSource.end();

	for (start; start != end; ++start)
	{
		gestureSource->RemoveGesture(start->second);
		start->second->Release();
	}

	gesturesInSource.clear();
}

IGesture* KinectGestureReader::GetGestureByName(std::wstring gestureName)
{
	std::map<std::wstring, IGesture*>::iterator result = 
		gesturesInSource.find(gestureName);

	if(result != gesturesInSource.end())
	{
		return result->second;
	}

	return nullptr;
}

bool KinectGestureReader::Capture()
{
	IVisualGestureBuilderFrame* frame;

	//Get the frame
	HRESULT hr = gestureReader->CalculateAndAcquireLatestFrame(&frame);

	if(FAILED(hr))
	{
		if(frame)
			frame->Release();
		return false;
	}

	//Prepare Result holders
	IGestureResult* result;
	IContinuousGestureResult* continuousResult;
	IDiscreteGestureResult* discreteResult;
	KinectGestureResult gestureResult;

	std::vector<KinectGestureResult> discreteResults = std::vector<KinectGestureResult>();
	std::vector<KinectGestureResult> continuousResults = std::vector<KinectGestureResult>();

	//Make sure results is initialized
	std::map<std::wstring, IGesture*>::iterator start = gesturesInSource.begin();
	std::map<std::wstring, IGesture*>::iterator end = gesturesInSource.end();
	
	for (start; start != end; ++start)
	{
		BOOLEAN gestureEnabled;

		//Make sure the gesture is enabled
		hr = gestureSource->GetIsEnabled(start->second, &gestureEnabled);

		if(FAILED(hr))
			continue;

		if(gestureEnabled)
		{
			//Get the results
			hr = frame->get_GestureResult(start->second, &result);
		
			if(FAILED(hr))
			{
				if(result)
				{
					result->Release();
					result = NULL;
				}
				continue;
			}
		
			//Extract the frame information
			gestureResult.gestureName = start->first;

			hr = start->second->get_GestureType(&gestureResult.gestureType);

			if(FAILED(hr))
				gestureResult.gestureType = GestureType_None;
	
			if(gestureResult.gestureType == GestureType_Continuous)
			{
				continuousResult = (IContinuousGestureResult*)result;
				continuousResult->get_Progress(&gestureResult.continuousProgress);
				//Add to the list of continuous results
				continuousResults.push_back(gestureResult);
			}
			else if(gestureResult.gestureType == GestureType_Discrete)
			{
				discreteResult = (IDiscreteGestureResult*)result;
				discreteResult->get_Confidence(&gestureResult.discreteConfidence);
				discreteResult->get_Detected(&gestureResult.discreteDetected);
				discreteResult->get_FirstFrameDetected(&gestureResult.discreteFirstFrameDetected);
				//Add to the list of discrete results
				discreteResults.push_back(gestureResult);
			}

			if(result)
			{
				result->Release();
				result = NULL;
			}
		}
	}

	//Pass the gesture data to the Event Manager
	KinectBodyEventNotifier::GetInstance()->InjectGestureFrameData(bodyOwner, &discreteResults, &continuousResults);

	return true;
}

