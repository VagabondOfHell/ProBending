#include "KinectAudioEventNotifier.h"
#include <vector>
#include <concurrent_queue.h>
#include "KinectSpeechReader.h"

KinectAudioEventNotifier* KinectAudioEventNotifier::instance;
std::vector<KinectAudioListener*> listeners;

concurrency::concurrent_queue<AudioData*> speechDataToProcess;

KinectAudioEventNotifier::KinectAudioEventNotifier(void)
{
	listeners = std::vector<KinectAudioListener*>();
}

KinectAudioEventNotifier::~KinectAudioEventNotifier(void)
{
	ProcessEvents();
}

void KinectAudioEventNotifier::DestroySingleton()
{
	if(instance)
	{
		delete instance;
		instance = NULL;
	}
}

bool KinectAudioEventNotifier::RegisterAudioListener(KinectAudioListener* listener)
{
	bool found = false;

	for (int i = 0; i < listeners.size(); i++)
	{
		if(listeners[i] == listener)
		{
			found = true;
			break;
		}
	}

	if(!found)
	{
		listeners.push_back(listener);
		return true;
	}
	
	return false;
}

bool KinectAudioEventNotifier::UnregisterAudioListener(KinectAudioListener* listener)
{
	std::vector<KinectAudioListener*>::iterator result = 
		std::find(listeners.begin(), listeners.end(), listener);

	if(result !=listeners.end())
	{
		listeners.erase(result);
		return true;
	}

	return false;
}

void KinectAudioEventNotifier::ProcessEvents()
{
	bool processing = true;

	AudioData* currentData = NULL;

	while (processing)
	{
		processing = speechDataToProcess.try_pop(currentData);
		
		if(processing)
		{
			for (int j = 0; j < listeners.size(); j++)
			{
				if(listeners[j]->Enabled)
					listeners[j]->AudioDataReceived(currentData);
			}
			
			if(currentData)
			{
				delete currentData;
				currentData = NULL;
			}
		}
	}
}

void KinectAudioEventNotifier::InjectAudioFrameData(AudioData* audioData)
{
	speechDataToProcess.push(audioData);
}