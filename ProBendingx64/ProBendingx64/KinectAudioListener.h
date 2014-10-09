#pragma once
#include "KinectSpeechReader.h"

//struct AudioData;

class KinectAudioListener
{
public:
	KinectAudioListener(void){}
	virtual ~KinectAudioListener(void){}

	virtual void AudioDataReceived(AudioData* audioData){}
};

