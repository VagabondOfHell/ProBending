#pragma once
#include "KinectSpeechReader.h"

//struct AudioData;

class KinectAudioListener
{
	friend class KinectAudioEventNotifier;

public:
	//True to be notified of events, false to not. This is useful for 
	//temporarily disabling input notification without removing the listener.
	bool Enabled;

	KinectAudioListener(void){Enabled = true;}
	virtual ~KinectAudioListener(void){}

protected:
	virtual void AudioDataReceived(AudioData* audioData) = 0;
};

