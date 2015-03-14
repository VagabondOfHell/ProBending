#include "InputManager.h"
#include "KinectGestureReader.h"
#include "KinectBodyEventNotifier.h"
#include "KinectAudioEventNotifier.h"
#include "KinectGestureDatabase.h"
#include <thread>
#include <atomic>

InputManager* InputManager::instance = NULL;

std::atomic_bool cancelSpeechCapture;
std::atomic_bool cancelBodyCapture;

std::atomic_bool pauseSpeechCapture;
std::atomic_bool pauseBodyCapture;

std::thread speechCaptureThread;
std::thread bodyCaptureThread;

InputManager::InputManager(void)
{
	kinectReader = NULL;
	gestureReader = NULL;

}

InputManager::~InputManager(void)
{
	ShutdownThreads();

	KinectBodyEventNotifier::DestroySingleton();
	KinectAudioEventNotifier::DestroySingleton();

	if(gestureReader)
	{
		delete gestureReader;
		gestureReader = NULL;
	}
}

void InputManager::ShutdownThreads()
{
	cancelSpeechCapture = true;
	cancelBodyCapture = true;

	DWORD result = WaitForSingleObject(speechCaptureThread.native_handle(), 100);
	printf("First Wait Done\n");

	speechCaptureThread.join();

	printf("First Join Done\n");

	cancelBodyCapture = true;
	result = WaitForSingleObject(bodyCaptureThread.native_handle(), 100);

	printf("Second Wait Done\n");
	if(bodyCaptureThread.joinable())
		bodyCaptureThread.join();

	printf("Second Join Done\n");

	if(kinectReader)
	{
		delete kinectReader;
		kinectReader = NULL;
	}
}

void InputManager::CloseKinect()
{
	ShutdownThreads();
}

bool InputManager::InitializeKinect(const UINT32 windowWidth, const UINT32 windowHeight)
{
	HRESULT hr;

	kinectReader = new KinectReader(true);
	hr = kinectReader->InitializeKinect(windowWidth, windowHeight);

	if(FAILED(hr))
		return false;

	//Give the Kinect time to turn on
	Sleep(1000);

	 bool result = kinectReader->OpenBodyReader();
	 
	if(!result)
		return false;

	kinectConnected = true;

	result = kinectReader->OpenSpeechReader();

	if(!result)
		return false;

	result = kinectReader->GetSpeechReader()->StartRecognition();

	if(!result)
		return false;

	speechConnected = true;

	//Initialize the two singletons to make sure 
	//they aren't modified from another thread
	KinectAudioEventNotifier::GetInstance();
	KinectBodyEventNotifier::GetInstance();

	gestureReader = new KinectGestureReader();
	result = gestureReader->Initialize(kinectReader);

	if(!result)
		return false;

	return true;
}

bool InputManager::FillGestureReader(std::string databaseFileName)
{
	return FillGestureReader(std::wstring(databaseFileName.begin(), databaseFileName.end()));
}

bool InputManager::FillGestureReader(std::wstring databaseFileName)
{
	if(gestureReader)
	{
		bool result = KinectGestureDatabase::GetInstance()->OpenDatabase(databaseFileName);
	
		if(!result)
			return false;

		///NEED TO TRY CHANGING FROM SEPARATE THREAD///
		int gestures = KinectGestureDatabase::GetInstance()->FillSourceWithAllGestures(gestureReader);
	
		KinectGestureDatabase::GetInstance()->CloseDatabase();
	
		KinectGestureDatabase::GetInstance()->DestroySingleton();

		if(gestures < 0)
			return false;
		return true;
	}
	else 
		return false;
}

bool InputManager::RegisterListenerToNewBody(KinectBodyListener* listener)
{
	//Make sure we aren't being passed a NULL listener
	if(listener)
	{
		if(kinectConnected)
		{
			//Flag to remove the listener from the current body (if any)
			UnregisterBodyListener(listener);

			//Get the body ID
			int bodyID = kinectReader->GetBodyReader()->
				GetFirstValidUnlistenedBodyIndex();

			if( RegisterListenerToBody(bodyID, listener))
			{
				listener->GetBody()->AttachGestureReader(gestureReader);
				return true;
			}
		}
	}

	return false;
}

void InputManager::FlushListeners()
{
	KinectBodyEventNotifier::GetInstance()->FlushListeners();
}

bool InputManager::RegisterListenerToBody(UINT8 bodyID, KinectBodyListener* listener)
{
	if(kinectConnected)
	{
		if(bodyID >= 0)
		{
			//Get the body at the valid ID
			KinectBody* body = kinectReader->GetBodyReader()->GetBodyAtIndex(bodyID);
			
			if(body)
			{
				//Register the listener
				if(KinectBodyEventNotifier::GetInstance()->
					RegisterListener(body, listener))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool InputManager::SwapBodyListeners(KinectBodyListener* listener1, KinectBodyListener* listener2)
{
	//Not passing NULL
	if(listener1 && listener2)
	{
		//If kinect initialization was successful
		if(kinectConnected)
		{
			//Get the current bodies associated
			KinectBody* body1 = listener1->GetBody();
			KinectBody* body2 = listener2->GetBody();

			//If bodies exist
			if(body1 && body2)
			{
				KinectBodyEventNotifier* bodyNoti = KinectBodyEventNotifier::GetInstance();
				//Unregister both
				UnregisterBodyListener(listener1);
				UnregisterBodyListener(listener2);

				//Try to register the first listener to body 2
				bool result = bodyNoti->RegisterListener(body2, listener1);
				
				if(!result)
				{
					//if we failed, reset listening and return false
					bodyNoti->RegisterListener(body1, listener1);

					return false;
				}

				//Try to register the second listener to body1
				result = bodyNoti->RegisterListener(body1, listener2);

				if(!result)
				{
					//If failed, reset both bodies
					UnregisterBodyListener(listener1);					
					result = bodyNoti->RegisterListener(body1, listener1);
					result = bodyNoti->RegisterListener(body2, listener2);

					return false;
				}

				return true;
			}
		}
	}

	return false;
}

void InputManager::UnregisterBodyListener(KinectBodyListener* listener)
{
	KinectBodyEventNotifier::GetInstance()->
		FlagListenerForRemoval(listener->GetBody(), listener);
}

bool InputManager::RegisterAudioListener(KinectAudioListener* audioListener)
{
	return KinectAudioEventNotifier::GetInstance()->RegisterAudioListener(audioListener);
}

bool InputManager::UnregisterAudioListener(KinectAudioListener* audioListener)
{
	return KinectAudioEventNotifier::GetInstance()->UnregisterAudioListener(audioListener);
}

void InputManager::BodyCapture()
{
	cancelBodyCapture = false;
	pauseBodyCapture = false;

	bool breakNow = false;

//	while (!cancelBodyCapture)
	{
		if(!pauseBodyCapture)
			breakNow = !kinectReader->CaptureBodyReader();

		//if(breakNow)
		//	break;

		if(cancelBodyCapture)
			printf("Wanna Leave\n");
	}
}

void InputManager::AudioCapture()
{
	cancelSpeechCapture = false;
	pauseSpeechCapture = false;

	while (!cancelSpeechCapture)
	{
		if(!pauseSpeechCapture)
			kinectReader->CaptureSpeechReader();
	}
	
}

void InputManager::BeginAllCapture()
{
	//bodyCaptureThread = std::thread(&InputManager::BodyCapture, this);
	speechCaptureThread = std::thread(&InputManager::AudioCapture, this);
}

void InputManager::SetSpeechCaptureState(bool enabled)
{
	pauseSpeechCapture = enabled;
}

void InputManager::SetBodyCaptureState(bool enabled)
{
	pauseBodyCapture = enabled;
}

void InputManager::ProcessEvents()
{
	BodyCapture();
	KinectBodyEventNotifier::GetInstance()->ProcessEvents();
	KinectAudioEventNotifier::GetInstance()->ProcessEvents();
	FlushListeners();
}